#include "MainWindow.h"
#include "ui_MainWindow.h"

// подробна€ статистика
QString statisticsToString(const FL::TreeCompareResult &cmp, FL::ParseTreeSet &trees, int choosedTree)
{
    QString result = QString(
            "Total tree count (choosed tree):\t%1(%2)\n"
            "Total nodes in ideal/parsed tree:\t%3/%4\n"
            "Common nodes:\t\t%5\n"
            "Unique nodes in ideal:\t\t%6\n"
            "Unique nodes in parsed:\t\t%7\n"
            "Ideal tree contains parsed tree:\t%8\n"
            "Parsed tree contains ideal tree:\t%9\n"

    ).arg(trees.size()).
      arg(choosedTree+1).
      arg(cmp.totalInFirst).
      arg(cmp.totalInSecond).
      arg(cmp.commonNodes).
      arg(cmp.uniqueInFirst).
      arg(cmp.uniqueInSecond).
      arg(cmp.firstContainsSecond()).
      arg(cmp.secondContainsFirst());
    result += QString("Common percent:\t\t%1%").arg(cmp.commonPercent());
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    logg.setDebugLevel(G_DBG); // записываем отладочную инфо
    logg.info("Initializing main form...").endl();
    m_env.loadFromFile("config.env");
    //watcher.loadFromEnv(m_env);

    logg.info("Initializing Fractal Library... ");
    watcher.setOption("safeMode", "yes");
    watcher.setOption("marker", "simple");
    watcher.setOption("parser", "ideal");
    watcher.setOption("threadingMode", "none");
    logg.log("OK").endl();

    string patternsFile = m_env["Patterns.LoadFile"];
    logg.info("Loading patterns from " + patternsFile + "... ");
    if (!watcher.setOption("patternsFile", patternsFile))
    {
        logg.log("Failed").endl();
        error("Invalid patterns file");
    }
    else
        logg.log("OK").endl();

    // настройка интерфейса
    logg.info("Initializing render... ");
    m_render = new FL::QtRender();
    ui->setupUi(this);
    ui->graphicsView->setScene(m_render->scene());
    connect(m_render->scene(), SIGNAL(mousePress(QGraphicsSceneMouseEvent*)),
            this, SLOT(onMousePress(QGraphicsSceneMouseEvent*)));
    connect(m_render->scene(), SIGNAL(mouseRelease(QGraphicsSceneMouseEvent*)),
            this, SLOT(onMouseRelease(QGraphicsSceneMouseEvent*)));
    logg.log("OK").endl();

    // внутренние настройки
    m_idealTree = NULL;
    m_wantDrawIdealTree = true;
    setMode(ANALYSE_TS);
    m_prevTSPos = 0;

    // загрузим настройки, нарисуем интерфейс
    loadSettings(m_env);
    redraw();
    logg.info("Main form initialized OK").endl();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_render;
 //   delete watcher.timeSeries();
}

FL::ParseTree* MainWindow::currentTree() {
    return watcher.parseTreeSet().size() && ui->sbTreeNo->value() > 0 ?
            (FL::ParseTree*) watcher.parseTreeSet()[ui->sbTreeNo->value() - 1]
           : NULL;
}

void MainWindow::info(const QString &msg, bool quite, bool alsoSetStatus)
{
    if (!quite)
        QMessageBox::information(this, "Info", msg);
    if (alsoSetStatus)
        ui->statusBar->showMessage(msg);
    logg.info("MainWindow: ").log(STR(msg)).endl();
}

void MainWindow::redraw()
{
    // перерисовывает систему координат, временной р€д и разметку
    m_render->clear();
    m_render->drawTS(watcher.timeSeries(), 0);
    if (currentTree())
        m_render->drawTree(*currentTree(), watcher.timeSeries(), 0, 0);
    if (m_idealTree && m_wantDrawIdealTree)
        m_render->drawTree(*m_idealTree, watcher.timeSeries(), 0, 1);
    m_render->swap();
}

void MainWindow::setMode(WorkingMode mode)
{
    m_mode = mode;
}

