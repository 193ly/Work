#include "ckernel.h"
#include<QDebug>
#include<QMessageBox>
#include<QCryptographicHash>
#include<QString>
#include<QByteArray>
#include"packdef.h"
#include<QTime>
#include<QRandomGenerator>
#define RTMP_FRONT "rtmp://192.168.43.125/vod"
//密码转换为MD5格式
static QByteArray GetMD5(QString val){
    QCryptographicHash hash(QCryptographicHash::Md5);
    QString tmp = QString("%1 %2").arg(val).arg(MD5_KEY);
    hash.addData(tmp.toStdString().c_str(), strlen(tmp.toStdString().c_str()));
    QByteArray bt = hash.result();
    return bt.toHex();
}
//为空
//OnlineDialog* CKernel::m_onlin = nullptr;
CKernel::CKernel(const&CKernel)
{

}

CKernel::CKernel(QObject *parent) : QObject(parent)
{
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));
    memset(pFun, 0, MAX_FUN_COUNT);
    setMap();
    m_uploadWork = new UploadWorker(this);
    m_ip = _DEF_SERVER_IP;
    m_port = "8000";
    loadIniFile();
    m_tcpClient = new TcpClientMediator();
    connect(m_tcpClient, SIGNAL(SIG_ReadyData(uint,char*,int)), this, SLOT(slot_ReadyData(uint, char*, int)));
    //qDebug() << m_ip;
    //m_tcpClient->OpenNet("192.168.43.128", 8000);
    /////////////////连接网络的函数///////////
    if(m_tcpClient->OpenNet("192.168.43.128", 8000) <= 0)
    {
        qDebug() << "连接失败";
        return;
    }
    else
        qDebug() << "连接成功";

    //发送加密后的AES

    ////////////////////////////////////////
    m_liveWork = new LiveWork();
    m_uploadDialog = new UpLoadDialog;
    m_onlineDialog = new OnlineDialog;
    m_recorderDialog = new RecorderDialog;
    m_playerDialog = new PlayerDialog;
    m_loginDialog = new LoginDialog;
    m_loginDialog->setTcp(m_tcpClient);
    m_loginDialog->show();

    connect(this, SIGNAL(SIG_setProcess(float)), m_uploadDialog, SLOT(slot_setProcessBar(float)));
    connect(m_onlineDialog, &OnlineDialog::SIG_login_show, [=](){m_loginDialog->show();});
    connect(m_onlineDialog, &OnlineDialog::SIG_upload_show,[=](){m_uploadDialog->show();});
    connect(m_onlineDialog, SIGNAL(SIG_quit()), this, SLOT(slot_outLine()));
    connect(m_onlineDialog, SIGNAL(SIG_fresh()), this, SLOT(slot_downRq()));
    connect(m_onlineDialog, SIGNAL(SIG_haveUploaded()), this, SLOT(slot_haveUploadRq()));
    connect(m_loginDialog, SIGNAL(SIG_login(QString,QString)), this, SLOT(slot_login_Commit(QString, QString)));
    connect(m_loginDialog, SIGNAL(SIG_register(QString, QString,QString,HOBBY)), this, SLOT(slot_register(QString,QString,QString,HOBBY)));
    connect(m_uploadDialog, SIGNAL(SIG_UpLoadFile(QString,QString,HOBBY)), m_uploadWork, SLOT(slot_UpLoadFile(QString,QString,HOBBY)));
