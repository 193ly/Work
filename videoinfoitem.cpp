#include "videoinfoitem.h"
#include "ui_videoinfoitem.h"

VideoInfoItem::VideoInfoItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoInfoItem)
{
    ui->setupUi(this);
}

VideoInfoItem::~VideoInfoItem()
{
    delete ui;
}

void VideoInfoItem::setName(QString name)
{
    ui->lb_name->setText(name);
}

void VideoInfoItem::setMovie(QMovie* movie)
{
    QMovie* last = ui->wd_movie->getMovie();
    if(last && last->isValid())
        delete last;
    ui->wd_movie->setMovie(movie);
}

void VideoInfoItem::setSize(int size)
{
    size /= 1024 * 1024;
    char c[100] = {0};
    itoa(size, c, 10);
    QString str(c);
    ui->lb_size->setText(QString("%1 M").arg(str));
}

void VideoInfoItem::setInfo(QString name, QMovie* movie, int size, QString rmpt)
{
    m_rmpt = rmpt;
    setName(name);
    setMovie(movie);
    setSize(size);
}
