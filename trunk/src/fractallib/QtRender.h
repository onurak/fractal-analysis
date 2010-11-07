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

#ifndef RENDER_H
#define RENDER_H

#include <QGraphicsScene>
#include <QPainter>
#include "TimeSeries.h"
#include "CoordSystem.h"
#include "Environment.h"
#include "ParseTree.h"
#include "ParseContext.h"
#include "flqt.h"
#include "G/GLogger.h"
#include "G/GDelegate.h"

namespace FL {

/*! \addtogroup FLGUI
  */
/*@{*/

const double AUTO = 0;

/*! \class QtScene
  * \brief Qt Scene used for event handling
  *
  */
class QtScene: public QGraphicsScene {

    Q_OBJECT

public:
    QtScene(QObject *parent = NULL);
signals:
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);
protected:
    void keyPressEvent(QKeyEvent *keyEvent);
    void keyReleaseEvent(QKeyEvent *keyEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
};

/*! \class QtRender
  * \brief Can draw time series and fractal trees
  *
  */
class QtRender
{
public:
    //! Settings of tree drawing
    enum TreeDrawingSettings
    {
        DRAW_NONE       = 0x0,    //!< Draw nothing
        DRAW_NODES      = 0x1,    //!< Draw nodes of tree
        DRAW_TIMESERIES = 0x2,    //!< Draw new time series that created by tree
        DRAW_ROOTS      = 0x4,    //!< Draw roots of tree
        DRAW_ALL        = 0x7     //!< Draw all data
    };
private:
    //! Internal structure for storing tree and its drawing settings
    struct ParseTreeReg
    {
        ParseTreeReg(ParseTree *tree, int level, TreeDrawingSettings settings)
            : tree(tree), level(level), settings(settings) {}
        ParseTree *tree;
        int level;
        TreeDrawingSettings settings;
    };

public:
    //! Default destructor
    QtRender();

    //! Destructor
    ~QtRender();

    //! Draw current time series and trees (if any)
    void draw();

    //! Set current time series to draw
    void setTimeSeries(FL::TimeSeries *ts, int dimension = 0);

    //! Add new tree to draw
    void addTree(ParseTree *tree, int level = 0, TreeDrawingSettings settings = DRAW_ALL);

    //! Remove registered tree
    void remTree(ParseTree *tree);

    //! Clear registered trees
    void clearTrees();

    //! Get current drawing time series
    TimeSeries* timeSeries()
    {
        return m_timeSeries;
    }

    //! Get used time seris dimension
    int tsDim()
    {
        return m_tsdim;
    }

    //! Set used time seris dimension
    void setTsDim(int value)
    {
        m_tsdim = value;
    }

    //! Resize drawing area
    void resize(int width, int height);

    //! Make drawing area such to fit the current time series
    /*! \param timeRange Range of time to get. If AUTO than it will choosed automaticaly for best fit
      * \param minY Minimal value
      * \param maxY Maximal value
      */
    void fitTimeSeries(double timeRange = AUTO, double minY = -10e10, double maxY = +10e10);

    //! Draw analysis context data
    /*! This function can be used to visualise parsing process.
      * \param context Current ParseContext
      */
    void drawParseContext(ParseContext &context);

public:
    //! Getter for Qt scene object
    QtScene* scene() { return m_scene; }

    //! Getter for coordinate system object
//    CoordSystem& cs() { return *m_cs; }

    //! Set axis minimum/maximum values
    void setAxis(double minX, double maxX, double minY, double maxY)
    {
        if (maxX <= minX || maxY <= minY)
        {
            GError(GCritical, "QtRender", 0, "Invalid axis parameters");
            return;
        }
        m_minX = minX;
        m_maxX = maxX;
        m_minY = minY;
        m_maxY = maxY;
        m_xrange = maxX - minX;
        m_yrange = maxY - minY;
        m_horScrollerPos = 0;
        updateScroller();
        bufferCoordSystem();
    }

    //! Get minimum Ox scrolling value
    int minimumHorizontalScroll()
    {
        return 0;
    }

    //! Get maximum Ox scrolling value
    int maximumHorizontalScroll()
    {
        return m_timeSeries ? m_timeSeries->dimSize(0) - m_tsFitCountX : minimumHorizontalScroll();
    }

    //! Get Ox scrolling position
    int horizontalScrollerPos()
    {
        return m_horScrollerPos;
    }

    //! Set Ox scrolling position
    void setHorizontalScrollerPos(int value)
    {
        m_horScrollerPos = std::min(maximumHorizontalScroll(), std::max(minimumHorizontalScroll(), value));
    }

    //! Check if horizontal scrolling is posible to fit time series
    bool isNeedHorizontalScroll()
    {
        // todo: its really wrong
        return maximumHorizontalScroll() != minimumHorizontalScroll();
    }

    //! Zoom visible area to specified scalar
    void zoom(double factor);

    //! Set drawing settings for all trees
    void setTreeDrawingSettings(TreeDrawingSettings settings);
protected:
    //! Recreate buffer where coordinate system if drawn
    void bufferCoordSystem();

    //! Translate X coordinate
    inline int tx(double x)
    {
        return m_xborder + (x - m_minX) / m_xrange * m_width;
    }

    //! Translate Y coordinate
    inline int ty(double y)
    {
        return m_yborder + (m_height - (y - m_minY) / m_yrange * m_height);
    }

    //! Clear screen buffers
    void clear();

    //! Drawout buffers to scene
    void swap();

    //! Draw time series
    void drawTS(TimeSeries *ts, int dimension);

    //! Draw parse tree layer
    void drawLayer(ParseTree::ConstLayer &layer, int treeNo,
                   TreeDrawingSettings dSettings, QColor *specificColor = NULL);

    //! Draw whole parse tree
    void drawTree(ParseTree &tree, int treeNo, TreeDrawingSettings dSettings);

    //! Update scrolling info
    void updateScroller();
private:
    //! Qt scene
    QtScene *m_scene;

    //! Main drawing buffer
    QPixmap *m_framebuffer;

    //! Buffer with renddeed coordinate system
    QPixmap *m_csbuffer;

    //! QPainter object
    QPainter m_painter;

    //! Coordinate system object
//    CoordSystem *m_cs;

    //! Current drawing time series
    TimeSeries *m_timeSeries;

    //! Used time seris dimension
    int m_tsdim;

    //! Registered trees
    std::vector<ParseTreeReg> m_trees;

    double m_minX, m_maxX, m_minY, m_maxY, m_xrange, m_yrange, m_width, m_height;
    double m_xborder, m_yborder;

    //! Count of points of time series that can fit in width
    int m_tsFitCountX;

    //! Position of horizontal scroller (first visible value of time series)
    int m_horScrollerPos;

    //! Current operatable layer of tree
    int m_currentLayer;
};

/*@}*/

} // namespace

#endif // RENDER_H
