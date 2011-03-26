#include "QParseTreeRender.h"

QParseTreeRender::QParseTreeRender()
{
    m_scene = new QParseTreeScene();

    m_ts = NULL;
}

QParseTreeRender::~QParseTreeRender()
{
    delete m_scene;
}

void QParseTreeRender::setTimeSeries(FL::TimeSeries *ts)
{
    m_scene->setBackgroundBrush(Qt::black);
    m_scene->clear();

    clearCollection(m_tsItems);
    m_ts = ts;
    if (m_ts)
    {
        for (int time = 0; time < ts->values().size()-1; ++time)
        {
            double x1 = time;
            double y1 = ts->values()[time];
            double x2 = time + 1;
            double y2 = ts->values()[time+1];
            //m_tsItems << m_scene->addRect(x1, y1, 5, 5, QPen(Qt::red), QBrush(Qt::red));
            m_tsItems << m_scene->addLine(x1, y1, x2, y2, QPen(Qt::red));
        }
    }
    draw();
}

void QParseTreeRender::draw()
{
    drawCoordinateSystem();
}

void QParseTreeRender::drawCoordinateSystem()
{
    int x_max = 1000;
    int y_max = 1000;
    int x_step = 50;
    int y_step = 50;


    clearCollection(m_coordSystemItems);

    QPen penGrid(QColor(50, 100, 50));
    for (int x = 0; x <= x_max; x += x_step)
        m_coordSystemItems <<
            m_scene->addLine(x, -y_max, x, y_max, penGrid);
    for (int y = -y_max; y <= y_max; y += y_step)
        m_coordSystemItems <<
                m_scene->addLine(0, y, x_max, y, penGrid);

    QPen penAxis(QColor(100, 150, 100));
    m_coordSystemItems <<
            m_scene->addLine(0,  0,      x_max, 0,     penAxis);
    m_coordSystemItems <<
            m_scene->addLine(0, -y_max,  0,     y_max, penAxis);
}
