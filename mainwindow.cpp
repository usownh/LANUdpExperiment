#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    oldindex=0;
    this->setWindowTitle("UDP通信实验程序");
    ui->tableView->setModel(friends.tableViewModel);
    ui->lineEditNickName->setText(QHostInfo::localHostName());
    //发送信息的快捷键设置为Ctrl+Enter
    ui->pushButtonSend->setShortcut(Qt::CTRL+Qt::Key_Return);
    //初始化设备列表
    comboxInit=true;
    deviceList=QNetworkInterface::allInterfaces();
    while(!deviceList.isEmpty())
    {
        entryList =deviceList.first().addressEntries();
        if(entryList.isEmpty())
        {
            deviceList.removeFirst();
            continue;
        }
        for(int i=0;i<entryList.size();i++)
        {
            if(entryList.at(i).broadcast().toString()!="")
            {
                deviceName.append(deviceList.first().humanReadableName());
                deviceIP.append(entryList.at(i).ip().toString());
                DeviceBroadcast.append(entryList.at(i).broadcast().toString());
                ui->comboBoxDevice->addItem(deviceList.first().humanReadableName()+"  "+entryList.at(i).ip().toString());
            }
        }
        deviceList.removeFirst();
    }
    comboxInit=false;

    connect(&friends,SIGNAL(isAlive(QString)),this,SLOT(isAlive(QString)));
    connect(&messageRecSocket,SIGNAL(msgReadyToRead()),this,SLOT(processMsg()));
    connect(&senddialog,SIGNAL(cancel()),this,SLOT(cancelSendFile()));
    connect(&recdialog,SIGNAL(reject(QString)),this,SLOT(rejectfile(QString)));
    connect(&recdialog,SIGNAL(accept(QString,QString)),this,SLOT(startRec(QString,QString)));
    connect(&fileReceiveSocket,SIGNAL(progress(qint64)),&recdialog,SLOT(updateProgress(qint64)));
    connect(&fileReceiveSocket,SIGNAL(cancel(QString)),this,SLOT(rejectfile(QString)));
    connect(&fileReceiveSocket,SIGNAL(receiveFileOffsetExpected(QString,int)),this,SLOT(receiveFileOffsetExpected(QString,int)));
    connect(&fileSendOutSocket,SIGNAL(progress(qint64,qint64)),&senddialog,SLOT(updateProgress(qint64,qint64)));
    connect(&fileSendOutSocket,SIGNAL(sendSuccess()),&senddialog,SLOT(sendSuccess()));


    msgReadyToProcess = new QList<msg>;
    msgReadyToSend = new QList<msg>;

    //初始化信息、文件发送接收的socket
    messageRecSocket.setMsgList(msgReadyToProcess);
    messageRecSocket.startWithPort(UDP_PORT);
    messageRecSocket.setMutex(&mutexMsgRec);
    messageSendSocket.setMsgList(msgReadyToSend);
    messageSendSocket.setPort(UDP_PORT);
    messageSendSocket.setMutex(&mutexMsgSend);
    messageSendSocket.start();


    fileSendOutSocket.setOffset(&offset);
    fileSendOutSocket.setPort(UDP_FILE_PORT);
    offset=0;

    fileReceiveSocket.setPort(UDP_FILE_PORT);



    localNickName=ui->lineEditNickName->text();
    onlineMsg="onlinenickname"+localNickName+";";
    //默认选中第一个设备
    this->on_comboBoxDevice_currentIndexChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    //下线
    sendUdpMessage(this->broadcastIP(),"offline");
    //结束所有子线程
    messageRecSocket.terminate();
    messageSendSocket.terminate();
    if(fileSendOutSocket.isRunning()) fileSendOutSocket.terminate();
    if(fileReceiveSocket.isRunning()) fileReceiveSocket.terminate();
}

