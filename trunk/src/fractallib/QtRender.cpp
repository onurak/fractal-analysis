/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "QtRender.h"
#include <QLinearGradient>

using namespace FL;

/* IMPLEMENTATION OF QtScene */

QtScene::QtScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 1000, 600);
}

void QtScene::keyPressEvent (QKeyEvent *keyEvent)
{

}

void QtScene::keyReleaseEvent (QKeyEvent *keyEvent)
{

}

void QtScene::mouseDoubleClickEvent (QGraphicsSceneMouseEvent *mouseEvent)
{

}

void QtScene::mouseMoveEvent (QGraphicsSceneMouseEvent *mouseEvent)
{
    emit mouseMove(mouseEvent);
}

void QtScene::mousePressEvent (QGraphicsSceneMouseEvent *mouseEvent)
{
    emit mousePress(mouseEvent);
}

void QtScene::mouseReleaseEvent (QGraphicsSceneMouseEvent *mouseEvent)
{
    emit mouseRelease(mouseEvent);
}

void QtScene::wheelEvent (QGraphicsSceneWheelEvent *wheelEvent)
{

}

/* IMPLEMENTATION OF QtRender */

QtRender::QtRender()
{
    m_timeSeries = NULL;
    //m_cs = new CoordSystem(2, 2);
    m_scene = new QtScene(NULL);
    m_framebuffer = m_csbuffer = NULL;
    m_minX = 0;
    m_maxX = 100;
    m_minY = 0;
    m_maxY = 100;
    m_xrange = m_maxX - m_minX;
    m_yrange = m_maxY - m_minY;
    m_xborder = 30;
    m_yborder = 20;
    m_horScrollerPos = minimumHorizontalScroll();
    updateScroller();
    resize(m_scene->width(), m_scene->height());
    //m_cs->onChange += delegate(this, &QtRender::bufferCoordSystem);
    logg.info("QtRender: Init OK");
}

QtRender::~QtRender()
{
    m_scene->clear();
    delete m_framebuffer;
    delete m_csbuffer;
    delete m_scene;
 //   delete m_cs;
    logg.info("QtRender: Destroy OK");
}

void QtRender::resize(int width, int height)
{
    delete m_framebuffer;
    m_framebuffer = new QPixmap(width, height);
    m_width = width - 2*m_xborder;
    m_height = height - 2*m_yborder;
    //m_cs->setWidth(width);
    //m_cs->setHeight(height);
    m_scene->clear();
    m_scene->setSceneRect(0, 0, width, height);
    bufferCoordSystem();
}

void QtRender::clear()
{
    m_scene->clear();
    m_painter.begin(m_framebuffer);
    m_painter.setBrush(Qt::black);
    m_painter.setPen(Qt::black);
    m_painter.drawRect(0, 0, m_framebuffer->width(), m_framebuffer->height());
    if (m_csbuffer)
        m_painter.drawPixmap(0, 0, *m_csbuffer);
}

void QtRender::swap()
{
    m_painter.end();
    m_scene->addPixmap(*m_framebuffer);
}

void QtRender::draw()
{
    clear();
    if (m_timeSeries)
    {
        drawTS(m_timeSeries, m_tsdim);

        std::vector<ParseTreeReg>::iterator i;
        int no;
        for (no = 0, i = m_trees.begin(); i != m_trees.end(); ++i, ++no)
            drawTree(*(i->tree), no, i->settings);

        std::vector<Forecast*>::iterator f;
        for_each_(f, m_forecasts)
            drawForecast(*f);
    }
    swap();
}

void QtRender::zoom(double factor)
{
    setAxis(m_minX * factor, m_maxX * factor, m_minY, m_maxY);
}

void QtRender::addTree(ParseTree *tree, int level, TreeDrawingSettings settings)
{
    std::vector<ParseTreeReg>::iterator i;
    // If tree already registered -refresh its settings
    for (i = m_trees.begin(); i != m_trees.end(); ++i)
    {
        if (i->tree == tree)
        {
            i->level = level;
            i->settings = settings;
            return;
        }
    }
    // If tree not registered - add registration
    m_trees.push_back( ParseTreeReg(tree, level, settings) );
}

void QtRender::remTree(ParseTree *tree)
{
    std::vector<ParseTreeReg>::iterator i;
    for (i = m_trees.begin(); i != m_trees.end(); ++i)
    {
        if (i->tree == tree)
        {
            m_trees.erase(i);
            return;
        }
    }
}

