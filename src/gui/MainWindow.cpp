#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_settings(QSettings::IniFormat, QSettings::UserScope, "FL", "FLAnalyzer")
{
    m_isInitializing = true;
    m_wantInterrupt = false;
    ui->setupUi(this);

    ui->statusBar->addPermanentWidget(m_lbCurrentTime = new QLabel());
    ui->statusBar->addPermanentWidget(m_lbCurrentValue = new QLabel());
    ui->statusBar->addPermanentWidget(m_lbCurrentNode = new QLabel());

    m_matcher = NULL;
    m_preprocessingMatcher = NULL;

    m_render = new QParseTreeRender();
    ui->graphicsView->setScene(m_render->scene());
    ui->graphicsView->scale(1, -1);
    m_render->setTimeSeries(&m_timeSeries);
    m_render->setForest(&m_forest);
    m_render->setView(ui->graphicsView);

    connect(m_render->scene(), SIGNAL(onMouseEvent(QGraphicsSceneMouseEvent*)),
            this,              SLOT(onSceneMouseEvent(QGraphicsSceneMouseEvent*)));
    connect(m_render->scene(), SIGNAL(onWheelEvent(QGraphicsSceneWheelEvent*)),
            this,              SLOT(onSceneWheelEvent(QGraphicsSceneWheelEvent*)));
    connect(ui->bnOpenPatterns, SIGNAL(clicked()),
            ui->actionOpen_patterns, SLOT(trigger()));
    connect(ui->bnOpenPreprocessingPatterns, SIGNAL(clicked()),
            ui->actionOpen_preprocessing_patterns, SLOT(trigger()));

    readSettings();
    initMetrics();
    updateForestInfo();

    ui->statusBar->showMessage(tr("Ready"), 5000);
    m_isInitializing = false;
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
    delete m_render;
    delete m_matcher;
    delete m_preprocessingMatcher;
    m_patterns.cleanup();
    m_preprocessingPatterns.cleanup();
}

void MainWindow::onSceneMouseEvent(QGraphicsSceneMouseEvent *event)
{
    double x = event->lastScenePos().x();
    double y = event->lastScenePos().y();
    m_render->setMousePos(x, y);

    bool ok_t, ok_v;
    double time  = m_render->currentTime(&ok_t);
    double value = m_render->currentValue(&ok_v);

    if (ok_t)
        m_lbCurrentTime->setText(QString("Time: %1").arg(time));
    else
        m_lbCurrentTime->setText("");

    if (ok_v)
        m_lbCurrentValue->setText(QString("Value: %1").arg(value));
    else
        m_lbCurrentValue->setText("");

    const FL::Trees::Node *node = m_render->currentNode();
    if (node != NULL)
    {
        QString nodeStatus;
        switch (node->status())
        {
            case FL::nsFinished:   nodeStatus = "finished"; break;
            case FL::nsFloating:   nodeStatus = "floating"; break;
            case FL::nsUnfinished: nodeStatus = "not finished"; break;
        }

        QString msg =
            QString("Current node: %1, %2, at %3 - %4")
                .arg(QString().fromStdString(FL::IDGenerator::nameOf(node->id())))
                .arg(nodeStatus)
                .arg(m_timeSeries.time(node->begin()), 0, 'f', 2)
                .arg(m_timeSeries.time(node->end()), 0, 'f', 2);

        m_lbCurrentNode->setText(msg);
    }
    else
    {
        m_lbCurrentNode->setText("");
    }
}

void MainWindow::onSceneWheelEvent(QGraphicsSceneWheelEvent *event)
{
    double s = event->delta() < 0 ? 0.9 : 1.0/0.9;
    ui->graphicsView->scale(s, s);
}

