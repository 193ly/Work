#include "onlinedialog.h"
#include "ui_onlinedialog.h"
#include<QCryptographicHash>
#include<QMessageBox>
#define MD5_KEY 12345
//暂时/永久注释
//OnlineDialog* OnlineDialog::m_online = nullptr;

OnlineDialog::OnlineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OnlineDialog)
{
    ui->setupUi(this);
    //暂时/永久注释
    //    m_uploadDialog = new UpLoadDialog();
    //    m_uploadThread = new QThread();
    //    m_uploadWorker = new UploadWorker();
    //    OnlineDialog::m_online = this;
    //    connect(m_uploadDialog, SIGNAL(SIG_UpLoadFile(QString,QString,HOBBY)),
    //            m_uploadWorker, SLOT(slot_UpLoadFile(QString,QString,HOBBY)));
    //    m_uploadWorker->moveToThread(m_uploadThread);
    //设置播放短视频

    connect(ui->ml_1, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_2, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_3, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_4, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_5, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_6, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_7, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_8, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_9, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_10, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_11, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_12, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_13, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_14, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));
    connect(ui->ml_15, SIGNAL(SIG_playUrl(QString)), this, SLOT(slot_addHotAndPlay(QString)));

    m_changeName = new ChangeName;
    connect(m_changeName, SIGNAL(SIG_submit(QString)), this, SIGNAL(SIG_changeName(QString)));

    m_haveUploadDialog = new HaveUploadDialog;
}

QString OnlineDialog::getUrl()
{
    return rmpt;
}

OnlineDialog::~OnlineDialog()
{
    delete ui;
    //暂时/永久注释
    //    if(m_uploadThread)
    //    {
    //        m_uploadThread->quit(); //退出
    //        m_uploadThread->wait(100); //等待
    //        if(m_uploadThread->isRunning()) //如果还在运行，就杀掉
    //        {
    //            m_uploadThread->terminate();
    //            m_uploadThread->wait(100);
    //        }
    //    }
    if(m_changeName)
        delete m_changeName;
    if(m_haveUploadDialog)
        delete m_haveUploadDialog;
    m_changeName = nullptr;
    m_haveUploadDialog = nullptr;
}

void OnlineDialog::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this, "退出提示", "是否退出") == QMessageBox::Yes)
    {
        Q_EMIT SIG_quit();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

QMyMoveLable *OnlineDialog::getMovie(QString name)
{
    return ui->stackedWidget->findChild<QMyMoveLable*>(name);
}

void OnlineDialog::setName(QString name)
{
    ui->lb_name->setText(name);
}

void OnlineDialog::addHaveVideo(QString name, QMovie* movie, int size, QString rmpt)
{
    m_haveUploadDialog->addItem(name, movie, size, rmpt);
}

void OnlineDialog::setProcessBar(float p)
{
}

HaveUploadDialog *OnlineDialog::findHaveUploadDialog()
{
    return m_haveUploadDialog;
}

void OnlineDialog::on_pb_login_clicked()
{
    //    QString name = ui->le_name->text();
    //    QString password = ui->le_password->text();
    //    Q_EMIT SIG_login(name, password);
    //    //使用密码
    //    QByteArray bt = GetMD5(password);
    //    memcpy()
    emit SIG_login_show();
}


void OnlineDialog::on_pb_rmtpPlay_clicked()
{
    rmpt = ui->le_rmptUrl->text();
}

//点击上传视频
void OnlineDialog::on_pb_upload_clicked()
{
    if(m_id == 0)
    {
        QMessageBox::about(this, "提示","请先登录");
        return;
    }
    Q_EMIT  SIG_upload_show();
    //暂时/永久注释
    //    m_uploadDialog->Clear();
    //    m_uploadDialog->show();
}
//暂时/永久注释
//void OnlineDialog::slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby)
//{
//    qDebug() << "开始上传";
//    UpLoadFile(filePath, hobby);
//    UpLoadFile(filePath, hobby, imgPath);
//}
//暂时/永久注释
//void OnlineDialog::UpLoadFile(QString filePath, HOBBY hobby, QString gifName)
//{
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
//    阿瓦达sendData;

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
//            int64_t res = fi.pFile->read(blockRq.m_szFileContent, MAX_CONTENT_SIZE);
//            fi.filePos += res;
//            blockRq.m_nBlockLen = res;
//            blockRq.m_nFileId = rq.m_nFileId;
//            blockRq.m_nUserId = m_id;
//            //发送文件
//            阿达瓦sendData();
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
//}
//工作者上传流程
//暂时/永久注释
//void UploadWorker::slot_UpLoadFile(QString filePath, QString imgPath, HOBBY hobby)
//{
//    OnlineDialog::m_online->slot_UpLoadFile(filePath, imgPath, hobby);
//}

void OnlineDialog::on_pb_tv_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void OnlineDialog::on_pb_recommendTV_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    emit SIG_fresh();
}


void OnlineDialog::on_pb_fresh_clicked()
{
    emit SIG_fresh();
}


void OnlineDialog::on_pb_change_name_clicked()
{
    m_changeName->show();
}


void OnlineDialog::on_pb_uploaded_clicked()
{
    emit SIG_haveUploaded();
    m_haveUploadDialog->show();
}

void OnlineDialog::slot_addHotAndPlay(QString rmpt)
{
    emit SIG_addHot(rmpt);
    emit SIG_rmpt_play(rmpt);
}

