// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// schoolvacations.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-11  tt  Initial version created
// ***************************************************************************
#ifndef SCHOOLVACATIONS_H
#define SCHOOLVACATIONS_H

#include <QList>
#include <QDate>

class QJsonArray;

class Vacation
{
public:
    Vacation();
    Vacation(const QString &start, const QString &end, const QString &name);

    QString checkVacation(const QDate &date) const;
    bool isValid() const;
    QString toString() const;

private:
    QDate       m_start;
    QDate       m_end;
    QString     m_name;
};


class SchoolVacations
{
public:
    SchoolVacations(const QJsonArray &dates);

    QString checkVacation(const QDate &date) const;
    bool allValid() const;
    int size() const;
    Vacation operator[](int index) const;
    SchoolVacations &operator+=(const SchoolVacations &other);

private:
    bool                m_valid;
    QList<Vacation>     m_vacations;
};

#endif // SCHOOLVACATIONS_H
