#include "recorderdialog.h"
#include "ui_recorderdialog.h"

RecorderDialog::RecorderDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RecorderDialog)
{
    ui->setupUi(this);
    m_pictureWidget = new PictureWidget();
    m_pictureWidget->hide();
    m_pictureWidget->move(0,0);
    m_saveVideoFileThread = new SaveVideoFileThread();
    this->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    connect(m_saveVideoFileThread, SIGNAL(SIG_sendPicInPic(QImage)), m_pictureWidget, SLOT(slot_setImage(QImage)));
    connect(m_saveVideoFileThread, SIGNAL(SIG_sendVideoFrame(QImage)), this, SLOT(slot_setImage(QImage)));
    hasAudio = ui->cb_audio->isChecked();
    hasVideo = ui->cb_camera->isChecked();
    hasDesk = ui->cb_desktop->isChecked();
}

RecorderDialog::~RecorderDialog()
{
    delete ui;
}


void RecorderDialog::on_pb_setUrl_clicked()
{
    m_saveUrl = ui->le_url->text();
    m_saveUrl = m_saveUrl.replace("/", "\\");
}


void RecorderDialog::on_pb_stop_clicked()
{
    m_pictureWidget->hide();
    //ui->lb_show->clear();
    m_saveVideoFileThread->slot_closeVideo();
    ui->lb_show->clear();
}


void RecorderDialog::on_pb_start_clicked()
{
    this->showMinimized();
    m_pictureWidget->show();

    STRU_AV_FORMAT format;
    //format.clear();
    format.fileName = m_saveUrl;
    format.frame_rate = FRAME_RATE;
    format.hasAudio = hasAudio;
    format.hasDesk = hasDesk;
    format.hasCamera = hasAudio;
    format.videoBitRate = 2000000;
    //采样频率41100
    //码率 6400
    //声道 2
    //精度位数 16
    //
    QScreen * src = QApplication::primaryScreen();
    QRect rect = src->geometry();
    format.width =rect.width();
    format.height = rect.height();
    m_saveVideoFileThread->slot_setInfo(format);
    m_saveVideoFileThread->slot_openVideo();
}

void RecorderDialog::slot_setImage(QImage img)
{
    QPixmap pixmap;
    if(!img.isNull())
    {
        pixmap = QPixmap::fromImage(img.scaled(ui->lb_show->size(), Qt::KeepAspectRatio));
    }
    else
    {
        pixmap = QPixmap::fromImage(img);
    }
    ui->lb_show->setPixmap(pixmap);
}

void RecorderDialog::on_cb_desktop_stateChanged(int arg1)
{
    hasDesk = arg1;
}


void RecorderDialog::on_cb_camera_stateChanged(int arg1)
{
    hasVideo = arg1;
}


void RecorderDialog::on_cb_audio_stateChanged(int arg1)
{
    hasAudio = arg1;
}

