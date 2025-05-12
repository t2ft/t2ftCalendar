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

#include <tapp.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    TApp a(argc, argv);
    qDebug() << "=== Starting" << qApp->applicationDisplayName() << "=============================================================";
    MainWidget w;
    w.show();
    int ret = a.exec();
    qDebug() << "--- DONE --------------------------------------------------------------------------------------------------------";
    return ret;
}
