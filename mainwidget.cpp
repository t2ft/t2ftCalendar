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
#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "mouseeventfilter.h"
#include "caldaygraphicsitem.h"
#include "schoolvacations.h"
#include "publicholydays.h"
#include "tpowereventfilter.h"
#include "importedcalendar.h"

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <QDate>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAbstractEventDispatcher>

#define CFG_GRP_WINDOW  "TWindow"
#define CFG_GEOMETRY    "geometry"

#ifdef QT_DEBUG
static const Qt::WindowFlags wf = Qt::FramelessWindowHint | Qt::Tool;
#else
static const Qt::WindowFlags wf = Qt::WindowStaysOnBottomHint | Qt::FramelessWindowHint | Qt::Tool;
#endif

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_mouseResizing(false)
    , m_mouseMoving(false)
    , m_scene(nullptr)
    , m_accessManager(nullptr)
    , m_requestState(RequestIdle)
    , m_holidays(nullptr)
    , m_vacations(nullptr)
    , m_timerUpdate(nullptr)
    , m_currentDate(QDate::currentDate())
    , m_workDate(m_currentDate)
    , m_icalSTK(   new ImportedCalendar(m_currentDate.year(), "https://rest.konzertmeister.app/api/v1/ical/d7ede305-dd3d-49d7-af71-4574cd109f06?hideNegative=true&excludeMeetingPoints=true&onlyExternal=false", 3600))
    , m_icalThomas(new ImportedCalendar(m_currentDate.year(), "https://thblue.my-gateway.de/remote.php/dav/public-calendars/gTws6bpLTNrMqyoa?export",                                                            3907))
    , m_icalT2ft(  new ImportedCalendar(m_currentDate.year(), "https://thblue.my-gateway.de/remote.php/dav/public-calendars/yfEBqz5pwJ6t2saC?export",                                                            4211))
    , m_icalHnF(   new ImportedCalendar(m_currentDate.year(), "https://thblue.my-gateway.de/remote.php/dav/public-calendars/b2tesWXFD8zK89kH?export",                                                            4513))
    , m_icalHoS(   new ImportedCalendar(m_currentDate.year(), "https://thblue.my-gateway.de/remote.php/dav/public-calendars/axpyXR4mSCTiHToX?export",                                                            4817))
{
    // allow us to dispatch windows power events
    TPowerEventFilter *pFilter = new TPowerEventFilter(this);
    QAbstractEventDispatcher::instance()->installNativeEventFilter(pFilter);
    connect(pFilter, &TPowerEventFilter::ResumeSuspend, this, &MainWidget::updateCalendar);

    connect(m_icalSTK,    &ImportedCalendar::newEntries, this, &MainWidget::onNewCalendarEntries);
    connect(m_icalThomas, &ImportedCalendar::newEntries, this, &MainWidget::onNewCalendarEntries);
    connect(m_icalT2ft,   &ImportedCalendar::newEntries, this, &MainWidget::onNewCalendarEntries);
    connect(m_icalHnF,    &ImportedCalendar::newEntries, this, &MainWidget::onNewCalendarEntries);
    connect(m_icalHoS,    &ImportedCalendar::newEntries, this, &MainWidget::onNewCalendarEntries);

    ui->setupUi(this);
    m_scene = ui->graphicsView->scene();
    if (m_scene == nullptr) {
        qDebug() << "creation of a new QGraphicsSene is required";
        m_scene = new QGraphicsScene;
        ui->graphicsView->setScene(m_scene);
    }
    setWindowFlags(wf);
    // Installation des Ereignisfilters
    MouseEventFilter* filter = new MouseEventFilter(this);
    ui->graphicsView->viewport()->installEventFilter(filter);
    createCalendar();
    updateCalendarYearly(m_currentDate);
    QTimer::singleShot(1, this, SLOT(updateGeometry()));


    m_timerUpdate = new QTimer(this);
    connect(m_timerUpdate, &QTimer::timeout, this, &MainWidget::updateCalendar);
#ifdef QT_DEBUG_UPDATE
    m_timerUpdate->start(3000);  // run every 50ms for faster debugging
#else
    m_timerUpdate->start(3600000);  // run every hour
#endif
    }

