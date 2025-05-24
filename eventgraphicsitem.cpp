// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// eventgraphicsitem.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-22  tt  Initial version created
// ***************************************************************************
#include "eventgraphicsitem.h"

#include <QPainter>
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>


EventGraphicsItem::EventGraphicsItem(const QString &summary, const QColor &color, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_summary(summary)
    , m_color(color)
    , m_rect(rect)
{
    setAcceptHoverEvents(true);
}

QRectF EventGraphicsItem::boundingRect() const
{
    return m_rect;
}

void EventGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->fillRect(m_rect, m_color);
}

void EventGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << "EventGraphicsItem::hoverEnterEvent(" << m_summary << ")";
    QToolTip::showText(event->screenPos(), m_summary);
}

void EventGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)

    QToolTip::hideText();
    qDebug() << "EventGraphicsItem::hoverLeaveEvent()";
}
