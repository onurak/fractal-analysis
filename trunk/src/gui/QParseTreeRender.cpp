#include "QParseTreeRender.h"

QParseTreeRender::QParseTreeRender()
{
    m_scene = new QParseTreeScene();
    //m_scene->setSceneRect(0.0, -1.0, 1.0, 2.0);
    m_view = NULL;
    m_ts = NULL;
    m_forest = NULL;
    m_showRoots = true;
    m_mouseX = m_mouseY = 100;
    m_haveCurrentNode = false;
    m_forecastStyle = fsNone;
    m_isYLogScale = false;
    m_fontSize = 10;
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

void QParseTreeRender::addForecast(const FL::Forecasting::Forecast &forecast)
{
    if (m_forecasts.size() > 0)
        m_forecasts.back().nullTrees();
    m_forecasts.push_back(forecast);
    if (m_forecasts.size() > 5)
        m_forecasts.pop_front();
    forecastChanged();
}

void QParseTreeRender::forestChanged()
{
    draw();
}

void QParseTreeRender::forecastChanged()
{
    draw();
}

void QParseTreeRender::draw()
{
    m_scene->setBackgroundBrush(Qt::black);
    m_scene->clear();
    prepare();
    drawCoordinateSystem();
    drawForecast();
    drawTimeSeries();
    drawForest();
}

struct TextRec {
    QString s;
    double x1, y1, x2, y2;
};

void QParseTreeRender::drawForecast()
{
    using namespace FL::Forecasting;

    if (m_forecasts.size() == 0 || m_forest == NULL || m_forest->size() == 0 || m_ts == NULL)
        return;

    for (int i = 0; i < m_forecasts.size(); ++i)
    {
        double no = double(i+1) / double(m_forecasts.size());
        const FL::Forecasting::Forecast &forecast = m_forecasts[i];
        double tx1 = 0.0, tx2 = 10e10, ty1 = -10e10, ty2 = 10e10;

        QBrush brush(QColor(255.0 - 255.0*no, 0, 255.0*no, 100.0*no));

        QPen pen;
        QFont font("Arial");
        font.setPixelSize(m_fontSize);

        const FL::Trees::Tree *tree = m_forest->at(m_currentTree);

        QStack<TextRec> textStack;

        Forecast::const_iterator itfi;
        forall(itfi, forecast)
        {
            const FL::Forecasting::ForecastItem &fi = *itfi;
            if (fi.tree != NULL && !(m_forecastStyle & fsAllForest) && fi.tree != tree)
                continue;

            double x = m_ts->time(fi.pos);
            double y = m_ts->value(fi.pos);

            double x1 = tox(x + fi.minDuration),  x2 = tox(x + fi.maxDuration);
            double y1 = toy(y + fi.minValue),     y2 = toy(y + fi.maxValue);

            if (!(m_forecastStyle & fsIntersectionOnly))
            {
                TextRec tr;
                tr.s = QString("(%1: time: %2-%3, value: %4-%5)")
                        .arg(QStr(fi.patternName))
                        .arg(fi.minDuration, 0, 'f', 2)
                        .arg(fi.maxDuration, 0, 'f', 2)
                        .arg(fi.minValue, 0, 'f', 2)
                        .arg(fi.maxValue, 0, 'f', 2);
                tr.x1 = x1;
                tr.x2 = x2;
                tr.y1 = y1;
                tr.y2 = y2;
                tr.s = QStr(fi.patternName);
                textStack.push(tr);

                m_scene->addRect(x1, y1, x2-x1, y2-y1, pen, brush);
            }
            else
            {
                if (x1 > tx1)
                    tx1 = x1;
                if (x2 < tx2)
                    tx2 = x2;
                if (y1 > ty1)
                    ty1 = y1;
                if (y2 < ty2)
                    ty2 = y2;
            }
        }

        if (m_forecastStyle & fsIntersectionOnly)
        {
            m_scene->addRect(tx1, ty1, tx2-tx1, ty2-ty1, pen, brush);
        }

        for (QStack<TextRec>::iterator i = textStack.begin(); i != textStack.end(); ++i)
        {

            QGraphicsSimpleTextItem *item = m_scene->addSimpleText(i->s, font);
            double x1 = i->x1, y1 = i->y1, x2 = i->x2, y2 = i->y2;
            item->setPos(x1 + (x2-x1)/2, y1 + (y2-y1)/2);
            item->setBrush(Qt::yellow);
            item->scale(0.1, -0.1);
        }
    }
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
    QFont font("Arial");
    font.setPixelSize(m_fontSize);
    const double treeOffset = 0.2*m_fontSize;
    const double delta = 0.1*m_fontSize;


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
        item->scale(0.1, -0.1);

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
            m_scene->addLine(x1, y, x1, y+0.1, finishedNodePen);
            if (node->isFinished())
                m_scene->addLine(x2, y, x2, y+0.1, finishedNodePen)->setData(ASSIGNED_NODE, varNode);
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
    // Draw axis
    QPen penGrid(QColor(10, 50, 10));
    QPen penAxis(QColor(80, 170, 80));
    QPen penCoords(QColor(80, 170, 80));
    QFont font("Arial");
    font.setPixelSize(m_fontSize);

    double minX = tox(m_tsMinTime), maxX = tox(m_tsMaxTime);
    double minY = toy(m_tsMinValue), maxY = toy(m_tsMaxValue);

    if (maxX - minX < 0.1)
        maxX = minX + 1;
    if (maxY - minY < 0.1)
        maxY = minY + 1;
    double xStep = (maxX-minX)/10;
    double yStep = (maxY-minY)/20;

    // Ox
    for (double x = minX; x <= maxX; x += xStep)
    {
        m_scene->addLine(x, minY, x, maxY, penGrid);
        QGraphicsSimpleTextItem *item = m_scene->addSimpleText(QString("%1").arg(x, 0, 'f', 2), font);
        if (x != 0)
        {
            item->setPos(x + 0.005, (maxY-minY)/2. - 0.005);
            item->scale(0.1, -0.1);
            item->setBrush(penCoords.color());
        }
    }

    // Oy
    for (double y = minY; y <= maxY; y += yStep)
    {
        m_scene->addLine(minX, y, maxX, y, penGrid);
        QGraphicsSimpleTextItem *item = m_scene->addSimpleText(QString("%1").arg(y, 0, 'f', 2), font);
        item->setPos(minX + 0.005, y);
        item->scale(0.1, -0.1);
        item->setBrush(penCoords.color());
    }

    // Draw axis itselves
    m_scene->addLine(minX, minY, minX, maxY, penAxis);
    if (minY <= 0 && maxY >= 0)
        m_scene->addLine(minX, 0, maxX, 0, penAxis);
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

        double x = m_tsMinTime;
        double y = m_isYLogScale ? log(m_tsMinValue) : m_tsMinValue;
        double w = m_tsMaxTime-m_tsMinTime;
        double h = m_isYLogScale ? log(m_tsMaxValue) - log(m_tsMinValue) : m_tsMaxValue-m_tsMinValue;

        m_view->setSceneRect(QRectF(x-10, y-10, w+20, h+20));
    }
}

void QParseTreeRender::setMousePos(double x, double y)
{
    m_mouseX = x;
    m_mouseY = y;

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

void QParseTreeRender::setYLogScale(bool value)
{
    if (m_isYLogScale != value)
    {
        m_isYLogScale = value;
        draw();
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

void QParseTreeRender::fitScene()
{
    double x = m_tsMinTime;
    double y = m_isYLogScale ? log(m_tsMinValue) : m_tsMinValue;
    double w = m_tsMaxTime-m_tsMinTime;
    double h = m_isYLogScale ? log(m_tsMaxValue) - log(m_tsMinValue) : m_tsMaxValue-m_tsMinValue;

    m_view->setSceneRect(QRectF(x-10, y-10, w+20, h+20));
    m_view->fitInView(x, y, w, h, Qt::KeepAspectRatio);
}