MainWidget::~MainWidget()
{
    delete ui;
    delete m_vacations;
    delete m_holidays;
    delete m_icalSTK;
    delete m_icalThomas;
    delete m_icalT2ft;
    delete m_icalHnF;
    delete m_icalHoS;
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << "+++ MainWidget::closeEvent(QCloseEvent *event)";
    QSettings cfg;
    cfg.beginGroup(CFG_GRP_WINDOW);
    cfg.setValue(CFG_GEOMETRY, geometry());
    QWidget::closeEvent(event);
    qApp->quit();
    qDebug() << "--- MainWidget::closeEvent(QCloseEvent *event)";
}

void MainWidget::updateGeometry()
{
    qDebug() << "+++ MainWidget::updateGeometry()";
    QSettings cfg;
    cfg.beginGroup(CFG_GRP_WINDOW);
    QRect rc = cfg.value(CFG_GEOMETRY).toRect();
    if (!rc.isEmpty()) {
        setGeometry(rc);
    }
    cfg.endGroup();
    qDebug() << "--- MainWidget::updateGeometry()";
}

void MainWidget::createCalendar()
{
    qDebug() << "+++ MainWidget::createCalendar()";
    const int H = 1090;
    const int W = 798;
    QPen pen;
    QBrush brush;
    QFont font;
    QRectF rc1, rc2;
    QGraphicsSimpleTextItem *t;

    // draw main frame
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(3);
    brush.setColor(QColor("#E6E6E6"));
    brush.setStyle(Qt::SolidPattern);
    m_scene->addRect(0, 0, W, H, pen, brush);

    // draw month columns
    pen.setWidth(2);
    brush.setColor(Qt::black);
    font.setFamily("Source Sans Pro");
    font.setWeight(QFont::DemiBold);
    font.setPointSizeF(14.66395569);
    rc1.setRect(20, 0, 65, 36);
    for (int i=0; i<12; ++i) {
        t = new QGraphicsSimpleTextItem(QLocale::system().toString(QDate(m_currentDate.year(), i+1, 1), "MMM"));
        t->setFont(font);
        t->setBrush(brush);
        t->setPos(centered(t->boundingRect(), rc1));
        m_scene->addItem(t);
        m_scene->addLine(rc1.left(), 0, rc1.left(), H, pen);
        rc1.moveLeft(rc1.right()-pen.widthF());
    }
    m_scene->addLine(rc1.left(), 0, rc1.left(), H, pen);

    // draw day rows
    font.setPointSizeF(11.);
    rc1.setRect(0, 36, 20, 36);
    rc2 = rc1;
    rc2.moveRight(W);
    for (int i=1; i<32; ++i) {
        t = new QGraphicsSimpleTextItem(QString::number(i));
        t->setFont(font);
        t->setBrush(brush);
        t->setPos(centered(t->boundingRect(), rc1));
        m_scene->addItem(t);
        t = new QGraphicsSimpleTextItem(QString::number(i));
        t->setFont(font);
        t->setBrush(brush);
        t->setPos(centered(t->boundingRect(), rc2));
        m_scene->addItem(t);
        m_scene->addLine(0, rc1.top(), W, rc1.top(), pen);
        rc1.moveTop(rc1.bottom()-pen.widthF());
        rc2.moveTop(rc2.bottom()-pen.widthF());
    }
    qDebug() << "--- MainWidget::createCalendar()";
}

void MainWidget::updateCalendar()
{
    //qDebug() << "+++ MainWidget::updateCalendar()";
#ifdef QT_DEBUG
    m_workDate = m_currentDate.addDays(1);
#else
    m_workDate = QDate::currentDate();
#endif
    if (m_workDate != m_currentDate) {
        // date has changed
        if (m_workDate.year() != m_currentDate.year()) {
            qDebug() << "  HAPPY NEW YEAR !";
            updateCalendarYearly(m_workDate);
        } else {
            if (m_workDate.day() != m_currentDate.day()) {
                //qDebug() << "  welcome to another day";
                updateCalendarDaily(m_workDate);
            }
        }
        m_currentDate = m_workDate;
    }
    //qDebug() << "--- MainWidget::updateCalendar()";
}

