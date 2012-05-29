#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QSettings>
#include <QMessageBox>
#include <QThread>
#include <QDesktopWidget>
#include <QCheckBox>
#include <QMutex>
#include <QMutexLocker>
#include <QTime>
#include <QResizeEvent>
#include <numeric>
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
    void onSceneMouseEvent(QGraphicsSceneMouseEvent *event);
    void onSceneWheelEvent(QGraphicsSceneWheelEvent *event);

private:
    void readSettings();
    void writeSettings();
    void markup();
    void staticAnalysis();
    void dynamicAnalysis();
    void updateForecast();

    void loadPatterns(const QString &fileName,
                      FL::Patterns::PatternsSet &patterns,
                      FL::Patterns::Matcher **matcher,
                      QLabel *label = NULL);



    enum TSLoadMethod {
        tloStatic,
        tloDynamic
    };

    bool loadTimeSeries(
        const QString &fileName,
        TSLoadMethod options = tloStatic,
        QString valuesColumnName = "",
        QString timeColumnName = "");

    void updateForestInfo();

    void initMetrics();
    void readMetrics();

    void updateMetrics(const QPair<double, double> newValue);
    std::vector<double> m_efficiency;
    std::vector<double> m_accuracy;
private:
    Ui::MainWindow *ui;
    QSettings m_settings;
    QParseTreeRender *m_render;

    // While this flag is enabled, window performs first
    // initialization, so no one must show error messages
    bool m_isInitializing;
private:
    /****************************************
     * FL data
     ****************************************/
    FL::TimeSeries              m_timeSeries;      // Current time series values
    FL::Patterns::PatternsSet   m_patterns;
    FL::Patterns::PatternsSet   m_preprocessingPatterns;
    FL::Trees::Forest           m_forest;
    FL::Trees::MetricsSet       m_metrics;
    FL::Patterns::Matcher      *m_matcher;
    FL::Patterns::Matcher      *m_preprocessingMatcher;
    FL::Forecasting::Forecast   m_forecast;
    bool                        m_wantInterrupt;
    QTime                       m_lastUpdateTime;
protected:
    /****************************************
     * Application internal data
     ****************************************/
    QString m_staticParserName;
    QString m_markerName;
    QList< QPair<double, double> > m_timeSeriesCache; // Arrived values that hasn't been analyzed yet
    QMutex addDataMutex;
    QVector<QThread*> m_backgroundTasks;

    QLabel *m_lbCurrentTime, *m_lbCurrentValue;
    QLabel *m_lbCurrentNode;

protected:
    void setUiEnables(bool enabled);
    void addBackgroundTask(QThread *task);
    friend class BackgroundParserTask;
private slots:
    void onParsingBegin();
    bool onParsingProgress(FL::Trees::Forest *forest);
    void onParsingFinished(FL::Parsers::AbstractParser *parser);
    void onNewTimeSeriesData(const QList< QPair<double, double> > &data);
    void onBackgroundTaskFinished();
protected:
    virtual void resizeEvent(QResizeEvent *event);
private slots:
    void on_actionMarkup_triggered();
    void on_actionOpen_patterns_triggered();
    void on_actionZoomOut_triggered();
    void on_actionZoomIn_triggered();
    void on_actionOpen_time_series_triggered();
    void on_actionFitAll_triggered();
    void on_actionMarkup_with_roots_triggered();
    void on_bnClearPatterns_clicked();
    void on_actionBuild_trees_triggered();
    void on_actionOpen_preprocessing_patterns_triggered();
    void on_bnClearPreprocessingPatterns_clicked();
    void on_cbStaticParser_currentIndexChanged(const QString &arg1);
    void on_cbMarker_currentIndexChanged(const QString &arg1);
    void on_sbParseTreeIndex_valueChanged(int arg1);
    void on_bnClearForest_clicked();
    void on_tableMetrics_cellChanged(int row, int column);
    void on_actionOpen_dynamic_time_series_triggered();
    void on_action_Dynamic_step_triggered();
    void on_bnDeleteCurrentTree_clicked();
    void on_bnRefreshPreprocessingPatterns_clicked();
    void on_bnRefreshPatterns_clicked();
    void on_bnHalt_clicked();
    void on_actionQuit_triggered();
    void on_cbForecastStyle_currentIndexChanged(int index);
    void on_actionLogScaleY_triggered();
    void on_actionIncFontSize_triggered();
    void on_actionDecFontSize_triggered();
};

#endif // MAINWINDOW_H
