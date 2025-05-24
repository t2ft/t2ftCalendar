// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// caldaygraphicsitem.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#include "caldaygraphicsitem.h"
#include "eventgraphicsitem.h"
#include <QPainter>


CalDayGraphicsItem::CalDayGraphicsItem(const QDate &date, const QRectF &rect, const QString &holidayName, bool isPublicHoliday, bool isVacation, QGraphicsItem *parent)
    : QGraphicsItem(parent),
    m_date(date),
    m_rect(rect),
    m_rcHoliday(rect.adjusted(m_rect.width()*0.3,-1,-1,-1)),
    m_holidayName(holidayName)
{
    m_stringDay = QLocale::system().toString(m_date, "ddd").left(2);
    m_stringDate = QLocale::system().toString(m_date, "dd.MMM");
    if ((date.dayOfWeek()==1) || ((date.month()==1) && (date.day()==1))) {
        int w = date.weekNumber();
        if ((w==1) && (date.month()!=1)) {
            w += 52;
        }
        m_stringWeek = QString::number(w);
        m_linesWeek.append(QLineF(m_rect.topLeft(),  m_rect.topRight()));
        m_linesWeek.append(QLineF(m_rect.topRight(), m_rect.bottomRight()));
    }

    const bool isSunday = m_date.dayOfWeek()==7;
    const bool isSaturday = m_date.dayOfWeek()==6;
    const bool isWeekend = m_date.dayOfWeek() > 5;

    m_fontDay.setFamily("Calibri");
    m_fontDay.setPointSizeF(9.2);
    m_fontDay.setWeight((isWeekend || !m_holidayName.isEmpty()) ? QFont::DemiBold : QFont::Normal);

    m_fontDate.setFamily("Bahnschrift");
    m_fontDate.setPointSizeF(5);
    m_fontDate.setWeight(QFont::Light);

    m_fontHoliday.setFamily("Calibri");
    m_fontHoliday.setPointSizeF(7);
    m_fontHoliday.setWeight(QFont::Normal);

    m_fontWeek.setFamily("Source Sans Pro");
    m_fontWeek.setPointSizeF(14);
    m_fontWeek.setWeight(QFont::DemiBold);

    m_penWeek.setStyle(Qt::SolidLine);
    m_penWeek.setWidthF(3.0);
    m_penWeek.setColor(Qt::blue);

    m_toHoliday = QTextOption(Qt::AlignLeft | Qt::AlignVCenter);
    m_toHoliday.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    m_brushBackground.setStyle(Qt::SolidPattern);
    if (isVacation && !isSunday && !isPublicHoliday) {
        // vacation, but not a sunday and not a holiday
        m_brushBackground.setColor(QColor("#FFFF00"));
    } else {
        if (!isPublicHoliday) {
            // regular day
            if (isSunday) {
                m_brushBackground.setColor(QColor("#FFCC99"));
            } else if (isSaturday) {
                m_brushBackground.setColor(QColor("#FFFFCC"));
            } else {
                m_brushBackground.setColor(Qt::white);
            }
        } else {
            // holiday
            m_brushBackground.setColor(QColor("#FFD9D9"));
        }
    }
    m_penText.setStyle(Qt::SolidLine);
    m_penText.setColor(isPublicHoliday ? Qt::red : Qt::black);

    m_rcEvent.setRect(m_rect.left()+3, m_rect.top()+3, 5, 5);
}


QRectF CalDayGraphicsItem::boundingRect() const
{
    return m_rect;
}

void CalDayGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(m_penBorder);
    painter->setBrush(m_brushBackground);
    painter->drawRect(m_rect);

    painter->setPen(m_penText);
    painter->setFont(m_fontDay);
    painter->drawText(m_rect.adjusted( 1, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, m_stringDay);

    painter->setPen(Qt::black);
    painter->setFont(m_fontDate);
    painter->drawText(m_rect.adjusted( 0, 0,-1,-1), Qt::AlignRight | Qt::AlignBottom, m_stringDate);

    if (!m_holidayName.isEmpty()) {
        painter->setPen(m_penText);
        painter->setFont(m_fontHoliday);
        painter->drawText(m_rcHoliday, m_holidayName, m_toHoliday);
    }

    if (!m_stringWeek.isEmpty()) {
        painter->setPen(m_penWeek);
        painter->drawLines(m_linesWeek);
        painter->setFont(m_fontWeek);
        painter->drawText(m_rect.adjusted(0,0,-2,0), Qt::AlignRight | Qt::AlignVCenter, m_stringWeek);
    }
}

void CalDayGraphicsItem::setEvents(const QList<CalendarEvent> &events)
{
    qDeleteAll(m_events);
    m_events.clear();
    QRectF rc = m_rcEvent;
    for (const auto &e : events) {
        EventGraphicsItem *egi = new EventGraphicsItem(e.summary(), e.color(), rc, this);
        m_events.append(egi);
        rc.translate(rc.width()+2, 0);
    }
    update();
}

void CalDayGraphicsItem::updateDay(const QDate &date)
{
    if (m_date==date) {
        m_penBorder.setStyle(Qt::SolidLine);
        m_penBorder.setColor(QColor("#FF0000"));
        m_penBorder.setWidth(5);
    } else {
        m_penBorder.setStyle(Qt::NoPen);
    }
    update();
}
