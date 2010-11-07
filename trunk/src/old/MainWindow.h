#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QResizeEvent>
#include <QSpinBox>
#include <QTime>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>
#include "DebugDialog.h"
#include "TemplateEditor.h"
#include "ChooseValueDialog.h"
#include "fractallib/Watcher.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    //! default constructor
    MainWindow(QWidget *parent = 0);

    //! destructor
    ~MainWindow();

    //! �������� ��������� � �������
    void error(const std::string &msg) {
        QMessageBox::critical(this, "Error", QSTR(msg));
    }

    //! �������� �������������� ���������
    void info(const QString &msg, bool quite = false, bool alsoSetStatus = true);

    //! ������������ ������, ��������� � ��.
    void redraw();

    //! getter
    FL::Environment& env() { return m_env; }
    FL::ParseTree* currentTree();

    //! �������� ���������� ����
    void loadTS(QString fileName);

    //! ���������� ����������� ����������
    FL::Watcher watcher;
public:
    enum WorkingMode { ANALYSE_TS, EDIT_TS };
    void setMode(WorkingMode mode);
    WorkingMode mode() { return m_mode; }
protected:
    void changeEvent(QEvent *e);

    //! ��������� ��������� �� env
    void loadSettings(FL::Environment &env);

    //! �������� �������
    void onProgress(unsigned char progressValue);
private:
    //! ���������
    Ui::MainWindow *ui;

    //! ��������� ����������
    FL::Environment m_env;

    //! �����������
    FL::QtRender *m_render;

    //! ��������� ������ �������
    FL::ParseTree *m_idealTree;

    //! ������
    QTimer m_timer;

    //! ���������� ��������� ��������� ����
    int m_prevTSPos;

    //! ����� ������
    WorkingMode m_mode;

    //! �������� �� ��������� ������
    bool m_wantDrawIdealTree;

    //! ���������� ������ ���������� ��������� � ����� ������� ������
     int findBestCompareResult(FL::TreeCompareResult &result) {
        int treeNo = -1;
        if (m_idealTree != NULL && watcher.parseTreeSet().size()) {
            for (int i = 0; i < watcher.parseTreeSet().size(); ++i)
            {
                FL::ParseTree *tree = (FL::ParseTree*) watcher.parseTreeSet()[i];
                FL::TreeCompareResult newResult =
                        m_idealTree->compare(tree, FL::tcoIgnoreZeroLevel);
                if (newResult.commonPercent() > result.commonPercent()) {
                    result = newResult;
                    treeNo = i;
                }
            }
        }
        return treeNo;
    }

    //! ���������� ��������� ������
    void generateIdealData(int size);

public slots:
    void onMousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void onMouseRelease(QGraphicsSceneMouseEvent *mouseEvent);
private slots:
    void on_actionIdeal_triggered();
    void on_actionSimple_triggered();
    void on_yMultSlider_valueChanged(int value);
    void on_actionGeneric_Test_triggered();
    void on_checkBox_stateChanged(int );
    void on_sbGeneratorDeep_valueChanged(int );
    void on_actionClear_Time_Series_triggered();
    void on_actionGenerete_one_triggered();
    void on_actionStop_Generator_triggered();
    void on_actionStart_Generator_triggered();
    void on_actionGenerate_triggered();
    void on_sbTreeNo_valueChanged(int );
    void on_actionSave_time_series_triggered();
    void on_actionBuild_tree_triggered();
    void on_actionTemplates_editor_triggered();
    void on_actionDebugWindow_triggered();
    void on_actionUndo_TS_triggered();
    void on_actionCreate_TS_triggered();
    void on_scrollTimeSeries_valueChanged(int value);
    void on_actionLoad_templates_triggered();
    void on_actionNext_fractal_level_triggered();
    void on_actionMarkup_TS_triggered();
    void on_verticalSlider_valueChanged(int value);
    void on_horizontalSlider_valueChanged(int value);
    void on_action_Open_triggered();
    void on_action_LoadEnvironment_triggered();
    void on_timeout();
};


#endif // MAINWINDOW_H
