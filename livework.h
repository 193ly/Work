#ifndef LIVEWORK_H
#define LIVEWORK_H

#include <QThread>
#include <QObject>
#include "packdef.h"
#include"TcpClientMediator.h"
class LiveWork : public QThread
{
    Q_OBJECT
public:
    explicit LiveWork(QObject *parent = nullptr);
    ~LiveWork();
    void run();
    void init(TcpClientMediator* tcp, int id);
private:
    TcpClientMediator* m_tcp;
    bool m_quit;
    int m_id;
};

#endif // LIVEWORK_H