void QtRender::clearTrees()
{
    m_trees.clear();
}

void QtRender::addForecast(Forecast *forecast)
{
    if (!find_(forecast, m_forecasts))
        m_forecasts.push_back( forecast );
}

void QtRender::remForecast(Forecast *forecast)
{
    std::vector<Forecast*>::iterator i;
    for (i = m_forecasts.begin(); i != m_forecasts.end(); ++i)
    {
        if (*i == forecast)
        {
            m_forecasts.erase(i);
            return;
        }
    }
}

void QtRender::clearForecasts()
{
    m_forecasts.clear();
}

void QtRender::setTimeSeries(FL::TimeSeries *ts, int dimension)
{
    m_timeSeries = ts;
    m_tsdim = dimension;
    m_horScrollerPos = 0;
    if (ts)
        fitTimeSeries();
}

void QtRender::fitTimeSeries(double timeRange, double minY, double maxY)
{
    std::vector<double> vec;
    if (m_timeSeries == NULL || !m_timeSeries->getTSByIndex(vec, m_tsdim))
        return;
    // Find maximum and minimum of time series
    double yMin = 10e10, yMax = -10e10;
    std::vector<double>::const_iterator p;
    for (p = vec.begin(); p != vec.end(); ++p)
    {
        if (yMin > *p) yMin = *p;
        if (yMax < *p) yMax = *p;
    }
    if (timeRange == AUTO)
        timeRange = std::min(m_timeSeries->dimSize(m_tsdim), 100);
    yMin = yMin - 30;
    setAxis(0, timeRange, std::max(yMin, minY), std::min(yMax+1, maxY));
    updateScroller();
}

void QtRender::updateScroller()
{
    m_tsFitCountX = 0;
    if (m_timeSeries)
    {
        // Get time series
        std::vector<double> vec;
        if (!m_timeSeries->getTSByIndex(vec, m_tsdim))
            return;
        // Limit scroller position
        int dimsz = m_timeSeries->dimSize(m_tsdim);
        if (m_horScrollerPos >= dimsz)
            m_horScrollerPos = dimsz-1;

        // Find how much values of time series can be fit
        //std::vector<double>::const_iterator
        //        i = vec.begin() + m_horScrollerPos;
        //while (i != vec.end() && tx(*i) <= m_width)
        //    ++i;
        int i = 0;
        while (i < (int)dimsz && tx(i) <= m_width)
            ++i;
        m_tsFitCountX = i;
    }
}

void QtRender::setTreeDrawingSettings(TreeDrawingSettings settings)
{
    std::vector<ParseTreeReg>::iterator i;
    for_each_(i, m_trees)
        i->settings = settings;
}

void QtRender::bufferCoordSystem()
{
    // Recreate coordinate system buffer
    delete m_csbuffer;
    m_csbuffer = new QPixmap(m_framebuffer->width(), m_framebuffer->height());

    QPainter bPainter;
    bPainter.begin(m_csbuffer);

    bPainter.setBrush(Qt::black);
    bPainter.drawRect(0, 0, m_csbuffer->width(), m_csbuffer->height());

    // If zero is inside (min_, max_) than draw line trought it, otherwize throuht min_
    int x0 = tx(m_minX > 0 || m_maxX < 0 ? m_minX : 0);
    int y0 = ty(m_minY > 0 || m_maxY < 0 ? m_minY : 0);
    int xMin = tx(m_minX), xMax = tx(m_maxX);
    int yMin = ty(m_minY), yMax = ty(m_maxY);

    // Prepare some extra variables
    int prev;
    QString s;
    QColor axisColor(0, 100, 0);
    QColor gridColor(0, 30, 0);
    double step;

    bPainter.setPen(gridColor);

    // Draw markes on the Oy axe
    prev = yMin;
    step = m_yrange / 10;
    for (double y = m_minY; y <= m_maxY; y += step)
    {
        int ry = ty(y);
        bPainter.drawLine(xMin, ry, xMax, ry);
        // To not let numbers overlap each other
        if (abs(prev - ry) > 20)
        {
            bPainter.setPen(axisColor);
            bPainter.drawText(x0 - m_xborder + 1, ry - 5, s.setNum(y));
            bPainter.setPen(gridColor);
            prev = ry;
        }
    }

    // Draw markes on the Ox axe
    prev = xMin;
    step = m_xrange / 10;
    for (double x = m_minX; x <= m_maxX; x += step)
    {
        int rx = tx(x);
        bPainter.drawLine(rx, yMin, rx, yMax);
        // To not let numbers overlap each other
        if (abs(prev - rx) > 20)
        {
            bPainter.setPen(axisColor);
            bPainter.drawText(rx - 15, y0 + 10, s.setNum(x));
            bPainter.setPen(gridColor);
            prev = rx;
        }
    }

    // Draw axis itselves
    bPainter.setPen(axisColor);
    bPainter.drawLine(xMin, y0, xMax, y0); // Ox
    bPainter.drawLine(x0, yMin, x0, yMax); // Oy
    bPainter.end();
}

