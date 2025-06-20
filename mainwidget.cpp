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
#include <QDir>

#define CFG_GRP_WINDOW  "TWindow"
#define CFG_GEOMETRY    "geometry"

#define CFG_CONFIGFILE  "ConfigFile"


#ifdef QT_DEBUG
static const Qt::WindowFlags wf = Qt::FramelessWindowHint | Qt::Tool;
#else
static const Qt::WindowFlags wf = Qt::WindowStaysOnBottomHint | Qt::FramelessWindowHint | Qt::Tool;
#endif

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_scene(nullptr)
    , m_accessManager(nullptr)
    , m_requestState(RequestIdle)
    , m_holidays(nullptr)
    , m_vacations(nullptr)
    , m_timerUpdate(nullptr)
    , m_currentDate(QDate::currentDate())
    , m_workDate(m_currentDate)

{
    // allow us to dispatch windows power events
    TPowerEventFilter *pFilter = new TPowerEventFilter(this);
    QAbstractEventDispatcher::instance()->installNativeEventFilter(pFilter);
    connect(pFilter, &TPowerEventFilter::ResumeSuspend, this, &MainWidget::updateCalendar);

    // get configuration of Bundesland and imported calendars from file
    QSettings set;
    QString cfgFile = QDir::fromNativeSeparators(set.value(CFG_CONFIGFILE, "./t2ftcalendar.cfg").toString());
    qDebug() << "reading configuration from:" << cfgFile;
    QSettings cfg(cfgFile, QSettings::IniFormat);
    m_bundesland = cfg.value("Bundesland").toString();
    qDebug() << "Bundesland=" << m_bundesland;
    int n=1;
    while(true) {
        qDebug().nospace() << "Calendar #" << n;
        QString url = cfg.value(QString("URL%1").arg(n)).toString();
        if (url.isEmpty()) {
            break;
        }
        QString defaultColor = cfg.value(QString("DefaultColor%1").arg(n), "#FF0000").toString();
        QString forcedColor = cfg.value(QString("ForcedColor%1").arg(n)).toString();
        qint64 interval = cfg.value(QString("UpdateInterval%1").arg(n), 3600).toLongLong();
        QString defaultName = cfg.value(QString("DefaultName%1").arg(n), QString("Calendar#%1").arg(n)).toString();
        ImportedCalendar *cal = new ImportedCalendar(m_currentDate.year(), url, interval, defaultColor, defaultName, forcedColor, this);
        connect(cal, &ImportedCalendar::newEntries, this, &MainWidget::onNewCalendarEntries);
        m_iCals << cal;
        ++n;
        qDebug() << "  URL          =" << url;
        qDebug() << "  defaultName  =" << defaultName;
        qDebug() << "  defaultColor =" << defaultColor << "; forcedColor=" << forcedColor;
        qDebug() << "  interval     =" << interval;
    }

    // init GUI
    ui->setupUi(this);
    connect(ui->graphicsView, &ZoomView::mouseMove, this, &MainWidget::onMouseMove);
    connect(ui->graphicsView, &ZoomView::mouseResize, this, &MainWidget::onMouseResize);

    m_scene = ui->graphicsView->scene();
    if (m_scene == nullptr) {
        qDebug() << "creation of a new QGraphicsSene is required";
        m_scene = new QGraphicsScene;
        ui->graphicsView->setScene(m_scene);
    }
    setWindowFlags(wf);
    // Installation des Ereignisfilters
    createCalendar();
    updateCalendarYearly(m_currentDate);
    QTimer::singleShot(1, this, SLOT(updateGeometry()));

    setMouseTracking(true);
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->viewport()->setMouseTracking(true);

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
        QList<CalendarEvent> events;
        for (auto &c : m_iCals) {
            if (c) {
                CalendarEvent e = c->entry(d->date());
                if (e.isValid()) {
                    events.append(e);
                }
            }
        }
        d->setEvents(events);
    }
//    qDebug() << "--- MainWidget::onNewCalendarEntries()";
}

void MainWidget::onMouseResize(QSize sz)
{
    qDebug() << "+++ MainWidget::onMouseResize(sz=" << sz << ")";
    resize(sz);
    qDebug() << "--- MainWidget::onMouseResize()";
}

void MainWidget::onMouseMove(QPoint pt)
{
    qDebug() << "+++ MainWidget::onMouseMove(pt=" << pt << ")";
    move(pt);
    qDebug() << "--- MainWidget::onMouseMove()";
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
    if (!m_bundesland.isEmpty()) {
        m_requestState = RequestLastYearsVacations;
        QString url = QString("https://ferien-api.de/api/v1/holidays/%1/%2").arg(m_bundesland).arg(m_workDate.year()-1);
        qDebug() << "*** requesting last year's school holidays:" << url;
        m_accessManager->get(QNetworkRequest(QUrl(url)));
    } else {
        m_requestState = RequestIdle;
        createDays();
    }
    qDebug() << "--- MainWidget::updateCalendarYearly()";
}

void MainWidget::createDays()
{
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
                    m_accessManager->get(QNetworkRequest(QUrl(QString("https://feiertage-api.de/api/?jahr=%1&nur_land=%2").arg(m_workDate.year()).arg(m_bundesland))));
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
        createDays();
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
