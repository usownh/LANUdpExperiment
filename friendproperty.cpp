#include "friendproperty.h"

friendProperty::friendProperty(QObject *parent) :
    QObject(parent)
{
    tableViewModel=  new QStandardItemModel;
    tableViewModel->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("昵称")));
    tableViewModel->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("IP")));
    timeInterval=1;
    this->startTimer(1000*timeInterval);
}


//添加在线主机
void friendProperty::add(QString ip, QString nickName)
{
    if(aliveCountMap.contains(ip))
    {
        aliveCountMap[ip]=10;
        if (nickName!="")
        {
            ipNickNameMap[ip]=nickName;
            tableViewModel->setItem(tableViewModel->findItems(ip,Qt::MatchExactly,1).first()->row(),0,new QStandardItem(nickName));
        }
        return;
    }
    tableViewModel->setItem(aliveCountMap.size(), 0, new QStandardItem(nickName));
    tableViewModel->setItem(aliveCountMap.size(), 1, new QStandardItem(ip));
    aliveCountMap.insert(ip,10);
    ipNickNameMap.insert(ip,nickName);
}


//根据IP移除在线主机
void friendProperty::remove(QString ip)
{
    if(aliveCountMap.contains(ip))
    {
        aliveCountMap.remove(ip);
        ipNickNameMap.remove(ip);
        tableViewModel->removeRow(tableViewModel->findItems(ip,Qt::MatchExactly,1).first()->row());
    }
}

//定时器，当列表中的主机的计数值为1时，询问是否存活，小于0时则直接移除
void friendProperty::timerEvent(QTimerEvent *)
{
    if(aliveCountMap.isEmpty()) return;
    QMap<QString,int>::iterator it;
    for ( it = aliveCountMap.begin(); it != aliveCountMap.end(); it++ )
    {
        it.value()--;
        if(it.value()==1) emit isAlive(it.key());
        else if(it.value()<0)
        {
            QString ip=it.key();
            it--;
            remove(ip);
        }
    }
}

