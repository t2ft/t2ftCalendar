// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// %{Cpp:License:FileName}
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-9  tt  Initial version created
// ***************************************************************************
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QDate>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class QGraphicsScene;
class CalDayGraphicsItem;
class QNetworkAccessManager;
class QNetworkReply;
class SchoolVacations;
class PublicHolidays;
class ImportedCalendar;


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateGeometry();
    void onReplyFinished(QNetworkReply *reply);
    void updateCalendar();
    void onNewCalendarEntries();

private:
    typedef enum {
        RequestIdle,
        RequestLastYearsVacations,
        RequestThisYearsVacations,
        RequestHolidays
    } RequestState;

    void createCalendar();
    void updateCalendarYearly(const QDate &date);
    void updateCalendarDaily(const QDate &date);
    QPointF centered(const QRectF &a, const QRectF &b);
    void appendColor(const ImportedCalendar *ical, const QDate &date, QStringList &events);

    Ui::MainWidget *ui;

    bool                        m_mouseResizing;
    bool                        m_mouseMoving;
    QSizeF                      m_startSize;
    QPointF                     m_startPosition;
    QGraphicsScene              *m_scene;
    QList<CalDayGraphicsItem*>         m_days;
    QNetworkAccessManager       *m_accessManager;
    RequestState                m_requestState;
    PublicHolidays              *m_holidays;
    SchoolVacations             *m_vacations;
    QTimer                      *m_timerUpdate;
    QDate                       m_currentDate;
    QDate                       m_workDate;
    ImportedCalendar            *m_icalSTK;
    ImportedCalendar            *m_icalThomas;
    ImportedCalendar            *m_icalT2ft;
    ImportedCalendar            *m_icalHnF;
    ImportedCalendar            *m_icalHoS;
};
#endif // MAINWIDGET_H
