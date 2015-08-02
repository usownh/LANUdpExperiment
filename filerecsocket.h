#ifndef FILERECSOCKET_H
#define FILERECSOCKET_H

#include <QThread>
#include <msg.h>
#include <winsock2.h>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <udpsocket.h>

class fileRecSocket : public udpSocket
{
    Q_OBJECT
public:
    explicit fileRecSocket(QObject *parent = 0);
    void setFileName(QString name){fileName=name;}
private:
    void run();
    void receiveFile();
signals:
    void progress(qint64);
    void cancel(QString);
    void receiveFileOffsetExpected(QString,int);
public slots:
private:
    QString fileName;
    QString ip;
    int offset;
    QMap<int,QByteArray> fileReadyToProcess;
    msg message;
    QFile file;
    int recOffset;
    int count;
};

#endif // FILERECSOCKET_H
