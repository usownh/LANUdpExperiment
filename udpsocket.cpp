#include "udpsocket.h"

udpSocket::udpSocket(QObject *parent) :
    QThread(parent)
{
}
int udpSocket::bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}
QByteArray udpSocket::intToByte(int i)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar)  (0x000000ff & i);
    abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
    return abyte0;
}

bool udpSocket::init()
{
    errmsg="";
/////加载套接字库(WSAStartup())
    WSAData wsaData;
    if( 0 != WSAStartup(MAKEWORD(2,2),&wsaData) )
    {
        errmsg="LoadWSA failed";
        return false;
    }
    qDebug()<<"LoadWSA success";
/////创建套接字(socket())
    sock=socket(AF_INET,SOCK_DGRAM,0);
    if(INVALID_SOCKET == sock)
    {
        errmsg="CreateSocket falid.";
        return false;
    }
    qDebug()<<"CreateSocket success";
    return true;
}

//绑定端口
bool udpSocket::bindport()
{
    errmsg="";

    SOCKADDR_IN address;
    address.sin_family=AF_INET;
    address.sin_addr.s_addr =inet_addr("0.0.0.0");
    address.sin_port=htons(port);
    memset(address.sin_zero,0,sizeof(address.sin_zero));

    if(SOCKET_ERROR == bind(sock,(const struct sockaddr*)&address,sizeof(struct sockaddr)) )
    {
        errmsg="BindSocket falid.";
        return false;
    }
    qDebug()<<"BindSocket success";
    return true;
}

//发送信息
void udpSocket::send(QString dst, QByteArray content)
{
    SOCKADDR_IN address;
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    memset(address.sin_zero,0,sizeof(address.sin_zero));
    address.sin_addr.s_addr=inet_addr(dst.toLocal8Bit());
    sendto(sock,content.data(),content.length(),0,(const struct sockaddr*)&address,sizeof(struct sockaddr) );
}
//接收信息
msg udpSocket::receive()
{
    msg message;
    SOCKADDR_IN address;
    int n=sizeof(struct sockaddr);
    char buffer[1500] = {0};
    int length;
    length=recvfrom(sock,buffer, 1500,0,(struct sockaddr*)&address,&n);
 //   qDebug()<<buffer<<inet_ntoa(address.sin_addr);
    message.ip=QString(inet_ntoa(address.sin_addr));
    message.content=QByteArray(buffer,length);
    return message;
}

//关闭套接字
bool udpSocket::close()
{
    errmsg="";
    if( closesocket(sock)==SOCKET_ERROR )
    {

        errmsg="CloseSocket falid.";
        return false;
    }
    WSACleanup();
    return true;
}
