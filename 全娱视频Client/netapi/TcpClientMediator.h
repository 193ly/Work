#pragma once

#include"INetMediator.h"
#include<string>
#include"mycrypt/myaes.h"
#include"mycrypt/myrsa.h"
using namespace std;
class TcpClientMediator :public INetMediator
{
//    Q_OBJECT
public:
	TcpClientMediator(void);
	~TcpClientMediator(void);

	//初始化网络
    bool OpenNet(const char* szBufIP = "0.0.0.0"
                    , unsigned short port = _DEF_TCP_PORT );
	//关闭网络
	void CloseNet(); 
    //发送加密秘钥
    //bool SendKey(unsigned int lSendIP , char* buf , int nlen);
	//发送 : 同时兼容tcp udp 
    bool SendData( unsigned int lSendIP , char* buf , int nlen );
	//接收
    void DealData( unsigned int lSendIP , char* buf , int nlen );

    bool SendAES();

    void disConnect();
    bool IsConnected();

private:
    MyAES* aes = nullptr;
};

