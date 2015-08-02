#ifndef FILESENDSOCKET_H
#define FILESENDSOCKET_H

#include <QThread>
#include <msg.h>
#include <winsock2.h>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <udpsocket.h>

class fileSendSocket : public udpSocket
{
    Q_OBJECT
public:
    explicit fileSendSocket(QObject *parent = 0);
    void setOffset(int *o){offset=o;}
    void setDstAndFile(QString ip,QString file)
        {dst=ip;fileName=file;}
private:
    void sendFile();
    void run();
signals:
    void progress(qint64,qint64);
    void sendSuccess();
public slots:
private:
    int *offset;
    QMap<int,QByteArray> fileReadyToSend;
    QByteArray content;
    QFile file;
    QString dst,fileName;
};

#endif // FILESENDSOCKET_H
