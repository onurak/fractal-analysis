#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < 1000; ++i)
        m_timeSeries.values().push_back(33);

    m_render = new QParseTreeRender();
    ui->graphicsView->setScene(m_render->scene());
    m_render->setTimeSeries(&m_timeSeries);

    connect(m_render->scene(), SIGNAL(mouseMove(QGraphicsSceneMouseEvent*)),
            this,              SLOT(onSceneMouseMove(QGraphicsSceneMouseEvent*)));

    ui->statusBar->showMessage(tr("Ready"), 5000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_render;
}

void MainWindow::onSceneMouseMove(QGraphicsSceneMouseEvent *event)
{
    QString pos = QString("(%1, %2)").arg(event->lastScenePos().x())
                                     .arg(event->lastScenePos().y());
    ui->statusBar->showMessage(pos, 1000);
}
