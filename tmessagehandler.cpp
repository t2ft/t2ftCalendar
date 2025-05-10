// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tmessagegandler.cpp
// improved message handler for qDebug() etc.
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------

#include "tmessagehandler.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>

#define MESSAGE_LIMIT   32000
static const int msgCollateTime = 5;   // print out identical messages after 5 seconds, latest

TMessageHandler::TMessageHandler(const QString &filename, QObject *parent)
    : QObject(parent)
    , m_msg(MESSAGE_LIMIT)
    , m_msgStart(0)
    , m_msgNext(0)
    , m_numMessages(0)
    , m_filename(filename)
    , m_finallySaved(false)
{
#ifdef QT_DEBUG
    fprintf(stderr, "+++ TMessageHandler::TMessageHandler()\n");
#endif
    qRegisterMetaType<TMessageHandler*>("TMessageHandlerStar");
    memset(m_lastMsg.text, '\0', sizeof(m_lastMsg.text));
    m_lastMsg.firstTime = QDateTime::currentMSecsSinceEpoch();
    m_lastMsg.lastTime = m_lastMsg.firstTime;
    m_lastMsg.repeat = 0;
#ifdef QT_DEBUG
    fprintf(stderr, "--- TMessageHandler::TMessageHandler()\n");
#endif
}

TMessageHandler::~TMessageHandler()
{
#ifdef QT_DEBUG
    fprintf(stderr, "+++ TMessageHandler::~TMessageHandler()\n");
#endif
    if (!m_finallySaved)
        saveMessages(m_filename, true);
#ifdef QT_DEBUG
    fprintf(stderr, "--- TMessageHandler::~TMessageHandler()\n");
#endif
}

void TMessageHandler::addMessage(const QString &text)
{
#ifdef QT_DEBUG
    //fprintf(stderr, "+++ TMessageHandler::addMessage()\n");
#endif
    MSG_ENTRY msg;
    QByteArray utf8 = text.toUtf8();
    strncpy(msg.text, utf8.constData(), sizeof(msg.text)-1);
    msg.text[sizeof(msg.text)-1]='\0';
    msg.lastTime = QDateTime::currentMSecsSinceEpoch();
    msg.repeat = 0;
    if (msg.text != m_lastMsg.text) {
        //this is a new message
        if (m_lastMsg.repeat)
            append(m_lastMsg);
        append(msg);
        m_lastMsg = msg;
        m_lastMsg.firstTime = msg.lastTime;
    } else {
        m_lastMsg.repeat++;
        m_lastMsg.lastTime = msg.lastTime;
        if ((m_lastMsg.lastTime - m_lastMsg.firstTime) >= msgCollateTime) {
            // repeated same message for too long -> print out message
            append(m_lastMsg);
            m_lastMsg.repeat = 0;
            m_lastMsg.firstTime = m_lastMsg.lastTime;
        }
    }
#ifdef QT_DEBUG
    //fprintf(stderr, "--- TMessageHandler::addMessage()\n");
#endif
}

void TMessageHandler::saveMessagesOnQuit()
{
    saveMessages(m_filename, true);
    m_finallySaved = true;
}

void TMessageHandler::saveMessages(const QString &fileName, bool includeDebug)
{
#ifdef QT_DEBUG
    fprintf(stderr, "+++ TMessageHandler::saveMessages(fileName=\"%s\")\n", qPrintable(fileName));
#endif
    if (m_numMessages) {
        QFile f(fileName);
        if (f.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream t(&f);
            int inx = m_msgStart;
            for (auto n=0; n<m_numMessages; ++n) {
                MSG_ENTRY *m = &m_msg[inx];
                QString s = QString::fromUtf8(m->text);
                if (includeDebug || !s.contains("DBUG")) {
                    //t << QDateTime::fromMSecsSinceEpoch(m->lastTime).toString("[yyyy-MM-dd hh:mm:ss.zzz] ") + m->text;
                    t << s;
                    if (m->repeat > 1) {
                        t << " (repeated " << m->repeat << " times";
                        uint dT = m->lastTime - m->firstTime;
                        if (dT)
                            t << ", within last " << dT << " seconds)";
                        else
                            t << ")";
                    }
                    t << Qt::endl;
                }
                ++inx;
                if (inx==MESSAGE_LIMIT) {
                    inx = 0;
                }
            }
            f.close();
            emit messageSaved();
#ifdef QT_DEBUG
            fprintf(stderr, "    TMessageHandler::saveMessages(): messageSaved!\n");
#endif
        }
    }
#ifdef QT_DEBUG
    fprintf(stderr, "--- TMessageHandler::saveMessages()\n");
#endif
}

void TMessageHandler::clearMessages()
{
    m_msgNext = m_msgStart;
    m_numMessages = 0;
    emit messageCleared();
}

void TMessageHandler::append(const MSG_ENTRY &msg)
{
    m_msg[m_msgNext++] = msg;
    if (m_msgNext == MESSAGE_LIMIT) {
        m_msgNext = 0;
    }
    if (m_msgStart==m_msgNext) {
        ++m_msgStart;
        if (m_msgStart == MESSAGE_LIMIT) {
            m_msgStart = 0;
        }
    }
    m_numMessages = qMin(m_numMessages+1, MESSAGE_LIMIT);
    emit messageAdded(QString::fromUtf8(msg.text));
}

QString TMessageHandler::filename() const
{
    return m_filename;
}

void TMessageHandler::setFilename(const QString &filename)
{
    m_filename = filename;
}

TMessageHandler::MSG_ENTRY::MSG_ENTRY()
    : repeat(0)
    , firstTime(0)
    , lastTime(0)
{
    memset(text, '\0', sizeof(text));
}

TMessageHandler::MSG_ENTRY::MSG_ENTRY(const MSG_ENTRY *other)
    : repeat(other->repeat)
    , firstTime(other->firstTime)
    , lastTime(other->lastTime)
{
    memcpy(text, other->text, sizeof(text));
}
