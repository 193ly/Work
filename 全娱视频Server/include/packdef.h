#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "err_str.h"
#include <malloc.h>

#include<iostream>
#include<map>
#include<list>

typedef  long long int lint;
typedef int PackType;
#define DEF_HOBBY_COUNT     (10)
#define MAX_HOBBY_TEXT_SIZE (10)
typedef struct HOBBY{
    HOBBY()
    {
        memset(m_hobby, 0, sizeof(m_hobby));
    }
    char* operator [](int i)
    {
        return m_hobby[i];
    }
    int food = 0;
    int funny = 0;
    int ennegy = 0;
    int dance = 0;
    int music = 0;
    int video = 0;
    int outside = 0;
    int edu = 0;
    char m_hobby[DEF_HOBBY_COUNT][MAX_HOBBY_TEXT_SIZE];
}HOBBY;
//边界值
#define _DEF_SIZE           45
#define _DEF_BUFFERSIZE     1000
#define _DEF_PORT           8000
#define _DEF_SERVERIP       "0.0.0.0"
#define _DEF_LISTEN         128
#define _DEF_EPOLLSIZE      4096
#define _DEF_IPSIZE         16
#define _DEF_COUNT          10
#define _DEF_TIMEOUT        10
#define _DEF_SQLIEN         400
#define TRUE                true
#define FALSE               false

//加密传输
#define PRIVATE_KEY "MIIBIDANBgkqhkiG9w0BAQEFAAOCAQ0AMIIBCAKCAQEAiXr5iB0qIrTtukCqVejf4YTi7RfNRupbaP4qmKTJDWc97AV1CPGYEvoKpkJkL9uGEyE+gbiAdPpJqCv9jRRaXO0DaLNjdRXPT8v1xnjn45PctWV7KlUieCtLJE42wlJx7XZObHAnN8zg8BiXG++U4JuD2MGa5Xm/eEGnvjZkgUOjhYtrjRmha7HLg1bZh12cukFG0b4/K0BKGbYlrsAvL//fFZFThnp9Zb9taeigtXMXOHOYFf6TGouhNL++bTRNG/4lW31WzkUS+SdvnVaayQQyDgORum8BEd9W6sJ9QhffCajryTcknUBwywksy5gnrDe+82EC0PMDVmiO/S2biwIBEQ=="

struct STRU_AES
{
    //秘钥
    char sKey[16] = "";
    //偏移量
    char sIV[16] = "";
};
///////////////redis/////////////
#define RECOMMEND_MAX 15
/*-------------数据库信息-----------------*/
#define _DEF_DB_NAME    "NetDisk"
#define _DEF_DB_IP      "localhost"
#define _DEF_DB_USER    "root"
#define _DEF_DB_PWD     "1935505668"
/*--------------------------------------*/
#define DEF_BUFFER         (4096)
#define DEF_CONTENT_SIZE	(1024)
#define MAX_SIZE           (40)
#define MAX_PATH           (260)
#define MAX_CONTENT_SIZE   (1024 * 10)
#define DEF_HOBBY_COUNT     (10)
#define DEF_HONNY_BLOACK_MAX    (20)
#define DEF_HOBBY_MAX       (100)
#define MAX_FUN_COUNT       (20)
#define LIVE                ("live")
#define LIVE_LEN            (sizeof(LIVE))

//自定义协议   先写协议头 再写协议结构
//登录 注册 获取好友信息 添加好友 聊天 发文件 下线请求
#define _DEF_PACK_BASE	(10000)
#define _DEF_PACK_COUNT (100)

//注册
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )
//登录
#define _DEF_PACK_LOGIN_RQ	(_DEF_PACK_BASE + 2 )
#define _DEF_PACK_LOGIN_RS	(_DEF_PACK_BASE + 3 )
//文件请求
#define DEF_PACK_UPLOAD_RQ  (_DEF_PACK_BASE + 4 )
#define DEF_PACK_UPLOAD_RS  (_DEF_PACK_BASE + 5)
#define DEF_PACK_FILEBLOCK_RQ   (_DEF_PACK_BASE + 6)
//心跳机制
#define DEF_PACK_LIVE_RQ    (_DEF_PACK_BASE + 7)
//下载
#define DEF_PACK_DOWNLOAD_RQ    (_DEF_PACK_BASE + 8)
#define DEF_PACK_DOWNLOAD_RS    (_DEF_PACK_BASE + 9)
//更改名字
#define DEF_PACK_CHANGE_NAME_RQ (_DEF_PACK_BASE + 10)
#define DEF_PACK_CHANGE_NAME_RS (_DEF_PACK_BASE + 11)
//查看已经上传的视频
#define DEF_PACK_HAVEUPLOAD_RQ   (_DEF_PACK_BASE + 12)
#define DEF_PACK_HAVEUPLOAD_RS   (_DEF_PACK_BASE + 13)
//添加热度
#define DEF_PACK_ADDHOT_RQ      (_DEF_PACK_BASE + 14)
#define DEF_PACK_ADDHOT_RS      (_DEF_PACK_BASE + 15)
//下线
#define DEF_PACK_OUTLINE_RQ     (_DEF_PACK_BASE + 16)
//返回的结果
//注册请求的结果
#define register_fail		(-1)
#define register_success	(1)
#define register_had        (0)
//登录请求的结果
#define login_user_not_exist		(0)
#define login_password_error		(1)
#define login_success		(2)
//文件请求的结果
#define send_file_success    (0)
#define send_file_fail      (1)
#define send_file_working  (2)
//改名的请求的结果
#define change_name_success  (1)
#define change_name_fail    (0)