void MainWidget::onNewCalendarEntries()
{
//    qDebug() << "+++ MainWidget::onNewCalendarEntries()";
    for (auto &d : m_days) {
        QStringList eventColors;
        appendColor(m_icalSTK,    d->date(), eventColors);
        appendColor(m_icalThomas, d->date(), eventColors);
        appendColor(m_icalT2ft,   d->date(), eventColors);
        appendColor(m_icalHnF,    d->date(), eventColors);
        appendColor(m_icalHoS,    d->date(), eventColors);
        d->setEvents(eventColors);
    }
//    qDebug() << "--- MainWidget::onNewCalendarEntries()";
}

void MainWidget::appendColor(const ImportedCalendar *ical, const QDate &date, QStringList &events)
{
    if (ical && ical->hasEntry(date)) {
        events.append(ical->color());
    }
}



void MainWidget::updateCalendarYearly(const QDate &date)
{
    qDebug() << "+++ MainWidget::updateCalendarYearly(" << date << ")";
    m_workDate = date;

    // cleanup old days
    for (auto &d : m_days) {
        if (m_scene) {
            m_scene->removeItem(d);
        }
        delete d;
    }
    m_days.clear();

    delete m_holidays;
    m_holidays = nullptr;

    delete m_vacations;
    m_vacations = nullptr;

    m_accessManager = new QNetworkAccessManager(this);
    connect(m_accessManager, &QNetworkAccessManager::finished, this, &MainWidget::onReplyFinished);
    m_requestState = RequestLastYearsVacations;
    QString url = QString("https://ferien-api.de/api/v1/holidays/BY/%1").arg(m_workDate.year()-1);
    qDebug() << "*** requesting last year's school holidays:" << url;
    m_accessManager->get(QNetworkRequest(QUrl(url)));
    qDebug() << "--- MainWidget::updateCalendarYearly()";
}

void MainWidget::onReplyFinished(QNetworkReply *reply)
{
    qDebug() << "+++ MainWidget::onReplyFinished()";
    if (reply) {
        QByteArray data = reply->readAll();
        if (!data.isEmpty()) {
            //qDebug() << data;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
            switch (m_requestState) {
            case RequestLastYearsVacations: {
                // process data from last year, wich is required to get christmas holidays right
                qDebug() << "*** last years holidays received:";
                m_vacations = new SchoolVacations(jsonDoc.array());
                // request school vacations for this year next
                QString url = QString("https://ferien-api.de/api/v1/holidays/BY/%1").arg(m_workDate.year());
                qDebug() << "*** requesting this year's school holidays:" << url;
                m_requestState = RequestThisYearsVacations;
                m_accessManager->get(QNetworkRequest(QUrl(url)));
                break;
            }
            case RequestThisYearsVacations:
                // process data for this year
                qDebug() << "*** this years holidays received:";
                if (m_vacations!=nullptr) {
                    *m_vacations += SchoolVacations(jsonDoc.array());
                    const int n = m_vacations->size();
                    for (int i=0; i<n; ++i) {
                        qDebug() << (*m_vacations)[i].toString();
                    }
                    // request public holidays next
                    m_requestState = RequestHolidays;
                    qDebug() << "*** requesting public holidays";
                    m_accessManager->get(QNetworkRequest(QUrl(QString("https://feiertage-api.de/api/?jahr=%1&nur_land=BY").arg(m_workDate.year()))));
                } else {
                    qWarning() << "!!! m_vacations is a nullptr";
                }
                break;
            case RequestHolidays:
                // process data
                qDebug() << "*** public holidays received:";
                m_holidays = new PublicHolidays(jsonDoc.object(), m_workDate);
                if (m_holidays!=nullptr) {
                    const int n = m_holidays->size();
                    for (int i=0; i<n; ++i) {
                        qDebug() << (*m_holidays)[i].toString();
                    }
                } else {
                    qWarning() << "!!! m_holidays is a nullptr";
                }
                // no more requests
                m_requestState = RequestIdle;
                break;
            default:
                qWarning() << "!!! unexpected reply state";
                m_requestState = RequestIdle;
                break;
            }
        } else {
            qWarning() << "!!! no data replied: " << reply->errorString();
            m_requestState = RequestIdle;
        }
    } else {
        qWarning() << "!!! reply was nullptr";
        m_requestState = RequestIdle;
    }
    if (m_requestState==RequestIdle) {
        qDebug() << "creating days...";
        // create new days
        QRectF rc1, rc2;
        // draw all of the days
        QDate date(m_workDate.year(), 1, 1);
        rc1.setRect(21, 37, 61, 32);
        rc2 = rc1;
        int month = date.month();
        while (date.year()==m_workDate.year()) {
            if (month != date.month()) {
                month = date.month();
                rc1.moveLeft(rc1.right()+2);
                rc2 = rc1;
            }
            bool isVacation = false;
            if (m_vacations!=nullptr) {
                isVacation = !m_vacations->checkVacation(date).isEmpty();
            }
            QString holidayName;
            bool isPublic = false;
            if (m_holidays!=nullptr) {
                m_holidays->isHoliday(date, holidayName, isPublic);
            }
            CalDayGraphicsItem *d = new CalDayGraphicsItem(date, rc2, holidayName, isPublic, isVacation);
            m_scene->addItem(d);
            m_days.append(d);
            rc2.moveTop(rc2.bottom()+2);
            date = date.addDays(1);
        }
        updateCalendarDaily(m_workDate);
    }
    qDebug() << "--- MainWidget::onReplyFinished()";
}

