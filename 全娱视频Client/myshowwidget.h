#ifndef MYSHOWWIDGET_H
#define MYSHOWWIDGET_H

#include <QWidget>
#include<QImage>
#include<QPaintEvent>
class MyShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MyShowWidget(QWidget *parent = nullptr);
    void SetImage(QImage imgg)
    {
        img = imgg.copy();
        this->repaint();
    }
    void paintEvent(QPaintEvent *event);
signals:

private:
    QImage img;

};

#endif // MYSHOWWIDGET_H