//发送UDP消息
void MainWindow::sendUdpMessage(QString dst, QByteArray content)
{
    message.content=content;
    message.ip=dst;
    //对共享内存加锁
    QMutexLocker locker(&mutexMsgSend);
    msgReadyToSend->append(message);
    locker.unlock();
}
void MainWindow::on_pushButtonSend_clicked()
{
    //若没有消息则不发送
    if(ui->textEdit->toPlainText()=="")
    {
        return;
    }
    //消息过长不发送
    if(ui->textEdit->toPlainText().length()>800)
    {
        QMessageBox msgBox;
        QString tmp;
        tmp.setNum(ui->textEdit->toPlainText().length());
        msgBox.setText("内容过长！请限制字节数在少于800！\r\n当前字节数为："+tmp);
        msgBox.exec();
        return;
    }
    if(ui->textEdit->toHtml().length()>1400)
    {
        QMessageBox msgBox;
        QString tmp;
        tmp.setNum(ui->textEdit->toHtml().length());
        msgBox.setText("内容过长或格式太复杂！请限制总字节数在少于1400！\r\n当前字节数为："+tmp);
        msgBox.exec();
        return;
    }
    QHostAddress address;
    address.setAddress(ui->lineEditDstIP->text());
    //发送使用html标签的消息
    sendUdpMessage(address.toString(),"msg"+ui->textEdit->toHtml().toLocal8Bit());
    ui->textBrowser->append("<html>发到："+address.toString()+" "+friends.findNickName(address.toString())+"   时间："+QTime::currentTime().toString()+"</html>");
    ui->textBrowser->append(ui->textEdit->toHtml().toLocal8Bit());
    ui->textEdit->clear();
}

//广播IP
QString MainWindow::broadcastIP()
{
    return DeviceBroadcast.at(ui->comboBoxDevice->currentIndex());
}

//询问是否存活
void MainWindow::isAlive(QString ip)
{
    sendUdpMessage(ip,"alive?");
}

//取消文件发送
void MainWindow::cancelSendFile()
{
    offset=-1;
}

//拒绝接收文件
void MainWindow::rejectfile(QString ip)
{
    sendUdpMessage(ip,"rejectfile");
}

//开始接收文件
void MainWindow::startRec(QString ip, QString filename)
{
    fileReceiveSocket.setFileName(filename);
    fileReceiveSocket.start();
    sendUdpMessage(ip,"acceptfile");
}

//发送文件期望偏移
void MainWindow::receiveFileOffsetExpected(QString ip, int o)
{
    sendUdpMessage(ip,"fileok"+intToByte(o));
}

//处理收到的消息
void MainWindow::processMsg()
{
    //访问共享内存，信号量加锁
    QMutexLocker locker(&mutexMsgRec);
    while(!msgReadyToProcess->isEmpty())
    {
        QString src=msgReadyToProcess->first().ip;
        //常规消息
        if(msgReadyToProcess->first().content.startsWith("msg"))
        {
            msgReadyToProcess->first().content.remove(0,3);
            ui->textBrowser->append("<html>来自："+src+" "+friends.findNickName(src)+"   时间："+QTime::currentTime().toString()+"</html>");
            ui->textBrowser->append(QString(msgReadyToProcess->first().content));
        }
        //上线
        else if(msgReadyToProcess->first().content.startsWith("firstonline"))
        {
            msgReadyToProcess->first().content.remove(0,11);
            QString nickName;
            if(msgReadyToProcess->first().content.startsWith("nickname"))
            {
                msgReadyToProcess->first().content.remove(0,8);
                nickName=msgReadyToProcess->first().content.left(msgReadyToProcess->first().content.indexOf(";"));
            }
            friends.add(src,nickName);
            if(src!=deviceIP.at(oldindex))
                ui->textBrowser->append(src+" "+nickName+"上线了!");
            sendUdpMessage(src,onlineMsg.toLocal8Bit());
        }
        //在线
        else if(msgReadyToProcess->first().content.startsWith("online"))
        {
            msgReadyToProcess->first().content.remove(0,6);
            QString nickName;
            if(msgReadyToProcess->first().content.startsWith("nickname"))
            {
                msgReadyToProcess->first().content.remove(0,8);
                nickName=msgReadyToProcess->first().content.left(msgReadyToProcess->first().content.indexOf(";"));
            }
            friends.add(src,nickName);
        }
        //下线
        else if(msgReadyToProcess->first().content.startsWith("offline"))
        {
            if(src!=deviceIP.at(oldindex2))
                ui->textBrowser->append(src+" "+friends.findNickName(src)+"下线了!");
            friends.remove(src);
        }
        //询问存活
        else if(msgReadyToProcess->first().content.startsWith("alive?"))
        {
            sendUdpMessage(broadcastIP(),onlineMsg.toLocal8Bit());
        }
        //发送文件
        else if (msgReadyToProcess->first().content.startsWith("sendfile"))
        {
            msgReadyToProcess->first().content.remove(0,8);
            QString name,size;
            if(msgReadyToProcess->first().content.startsWith("filename"))
            {
                msgReadyToProcess->first().content.remove(0,8);
                name=msgReadyToProcess->first().content.left(msgReadyToProcess->first().content.indexOf(";"));
                msgReadyToProcess->first().content.remove(0,msgReadyToProcess->first().content.indexOf(";")+1);
            }
            if(msgReadyToProcess->first().content.startsWith("filesize"))
            {
                msgReadyToProcess->first().content.remove(0,8);
                size=msgReadyToProcess->first().content.left(msgReadyToProcess->first().content.indexOf(";"));
            }
            if (fileReceiveSocket.isRunning())
            {
                sendUdpMessage(src,"busy");
            }
            else
            {
                offset=0;
                recdialog.setShow(src+"请求发送文件："+name+",文件大小："+size+"字节。是否接收？");
                recdialog.setInfo(src,name,size.toLongLong());
                recdialog.show();
            }
        }
        //同意接收文件
        else if(msgReadyToProcess->first().content.startsWith("acceptfile"))
        {
            if (!fileSendOutSocket.isRunning()) fileSendOutSocket.start();
        }
        //主机忙
        else if(msgReadyToProcess->first().content.startsWith("busy"))
        {
            senddialog.setShow("目标主机正忙...");
        }
        //拒绝接收文件
        else if(msgReadyToProcess->first().content.startsWith("rejectfile"))
        {
            senddialog.setShow("文件发送请求被拒绝！");
        }
        //文件期望偏移
        else if(msgReadyToProcess->first().content.startsWith("fileok"))
        {
            int i=bytesToInt(msgReadyToProcess->first().content.mid(6,4));
            if(offset<i) offset=i;
        }
        msgReadyToProcess->removeFirst();
    }
    locker.unlock();
}


