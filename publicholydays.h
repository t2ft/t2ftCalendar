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
    Holiday(const QDate &date, const QString &name, const QString &hint, bool isPublic);

    bool isHoliday(const QDate &dat, QString &name, bool &isPublic) const;
    bool isValid() const;
    QString toString() const;

private:
    QDate       m_date;
    QString     m_name;
    QString     m_hint;
    bool        m_isPublic;
};


class PublicHolidays
{
public:
    PublicHolidays(const QJsonObject &dates, const QDate &currentDate);

    bool isHoliday(const QDate &date, QString &name, bool &isPublic) const;
    bool allValid() const;
    int size() const;
    Holiday operator[](int index) const;

private:
    bool                m_valid;
    QList<Holiday>      m_holidays;

};

#endif // PUBLICHOLIDAYS_H
