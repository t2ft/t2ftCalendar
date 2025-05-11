// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// publicholydays.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-11  tt  Initial version created
// ***************************************************************************
#include "publicholydays.h"
#include <QJsonObject>

PublicHolidays::PublicHolidays(const QJsonObject &dates)
    : m_valid(false)
{
    bool allValid = true;
    QDate date;
    QJsonObject::const_iterator i = dates.constBegin();
    int year = QDate::currentDate().year();
    while (i!=dates.constEnd()) {
        QString name = i.key();
        QString dateString = i.value().toObject()["datum"].toString();
        date = QDate::fromString(dateString, Qt::ISODate);
        QString hint = i.value().toObject()["hinweis"].toString();
        if (name == "Neujahrstag") {
            m_holidays.append(Holiday(date, "Neujahr", hint, true));
        } else if (name == "Ostermontag") {
            m_holidays.append(Holiday(date, "Oster- montag", hint, true));
            m_holidays.append(Holiday(date.addDays(-1), "Oster- sonntag", "", false));
            m_holidays.append(Holiday(date.addDays(-49), "Rosen- montag", "", false));
        } else if (name == "Pfingstmontag") {
            m_holidays.append(Holiday(date, "Pfingst- montag", "", true));
            m_holidays.append(Holiday(date.addDays(-1), "Pfingst- sonntag", "", false));
        } else if (name == "1. Weihnachtstag") {
            m_holidays.append(Holiday(date, "1. Weih- nachtstag", hint, true));
            m_holidays.append(Holiday(date.addDays(-1), "Heilig Abend", "", false));
            m_holidays.append(Holiday(date.addDays(-date.dayOfWeek()-21), "1. Advent", "", false));
        } else if (name == "2. Weihnachtstag") {
            m_holidays.append(Holiday(date, "2. Weih- nachtstag", hint, true));
        } else if (name == "Christi Himmelfahrt") {
            m_holidays.append(Holiday(date, "Christi Himmel- fahrt", hint, true));
        } else if (name == "Fronleichnam") {
            m_holidays.append(Holiday(date, "Fron- leichnam", hint, true));
        } else if (name == "Augsburger Friedensfest") {
            m_holidays.append(Holiday(date, "Augs- burger Friedensfest", hint, false));
        } else if (name == "Mariä Himmelfahrt") {
            m_holidays.append(Holiday(date, "Mariä Himmel- fahrt", hint, true));
        } else if (name == "Allerheiligen") {
            m_holidays.append(Holiday(date, "Aller- heiligen", hint, true));
        } else {
            m_holidays.append(Holiday(date, name, hint, true));
        }
        ++i;
    }
    // add some more dates wich are not public holidays, but interesting
    date = QDate(year, 3, 31);
    m_holidays.append(Holiday(date.addDays(date.dayOfWeek()==7 ? 0 : -date.dayOfWeek()), "Beginn der Sommer- zeit", "", false));
    date = QDate(year, 10, 31);
    m_holidays.append(Holiday(date.addDays(date.dayOfWeek()==7 ? 0 : -date.dayOfWeek()), "Ende der Sommer- zeit", "", false));
    date = QDate(year,5,1);
    m_holidays.append(Holiday(date.addDays(7+(7-date.dayOfWeek())), "Muttertag", "", false));
    m_holidays.append(Holiday(QDate(year,10,31), "Reforma- tionstag", "", false));
    m_holidays.append(Holiday(QDate(year,12,06), "Nikolaus", "", false));
    m_holidays.append(Holiday(QDate(year,12,31), "Silvester", "", false));

    m_valid = allValid;
}

bool PublicHolidays::isHoliday(const QDate &date, QString &name, bool &isPublic) const
{
    bool ret;
    for (const auto &h : m_holidays) {
        ret = h.isHoliday(date, name, isPublic);
        if (ret) {
            break;
        }
    }
    return ret;
}

bool PublicHolidays::allValid() const
{
    return m_valid;
}

int PublicHolidays::size() const
{
    return m_holidays.size();
}

Holiday PublicHolidays::operator[](int index) const
{
    if (index < m_holidays.size()) {
        return m_holidays[index];
    }
    return Holiday();
}

Holiday::Holiday()
    : m_isPublic(false)
{
}

Holiday::Holiday(const QDate &date, const QString &name, const QString &hint, bool isPublic)
    : m_date(date)
    , m_name(name)
    , m_hint(hint)
    , m_isPublic(isPublic)
{

}

bool Holiday::isHoliday(const QDate &dat, QString &name, bool &isPublic) const
{
    bool ret = false;
    if (dat==m_date) {
        name = m_name;
        isPublic = m_isPublic;
        ret = true;
    }
    return ret;
}

bool Holiday::isValid() const
{
    return (m_date.isValid() && !m_name.isEmpty());
}

QString Holiday::toString() const
{
    QString ret = "invalid";
    if (isValid()) {
        ret = QString("%1: %2%3")
                  .arg(m_date.toString("yyyy-MM-dd"))
                  .arg(m_name)
                  .arg(m_hint.isEmpty() ? "" : " ("+m_hint+")");
    }
    return ret;
}



