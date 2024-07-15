TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


#LIBS += -L$$PWD/openssl/lib/ -llibssl
#LIBS += -L$$PWD/openssl/lib/ -llibcrypto

LIBS += -L/usr/lib/ssl -lssl

LIBS += -L/usr/lib/ssl -lcrypto

LIBS += -L/usr/local/lib -lhiredis




INCLUDEPATH += $$PWD/openssl/include
DEPENDPATH += $$PWD/openssl/include

INCLUDEPATH += ./include

LIBS += -lpthread\
        -lmysqlclient\

SOURCES += \
    mycrypt/myaes.cpp \
    mycrypt/myrsa.cpp \
    myredis/myredis.cpp \
    src/Mysql.cpp \
    src/TCPKernel.cpp \
    src/Thread_pool.cpp \
    src/block_epoll_net.cpp \
    src/clogic.cpp \
    src/err_str.cpp \
    src/main.cpp

HEADERS += \
    include/Mysql.h \
    include/TCPKernel.h \
    include/Thread_pool.h \
    include/block_epoll_net.h \
    include/clogic.h \
    include/err_str.h \
    include/packdef.h \
    mycrypt/myaes.h \
    mycrypt/myrsa.h \
    myredis/myredis.h