//协议结构
//注册
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
    {
        memset( tel  , 0, sizeof(tel));
        memset( name  , 0, sizeof(name));
        memset( password , 0, sizeof(password) );
    }
    //需要手机号码 , 密码, 昵称
    PackType type;
    char tel[MAX_SIZE];
    char name[MAX_SIZE];
    char password[MAX_SIZE];
    HOBBY hobby;

}STRU_REGISTER_RQ;

typedef struct STRU_REGISTER_RS
{
    //回复结果
    STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_fail), id(-1)
    {
        memset(name, 0, MAX_SIZE);
    }
    PackType type;
    int result;
    char name[MAX_SIZE];
    int id;
}STRU_REGISTER_RS;

//登录
typedef struct STRU_LOGIN_RQ
{
    //登录需要: 手机号 密码
    STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
    {
        memset( tel , 0, sizeof(tel) );
        memset( password , 0, sizeof(password) );
    }
    PackType type;
    char tel[MAX_SIZE];
    char password[MAX_SIZE];

}STRU_LOGIN_RQ;

typedef struct STRU_LOGIN_RS
{
    // 需要 结果 , 用户的id
    STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(-1)
    {
        memset(name, 0, MAX_SIZE);
    }
    PackType type;
    int result;
    int userid;
    char name[MAX_SIZE];
}STRU_LOGIN_RS;

