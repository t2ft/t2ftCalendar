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
    QJsonObject::const_iterator i = dates.constBegin();
    while (i!=dates.constEnd()) {
        QString name = i.key();
        QString date = i.value().toObject()["datum"].toString();
        QString hint = i.value().toObject()["hinweis"].toString();
        m_holidays.append(Holiday(date, name, hint));
        ++i;
    }
    m_valid = allValid;
}

QString PublicHolidays::isHoliday(const QDate &date) const
{
    QString ret;
    for (const auto &h : m_holidays) {
        ret = h.isHoliday(date);
        if (!ret.isEmpty()) {
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
{
}

Holiday::Holiday(const QString &date, const QString &name, const QString &hint)
    : m_date(QDate::fromString(date, Qt::ISODate))
    , m_name(name)
    , m_hint(hint)
{

}

QString Holiday::isHoliday(const QDate &date) const
{
    QString ret;
    if (date==m_date) {
        ret = m_name;
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



