#ifndef MSG_H
#define MSG_H

#include <QByteArray>

#define UDP_PORT   6000
#define UDP_FILE_PORT 6001


struct msg
{
    QString ip;
    QByteArray content;
};
#endif // MSG_H