void MainWindow::on_actionOpen_time_series_triggered()
{
    QFileDialog openDialog(
            this, tr("Open time series"),
            m_settings.value("gui/ts/Path", ".").toString(),
            "Comma-separated values (*.csv);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    m_settings.setValue("gui/ts/Path", openDialog.directory().absolutePath());
    m_settings.setValue("gui/ts/File", openDialog.selectedFiles()[0]);

    if (loadTimeSeries(openDialog.selectedFiles()[0]))
    {
        m_settings.setValue("gui/ts/ValuesColumn",
                            QString().fromStdString(m_timeSeries.header()[0]));

        if (!m_timeSeries.header()[1].empty())
            m_settings.setValue("gui/ts/TimeColumn",
                                QString().fromStdString(m_timeSeries.header()[1]));
        else
            m_settings.setValue("gui/ts/TimeColumn", tr("[Use default time]"));

        m_settings.setValue("gui/ts/IsDynamic", false);
    }
}

void MainWindow::on_actionOpen_patterns_triggered()
{
    QFileDialog openDialog(
            this, tr("Open patterns"),
            m_settings.value("gui/patterns/Path", ".").toString(),
            "PAT-files (*.pat);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    m_settings.setValue("gui/patterns/Path", openDialog.directory().absolutePath());
    m_settings.setValue("gui/patterns/File", openDialog.selectedFiles()[0]);

    loadPatterns(openDialog.selectedFiles()[0],
                 m_patterns,
                 &m_matcher,
                 ui->lbPatternsFile);
    if (m_matcher != NULL)
        m_matcher->setAllowUnfinished(true);

    if (m_patterns.haveLeftRecursion())
    {
        QMessageBox::warning(this, "Warning",
            "Patterns set contains left recursion.\n\n"
            "It is recomended to enable \"Max levels count\" metric,\n"
            "otherwise analysis process may become infinite.");
    }
}

void MainWindow::on_actionOpen_preprocessing_patterns_triggered()
{
    QFileDialog openDialog(
            this, tr("Open preprocessing patterns"),
            m_settings.value("gui/patterns/Path", ".").toString(),
            "PAT-files (*.pat);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    m_settings.setValue("gui/patterns/Path", openDialog.directory().absolutePath());
    m_settings.setValue("gui/patterns/PreprocessingFile", openDialog.selectedFiles()[0]);

    loadPatterns(openDialog.selectedFiles()[0],
                 m_preprocessingPatterns,
                 &m_preprocessingMatcher,
                 ui->lbPreprocessingPatternsFile);
    if (m_preprocessingMatcher)
        m_preprocessingMatcher->setAllowUnfinished(false);
}

void MainWindow::loadPatterns(
    const QString &fileName,
    FL::Patterns::PatternsSet &patterns,
    FL::Patterns::Matcher **matcher,
    QLabel *label)
{
    if (label != NULL)
        label->setText("");

    if (fileName.isEmpty())
        return;

    if (matcher)
    {
        delete *matcher;
        *matcher = NULL;
    }

    try
    {
        patterns.cleanup();
        FL::IO::FilePAT file;
        if (!file.open(fileName.toStdString(), patterns))
        {
            throw QString("Error opening %1.\n%2").
                    arg(fileName).
                    arg(FL::errorDescription(file.lastError()).c_str());
        }

        if (matcher)
            *matcher = new FL::Patterns::Standard::TreeMatcher(patterns);
        if (label != NULL)
            label->setText(QFileInfo(fileName).fileName());
    }
    catch (const QString &s)
    {
        showError(s);
    }
}

bool MainWindow::loadTimeSeries(
    const QString &fileName,
    TSLoadMethod loadMethod,
    QString valuesColumnName,
    QString timeColumnName)
{
    ui->lbTimeSeriesFile->setText("");
    ui->lbTimeSeriesSize->setText("");

    if (fileName.isEmpty())
        return false;
    try
    {
        FL::IO::FileCSV file;
        if (!file.open(fileName.toStdString()))
            throw QString(tr("Error opening %1")).arg(fileName);

        int valuesColumnIndex = 0;
        int timeColumnIndex = 0;

        if (file.header().size() == 0)
            throw QString(tr("Error opening %1")).arg(fileName);

        if (file.header().size() > 1)
        {
            bool ok;

            // If column name specified in parameter - try to find it
            FL::TimeSeries::Header::const_iterator headerItem;

            if (!valuesColumnName.isEmpty() && !timeColumnName.isEmpty())
            {
                valuesColumnIndex = file.columnIndex(valuesColumnName.toStdString());
                if (timeColumnName != tr("[Use default time]"))
                    timeColumnIndex = file.columnIndex(timeColumnName.toStdString()) + 1;
                else
                    timeColumnIndex = 0;
                ok  = valuesColumnIndex >= 0 && timeColumnIndex >= 0;
            }
            else
                ok = false;

            // If column name not specified or not found - ask user
            if (!ok)
            {
                QStringList columns;
                forall(headerItem, file.header())
                    columns << QStr(*headerItem);

                valuesColumnName =
                   QInputDialog::getItem(
                        this, tr("Open time series"),
                        tr("Which column contains VALUES?"),
                        columns, 0, false, &ok);

                if (!ok)
                    return false;
                valuesColumnIndex = file.columnIndex(valuesColumnName.toStdString());
                if (valuesColumnIndex < 0)
                    return false;

                columns.push_front(tr("[Use default time]"));
                timeColumnName =
                   QInputDialog::getItem(
                        this, tr("Open time series"),
                        tr("Which column contains TIME?"),
                        columns, 0, false, &ok);

                if (!ok)
                    return false;
                if (timeColumnName != tr("[Use default time]"))
                {
                    timeColumnIndex = file.columnIndex(timeColumnName.toStdString()) + 1;
                    if (timeColumnIndex < 0)
                        return false;
                }
                else
                {
                    timeColumnIndex = 0;
                }
            }
        }
        m_timeSeries.clear();
        m_forest.cleanup();
        m_timeSeriesCache.clear();

        // Get actual column index
        if (timeColumnIndex > 0)
            timeColumnIndex -= 1;
        else
            timeColumnIndex = 0;

        bool readOk;

        readOk = file.read(m_timeSeries, valuesColumnIndex, timeColumnIndex);
        if (timeColumnIndex <= 0 && readOk)
            m_timeSeries.setLinearTime();

        if (!readOk)
            throw QString(tr("Error opening %1")).arg(fileName);

        // If loading method is dynamic then ask user how many values
        // must be in static part, and how many in dynamic
        if (loadMethod & tloDynamic)
        {
            int staticPartSize;

            if (!m_isInitializing)
            {
                bool ok = false;
                staticPartSize =
                   QInputDialog::getInt(
                        this, tr("Open time series"),
                        tr("How many values must be in static part?"),
                        m_timeSeries.size()/2,
                        0, m_timeSeries.size(), 1,
                        &ok);
                if (!ok)
                {
                    m_timeSeries.clear();
                    updateForestInfo();
                    return false;
                }

                m_settings.setValue("gui/ts/StaticPartSize", staticPartSize);
            }
            else
            {
                staticPartSize = m_settings.value("gui/ts/StaticPartSize", m_timeSeries.size()/2).toInt();
            }

            for (int i = m_timeSeries.size()-1; i >= staticPartSize; --i)
                m_timeSeriesCache.append(
                            qMakePair(m_timeSeries.time(i), m_timeSeries.value(i)));
            m_timeSeries.remove(staticPartSize, m_timeSeries.size() - staticPartSize);
        }

        ui->lbTimeSeriesFile->setText(QFileInfo(fileName).fileName());
        ui->lbTimeSeriesSize->setNum(m_timeSeries.size());
    }
    catch (const QString &err)
    {
        showError(err);
    }

    updateForestInfo();
    return true;
}

void MainWindow::showError(const QString &text)
{
    if (!m_isInitializing)
        QMessageBox::critical(this, tr("Error"), text);
    ui->statusBar->showMessage(text, 3000);
}

void MainWindow::showError(const std::string &text)
{
    showError(QStr(text));
}

void MainWindow::readSettings()
{
    // This code lead to unexpected behaviour in Qt 4.7.x and MacOS Lion
    /*
    if (m_settings.value("gui/mainform/maximized", true).toBool())
        setWindowState(windowState() ^ Qt::WindowMaximized);
    else
        setWindowState( QFlags<Qt::WindowState>(windowState() ^ ~Qt::WindowMaximized) );
    */

    int x = m_settings.value("gui/mainform/x", 100).toInt();
    if (x >= QApplication::desktop()->geometry().width())
        x = 100;

    int y = m_settings.value("gui/mainform/y", 100).toInt();
    if (y >= QApplication::desktop()->geometry().height())
        y = 100;

    int width = m_settings.value("gui/mainform/width", 800).toInt();
    if (x + width >= QApplication::desktop()->geometry().width())
        width = QApplication::desktop()->geometry().width() - x;

    int height = m_settings.value("gui/mainform/height", 600).toInt();
    if (y + height >= QApplication::desktop()->geometry().height())
        height = QApplication::desktop()->geometry().height() - y;

    setGeometry(x, y, width, height);

    loadTimeSeries(
                m_settings.value("gui/ts/File", "").toString(),
                m_settings.value("gui/ts/IsDynamic", false).toBool() ? tloDynamic : tloStatic,
                m_settings.value("gui/ts/ValuesColumn", "").toString(),
                m_settings.value("gui/ts/TimeColumn", "").toString());

    loadPatterns(
                m_settings.value("gui/patterns/File", "").toString(),
                m_patterns,
                &m_matcher,
                ui->lbPatternsFile);
    if (m_matcher != NULL)
        m_matcher->setAllowUnfinished(true);

    loadPatterns(
                m_settings.value("gui/patterns/PreprocessingFile", "").toString(),
                m_preprocessingPatterns,
                &m_preprocessingMatcher,
                ui->lbPreprocessingPatternsFile);
    if (m_preprocessingMatcher != NULL)
        m_preprocessingMatcher->setAllowUnfinished(false);


    m_markerName = ui->cbMarker->currentText();

    QString staticParser = m_settings.value("gui/StaticParser", "MultiPass").toString();
    int staticParserIndex = ui->cbStaticParser->findText(staticParser);
    ui->cbStaticParser->setCurrentIndex(staticParserIndex >= 0 ? staticParserIndex : 0);
    m_staticParserName = ui->cbStaticParser->currentText();


    ui->sbParseTreeIndex->setEnabled(false);
    ui->sbParseTreeIndex->setMinimum(1);
    ui->sbParseTreeIndex->setMaximum(1);
    ui->sbParseTreeIndex->setValue(1);
    ui->lbParseTreeCount->setNum(0);

    ui->bnHalt->setEnabled(false);
}

void MainWindow::writeSettings()
{
    if (windowState() & Qt::WindowMaximized)
        m_settings.setValue("gui/mainform/maximized", true);
    else
    {
        m_settings.setValue("gui/mainform/x", geometry().x());
        m_settings.setValue("gui/mainform/y", geometry().y());
        m_settings.setValue("gui/mainform/width", geometry().width());
        m_settings.setValue("gui/mainform/height", geometry().height());
        m_settings.setValue("gui/mainform/maximized", false);
    }

    m_settings.setValue("gui/staticParser", ui->cbStaticParser->currentText());
}

void MainWindow::on_actionZoomIn_triggered()
{
    ui->graphicsView->scale(1.0/0.9, 1.0/0.9);
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->graphicsView->scale(0.9, 0.9);
}

void MainWindow::on_actionMarkup_triggered()
{
    markup();
}

void MainWindow::markup()
{
    m_forest.cleanup();

    FL::Markers::AbstractMarker *marker = new FL::Markers::AB();
    marker->analyze(m_timeSeries, m_forest, *m_matcher, m_patterns);
    if (marker->wasOk())
    {
        if (m_preprocessingPatterns.size() > 0)
        {
            FL::Parsers::AbstractParser *parser = new FL::Parsers::SinglePass();
            parser->analyze(
                        m_timeSeries, m_forest, m_preprocessingPatterns,
                        *m_preprocessingMatcher, m_metrics);
            if (parser->wasOk())
            {
                FL::Trees::Forest::Iterator tree;
                forall(tree, m_forest)
                    (*tree)->markupWithRoots();
            }
            else
            {
                showError(parser->lastError().arg());
            }

            delete parser;
        }
    }
    else
    {
        showError(marker->lastError().arg());
    }
    delete marker;


    updateForestInfo();
}

void MainWindow::buildTrees()
{
    readMetrics();

    FL::Parsers::AbstractParser *parser = NULL;
    if (m_staticParserName == "SinglePass")
        parser = new FL::Parsers::SinglePass();
    else
        parser = new FL::Parsers::MultiPass();
    parser->onProgress = delegate(this, &MainWindow::onParsingProgress);

    onParsingBegin();

    parser->analyze(
          m_timeSeries, m_forest, m_patterns, *m_matcher, m_metrics);

    onParsingFinished(parser);

    //addBackgroundTask(new BackgroundParserTask(*this, parser));
}

void MainWindow::on_actionFitAll_triggered()
{
    ui->graphicsView->fitInView(0.0, -1.0, 1.0, 2.0);
}

void MainWindow::on_actionMarkup_with_roots_triggered()
{
    FL::Trees::Forest::Iterator tree;
    forall(tree, m_forest)
        (*tree)->markupWithRoots();
    updateForestInfo();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    on_actionFitAll_triggered();
    QMainWindow::resizeEvent(event);
}


void MainWindow::on_bnClearPatterns_clicked()
{
    m_patterns.cleanup();
    delete m_matcher;
    m_matcher = NULL;
    ui->lbPatternsFile->setText("");
}

void MainWindow::on_actionBuild_trees_triggered()
{
    buildTrees();
}


void MainWindow::on_bnClearPreprocessingPatterns_clicked()
{
    m_preprocessingPatterns.cleanup();
    delete m_preprocessingMatcher;
    m_preprocessingMatcher = NULL;
    ui->lbPreprocessingPatternsFile->setText("");
}

void MainWindow::on_cbStaticParser_currentIndexChanged(const QString &arg)
{
    m_staticParserName = arg;
}

void MainWindow::on_cbMarker_currentIndexChanged(const QString &arg)
{
    m_markerName = arg;
}

void MainWindow::setUiEnables(bool enabled)
{
    /*
    QList<QWidget*> uicontrols = findChildren<QWidget*>();
    QList<QWidget*>::iterator w;
    forall(w, uicontrols)
        (*w)->setEnabled(enabled);
    */

    ui->bnHalt->setEnabled(!enabled);
    ui->bnClearForest->setEnabled(enabled);
    ui->bnClearPatterns->setEnabled(enabled);
    ui->bnClearPreprocessingPatterns->setEnabled(enabled);
    ui->bnDeleteCurrentTree->setEnabled(enabled);
    ui->bnOpenPatterns->setEnabled(enabled);
    ui->bnOpenPreprocessingPatterns->setEnabled(enabled);
    ui->bnRefreshPatterns->setEnabled(enabled);
    ui->bnRefreshPreprocessingPatterns->setEnabled(enabled);
    ui->cbStaticParser->setEnabled(enabled);
    ui->cbDynamicParser->setEnabled(enabled);
    ui->cbMarker->setEnabled(enabled);
    ui->sbParseTreeIndex->setEnabled(enabled);

    QList<QAction*> uicontrols = findChildren<QAction*>();
    QList<QAction*>::iterator w;
    forall(w, uicontrols)
        (*w)->setEnabled(enabled);
}

void MainWindow::onParsingBegin()
{
    m_wantInterrupt = false;
    setUiEnables(false);
    ui->lbState->setText("Analyzing...");
    m_lastUpdateTime = QTime::currentTime();
}

bool MainWindow::onParsingProgress(FL::Trees::Forest *forest)
{
    const int MAX_CALC_TIME_SECS = 1;

    if (forest->size() % 10 == 0 ||
        m_lastUpdateTime.secsTo(QTime::currentTime()) > MAX_CALC_TIME_SECS)
    {
        ui->lbParseTreeCount->setNum((int)forest->size());
        qApp->processEvents();
        m_lastUpdateTime = QTime::currentTime();
    }
    return m_wantInterrupt;
}

void MainWindow::onParsingFinished(FL::Parsers::AbstractParser *parser)
{
    setUiEnables(true);

    if (!parser->wasOk())
        showError(parser->lastError().arg());
    delete parser;

    updateForestInfo();
    m_wantInterrupt = false;
    ui->lbState->setText("Ready");
}

void MainWindow::addBackgroundTask(QThread *task)
{
    m_backgroundTasks.append(task);
    connect(task, SIGNAL(finished()), this, SLOT(onBackgroundTaskFinished()));
    task->start();
}

void MainWindow::onBackgroundTaskFinished()
{
    for (int i = 0; i < m_backgroundTasks.size(); )
    {
        QThread *task = m_backgroundTasks[i];
        if (task->isFinished())
        {
            delete task;
            m_backgroundTasks.remove(i);
        }
        else
            ++i;
    }
}

void MainWindow::on_sbParseTreeIndex_valueChanged(int value)
{
    if (m_forest.size() == 0)
        return;
    if (value <= 0)
        value = 1;
    if (value > (int)m_forest.size())
        value = (int)m_forest.size();
    value -= 1;

    ui->lbLevelsNo->setNum(m_forest[value]->levelCount());
    m_render->setCurrentTree(value);
}

void MainWindow::on_bnClearForest_clicked()
{
    m_forest.cleanup();
    updateForestInfo();
}

void MainWindow::updateForestInfo()
{
    if (m_forest.size() > 0)
    {
        ui->sbParseTreeIndex->setMaximum(m_forest.size());
        ui->sbParseTreeIndex->setValue(1);
        ui->sbParseTreeIndex->setEnabled(true);
        ui->lbLevelsNo->setNum(m_forest[0]->levelCount());
    }
    else
    {
        ui->sbParseTreeIndex->setMaximum(1);
        ui->sbParseTreeIndex->setValue(1);
        ui->sbParseTreeIndex->setEnabled(false);
        ui->lbLevelsNo->setNum(0);
    }

    ui->lbParseTreeCount->setNum((int)m_forest.size());
    ui->lbTimeSeriesSize->setNum((int)m_timeSeries.size());

    m_render->setCurrentTree(ui->sbParseTreeIndex->value()-1);
}

void MainWindow::initMetrics()
{
    ui->tableMetrics->clear();

    QHeaderView *header = new QHeaderView(Qt::Horizontal);
    header->setResizeMode(QHeaderView::ResizeToContents);
    header->setStretchLastSection(true);
    ui->tableMetrics->setHorizontalHeader(header);

    ui->tableMetrics->setColumnCount(3);
    ui->tableMetrics->setRowCount(m_metrics.size());

    for (size_t i = 0; i < m_metrics.size(); ++i)
    {
        FL::Trees::Metric *metric = m_metrics[i];

        QTableWidgetItem *itemName = new QTableWidgetItem(QString().fromStdString(metric->name()));
        itemName->setFlags(Qt::ItemIsEnabled);
        ui->tableMetrics->setItem(i, 0, itemName);

        //QTableWidgetItem *itemEnabled = new QTableWidgetItem();
        //itemEnabled->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        //ui->tableMetrics->setItem(i, 1, itemEnabled);
        QCheckBox *itemEnabled = new QCheckBox();
        itemEnabled->setChecked(true);
        ui->tableMetrics->setCellWidget(i, 1, itemEnabled);


        QTableWidgetItem *itemLimit = new QTableWidgetItem((char*) metric->value());
        itemLimit->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->tableMetrics->setItem(i, 2, itemLimit);
    }
}


void MainWindow::on_tableMetrics_cellChanged(int row, int column)
{
    if (row < 0 || row >= (int) m_metrics.size())
        return;

    FL::Trees::Metric *metric = m_metrics[row];

    switch (column)
    {
        case 2:
        {
            QTableWidgetItem *item = ui->tableMetrics->item(row, column);
            metric->setValue(item->text().toStdString());
            item->setText((char*) metric->value());
            break;
        }
    }
}

void MainWindow::readMetrics()
{
    for (size_t i = 0; i < m_metrics.size(); ++i)
    {
        QCheckBox *cb = (QCheckBox*) ui->tableMetrics->cellWidget(i, 1);
        m_metrics[i]->setEnabled(cb->isChecked());
    }
}

void MainWindow::onNewTimeSeriesData(const QList< QPair<double, double> > &data)
{
    addDataMutex.lock();
    m_timeSeriesCache.append(data);
    addDataMutex.unlock();
}

void MainWindow::on_actionOpen_dynamic_time_series_triggered()
{
    QFileDialog openDialog(
            this, tr("Open dynamic time series"),
            m_settings.value("gui/ts/Path", ".").toString(),
            "Comma-separated values (*.csv);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    QString fileName = openDialog.selectedFiles()[0];

    m_settings.setValue("gui/ts/Path", openDialog.directory().absolutePath());
    m_settings.setValue("gui/ts/File", fileName);

    if (loadTimeSeries(fileName, tloDynamic))
    {
        m_settings.setValue("gui/ts/ValuesColumn",
                            QString().fromStdString(m_timeSeries.header()[0]));

        if (!m_timeSeries.header()[1].empty())
            m_settings.setValue("gui/ts/TimeColumn",
                                QString().fromStdString(m_timeSeries.header()[1]));
        else
            m_settings.setValue("gui/ts/TimeColumn", tr("[Use default time]"));

        m_settings.setValue("gui/ts/IsDynamic", true);
    }
}

void MainWindow::dynamicStep()
{
    if (m_timeSeriesCache.size() == 0)
        return;

    const QPair<double, double> p = m_timeSeriesCache.back();
    m_timeSeries.append(p.first, p.second);
    m_timeSeriesCache.pop_back();

    FL::ParseResult pr;

    FL::Markers::AbstractMarker *marker = NULL;
    FL::Parsers::AbstractDynamicParser *parser = NULL;

    try
    {
        marker = new FL::Markers::ABIncremental();
        pr = marker->analyze(m_timeSeries, m_forest, *m_preprocessingMatcher, m_preprocessingPatterns);
        if (!marker->wasOk())
            throw "Can not markup";


        parser = new FL::Parsers::DynamicMultiPass();
        //pr = parser->analyze(
        //            m_timeSeries, m_forest, m_patterns, m_metrics, *m_matcher, m_forecast);
        //if (!parser->wasOk())
          //  throw "Can not parse";
    }
    catch (const char *s)
    {
        showError(QString(s));
    }


    delete marker;
    delete parser;

    updateForestInfo();
    on_actionFitAll_triggered();
}

void MainWindow::on_action_Dynamic_step_triggered()
{
    dynamicStep();
}

void MainWindow::on_bnDeleteCurrentTree_clicked()
{
    int treeIndex = ui->sbParseTreeIndex->value() - 1;

    if (m_forest.size() == 0 || treeIndex < 0 || treeIndex >= int(m_forest.size()))
        return;

    delete m_forest[treeIndex];
    m_forest.erase(m_forest.begin() + treeIndex);
    updateForestInfo();

    if (m_forest.size() > 0)
        ui->sbParseTreeIndex->setValue(std::min(treeIndex+1, int(m_forest.size())));
}

void MainWindow::on_bnRefreshPreprocessingPatterns_clicked()
{
    loadPatterns(
                m_settings.value("gui/patterns/PreprocessingFile", "").toString(),
                m_preprocessingPatterns,
                &m_preprocessingMatcher,
                ui->lbPreprocessingPatternsFile);
    m_preprocessingMatcher->setAllowUnfinished(false);
}

void MainWindow::on_bnRefreshPatterns_clicked()
{
    loadPatterns(
                m_settings.value("gui/patterns/File", "").toString(),
                m_patterns,
                &m_matcher,
                ui->lbPatternsFile);
    m_matcher->setAllowUnfinished(true);
}

void MainWindow::on_bnHalt_clicked()
{
    m_wantInterrupt = true;
    ui->lbState->setText("Aborting...");
    qApp->processEvents();
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}
