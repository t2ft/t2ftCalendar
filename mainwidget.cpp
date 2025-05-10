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
#include "calendarday.h"

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <QDate>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>

#define CFG_GRP_WINDOW  "TWindow"
#define CFG_GEOMETRY    "geometry"

#ifdef QT_DEBUG
static const Qt::WindowFlags wf = Qt::FramelessWindowHint;
#else
static const Qt::WindowFlags wf = Qt::WindowStaysOnBottomHint | Qt::FramelessWindowHint;
#endif

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_mouseResizing(false)
    , m_mouseMoving(false)
    , m_scene(nullptr)
{
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

    createCalendar(QDate::currentDate().year());
    QTimer::singleShot(1, this, SLOT(updateGeometry()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    qDebug() << "+++ MainWidget::closeEvent(QCloseEvent *event)";
    QSettings cfg;
    cfg.beginGroup(CFG_GRP_WINDOW);
    cfg.setValue(CFG_GEOMETRY, geometry());
    QWidget::closeEvent(event);
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

void MainWidget::createCalendar(int year)
{
    const QString monthName[12] = { "Jan", "Feb", "Mär", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez" };
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
    brush.setColor(Qt::white);
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
        t = new QGraphicsSimpleTextItem(monthName[i]);
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

    // draw all of the days
    QDate date(year, 1, 1);
    rc1.setRect(21, 37, 61, 32);
    rc2 = rc1;
    int month = date.month();
    while (date.year()==year) {
        if (month != date.month()) {
            month = date.month();
            rc1.moveLeft(rc1.right()+2);
            rc2 = rc1;
        }
        CalendarDay *d = new CalendarDay(date, rc2);
        m_scene->addItem(d);
        rc2.moveTop(rc2.bottom()+2);
        date = date.addDays(1);
    }
}

QPointF MainWidget::centered(const QRectF &a, const QRectF &b)
{
    qDebug() << "+++ MainWidget::centered(a =" << a << "b =" << b << ")";
    QPointF pt = QPointF(b.left() + (b.width()-a.width())/2., b.top() + (b.height()-a.height())/2.);
    qDebug() << "--- MainWidget::centered() =" << pt;
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
