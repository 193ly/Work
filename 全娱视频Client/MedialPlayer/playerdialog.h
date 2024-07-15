#pragma once
#include <QDialog>
#include"videoplayer.h"
#include<QObject>
#include<QDebug>
#include<QTimer>
#include"myshowwidget.h"
namespace Ui {
class PlayerDialog;
}

class PlayerDialog : public QDialog
{
    Q_OBJECT

public:
    void closeEvent(QCloseEvent *);
    explicit PlayerDialog(QWidget *parent = 0);
    ~PlayerDialog();
    bool eventFilter(QObject *, QEvent *);
    //void paintEvent(QPaintEvent *event);
public slots:
    void slot_PlayerStateChanged(int state);
    void slots_getOneImage(QImage img);
    void slot_getTotalTime(qint64 uSec);
signals:
private slots:
    //void on_pb_play_clicked();

    void on_pb_start_clicked();

    void on_pb_play_clicked();

    void on_pb_pause_clicked();

    void on_pb_stop_clicked();

    void slot_TimerTimeOut();

    void slot_showAndPlay(QString path);
private:
    Ui::PlayerDialog *ui;
    VideoPlayer* m_player;
    QTimer m_timer;
    bool isStop;
    bool isTimerStop;
    bool isPause;
    QPixmap pix;
};