//    connect(m_loginDialog, &LoginDialog::SIG_openNet,m_tcpClient, [=](){
//        if(m_tcpClient->OpenNet() > 0)
//            m_loginDialog->opened = true;
//        else
//            m_loginDialog->opened = false;
//    }, Qt::BlockingQueuedConnection);
    connect(m_onlineDialog, SIGNAL(SIG_changeName(QString)), this, SLOT(slot_changName(QString)));
    connect(m_uploadDialog, &UpLoadDialog::SIG_openRecorder, [=](){m_recorderDialog->show();});
    connect(m_onlineDialog, SIGNAL(SIG_rmpt_play(QString)),m_playerDialog, SLOT(slot_showAndPlay(QString)));
    connect(m_onlineDialog, SIGNAL(SIG_addHot(QString)),this, SLOT(slot_addHot(QString)));
    //    m_mainDialog = new MainDialog();

    //    connect(m_mainDialog, SIGNAL(SIG_close()), this, SLOT(slot_destory()));

    //    m_mainDialog->show();
    //    m_loginDialog = new LoginDialog;
    //    m_loginDialog->show();
    //测试 对服务器发送数据
    //    STRU_LOGIN_RQ st;
    //    m_tcpClient->SendData(0, (char*)&st, sizeof(st));
    //m_thread = new QThread;    
    m_uploadWork = new UploadWorker(this);
    //m_uploadWork->moveToThread(m_thread);
    //m_thread->start();
}

CKernel::~CKernel()
{
    //    if(m_thread)
    //    {
    //        m_thread->quit();
    //        m_thread->wait(100);
    //        if(m_thread->isRunning())
    //        {
    //            m_thread->terminate();
    //            m_thread->wait();
    //        }
    //        delete m_thread;
    //    }
    if(m_loginDialog)
    {
        delete m_loginDialog;
        m_loginDialog = nullptr;
    }

    if(m_uploadDialog)
    {
        delete m_uploadDialog;
        m_uploadDialog = nullptr;
    }
    if(m_liveWork)
    {
        delete m_liveWork;
        m_liveWork = nullptr;
    }
    if(m_uploadWork)
    {
        delete m_uploadWork;
        m_uploadWork = nullptr;
    }
}
#define pFun(a)  pFun[a - _DEF_PACK_BASE]
CKernel::setMap()
{
    pFun(_DEF_PACK_REGISTER_RS) = &CKernel::slot_resiger_Rs;
    pFun(_DEF_PACK_LOGIN_RS) = &CKernel::slot_login_Rs;
    pFun(DEF_PACK_UPLOAD_RS) = &CKernel::slot_upload_Rs;
    pFun(DEF_PACK_DOWNLOAD_RS) = &CKernel::slot_downloadRs;
    pFun(DEF_PACK_FILEBLOCK_RQ) = &CKernel::slot_fileBlockRs;
    pFun(DEF_PACK_CHANGE_NAME_RS) = &CKernel::slot_changNameRs;
    pFun(DEF_PACK_UPLOAD_RS) = &CKernel::slot_upload_Rs;
    pFun(DEF_PACK_HAVEUPLOAD_RS) = &CKernel::slot_haveUploadRs;
}
void CKernel::slot_destory()
{
    qDebug() << __func__;
    //delete m_mainDialog;
}
void CKernel::slot_ReadyData(unsigned int lSendIp, char *buff, int nLen)
{
    int type = *(int*)buff - _DEF_PACK_BASE;
    qDebug()<< __func__ << type;
    if(type < MAX_FUN_COUNT && type >= 0)
    {
        qDebug() << "函数合法,开始执行";
        (this->*pFun[type])(lSendIp, buff, nLen);
    }
    else
    {
        qDebug()<<"函数不合法 || 接收到文件有问题";
    }
    delete[] buff;
}

void CKernel::slot_login_Rs(unsigned int lSendIp, char *buff, int nLen)
{
    qDebug() << __func__;
    STRU_LOGIN_RS rs = *(STRU_LOGIN_RS*)buff;

    if(rs.result == login_user_not_exist)
        QMessageBox::about(m_loginDialog, QString("错误"),QString("用户不存在"));
    else if(rs.result == login_password_error)
        QMessageBox::about(m_loginDialog, QString("错误"), QString("密码错误"));
    else if(rs.result == login_success)
    {
        QMessageBox::about(m_loginDialog, QString("成功"), QString("登录成功"));
        m_onlineDialog->m_id = rs.userid;
        m_id = rs.userid;
        m_loginDialog->hide();
        m_onlineDialog->setName(QString(rs.name));
        m_onlineDialog->show();
        m_liveWork->init(m_tcpClient, rs.userid);
        //m_liveWork->start();

        //发送下载请求
        //slot_downRq();
    }
    else
    {
        QMessageBox::about(m_loginDialog, QString("错误"), QString("返回出错"));
        return;
    }
    m_id = rs.userid;

    //发送刷新请求
    slot_destory();
}

