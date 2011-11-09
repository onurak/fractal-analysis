#include "QParseTreeRender.h"

QParseTreeRender::QParseTreeRender()
{
    m_scene = new QParseTreeScene();
    m_scene->setSceneRect(0.0, -1.0, 1.0, 2.0);
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
    prepare();
    drawCoordinateSystem();
    drawTimeSeries();
    drawForest();
}

void QParseTreeRender::drawForest()
{
    if (m_forest && m_ts && m_forest->size())
    {
        //FL::Trees::Forest::ConstIterator tree;
        //forall(tree, *m_forest)
          //  drawParseTree(*tree);
        if (m_currentTree < 0)
            m_currentTree = 0;
        if (m_currentTree >= (int) m_forest->size())
            m_currentTree = (int) m_forest->size()-1;
        drawParseTree(m_forest->at(m_currentTree));
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
    QFont font("Arial", 8);
    const double treeOffset = 0.1;
    const double delta = 0.1;


    FL::Trees::Layer::ConstIterator itNode;
    forall(itNode, layer)
    {
        FL::Trees::Node *node = *itNode;

        // Draw node platform
        qreal y  = toy(m_tsMinValue) - treeOffset - node->level() * delta;
        qreal x1 = tox(m_ts->time(node->begin()));
        qreal x2 = tox(m_ts->time(node->end()));
        m_scene->addLine(x1, y, x2, y, pen);

        // Draw node text
        QString nodeName = QStr(FL::IDGenerator::nameOf(node->id()));
        if (nodeName.isEmpty())
            nodeName = "?";
        QGraphicsSimpleTextItem *item = m_scene->addSimpleText(nodeName, font);
        item->setBrush(color);
        item->setPos((x1 + x2)/2, y);
        qreal hScale = 0.5/(m_view->matrix().m11());
        qreal vScale = 0.5/(m_view->matrix().m22());

        item->scale(hScale, vScale);

        // Draw node connections
        if (node->level() == 0)
        {
            if (node->begin() == 0)
            {
                qreal tsY1 = toy(m_ts->value(node->begin()));
                m_scene->addLine(x1, y, x1, tsY1, dotPen);
            }
            qreal tsY2 = toy(m_ts->value(node->end()));
            m_scene->addLine(x2, y, x2, tsY2, dotPen);
        }
        else
        {
            m_scene->addLine(x1, y, x1, y+0.01, pen);
            m_scene->addLine(x2, y, x2, y+0.01, pen);
        }

        // Draw node time series
        if (options & ldoDrawTimeSeries)
        {
            qreal tsY1 = toy(m_ts->value(node->begin()));
            qreal tsY2 = toy(m_ts->value(node->end()));
            m_scene->addLine(x1, tsY1, x2, tsY2, pen);
        }
    }
}

void QParseTreeRender::drawTimeSeries()
{
    if (m_ts)
    {
        for (int i = 0; i < m_ts->size()-1; ++i)
        {
            double x1 = tox(m_ts->time(i));
            double x2 = tox(m_ts->time(i+1));
            double y1 = toy(m_ts->value(i));
            double y2 = toy(m_ts->value(i+1));
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
//    double x_min = 0, x_max = 1000;
//    double y_min = -500, y_max = 500;
//    double x_step = 50;
//    double y_step = 50;

//    // Compute min/max values
//    if (m_ts == NULL || m_ts->values().size() < 2)
//    {
//        x_min = 0;
//        x_max = 100;
//        y_min = -50;
//        y_max = 50;
//        x_step = y_step = 10;
//    }
//    else
//    {
//        x_min = 0;
//        x_max = m_ts->values().size() * 7;
//        x_step = (x_max - x_min) / 10;

//        y_min = floor(m_tsYMin) - 10.;
//        y_max = floor(m_tsYMax) + 10.;

//        y_step = (y_max - y_min) / 10;
//    }


    // Draw axis
    QPen penGrid(QColor(10, 50, 10));
    QPen penAxis(QColor(80, 170, 80));

    // Ox
    for (double x = 0.0; x <= 1.0; x += 0.1)
        m_scene->addLine(x, -1.0, x, 1.0, penGrid);

    // Oy
    for (double y = -1.0; y <= 1.0; y += 0.1)
        m_scene->addLine(0, y, 1.0, y, penGrid);

    // Draw axis itselves
    m_scene->addLine(0.0,-1.0, 0.0, 1.0, penAxis);
    m_scene->addLine(0.0, 0.0, 1.0, 0.0, penAxis);

    // Fit scene
    if (m_view)
        m_view->fitInView(0.0, -1.0, 1.0, 2.0);
}

void QParseTreeRender::prepare()
{
    if (m_ts && m_ts->size() > 1)
    {
        m_tsMinTime  = m_ts->time(0);
        m_tsMaxTime  = m_ts->time(-1);

        m_tsMinValue = m_ts->value(0);
        m_tsMaxValue = m_ts->value(0);

        for (int i = 1; i < m_ts->size(); ++i)
        {
            if (m_tsMinValue > m_ts->value(i))
                m_tsMinValue = m_ts->value(i);
            if (m_tsMaxValue < m_ts->value(i))
                m_tsMaxValue = m_ts->value(i);
        }
    }
}
