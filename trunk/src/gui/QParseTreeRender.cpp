#include "QParseTreeRender.h"

QParseTreeRender::QParseTreeRender()
{
    m_scene = new QParseTreeScene();
    m_view = NULL;
    m_ts = NULL;
    m_forest = NULL;
    m_showRoots = true;
}

QParseTreeRender::~QParseTreeRender()
{
    delete m_scene;
}

void QParseTreeRender::setTimeSeries(FL::TimeSeries *ts)
{
    m_ts = ts;
    draw();
}

void QParseTreeRender::timeSeriesChanged()
{
    setTimeSeries(m_ts);
}

void QParseTreeRender::setForest(FL::Trees::Forest *forest)
{
    m_forest = forest;
    forestChanged();
}

void QParseTreeRender::forestChanged()
{
    draw();
}

void QParseTreeRender::draw()
{
    m_scene->setBackgroundBrush(Qt::black);
    m_scene->clear();
    drawCoordinateSystem();
    drawTimeSeries();
    drawForest();
}

void QParseTreeRender::drawForest()
{
    if (m_forest && m_ts)
    {
        FL::Trees::Forest::ConstIterator tree;
        forall(tree, *m_forest)
            drawParseTree(*tree);
    }
}

void QParseTreeRender::drawParseTree(FL::Trees::Tree *tree)
{
    if (!tree) return;

    QColor normalColor(200, 50, 50);
    for (int level = 0; level < tree->levelCount(); ++level)
    {
        const FL::Trees::Layer & layer = tree->nodesByLevel(level);
        drawTreeLayer(layer, normalColor);
    }

    if (m_showRoots)
    {
        QColor rootColor(50, 200, 50);
        drawTreeLayer(tree->roots(), rootColor, ldoDrawTimeSeries);
    }
}

void QParseTreeRender::drawTreeLayer(
    const FL::Trees::Layer &layer,
    QColor color,
    LayerDrawingOptions options)
{
    QPen pen(color);
    QPen dotPen(Qt::DashDotLine);
    dotPen.setColor(color);
    QFont font("Arial", 2);
    const double treeOffset = fabs(m_tsYMin - m_tsYMax) / 14;
    const double delta = 10;


    FL::Trees::Layer::ConstIterator itNode;
    forall(itNode, layer)
    {
        FL::Trees::Node *node = *itNode;

        // Draw node platform
        qreal y = m_tsYMin - treeOffset - node->level() * delta;
        qreal x1 = node->begin() * 5;
        qreal x2 = node->end()   * 5;
        m_scene->addLine(x1, y, x2, y, pen);

        // Draw node text
        QString nodeName =
                QStr(FL::IDGenerator::nameOf(node->id()));
        QGraphicsSimpleTextItem *item = m_scene->addSimpleText(nodeName, font);
        item->setBrush(color);
        item->setPos(x1, y);
        item->scale(1, -1);

        // Draw node connections
        if (node->level() == 0)
        {
            if (node->begin() == 0)
            {
                qreal tsY1 = m_ts->values()[ node->begin() ];
                m_scene->addLine(x1, y, x1, tsY1, dotPen);
            }
            qreal tsY2 = m_ts->values()[ node->end() ];
            m_scene->addLine(x2, y, x2, tsY2, dotPen);
        }
        else
        {
            m_scene->addLine(x1, y, x1, y+1, pen);
            m_scene->addLine(x2, y, x2, y+1, pen);
        }

        // Draw node time series
        if (options & ldoDrawTimeSeries)
        {
            qreal tsY1 = m_ts->values()[ node->begin() ];
            qreal tsY2 = m_ts->values()[ node->end() ];
            m_scene->addLine(x1, tsY1, x2, tsY2, pen);
        }
    }
}

void QParseTreeRender::drawTimeSeries()
{
    if (m_ts)
    {
        for (int time = 0; time < int(m_ts->values().size()-1); ++time)
        {
            double x1 = time*5;
            double y1 = m_ts->values()[time];
            double x2 = (time + 1)*5;
            double y2 = m_ts->values()[time+1];
            m_scene->addLine(x1, y1, x2, y2, QPen(Qt::red));
        }
    }
}

void QParseTreeRender::setView(QGraphicsView *view)
{
    m_view = view;
}

void QParseTreeRender::drawCoordinateSystem()
{
    double x_min = 0, x_max = 1000;
    double y_min = -500, y_max = 500;
    double x_step = 50;
    double y_step = 50;

    // Compute min/max values
    if (m_ts == NULL || m_ts->values().size() < 2)
    {
        x_min = 0;
        x_max = 100;
        y_min = -50;
        y_max = 50;
        x_step = y_step = 10;
    }
    else
    {
        x_min = 0;
        x_max = m_ts->values().size() * 7;
        x_step = (x_max - x_min) / 10;

        m_tsYMin = m_ts->values()[0];
        m_tsYMax = m_ts->values()[0];

        FL::TimeSeries::Data::const_iterator v;
        forall(v, m_ts->values())
        {
            if (m_tsYMin > *v)
                m_tsYMin = *v;
            if (m_tsYMax < *v)
                m_tsYMax = *v;
        }

        y_min = floor(m_tsYMin) - 10.;
        y_max = floor(m_tsYMax) + 10.;

        y_step = (y_max - y_min) / 10;
    }


    // Draw axis
    QPen penGrid(QColor(10, 50, 10));
    QPen penAxis(QColor(80, 170, 80));

    // Ox
    for (double x = x_min; x <= x_max; x += x_step)
        m_scene->addLine(x, y_min, x, y_max, penGrid);

    // Oy
    for (double y = y_min; y <= y_max; y += y_step)
        m_scene->addLine(0, y, x_max, y, penGrid);

    // Draw axis itselves
    m_scene->addLine(0, y_min, 0, y_max, penAxis);
    if (y_min < 0 && y_max > 0)
        m_scene->addLine(x_min, 0, x_max, 0, penAxis);
    else
        m_scene->addLine(x_min, y_max, x_max, y_max, penAxis);

    // Fit scene
    if (m_view)
        m_view->fitInView(x_min-5, y_min, x_max-x_min+10, y_max-y_min, Qt::IgnoreAspectRatio);
}
