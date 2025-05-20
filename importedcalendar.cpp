// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// importedcalendar.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-20  tt  Initial version created
// ***************************************************************************
#include "importedcalendar.h"

#include <QTimer>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QTimeZone>
#include <libical/ical.h>


ImportedCalendar::ImportedCalendar(QObject *parent)
    : QObject(parent)
    , m_year(0)
    , m_updatePeriod(0)
    , m_updateTimer(nullptr)
    , m_accessManager(nullptr)
{
    qDebug() << "+++ ImportedCalendar::ImportedCalendar()";
    qDebug() << "--- ImportedCalendar::ImportedCalendar()";
}

ImportedCalendar::ImportedCalendar(int year, const QString &link, qint64 updateSeconds, QObject *parent)
    : QObject(parent)
    , m_year(year)
    , m_link(link)
    , m_updatePeriod(updateSeconds)
    , m_updateTimer(nullptr)
    , m_accessManager(nullptr)
{
    qDebug() << "+++ ImportedCalendar::ImportedCalendar(link =" << link << "updateSeconds =" << updateSeconds << ")";
    update();
    qDebug() << "--- ImportedCalendar::ImportedCalendar()";
}

ImportedCalendar::~ImportedCalendar()
{
    qDebug() << "+++ ImportedCalendar::~ImportedCalendar()";
    killUpdateTimer();
    clearEntries();
    qDebug() << "--- ImportedCalendar::~ImportedCalendar()";
}

void ImportedCalendar::setLink(const QString &link)
{
    qDebug() << "+++ ImportedCalendar::setLink(line =" << link << ")";
    if (link.isEmpty()) {
        killUpdateTimer();
    }
    m_link = link;
    qDebug() << "--- ImportedCalendar::setLink()";
}

void ImportedCalendar::setUpdatePeriod(qint64 seconds)
{
    qDebug() << "+++ ImportedCalendar::setUpdatePeriod(seconds =" << seconds << ")";
    bool restart = false;
    if (m_updateTimer) {
        killUpdateTimer();
        restart = true;
    }
    m_updatePeriod = seconds;
    if (restart) {
        startUpdateTimer();
    }
    qDebug() << "--- ImportedCalendar::setUpdatePeriod()";
}

void ImportedCalendar::onTimer()
{
    qDebug() << "+++ ImportedCalendar::onTimer()";
    update();
    qDebug() << "--- ImportedCalendar::onTimer()";
}


void ImportedCalendar::update()
{
    qDebug() << "+++ ImportedCalendar::update()";
    if (!m_accessManager) {
        m_accessManager = new QNetworkAccessManager(this);
        connect(m_accessManager, &QNetworkAccessManager::finished, this, &ImportedCalendar::onReplyFinished);
    }
    QString url(m_link);
    m_accessManager->get(QNetworkRequest(QUrl(url)));
    qDebug() << "--- ImportedCalendar::update()";
}


void ImportedCalendar::onReplyFinished(QNetworkReply *reply)
{
    qDebug() << "+++ ImportedCalendar::onReplyFinished()";
    if (reply) {
        QByteArray data = reply->readAll();
        if (!data.isEmpty()) {
            clearEntries();
            m_dates = extractAllDatesForYear(data, m_year);
            emit newEntries();
        }
    }
    if (m_updateTimer==nullptr) {
        // start update timer, if von already running
        startUpdateTimer();
    }
    qDebug() << "--- ImportedCalendar::onReplyFinished()";
}


void ImportedCalendar::startUpdateTimer()
{
    qDebug() << "+++ ImportedCalendar::startUpdateTimer()";
    killUpdateTimer();
    if (m_updatePeriod>0) {
        m_updateTimer = new QTimer(this);
        connect(m_updateTimer, &QTimer::timeout, this, &ImportedCalendar::update);
        m_updateTimer->start(m_updatePeriod*1000);
    }
    qDebug() << "--- ImportedCalendar::startUpdateTimer()";
}

void ImportedCalendar::clearEntries()
{
    qDebug() << "+++ ImportedCalendar::clearEntries()";
    m_dates.clear();
    m_color.clear();
    m_name.clear();
    qDebug() << "--- ImportedCalendar::clearEntries()";
}


void ImportedCalendar::killUpdateTimer()
{
    qDebug() << "+++ ImportedCalendar::killUpdateTimer()";
    if (m_updateTimer) {
        delete m_updateTimer;
        m_updateTimer = nullptr;
    }
    qDebug() << "--- ImportedCalendar::killUpdateTimer()";
}


