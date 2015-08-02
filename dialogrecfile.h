#ifndef DIALOGRECFILE_H
#define DIALOGRECFILE_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QFile>

namespace Ui {
class DialogRecFile;
}

class DialogRecFile : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogRecFile(QWidget *parent = 0);
    ~DialogRecFile();
    void setShow(QString show);
    void setInfo(QString IP,QString filename,qint64 filesize){ip=IP;fileName=filename;size=filesize;}
    void show();
private:
    void closeEvent(QCloseEvent *);
    void clear();
signals:
    void accept(QString src,QString filename);
    void reject(QString);
private slots:
    void on_pushButtonReject_clicked();

    void on_pushButtonAccept_clicked();

    void on_pushButtonSaveAs_clicked();
    void updateProgress(qint64 received);
    void timerEvent(QTimerEvent *);

private:
    Ui::DialogRecFile *ui;
    QString ip;
    QString fileName;
    qint64 size;
    qint64 rec1,rec2;
};

#endif // DIALOGRECFILE_H
