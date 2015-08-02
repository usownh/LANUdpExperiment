#ifndef MSGRECSOCKET_H
#define MSGRECSOCKET_H

#include <QThread>
#include <msg.h>
#include <winsock2.h>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <udpsocket.h>

class msgRecSocket : public udpSocket
{
    Q_OBJECT
public:
    explicit msgRecSocket(QObject *parent = 0);
    void startWithPort(int p);
    void setMsgList(QList<msg> *msgToProcess)
        {msgReadyToProcess=msgToProcess;}
    void setMutex(QMutex *m){mutexMsgRec=m;}
private:
    void receiveToList();
    void run();
signals:
    void msgReadyToRead();
    
public slots:
private:
    QList<msg> *msgReadyToProcess;
    msg message;
    QMutex *mutexMsgRec;
};

#endif // MSGRECSOCKET_H
