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
//    qDebug() << "+++ ImportedCalendar::startUpdateTimer()";
    killUpdateTimer();
    if (m_updatePeriod>0) {
        m_updateTimer = new QTimer(this);
        connect(m_updateTimer, &QTimer::timeout, this, &ImportedCalendar::update);
        m_updateTimer->start(m_updatePeriod*1000);
    }
//    qDebug() << "--- ImportedCalendar::startUpdateTimer()";
}

void ImportedCalendar::clearEntries()
{
//    qDebug() << "+++ ImportedCalendar::clearEntries()";
    m_dates.clear();
    m_color.clear();
    m_name.clear();
//    qDebug() << "--- ImportedCalendar::clearEntries()";
}


void ImportedCalendar::killUpdateTimer()
{
//    qDebug() << "+++ ImportedCalendar::killUpdateTimer()";
    if (m_updateTimer) {
        delete m_updateTimer;
        m_updateTimer = nullptr;
    }
//    qDebug() << "--- ImportedCalendar::killUpdateTimer()";
}


QList<QDate> ImportedCalendar::extractAllDatesForYear(const QByteArray &icsContent, int year)
{
    QList<QDate> resultDates;
    QSet<QDate> dateSet;

    icalcomponent* calendar = icalparser_parse_string(icsContent.constData());
    if (!calendar) {
        qWarning() << "ICS parsing failed.";
        return resultDates;
    }

    // VCALENDAR-Level Properties durchgehen
    for (icalproperty* prop = icalcomponent_get_first_property(calendar, ICAL_ANY_PROPERTY);
         prop != nullptr;
         prop = icalcomponent_get_next_property(calendar, ICAL_ANY_PROPERTY)) {

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

    // Durchlaufe alle VEVENT-Komponenten
    for (icalcomponent* event = icalcomponent_get_first_component(calendar, ICAL_VEVENT_COMPONENT);
         event != nullptr;
         event = icalcomponent_get_next_component(calendar, ICAL_VEVENT_COMPONENT))
    {
        icaltimetype dtstart = icalcomponent_get_dtstart(event);
        if (icaltime_is_null_time(dtstart))
            continue;

        icaltimetype dtend = icalcomponent_get_dtend(event); // kann null sein

        // Berechne Dauer
        icaldurationtype dur = icalcomponent_get_duration(event);
        int durationDays = std::max(1, static_cast<int>((icaldurationtype_as_int(dur) + 24.*3600-1)/ (24.*3600))); // always round up
        // Wiederholungen verarbeiten (RRULE, RDATE, EXDATE)
        icalrecur_iterator* recurIter = nullptr;
        icalproperty* rruleProp = icalcomponent_get_first_property(event, ICAL_RRULE_PROPERTY);

#ifdef QT_DEBUG_REPEATS
        if ((dtstart.year==year) && (durationDays>1)) {
            qDebug().nospace() << "calendar " << m_name << ": dtstart = " << icaltime_as_ical_string_r(dtstart) << ", dtend = " << icaltime_as_ical_string(dtend);
            qDebug() << "  -> \"" << icalcomponent_get_summary(event) << "\"";
            qDebug() << "  -> durationDays =" << durationDays << "rruleProp =" << rruleProp;
        }
#endif

        // Liste mit EXDATEs vorbereiten
        QSet<QDate> excludedDates;
        for (icalproperty* ex = icalcomponent_get_first_property(event, ICAL_EXDATE_PROPERTY);
             ex != nullptr;
             ex = icalcomponent_get_next_property(event, ICAL_EXDATE_PROPERTY))
        {
            icaltimetype exTime = icalproperty_get_exdate(ex);
            QDate exDate(exTime.year, exTime.month, exTime.day);
            excludedDates.insert(exDate);
        }

        if (rruleProp) {
            icalrecurrencetype rrule = icalproperty_get_rrule(rruleProp);
            recurIter = icalrecur_iterator_new(rrule, dtstart);

            if (!recurIter) continue;

            icaltimetype occ;
            while (!(icaltime_is_null_time(occ = icalrecur_iterator_next(recurIter)))) {
                for (int i = 0; i < durationDays; ++i) {
                    QDate occDate = QDate(occ.year, occ.month, occ.day).addDays(i);
                    if (occDate.year() == year && !excludedDates.contains(occDate))
                        dateSet.insert(occDate);
                }
            }

            icalrecur_iterator_free(recurIter);
        } else {
            // Kein RRULE: Einzelereignis
            for (int i = 0; i < durationDays; ++i) {
                QDate date(dtstart.year, dtstart.month, dtstart.day);
                QDate occDate = date.addDays(i);
                if (occDate.year() == year && !excludedDates.contains(occDate))
                    dateSet.insert(occDate);
            }
        }

        // Zusätzliche einzelne RDATEs
        for (icalproperty* rdateProp = icalcomponent_get_first_property(event, ICAL_RDATE_PROPERTY);
             rdateProp != nullptr;
             rdateProp = icalcomponent_get_next_property(event, ICAL_RDATE_PROPERTY))
        {
            icaldatetimeperiodtype dtperiod = icalproperty_get_rdate(rdateProp);
            if (!icaltime_is_null_time(dtperiod.time)) {
                icaltimetype rdate = dtperiod.time;
                for (int i = 0; i < durationDays; ++i) {
                    QDate occDate(rdate.year, rdate.month, rdate.day);
                    occDate = occDate.addDays(i);
                    if (occDate.year() == year && !excludedDates.contains(occDate))
                        dateSet.insert(occDate);
                }
            }
        }
    }

    // Ergebnis übertragen
    resultDates = QList<QDate>(dateSet.begin(), dateSet.end());
    std::sort(resultDates.begin(), resultDates.end());

    return resultDates;
}