void MainWidget::updateCalendarDaily(const QDate &date)
{
    //qDebug() << "+++ MainWidget::updateCalendarDaily(" << date << ")";
    for (auto &d : m_days) {
        d->updateDay(date);
    }
    //qDebug() << "--- MainWidget::updateCalendarDaily()";
}

QPointF MainWidget::centered(const QRectF &a, const QRectF &b)
{
    //qDebug() << "+++ MainWidget::centered(a =" << a << "b =" << b << ")";
    QPointF pt = QPointF(b.left() + (b.width()-a.width())/2., b.top() + (b.height()-a.height())/2.);
    //qDebug() << "--- MainWidget::centered() =" << pt;
    return pt;
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "+++ MainWidget::mousePressEvent(" << event->pos() << "(" << event->globalPosition() << ") /" << event->buttons() << ")";
    switch (event->button()) {
    case Qt::RightButton:
        m_mouseResizing = true;
        m_startSize = size();
        m_startPosition = event->pos();
//        qDebug() << "start resizing from" << m_startSize << "at" << m_startPosition;
        break;
    case Qt::MiddleButton:
        m_mouseMoving = true;
        m_startPosition = event->globalPosition() - pos();
//        qDebug() << "start moving from" << pos() << "with delta" << m_startPosition;
        break;
    default:
        break;
    }
//    qDebug() << "m_mouseMoving=" << m_mouseMoving;
//    qDebug() << "m_mouseResizing=" << m_mouseResizing;
//    qDebug() << "--- MainWidget::mousePressEvent()";
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "+++ MainWidget::mouseMoveEvent(" << event->pos() << "/" << event->buttons() << ")";
//    qDebug() << "m_mouseMoving=" << m_mouseMoving;
//    qDebug() << "m_mouseResizing=" << m_mouseResizing;
    if (m_mouseMoving) {
        QPointF pt = event->globalPosition()-m_startPosition;
//        qDebug() << "moving to" << pt;
        move(pt.toPoint());
    }
    if (m_mouseResizing) {
        QSize sz = QSize(m_startSize.width() + (event->pos().x() - m_startPosition.x()), m_startSize.height() + (event->pos().y() - m_startPosition.y()));
//        qDebug() << "resizing to" << sz;
        resize(sz);
    }
//    qDebug() << "--- MainWidget::mouseMoveEvent()";
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << "+++ MainWidget::mouseReleaseEvent(" << event->pos() << "/" << event->buttons() << ")";
    switch (event->button()) {
    case Qt::RightButton:
        m_mouseResizing = false;
//        qDebug() << "resizing done";
        break;
    case Qt::MiddleButton:
        m_mouseMoving = false;
//        qDebug() << "moving done";
        break;
    default:
        break;
    }
//    qDebug() << "--- MainWidget::mouseReleaseEvent()";
}
