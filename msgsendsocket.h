#ifndef MSGSENDSOCKET_H
#define MSGSENDSOCKET_H

#include <QThread>
#include <msg.h>
#include <winsock2.h>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <udpsocket.h>

class msgSendSocket : public udpSocket
{
    Q_OBJECT
public:
    explicit msgSendSocket(QObject *parent = 0);
    void setMsgList(QList<msg> *msgToSend)
        {msgReadyToSend=msgToSend;}
    void setPort(int p){port=p;}
    void setMutex(QMutex *m){mutexMsgSend=m;}

private:
    void sendList();
    void run();

signals:
    
public slots:
private:
    QList<msg> *msgReadyToSend;
    msg message;
    QMutex *mutexMsgSend;
};

#endif // MSGSENDSOCKET_H
