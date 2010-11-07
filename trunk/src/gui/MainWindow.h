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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QFileDialog>
#include <QInputDialog>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "fractallib/FL.h"
#include "fractallib/QtRender.h"
#include "fractallib/flqt.h"
#include "WorkerThreads.h"
#include "GuiCommon.h"
#include "FLGraphicsView.h"
#include "DebugWindow.h"
#include "TreeFilter.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

/*### Fractal Library Section ###*/
public:
    FL::Watcher watcher;
    FL::Environment env;
    WorkerThread *currentWorker;
private:
    void initializeFractalLibrary();
    void loadTimeSeries(QString fileName);
    void loadPatterns(QString fileName);
    void loadEnvironment();
    void haltAnalysis();
    FL::ParseTree* createRootsMarkupFromTree(FL::ParseTree *tree);

/*### Graphical User Interface Section ###*/
public:
    enum ProgramState
    {
        STATE_INIT,     // первичная инициализация
        STATE_READY,    // ожидание команд пользователя
        STATE_ANALYSIS, // идет анализ
        STATE_DEBUG     // состояние отладки при анализе
    };
private:
    Ui::MainWindow *ui;
    QProgressBar *progressBar;
    FL::QtRender *render;
    int treeNo; //!< Current ParseTree index
    ProgramState state;
    FLGraphicsView *graphicsView;
private:
    void initializeControls();
    void setStatus(QString text, int timeout = 3000);
    void beginProgress(QString text, int min = 0, int max = 100,
                       WorkerThread *thread = NULL, bool async = true);
    void updateProgress(int value, QString text);
    void updateProgress(int step);
    void endProgress();
    void showError(QString msg);
    void showInfo(QString msg);
    void draw();
    void updateInfo();
    void timeSeriesChanged();
    FL::QtRender::TreeDrawingSettings treeDrawingSettings();
    QString defaultTSPath();
    QString defaultPatternsPath();
    void readAnalysisProperties();
    void writeAnalysisProperties();
    void refreshFilters();
/* GUI Events */
private:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void closeEvent(QCloseEvent *event);
public slots:
    void workerThreadStarted();
    void workerThreadProgress(int progress, QString msg);
    void workerThreadFinished(WorkerThread *thread);
    void workerThreadDebugParser(FL::TSParser::DebugEvent event);
private:

private slots:
    void on_action_Debug_window_triggered();
    void on_actionShow_Analysys_properties_triggered();
    void on_actionCreate_markup_from_roots_triggered();
    void on_action_Fit_time_series_triggered();
    void on_actionBuild_trees_triggered();
    void on_action_Run_step_by_step_triggered();
    void on_actionOpen_Environment_triggered();
    void on_actionZoom_out_triggered();
    void on_actionZoom_in_triggered();
    void on_timeSeriesHorizontalScroller_sliderMoved(int position);
    void on_bnHaltAnalysis_clicked();
    void on_bnClearParseTrees_clicked();
    void on_sbTreeNo_valueChanged(int treeNo);
    void on_actionSettings_triggered();
    void on_action_Quit_triggered();
    void on_actionGenerate_time_series_triggered();
    void on_actionOpen_Patterns_triggered();
    void on_actionOpen_Time_Series_triggered();
    void on_actionNext_level_triggered();
    void spc_on_analysisPropertiesChanged();
    void spc_on_drawingWindowSizeChanged();
    void spc_action_ExecuteFilter(QAction* action);
};

#endif // MAINWINDOW_H
