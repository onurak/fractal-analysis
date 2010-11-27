/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "TreeFilter.h"

void MainWindow::initializeControls()
{
    logg.info("Initializing GUI...");

    // Setup menus
    #ifndef FL_ENABLE_PARSER_DEBUG
    ui->menu_Debug->setVisible(false);
    ui->action_Debug_window->setVisible(false);
    #endif
    refreshFilters();
    refreshDynamicTS();

    // Setup render
    graphicsView = new FLGraphicsView(this->centralWidget());
    ui->gridLayout_2->addWidget(graphicsView, 0, 0);
    connect(graphicsView, SIGNAL(resize(QSize)), this, SLOT(spc_on_drawingWindowSizeChanged()));
    render = new FL::QtRender();
    graphicsView->setScene(render->scene());
    this->treeNo = 0;

    // Setup widgets
    progressBar = new QProgressBar(ui->statusbar);
    progressBar->setGeometry(200, 0, 100, 14);
    progressBar->setVisible(false);

    // Setup MainWindow
    if (env["MainWindow"].valueOf("ViewAnalysisProperties", true))
        setWindowState(Qt::WindowMaximized);

    // Setup Analysis Properties window
    ui->analysisPropertiesWidget->setVisible(
            env["MainWindow"].valueOf("ViewAnalysisProperties", true));
    ui->actionShow_Analysys_properties->setChecked(
            ui->analysisPropertiesWidget->isVisible());
    readAnalysisProperties();
    connect(ui->cbTSMarker, SIGNAL(currentIndexChanged(int)),
            this, SLOT(spc_on_analysisPropertiesChanged()));
    connect(ui->cbTSParser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(spc_on_analysisPropertiesChanged()));
    connect(ui->cbShowParseTreeNodes, SIGNAL(stateChanged(int)),
            this, SLOT(spc_on_analysisPropertiesChanged()));
    connect(ui->cbShowParseTreeRoots, SIGNAL(stateChanged(int)),
            this, SLOT(spc_on_analysisPropertiesChanged()));
    connect(ui->cbShowParseTreeTimeSeries, SIGNAL(stateChanged(int)),
            this, SLOT(spc_on_analysisPropertiesChanged()));
    connect(ui->analysisPropertiesWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(spc_on_drawingWindowSizeChanged()));
    connect(&timerDynamicTS, SIGNAL(timeout()), this, SLOT(dynamicTSCollectValues()));

    // Ready
    logg.info("GUI initialized OK");
}

void MainWindow::setStatus(QString text, int timeout)
{
    ui->statusbar->showMessage(text, timeout);
}

void MainWindow::beginProgress(QString text, int min, int max,
                               WorkerThread *thread, bool async)
{
    // Show progress bar, run thread
    setStatus(text, 0);
    progressBar->setVisible(true);
    progressBar->setMinimum(min);
    progressBar->setMaximum(max);
    progressBar->setValue(min);
    currentWorker = thread;
    if (thread)
    {
        connect(thread, SIGNAL(started()),
                this,   SLOT(workerThreadStarted()));
        connect(thread, SIGNAL(progressChanged(int, QString)),
                this,   SLOT(workerThreadProgress(int, QString)));
        connect(thread, SIGNAL(finished(WorkerThread*)),
                this,   SLOT(workerThreadFinished(WorkerThread*)));
        connect(thread, SIGNAL(debugEvent(FL::TSParser::DebugEvent)),
                this,   SLOT(workerThreadDebugParser(FL::TSParser::DebugEvent)));
        //if (async)
          //  thread->start();
        //else
            thread->run();
    }
}

void MainWindow::updateProgress(int value, QString text)
{
    setStatus(text, 0);
    progressBar->setValue(value);
}

void MainWindow::updateProgress(int step)
{
    progressBar->setValue(progressBar->value() + step);
}

void MainWindow::endProgress()
{
    progressBar->setVisible(false);
}

void MainWindow::workerThreadStarted()
{
    this->state = STATE_ANALYSIS;
    updateInfo();
}

void MainWindow::workerThreadProgress(int progress, QString msg)
{
    updateProgress(progress, msg);
    updateInfo();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents, 20);
}

void MainWindow::workerThreadDebugParser(FL::TSParser::DebugEvent event)
{
    render->drawParseContext(*event.context);
}

void MainWindow::workerThreadFinished(WorkerThread *thread)
{
    this->state = STATE_READY;
    updateInfo();
    endProgress();
    draw();
    switch (thread->result())
    {
        case R_OK: break;
        case R_ERROR:
            showError("Error occures: " + thread->resultDescription());
            break;
        case R_LAST_LEVEL:
            showInfo("Last level parsed!");
            setStatus("Last level parsed!");
            break;
    }
    //m_valuesAdded -= watcher.marker();
    m_valuesAdded = 0;
}

void MainWindow::showError(QString msg)
{
    if (this->state != STATE_INIT)
        QMessageBox::critical(this, tr("Error"), msg);
    setStatus(msg, 5000);
}

