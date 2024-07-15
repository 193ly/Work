#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QWidget>

namespace Ui {
class VideoItem;
}
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include"libswresample/swresample.h"
#include"libavutil/time.h"
}
class VideoItem : public QWidget
{
    Q_OBJECT

public:
    explicit VideoItem(QWidget *parent = nullptr);
    ~VideoItem();

private:
    Ui::VideoItem *ui;
};

#endif // VIDEOITEM_H
