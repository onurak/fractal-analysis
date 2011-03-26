#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QParseTreeRender.h"
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

public slots:
    void onSceneMouseMove(QGraphicsSceneMouseEvent *event);

private:
    Ui::MainWindow *ui;
    QParseTreeRender *m_render;
    FL::TimeSeries m_timeSeries;
};

#endif // MAINWINDOW_H
