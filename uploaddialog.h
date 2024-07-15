#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QDialog>
#include"packdef.h"
#include<QFileDialog>
#include<QFileInfo>
#include<QDebug>
#include<QProcess>
#include<QString>
#include<QDir>
#include<QThread>
#include<QVBoxLayout>
#include<list>
#include<QList>
#include"hobbylinedialog.h"
#include"logindialog.h"
using namespace std;
namespace Ui {
class UpLoadDialog;
}
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include"libswresample/swresample.h"
#include"libavutil/time.h"
}
class UpLoadDialog;
class SaveJPGWorker:public QThread
{
    Q_OBJECT
public:
    SaveJPGWorker(UpLoadDialog* upload)
    {
        m_upload = upload;
        m_thread = new QThread;
        this->moveToThread(m_thread);
        m_thread->start();
    }
    ~SaveJPGWorker()
    {
        if(m_thread)
        {
            m_thread->quit();
            m_thread->wait(100);
            if(m_thread->isRunning())
            {
                m_thread->terminate();
                m_thread->wait();
            }
            delete m_thread;
        }
        if(m_upload)
            delete m_upload;
    }
private slots:
    void slot_work(QString path);
signals:
    SIG_setUI(QString path, QString imgPath);
private:
    UpLoadDialog* m_upload;
    QThread* m_thread;

};

class UpLoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpLoadDialog(QWidget *parent = nullptr);
    ~UpLoadDialog();

private slots:
    void on_pb_beginUpload_clicked();

    void on_pb_view_check_clicked();
    void on_pb_view_clicked();

    void slot_setProcessBar(float p);
    void slot_addItem();
    void on_pb_addItem_clicked();

public slots:
    //void slot_setProcess(qint64 cur, qint64 max);
    void slot_setUI(QString path, QString imgPath);
    void slot_deleteHobby(HobbyLineDialog* hobbyDialog);
signals:
    SIG_GetOnePicture(QImage img);
    SIG_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby);
    SIG_openRecorder();
    SIG_saveJPG(QString path);
private:
    QString SaveVideoJpg(QString FilePath);
public:
    void Clear();
private:
    Ui::UpLoadDialog *ui;
    QString m_filePath;
    QString m_imgPath;
    SaveJPGWorker* m_saveJPGWork;
    friend class SaveJPGWorker;
    QVBoxLayout* m_pLayOut;
    QList<HobbyLineDialog*> list_hobbyLineDialog;
};

#endif // UPLOADDIALOG_H
