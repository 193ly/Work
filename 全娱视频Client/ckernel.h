#ifndef CKERNEL_H
#define CKERNEL_H

#include <QObject>
#include<QThread>
#include"maindialog.h"
#include"TcpServerMediator.h"
#include"TcpClientMediator.h"
#include"logindialog.h"
#include"onlinedialog.h"
#include"uploaddialog.h"
#include"videoitem.h"
#include"livework.h"
#include"qmymovelable.h"
#include"playerdialog.h"
#include"recorderdialog.h"
#include<QMap>
//#define USE_SEVER 1
#define USE_CLIENT 1
//核心处理类
//单例
//1.构造、拷贝、析构的私有化
//2.提供静态的公有的获取对象的方法
#define MD5_KEY 12345
class CKernel;
typedef void (CKernel::*PFun)(unsigned int, char*, int);
class UploadWorker: public QThread{
    Q_OBJECT
    UploadWorker(CKernel* m){
        m_onlin = m;
        m_thread = new QThread;
        this->moveToThread(m_thread);
        m_thread->start();
    }
    ~UploadWorker()
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
    }
public slots:
    void slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby);
    //friend class OnlineDialog;
private:
    CKernel* m_onlin;
    friend class CKernel;
    QThread* m_thread;
};

class CKernel : public QObject
{
    Q_OBJECT
private:
    explicit CKernel(const& CKernel);
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();
    setMap();
signals:
    SIG_login(QString tel, QString password);
    SIG_setProcess(float value); //线程中不能设置控件，采用发送信号的方式

public:
    static CKernel* GetInstance(){
        static CKernel* kernel = new CKernel;
        return kernel;
    }
private slots:
    void slot_destory();

    void slot_ReadyData(unsigned int lSendIp, char* buff, int nLen);
    void slot_login_Rs(unsigned int lSendIp, char* buff, int nLen);
    void slot_resiger_Rs(unsigned int lSendIp, char* buff, int nLen);
    void slot_upload_Rs(unsigned int lSendIp, char* buff, int nLen);
    void slot_downloadRs(unsigned int lSendIp, char* buff, int nLen);
    void slot_fileBlockRs(unsigned int lSendIp, char* buff, int nLen);
    void slot_changNameRs(unsigned int lSendIp, char* buff, int nLen);
    void slot_haveUploadRs(unsigned int lSendIp, char* buff, int nLen);


    void slot_register(QString name, QString tel, QString password, HOBBY);
    void slot_login_Commit(QString tel, QString password);
    void slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby);
    void slot_downRq();
    void slot_changName(QString name);
    void slot_haveUploadRq();
    void slot_addHot(QString rtmp);
    void slot_outLine();
private:
    void UpLoadFile(QString path, HOBBY hobby, QString gifPath = "");
    //void showLogin();
private:
    //MainDialog* m_mainDialog = nullptr;
    TcpClientMediator* m_tcpClient;

    QString m_ip;
    QString m_port;
    loadIniFile();
private:
    LoginDialog* m_loginDialog = nullptr;

    OnlineDialog* m_onlineDialog= nullptr;
    UpLoadDialog* m_uploadDialog= nullptr;
    RecorderDialog* m_recorderDialog= nullptr;
    PlayerDialog* m_playerDialog= nullptr;

    UploadWorker* m_uploadWork;
    LiveWork* m_liveWork;
    QMap<int, FileInfo*> m_mapVideoIdToFileInfo;
    QMap<int, FileInfo*> m_mapHaveUploadVideoIdToFileInfo;
    PFun pFun[MAX_FUN_COUNT];
    int m_id;
public:
    //static OnlineDialog* m_onlin;
    friend class UploadWorker;
};

#endif // CKERNEL_H
