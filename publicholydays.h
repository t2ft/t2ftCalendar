// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// publicholydays.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-11  tt  Initial version created
// ***************************************************************************
#ifndef PUBLICHOLIDAYS_H
#define PUBLICHOLIDAYS_H

#include <QList>
#include <QDate>

class QJsonObject;

class Holiday
{
public:
    Holiday();
    Holiday(const QString &date, const QString &name, const QString &hint);

    QString isHoliday(const QDate &date) const;
    bool isValid() const;
    QString toString() const;

private:
    QDate       m_date;
    QString     m_name;
    QString     m_hint;
};


class PublicHolidays
{
public:
    PublicHolidays(const QJsonObject &dates);

    QString isHoliday(const QDate &date) const;
    bool allValid() const;
    int size() const;
    Holiday operator[](int index) const;

private:
    bool                m_valid;
    QList<Holiday>      m_holidays;

};

#endif // PUBLICHOLIDAYS_H
