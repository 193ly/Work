#ifndef VIDEOINFOITEM_H
#define VIDEOINFOITEM_H

#include <QWidget>
#include<QMovie>
namespace Ui {
class VideoInfoItem;
}

class VideoInfoItem : public QWidget
{
    Q_OBJECT

public:
    explicit VideoInfoItem(QWidget *parent = nullptr);
    ~VideoInfoItem();
    void setName(QString name);
    void setMovie(QMovie* movie);
    void setSize(int size);
    void setInfo(QString name, QMovie* moive, int size, QString rmpt);
private:
    Ui::VideoInfoItem *ui;

    QString m_rmpt;
};

#endif // VIDEOINFOITEM_H
