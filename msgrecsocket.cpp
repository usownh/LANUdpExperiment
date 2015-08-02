#include "msgrecsocket.h"

msgRecSocket::msgRecSocket(QObject *parent) :
    udpSocket(parent)
{
}



//接收消息并添加到共享内存QList中
void msgRecSocket::receiveToList()
{

    message=this->receive();
    qDebug()<<"received from"<<message.ip<<"  content:"<<message.content;
    QMutexLocker locker(mutexMsgRec);
    msgReadyToProcess->append(message);
    locker.unlock();
    emit msgReadyToRead();
}

//监听端口并持续接收
void msgRecSocket::run()
{
    this->init();
    this->bindport();
    while(true)
    {
        this->receiveToList();
        this->sleep(0);
    }
}

void msgRecSocket::startWithPort(int p)
{
    port=p;
    this->start();
}
