// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// mouseeventfilter.cpp
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#include "mouseeventfilter.h"
#include "mainwidget.h"
#include <QMouseEvent>

MouseEventFilter::MouseEventFilter(QObject *parent)
    : QObject{parent}
{
}


bool MouseEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    MainWidget *w = qobject_cast<MainWidget*>(parent());
    if (w !=nullptr) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (event->type() == QEvent::MouseMove) {
            w->mouseMoveEvent(mouseEvent);
            return true; // Ereignis wurde verarbeitet
        }
        if (event->type() == QEvent::MouseButtonRelease) {
            w->mouseReleaseEvent(mouseEvent);
            return true; // Ereignis wurde verarbeitet
        }
    }
    return QObject::eventFilter(obj, event);
}
