// ***************************************************************************
// <project description>
// ---------------------------------------------------------------------------
// mouseeventfilter.h
// <file description>
// ---------------------------------------------------------------------------
// Copyright (C) 2025 by t2ft - Thomas Thanner
// Waldstrasse 15, 86399 Bobingen, Germany
// thomas@t2ft.de
// ---------------------------------------------------------------------------
// 2025-5-10  tt  Initial version created
// ***************************************************************************
#ifndef MOUSEEVENTFILTER_H
#define MOUSEEVENTFILTER_H

#include <QObject>

class MouseEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit MouseEventFilter(QObject *parent);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // MOUSEEVENTFILTER_H
