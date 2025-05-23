// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// importedcalendar.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-20  tt  Initial version created
// ***************************************************************************
#ifndef IMPORTEDCALENDAR_H
#define IMPORTEDCALENDAR_H

#include "calendarevent.h"
#include <QObject>
#include <QDateTime>

class QTimer;
class QNetworkAccessManager;
class QNetworkReply;

class ImportedCalendar : public QObject
{
    Q_OBJECT

public:
    explicit ImportedCalendar(QObject *parent = nullptr);
    ImportedCalendar(int year, const QString &link, qint64 updateSeconds, const QString &defaultColor, const QString &defaultName = QString(), const QString &forcedColor = QString(), QObject *parent = nullptr);
    ~ImportedCalendar();

    void setLink(const QString &link);
    void setUpdatePeriod(qint64 seconds);
    void update();

    bool isValid() const;

    int size() const { return m_events.size(); }
    CalendarEvent entry(int index) const {return m_events.value(index); }
    CalendarEvent entry(const QDate &date) const;
    bool hasEntry(const QDate &date) const;
    QString color() const { return m_color; }
    QString name() const { return m_name; }

signals:
    void newEntries();

protected slots:
    void onTimer();
    void onReplyFinished(QNetworkReply *reply);

protected:
    void killUpdateTimer();
    void startUpdateTimer();
    void clearEntries();
    QList<CalendarEvent> extractAllDatesForYear(const QByteArray &icsContent, int year);

    int                     m_year;
    QString                 m_link;
    qint64                  m_updatePeriod;
    QList<CalendarEvent>    m_events;
    QTimer                  *m_updateTimer;
    QNetworkAccessManager   *m_accessManager;
    QString                 m_color;
    QString                 m_defaultColor;
    QString                 m_forcedColor;
    QString                 m_name;
    QString                 m_defaultName;
};

#endif // IMPORTEDCALENDAR_H