void MainWindow::showInfo(QString msg)
{
    if (this->state != STATE_INIT)
        QMessageBox::information(this, tr("Info"), msg);
    setStatus(msg, 5000);
}

void MainWindow::draw()
{    
    render->clearTrees();
    if (watcher.parseTreeSet().size() > 0)
    {
        FL::ParseTree *tree = (FL::ParseTree*) watcher.parseTreeSet()[treeNo];
        render->addTree(tree, 0, treeDrawingSettings());
    }

    render->clearForecasts();
    //render->addForecast(new FL::Forecast(NULL, NULL, 5, 1, 3, 15, 1400, 1453));

    render->draw();    
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    int width  = graphicsView->size().width() - 5;
    int height = graphicsView->size().height() - 5;
    render->resize(width, height);
    draw();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (this->state == STATE_ANALYSIS)
    {
        int ret = QMessageBox::warning(this, tr("Warning!"), tr("Analysis in progress. Would you like to halt it?"),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            haltAnalysis();
            event->accept();
        } else
            event->ignore();
    }
    else
        event->accept();
 }

void MainWindow::updateInfo()
{
    if (watcher.timeSeries() != NULL)
        ui->lbTimeSeriesSize->setText(QString().setNum(watcher.timeSeries()->dimSize(0)));
    else
        ui->lbTimeSeriesSize->setText(tr("not loaded"));

    if (watcher.patterns().size() > 0)
        ui->lbPatternsCount->setText(QString().setNum(watcher.patterns().size()));
    else
        ui->lbPatternsCount->setText(tr("not loaded"));

    ui->lbTreesCount->setText(QString().setNum(watcher.parseTreeSet().size()));
    ui->sbTreeNo->setMinimum(watcher.parseTreeSet().size() > 0 ? 1 : 0);
    ui->sbTreeNo->setMaximum(watcher.parseTreeSet().size());
    ui->sbTreeNo->setEnabled(watcher.parseTreeSet().size() > 0 && this->state == STATE_READY);
    ui->bnClearParseTrees->setEnabled(ui->sbTreeNo->isEnabled());
    ui->lbMemory->setText(QString("%1 KB").arg(watcher.parseTreeSet().bytesUsed() / 1024));
    ui->bnHaltAnalysis->setEnabled(this->state != STATE_READY);
    ui->analysisPropertiesWidget->setEnabled(this->state == STATE_READY);
    ui->toolBar->setEnabled(this->state == STATE_READY);
    ui->menubar->setEnabled(this->state == STATE_READY);

    switch (this->state)
    {
        case STATE_READY: ui->lbState->setText(tr("Ready")); break;
        case STATE_ANALYSIS: ui->lbState->setText(tr("Analysing...")); break;
        case STATE_DEBUG: ui->lbState->setText(tr("Debugging")); break;
        case STATE_INIT: break;
    }
}

void MainWindow::timeSeriesChanged()
{
    if (render->timeSeries() != watcher.timeSeries())
        render->setTimeSeries(watcher.timeSeries());
    ui->timeSeriesHorizontalScroller->setMinimum(render->minimumHorizontalScroll());
    ui->timeSeriesHorizontalScroller->setMaximum(render->maximumHorizontalScroll());
    ui->timeSeriesHorizontalScroller->setValue(render->minimumHorizontalScroll());
    ui->timeSeriesHorizontalScroller->setEnabled(render->isNeedHorizontalScroll());
}

FL::QtRender::TreeDrawingSettings MainWindow::treeDrawingSettings()
{
    FL::QtRender::TreeDrawingSettings result = FL::QtRender::DRAW_NONE;
    if (env["Rendering"].valueOf("ShowTreeNodes", true))
        result = FL::QtRender::TreeDrawingSettings( result ^ FL::QtRender::DRAW_NODES );
    if (env["Rendering"].valueOf("ShowTreeTimeSeries", true))
        result = FL::QtRender::TreeDrawingSettings( result ^ FL::QtRender::DRAW_TIMESERIES );
    if (env["Rendering"].valueOf("ShowTreeRoots", true))
        result = FL::QtRender::TreeDrawingSettings( result ^ FL::QtRender::DRAW_ROOTS );
    return result;
}

QString MainWindow::defaultTSPath()
{
    QFileInfo fi(QSTR(env["TimeSeries"].valueOf("FileName", ".")));
    return fi.absolutePath();
}

QString MainWindow::defaultPatternsPath()
{
    QFileInfo fi(QSTR(env["Patterns"].valueOf("FileName", ".")));
    return fi.absolutePath();
}

