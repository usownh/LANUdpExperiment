#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QThread>
#include <QByteArray>
#include <msg.h>
#include <winsock2.h>
#include <QDebug>

class udpSocket : public QThread
{
    Q_OBJECT
public:
    explicit udpSocket(QObject *parent = 0);
    QByteArray intToByte(int i);
    int bytesToInt(QByteArray bytes);
    void setPort(int p){port=p;}
protected:
    bool init();
    bool bindport();
    void send(QString dst,QByteArray content);
    msg receive();
    bool close();
signals:
    
public slots:
protected:
    int port;
    QString errmsg;
    SOCKET sock;
};

#endif // UDPSOCKET_H
