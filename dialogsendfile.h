#ifndef DIALOGSENDFILE_H
#define DIALOGSENDFILE_H

#include <QDialog>

namespace Ui {
class DialogSendFile;
}

class DialogSendFile : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSendFile(QWidget *parent = 0);
    ~DialogSendFile();
    void setShow(QString show);
signals:
    void cancel();
private slots:
    void on_pushButtonCancel_clicked();
    void updateProgress(qint64 alreadySend,qint64 total);
    void sendSuccess();
    void timerEvent(QTimerEvent *);

private:
    void closeEvent(QCloseEvent *);
    Ui::DialogSendFile *ui;
    qint64 send1,send2;
};

#endif // DIALOGSENDFILE_H