void MainWindow::readAnalysisProperties()
{
    std::vector<std::string>::const_iterator i;
    int idx;

    // Refresh marker list
    ui->cbTSMarker->clear();
    for (i = watcher.markerList().begin(); i != watcher.markerList().end(); ++i)
        ui->cbTSMarker->addItem(QSTR(*i));
    for (idx = 0, i = watcher.markerList().begin(); i != watcher.markerList().end(); ++i, ++idx)
        if (*i == watcher.option("marker"))
            break;

    ui->cbTSMarker->setCurrentIndex(idx);

    // Refresh parser list
    ui->cbTSParser->clear();
    for (i = watcher.parserList().begin(); i != watcher.parserList().end(); ++i)
        ui->cbTSParser->addItem(QSTR(*i));
    for (idx=0, i = watcher.parserList().begin(); i != watcher.parserList().end(); ++i, ++idx)
        if (*i == watcher.option("parser"))
            break;
    ui->cbTSParser->setCurrentIndex(idx);

    // Refresh drawing settings
    ui->cbShowParseTreeNodes->setChecked( env["Rendering"].valueOf("ShowTreeNodes", true) );
    ui->cbShowParseTreeTimeSeries->setChecked( env["Rendering"].valueOf("ShowTreeTimeSeries", true) );
    ui->cbShowParseTreeRoots->setChecked( env["Rendering"].valueOf("ShowTreeRoots", true) );
}

void MainWindow::writeAnalysisProperties()
{
    watcher.setOption("marker", STR(ui->cbTSMarker->currentText()));
    watcher.setOption("parser", STR(ui->cbTSParser->currentText()));
    env["Rendering"]["ShowTreeNodes"] = ui->cbShowParseTreeNodes->isChecked();
    env["Rendering"]["ShowTreeTimeSeries"] = ui->cbShowParseTreeTimeSeries->isChecked();
    env["Rendering"]["ShowTreeRoots"] = ui->cbShowParseTreeRoots->isChecked();
}

void MainWindow::spc_on_analysisPropertiesChanged()
{
    writeAnalysisProperties();
    render->setTreeDrawingSettings(treeDrawingSettings());
    draw();
}

void MainWindow::spc_on_drawingWindowSizeChanged()
{
    int width  = graphicsView->size().width() - 5;
    int height = graphicsView->size().height() - 5;
    render->resize(width, height);
    draw();
}

void MainWindow::refreshFilters()
{
    QString filtersDir = QSTR(env["Filters"].valueOf("Path", "filters/"));
    QStringList filesOfInterest;
    filesOfInterest.append("*.pat");

    QDir dir(filtersDir);
    QStringList entries =
            dir.entryList(filesOfInterest, QDir::Files, QDir::Name);

    ui->menuFilters->actions().clear();
    connect(ui->menuFilters, SIGNAL(triggered(QAction*)),
            this, SLOT(spc_action_ExecuteFilter(QAction*)));
    foreach (QString fileName, entries)
    {
        QAction *action = ui->menuFilters->addAction(QFileInfo(fileName).baseName());
        action->setData(filtersDir + fileName);
    }
    ui->menuFilters->setVisible(  ui->menuFilters->actions().size() != 0  );
}

void MainWindow::spc_action_ExecuteFilter(QAction *action)
{
    if (watcher.timeSeries() == NULL)
    {
        showError("No time series loaded");
        return;
    }

    // markup trees (if it wasn't)
    if (watcher.parseTreeSet().size() == 0)
    {
        if (!watcher.markup())
        {
            showError("Can't create markup for time series");
            return;
        }
    }

    // apply filter
    QString filterName = action->data().toString();

    if (!QFile::exists(filterName))
    {
        showError("Filter file " + filterName + " not exists");
        return;
    }
    TreeFilter filter;
    try {
        filter.createFromFile(STR(filterName));
        filter.apply(watcher.parseTreeSet(), watcher.timeSeries());

    } catch (ETreeFilter &e) {
        showError(QSTR(e.msg));
    }

    // create markup from roots for each tree
    for (int i = 0; i < watcher.parseTreeSet().size(); ++i)
    {
        FL::ParseTree *tree = watcher.parseTreeSet()[i];
        watcher.parseTreeSet().trees[i] = createRootsMarkupFromTree(tree);
        delete tree;
    }
    draw();
}

void MainWindow::refreshDynamicTS()
{
    if (m_tsDataSource == NULL)
    {
        ui->actionDynamic_Time_Series_Run->setEnabled(false);
        ui->actionDynamic_Time_Series_Step->setEnabled(false);
        ui->actionDynamic_Time_Series_Stop->setEnabled(false);
    }
    else
    {
        //bool enable = timerDynamicTS.isActive();
        //ui->actionDynamic_Time_Series_Run->setEnabled(!enable);
        //ui->actionDynamic_Time_Series_Step->setEnabled(enable);
        //ui->actionDynamic_Time_Series_Stop->setEnabled(enable);
        ui->actionDynamic_Time_Series_Step->setEnabled(true);
    }
}

void MainWindow::dynamicTSCollectValues()
{
    m_valuesAdded += 1;
    setStatus(QString("Added: %1").arg(m_valuesAdded), 10000);
    draw();
}
