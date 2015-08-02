#include "dialogrecfile.h"
#include "ui_dialogrecfile.h"

DialogRecFile::DialogRecFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRecFile)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    rec1=rec2=0;
    startTimer(500);
    this->setWindowTitle("接收文件");
}

DialogRecFile::~DialogRecFile()
{
    delete ui;
}

void DialogRecFile::clear()
{
    ui->pushButtonAccept->setVisible(true);
    ui->pushButtonSaveAs->setVisible(true);
    rec1=rec2=0;
    this->setWindowTitle("接收文件");
}

//定时器，用来计算文件传输速度
void DialogRecFile::timerEvent(QTimerEvent *e)
{
    if(rec2==0) return;
    QString speed;
    speed.setNum((rec2-rec1)*2/1024);
    this->setWindowTitle("接收速度"+speed+"KB/s");
    rec1=rec2;
}

void DialogRecFile::setShow(QString show)
{
    ui->label->setText(show);
}

void DialogRecFile::show()
{
    ui->pushButtonAccept->setDisabled(false);
    ui->pushButtonSaveAs->setDisabled(false);
    ui->pushButtonReject->setDisabled(false);
    ui->progressBar->setVisible(false);
    QDialog::show();
}

void DialogRecFile::closeEvent(QCloseEvent *)
{
    if(ui->label->text()!="文件接收成功！")  emit reject(ip);
    this->close();
    ui->pushButtonReject->setText("拒绝");
    this->clear();
}


//拒绝接收按钮
void DialogRecFile::on_pushButtonReject_clicked()
{
    if(ui->label->text()!="文件接收成功！") emit reject(ip);
    this->close();
    ui->pushButtonReject->setText("拒绝");
    this->clear();
}

//同意接收文件
void DialogRecFile::on_pushButtonAccept_clicked()
{
    QFile file;
    file.setFileName(fileName);
    //同名文件自动重命名
    if(file.exists())
    {
        int i=1;
        QString tmp;
        do
        {
            file.setFileName(fileName+"("+tmp.setNum(i)+")");
            i++;
        }while(file.exists());
        fileName=fileName+"("+tmp.setNum(--i)+")";
    }
    emit accept(ip,fileName);
    ui->pushButtonAccept->setVisible(false);
    ui->pushButtonSaveAs->setVisible(false);
}

//另存为
void DialogRecFile::on_pushButtonSaveAs_clicked()
{
    fileName=QFileDialog::getSaveFileName(this,"请选择要保存的文件位置","/"+fileName,"文件(*)");
    emit accept(ip,fileName);
    ui->pushButtonAccept->setVisible(false);
    ui->pushButtonSaveAs->setVisible(false);
}

//更新进度条
void DialogRecFile::updateProgress(qint64 received)
{
    rec2=received;
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(100*received/size);
    QString rec,total;
    rec.setNum(received);
    total.setNum(size);
    if(received>1024*1024)
    {
        rec.setNum(received/(1024*1024));
        rec+="MB";
    }
    else if(received>1024)
    {
        rec.setNum(received/1024);
        rec+="KB";
    }
    else
    {
        rec.setNum(received);
        rec+="B";
    }
    if(size>1024*1024)
    {
        total.setNum(size/(1024*1024));
        total+="MB";
    }
    else if(size>1024)
    {
        total.setNum(size/1024);
        total+="KB";
    }
    else
    {
        total.setNum(size);
        total+="B";
    }
    ui->label->setText(fileName+"接收中，已接收"+rec+"，总共"+total+"！");
    if(received==size)
    {
        ui->label->setText("文件接收成功！");
        rec1=rec2=0;
        this->setWindowTitle("接收文件");
    }
    ui->pushButtonReject->setText("确定");
}