void MainWindow::loadTS(QString fileName)
{
    try {
        if (!fileName.isEmpty()) {
            // открываем файла
            FL::FileCSV file(STR(fileName));

            // пробуем угадать или спрашиваем структуру временного р€да
            int index = file.guessTimeSeries();
            if (index == -1)
            {
                ChooseValueDialog dlg(this);
                QStringList columns;
                for (unsigned int i = 0; i < file.header().size(); i++)
                    columns.append(QSTR(file.header()[i]));
                index = dlg.execute("Wich column contains data?", columns);
                if (index == -1)
                    return;
            }

            // записываем ее в watcher.timeSeries(), удал€ем разметку
            FL::TimeSeries *ts = new FL::TimeSeries();
            file.loadTimeSeries(ts, index);
            watcher.setTimeSeries(ts);

            setMode(ANALYSE_TS);
            // сообщение
            info(
                QString(tr("Time series loaded (%1 values)")).arg(watcher.timeSeries()->dimSize(0)),
                true
            );
            ui->lbTSSize->setText(QString("TimeSeries size: %1").arg(watcher.timeSeries()->dimSize(0)));
            redraw();
        }
    } catch (FL::EFile e) {
        error(e.msg());
    }
    ui->scrollTimeSeries->setMaximum(watcher.timeSeries()->dimSize(0) *
        (m_render->cs().systX(1) - m_render->cs().systX(0)) / 5
    );
    ui->scrollTimeSeries->setMinimum(0);
    ui->scrollTimeSeries->setValue(0);
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::onMousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_mode != EDIT_TS) return;
    double value =
            m_render->cs().bY( mouseEvent->buttonDownScenePos(Qt::LeftButton).y() );
    watcher.timeSeries()->add(value);
    redraw();
    ui->statusBar->showMessage(QString("Value added: %1").arg(value));
}

void MainWindow::onMouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_mode != EDIT_TS) return;
}

void MainWindow::on_action_LoadEnvironment_triggered()
{
    // загрузить переменные Environment из файла
    try {
        const QString filter = tr("Config set files (*.env);;Any files (*.*)");
        QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open set file"), tr("./"), filter);
        if (!fileName.isEmpty()) {
            // полностью перезагружаем переменные Environment
            env().loadFromFile(STR(fileName));
            loadSettings(env());

            // Status-сообщение
            ui->statusBar->showMessage(tr("Environment loaded"));
        }
    } catch (FL::EEnvironment e) {
        error(e.msg());
    }
}

void MainWindow::on_action_Open_triggered()
{
    // открытие файлов с временными р€дами
    const QString filter = tr("Comma Separated Values (*.csv);;Any files (*.*)");
    QString fileName =
        QFileDialog::getOpenFileName(this, tr("Open set file"), tr("./"), filter);
    loadTS(fileName);
}

void MainWindow::loadSettings(FL::Environment &env)
{
    /***** Ќастройки интерфейса *****/
    setGeometry(env.valueOf("MainWindow.Left", geometry().x()),
                env.valueOf("MainWindow.Top", geometry().y()),
                env.valueOf("MainWindow.Width", geometry().width()),
                env.valueOf("MainWindow.Height", geometry().height())
        );
    Qt::WindowState miximizeFalg =
            (int)env.valueOf("MainWindow.Maximized", 1) ? Qt::WindowMaximized : Qt::WindowNoState;
    setWindowState(windowState() | miximizeFalg);

    ui->horizontalSlider->setValue(env.valueOf("MainWindow.HorizontalSlider", 100));
    ui->verticalSlider->setValue(env.valueOf("MainWindow.VerticalSlider", 100));

    if (env.valueOf("MainWindow.LogLevel", "debug") != "debug")
        logg.setDebugLevel(G_MED);
    else
        logg.setDebugLevel(G_DBG);

    /***** Ќастройки предыдущей сессии *****/

    // шаблоны
    /*
    string tplDir = env.valueOf("Patterns.LoadDirectory", "");
    if (tplDir != "")
        loadTemplatesDir(QSTR(tplDir));
    else
        loadTemplateFile(QSTR(env.valueOf("Patterns.LoadFile", "")));
    */
    // р€д
    loadTS(QSTR(env.valueOf("TimeSeries.Load", "")));

    logg.info("Main form: settings loaded OK").endl();
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    m_render->cs().setPpx(double(value) / 10.);
    // установим новый максимум дл€ скроллера как (число замеров в р€ду) * (длинна замера) / (5)
    if (watcher.timeSeries())
        ui->scrollTimeSeries->setMaximum(watcher.timeSeries()->dimSize(0) *
            (m_render->cs().systX(1) - m_render->cs().systX(0)) / 5
        );
    else
        ui->scrollTimeSeries->setMaximum(0);
    ui->scrollTimeSeries->setMinimum(0);
    redraw();
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    m_render->cs().setPpy(double(value) / 10.);
    redraw();
}

