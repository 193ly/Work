#include "qmymovelable.h"
#include "ui_qmymovelable.h"
#include<QDebug>
QMyMoveLable::QMyMoveLable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMyMoveLable)
{
    ui->setupUi(this);
    m_movie = nullptr;
    ui->lb_move->installEventFilter(this);

//    QMovie* movie = new QMovie("D:/qtobject/NetDisk/bulid-debug/temp/210735349.gif");
//    ui->lb_move->setMovie(movie);
//    movie->start();
    //暂时使用
    m_rmptUrl = "http://39.134.65.162/PLTV/88888888/224/3221225611/index.m3u8";
    m_rmptUrl = "";
    //m_rmptUrl = "D:/SteamLibrary/steamapps/workshop/content/431960/2211765165/ahegao-mr-blue-sky.mp4";
}

QMyMoveLable::~QMyMoveLable()
{
    delete ui;
}

void QMyMoveLable::enterEvent(QEvent *event)
{
    if(m_movie && m_movie->isValid())
        m_movie->start();
}

void QMyMoveLable::leaveEvent(QEvent *event)
{
    if(m_movie&& m_movie->isValid())
        m_movie->stop();
}

bool QMyMoveLable::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lb_move && event->type() == QEvent::MouseButtonPress)
    {
        QPixmap pix;
        pix.fill(Qt::yellow);
        ui->lb_move->setPixmap(pix);
        qDebug() << __func__ << "点击gif图片";
        Q_EMIT SIG_playUrl(m_rmptUrl);
    }
    else
        event->accept();
}

QMovie *QMyMoveLable::getMovie()
{
    return m_movie;
}

void QMyMoveLable::setMovie(QMovie *movie)
{
//    if(m_movie)
//        delete m_movie;
    m_movie = movie;
    ui->lb_move->setMovie(m_movie);
    if(m_movie && m_movie->isValid())
        qDebug() << "set movie sucess";
    else
        qDebug() << "set movie fail";
    m_movie->start();
    m_movie->stop();
}

void QMyMoveLable::setUrl(QString url)
{
    m_rmptUrl = url;
}

