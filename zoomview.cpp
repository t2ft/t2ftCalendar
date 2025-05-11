#include <QDebug>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QScrollBar>
#include "zoomview.h"

ZoomView::ZoomView(QWidget *parent)
   : QGraphicsView(parent)
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
