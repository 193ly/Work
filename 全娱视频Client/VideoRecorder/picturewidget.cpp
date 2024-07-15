#include "picturewidget.h"
#include "ui_picturewidget.h"
#include<QPixmap>
PictureWidget::PictureWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PictureWidget)
{
    ui->setupUi(this);
    //无边框   |   一直在最上面
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}

PictureWidget::~PictureWidget()
{
    delete ui;
}

void PictureWidget::slot_setImage(QImage img)
{
    QPixmap pixmap;
    if(!img.isNull())
    {
        pixmap = QPixmap::fromImage(img.scaled(ui->lb_showImage->size(), Qt::KeepAspectRatio));//保持缩放比的方式来填充
    }
    else
    {
        pixmap = QPixmap::fromImage(img);
    }
    ui->lb_showImage->setPixmap(pixmap);
}
