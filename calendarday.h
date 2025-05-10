// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// calendarday.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#ifndef CALENDARDAY_H
#define CALENDARDAY_H

#include <QGraphicsItem>
#include <QDate>

class CalendarDay : public QGraphicsItem
{
public:
    CalendarDay(const QDate &date, const QRectF &rect, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QDate       m_date;
    QRectF      m_rect;
};

#endif // CALENDARDAY_H