void MainWindow::on_actionMarkup_TS_triggered()
{
    setMode(ANALYSE_TS);
    if (watcher.markup()) {
        ui->sbTreeNo->setEnabled(watcher.parseTreeSet().size());
        ui->sbTreeNo->setMaximum(watcher.parseTreeSet().size());
        ui->lbTreeCount->setText(QString().setNum(watcher.parseTreeSet().size()));
        if (watcher.parseTreeSet().size())
            ui->sbTreeNo->setValue(1);
        else
            ui->sbTreeNo->setValue(0);
        redraw();
    } else
        error(watcher.lastExceptionMsg());        
}

void MainWindow::on_actionNext_fractal_level_triggered()
{
    setMode(ANALYSE_TS);
    watcher.parser()->onProgress = delegate(this, &MainWindow::onProgress);
    if (watcher.parseLevel()) {
        ui->sbTreeNo->setEnabled(watcher.parseTreeSet().size());
        ui->sbTreeNo->setMaximum(watcher.parseTreeSet().size());
        ui->lbTreeCount->setText(QString().setNum(watcher.parseTreeSet().size()));
        if (watcher.parseTreeSet().size())
            ui->sbTreeNo->setValue(1);
        else
            ui->sbTreeNo->setValue(0);
        redraw();
    } else
        error(watcher.lastExceptionMsg());
}

void MainWindow::on_actionBuild_tree_triggered()
{
    setMode(ANALYSE_TS);
    int levelsCount;
    QTime startTime = QTime::currentTime();

    watcher.parser()->onProgress = delegate(this, &MainWindow::onProgress);
    if (watcher.analyse(&levelsCount)) {
        // расчет времени
        int elapsed = startTime.secsTo( QTime::currentTime() );
        QString text = QString("Complete! Analyse time: %1 sec.\n").arg(elapsed);
        int treeNo = 0;

        // если есть эталонное дерево - вывести результаты сравнени€
        if (m_idealTree) {
            FL::TreeCompareResult cmp;
            treeNo = findBestCompareResult(cmp);
            QMessageBox::information(this, "Total statistics",
                    text + statisticsToString(cmp, watcher.parseTreeSet(), treeNo));
        } else
            info(text);
        ui->sbTreeNo->setEnabled(watcher.parseTreeSet().size());
        ui->sbTreeNo->setMaximum(watcher.parseTreeSet().size());
        ui->lbTreeCount->setText(QString().setNum(watcher.parseTreeSet().size()));
        ui->sbTreeNo->setValue(treeNo+1);
        redraw();
    } else
        error(watcher.lastExceptionMsg());
}


void MainWindow::on_actionLoad_templates_triggered()
{
    const QString filter = tr("Patterns file (*.pat);;Any files (*.*)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open pattern file"), tr("./"), filter);
    watcher.setOption("patternsFile", STR(fileName));
    info(QString("%1 patterns loaded").arg(watcher.patterns().size()), false, true);
}


void MainWindow::on_scrollTimeSeries_valueChanged(int value)
{
    m_render->cs().setXOffset((m_prevTSPos - value*5));
    m_prevTSPos = value;
    redraw();
}

void MainWindow::on_actionCreate_TS_triggered()
{
    setMode(EDIT_TS);
    info("Click to create new value, Ctrl+Z to delete last value", false, true);
    watcher.setTimeSeries(new FL::TimeSeries());
    redraw();
}

void MainWindow::on_actionUndo_TS_triggered()
{
    if (m_mode != EDIT_TS) return;
    int size = watcher.timeSeries()->dimSize(0);
    if (size > 0) {
        watcher.timeSeries()->remove(size-1);
        redraw();
    }
}

void MainWindow::on_actionDebugWindow_triggered()
{
    DebugDialog dbg(this, &watcher.patterns());
    dbg.exec();
}

void MainWindow::on_actionTemplates_editor_triggered()
{
    TemplateEditor editor;
    editor.editTemplates(&watcher.patterns());
}

void MainWindow::on_actionSave_time_series_triggered()
{
    string fileName = STR( QFileDialog::getSaveFileName(this, "Save CSV", ".", "*.csv") );
    if (!fileName.empty()) {
        FL::FileCSV file;
        file.saveToFile(fileName, watcher.timeSeries());
    }
}

void MainWindow::on_sbTreeNo_valueChanged(int value)
{
    if (value != 0)
        redraw();
}

void MainWindow::on_actionGenerate_triggered()
{
    bool ok;
    int size = QInputDialog::getInt(this, tr("Generate Time Series"),
                                    tr("Time series size (minimum)"),
                                    30, 1, 10000, 1, &ok);
    if (ok) {
        generateIdealData(size);
        ui->lbTSSize->setText(QString("Time series size: %1").arg(watcher.timeSeries()->dimSize(0)));
        redraw();
    }
}

