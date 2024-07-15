#ifndef RECORDERDIALOG_H
#define RECORDERDIALOG_H

#include <QDialog>
#include<QImage>
#include"picturewidget.h"
#include"savevideofilethread.h"
QT_BEGIN_NAMESPACE
namespace Ui { class RecorderDialog; }
QT_END_NAMESPACE

class RecorderDialog : public QDialog
{
    Q_OBJECT

public:
    RecorderDialog(QWidget *parent = nullptr);
    ~RecorderDialog();


private slots:

    void on_pb_setUrl_clicked();

    void on_pb_stop_clicked();

    void on_pb_start_clicked();

    void slot_setImage(QImage img);
    void on_cb_desktop_stateChanged(int arg1);

    void on_cb_camera_stateChanged(int arg1);

    void on_cb_audio_stateChanged(int arg1);

private:
    Ui::RecorderDialog *ui;
    PictureWidget* m_pictureWidget;
    SaveVideoFileThread* m_saveVideoFileThread;
    QString m_saveUrl;

    bool hasAudio;
    bool hasVideo;
    bool hasDesk;
};
#endif // RECORDERDIALOG_H
