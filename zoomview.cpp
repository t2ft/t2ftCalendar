#include <QDebug>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QScrollBar>
#include "zoomview.h"

ZoomView::ZoomView(QWidget *parent)
   : QGraphicsView(parent)
    , m_mouseResizing(false)
    , m_mouseMoving(false)
{
}


//////////////////////////////////////////////////////////////////////////////
// [ZoomView::resizeEvent]
void ZoomView::resizeEvent(QResizeEvent *event)
{
    fitInView();
    QGraphicsView::resizeEvent(event);
}

void ZoomView::softFitInView()
{
    fitInView();
}

void ZoomView::fitInView()
{
    QRectF rc = scene()->sceneRect();
    QGraphicsView::fitInView( rc, Qt::IgnoreAspectRatio );
}


void ZoomView::mousePressEvent(QMouseEvent *event)
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
        m_startPosition = event->pos();
        //        qDebug() << "start moving from" << pos() << "with delta" << m_startPosition;
        break;
    default:
        break;
    }
    //    qDebug() << "m_mouseMoving=" << m_mouseMoving;
    //    qDebug() << "m_mouseResizing=" << m_mouseResizing;
    //    qDebug() << "--- MainWidget::mousePressEvent()";
}

void ZoomView::mouseMoveEvent(QMouseEvent *event)
{
    //    qDebug() << "m_mouseMoving=" << m_mouseMoving;
    //    qDebug() << "m_mouseResizing=" << m_mouseResizing;
    if (m_mouseMoving) {

        QPointF pt = event->globalPosition()-m_startPosition;
        //        qDebug() << "moving to" << pt;
        emit mouseMove(pt.toPoint());
    }
    if (m_mouseResizing) {
        QSize sz = QSize(m_startSize.width() + (event->pos().x() - m_startPosition.x()), m_startSize.height() + (event->pos().y() - m_startPosition.y()));
        //        qDebug() << "resizing to" << sz;
        emit mouseResize(sz);
    }
    if (!m_mouseMoving && !m_mouseResizing) {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void ZoomView::mouseReleaseEvent(QMouseEvent *event)
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
