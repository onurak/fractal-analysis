#ifndef QPARSETREERENDER_H
#define QPARSETREERENDER_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QLinkedList>
#include <QGraphicsSceneMouseEvent>
#include "../fractallib/TimeSeries.h"
#include "../fractallib/parsers/SinglePass.h"

class QParseTreeScene: public QGraphicsScene
{
    Q_OBJECT
public:
    QParseTreeScene(): QGraphicsScene(0)
    {

    }

    ~QParseTreeScene()
    {

    }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
        emit mouseMove(event);
        event->accept();
    }

signals:
    void mouseMove(QGraphicsSceneMouseEvent *event);
};



class QParseTreeRender
{
public:
    QParseTreeRender();
    virtual ~QParseTreeRender();

    void setTimeSeries(FL::TimeSeries *ts);

    QParseTreeScene* scene() { return m_scene; }

protected:
    void draw();
    void drawCoordinateSystem();

    template <typename T>
    void clearCollection(T &t)
    {
        while (!t.isEmpty())
            delete t.takeFirst();
    }

private:
    QParseTreeScene *m_scene;
    FL::TimeSeries *m_ts;
    QLinkedList<QGraphicsItem*> m_tsItems;
    QLinkedList<QGraphicsItem*> m_coordSystemItems;
};

#endif // QPARSETREERENDER_H
