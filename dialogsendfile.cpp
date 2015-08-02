#include "dialogsendfile.h"
#include "ui_dialogsendfile.h"

DialogSendFile::DialogSendFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSendFile)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    send1=send2=0;
    this->setWindowTitle("发送文件");
    startTimer(500);
}

DialogSendFile::~DialogSendFile()
{
    delete ui;
}

void DialogSendFile::closeEvent(QCloseEvent *)
{
    this->close();
    ui->progressBar->setValue(0);
    send1=send2=0;
    this->setWindowTitle("发送文件");
}


//取消文件发送
void DialogSendFile::on_pushButtonCancel_clicked()
{
    if(ui->label->text()!="文件发送成功！")  emit cancel();
    this->close();
    ui->progressBar->setValue(0);
    ui->pushButtonCancel->setText("取消");
    send1=send2=0;
    this->setWindowTitle("发送文件");
}

//定时器，计算文件传输速度
void DialogSendFile::timerEvent(QTimerEvent *e)
{
    if(send2==0) return;
    QString s;
    qint64 speed=(send2-send1)*2/1024;
    s.setNum(speed);
    this->setWindowTitle("传输速度"+s+"KB/s");
    send1=send2;
}

//更新进度条
void DialogSendFile::updateProgress(qint64 alreadySend, qint64 total)
{
    send2=alreadySend;
    int progress;
    if(alreadySend==total) progress=100;
    else progress=alreadySend*100/total;
    ui->progressBar->setValue(progress);
    QString tmp1,tmp2;
    if(alreadySend>1024*1024)
    {
        tmp1.setNum(alreadySend/(1024*1024));
        tmp1+="MB";
    }
    else if(alreadySend>1024)
    {
        tmp1.setNum(alreadySend/1024);
        tmp1+="KB";
    }
    else
    {
        tmp1.setNum(alreadySend);
        tmp1+="B";
    }
    if(total>1024*1024)
    {
        tmp2.setNum(total/(1024*1024));
        tmp2+="MB";
    }
    else if(total>1024)
    {
        tmp2.setNum(total/1024);
        tmp2+="KB";
    }
    else
    {
        tmp2.setNum(total);
        tmp2+="B";
    }
    ui->label->setText("已发送："+tmp1+"/总共需要发送："+tmp2);
}



void DialogSendFile::sendSuccess()
{
    ui->label->setText("文件发送成功！");
    ui->pushButtonCancel->setText("确定");
    send1=send2=0;
    this->setWindowTitle("发送文件");

}

void DialogSendFile::setShow(QString show)
{
    ui->label->setText(show);
}
