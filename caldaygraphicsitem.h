// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// caldaygraphicsitem.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#ifndef CALDAYGRAPHICSITEM_H
#define CALDAYGRAPHICSITEM_H

#include "calendarevent.h"

#include <QGraphicsItem>
#include <QDate>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QTextOption>
#include <QLineF>

class EventGraphicsItem;

class CalDayGraphicsItem : public QGraphicsItem
{
public:
    CalDayGraphicsItem(const QDate &date, const QRectF &rect, const QString &holidayName, bool isPublicHoliday, bool isVacation, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void setEvents(const QList<CalendarEvent> &events);
    void updateDay(const QDate &date);

    QDate date() const { return m_date; }

private:
    QDate                       m_date;
    QRectF                      m_rect;
    QRectF                      m_rcHoliday;
    QRectF                      m_rcEvent;
    QString                     m_holidayName;
    QFont                       m_fontDay;
    QFont                       m_fontDate;
    QFont                       m_fontHoliday;
    QFont                       m_fontWeek;
    QBrush                      m_brushBackground;
    QPen                        m_penText;
    QPen                        m_penBorder;
    QPen                        m_penWeek;
    QString                     m_stringDay;
    QString                     m_stringDate;
    QString                     m_stringWeek;
    QTextOption                 m_toHoliday;
    QList<QLineF>               m_linesWeek;
    QList<EventGraphicsItem*>   m_events;
};

#endif // CALDAYGRAPHICSITEM_H
