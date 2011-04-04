#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QMessageBox>
#include "QParseTreeRender.h"
#include "Utils.h"
#include "../fractallib/FL.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSettings& settings() { return m_settings; }

    void showError(const QString &text);
    void showError(const std::string &text);
public slots:
    void onSceneMouseMove(QGraphicsSceneMouseEvent *event);

private:
    void readSettings();
    void writeSettings();
    void markup();
    bool parseLevel();
private:
    Ui::MainWindow *ui;
    QSettings m_settings;
    QParseTreeRender *m_render;
private:
    FL::TimeSeries m_timeSeries;
    FL::Patterns::PatternsSet m_patterns;
    FL::Trees::Forest m_forest;
private slots:
    void on_actionMarkup_triggered();
    void on_actionBuild_layers_triggered();
    void on_actionOpen_patterns_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomIn_triggered();
    void on_actionOpen_time_series_triggered();
    void on_actionFitAll_triggered();
};

#endif // MAINWINDOW_H
