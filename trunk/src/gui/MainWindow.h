#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QMessageBox>
#include <QResizeEvent>
#include <QTableWidgetItem>
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
    static int NONE;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QSettings& settings() { return m_settings; }

    void showError(const QString &text);
    void showError(const std::string &text);

    void refreshForestInfo();
public slots:
    void onSceneMouseMove(QGraphicsSceneMouseEvent *event);
    bool onParsingProgress(FL::ParseResult pr, FL::Trees::Forest *currentForest);
private:
    void readSettings();
    void writeSettings();
    void markup();
    bool parseAll();
    void loadPatterns(const QString &fileName, FL::Patterns::PatternsSet &patterns);
    QString loadTimeSeries(
        const QString &fileName,
        QString columnName = "",
        bool isDynamic = false,
        int &staticSize = NONE);

    void refreshFilters(QString filtersDir = "");
    QString extractFileName(const QString &fullName);
    void prepareForLongAnalysis();
    void longAnalysisComplete();
private:
    Ui::MainWindow *ui;
    QSettings m_settings;
    QParseTreeRender *m_render;
    bool m_isDynamicTS;
    bool m_isDynamicFirstStep;
    bool m_wantInterrupt;
    int m_cyclesCount;
    QString m_defMetricText;
    bool m_isInitializing;
private:
    FL::TimeSeries m_timeSeries;
    FL::TimeSeries m_dynamicTimeSeries;
    FL::Patterns::PatternsSet m_patterns;
    FL::Patterns::PatternsSet m_markerPatterns;
    FL::Trees::Forest m_forest;
    FL::Trees::MetricsSet m_metrics;
    FL::Forecast m_forecast;
protected:
    virtual void resizeEvent ( QResizeEvent * event );

private slots:
    void spc_action_ExecuteFilter(QAction*);
    void on_actionMarkup_triggered();
    void on_actionBuild_layers_triggered();
    void on_actionOpen_patterns_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomIn_triggered();
    void on_actionOpen_time_series_triggered();
    void on_actionFitAll_triggered();
    void on_actionMarkup_with_roots_triggered();
    void on_sbCurrentTreeIndex_valueChanged(int index);
    void on_actionDynamic_Step_triggered();
    void on_actionOpen_dynamic_time_series_triggered();
    void on_tbOpenMarkerPatternsSet_clicked();
    void on_bnHalt_clicked();
    void on_bnClearForest_clicked();
    void on_tbClearMarkerPatterns_clicked();
    void on_actionBuild_trees_triggered();
    void on_tblMetrics_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
    void on_tblMetrics_itemChanged(QTableWidgetItem* item);
};

#endif // MAINWINDOW_H