void CKernel::slot_resiger_Rs(unsigned int lSendIp, char *buff, int nLen)
{
    qDebug() << __func__;
    STRU_REGISTER_RS rs = *(STRU_REGISTER_RS*)buff;
    if(rs.result == register_had)
        QMessageBox::about(m_loginDialog, "错误", "手机号已经被使用！");
    else if(rs.result == register_fail)
    {
        QMessageBox::about(m_loginDialog, "提示", "登录失败");
        return;
    }
    else if(rs.result == register_success)
    {
        m_loginDialog->hide();
        //delete m_loginDialog;
        m_onlineDialog->show();
        m_onlineDialog->setName(rs.name);
    }
}

void CKernel::slot_upload_Rs(unsigned int lSendIp, char *buff, int nLen)
{
    qDebug() << __func__;
    STRU_UPLOAD_RS rs = *(STRU_UPLOAD_RS*)buff;
    if(rs.m_nResult == send_file_fail)
    {
        qDebug() << "文件接收失败";
        //结束发送线程
        return;
    }
    else if(rs.m_nResult == send_file_success)
    {
        QMessageBox::about(m_uploadDialog, "提示", "发送成功");
        Q_EMIT SIG_setProcess(0);
        qDebug() << "文件发送成功";
    }
}

void CKernel::slot_login_Commit(QString tel, QString password)
{
    qDebug() << __func__;
    STRU_LOGIN_RQ rq;
    const char* t = tel.toStdString().c_str();
    memcpy(rq.tel, t, MAX_SIZE);
    QByteArray bt = GetMD5(password);
    memcpy(rq.password, bt.toStdString().c_str(), MAX_SIZE);
    //qDebug() << bt.toStdString().c_str() << "  " << strlen(bt.toStdString().c_str());
    if(m_tcpClient->SendData(0,(char*)&rq, sizeof(rq)) < 0)
    {
        QMessageBox::about(this->m_loginDialog, QString("提示"), QString("网络故障"));
        return;
    }
    else
        qDebug() << "发送成功";
//    //调试使用
//    STRU_LOGIN_RS rs;
//    rs.userid = 100;
//    rs.result = login_success;
//    slot_login_Rs(0, (char*)&rs, sizeof(rs));
}
void CKernel::slot_register(QString name, QString tel,QString password, HOBBY hobby)
{
    qDebug() << __func__;
    STRU_REGISTER_RQ rq;
    rq.hobby = hobby;
    QByteArray bt = GetMD5(password);
//    qDebug() << bt.toStdString().c_str() << "  " << strlen(bt.toStdString().c_str());
    memcpy(rq.tel, tel.toStdString().c_str(), MAX_SIZE);
    //memcpy(rq.password, password.toStdString().c_str(), MAX_SIZE);
    memcpy(rq.password, bt.data(), bt.size());
    memcpy(rq.name, name.toStdString().c_str(), MAX_SIZE);
    if(m_tcpClient->SendData(0, (char*)&rq, sizeof(rq)) < 0)
        qDebug() << "发送失败 ";

}


