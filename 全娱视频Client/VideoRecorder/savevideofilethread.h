#ifndef SAVEVIDEOFILETHREAD_H
#define SAVEVIDEOFILETHREAD_H

#include <QThread>
#include <QObject>
#include"picinpic_read.h"
#include"audio_read.h"
#include<QMutex>
#include<QList>
typedef struct OutputStream
{
    AVStream* st;
    AVCodecContext *enc;
    int64_t next_pts;
    int samples_count;
    AVFrame *frame;
    uint8_t* frameBuffer;
    int frameBufferSize;
    AVFrame *tmp_frame;
    float t,tincr, tincr2;
    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
}OutputStream;
struct STRU_AV_FORMAT
{
    void clear()
    {
        memset(this, 0, sizeof(STRU_AV_FORMAT));
    }
    bool hasCamera;
    bool hasDesk;
    bool hasAudio;

    ////////视频信息//////
    int width;
    int height;
    int frame_rate;
    int videoBitRate;
    int codec_id;
    QString fileName;
    //int pix_fmt;//像素格式 YUV420默认
    //int code_id;  //编码器  H.264默认

    ////////音频信息///////
    ///
    ///
};
struct BufferDataNode
{
    uint8_t * buffer;
    int bufferSize;
    int64_t time; //视频帧用于稳帧, 比较时间
};
class SaveVideoFileThread : public QThread
{
    Q_OBJECT
signals:
    void SIG_sendVideoFrame(QImage img);
    void SIG_sendPicInPic(QImage img);
public slots:
    //void slot_writeVideoFrameDate(uint8_t* picture_buf, int buff_size);//采集的数据格式
    void slot_setInfo(STRU_AV_FORMAT& avFr);
    void slot_openVideo();
    void slot_closeVideo();
    void slot_writeAudioFrameData(uint8_t *picture_buf, int buffer_size);

    void slot_writeVideoFrameData(uint8_t *picture_buf, int buffer_size);
public:
    SaveVideoFileThread();
    void add_video_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, AVCodecID codec_id);
    void close_stream(AVFormatContext *oc, OutputStream *ost);
    int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c, AVStream *st, AVFrame *frame);
    void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost);
    const STRU_AV_FORMAT& avFromat() const;
    void add_audio_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, AVCodecID codec_id);
    void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost);
    void videoDataQuene_Input(uint8_t *buffer, int size, int64_t time);
    BufferDataNode *videoDataQuene_get(int64_t time);
    void audioDataQuene_Input(const uint8_t *buffer, const int &size);
    BufferDataNode *audioDataQuene_get();
    bool write_video_frame(AVFormatContext *oc, OutputStream *ost, double time);
    int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c, AVStream *st, AVFrame *frame, int64_t &pts, OutputStream *ost);
    bool write_audio_frame(AVFormatContext *oc, OutputStream *ost);
    //void slot_writeVideoFrameData(uint8_t *picture_buf, int buffer_size);
private:
    void run();
    PicInPic_Read* m_picInPicRead;
    Audio_Read* m_audioRead;
    STRU_AV_FORMAT m_avFormat;
    OutputStream video_st = {0},audio_st = {0};
    const char* fileName;
    AVOutputFormat* fmt;
    AVFormatContext *oc;
    AVCodec* audio_codec, *video_codec;
    int ret;
    int have_video = 0,have_audio = 0;
    int encode_video = 0, encode_audio = 0;
    AVDictionary* opt = NULL;

    int mAudioOneFrameSize = 0;


    QList<BufferDataNode*> m_videoDataList;
    QList<BufferDataNode*> m_audioDataList;
    BufferDataNode* lastVideoNode;
    bool m_videoBeginFlag;
    qint64 m_videoBeginTime;
    QMutex m_videoMutex;
    QMutex m_audioMutex;
    bool isStop;
    int64_t video_pts;
    int64_t audio_pts;
};

#endif // SAVEVIDEOFILETHREAD_H
