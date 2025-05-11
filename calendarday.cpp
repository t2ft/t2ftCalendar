// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// calendarday.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#include "calendarday.h"
#include <QPainter>


CalendarDay::CalendarDay(const QDate &date, const QRectF &rect, const QString &holidayName, bool isVacation, QGraphicsItem *parent)
    : QGraphicsItem(parent),
    m_date(date),
    m_rect(rect),
    m_rcHoliday(rect.adjusted(m_rect.width()*0.3,-1,-1,-1)),
    m_holidayName(holidayName)
{
    m_stringDay = QLocale::system().toString(m_date, "ddd").left(2);
    m_stringDate = QLocale::system().toString(m_date, "dd.MMM");

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

    m_toHoliday = QTextOption(Qt::AlignLeft | Qt::AlignVCenter);
    m_toHoliday.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    m_brushBackground.setStyle(Qt::SolidPattern);
    if (isVacation && !isSunday && holidayName.isEmpty()) {
        // vacation, but not a sunday and not a holiday
        m_brushBackground.setColor(QColor("#FFFF00"));
    } else {
        if (m_holidayName.isEmpty()) {
            // regular day
            if (isSunday) {
                m_brushBackground.setColor(QColor("#FFCC99"));
            } else if (isSaturday) {
                m_brushBackground.setColor(QColor("#FFFFCC"));
            } else {
                m_brushBackground.setStyle(Qt::NoBrush);
            }
        } else {
            // holiday
            m_brushBackground.setColor(QColor("#FFD9D9"));
        }
    }
    m_penText.setStyle(Qt::SolidLine);
    m_penText.setColor(m_holidayName.isEmpty() ? Qt::black : Qt::red);
    updateDay();
}


QRectF CalendarDay::boundingRect() const
{
    return m_rect;
}

void CalendarDay::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
}

void CalendarDay::updateDay()
{
    if (m_date==QDate::currentDate()) {
        m_penBorder.setStyle(Qt::SolidLine);
        m_penBorder.setColor(QColor("#FF0000"));
        m_penBorder.setWidth(5);
    } else {
        m_penBorder.setStyle(Qt::NoPen);
    }
    update();
}
