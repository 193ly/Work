#ifndef ONLINEDIALOG_H
#define ONLINEDIALOG_H

//#include <QWidget>
#include<QString>
#include"uploaddialog.h"
#include<QObject>
#include<QThread>
#include<QCloseEvent>
#include"qmymovelable.h"
#include"changename.h"
#include"haveuploaddialog.h"
#include<QMovie>
namespace Ui {
class OnlineDialog;
}
//暂时/永久注释
//class UploadWorker: public QObject{
//    Q_OBJECT
//public slots:
//    void slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby);
//};

class OnlineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OnlineDialog(QWidget *parent = nullptr);
    QString getUrl();
    ~OnlineDialog();
    void closeEvent(QCloseEvent *);

    QMyMoveLable* getMovie(QString name);
    void setName(QString name);
    void addHaveVideo(QString name, QMovie* movie, int size, QString rmpt);
    void setProcessBar(float p);

    HaveUploadDialog* findHaveUploadDialog();
signals:
    //SIG_login(QString tel, QString password);
    //SIG_setProcess(int cur, int max);
    SIG_login_show();
    void SIG_rmpt_play(QString);
    void SIG_addHot(QString);
    SIG_upload_show();
    SIG_quit();
    void SIG_changeName(QString);
    SIG_fresh();
    SIG_haveUploaded();
private slots:
    void on_pb_login_clicked();

    void on_pb_rmtpPlay_clicked();

    void on_pb_upload_clicked();

//暂时/永久注释
//    void UpLoadFile(QString path, HOBBY hobby, QString gifName = "");
//public:
//    void slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby);
    void on_pb_tv_clicked();

    void on_pb_recommendTV_clicked();

    void on_pb_fresh_clicked();

    void on_pb_change_name_clicked();

    void on_pb_uploaded_clicked();

    void slot_addHotAndPlay(QString rmpt);
private:
    Ui::OnlineDialog *ui;
    ChangeName* m_changeName;
//暂时/永久注释
//    UpLoadDialog* m_uploadDialog;
//    QThread* m_uploadThread;
//    UploadWorker* m_uploadWorker;
    HaveUploadDialog* m_haveUploadDialog;
    QString rmpt;
public:
    int m_id = 0;
//暂时/永久注释
//public:
//    static OnlineDialog* m_online;
};

#endif // ONLINEDIALOG_H