void MainWindow::on_timeout()
{
    watcher.generator()->generate(watcher.timeSeries(), 1);
    ui->lbTSSize->setText(QString("Time series size: %1").arg(watcher.timeSeries()->dimSize(0)));
    redraw();
}

void MainWindow::on_actionStart_Generator_triggered()
{
    ui->actionStart_Generator->setEnabled(false);
    ui->actionStop_Generator->setEnabled(true);
    FL::TimeSeries *ts = watcher.timeSeries();
    if (ts == NULL)
        watcher.setTimeSeries(ts = new FL::TimeSeries());
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_timeout()));
    m_timer.start(1000);
}

void MainWindow::on_actionStop_Generator_triggered()
{
    ui->actionStart_Generator->setEnabled(true);
    ui->actionStop_Generator->setEnabled(false);
    m_timer.stop();
}

void MainWindow::on_actionGenerete_one_triggered()
{
    FL::TimeSeries *ts = watcher.timeSeries();
    if (ts == NULL)
        watcher.setTimeSeries(ts = new FL::TimeSeries());
    watcher.generator()->generate(ts, 1);
    redraw();
}

void MainWindow::on_actionClear_Time_Series_triggered()
{
    FL::TimeSeries *ts = watcher.timeSeries();
    if (ts == NULL)
        watcher.setTimeSeries(ts = new FL::TimeSeries());
    else
        ts->data[0].clear();
    watcher.parseTreeSet().clear();
    ui->lbTSSize->setText(QString("Time series size: %1").arg(ts->dimSize(0)));
    redraw();
}

void MainWindow::on_sbGeneratorDeep_valueChanged(int value)
{
    watcher.generator()->setDepth(value);
}


void MainWindow::on_checkBox_stateChanged(int state)
{
    m_wantDrawIdealTree = ui->checkBox->checkState() == Qt::Checked;
    redraw();
}

void MainWindow::generateIdealData(int size)
{
    FL::TimeSeries *ts = watcher.timeSeries();
    if (ts == NULL)
        watcher.setTimeSeries(ts = new FL::TimeSeries());
    ts->data.clear();
    watcher.parseTreeSet().clear();
    if (m_idealTree)
        delete m_idealTree;
    m_idealTree = new FL::ParseTree();
    watcher.generator()->reset();
    watcher.generator()->generate(ts, size, m_idealTree);
}

void MainWindow::on_actionGeneric_Test_triggered()
{
    bool ok;
    int testCount = QInputDialog::getInt(this, tr("Test count"),
                        tr("How much tests?"), 100, 1, 1000, 1, &ok);
    if (!ok)
        return;
    int tsSize = QInputDialog::getInt(this, tr("Time series generator"),
                                      tr("Minimal time series size?"), 1000, 1, 10000, 1, &ok);
    if (!ok)
        return;

    int levelsCount, elapsed = 0, failure = 0, success;

    QTime startTime = QTime::currentTime();
    for (int tmpTest = testCount; tmpTest > 0; --tmpTest)
    {
        generateIdealData(tsSize);
        if (!watcher.analyse(&levelsCount)) {
            error(watcher.lastExceptionMsg());
            return;
        } else {
            FL::TreeCompareResult cmp;
            findBestCompareResult(cmp);
            if (!cmp.secondContainsFirst())
                failure++;
        }
    }
    elapsed += startTime.secsTo( QTime::currentTime() );
    success = testCount - failure;

    QString text = QString(
            "Complete! Tests results:\n"
            "Total tests count/success/failure:\t%1/%2/%3\n"
            "Success percent:\t\t%4%\n"
            "Total time:\t\t\t%5 sec\n"
            "Average analyse time:\t\t%6 sec\n"
       ).arg(testCount).arg(success).arg(failure)
        .arg(int(double(success)/testCount*100.))
        .arg(elapsed).arg(elapsed / testCount);

    info(text);
    redraw();
}

void MainWindow::on_yMultSlider_valueChanged(int value)
{
    m_render->setYMult(double(value) * 0.1);
    redraw();
}

void MainWindow::onProgress(unsigned char progressValue)
{
    ui->progressBar->setValue(progressValue);
    ui->progressBar->repaint();
}

void MainWindow::on_actionSimple_triggered()
{
    watcher.setOption("parser", "simple");
}

void MainWindow::on_actionIdeal_triggered()
{
    watcher.setOption("parser", "ideal");
}
