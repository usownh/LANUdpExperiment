#include "filesendsocket.h"

fileSendSocket::fileSendSocket(QObject *parent) :
    udpSocket(parent)
{
}

void fileSendSocket::run()
{
    this->init();
    file.setFileName(fileName);
    file.open(QIODevice::ReadOnly);
    qint64 totalpart=0;
    qint64 alreadySend=0;
    qint64 filesize;
    int fileOffset=0;
    filesize=file.size();
    totalpart= file.size()/1400;
    if(file.size()%1400==0) totalpart--;
    qDebug()<<"文件分块："<<totalpart;
    while(true)
    {
        //-1为文件发送终止标记
        if(*offset==-1)
        {
            file.close();
            this->close();
            return;
        }
        //文件读取完成并且缓冲区已经清空，则发送成功
        if(file.atEnd()&&fileReadyToSend.isEmpty())
        {
            file.close();
            emit sendSuccess();
            this->close();
            return;
        }
        if(!fileReadyToSend.isEmpty())
        {
            //移除已经确认收到的包
            for(int i=*offset-1000;i<*offset;i++)
            {
                if(*offset==0) break;
                if(i<0) i=0;
                if(fileReadyToSend.contains(i)) fileReadyToSend.remove(i);
            }
        }
        if(fileReadyToSend.size()<1000)
        {
            //构造要发送的数据包，并放入缓冲区
            for(int i=0;!file.atEnd()&&i<100;i++)
            {
                content.clear();
                content.append("filepart");
                content.append(intToByte(fileOffset));
                if(fileOffset<totalpart) content.append("mor");
                else content.append("end");
                content.append(file.read(1400));
                alreadySend+=1400;
                if(alreadySend>filesize) alreadySend=filesize;
                fileReadyToSend.insert(fileOffset,content);
                fileOffset++;
                emit progress(alreadySend,filesize);
            }
        }

        this->sendFile();
        this->sleep(0);
    }
}


void fileSendSocket::sendFile()
{
    //发送期望偏移后的1000个数据包
    for(int i=0;i<1000;i++)
    {
        if(fileReadyToSend.contains(*offset+i))
        {
            this->send(dst,fileReadyToSend[*offset+i]);
        }
    }
}
