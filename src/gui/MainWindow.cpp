#include "MainWindow.h"
#include "ui_MainWindow.h"

inline QString QSTR(const std::string &s)
{
    return QString().fromStdString(s);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_settings(QSettings::IniFormat, QSettings::UserScope, "FL", "FLAnalyzer")
{
    ui->setupUi(this);
    ui->graphicsView->setAlignment(Qt::AlignCenter);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    ui->lbPatternsFile->setText("");
    ui->lbTimeSeriesFile->setText("");

    ui->cbMarker->addItem("AB");
    ui->cbMarker->setCurrentIndex(0);
    ui->cbStaticParser->addItem("Multi Pass");
    ui->cbStaticParser->addItem("Single Pass");
    ui->cbStaticParser->setCurrentIndex(0);
    ui->cbDynamicParser->addItem("Multi Pass");

    m_render = new QParseTreeRender();
    ui->graphicsView->setScene(m_render->scene());
    ui->graphicsView->scale(1, -1);
    m_render->setTimeSeries(&m_timeSeries);
    m_render->setForest(&m_forest);
    m_render->setView(ui->graphicsView);

    connect(m_render->scene(), SIGNAL(mouseMove(QGraphicsSceneMouseEvent*)),
            this,              SLOT(onSceneMouseMove(QGraphicsSceneMouseEvent*)));

    m_isDynamicTS = false;
    m_isDynamicFirstStep = false;
    ui->lbTimeSeriesSize->setText("0");
    readSettings();
    refreshForestInfo();
    ui->statusBar->showMessage(tr("Ready"), 5000);
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
    delete m_render;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    //ui->graphicsView->
}

void MainWindow::onSceneMouseMove(QGraphicsSceneMouseEvent *event)
{
    QString pos = QString("(%1, %2)").arg(event->lastScenePos().x())
                                     .arg(event->lastScenePos().y());
    ui->statusBar->showMessage(pos, 1000);
}

void MainWindow::on_actionOpen_time_series_triggered()
{
    QFileDialog openDialog(
            this, tr("Open time series"),
            m_settings.value("gui/defaultTimeSeriesPath", ".").toString(),
            "Comma-separated values (*.csv);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    m_settings.setValue("gui/defaultTimeSeriesPath", openDialog.directory().absolutePath());
    m_settings.setValue("gui/defaultTimeSeriesFile", openDialog.selectedFiles()[0]);

    QString loadedColumn = loadTimeSeries(openDialog.selectedFiles()[0], "");

    m_settings.setValue("gui/defaultTimeSeriesColumn", loadedColumn);
}

void MainWindow::on_actionOpen_patterns_triggered()
{
    QFileDialog openDialog(
            this, tr("Open patterns"),
            m_settings.value("gui/defaultPatternsPath", ".").toString(),
            "PAT-files (*.pat);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    m_settings.setValue("gui/defaultPatternsPath", openDialog.directory().absolutePath());
    m_settings.setValue("gui/defaultPatternsFile", openDialog.selectedFiles()[0]);

    loadPatterns(openDialog.selectedFiles()[0], m_patterns);
    ui->lbPatternsFile->setText( extractFileName(openDialog.selectedFiles()[0]) );
    ui->lbPatternsCount->setText(QString().setNum(m_patterns.size()));
}

void MainWindow::loadPatterns(const QString &fileName,
                              FL::Patterns::PatternsSet &patterns)
{
    if (fileName.isEmpty())
        return;
    try
    {
        patterns.cleanup();
        FL::IO::FilePAT file;
        if (!file.open(fileName.toStdString(), patterns))
        {
            throw QString("Error opening file!\n") +
                  QStr(FL::errorDescription(file.lastError()));
        }
    }
    catch (const QString &s)
    {
        showError(s);
    }
}

QString MainWindow::loadTimeSeries(const QString &fileName, QString columnName, bool isDynamic)
{
    if (fileName.isEmpty())
        return "";
    try
    {
        FL::IO::FileCSV file;
        if (!file.open(fileName.toStdString()))
            throw QString(tr("Cannot open file"));

        // Load specified column or ask user to peek one
        int columnIndex = 0;
        if (file.header().size() < 1)
            throw QString(tr("File do not contain valid data"));
        if (file.header().size() > 1)
        {
            bool ok;

            // If column name specified in parameter - try to find it
            FL::TimeSeries::Header::const_iterator headerItem;
            if (!columnName.isEmpty())
            {
                columnIndex = file.columnIndex(columnName.toStdString());
                ok  = columnIndex >= 0;
            }
            else
                ok = false;

            // If column name not specified or not found - ask user
            if (!ok)
            {
                QStringList columns;
                forall(headerItem, file.header())
                    columns << QStr(*headerItem);

                columnName =
                   QInputDialog::getItem(
                        this, tr("Open time series"),
                        tr("Which column must be loaded?"),
                        columns, 0, false, &ok);

                if (!ok)
                    return "";
                columnIndex = file.columnIndex(columnName.toStdString());
                if (columnIndex < 0)
                    return "";
            }
        }

        // Clear old value, load new
        FL::TimeSeries tempTimeSeries;
        if (!file.read(tempTimeSeries, columnIndex, FL::TimeSeries::dtValues))
            throw QString(tr("Error reading from file (is it invalid?)"));

        // Want dynamic time series?
        int staticTSCount = tempTimeSeries.values().size();
        if ((m_isDynamicTS = isDynamic) == true)
        {
            bool ok;
            QString text =
                    QString(tr("How many of the %1 values take to a static part?"))
                            .arg(tempTimeSeries.values().size());

            staticTSCount = QInputDialog::getInteger(
                        this, tr("Dynamic time series"),
                        text, tempTimeSeries.values().size() / 2,
                        1, tempTimeSeries.values().size(), 1, &ok);

            if (!ok)
                return "";
        }


        m_timeSeries.clear();
        m_dynamicTimeSeries.clear();
        m_forest.cleanup();

        m_timeSeries.values().assign(
            tempTimeSeries.values().begin(),
            tempTimeSeries.values().begin() + staticTSCount);

        if (m_isDynamicTS && staticTSCount < (int)tempTimeSeries.values().size())
        {
            m_dynamicTimeSeries.values().assign(
                tempTimeSeries.values().begin() + staticTSCount,
                tempTimeSeries.values().end());
        }
    }
    catch (const QString &err)
    {
        isDynamic = false;
        showError(err);
    }
    m_render->timeSeriesChanged();
    ui->lbTimeSeriesSize->setText(QString().setNum(m_timeSeries.values().size()));
    ui->lbTimeSeriesFile->setText(extractFileName(fileName));
    ui->actionDynamic_Step->setEnabled(isDynamic);
    refreshForestInfo();
    return columnName;
}

QString MainWindow::extractFileName(const QString &fullName)
{
    QFileInfo fi(fullName);
    return fi.completeBaseName();
}

void MainWindow::showError(const QString &text)
{
    QMessageBox::critical(this, tr("Error"), text);
    ui->statusBar->showMessage(text, 3000);
}

void MainWindow::showError(const std::string &text)
{
    showError(QStr(text));
}

void MainWindow::readSettings()
{
    int x      = m_settings.value("gui/mainform/x", 50).toInt();
    int y      = m_settings.value("gui/mainform/y", 20).toInt();
    int width  = m_settings.value("gui/mainform/width", 550).toInt();
    int height = m_settings.value("gui/mainform/height", 450).toInt();
    setGeometry(x, y, width, height);

    if (m_settings.value("gui/mainform/maximized", true).toBool())
        setWindowState(windowState() ^ Qt::WindowMaximized);
    else
        setWindowState( QFlags<Qt::WindowState>(windowState() ^ ~Qt::WindowMaximized) );

    loadTimeSeries(
                m_settings.value("gui/defaultTimeSeriesFile", "").toString(),
                m_settings.value("gui/defaultTimeSeriesColumn", "").toString());

    QString fileName = m_settings.value("gui/defaultPatternsFile", "").toString();
    loadPatterns(fileName, m_patterns);
    ui->lbPatternsFile->setText(extractFileName(fileName));
    ui->lbPatternsCount->setText(QString().setNum(m_patterns.size()));

    fileName = m_settings.value("gui/defaultMarkerPatternsFile", "").toString();
    loadPatterns(fileName, m_markerPatterns);
    ui->lbMarkerPatterns->setText(extractFileName(fileName));

    refreshFilters();
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
}

void MainWindow::on_actionZoomIn_triggered()
{
    ui->graphicsView->scale(1.1, 1.1);
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->graphicsView->scale(0.9, 0.9);
}

void MainWindow::on_actionBuild_layers_triggered()
{
    if (m_forest.size() == 0)
        markup();
    else
        parseLevel();
}

void MainWindow::on_actionMarkup_triggered()
{
    markup();
}

void MainWindow::markup()
{
    FL::Markers::AbstractMarker *marker = NULL;
    switch (ui->cbMarker->currentIndex())
    {
        case 0:
            marker = new FL::Markers::AB();
            break;
        default:
            showError(tr("Unknown marker"));
            return;
    }

    m_forest.cleanup();

    FL::ParseResult result = marker->analyze(m_timeSeries, m_forest, m_patterns);
    try
    {
        if (!marker->wasOk())
            throw marker->lastError().arg();

        if (m_markerPatterns.size() > 0)
        {
            FL::Parsers::SinglePass parser;
            parser.parseAll(m_timeSeries, m_forest, m_markerPatterns);
            if (!parser.wasOk())
                throw parser.lastError().arg();
        }

        if (result.somethingAdded())
            m_render->forestChanged();
    }
    catch (const std::string &e)
    {
        showError(e);
    }
    delete marker;
    refreshForestInfo();
}

bool MainWindow::parseLevel()
{
    FL::Parsers::AbstractParser *parser = NULL;

    switch (ui->cbStaticParser->currentIndex())
    {
        case 0:
            parser = new FL::Parsers::MultiPass();
            break;
        case 1:
            parser = new FL::Parsers::SinglePass();
            break;
        default:
            showError(tr("Unknown parser"));
            return false;
    }

    parser->onProgress = delegate(this, &MainWindow::onParsingProgress);

    FL::ParseResult parseResult = parser->analyze(m_timeSeries, m_forest, m_patterns);

    bool result = parser->wasOk();
    if (parser->wasOk())
    {
        if (parseResult.somethingAdded())
            m_render->forestChanged();
    }
    else
    {
        showError(parser->lastError().arg());
    }
    delete parser;
    refreshForestInfo();
    ui->statusBar->showMessage("Ready");

    return result;
}

void MainWindow::on_actionFitAll_triggered()
{
    ui->graphicsView->fitInView(-5, -100, 200, 200);
}

void MainWindow::on_actionMarkup_with_roots_triggered()
{
    FL::Trees::Forest::Iterator tree;
    forall(tree, m_forest)
        (*tree)->markupWithRoots();
    m_render->forestChanged();
}

void MainWindow::refreshForestInfo()
{
    ui->sbCurrentTreeIndex->setEnabled(m_forest.size() > 0);
    ui->sbCurrentTreeIndex->setMinimum(1);
    if (m_forest.size() == 0)
        ui->sbCurrentTreeIndex->setMaximum(1);
    else
        ui->sbCurrentTreeIndex->setMaximum(m_forest.size());
    ui->sbCurrentTreeIndex->setValue(1);
    ui->lbTreeCount->setText(QString().setNum(m_forest.size()));

    int memUsage = 0;
    FL::Trees::Forest::ConstIterator tree;
    forall(tree, m_forest)
        memUsage += (*tree)->bytesUsed();
    ui->lbMemUsage->setText(QString("%1 KB").arg(memUsage/1024));
}

void MainWindow::on_sbCurrentTreeIndex_valueChanged(int index)
{
    if (ui->sbCurrentTreeIndex->isEnabled())
        m_render->setCurrentTree(index-1);
}

bool MainWindow::onParsingProgress(FL::ParseResult pr)
{
    ui->sbCurrentTreeIndex->setEnabled(m_forest.size() > 0);
    ui->sbCurrentTreeIndex->setMinimum(1);
    if (m_forest.size() == 0)
        ui->sbCurrentTreeIndex->setMaximum(1);
    else
        ui->sbCurrentTreeIndex->setMaximum(m_forest.size());
    ui->sbCurrentTreeIndex->setValue(1);
    ui->lbTreeCount->setText(QString().setNum(pr.treesAdded));

    ui->statusBar->showMessage("Busy...");

    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    return true;
}

void MainWindow::spc_action_ExecuteFilter(QAction* a)
{
    if (m_timeSeries.values().size() == 0)
        return;
    //m_forest.cleanup();

    using namespace FL;

    Markers::AbstractMarker *marker = NULL;
    Parsers::AbstractParser *parser = NULL;
    try
    {
        // Load patterns
        Patterns::PatternsSet patterns;
        IO::FilePAT file;
        if ( !file.open(a->data().toString().toStdString(), patterns) )
            throw QString("Error loading filter");

        // Markup
        if (m_forest.size() == 0)
        {
            marker = new Markers::AB();
            marker->analyze(m_timeSeries, m_forest, patterns);
            if (!marker->wasOk())
                throw QString("Marking up error: %1").arg(QSTR(marker->lastError().arg()));
        }

        // Parse
        parser = new Parsers::SinglePass();
        parser->parseAll(m_timeSeries, m_forest, patterns);
        if (!parser->wasOk())
            throw QString("Parsing error: %1").arg(QSTR(parser->lastError().arg()));
    }
    catch (QString s)
    {
        showError(s);
    }

    delete marker;
    delete parser;

    FL::Trees::Forest::Iterator tree;
    forall(tree, m_forest)
        (*tree)->markupWithRoots();
    m_render->setCurrentTree(0);
}

void MainWindow::refreshFilters(QString filtersDir)
{
    if (filtersDir.isEmpty())
        filtersDir = m_settings.value("gui/filtersPath", "../data/patterns/filters/").toString();

    QStringList filesOfInterest;
    filesOfInterest.append("*.pat");

    QDir dir(filtersDir);
    QStringList entries = dir.entryList(filesOfInterest, QDir::Files, QDir::Name);

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

void MainWindow::on_actionDynamic_Step_triggered()
{
    if (!m_isDynamicTS)
        return;

    if (m_dynamicTimeSeries.values().size() == 0)
    {
        showError(tr("No more values in time series"));
        return;
    }

    using namespace FL;

    // Prepare trees for dynamic analysis
    if (m_isDynamicFirstStep)
    {
        Trees::Forest::Iterator tree;
        forall(tree, m_forest)
            (*tree)->makeDynamicEnd(m_timeSeries.values().size()-1);
        m_isDynamicFirstStep = false;
    }

    // Add new value to time series
    m_timeSeries.values().push_back(m_dynamicTimeSeries.values().front());
    m_dynamicTimeSeries.values().erase(m_dynamicTimeSeries.values().begin());

    // Markup and parse with incremental algorithms
    Markers::AbstractMarker *marker = new Markers::Incremental();
    marker->analyze(m_timeSeries, m_forest, m_markerPatterns);
    delete marker;


    //Parsers::AbstractDynamicParser *parser =
      //      new Parsers::DynamicMultiPass();

    //ParseResult pr =
      //      parser->analyze(m_timeSeries, m_forest, m_patterns, m_metrics, m_forecast);

    //delete parser;


    // Refresh UI
    m_render->timeSeriesChanged();
    ui->lbTimeSeriesSize->setText(QString().setNum(m_timeSeries.values().size()));
}

void MainWindow::on_actionOpen_dynamic_time_series_triggered()
{
    QFileDialog openDialog(
            this, tr("Open time series"),
            m_settings.value("gui/defaultTimeSeriesPath", ".").toString(),
            "Comma-separated values (*.csv);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    QString loadedColumn = loadTimeSeries(openDialog.selectedFiles()[0], "", true);
    m_isDynamicFirstStep = true;

    m_settings.setValue("gui/defaultTimeSeriesPath", openDialog.directory().absolutePath());
    m_settings.setValue("gui/defaultTimeSeriesFile", openDialog.selectedFiles()[0]);
    m_settings.setValue("gui/defaultTimeSeriesColumn", loadedColumn);
}

void MainWindow::on_tbOpenMarkerPatternsSet_clicked()
{
    QFileDialog openDialog(
            this, tr("Open patterns"),
            m_settings.value("gui/defaultMarkerPatternsPath", ".").toString(),
            "PAT-files (*.pat);;Any files (*.*)");

    if (!openDialog.exec())
        return;

    m_settings.setValue("gui/defaultMarkerPatternsPath", openDialog.directory().absolutePath());
    m_settings.setValue("gui/defaultMarkerPatternsFile", openDialog.selectedFiles()[0]);

    loadPatterns(openDialog.selectedFiles()[0], m_markerPatterns);
    ui->lbMarkerPatterns->setText( extractFileName(openDialog.selectedFiles()[0]) );
}