void QtRender::drawTS(TimeSeries *ts, int dimension)
{
    // If time series not exists or too short or have invalid dimension params - don't draw it
    std::vector<double> vec;
    if (ts == NULL || ts->dimSize(dimension) < 2 || !ts->getTSByIndex(vec, dimension))
        return;

    std::vector<double>::const_iterator p;

    // Prepare drawing tools
    QPen pointPen(Qt::SolidPattern, 1), tsPen;
    pointPen.setColor(Qt::red);
    tsPen.setColor(Qt::white);

    // Find the most left visible coordinate.
    // Examine time series for value that in drawing area
    int xPos = m_minX;
    int xMax = tx(m_maxX);
    p = vec.begin() + m_horScrollerPos;
    //for (p = vec.begin() + m_horScrollerPos; p+1 != vec.end(); ++xPos, ++p)
      //  if (tx(xPos) >= -3) break;

    // Draw all visible elements
    for (; p+1 != vec.end(); ++xPos, ++p)
    {
        int x1 = tx(xPos),   y1 = ty(*p),
            x2 = tx(xPos+1), y2 = ty(*(p+1));
        m_painter.setPen(tsPen);
        m_painter.drawLine(x1, y1, x2, y2);
        m_painter.setPen(pointPen);
        m_painter.drawPoint(x1, y1);
        // if value is out of drawing area - break cycle
        if (x1 > xMax) break;
    }
    m_painter.drawPoint(tx(xPos), ty(*p));

}

void QtRender::drawLayer(ParseTree::ConstLayer &layer, int treeNo,
                         TreeDrawingSettings dSettings, QColor *specificColor)
{
    std::vector<double> vec;
    if (m_timeSeries == NULL || !m_timeSeries->getTSByIndex(vec, m_tsdim))
        return;

    int xMax = tx(m_maxX);

    int y0 = ty(m_minY + m_yrange/2 - treeNo*20);
    int prevTextX = -50;

    QPen pen1(Qt::SolidLine);
    QPen pen2(Qt::DashDotLine);
    QPen pen3(Qt::DotLine);
    QPen pen4(Qt::SolidLine);
    QColor colorMarked;
    if (specificColor == NULL)
    {
        switch (treeNo)
        {
            case 0:  colorMarked.setRgb(255, 50, 50); break;
            case 1:  colorMarked.setRgb(50, 255, 50); break;
            case 2:  colorMarked.setRgb(50, 50, 255); break;
            default: colorMarked.setRgb(50, 50, 50); break;
        }
        pen4.setColor(QColor(200, 30, 200, 150));
    }
    else
    {
        colorMarked = *specificColor;
        pen4.setColor(QColor(colorMarked));
    }
    pen3.setColor(colorMarked);


    int tsFirstVisible = tx(m_horScrollerPos) - m_xborder;

    ParseTree::ConstLayer::const_iterator iterator = layer.begin();


    // Draw all nodes while thay are visible
    for (int i = 0; iterator != layer.end(); ++i, ++iterator)
    {
        ParseTreeNode *node = *iterator;

        // Get node position
        int start = node->tsBegin;
        int end = node->tsEnd;
        int level = y0 + node->level*20;

        // Excelude potintial problems with indices
        if (start == -1 || end == -1 || start > end || start >= int(vec.size()) || end >= int(vec.size()))
        {
            logg.warning("QtRender: skiped invalid index of TS element in ParsedSequence element number ") << i;
            continue;
        }

        // If starting point of node is invisible than it is don't make
        // any sense drawing this node and all next to this nodes
        int endX = tx(end+1) - tsFirstVisible;
        if (endX < 0)
            continue;
        int startX = tx(start) - tsFirstVisible;
        if (startX > xMax)
            break;


        pen1.setColor(colorMarked);
        pen2.setColor(colorMarked);

        // Draw parse tree nodes
        if (dSettings & DRAW_NODES)
        {
            // Draw line at the bottom
            m_painter.setPen(pen1);
            m_painter.drawLine(startX, level , endX, level);

            // Draw masked lines to proper points of time series
            m_painter.setPen(pen2);
            if (node->level == 0)
            {
                m_painter.drawLine(startX, level, startX, ty(vec[start]));
                m_painter.drawLine(endX,   level, endX,   ty(vec[end+1]));
            }
            else
            {
                m_painter.drawLine(startX, level, startX, level - 10);
                m_painter.drawLine(endX,   level, endX,   level - 10);
            }

            // Draw node name (if its not overlap with previous)
            if (startX - prevTextX > 10)
            {
                m_painter.drawText(startX, level + 10, QSTR(node->name));
                prevTextX = startX;
            }
        }

        // Draw new time series that generated by parse tree layer
        if (//node->level == m_currentLayer &&
            dSettings & DRAW_TIMESERIES)
        {
            m_painter.setPen(pen4);
            m_painter.drawLine(startX, ty(vec[start]), endX, ty(vec[end+1]));
        }
    }
}

