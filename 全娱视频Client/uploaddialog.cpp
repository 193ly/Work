#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include<QTime>
#include<QMovie>

UpLoadDialog::UpLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpLoadDialog)
{
    ui->setupUi(this);
    ui->pb_beginUpload->setEnabled(false);
    ui->progressBar->setRange(0, 100);

    m_pLayOut = new QVBoxLayout;
    m_pLayOut->setContentsMargins(3,0,3,0);

    //设置spacing
    m_pLayOut->setSpacing(3);
    ui->le_hobbyshow->setLayout(m_pLayOut);
    slot_addItem();
    qRegisterMetaType<HOBBY>("HOBBY");
    m_saveJPGWork = new SaveJPGWorker(this);
    connect(this, SIGNAL(SIG_saveJPG(QString)),m_saveJPGWork, SLOT(slot_work(QString)));
    connect(m_saveJPGWork, SIGNAL(SIG_setUI(QString,QString)), this, SLOT(slot_setUI(QString,QString)));
    //connect(ui->le_hobbyshow, SIGNAL(SIG_addItem()), this, SLOT(slot_addItem()));
}

UpLoadDialog::~UpLoadDialog()
{
    delete m_pLayOut;
    delete ui;
    for(auto& ite: list_hobbyLineDialog)
    {
        delete ite;
    }
}


void UpLoadDialog::on_pb_beginUpload_clicked()
{
    HOBBY hobby;
    int i = 0;
    for(auto* ite: list_hobbyLineDialog)
    {
        QString s = ite->GetText();
        memcpy(hobby.m_hobby[i++], s.toStdString().c_str(), s.size());
    }
    emit SIG_UpLoadFile(m_filePath, m_imgPath, hobby);
}


void UpLoadDialog::on_pb_view_check_clicked()
{
    qDebug() << QThread::currentThreadId();
    QString path = QFileDialog::getOpenFileName(this, "打开", "./", "(*.flv *.mp4)");
    emit SIG_saveJPG(path);
}



void UpLoadDialog::slot_setUI(QString path, QString imgPath)
{
    ui->le_path->setText(path);

    QMovie* movie = new QMovie(imgPath);
    ui->lb_gif->setMovie(movie);
    movie->start();

    m_filePath = path;
    m_imgPath = imgPath;
    ui->pb_beginUpload->setEnabled(true);
}

void UpLoadDialog::slot_deleteHobby(HobbyLineDialog *hobbyDialog)
{
    if(list_hobbyLineDialog.removeOne(hobbyDialog) )
    {
        m_pLayOut->removeWidget(hobbyDialog);
        delete hobbyDialog;
        qDebug() << "删除标签成功";
    }
    else
        qDebug() << "删除标签失败";
}


