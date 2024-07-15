#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H


#include"block_epoll_net.h"
#include "Mysql.h"
#include<list>
#include<unordered_map>

//cryption
#include"mycrypt/myaes.h"
#include"mycrypt/myrsa.h"

//redis
#include"myredis/myredis.h"

//类成员函数指针 , 用于定义协议映射表
class TcpKernel;
typedef int sock_fd;


#define NetPackMap(a)  TcpKernel::GetInstance()->m_NetPackMap[ a - _DEF_PACK_BASE ]

class CLogic;
typedef void (TcpKernel::*PFUN)(sock_fd,char*,int nlen);

class TcpKernel
{
public:
    //单例模式
    static TcpKernel* GetInstance();

    //开启核心服务
    int Open(int port);
    //初始化随机数
    void initRand();
    //设置协议映射
    void setNetPackMap();
    //关闭核心服务
    void Close();
    //处理网络接收
    static void DealData(sock_fd clientfd, char*szbuf, int nlen);
    //事件循环
    void EventLoop();
    //发送数据
    void SendData( sock_fd clientfd, char*szbuf, int nlen );
    //注册
    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);
    //登录
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);

    void UploadRq(int clientId, char* buff, int len);

    void UploadBlockRq(int clientId, char* buff, int len);

    void DownloadRq(int clientId, char* buff, int len);

    void LiveRq(int clientId, char* buff, int len);

    void ChangeNameRq(int client, char* buff, int len);

    void HaveUploadRq(int clientId, char* buff, int len);

    void AddHotRq(int clientId, char* buff, int len);

    void OutLineRq(int clientId, char* buff, int len );
private:
    void GetFileList(list<FileInfo*> &list, int id);
    list<FileInfo *> GetFileListByFileId(const list<string>& fileIdList, int id);
    void GetHaveUploadVideoList(list<FileInfo *> &fileList, int id);

    list<string> GetVideoIdByRedis(int id);

    void MyRedisComment_updataUserLableByLable(int id, list<string>& lableList);
private:
    TcpKernel();
    ~TcpKernel();
    //数据库
    CMysql * m_sql;
    //网络
    Block_Epoll_Net * m_tcp;
    //协议映射表
    PFUN m_NetPackMap[_DEF_PACK_COUNT];

    CLogic* m_logic;
    friend class CLogic;
    map<int, FileInfo*> m_mapUsertoFileInfo;
    //map<int, FileInfo*> m_mapFileIDToFileInfo;
    static unordered_map<long, MyAES*> mapIpToAES;
    //static Rsa_PrivateKey privateKey;  //base64解密后的秘钥
    static MyRSA* rsa;

    //redis
    MyRedis* redis = nullptr;
    redisContext *m_redisContext = nullptr;
};

#endif
