#include "msgsendsocket.h"

msgSendSocket::msgSendSocket(QObject *parent) :
    udpSocket(parent)
{
}

//发送List里有消息就发送出去
void msgSendSocket::run()
{
    this->init();
    while(true)
    {
        if(!msgReadyToSend->isEmpty()) this->sendList();
        this->sleep(0);
    }
}


//发送List中的消息
void msgSendSocket::sendList()
{
    QMutexLocker locker(mutexMsgSend);
    while(!msgReadyToSend->isEmpty())
    {
        this->send(msgReadyToSend->first().ip,msgReadyToSend->first().content.data());
        msgReadyToSend->removeFirst();
    }
    locker.unlock();
}

