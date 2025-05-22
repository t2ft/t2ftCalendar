// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// calendarevent.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-22  tt  Initial version created
// ***************************************************************************
#include "calendarevent.h"

CalendarEvent::CalendarEvent()
{

}

CalendarEvent::CalendarEvent(const QDate &date, const QString &summary, const QString &color)
    : m_date(date)
    , m_color(color)
{
    m_summary = "   " + QLocale::system().toString(date, "dd.MMMM") + "\n" + summary;
}

bool CalendarEvent::operator<(const CalendarEvent &other) const
{
    return (m_date < other.m_date);
}

bool CalendarEvent::operator==(const CalendarEvent &other) const
{
    return ((m_date == other.m_date) && (m_summary==other.m_summary));
}
