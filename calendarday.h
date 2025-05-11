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
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QTextOption>

class CalendarDay : public QGraphicsItem
{
public:
    CalendarDay(const QDate &date, const QRectF &rect, const QString &holidayName, bool isPublicHoliday, bool isVacation, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void updateDay();

private:
    QDate       m_date;
    QRectF      m_rect;
    QRectF      m_rcHoliday;
    QString     m_holidayName;
    QFont       m_fontDay;
    QFont       m_fontDate;
    QFont       m_fontHoliday;
    QBrush      m_brushBackground;
    QPen        m_penText;
    QPen        m_penBorder;
    QString     m_stringDay;
    QString     m_stringDate;
    QTextOption m_toHoliday;
};

#endif // CALENDARDAY_H