#include<QDir>
void CKernel::slot_downloadRs(unsigned int lSendIp, char *buff, int nLen)
{
    qDebug() << __func__;

    STRU_DOWNLOAD_RS rs = *(STRU_DOWNLOAD_RS*)buff;
    //当没有找到视频时
    if(rs.m_nFileId <= 0)
    {
        QMessageBox::about(m_onlineDialog->findHaveUploadDialog(), "提示", "没有找到视频");
        return;
    }

    FileInfo* fileInfo = new FileInfo;
    fileInfo->fileId = rs.m_nFileId;
    fileInfo->videoId = rs.m_nVideoId;
    fileInfo->fileName = rs.m_szFileName;

    QDir dir;
    if(dir.exists(QDir::currentPath() + "/temp/"))
        dir.mkdir(QDir::currentPath() + "/temp/");
    fileInfo->filePath = QString("./temp/%1").arg(rs.m_szFileName);
    fileInfo->filePos = 0;
    fileInfo->fileSize = rs.m_nFileSize;
    fileInfo->pFile = new QFile(fileInfo->filePath);
    char* url = (char*)malloc(strlen(rs.m_rtmp) + 1);
    memset(url, 0, strlen(rs.m_rtmp) + 1);
    memcpy(url, rs.m_rtmp, strlen(rs.m_rtmp));
    fileInfo->rtmpUrl = url;
    qDebug() << "rtmp--" << fileInfo->rtmpUrl;
    if(fileInfo->pFile->open(QIODevice::WriteOnly))
        m_mapVideoIdToFileInfo[fileInfo->videoId] = fileInfo;
    else
    {
        delete fileInfo;
    }
}
#define RTMP_FRONT "rtmp://192.168.43.128:1935/vod"
void CKernel::slot_fileBlockRs(unsigned int lSendIp, char *buff, int nLen)
{
    qDebug() << __func__;

    STRU_FILEBLOCK_RQ rq = *(STRU_FILEBLOCK_RQ*)buff;
    auto ite = m_mapVideoIdToFileInfo.find(rq.m_nFileId);
    if(ite != m_mapVideoIdToFileInfo.end())
    {
        //qDebug() << "find map in m_mapVideoIdToFileInfo success, fileId = " << rq.m_nFileId;
        FileInfo* info = m_mapVideoIdToFileInfo[rq.m_nFileId];
        int64_t res=  info->pFile->write(rq.m_szFileContent, rq.m_nBlockLen);
        //qDebug()<<"写入了" << res;
        info->filePos += res;
        //qDebug()<<"文件大小是" << info->filePos;
        if(info->filePos >= info->fileSize)
        {
            info->pFile->close();
            //删除节点
            m_mapVideoIdToFileInfo.erase(ite);
            //设置控件
            QString pbNum = QString("ml_%1").arg(info->fileId);
            QMyMoveLable* ml_play = m_onlineDialog->getMovie(pbNum);
            QMovie* lastMovie = ml_play->getMovie();
            if(lastMovie && lastMovie->isValid())
            {
                delete lastMovie;
            }
            QMovie* movie = new QMovie(info->filePath);
            ml_play->setMovie(movie);
            //设置rtmp
            QString path(RTMP_FRONT);
            path += info->rtmpUrl;
            ml_play->setUrl(path);

            delete info;
            info = nullptr;
            qDebug() << "文件接收完成";
        }
    }
    auto ite1 = m_mapHaveUploadVideoIdToFileInfo.find(rq.m_nFileId);
    if(ite1 != m_mapHaveUploadVideoIdToFileInfo.end())
    {
        //qDebug() << "find map in m_mapHaveUploadVideoIdToFileInfo success, fileId = " << rq.m_nFileId;
        FileInfo* info = m_mapHaveUploadVideoIdToFileInfo[rq.m_nFileId];
        int64_t res=  info->pFile->write(rq.m_szFileContent, rq.m_nBlockLen);
        //qDebug()<<"写入了" << res;
        info->filePos += res;
        //qDebug()<<"文件大小是" << info->filePos;
        if(info->filePos >= info->fileSize)
        {
            info->pFile->close();
            //删除节点
            m_mapHaveUploadVideoIdToFileInfo.erase(ite1);
            //设置控件
            QMovie* movie = new QMovie(info->filePath);
            m_onlineDialog->addHaveVideo(info->fileName, movie,info->fileSize1,info->rtmpUrl);
            delete info;
            info = nullptr;
            qDebug() << "文件接收完成";
        }
    }
}

