// ***************************************************************************
// General Support Classes
// ---------------------------------------------------------------------------
// tmessagegandler.h, header file
// improved message handler for qDebug() etc.
// maybe included into main.cpp only and only once.
// messagehandler is installed inserting the following lines at
// the very beginning of main():
//   pTMsgHandler = new TMessageHandler();
//   qInstallMessageHandler(_TMessageHandler);
// ---------------------------------------------------------------------------
// Copyright (C) 2021 by t2ft - Thomas Thanner
// ---------------------------------------------------------------------------
// 2021-6-7  tt  Initial version created
// ---------------------------------------------------------------------------
#ifndef TMESSAGEHANDLER_H
#define TMESSAGEHANDLER_H

#include <QObject>
#include <QMetaType>
#include <QStringList>
#include <QDateTime>
#include <QVector>

#define MAX_MSG_LENGTH  1024U

class TMessageHandler : public QObject
{
    Q_OBJECT

public:
    explicit TMessageHandler(const QString &filename, QObject *parent = nullptr);
    ~TMessageHandler();

    QString filename() const;
    void setFilename(const QString &filename);

public slots:
    void addMessage(const QString &text);
    void saveMessagesOnQuit();
    void saveMessages(const QString &fileName, bool includeDebug=false);
    void clearMessages();

signals:
    void messageAdded(const QString &msg);
    void messageSaved();
    void messageCleared();

private:
    class MSG_ENTRY {
    public:
        MSG_ENTRY();
        MSG_ENTRY(const MSG_ENTRY* other);

        char        text[MAX_MSG_LENGTH+1];
        int         repeat;
        qint64      firstTime;
        qint64      lastTime;
    } ;

    QVector<MSG_ENTRY>              m_msg;
    int                             m_msgStart;
    int                             m_msgNext;
    int                             m_numMessages;
    MSG_ENTRY                       m_lastMsg;

    void append(const MSG_ENTRY &msg);

    QString         m_filename;
    bool            m_finallySaved;
};

Q_DECLARE_METATYPE(TMessageHandler*)

#endif // TMESSAGEHANDLER_H