QList<QDate> ImportedCalendar::extractAllDatesForYear(const QByteArray& icsContent, int year)
{
    QList<QDate> result;

    icalcomponent* root = icalparser_parse_string(icsContent.constData());
    if (!root) {
        qWarning() << "ICS parsing failed.";
        return result;
    }

    // VCALENDAR-Level Properties durchgehen
    for (icalproperty* prop = icalcomponent_get_first_property(root, ICAL_ANY_PROPERTY);
         prop != nullptr;
         prop = icalcomponent_get_next_property(root, ICAL_ANY_PROPERTY)) {

        icalproperty_kind kind = icalproperty_isa(prop);

        if (kind == ICAL_X_PROPERTY) {
            const char* propName = icalproperty_get_x_name(prop);
            const char* value = icalproperty_get_value_as_string(prop);

            if (qstricmp(propName, "X-WR-CALNAME") == 0) {
                m_name = QString::fromUtf8(value);
                qDebug() << "Kalender-Name:" << m_name;
            } else if (qstricmp(propName, "X-APPLE-CALENDAR-COLOR") == 0) {
                m_color = QString::fromUtf8(value);
                qDebug() << "Kalender-Farbe:" << m_color;
            }
        }
    }
    if (m_color.isEmpty()) {
        m_color = "#FF0000";
    }

    // Zeitraum für das Jahr
    QDateTime startOfYear(QDate(year, 1, 1), QTime(0, 0), QTimeZone::UTC);
    QDateTime endOfYear(QDate(year, 12, 31).addDays(1), QTime(0, 0), QTimeZone::UTC);  // exklusiv

    icaltimezone* berlinTz = icaltimezone_get_builtin_timezone("Europe/Berlin");

    for (icalcomponent* comp = icalcomponent_get_first_component(root, ICAL_VEVENT_COMPONENT);
         comp != nullptr;
         comp = icalcomponent_get_next_component(root, ICAL_VEVENT_COMPONENT)) {

        icalproperty* dtstartProp = icalcomponent_get_first_property(comp, ICAL_DTSTART_PROPERTY);
        if (!dtstartProp) continue;

        icaltimetype dtstart = icalproperty_get_dtstart(dtstartProp);
        icaltimetype dtstartUtc = icaltime_convert_to_zone(dtstart, berlinTz);

        bool hasRRule = (icalcomponent_get_first_property(comp, ICAL_RRULE_PROPERTY) != nullptr);
        bool hasRDate = (icalcomponent_get_first_property(comp, ICAL_RDATE_PROPERTY) != nullptr);

        if (hasRRule) {
            // Wiederholungen mit RRULE
            icalproperty* rruleProp = icalcomponent_get_first_property(comp, ICAL_RRULE_PROPERTY);
            struct icalrecurrencetype recur = icalproperty_get_rrule(rruleProp);

            icalrecur_iterator* rit = icalrecur_iterator_new(recur, dtstart);
            if (!rit) continue;

            while (true) {
                icaltimetype next = icalrecur_iterator_next(rit);
                if (icaltime_is_null_time(next)) break;

                icaltimetype nextInBerlin = icaltime_convert_to_zone(next, berlinTz);
                QDate date(nextInBerlin.year, nextInBerlin.month, nextInBerlin.day);
                QDateTime dt(date, QTime(0, 0), QTimeZone::UTC);

                if (dt >= startOfYear && dt < endOfYear) {
                    // EXDATE prüfen
                    bool excluded = false;
                    for (icalproperty* exdateProp = icalcomponent_get_first_property(comp, ICAL_EXDATE_PROPERTY);
                         exdateProp != nullptr;
                         exdateProp = icalcomponent_get_next_property(comp, ICAL_EXDATE_PROPERTY)) {

                        icaltimetype ex = icalproperty_get_exdate(exdateProp);
                        ex = icaltime_convert_to_zone(ex, berlinTz);
                        QDate exdate(ex.year, ex.month, ex.day);
                        if (date == exdate) {
                            excluded = true;
                            break;
                        }
                    }

                    if (!excluded)
                        result.append(date);
                }

                // Frühzeitiger Abbruch
                QDateTime currentDate(QDate(next.year, next.month, next.day), QTime(0, 0), QTimeZone::UTC);
                if (currentDate >= endOfYear)
                    break;
            }

            icalrecur_iterator_free(rit);
        }

        // Einzeltermine / RDATEs
        if (!hasRRule || hasRDate) {
            // Add DTSTART falls im Jahr und nicht durch EXDATE entfernt
            QDate date(dtstartUtc.year, dtstartUtc.month, dtstartUtc.day);
            if (date.year() == year) {
                bool excluded = false;
                for (icalproperty* exdateProp = icalcomponent_get_first_property(comp, ICAL_EXDATE_PROPERTY);
                     exdateProp != nullptr;
                     exdateProp = icalcomponent_get_next_property(comp, ICAL_EXDATE_PROPERTY)) {

                    icaltimetype ex = icalproperty_get_exdate(exdateProp);
                    ex = icaltime_convert_to_zone(ex, berlinTz);
                    QDate exdate(ex.year, ex.month, ex.day);
                    if (date == exdate) {
                        excluded = true;
                        break;
                    }
                }
                if (!excluded)
                    result.append(date);
            }

            // RDATE
            for (icalproperty* rdateProp = icalcomponent_get_first_property(comp, ICAL_RDATE_PROPERTY);
                 rdateProp != nullptr;
                 rdateProp = icalcomponent_get_next_property(comp, ICAL_RDATE_PROPERTY)) {

                struct icaldatetimeperiodtype dtp = icalproperty_get_rdate(rdateProp);
                icaltimetype rdate = icaltime_convert_to_zone(dtp.time, berlinTz);
                QDate rdateQ(rdate.year, rdate.month, rdate.day);

                if (rdateQ.year() == year)
                    result.append(rdateQ);
            }
        }
    }

    icalcomponent_free(root);
    return result;
}
