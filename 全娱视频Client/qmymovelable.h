#ifndef QMYMOVELABLE_H
#define QMYMOVELABLE_H

#include <QWidget>
#include<QMovie>
#include<QEvent>
#include<QObject>
namespace Ui {
class QMyMoveLable;
}

class QMyMoveLable : public QWidget
{
    Q_OBJECT

public:
    explicit QMyMoveLable(QWidget *parent = nullptr);
    ~QMyMoveLable();
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    QMovie* getMovie();
public:
    void setMovie(QMovie* movie);
    void setUrl(QString url);
signals:
    SIG_playUrl(QString);
private:
    Ui::QMyMoveLable *ui;
    QString m_rmptUrl;
    QMovie* m_movie;
};

#endif // QMYMOVELABLE_H
