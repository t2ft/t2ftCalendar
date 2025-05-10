// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// calendarday.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#include "calendarday.h"
#include <QPainter>


CalendarDay::CalendarDay(const QDate &date, const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsItem(parent),
    m_date(date),
    m_rect(rect)
{

}

QRectF CalendarDay::boundingRect() const
{
    return m_rect;
}

void CalendarDay::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::lightGray);
    painter->drawRect(m_rect);

    QFont font;
    font.setFamily("Calibri");
    font.setPointSizeF(9.2);
    font.setWeight(QFont::Normal);
    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(m_rect.adjusted(1,0,0,0), Qt::AlignLeft | Qt::AlignVCenter, QLocale::system().toString(m_date, "ddd").left(2));
    font.setPointSizeF(6);
    painter->setFont(font);
    font.setWeight(QFont::Light);
    painter->drawText(m_rect.adjusted(0,0,-1,1), Qt::AlignRight | Qt::AlignBottom, QLocale::system().toString(m_date, "dd.MMM"));
}
