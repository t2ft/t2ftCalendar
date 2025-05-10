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

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QSettings>

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
{
    ui->setupUi(this);
    setWindowFlags(wf);
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

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "+++ MainWidget::mousePressEvent(" << event->pos() << "(" << event->globalPosition() << ") /" << event->buttons() << ")";
    switch (event->button()) {
    case Qt::RightButton:
        m_mouseResizing = true;
        m_startSize = size();
        m_startPosition = event->pos();
        qDebug() << "start resizing from" << m_startSize << "at" << m_startPosition;
        break;
    case Qt::MiddleButton:
        m_mouseMoving = true;
        m_startPosition = event->globalPosition() - pos();
        qDebug() << "start moving from" << pos() << "with delta" << m_startPosition;
        break;
    default:
        break;
    }
    qDebug() << "--- MainWidget::mousePressEvent()";
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "+++ MainWidget::mouseMoveEvent(" << event->pos() << "/" << event->buttons() << ")";
    if (m_mouseMoving) {
        QPointF pt = event->globalPosition()-m_startPosition;
        qDebug() << "moving to" << pt;
        move(pt.toPoint());
    }
    if (m_mouseResizing) {
        resize(m_startSize.width() + (event->pos().x() - m_startPosition.x()), m_startSize.height() + (event->pos().y() - m_startPosition.y()));
    }
    qDebug() << "--- MainWidget::mouseMoveEvent()";
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "+++ MainWidget::mouseReleaseEvent(" << event->pos() << "/" << event->buttons() << ")";
    switch (event->button()) {
    case Qt::RightButton:
        m_mouseResizing = false;
        qDebug() << "resizing done";
        break;
    case Qt::MiddleButton:
        m_mouseMoving = false;
        qDebug() << "moving done";
        break;
    default:
        break;
    }
    qDebug() << "--- MainWidget::mouseReleaseEvent()";
}