QString UpLoadDialog::SaveVideoJpg( QString FilePath )
{
    std::string tmp = FilePath.toStdString();
    char* file_path = (char*)tmp.c_str();
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB;
    AVPacket *packet;
    uint8_t *out_buffer;
    static struct SwsContext *img_convert_ctx;
    int videoStream, i, numBytes;
    int ret, got_picture;
    av_register_all(); //初始化 FFMPEG 调用了这个才能正常适用编码器和解码器
    //Allocate an AVFormatContext.
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, file_path, NULL, NULL) != 0) {
        qDebug() << "can't open the file." ;
        return QString ();
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        qDebug() <<"Could't find stream infomation.";
        return QString ();
    }
    videoStream = -1;
    ///循环查找视频中包含的流信息，直到找到视频类型的流
    ///便将其记录下来 保存到 videoStream 变量中///这里现在只处理视频流 音频流先不管他
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }
    ///如果 videoStream 为-1 说明没有找到视频流
    if (videoStream == -1) {
        qDebug() <<"Didn't find a video stream.";
        return QString ();
    }
    ///查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        qDebug() <<"Codec not found." ;
        return QString ();
    }
    ///打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        qDebug() <<"Could not open codec." ;
        return QString ();
    }
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                                     AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32,
                                  pCodecCtx->width,pCodecCtx->height);
    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,
                   pCodecCtx->width, pCodecCtx->height);
    int y_size = pCodecCtx->width * pCodecCtx->height;
    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个 packet
    av_new_packet(packet, y_size); //分配 packet 的数据
    int ncount = 0;
    while(1)
    {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            break; //这里认为视频读取完了
        }
        if (packet->stream_index == videoStream) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);
            if (ret < 0) {
                qDebug() <<"decode error." ;
                return QString ();
            }
            if (got_picture && pFrame->key_frame == 1 && pFrame->pict_type ==
                    AV_PICTURE_TYPE_I) {
                sws_scale(img_convert_ctx,
                          (uint8_t const * const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                          pFrameRGB->linesize);
                QImage tmpImg((uchar
                               *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.scaled(640 ,320 ,Qt::KeepAspectRatio );
                image.save( QString("./temp/%1.jpg").arg( ncount ,2 ,10,
                                                          QChar('0') ) );
                ncount++;
                if( ncount == 6 ) {
                    av_free_packet(packet);
                    break;
                }
            }
        }
        av_free_packet(packet);
    }
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    QProcess p(0);
    p.start("cmd" );//开启 cmd 窗口
    p.waitForStarted();
    QString curPath = QDir::currentPath();
    qDebug() << curPath;
    curPath.replace("/","\\\\");
    QString strcd = QString("%1/ \n").arg(QCoreApplication::applicationDirPath());
    strcd.replace("/","\\\\");
    strcd = QString("cd /d ")+strcd;
    p.write( strcd.toLocal8Bit() );
    QString imgName = QString("%1\\\\temp\\\\%2.gif")
            .arg(curPath).arg(QTime::currentTime().toString("hhmmsszzz"));
    QString cmd = QString("ffmpeg -r 2 -i %1\\\\temp\\\\").arg(curPath);
    cmd += "%02d.jpg ";
    cmd += imgName;
    cmd += " \n";
    qDebug() << "cmd "<< cmd;
    p.write( cmd.toLocal8Bit() );
    p.closeWriteChannel();
    p.waitForFinished();
    qDebug()<< QString::fromLocal8Bit( p.readAllStandardOutput() );
    p.close();

return imgName;
}

void UpLoadDialog::Clear()
{
    ui->pb_beginUpload->setEnabled(false);
    ui->le_path->setText("");
    emit SIG_GetOnePicture(QImage());
    ui->progressBar->setValue(0);
}

void UpLoadDialog::slot_setProcessBar(float p)
{
    int i = (int)p;
    ui->progressBar->setValue(i);
}

void UpLoadDialog::slot_addItem()
{
    qDebug() << __FUNCTION__;
    HobbyLineDialog* hobbyDialog = new HobbyLineDialog;
    //绑定发送重传文件的信号
    connect(hobbyDialog, SIGNAL(SIG_addItem()), this, SLOT(slot_addItem()));
    //绑定删除标签的信号
    connect(hobbyDialog, SIGNAL(SIG_deleteItem(HobbyLineDialog*)), this, SLOT(slot_deleteHobby(HobbyLineDialog*)), Qt::QueuedConnection);
    m_pLayOut->addWidget(hobbyDialog);
    list_hobbyLineDialog.push_back(hobbyDialog);
}

void UpLoadDialog::on_pb_view_clicked()
{
    Q_EMIT SIG_openRecorder();
}


void SaveJPGWorker::slot_work(QString path)
{
    qDebug() << QThread::currentThreadId();

    QDir dir;
    if(!dir.exists(QDir::currentPath() + "./temp/"))
        dir.mkpath(QDir::currentPath() + "./temp");
    //显示缩略图
    QString imgPath = m_upload->SaveVideoJpg(path);
    //QString imgPath = m_saveJPGWork->SaveVideoJpg(path);
    //QFileInfo info(path);
    //加载到控件
    emit SIG_setUI(path, imgPath);
//    QMovie* movie = new QMovie(imgPath);
//    m_upload->ui->lb_gif->setMovie(movie);
//    movie->start();

//    m_upload->m_filePath = path;
//    m_upload->m_imgPath = imgPath;
//    m_upload->ui->pb_beginUpload->setEnabled(true);
}

void UpLoadDialog::on_pb_addItem_clicked()
{
    slot_addItem();
}

