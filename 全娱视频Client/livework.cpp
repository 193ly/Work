#include "livework.h"
#include<QDebug>
LiveWork::LiveWork(QObject *parent) : QThread(parent),m_quit(false)
{

}

LiveWork::~LiveWork()
{
    if(!m_quit)
        m_quit= true;
    this->quit();
    this->wait(100);
    if(this->isRunning())
    {
        this->terminate();
        this->wait(100);
    }
}

void LiveWork::run()
{
    STRU_LIVE_RQ rq;
    rq.m_id = m_id;
    while(!m_quit)
    {
        if(m_tcp->SendData(0, (char*)&rq, sizeof(rq)) < 0)
            qDebug() << "live send error";
        else
            qDebug() << "live send sucess";
        QThread::sleep(5);
    }
}

void LiveWork::init(TcpClientMediator *tcp, int id)
{
    m_tcp = tcp;
    m_id = id;
}
