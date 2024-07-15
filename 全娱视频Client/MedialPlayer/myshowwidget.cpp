#include "myshowwidget.h"

MyShowWidget::MyShowWidget(QWidget *parent) : QWidget(parent)
{

}
#include<QPainter>

void MyShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(), this->height());
    int x = this->width() - img.width();
    int y = this->height() - img.height();
    x /= 2;
    y /= 2;
    painter.drawImage( x,y,img,img.width(),img.height());
    painter.end();
}
