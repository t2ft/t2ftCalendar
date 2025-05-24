// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// eventgraphicsitem.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-22  tt  Initial version created
// ***************************************************************************
#ifndef EVENTGRAPHICSITEM_H
#define EVENTGRAPHICSITEM_H

#include <QGraphicsObject>


class EventGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:
    EventGraphicsItem(const QString &summary, const QColor &color, const QRectF &rect, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QString     m_summary;
    QColor      m_color;
    QRectF      m_rect;
};

#endif // EVENTGRAPHICSITEM_H