void CKernel::slot_changNameRs(unsigned int lSendIp, char *buff, int nLen)
{
    STRU_CHANGE_NAME_RS rs = *(STRU_CHANGE_NAME_RS*) buff;
    if(rs.result == change_name_success)
    {
        m_onlineDialog->setName(rs.newName);
    }
    else
        QMessageBox::about(m_onlineDialog, "错误", "改名失败");
}

void CKernel::slot_haveUploadRs(unsigned int lSendIp, char *buff, int nLen)
{
    qDebug() << __func__;
    STRU_HAVEUPLOAD_RS rs = *(STRU_HAVEUPLOAD_RS*)buff;
    FileInfo* fileInfo = new FileInfo;
    fileInfo->fileId = rs.m_nFileId;
    fileInfo->videoId = rs.m_nVideoId;
    fileInfo->fileName = rs.m_szFileName;
    QDir dir;
    if(!dir.exists(QDir::currentPath() + "/temp/myUpload/"))
        dir.mkdir(QDir::currentPath() + "/temp/myUpload/");
    fileInfo->filePath = QString("./temp/myUpload/%1").arg(rs.m_szGifName);
    fileInfo->filePos = 0;
    fileInfo->fileSize = rs.m_nGifSize; //gif 大小
    fileInfo->pFile = new QFile(fileInfo->filePath);
    fileInfo->fileSize1 = rs.m_nFileSize; // 视频大小
    char* url = (char*)malloc(strlen(rs.m_rtmp));
    memcpy(url, rs.m_rtmp, strlen(rs.m_rtmp));
    fileInfo->rtmpUrl = url;
    qDebug() << "rtmp--" << fileInfo->rtmpUrl;
    if(fileInfo->pFile->open(QIODevice::WriteOnly))
        m_mapHaveUploadVideoIdToFileInfo[fileInfo->videoId] = fileInfo;
    else
    {
        delete fileInfo;
    }
}
void CKernel::slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby)
{
    qDebug() << "开始上传";
//    QDateTime currentTime =QDateTime::currentDateTime();
//    QString str =currentTime.toString("dhhmmsszzz");
//    int id = atoi(str.toStdString().c_str());
    UpLoadFile(filePath, hobby, imgPath);
    UpLoadFile(filePath, hobby);

}

void CKernel::slot_downRq()
{
    if(m_id <= 0)
    {
        QMessageBox::about(m_onlineDialog, "提示", "用户未登录");
        return;
    }
    STRU_DOWNLOAD_RQ rq;
    rq.m_nUserId = m_id;
    m_tcpClient->SendData(0, (char*)& rq, sizeof(rq));
}

void CKernel::slot_changName(QString name)
{
    STRU_CHANGE_NAME_RQ rq;
    memcpy(rq.name, name.toStdString().c_str(), MAX_SIZE);
    rq.m_id= m_id;
    m_tcpClient->SendData(0, (char*)&rq, sizeof(rq));
}

void CKernel::slot_haveUploadRq()
{
    qDebug() << __func__;
    if(m_id <= 0)
    {
        QMessageBox::about(m_onlineDialog, "提示", "用户未登录");
        return;
    }
    STRU_HAVEUPLOAD_RQ rq;
    rq.m_userId = m_id;
    m_tcpClient->SendData(0, (char*)& rq, sizeof(rq));
}

void CKernel::slot_addHot(QString rtmp)
{
    qDebug() << rtmp;
    STRU_ADDHOT_RQ rq;
    rq.id = m_id;
    memcpy(rq.m_rtmp, rtmp.toStdString().c_str() + sizeof("rtmp://192.168.43.125/vod") - 1, MAX_PATH);
    m_tcpClient->SendData(0, (char*)&rq, sizeof(rq));
}