//发送文件准备，告知目标要发送的文件信息
void MainWindow::on_pushButtonSendFile_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this,
         tr("请选择要发送的文件"), "/", tr("文件(*)"));
    if(fileName.isEmpty()) return;
    file.setFileName(fileName);
    QString size;
    size.setNum(file.size());
    QString name=fileName.mid(fileName.lastIndexOf("/")+1);
    fileSendOutSocket.setDstAndFile(ui->lineEditDstIP->text(),fileName);
    sendUdpMessage(ui->lineEditDstIP->text(),"sendfilefilename"+name.toLocal8Bit()+";filesize"+size.toLocal8Bit()+";");
    senddialog.setShow("准备发送文件"+name+",等待对方接受请求");
    senddialog.show();
}

//修改字体
void MainWindow::on_pushButtonFont_clicked()
{
    ui->textEdit->setFont(QFontDialog::getFont(0,ui->textEdit->font()));
}

//修改昵称
void MainWindow::on_lineEditNickName_editingFinished()
{
    if(localNickName==ui->lineEditNickName->text()) return;
    localNickName=ui->lineEditNickName->text();
    onlineMsg="onlinenickname"+localNickName+";";
    sendUdpMessage(broadcastIP(),onlineMsg.toLocal8Bit());
}

//点击在线主机，自动切换目标IP
void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    ui->tableView->selectRow(index.row());
    ui->lineEditDstIP->setText(ui->tableView->selectionModel()->selectedRows(1).first().data().toString());
}

//修改颜色
void MainWindow::on_pushButtonColor_clicked()
{
    ui->textEdit->setTextColor(QColorDialog::getColor(ui->textEdit->textColor()));
}

QByteArray MainWindow::intToByte(int i)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar)  (0x000000ff & i);
    abyte0[1] = (uchar) ((0x0000ff00 & i) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & i) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & i) >> 24);
    return abyte0;
}

int MainWindow::bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

//选择设备
void MainWindow::on_comboBoxDevice_currentIndexChanged(int index)
{
    if(comboxInit) return;
    QString firstOnline="firstonlinenickname"+localNickName+";";
    sendUdpMessage(DeviceBroadcast.at(oldindex),"offline");
    ui->lineEditDstIP->setText(deviceIP.at(index));
    sendUdpMessage(broadcastIP(),firstOnline.toLocal8Bit());
    oldindex2=oldindex;
    oldindex=index;
}

//编辑目标IP
void MainWindow::on_lineEditDstIP_editingFinished()
{
    QString firstOnline="firstonlinenickname"+localNickName+";";
    sendUdpMessage(ui->lineEditDstIP->text(),firstOnline.toLocal8Bit());
}
