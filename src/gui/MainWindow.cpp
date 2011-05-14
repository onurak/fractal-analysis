#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_settings(QSettings::IniFormat, QSettings::UserScope, "FL", "FLAnalyzer")
{
    ui->setupUi(this);

    m_render = new QParseTreeRender();
    ui->graphicsView->setScene(m_render->scene());
    ui->graphicsView->scale(1, -1);
    m_render->setTimeSeries(&m_timeSeries);
    m_render->setForest(&m_forest);
    m_render->setView(ui->graphicsView);

    connect(m_render->scene(), SIGNAL(mouseMove(QGraphicsSceneMouseEvent*)),
            this,              SLOT(onSceneMouseMove(QGraphicsSceneMouseEvent*)));

    ui->statusBar->showMessage(tr("Ready"), 5000);
    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
    delete m_render;
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

    QString loadedColumn = loadTimeSeries(openDialog.selectedFiles()[0]);

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

    loadPatterns(openDialog.selectedFiles()[0]);
}

void MainWindow::loadPatterns(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    try
    {
        m_patterns.cleanup();
        FL::IO::FilePAT file;
        if (!file.open(fileName.toStdString(), m_patterns))
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

QString MainWindow::loadTimeSeries(const QString &fileName, QString columnName)
{
    if (fileName.isEmpty())
        return "";
    try
    {
        FL::IO::FileCSV file;
        if (!file.open(fileName.toStdString()))
            throw QString(tr("Cannot open file"));

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
        m_timeSeries.clear();
        m_forest.cleanup();
        if (!file.read(m_timeSeries, columnIndex, FL::TimeSeries::dtValues))
            throw QString(tr("Error reading from file (is it invalid?)"));
    }
    catch (const QString &err)
    {
        showError(err);
    }
    m_render->timeSeriesChanged();
    return columnName;
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
    if (m_settings.value("gui/mainform/maximized", true).toBool())
        setWindowState(windowState() ^  Qt::WindowMaximized);
    else
        setWindowState( QFlags<Qt::WindowState>(windowState() ^ ~Qt::WindowMaximized) );

    int x      = m_settings.value("gui/mainform/x", 50).toInt();
    int y      = m_settings.value("gui/mainform/y", 20).toInt();
    int width  = m_settings.value("gui/mainform/width", 550).toInt();
    int height = m_settings.value("gui/mainform/height", 450).toInt();
    setGeometry(x, y, width, height);
    loadTimeSeries(
                m_settings.value("gui/defaultTimeSeriesFile", "").toString(),
                m_settings.value("gui/defaultTimeSeriesColumn", "").toString());
    loadPatterns(m_settings.value("gui/defaultPatternsFile", "").toString());
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
    m_forest.cleanup();

    FL::Markers::AbstractMarker *marker = new FL::Markers::AB();
    FL::ParseResult result = marker->analyze(m_timeSeries, m_forest, m_patterns);
    if (marker->wasOk())
    {
        if (result.somethingAdded())
            m_render->forestChanged();
    }
    else
    {
        showError(marker->lastError().arg());
    }
    delete marker;
}

bool MainWindow::parseLevel()
{
    FL::Parsers::AbstractParser *parser =
            new FL::Parsers::MultiPass();

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
