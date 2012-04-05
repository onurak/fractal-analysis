#include "QParseTreeRender.h"

QParseTreeRender::QParseTreeRender()
{
    m_scene = new QParseTreeScene();
    m_scene->setSceneRect(0.0, -1.0, 1.0, 2.0);
    m_view = NULL;
    m_ts = NULL;
    m_forest = NULL;
    m_showRoots = true;
    m_mouseX = m_mouseY = 100;
    m_horCoordLine = m_horCoordLine = NULL;
    m_haveCurrentNode = false;
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
    if (!tree || tree->levelCount() == 0)
        return;

    QColor leafsColor(150, 50, 50);
    drawTreeLayer(tree->nodesByLevel(0), leafsColor, ldoDrawTimeSeries);

    QColor normalColor(100, 0, 100);
    for (int level = 1; level < tree->levelCount(); ++level)
    {
        const FL::Trees::Layer & layer = tree->nodesByLevel(level);
        drawTreeLayer(layer, normalColor, ldoDrawTimeSeries);
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
    QPen finishedNodePen(color);
    QPen dotPen(Qt::DashDotLine);
    dotPen.setColor(color);
    QFont font("Arial", 8);
    const double treeOffset = 0.1;
    const double delta = 0.1;


    FL::Trees::Layer::ConstIterator itNode;
    forall(itNode, layer)
    {
        FL::Trees::Node *node = *itNode;

        FL::Trees::Node tmpNode;
        tmpNode.setId(node->id());
        tmpNode.setBegin(node->begin());
        tmpNode.setEnd(node->end());
        tmpNode.setLevel(node->level());
        //tmpNode.setStatus(node->status());

        QVariant varNode;
        varNode.setValue(tmpNode);

        // Draw node platform
        qreal y  = toy(m_tsMinValue) - treeOffset - node->level() * delta;
        qreal x1 = tox(m_ts->time(node->begin()));
        qreal x2 = tox(m_ts->time(node->end()));

        if (node->isFinished())
        {
            m_scene->addLine(x1, y, x2, y, finishedNodePen);
        }
        else
        {
            QLinearGradient gradient(x1, y, x2, y);
            gradient.setColorAt(0, color);
            gradient.setColorAt(1, QColor(color.red(), color.green(), color.blue(), 50));
            QPen unfinishedNodePen(gradient, 0);

            m_scene->addLine(x1, y, x2, y, unfinishedNodePen)->setData(ASSIGNED_NODE, varNode);
        }

        // Draw node text
        QString nodeName = QStr(FL::IDGenerator::nameOf(node->id()));
        if (nodeName.isEmpty())
            nodeName = "?";
        if (!node->isFinished())
        {
            nodeName += "  (";
            for (size_t i = node->children().size(); i < node->origSequence().size(); ++i)
            {
                QString cinodeName = QStr(FL::IDGenerator::nameOf(node->origSequence()[i].id));
                if (cinodeName.isEmpty())
                    cinodeName = "?";

                nodeName += cinodeName;
                if (i != node->origSequence().size()-1)
                    nodeName += ", ";
            }
            nodeName += ")";
        }

        QGraphicsSimpleTextItem *item = m_scene->addSimpleText(nodeName, font);
        item->setData(ASSIGNED_NODE, varNode);
        item->setBrush(color);
        //item->setPos((x1 + x2)/2, y);
        item->setPos(x1, y);
//        qreal hScale = 0.5/(m_view->matrix().m11());
//        qreal vScale = 0.5/(m_view->matrix().m22());
        qreal hScale = 0.003;
        qreal vScale = -0.009;
        item->scale(hScale, vScale);

        // Draw node connections
        if (node->level() == 0)
        {
            if (node->begin() == 0)
            {
                qreal tsY1 = toy(m_ts->value(node->begin()));
                m_scene->addLine(x1, y, x1, tsY1, dotPen)->setData(ASSIGNED_NODE, varNode);
            }
            qreal tsY2 = toy(m_ts->value(node->end()));
            if (node->isFinished())
                m_scene->addLine(x2, y, x2, tsY2, dotPen)->setData(ASSIGNED_NODE, varNode);
        }
        else
        {
            m_scene->addLine(x1, y, x1, y+0.01, finishedNodePen);
            if (node->isFinished())
                m_scene->addLine(x2, y, x2, y+0.01, finishedNodePen)->setData(ASSIGNED_NODE, varNode);
        }

        // Draw node time series
        if (options & ldoDrawTimeSeries)
        {
            qreal tsY1 = toy(m_ts->value(node->begin()));
            qreal tsY2 = toy(m_ts->value(node->end()));
            m_scene->addLine(x1, tsY1, x2, tsY2, finishedNodePen)->setData(ASSIGNED_NODE, varNode);
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
    QPen penCoords(QColor(5, 40, 5));

    // Ox
    for (double x = 0.0; x <= 1.0; x += 0.1)
        m_scene->addLine(x, -1.0, x, 1.0, penGrid);

    // Oy
    for (double y = -1.0; y <= 1.0; y += 0.1)
        m_scene->addLine(0, y, 1.0, y, penGrid);

    // Draw axis itselves
    m_scene->addLine(0.0,-1.0, 0.0, 1.0, penAxis);
    m_scene->addLine(0.0, 0.0, 1.0, 0.0, penAxis);

    m_horCoordLine = m_scene->addLine(0, 0, 1, 0, penCoords);
    m_horCoordLine->setVisible(false);
    m_vertCoordLine = m_scene->addLine(0, -1, 0, 1, penCoords);
    m_vertCoordLine->setVisible(false);

    // Fit scene
//    if (m_view)
//        m_view->fitInView(-0.5, -1.5, 2.0, 3.0);
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

        m_yHalfRange = std::max(fabs(m_tsMinValue), fabs(m_tsMaxValue));
    }
}

void QParseTreeRender::setMousePos(double x, double y)
{
    m_mouseX = x;
    m_mouseY = y;

    if (m_horCoordLine)
    {
//        m_horCoordLine->setVisible(y >= -1 && y <= 1);
        m_horCoordLine->setLine(0, y, 1, y);
    }

    if (m_vertCoordLine)
    {
//        m_vertCoordLine->setVisible(x >= 0 && x <= 1);
        m_vertCoordLine->setLine(x, -1, x, 1);
    }

    QGraphicsItem *item = m_scene->itemAt(x, y);

    if (item != NULL)
    {
        QVariant v = item->data(ASSIGNED_NODE);
        if (!v.isNull())
        {
            m_currentNode = v.value<FL::Trees::Node>();
            QString nodeName = QString().fromStdString(
                FL::IDGenerator::nameOf(m_currentNode.id()));
            m_view->setToolTip(nodeName);
            m_haveCurrentNode = true;
        }
        else
        {
            m_haveCurrentNode = false;
        }
    }
}

double QParseTreeRender::currentValue(bool *ok) const
{
    if (m_ts && m_ts->size() > 0 && m_mouseY >= -1 && m_mouseY <= 1)
    {
        double value = m_mouseY * m_yHalfRange;
        if (ok) *ok = true;
        return value;
    }
    else
    {
        if (ok) *ok = false;
        return 0;
    }
}

double QParseTreeRender::currentTime(bool *ok) const
{
    if (m_ts && m_ts->size() > 0 && m_mouseX >= 0 && m_mouseX <= 1)
    {
        int timeIndex = int(m_mouseX * m_ts->size());
        if (ok) *ok = true;
        return m_ts->time(timeIndex);
    }
    else
    {
        if (ok) *ok = false;
        return 0;
    }
}

const FL::Trees::Node* QParseTreeRender::currentNode() const
{
    if (m_haveCurrentNode)
        return &m_currentNode;
    else
        return NULL;
}