void CKernel::slot_outLine()
{
    STRU_OUTLINE_RQ rq;
    rq.id = m_id;
    m_tcpClient->SendData(0, (char*)&rq, sizeof(rq));
    delete this;
}

#include<QDateTime>
void CKernel::UpLoadFile( QString filePath, HOBBY hobby, QString gifPath)
{
    //可能有问题
    //    QFileInfo info(filePath);
    //    QString fileName = info.fileName();
    //    std::string strName = fileName.toStdString();
    //    const char* file_name = strName.c_str();

    //    STRU_UPLOAD_RQ rq;
    //    rq.m_nFileId = qrand() % 10000;
    //    rq.m_nFileSize = info.size();
    //    strcpy_s(rq.m_szFileName, MAX_PATH, file_name);

    //    QByteArray bt = filePath.right(filePath.length() - filePath.lastIndexOf('.') - 1).toLocal8Bit();

    //    memcpy(rq.m_szFileType,  bt.data(), bt.length());
    //    if(!gifName.isEmpty())
    //    {
    //        QFileInfo info(gifName);
    //        strcpy_s(rq.m_szGifName, MAX_PATH, info.fileName().toLocal8Bit().data());
    //    }
    //    memcpy(rq.m_szHobby, &hobby, sizeof(hobby));
    //    rq.m_UserId = m_id;
    //    //发送文件
    //    m_tcpClient->SendData(0, (char*)&rq, sizeof(rq));

    //    FileInfo fi;
    //    fi.fileId = rq.m_nFileId;
    //    fi.fileName = fileName;
    //    fi.filePath = filePath;
    //    fi.filePos = 0;
    //    fi.fileSize = rq.m_nFileSize;
    //    fi.pFile = new QFile(filePath);

    //    if(fi.pFile->open(QIODevice::ReadOnly))
    //    {
    //        while(1)
    //        {
    //            STRU_FILEBLOCK_RQ blockRq;
    //            int64_t res = fi.pFile->read(blockRq.m_szFileContent, DEF_CONTENT_SIZE);
    //            if(res < 0)
    //                qDebug() << "读取文件失败";
    //            fi.filePos += res;
    //            blockRq.m_nBlockLen = res;
    //            blockRq.m_nFileId = rq.m_nFileId;
    //            blockRq.m_nUserId = m_id;
    //            //发送文件
    //            m_tcpClient->SendData(0, (char*)&blockRq, sizeof(blockRq));
    //            emit SIG_setProcess(fi.filePos, fi.fileSize);

    //            if(fi.filePos >= fi.fileSize)
    //            {
    //                fi.pFile->close();
    //                delete fi.pFile;
    //                fi.pFile = nullptr;
    //                break;
    //            }
    //        }
    //    }
    QFileInfo info(filePath);
    QFileInfo* gifinfo = nullptr;
    QString fileName = info.fileName();
    QString gifName;
    STRU_UPLOAD_RQ rq;
    rq.m_UserId = m_id;
    QDateTime currentTime =QDateTime::currentDateTime();
    QString str =currentTime.toString("dhhmmsszzz");
    int fileId = atoi(str.toStdString().c_str());
    rq.m_nFileId = fileId;
    //rq.m_nFileId = qrand() % 10000;
    String bt;
    if(!gifPath.isEmpty())
    {
        gifinfo = new QFileInfo(gifPath);
        gifName = gifinfo->fileName();
        strcpy_s(rq.m_szGifName, MAX_PATH,
                 gifName.toStdString().c_str());
        rq.m_nFileSize = gifinfo->size();
        bt = gifName.right(gifName.length() - gifName.lastIndexOf('.') - 1).toStdString();

    }
    else
    {
        rq.m_nFileSize = info.size();
        bt = fileName.right(fileName.length() - fileName.lastIndexOf('.') - 1).toStdString();

    }
    strcpy_s(rq.m_szFileName, MAX_PATH,
             fileName.toStdString().c_str());
    memcpy(rq.m_szFileType,  bt.c_str(), MAX_SIZE);
    qDebug() << rq.m_szFileType;
    memcpy((char*)&rq.m_szHobby, (char*)&hobby, sizeof(HOBBY));

    m_tcpClient->SendData(0, (char*)&rq, sizeof(rq));
//    _sleep(1000  * 10000);
    //发送文件
    //
    _sleep(1000);
    FileInfo fi;
    fi.fileId = rq.m_nFileId;
    fi.fileName = !gifPath.isEmpty() ?  gifName :fileName;
    fi.filePath = !gifPath.isEmpty() ?  gifPath :filePath;
    fi.filePos = 0;
    fi.fileSize = rq.m_nFileSize;
    fi.pFile = new QFile(!gifPath.isEmpty() ? gifPath : filePath);
    if(fi.pFile->open(QIODevice::ReadOnly))
    {
//刘洋
        while(1)
        {
            STRU_FILEBLOCK_RQ blockRq;
            int64_t res = fi.pFile->read(blockRq.m_szFileContent, MAX_CONTENT_SIZE);
            if(res < 0)
                qDebug() << "读取文件失败";
            fi.filePos += res;
            blockRq.m_nBlockLen = res;
            blockRq.m_nFileId = rq.m_nFileId;
            blockRq.m_nUserId = m_id;
            blockRq.m_pos = fi.filePos;
            //发送文件
            m_tcpClient->SendData(0, (char*)&blockRq, sizeof(blockRq));
            //emit SIG_setProcess(fi.filePos, fi.fileSize);
            emit SIG_setProcess((float)fi.filePos / fi.fileSize * 100);
            if(fi.filePos >= fi.fileSize)
            {
                fi.pFile->close();
                delete fi.pFile;
                fi.pFile = nullptr;
                break;
            }
           // _sleep(100000000);
        }
    }
    if(gifinfo)
        delete gifinfo;
}