void QtRender::drawTree(ParseTree &tree, int treeNo, TreeDrawingSettings dSettings)
{
    // Draw tree = draw all its layers
    int level = tree.minLevel();
    while (level <= tree.maxLevel())
    {
        ParseTree::ConstLayer *layer = tree.getLevel(level);
        drawLayer(*layer, treeNo, dSettings);
        ++level;
    }
    // Highlight roots layer
    if (dSettings & DRAW_ROOTS)
    {
        QColor rootsColor(Qt::green);
        drawLayer(tree.roots(), treeNo, dSettings, &rootsColor);
    }
}

void QtRender::drawParseContext(ParseContext &context)
{
    clear();
    if (context.ts)
    {
        drawTS(m_timeSeries, m_tsdim);
        drawTree(*context.tree, 0, DRAW_ALL);
    }
    swap();
}

void QtRender::drawForecast(Forecast *forecast)
{
    QColor color(100, 100, 150, 255);
    QColor colorAlpha(100, 100, 150, 100);
    QPen dashedPen(Qt::DashDotLine);
    dashedPen.setColor(color);

    int tsFirstVisible = tx(m_horScrollerPos) - m_xborder;

    // Calc possible pattern position
    int xStart = tx(forecast->position()) - tsFirstVisible;
    int xEnd = xStart + tx(forecast->maxDuration());
    if (xEnd < 0 || xStart > tx(m_maxX))
        return;

    // Draw forecast pattern
    int yPos = ty(m_minY + m_yrange/2);
    int yTSPos = yPos;
    if (m_timeSeries && forecast->position() >= 0 &&
        forecast->position() < m_timeSeries->dimSize(m_tsdim))
    {
        std::vector<double> vec;
        if (m_timeSeries->getTSByIndex(vec, m_tsdim))
            yTSPos = ty(vec[forecast->position()]);
    }
    int yForecastStart = ty(forecast->maxValue());

    /*
    QLinearGradient gradient(xStart, yPos, xEnd, yPos);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setColorAt(0, QColor(100, 100, 255, 255));
    gradient.setColorAt(1, QColor(100, 100, 255, 0));
    m_painter.setBrush(gradient);
    m_painter.setPen(QPen(gradient, 0));
    */
    m_painter.setPen(color);
    m_painter.setBrush(color);
    m_painter.drawLine(xStart, yPos, xEnd, yPos);
    if (forecast->pattern() && forecast->pattern()->description())
        m_painter.drawText(xStart, yPos+12, QSTR(forecast->pattern()->description()->name()));
    else
        m_painter.drawText(xStart, yPos+12, QSTR("Unknown"));
    m_painter.setPen(dashedPen);
    m_painter.drawLine(xStart, yPos, xStart, yTSPos);
    m_painter.drawLine(xEnd, yPos, xEnd, yForecastStart);


    // Calc forecast edge points, clockwise, starting with left bottom point
    QPoint p0( xStart + tx(forecast->minDuration()),   yForecastStart );
    QPoint p1( xEnd,   ty(forecast->minValue()) );

    // Draw forecast area
    m_painter.setBrush(colorAlpha);
    m_painter.setPen(Qt::NoPen);
    m_painter.drawRect(QRect(p0, p1));
}
