#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMessageBox>
#include <QtNetwork/QHostAddress>
#include <QFile>
#include <QTime>
#include <QFontDialog>
#include <QColorDialog>
#include <QMutex>
#include <QMutexLocker>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QHostInfo>
#include <friendproperty.h>
#include <msgrecsocket.h>
#include <msgsendsocket.h>
#include <filerecsocket.h>
#include <filesendsocket.h>
#include <dialogrecfile.h>
#include <dialogsendfile.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QString broadcastIP();
    void sendUdpMessage(QString dst,QByteArray content);
    QByteArray intToByte(int i);
    int bytesToInt(QByteArray bytes);
    void closeEvent(QCloseEvent *);
signals:
private slots:
    void processMsg();
    void isAlive(QString ip);
    void cancelSendFile();
    void rejectfile(QString ip);
    void startRec(QString ip,QString filename);
    void receiveFileOffsetExpected(QString ip,int o);

    void on_pushButtonSend_clicked();
    void on_pushButtonSendFile_clicked();
    void on_pushButtonFont_clicked();
    void on_lineEditNickName_editingFinished();
    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButtonColor_clicked();

    void on_comboBoxDevice_currentIndexChanged(int index);

    void on_lineEditDstIP_editingFinished();


private:
    Ui::MainWindow *ui;
    int offset;
    msgRecSocket messageRecSocket;
    msgSendSocket messageSendSocket;
    fileRecSocket fileReceiveSocket;
    fileSendSocket fileSendOutSocket;
    QString localNickName;
    QString onlineMsg;
    msg  message;
    friendProperty friends;
    QList<msg> *msgReadyToProcess,*msgReadyToSend;
    DialogSendFile senddialog;
    DialogRecFile recdialog;
    QFile file;
    QMutex mutexMsgRec,mutexMsgSend;
    QList<QNetworkInterface> deviceList;
    QList<QNetworkAddressEntry> entryList ;
    QList<QString> deviceName,deviceIP,DeviceBroadcast;
    int oldindex,oldindex2;
    bool comboxInit;
};

#endif // MAINWINDOW_H
