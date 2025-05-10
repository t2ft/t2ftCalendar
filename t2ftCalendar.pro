QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# application version
VERSION = 1.0.0.0
QMAKE_TARGET_COMPANY = t2ft
QMAKE_TARGET_PRODUCT = t2ftCalendar
QMAKE_TARGET_DESCRIPTION = Year Long Desktop Calendar
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2025 by t2ft - Thomas Thanner

# Define some preprocessor macros to get the infos in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_ORGANIZATION=\\\"$$QMAKE_TARGET_COMPANY\\\"
DEFINES += APP_NAME=\\\"$$QMAKE_TARGET_PRODUCT\\\"
DEFINES += APP_DOMAIN=\\\"t2ft.de\\\"

SOURCES += \
    main.cpp \
    mainwidget.cpp \
    mouseeventfilter.cpp \
    tapp.cpp \
    tmessagehandler.cpp

HEADERS += \
    mainwidget.h \
    mouseeventfilter.h \
    tapp.h \
    tmessagehandler.h \
    tmsghandler_main.h

FORMS += \
    mainwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    t2ftcalendar.qrc

RC_ICONS = res/t2ftCalendar.ico
