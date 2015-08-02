#include "filerecsocket.h"

fileRecSocket::fileRecSocket(QObject *parent) :
    udpSocket(parent)
{

}


//content  filepart+4Bytes+mor/end+...
//接收文件，把收到的文件放入QMap缓冲区里
void fileRecSocket::receiveFile()
{
    message=this->receive();
    //抛弃不是filepart开头的数据包
    if(message.content.startsWith("filepart"))
    {
        ip=message.ip;
        int i=bytesToInt(message.content.mid(8,4));
        if(i<offset)
        {
            count++;
            //每抛弃1000个已接收过的重复数据包，就重发一次文件期望的偏移
            if(count%1000==0)  emit receiveFileOffsetExpected(ip,recOffset);
            return;
        }
        fileReadyToProcess.insert(i,message.content.mid(12));
    }
}

void fileRecSocket::run()
{
    recOffset=0;
    count=0;
    this->init();
    this->bindport();
    file.setFileName(fileName);
    offset=0;
    qint64 received=0;
    file.open(QIODevice::Append);
    while(true)
    {
        this->receiveFile();
        //处理缓冲区的数据
        if(fileReadyToProcess.contains(offset))
        {
            //找到已接收到的最大的连续的文件偏移，并发出文件偏移期望
            for(recOffset=offset;fileReadyToProcess.contains(recOffset);recOffset++);
            emit receiveFileOffsetExpected(ip,recOffset);

            //将前面连续的数据写入文件
            for(;offset<recOffset;offset++)
            {
                received=received+fileReadyToProcess[offset].size()-3;
                emit progress(received);
                file.write(fileReadyToProcess[offset].mid(3));
                if(fileReadyToProcess[offset].mid(0,3)=="end")
                {
                    file.close();
                    fileReadyToProcess.clear();
                    qDebug()<<"文件接收成功";
                    this->close();
                    return;
                }
                fileReadyToProcess.remove(offset);
            }
        }
    }
}




