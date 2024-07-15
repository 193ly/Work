#include "playerdialog.h"
#include "ui_playerdialog.h"
#include<QFileDialog>
#include<QMessageBox>
#define _DEF_PATH "rtmp://192.168.43.128:1935/vod//2.mp4"
#define _DEF_PATH2 "rtmp://192.168.43.128:1935/videotest"
#define hunan "rtmp://58.200.131.2:1935/livetv/hunantv"
void PlayerDialog::closeEvent(QCloseEvent *)
{
    on_pb_stop_clicked();
}

PlayerDialog::PlayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerDialog), isTimerStop(false),isPause(false)
{
    ui->setupUi(this);
    m_player = new VideoPlayer;
    connect(m_player, SIGNAL(SIG_getOneImage(QImage)), this, SLOT(slots_getOneImage(QImage)));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slot_TimerTimeOut()));
    connect(m_player, SIGNAL(SIG_playerStateChange(int)), this, SLOT(slot_PlayerStateChanged(int)));
    connect( m_player, SIGNAL( SIG_TotalTime(qint64)), this ,SLOT( slot_getTotalTime(qint64)) );
    m_timer.setInterval(500);//超时时间500毫秒
    slot_PlayerStateChanged(PlayerState::Stop);
    ui->slider_progress->installEventFilter(this); //安装时间过滤器,让该对象称为被观察对象，this去执行函数
    ui->wd_content->installEventFilter(this);
}

PlayerDialog::~PlayerDialog()
{
    delete ui;
}
#include<QStyle>
#include<QMouseEvent>
bool PlayerDialog::eventFilter(QObject * obj, QEvent * event)
{
    //focusPolicy()
    if (obj == ui->slider_progress) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            int min = ui->slider_progress->minimum();
            int max = ui->slider_progress->maximum();
            int value = QStyle::sliderValueFromPosition(
                        min, max, mouseEvent->pos().x(), ui->slider_progress->width());
            isTimerStop = true;
            m_timer.stop();
            ui->slider_progress->setValue(value);
            m_player->seek((qint64)value * 1000000);  // value 是秒
            isTimerStop = false;
            m_timer.start();
            //qDebug() << "Ate key press" << mouseEvent->key();
            return true;
        } else {
            return false;
        }
    }
    else if(obj == ui->wd_content )//|| obj == ui->lb_show)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            if(m_player->playerState() == PlayerState::Pause)
                on_pb_play_clicked();
            else
                on_pb_pause_clicked();
        }
        return QDialog::eventFilter(obj, event);
    }
    else {
        // pass the event on to the parent class
        return QDialog::eventFilter(obj, event);
    }
}
#include<QPainter>

//void PlayerDialog::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(ui->wd_content);
//    painter.drawPixmap(10,0,pix.width(),pix.height(),pix);
//    painter.end();
//}

void PlayerDialog::slots_getOneImage(QImage img)
{
    qDebug() << "show image";
    ui->wd_show->slot_setImage(img);
}

//void PlayerDialog::on_pb_play_clicked()
//{
//    //开始播放
//    m_palyer->start();
//}

void PlayerDialog::on_pb_start_clicked()
{
    //开始播放
    //m_player->start();
    //首先要关闭视频，判断状态 stop
//    if( m_player->playerState() == PlayerState::Stop)
//    {
//        m_player->stop(false);
//    }
    //打开浏览选择文件
    QString path = QFileDialog::getOpenFileName(this, "打开文件", "./") ;
    //"视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);; 所有文件(*.*);;"
    //判断
    if(path.isEmpty())
        return;

    on_pb_stop_clicked();

    //设置m_player的filename

    m_player->setFileName(path);
    slot_PlayerStateChanged(PlayerState::Playing);

}


void PlayerDialog::on_pb_play_clicked()
{
    if(m_player->playerState() != PlayerState::Pause)
        return;
    m_player->play();
    ui->pb_play->hide();
    ui->pb_pause->show();
    isPause = false;
}


void PlayerDialog::on_pb_pause_clicked()
{
    if(m_player->playerState() != PlayerState::Playing)
        return;
    m_player->pause();
    ui->pb_pause->hide();
    ui->pb_play->show();
    isPause = true;
}


void PlayerDialog::on_pb_stop_clicked()
{
    if(!isPause)
        m_player->pause();
    m_player->stop(true);
}
void PlayerDialog::slot_PlayerStateChanged(int state)
{
    switch( state )
    {
    case PlayerState::Stop:
        qDebug()<< "VideoPlayer::Stop";
        while(m_timer.isActive())
            m_timer.stop();
        if(isTimerStop)
            return;
        ui->slider_progress->setValue(0);
        ui->lb_totalTime->setText("00:00:00");
        ui->lb_curTime->setText("00:00:00");

        ui->pb_pause->hide();
        ui->pb_play->show();
    {
        QImage img;
        img.fill( Qt::black);
        slots_getOneImage(img);
    }
        this->update();
        isStop = true;
        break;
    case PlayerState::Playing:
        qDebug()<< "VideoPlayer::Playing";
        ui->pb_play->hide();
        ui->pb_pause->show();
        m_timer.start();
        this->update();
        isStop = false;
        break;
    }
}
void PlayerDialog::slot_getTotalTime(qint64 uSec)
{
    qint64 Sec = uSec/1000000;
    ui->slider_progress->setRange(0,Sec);//精确到秒
    QString hStr = QString("00%1").arg(Sec/3600);
    QString mStr = QString("00%1").arg(Sec/60);
    QString sStr = QString("00%1").arg(Sec%60);
    QString str = QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
    ui->lb_totalTime->setText(str);
}
void PlayerDialog::slot_TimerTimeOut()
{
    if (QObject::sender() == &m_timer)
    {
        qint64 Sec = m_player->getCurrentTime()/1000000;
        ui->slider_progress->setValue(Sec);
        QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60%60);
        QString sStr = QString("00%1").arg(Sec%60);
        QString str = QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
        ui->lb_curTime->setText(str);
        if(ui->slider_progress->value() == ui->slider_progress->maximum()
                && m_player->playerState() == PlayerState::Stop)
        {
            slot_PlayerStateChanged( PlayerState::Stop );
        }else if(ui->slider_progress->value() + 1 ==
                 ui->slider_progress->maximum()
                 && m_player->playerState() == PlayerState::Stop)
        {
            slot_PlayerStateChanged( PlayerState::Stop );
        }
    }
}

void PlayerDialog::slot_showAndPlay(QString path)
{

    qDebug() << __func__ << path;
    this->show();
    //开始播放
    //m_player->start();
    //首先要关闭视频，判断状态 stop


    //path = "rtmp://192.168.43.125:1935/vod//刘洋/final2.mp4";
    //path = "http://39.134.65.162/PLTV/88888888/224/3221225611/index.m3u8";
    //FILE* f = nullptr;
//    f = fopen(path.toStdString().c_str(), "r");
//    if(!f)
//    {
//        QMessageBox::about(this, "错误", "打开视频失败");
//        return;
//    }

    if( m_player->playerState() != PlayerState::Stop)
    {
        m_player->stop(false);
    }
    //打开浏览选择文件
    //QString path = QFileDialog::getOpenFileName(this, "打开文件", "./") ;
    //"视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);; 所有文件(*.*);;"
    //判断
    if(path.isEmpty())
        return;
    //设置m_player的filename
    m_player->setFileName(path);
    slot_PlayerStateChanged(PlayerState::Playing);
}
