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
#include "ui_MainWindow.h"
#include "MainWindow_GUI_Logic.cpp"
#include "MainWindow_FL_Logic.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    if (QFile::exists("config.env"))
        if (!env.loadFromFile("config.env"))
            showError("Error loading environment (it must be invalid file format)");
    this->state = STATE_INIT;
    ui->setupUi(this);
    initializeFractalLibrary();
    initializeControls();
    loadEnvironment();
    setStatus(tr("Ready"));
    this->state = STATE_READY;
    updateInfo();
}

MainWindow::~MainWindow()
{
    delete graphicsView;
    delete render;
    delete progressBar;
    delete ui;
}

void MainWindow::on_actionNext_level_triggered()
{
    // Never run two analysis process!
    if (this->state == STATE_ANALYSIS)
    {
        showError(tr("Already analysing!"));
        return;
    }
    // Try analyse
    if (watcher.patterns().size() > 0)
    {
        WorkerThreadLevelAnalyser *thread = new WorkerThreadLevelAnalyser(watcher);
        beginProgress(tr("Looking for fractal level..."), 0, 100, thread);
        draw();
    }
    else
    {
        logg.error("Can't analyse level - no patterns loaded");
        showError("Patterns not loaded");
    }
    updateInfo();
}

void MainWindow::on_actionBuild_trees_triggered()
{
    // Never run two analysis process!
    if (this->state == STATE_ANALYSIS)
    {
        showError(tr("Already analysing!"));
        return;
    }
    // Try analyse
    if (watcher.patterns().size() > 0)
    {
        WorkerThreadFullAnalyser *thread = new WorkerThreadFullAnalyser(watcher);
        beginProgress(tr("Looking for fractal level..."), 0, 100, thread);
        draw();
    }
    else
    {
        logg.error("Can't analyse level - no patterns loaded");
        showError("Patterns not loaded");
    }
    updateInfo();
}


void MainWindow::on_actionOpen_Time_Series_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open time series file"), defaultTSPath(),
            tr("CSV files (*.csv);;Any files (*)"));
    if (!fileName.isEmpty())
    {
        loadTimeSeries(fileName);
        timeSeriesChanged();
        draw();
        updateInfo();
    }
}

void MainWindow::on_actionOpen_Patterns_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
              tr("Open pattern file"),  defaultPatternsPath(),
              tr("Patterns file (*.pat);;Any files (*)"));
    if (!fileName.isEmpty())
    {
        loadPatterns(fileName);
        updateInfo();
    }
}

void MainWindow::on_actionGenerate_time_series_triggered()
{
    bool ok;
    int tsMinSize = QInputDialog::getInt(this,
                         "Generate time series",
                         "Minimum time series size to generate?",
                         50, 1, 10000, 1, &ok);
    if (ok)
    {
        FL::TimeSeries *ts = new FL::TimeSeries();
        FL::TSGeneratorElliott g;
        g.generate(ts, tsMinSize);
        watcher.setTimeSeries(ts);
        timeSeriesChanged();
        draw();
        updateInfo();
    }
}

void MainWindow::on_action_Quit_triggered()
{
    close();
}

void MainWindow::on_actionSettings_triggered()
{

}

void MainWindow::on_sbTreeNo_valueChanged(int treeNo)
{
    if (watcher.parseTreeSet().size() == 0)
        return;
    --treeNo;
    if (treeNo >= 0 && treeNo < watcher.parseTreeSet().size())
    {
        this->treeNo = treeNo;
        draw();
    }
    else
        setStatus(tr("Internal error: invalid parse tree index"));
}

void MainWindow::on_bnClearParseTrees_clicked()
{
    if (this->state != STATE_READY)
    {
        showError(tr("Wait until progress in process!"));
        return;
    }
    int ret = QMessageBox::warning(this, "Warning!", "Clear all parse trees?",
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes)
    {
        watcher.parseTreeSet().clear();
        updateInfo();
        draw();
    }
}

void MainWindow::on_bnHaltAnalysis_clicked()
{
    int ret = QMessageBox::warning(this, "Warning!", "Halt analysis process?",
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes)
        haltAnalysis();
}

void MainWindow::on_timeSeriesHorizontalScroller_sliderMoved(int position)
{
    render->setHorizontalScrollerPos(position);
    draw();
}

void MainWindow::on_actionZoom_in_triggered()
{
    render->zoom(0.7);
    timeSeriesChanged();
    draw();
}

void MainWindow::on_actionZoom_out_triggered()
{
    render->zoom(1.3);
    timeSeriesChanged();
    draw();
}

void MainWindow::on_actionOpen_Environment_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open environment file"), ".",
            tr("ENV files (*.env);;Any files (*)"));
    if (!fileName.isEmpty())
    {
        env.loadFromFile(STR(fileName));
        loadEnvironment();
    }
}

void MainWindow::on_action_Run_step_by_step_triggered()
{
    // Never run two analysis process!
    if (this->state == STATE_ANALYSIS)
    {
        showError(tr("Already analysing!"));
        return;
    }
    // Try analyse
    if (watcher.patterns().size() > 0)
    {
        WorkerThreadLevelAnalyser *thread = new WorkerThreadLevelAnalyser(watcher, true);
        beginProgress(tr("Debugging started..."), 0, 100, thread);
        draw();
    }
    else
    {
        logg.error("Can't analyse level - no patterns loaded");
        showError("Patterns not loaded");
    }
    updateInfo();
}

void MainWindow::on_action_Fit_time_series_triggered()
{
    render->fitTimeSeries();
    draw();
}

void MainWindow::on_actionCreate_markup_from_roots_triggered()
{
    if (watcher.parseTreeSet().size() == 0)
    {
        showError("Create parse tree first");
        return;
    }

    for (int i = 0; i < watcher.parseTreeSet().size(); ++i)
    {
        FL::ParseTree *tree = watcher.parseTreeSet()[i];
        watcher.parseTreeSet().trees[i] = createRootsMarkupFromTree(tree);
        delete tree;
    }
    draw();
}

void MainWindow::on_actionShow_Analysys_properties_triggered()
{
    bool visible = !ui->analysisPropertiesWidget->isVisible();
    ui->analysisPropertiesWidget->setVisible(visible);
    ui->actionShow_Analysys_properties->setChecked(visible);
    env["MainWindow"]["ViewAnalysisProperties"] = visible;
}

void MainWindow::on_action_Debug_window_triggered()
{
    DebugWindow wnd(this);
    wnd.exec();
}
