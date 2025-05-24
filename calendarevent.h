// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// calendarevent.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-22  tt  Initial version created
// ***************************************************************************
#ifndef CALENDAREVENT_H
#define CALENDAREVENT_H

#include <QDate>
#include <QString>

class CalendarEvent
{
public:
    CalendarEvent();
    CalendarEvent(const QDate &date, const QString &summary, const QString &color);

    QDate date() const  { return m_date; }
    QString summary() const  { return m_summary; }
    QString color() const { return m_color; }

    bool isNull() const { return (m_date.isNull() && m_summary.isNull()); }
    bool isValid() const { return (m_date.isValid() && !m_summary.isEmpty()); }

    bool operator<(const CalendarEvent &other) const;
    bool operator==(const CalendarEvent &other) const;

protected:
    QDate       m_date;
    QString     m_summary;
    QString     m_color;
};

inline size_t qHash(const CalendarEvent &key, size_t seed){
    return qHash(key.date(), seed) ^ qHash(key.summary(), seed+1);
}

#endif // CALENDAREVENT_H