//文件传输请求
typedef struct STRU_UPLOAD_RQ
{
    STRU_UPLOAD_RQ():m_nType(DEF_PACK_UPLOAD_RQ), m_nFileId(0),m_UserId(0)
    {
        memset(m_szFileType , 0 ,MAX_SIZE);
        memset(m_szGifName, 0 ,MAX_SIZE);
        memset(m_szFileName , 0 ,MAX_PATH);
    }
    PackType m_nType; //包类型
    int m_UserId; //用于查数据库, 获取用户名字, 拼接路径
    int m_nFileId; //区分不同文件, 可采用 md5 或 随机数 用户同时只能传一个所以相同概率较低
    int64_t m_nFileSize; //文件大小, 用于文件传输结束
//    int m_nAllSize;
    HOBBY m_szHobby;//喜好标签
    char m_szFileName[MAX_PATH]; //文件名, 用于存储文件
    char m_szGifName[MAX_PATH]; //动画名字, 方便直接写入数据库
    char m_szFileType[MAX_SIZE]; //用于区分视频和图片
}STRU_UPLOAD_RQ;
//文件回复请求
typedef struct STRU_UPLOAD_RS
{
    STRU_UPLOAD_RS():m_nType(DEF_PACK_UPLOAD_RS), m_nResult(send_file_fail)
    {
        m_nResult = 0;
    }
    PackType m_nType; //包类型
    int m_nResult;
//    float rate;
}STRU_UPLOAD_RS;
//文件块请求
//
typedef struct STRU_FILEBLOCK_RQ
{
    STRU_FILEBLOCK_RQ():m_nType(DEF_PACK_FILEBLOCK_RQ)
    {
        m_nUserId = 0;
        m_nFileId =0;
        m_nBlockLen =0;
        memset(m_szFileContent,0, MAX_CONTENT_SIZE);
        m_pos = 0;
    }
    PackType m_nType; //包类型
    int m_nUserId; //用户 ID
    int m_nFileId; //文件 id 用于区分文件
    int m_nBlockLen; //文件写入大小
    char m_szFileContent[MAX_CONTENT_SIZE];
    long long m_pos;
}STRU_FILEBLOCK_RQ;
//文件信息
typedef struct STRU_FILEINFO
{
public:
    STRU_FILEINFO():m_nFileID(0),m_VideoID(0),m_nFileSize(0),m_nPos(0),
        m_nUserId(0),pFile(nullptr)
    {
        memset(m_szFilePath, 0 , MAX_PATH);
        memset(m_szFileName, 0 , MAX_PATH);
        memset(m_szGifPath , 0 , MAX_PATH );
        memset(m_szGifName, 0 , MAX_PATH);
        memset(m_szFileType, 0 , MAX_SIZE);
        memset(m_UserName, 0 , MAX_SIZE);
        memset(m_szRtmp ,0 , MAX_PATH);
    }
    int m_nFileID;//下載的時候是用來做 UI 控件編號的， 上傳的時候是一個隨機數， 區分文件。
    int m_VideoID;//真是文件 ID 與 Mysql 的一致
    int64_t m_nFileSize;
    int64_t m_nPos;
    int m_nUserId;
//    int m_nAllSize;
  //  int m_nAllPos;
    FILE* pFile;
    char m_szFilePath[MAX_PATH];
    char m_szFileName[MAX_PATH];
    char m_szGifPath[MAX_PATH];
    char m_szGifName[MAX_PATH];
    char m_szFileType[MAX_SIZE];
    HOBBY m_Hobby;
    char m_UserName[MAX_SIZE];
    char m_szRtmp[MAX_PATH];
}FileInfo;
//回复movie
typedef struct STRU_LIVE_RQ
{
    STRU_LIVE_RQ():m_nType(DEF_PACK_LIVE_RQ)
    {
        memcpy(live, LIVE, LIVE_LEN);
        m_id = -1;
    }
    PackType m_nType;
    char live[LIVE_LEN];
    int m_id;
}STRU_LIVE_RQ;
//下载文件请求
typedef struct STRU_DOWNLOAD_RQ
{
    STRU_DOWNLOAD_RQ()
    {
        m_nType = DEF_PACK_DOWNLOAD_RQ;
        m_nUserId = 0;
    }
    PackType m_nType; //包类型
    int m_nUserId; //用户 ID
}STRU_DOWNLOAD_RQ;
//下载文件回复
typedef struct STRU_DOWNLOAD_RS
{
    STRU_DOWNLOAD_RS():m_nType(DEF_PACK_DOWNLOAD_RS), m_nFileId(0)
    {
        memset(m_szFileName , 0 ,MAX_PATH);
        memset(m_rtmp , 0 ,MAX_PATH);
    }
    PackType m_nType; //包类型
    int m_nFileId;
    int64_t m_nFileSize;
    int m_nVideoId;
    char m_szFileName[MAX_PATH];
    char m_rtmp[MAX_PATH]; // 播放地址 如//1/103.MP3 用户本地需要转化为 rtmp://服务器 ip/app 名/ + 这个字符串 //本项目 app 名为 vod
}STRU_DOWNLOAD_RS;
typedef struct STRU_CHANGE_NAME_RQ
{
    STRU_CHANGE_NAME_RQ():m_nType(DEF_PACK_CHANGE_NAME_RQ),m_id(0)
    {
        memset(name, 0, MAX_SIZE);
    }
    PackType m_nType;
    int m_id;
    char name[MAX_SIZE];
}STRU_CHANGE_NAME_RQ;
//发送改名的回复
typedef struct STRU_CHANGE_NAME_RS
{
    STRU_CHANGE_NAME_RS():m_nType(DEF_PACK_CHANGE_NAME_RS),result(change_name_fail)
    {
        memset(newName, 0, MAX_SIZE);
    }
    PackType m_nType;
    char newName[MAX_SIZE];
    int result;
}STRU_CHANGE_NAME_RS;
//查看已经上传的文件
typedef struct STRU_HAVEUPLOAD_RQ
{
    STRU_HAVEUPLOAD_RQ():m_nType(DEF_PACK_HAVEUPLOAD_RQ), m_userId(0)
    {

    }
    PackType m_nType;
    int m_userId;
}STRU_HAVEUPLOAD_RQ;

typedef struct STRU_HAVEUPLOAD_RS
{
    STRU_HAVEUPLOAD_RS():m_nType(DEF_PACK_HAVEUPLOAD_RS), m_nFileId(0), m_nFileSize(0),m_nVideoId(0),m_nGifSize(0)
    {
        memset(m_szFileName, 0, MAX_PATH);
        memset(m_rtmp, 0, MAX_PATH);
        memset(m_szGifName, 0, MAX_PATH);

    }
    PackType m_nType; //包类型
    int m_nFileId;
    int64_t m_nFileSize;
    int m_nVideoId;
    int64_t m_nGifSize;
    char m_szFileName[MAX_PATH];
    char m_szGifName[MAX_PATH];
    char m_rtmp[MAX_PATH]; // 播放地址 如//1/103.MP3 用户本地需要转化为 rtmp://服务器 ip/app 名/ + 这个字符串 //本项目 app 名为 vod
}STRU_HAVEUPLOAD_RS;
//增加热度的结构
typedef struct STRU_ADDHOT_RQ
{
    STRU_ADDHOT_RQ():m_nType(DEF_PACK_ADDHOT_RQ), id(0)
    {
        memset(m_rtmp, 0, MAX_PATH);
    }
    PackType m_nType; //包类型
    int id;
    char m_rtmp[MAX_PATH];
}STRU_ADDHOT_RQ;
typedef struct STRU_OUTLINE_RQ
{
    STRU_OUTLINE_RQ(){
        m_nType = DEF_PACK_OUTLINE_RQ;
        id = 0;
    }
    PackType m_nType;
    int id;
}STRU_OUTLINE_RQ;
