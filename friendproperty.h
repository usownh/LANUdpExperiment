#ifndef FRIENDPROPERTY_H
#define FRIENDPROPERTY_H

#include <QObject>
#include <QStandardItemModel>
#include <QMap>
#include <QDebug>

class friendProperty : public QObject
{
    Q_OBJECT
public:
    explicit friendProperty(QObject *parent = 0);
    void add(QString ip,QString nickName);
    void remove(QString ip);
    QString findNickName(QString ip){return ipNickNameMap[ip];}
signals:
    void isAlive(QString ip);
public slots:
private slots:
    void timerEvent(QTimerEvent *);
public:
    QStandardItemModel *tableViewModel;
private:
    QMap<QString,int> aliveCountMap;
    QMap<QString,QString> ipNickNameMap;
    int timeInterval;
};

#endif // FRIENDPROPERTY_H
