// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// schoolvacations.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-11  tt  Initial version created
// ***************************************************************************
#include "schoolvacations.h"
#include <QJsonArray>
#include <QJsonObject>

SchoolVacations::SchoolVacations(const QJsonArray &dates)
    : m_valid(false)
{
    bool allValid = true;
    for (const auto &i : dates) {
        QJsonObject o = i.toObject();
        m_vacations.append(Vacation(o["start"].toString(), o["end"].toString(), o["name"].toString()));
        allValid &= m_vacations.last().isValid();
    }
    m_valid = allValid;
}

QString SchoolVacations::checkVacation(const QDate &date) const
{
    QString ret;
    for (const auto &v : m_vacations) {
        ret = v.checkVacation(date);
        if (!ret.isEmpty()) {
            break;
        }
    }
    return ret;
}

bool SchoolVacations::allValid() const
{
    return m_valid;
}

int SchoolVacations::size() const
{
    return m_vacations.size();
}

Vacation SchoolVacations::operator[](int index) const
{
    if (index < m_vacations.size()) {
        return m_vacations[index];
    }
    return Vacation();
}

SchoolVacations &SchoolVacations::operator+=(const SchoolVacations &other)
{
    m_vacations += other.m_vacations;
    m_valid &= other.m_valid;
    return *this;
}



Vacation::Vacation()
{

}

Vacation::Vacation(const QString &start, const QString &end, const QString &name)
    : m_start(QDate::fromString(start, Qt::ISODate))
    , m_end(QDate::fromString(end, Qt::ISODate))
    , m_name(name)
{
}

QString Vacation::checkVacation(const QDate &date) const
{
    QString ret;

    if (isValid()) {
        if ((m_start <= date) && (date <= m_end)) {
            ret = m_name;
        }
    }
    return ret;
}

bool Vacation::isValid() const
{
    return(m_start.isValid() && m_end.isValid() && !m_name.isEmpty());
}

QString Vacation::toString() const
{
    QString ret = "invalid";
    if (isValid()) {
        ret = QString("%1 ... %2: %3")
                  .arg(m_start.toString("yyyy-MM-dd"))
                  .arg(m_end.toString("yyyy-MM-dd"))
                  .arg(m_name);
    }
    return ret;
}


