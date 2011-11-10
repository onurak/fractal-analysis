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
    void buildTrees();
    void loadPatterns(const QString &fileName,
                      FL::Patterns::PatternsSet &patterns,
                      FL::Patterns::Matcher **matcher,
                      QLabel *label = NULL);
    bool loadTimeSeries(
        const QString &fileName,
        QString valuesColumnName = "",
        QString timeColumnName = "");

    void updateForestInfo();

    void initMetrics();
    void readMetrics();
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
    FL::TimeSeries              m_timeSeries;
    FL::Patterns::PatternsSet   m_patterns;
    FL::Patterns::PatternsSet   m_preprocessingPatterns;
    FL::Trees::Forest           m_forest;
    FL::Trees::MetricsSet       m_metrics;
    FL::Patterns::Matcher      *m_matcher;
    FL::Patterns::Matcher      *m_preprocessingMatcher;

    QString m_staticParserName;
    QString m_markerName;
protected:
    void setUiEnables(bool enabled);
    void onParsingBegin();
    bool onParsingProgress(FL::ParseResult result, FL::Trees::Forest *forest);
    bool onParsingFinished(FL::Parsers::AbstractParser *parser);

private:
    QVector<QThread*> m_backgroundTasks;
    void addBackgroundTask(QThread *task);

    friend class BackgroundParserTask;
protected:
    virtual void resizeEvent(QResizeEvent *event);
private slots:
    void onBackgroundTaskFinished();
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
};

/* Background analysis task */
class BackgroundParserTask : public QThread
{
private:
    FL::Parsers::AbstractParser *m_parser;
    MainWindow &mw;
public:
    BackgroundParserTask(MainWindow &mw, FL::Parsers::AbstractParser *parser)
        : m_parser(parser), mw(mw) {}

protected:
    virtual void run()
    {
        m_parser->analyze(
              mw.m_timeSeries, mw.m_forest, mw.m_patterns, *mw.m_matcher, mw.m_metrics);
        mw.onParsingFinished(m_parser);
    }
};

#endif // MAINWINDOW_H
