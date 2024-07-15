QT       += core gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#include(./cryptopp/cryptopp.pri)

#INCLUDEPATH += $$PWD/cryptopp/cryptopp565-release/include

#LIBS += -L$$PWD/cryptopp/cryptopp565-release/bin/ -lcryptopp565

LIBS += -L$$PWD/openssl/lib/ -llibssl


LIBS += -L$$PWD/openssl/lib/ -llibcrypto


INCLUDEPATH += $$PWD/openssl/include
DEPENDPATH += $$PWD/openssl/include

include(./netapi/netapi.pri)

INCLUDEPATH += ./netapi/ \
               ./MedialPlayer/ \
               ./VideoRecorder


SOURCES += \
    MedialPlayer/PackQueue.cpp \
    MedialPlayer/myshowwidget.cpp \
    MedialPlayer/playerdialog.cpp \
    MedialPlayer/videoplayer.cpp \
    VideoRecorder/audio_read.cpp \
    VideoRecorder/picinpic_read.cpp \
    VideoRecorder/picturewidget.cpp \
    VideoRecorder/recorderdialog.cpp \
    VideoRecorder/savevideofilethread.cpp \
    changename.cpp \
    ckernel.cpp \
    haveuploaddialog.cpp \
    hobbylinedialog.cpp \
    livework.cpp \
    logindialog.cpp \
    main.cpp \
    maindialog.cpp \
    mycrypt/myaes.cpp \
    mycrypt/myrsa.cpp \
    onlinedialog.cpp \
    qmymovelable.cpp \
    uploaddialog.cpp \
    videoinfoitem.cpp \
    videoitem.cpp

HEADERS += \
    MedialPlayer/PackQueue.h \
    MedialPlayer/myshowwidget.h \
    MedialPlayer/playerdialog.h \
    MedialPlayer/videoplayer.h \
    VideoRecorder/audio_read.h \
    VideoRecorder/common.h \
    VideoRecorder/picinpic_read.h \
    VideoRecorder/picturewidget.h \
    VideoRecorder/recorderdialog.h \
    VideoRecorder/savevideofilethread.h \
    changename.h \
    ckernel.h \
    haveuploaddialog.h \
    hobbylinedialog.h \
    livework.h \
    logindialog.h \
    maindialog.h \
    mycrypt/myaes.h \
    mycrypt/myrsa.h \
    onlinedialog.h \
    qmymovelable.h \
    uploaddialog.h \
    videoinfoitem.h \
    videoitem.h

FORMS += \
    MedialPlayer/playerdialog.ui \
    VideoRecorder/picturewidget.ui \
    VideoRecorder/recorderdialog.ui \
    changename.ui \
    haveuploaddialog.ui \
    hobbylinedialog.ui \
    logindialog.ui \
    maindialog.ui \
    onlinedialog.ui \
    qmymovelable.ui \
    uploaddialog.ui \
    videoinfoitem.ui \
    videoitem.ui

include(./opengl/opengl.pri)

INCLUDEPATH += ./opengl/

INCLUDEPATH += $$PWD/ffmpeg-4.2.2/include\
 C:/Qt/opencv-release/include/opencv2\
 C:/Qt/opencv-release/include\
 $$PWD/SDL2-2.0.10/include

LIBS += $$PWD/ffmpeg-4.2.2/lib/avcodec.lib\
 $$PWD/ffmpeg-4.2.2/lib/avdevice.lib\
 $$PWD/ffmpeg-4.2.2/lib/avfilter.lib\
 $$PWD/ffmpeg-4.2.2/lib/avformat.lib\
 $$PWD/ffmpeg-4.2.2/lib/avutil.lib\
 $$PWD/ffmpeg-4.2.2/lib/postproc.lib\
 $$PWD/ffmpeg-4.2.2/lib/swresample.lib\
 $$PWD/ffmpeg-4.2.2/lib/swscale.lib\
 $$PWD/SDL2-2.0.10/lib/x86/SDL2.lib

LIBS+= C:\Qt\opencv-release\lib\libopencv_*.dll.a
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