#ifdef USE_SEVER
CKernel::slot_dealSeverData(unsigned int lSendIp, char *buff, int nLen)
{
    QString str = QString("来自服务端：%1").arg(QString::fromStdString(buff));
    QMessageBox::about(NULL, "提示", str);  //阻塞的，模态窗口
    //m_tcpClient->SendData(lSendIp, buff, nLen);
    delete[] buff;
}
#endif
//配置文件 //一般位于exe的同级目录
//思路：根据目录查看文件是否存在，存在则加载，不存在创建并且写入默认值
//.ini
//格式
//[组名]
//key = value

//例如
//[net]
//ip = 192.168……
//port = 8080
#include<QCoreApplication>
#include<QFileInfo>
//配置文件使用的类
#include<QSettings>
#include<string>
using namespace std;
CKernel::loadIniFile()
{
    //获取exe目录
    QString path = QCoreApplication::applicationDirPath() + "/config.ini";
    qDebug() << path;
    QFileInfo info(path);
    if(info.exists()) //存在文件
    {
        QSettings setting(path, QSettings::IniFormat); // 没有会创建
        //打开组
        setting.beginGroup("net");
        //设置key value
        QVariant strIP = setting.value("ip", "");
        QVariant strPort = setting.value("port", "");
        if(!strIP.toString().isEmpty())
            m_ip = strIP.toString();
        if(!strPort.toString().isEmpty())
            m_port = strPort.toString();
        //关闭组
        setting.endGroup();
    }
    else              //不存在文件
    {
        QSettings setting(path, QSettings::IniFormat); // 没有会创建
        //打开组
        setting.beginGroup("net");
        //设置key value
        setting.setValue("ip", m_ip);
        setting.setValue("port", m_port);
        //关闭组
        setting.endGroup();
    }
    qDebug() << m_ip << " " << m_port;
}

void UploadWorker::slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby)
{
    qDebug() << "工作者上传文件";
    this->m_onlin->slot_UpLoadFile(filePath, imgPath, hobby);
}
