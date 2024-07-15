#include<TCPKernel.h>
#include "packdef.h"
#include<stdio.h>
#include<sys/time.h>
#include"clogic.h"
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
using namespace std;

#define RootPath "/home/colin/video/"

//Rsa_PrivateKey TcpKernel::privateKey = CMyRsa::RSA_DecodePrivateKey(string(PRIVATE_KEY));
//设置网络协议映射
void TcpKernel::setNetPackMap()
{
    //清空映射
    bzero( m_NetPackMap , sizeof(m_NetPackMap) );

    //协议映射赋值
    // temporary
    //m_logic->setNetPackMap();
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &TcpKernel::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &TcpKernel::LoginRq;
    NetPackMap(DEF_PACK_UPLOAD_RQ) = &TcpKernel::UploadRq;
    NetPackMap(DEF_PACK_FILEBLOCK_RQ) = &TcpKernel::UploadBlockRq;
    NetPackMap(DEF_PACK_DOWNLOAD_RQ) = &TcpKernel::DownloadRq;
    NetPackMap(DEF_PACK_LIVE_RQ) = &TcpKernel::LiveRq;
    NetPackMap(DEF_PACK_CHANGE_NAME_RQ) = &TcpKernel::ChangeNameRq;
    NetPackMap(DEF_PACK_HAVEUPLOAD_RQ) = &TcpKernel::HaveUploadRq;
    NetPackMap(DEF_PACK_ADDHOT_RQ) = &TcpKernel::AddHotRq;
    NetPackMap(DEF_PACK_OUTLINE_RQ) = &TcpKernel::OutLineRq;
}


TcpKernel::TcpKernel()
{
    TcpKernel::rsa = MyRSA::GetRSA();
    m_redisContext = redisConnect("127.0.0.1", 6379);
    if (m_redisContext == NULL || m_redisContext->err) {
        if (m_redisContext) {
            printf("Error: %s\n", m_redisContext->errstr);
            redisFree(m_redisContext);

        } else {
            printf("Can't allocate redis context\n");
        }
        exit(1);
    }
}

TcpKernel::~TcpKernel()
{
    if( m_logic ) delete m_logic;
}

TcpKernel *TcpKernel::GetInstance()
{
    static TcpKernel kernel;
    return &kernel;
}

int TcpKernel::Open( int port)
{
    initRand();

    m_sql = new CMysql;
    // 数据库 使用127.0.0.1 地址  用户名root 密码colin123 数据库 wechat 没有的话需要创建 不然报错
    if(  !m_sql->ConnectMysql( _DEF_DB_IP , _DEF_DB_USER, _DEF_DB_PWD, _DEF_DB_NAME )  )
    {
        printf("Conncet Mysql Failed...\n");
        return FALSE;
    }
    else
    {
        printf("MySql Connect Success...\n");
    }
    //初始网络
    m_tcp = new Block_Epoll_Net;
    bool res = m_tcp->InitNet( port , &TcpKernel::DealData ) ;
    if( !res )
        err_str( "net init fail:" ,-1);

    m_logic = new CLogic(this);

    setNetPackMap();

    return TRUE;
}

void TcpKernel::Close()
{
    for(auto ite = m_mapUsertoFileInfo.begin(); ite != m_mapUsertoFileInfo.end(); ite++)
        delete ite->second;
    m_sql->DisConnect();
    m_mapUsertoFileInfo.clear();
    delete m_tcp;

}

//随机数初始化
void TcpKernel::initRand()
{
    struct timeval time;
    gettimeofday( &time , NULL);
    srand( time.tv_sec + time.tv_usec );
}
unordered_map<long, MyAES*> TcpKernel::mapIpToAES;
MyRSA* TcpKernel::rsa = nullptr;
//unordered_map<long, STRU_AES> TcpKernel::mapIpToAES;
void TcpKernel::DealData(sock_fd clientfd,char *szbuf,int nlen)
{
    if(TcpKernel::mapIpToAES.find(clientfd) == TcpKernel::mapIpToAES.end())
    {
        cout << "len = " << nlen << endl;
        char buff[257] = "";
        memcpy(buff, szbuf, 256);
        for (int i = 0; i < 256; i++)
        {
            buff[i] = szbuf[i];
        }
        string en;
        for(int i = 0; i < 256; i++)
            en += buff[i];
        cout << "en = " << en << endl;
        cout << "en size = " << en.size() << endl;

        string aesKey = MyRSA::rsaPriDecrypt(en);
        MyAES* key = new MyAES;
        for (int i = 0; i < 16; i++)
        {
            key->key[i] = aesKey[i];
        }
        cout << clientfd <<" aes key = ";
        cout << key->key << endl;
        TcpKernel::mapIpToAES[clientfd] = key;
        return;

        //       std::string decryptedtext = CMyRsa::RSA_OAEP_Decrypt_Private(szbuf, privateKey);
        //std::cout << "Decrypted text: " << decryptedtext.c_str() << std::endl;
        //     STRU_AES aes = *(STRU_AES*)decryptedtext.c_str();
        //   mapIpAES.insert(pair<long, STRU_AES>(clientfd, aes));
        // return;
    }
    MyAES* aes = TcpKernel::mapIpToAES[clientfd];

    char* base_out1 = new char[nlen]{0};
    MyAES::aes_decrypt(szbuf, base_out1, nlen, aes->key);
    //printf("base_out1 = %s\n", base_out1);
    int baseLen = strlen(base_out1);
    if(baseLen > nlen)
    {
        baseLen = nlen;
    }
    char* de = new char[baseLen]{0};
    int deLen = MyAES::Base64Decode(base_out1, baseLen, (unsigned char*)de);

    delete[] base_out1;
    base_out1 = nullptr;

    PackType type = *(PackType*)de;
    cout << type << endl;
    if( (type >= _DEF_PACK_BASE) && ( type < _DEF_PACK_BASE + _DEF_PACK_COUNT) )
    {
        PFUN pf = NetPackMap( type );
        if( pf )
        {
            // temporary
            //(TcpKernel::GetInstance()->m_logic->*pf)( clientfd , szbuf , nlen);
            (TcpKernel::GetInstance()->*pf)(clientfd, de, deLen);
        }
    }
    else
    {
        cout << endl;
        cout << "have no the type " << endl << endl;
    }
    delete[] de;
    de = nullptr;
    //    if(szbuf)
    //        delete szbuf;
    //    return;
}

void TcpKernel::EventLoop()
{
    printf("event loop\n");
    m_tcp->EventLoop();
}
#include<string.h>
void TcpKernel::SendData(sock_fd clientfd, char *szbuf, int nlen)
{
    if(TcpKernel::mapIpToAES.find(clientfd) != TcpKernel::mapIpToAES.end())
    {
        //        int type = *(int*)szbuf;
        //        cout << "type = " << type << endl;
        //        cout << endl << szbuf << endl;
        //        MyAES* aes = TcpKernel::mapIpToAES[clientfd];
        //        //char* en = (char*)malloc(nlen);
        //        //memset(en, 0, nlen);
        //        char* en = new char[nlen]{0};
        //        MyAES::aes_encrypt(szbuf, en, nlen, aes->key);

        //        cout << endl << en << endl;
        //        //memcpy(szbuf, en, nlen);
        int lenBase = ceil((double)nlen / 3) * 4;
        char* base_out = new char[lenBase]{0};
        lenBase = MyAES::Base64Encode((unsigned char*)szbuf, nlen, base_out);
        int blockLen = lenBase % AES_GROUP_BLOCK == 0 ? lenBase : (lenBase / AES_GROUP_BLOCK + 1) * AES_GROUP_BLOCK;

        char* buf = new char[blockLen]{0};
        MyAES::aes_encrypt(base_out, buf, lenBase, TcpKernel::mapIpToAES[clientfd]->key);
        m_tcp->SendData(clientfd , buf, blockLen);
        delete[] buf;
        buf = nullptr;
        delete[] base_out;
        base_out = nullptr;
    }
    else
    {
        cout << "have no aes, send error" << endl;
    }

}

//注册
void TcpKernel::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    //cout << "clientfd:"<< clientfd << __func__ << endl;
    STRU_REGISTER_RQ rq = *(STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS rs;
    rs.result = register_success;
    cout <<__func__;
    char sql[1024] = {0};
    list<string> reList;
    sprintf(sql, "select id from t_UserData where tel = '%s';", rq.tel);
    if(m_sql->SelectMysql(sql, 1, reList) < 0)
    {
        cout<<"select error" << endl;
        rs.result = register_fail;
    }
    if(reList.size() != 0)
    {
        rs.result = register_had;
    }
    else
    {
        ///////////////////set mysql//////////////////
        char buf[1024] = "";
        sprintf(buf, "insert into t_UserData(name, tel, password) values('%s','%s','%s')",rq.name,rq.tel, rq.password);
        if(!m_sql->UpdataMysql(buf))
        {
            rs.result = register_fail;
            cout << "register fail" << endl;
            exit(0);
        }
        ///////////////////set redis////////////////////
        //find userid
        list<string> re;
        sprintf(buf, "select id from t_UserData where tel = '%s';", rq.tel);
        if(!m_sql->SelectMysql(buf, 1, re))
        {
            cout << "select id error" << endl;
            exit(0);
        }
        int id = atoi(re.front().c_str());
        //set redis
        HOBBY hobby = rq.hobby;
        sprintf(buf, "zadd userid:%d:items", id);
        string redisBuf(buf);
        for(int i = 0; hobby[i][0] != 0; i++)
        {
            redisBuf += " ";
            redisBuf += hobby[i];
        }
        redisReply* rly = (redisReply*)redisCommand(m_redisContext, redisBuf.c_str());
        if(rly == nullptr || rly->elements == 0)
        {
            cout << "redis error" << endl;
            exit(0);
        }
        if(rly)
        {
            freeReplyObject(rly);
        }
        memcpy(rs.name, rq.name, MAX_SIZE);
        rs.id = id;
    }
    m_tcp->SendData(clientfd, (char*)&rs, sizeof(rs));
}

void TcpKernel::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
    //    cout << "clientfd:"<< clientfd << __func__ << endl;
    cout<< __func__;
    STRU_LOGIN_RQ rq = *(STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;

    char sql[1024] = {0};
    list<string> reList;
    sprintf(sql, "select name from t_UserData where tel = '%s';", rq.tel);
    if(m_sql->SelectMysql(sql, 1, reList) < 0)
    {
        cout << "select error" << endl;
        return;
    }
    if(reList.size() != 0)
    {
        reList.clear();
        sprintf(sql, "select id, name from t_UserData where tel = '%s' and password = '%s';", rq.tel, rq.password);
        if(m_sql->SelectMysql(sql, 2, reList) < 0)
        {
            cout << "select error" << endl;
            return;
        }
        if(reList.size() == 0)
            rs.result = login_password_error;
        else
        {
            rs.result = login_success;
            rs.userid = atoi(reList.front().c_str());
            reList.pop_front();
            memcpy(rs.name, reList.front().c_str(), MAX_SIZE);
            reList.clear();
        }
    }
    else {
        rs.result = login_user_not_exist;
    }
    SendData( clientfd , (char*)&rs , sizeof rs );
}

void TcpKernel::UploadRq(int clientId, char *buff, int len)
{
    cout << "recv uploadRs1" << endl;
    printf("recv from id = %d\n", clientId);
    cout << __func__;
    STRU_UPLOAD_RQ* rq = (STRU_UPLOAD_RQ*)buff;
    FileInfo* info = nullptr;
    if(m_mapUsertoFileInfo.find(rq->m_UserId) == m_mapUsertoFileInfo.end())
    {
        info = new FileInfo;
        //       info->m_nAllSize = rq->m_nAllSize;
        info->m_nFileID = rq->m_nFileId;
        info->m_nUserId = rq->m_UserId;
        info->m_nFileSize = rq->m_nFileSize;
        info->m_nPos = 0;
        strcpy(info->m_szGifName, rq->m_szGifName);
        m_mapUsertoFileInfo[rq->m_UserId] = info;
    }
    else
    {
        info = m_mapUsertoFileInfo[rq->m_UserId];
        memcpy((char*)&info->m_Hobby, (char*)&rq->m_szHobby, sizeof(rq->m_szHobby));
        info->m_nFileID = rq->m_nFileId;
        info->m_VideoID = info->m_nFileID;
        info->m_nUserId = rq->m_UserId;
        info->m_nFileSize = rq->m_nFileSize;
        strcpy(info->m_szFileName, rq->m_szFileName);
        info->m_nPos = 0;
        //m_mapFileIDToFileInfo[info->m_nFileID] = info;
    }
    strcpy(info->m_szFileType, rq->m_szFileType);
    cout << "recv uploadRs2" << endl;
    //info->m_szFilePath;
    //find user name
    //select mysql
    char sqlstr[1024] = "";
    sprintf(sqlstr, "select name from t_UserData where id = %d;", rq->m_UserId);
    list<string> resList;
    if(!m_sql->SelectMysql(sqlstr, 1, resList))
    {
        cout << "error" << endl;
        return;
    }
    if(resList.size() <= 0)
    {
        cout << "have no the user" << endl;
        return;
    }
    if(strcmp(info->m_szFileType, "gif") != 0)
    {
        strcpy(info->m_UserName, resList.front().c_str());
        strcpy(info->m_szFileName, rq->m_szFileName);
        sprintf(info->m_szFilePath, "%sflv/%s/%s", RootPath, info->m_UserName, info->m_szFileName);
        sprintf(info->m_szRtmp, "//%s/%s", info->m_UserName, info->m_szFileName);
        info->pFile = fopen(info->m_szFilePath, "wb");
    }
    else
    {
        strcpy(info->m_UserName, resList.front().c_str());
        strcpy(info->m_szGifName, rq->m_szGifName);
        sprintf(info->m_szGifPath, "%sflv/%s/%s", RootPath, info->m_UserName, info->m_szGifName);

        info->pFile = fopen(info->m_szGifPath, "wb");
    }
    fseek(info->pFile, info->m_nFileSize, SEEK_SET);
    fseek(info->pFile, 0, SEEK_SET);

    if(info->pFile == nullptr)
    {
        cout << "create file error" << endl;
        return;
    }
    ////////////////////set mysql/////////////////////////
    if(strcmp(info->m_szFileType, "gif") != 0)
    {
        sprintf(sqlstr, "insert into t_VideoInfo (userId , videoName ,picName ,videoPath , picPath, rtmp,hotdegree) values (%d, '%s', '%s','%s', '%s', '%s', 0);",
                rq->m_UserId, info->m_szFileName, info->m_szGifName,
                info->m_szFilePath, info->m_szGifPath,info->m_szRtmp);
        list<string> relist;
        if(!m_sql->UpdataMysql(sqlstr))
        {
            cout << "updata error" << endl;
            exit(0);
        }
        sprintf(sqlstr, "select videoid from t_VideoInfo where videoPath = '%s' and userId = %d;", info->m_szFilePath,info->m_nUserId);
        if(!m_sql->SelectMysql(sqlstr, 1, relist))
        {
            cout <<"select error" << endl;
            exit(0);
        }
        info->m_VideoID = atoi(relist.back().c_str());
    }
    /////////////////////////set redis////////////////////
    /// \brief memset
    if(strcmp(info->m_szFileType, "gif") != 0)
    {
        memset(sqlstr, 0, sizeof(sqlstr));
        list<string> reList;

        memset(sqlstr, 0, sizeof(sqlstr));
        sprintf(sqlstr, "sadd videoid:%d:items", info->m_VideoID);
        int i = 0;
        for(i = 0; info->m_Hobby[i][0] != 0; i++)
        {
            strcat(sqlstr, " ");
            strcat(sqlstr, info->m_Hobby[i]);
        }
        if(i != 0)
        {
            freeReplyObject(redisCommand(m_redisContext, "del videoid:%d:items", info->m_VideoID));
            redisReply* rly = (redisReply*)redisCommand(m_redisContext, sqlstr);
            freeReplyObject(rly);
        }
    }


    ////////////////////////////////////////////////////////

    //    cout << "recv uploadRs----------------" << endl;
    //    cout << info->m_nFileID <<endl;
    //    cout << info->m_szFileName <<endl;
    //    cout << info->m_szGifName <<endl;
    //    cout << info->m_szFilePath <<endl;
    //    cout << info->m_szGifPath <<endl;
    //    cout << info->m_szFileType <<endl;
    //    cout << info->pFile <<endl;
}

void TcpKernel::UploadBlockRq(int clientId, char *buff, int len)
{
    printf("client id = %d\n", clientId);
    cout << __func__;
    STRU_FILEBLOCK_RQ* rq = (STRU_FILEBLOCK_RQ*)buff;
    if(m_mapUsertoFileInfo.find(rq->m_nUserId) == m_mapUsertoFileInfo.end())
    {
        cout << "have no the file id" << endl;
        return;
    }
    FileInfo* info = m_mapUsertoFileInfo[rq->m_nUserId];
    fseek(info->pFile, rq->m_pos - rq->m_nBlockLen, SEEK_SET);
    int64_t res = fwrite(rq->m_szFileContent,1, rq->m_nBlockLen, info->pFile);

    info->m_nPos += res;
    //info->m_nAllPos += res;
    //send upload Rs

    if(info->m_nPos >= info->m_nFileSize)
    {
        info->m_nPos = 0;
        //analysis have gif
        if(strcmp( info->m_szFileType, "gif") != 0)
        {
            char sqlstr[1024] = {0};

            STRU_UPLOAD_RS rs;
            /////////////////////set mysql video info//////////////////

            //            ///////////////////set redis video lable///////////////
            //            memset(sqlstr, 0, sizeof(sqlstr));
            //            list<string> reList;
            ////            sprintf(sqlstr, "select videoid from t_VideoInfo where videoPath = '%s' and ;", info->m_szFilePath);
            ////            if(!m_sql->SelectMysql(sqlstr, 1, reList))
            ////            {
            ////                cout << "select error" << endl;
            ////                exit(0);
            ////            }
            ////            info->m_VideoID = atoi(reList.begin()->c_str());
            //            memset(sqlstr, 0, sizeof(sqlstr));
            //            sprintf(sqlstr, "sadd videoid:%d:items", info->m_VideoID);
            //            int i = 0;
            //            for(i = 0; info->m_Hobby[i][0] != 0; i++)
            //            {
            //                strcat(sqlstr, " ");
            //                strcat(sqlstr, info->m_Hobby[i]);
            //            }
            //            if(i != 0)
            //            {
            //                freeReplyObject(redisCommand(m_redisContext, "del videoid:%d:items", info->m_VideoID));
            //                redisReply* rly = (redisReply*)redisCommand(m_redisContext, sqlstr);
            //                freeReplyObject(rly);
            //            }

            //            ////////////////////////////////////////////////////////
            m_tcp->SendData(clientId, (char*)&rs, sizeof(rs));
            //m_mapFileIDToFileInfo.erase(rq->m_nFileId);

            fclose(info->pFile);
            delete info;
            info = nullptr;
            m_mapUsertoFileInfo.erase(rq->m_nUserId);
        }
        else
        {
            fclose(info->pFile);
        }
        //if(strlen(info->m_szGifName) != 0)
    }
}

void TcpKernel::DownloadRq(int clientId, char *buff, int len)
{
    printf("downloadRq, client = %d\n", clientId);
    STRU_DOWNLOAD_RQ rq = *(STRU_DOWNLOAD_RQ*)buff;
    /////////////////////////find Fileinfo/////////////////
    list<FileInfo*> fileList;
    //GetFileList(fileList, rq.m_nUserId);
    list<string> strList = GetVideoIdByRedis(rq.m_nUserId);
    cout << "video id = ";
    for(auto& ite: strList)
    {
        cout << ite << endl;
    }
    char buf[1024] = "";
    int fileId = 1;
    for(string& s: strList)
    {
        list<string> temp;
        sprintf(buf, "select videoid,userId,videoName,picName,videoPath,picPath,rtmp from t_VideoInfo where videoId = %s;", s.c_str());
        if(!m_sql->SelectMysql(buf, 7,temp))
        {
            cout << "select error" << endl;
            exit(0);
        }
        FileInfo* info = new FileInfo;
        info->m_nFileID = fileId++;
        info->m_nUserId = atoi(temp.front().c_str());
        temp.pop_front();
        info->m_nUserId = atoi(temp.front().c_str());
        temp.pop_front();
        strcpy(info->m_szFileName, temp.front().c_str());
        temp.pop_front();
        strcpy(info->m_szGifName, temp.front().c_str());
        temp.pop_front();
        strcpy(info->m_szFilePath, temp.front().c_str());
        temp.pop_front();
        strcpy(info->m_szGifPath, temp.front().c_str());
        temp.pop_front();
        strcpy(info->m_szRtmp, temp.front().c_str());
        temp.pop_front();

        info->pFile = fopen(info->m_szGifPath, "rb");
        fseek(info->pFile, 0, SEEK_END);
        info->m_nFileSize = ftell(info->pFile);
        fseek(info->pFile, 0, SEEK_SET);
        fileList.push_back(info);
    }
    /////////////////////////send file////////////////////
    while(fileList.size() > 0)
    {
        FileInfo* info = fileList.front();
        fileList.pop_front();

        STRU_DOWNLOAD_RS rs;
        strcpy(rs.m_rtmp,info->m_szRtmp);
        rs.m_nFileId = info->m_nFileID;
        rs.m_nVideoId = info->m_VideoID;
        rs.m_nFileSize = info->m_nFileSize;
        strcpy(rs.m_szFileName, info->m_szGifName);

        m_tcp->SendData(clientId, (char*)&rs, sizeof(rs));
        usleep(1000 * 100);

        if(info->pFile)
        {
            while(1)
            {
                STRU_FILEBLOCK_RQ blockRq;
                int res = fread(blockRq.m_szFileContent, 1, MAX_CONTENT_SIZE, info->pFile);
                info->m_nPos += res;

                blockRq.m_nUserId = rq.m_nUserId;
                blockRq.m_nFileId = info->m_VideoID;
                blockRq.m_nBlockLen = res;
                m_tcp->SendData(clientId, (char*)&blockRq, sizeof(blockRq));

                if(info->m_nPos >= info->m_nFileSize || res < MAX_CONTENT_SIZE)
                {
                    fclose(info->pFile);
                    delete info;
                    info = nullptr;
                    break;
                }
            }
        }
    }
}

void TcpKernel::LiveRq(int clientId, char *buff, int len)
{
    STRU_LIVE_RQ rq = *(STRU_LIVE_RQ*)buff;
    cout << "live client id = "<< rq.m_id << endl;
}

void TcpKernel::ChangeNameRq(int client, char *buff, int len)
{
    STRU_CHANGE_NAME_RQ rq = *(STRU_CHANGE_NAME_RQ*)buff;
    STRU_CHANGE_NAME_RS rs;
    if(strlen(rq.name) != 0)
    {
        char sql[1024] = {0};
        list<string> reList;
        sprintf(sql, "update t_UserData set name = '%s' where id = %d;", rq.name, rq.m_id);
        if(m_sql->UpdataMysql(sql) <= 0)
        {
            cout << "updata error" << endl;
            rs.result = change_name_fail;
        }
        else
        {
            memcpy(rs.newName, rq.name, MAX_SIZE);
            rs.result = change_name_success;
        }
    }
    else
    {
        rs.result = change_name_fail;
    }
    m_tcp->SendData(client, (char*)&rs, sizeof(rs));
}

void TcpKernel::HaveUploadRq(int clientId, char *buff, int len)
{
    printf("HaveUploadRq, client = %d\n", clientId);
    STRU_HAVEUPLOAD_RQ rq = *(STRU_HAVEUPLOAD_RQ*)buff;
    list<FileInfo*> fileList;

    GetHaveUploadVideoList(fileList, rq.m_userId);
    if(fileList.size() == 0)
    {
        STRU_HAVEUPLOAD_RS rs;
        rs.m_nFileId = -1; //respond have no file found
        m_tcp->SendData(clientId, (char*)&rs, sizeof(rs));
    }
    while(fileList.size() > 0)
    {
        FileInfo* info = fileList.front();
        fileList.pop_front();

        STRU_HAVEUPLOAD_RS rs;
        strcpy(rs.m_rtmp,info->m_szRtmp);
        rs.m_nFileId = info->m_nFileID;
        rs.m_nVideoId = info->m_VideoID;
        rs.m_nGifSize = info->m_nFileSize; //gif size
        strcpy(rs.m_szFileName, info->m_szFileName);
        strcpy(rs.m_szGifName, info->m_szGifName);

        //set the size of file
        FILE* f = fopen(info->m_szFilePath, "r");
        fseek(f, 0, SEEK_END); //file size
        rs.m_nFileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
        fclose(f);

        m_tcp->SendData(clientId, (char*)&rs, sizeof(rs));
        usleep(1000 * 100);
        info->pFile = fopen(info->m_szGifPath, "r");

        if(info->pFile)
        {
            while(1)
            {
                STRU_FILEBLOCK_RQ blockRq;
                int res = fread(blockRq.m_szFileContent, 1, MAX_CONTENT_SIZE, info->pFile);
                info->m_nPos += res;

                blockRq.m_nUserId = rq.m_userId;
                blockRq.m_nFileId = info->m_VideoID;
                blockRq.m_nBlockLen = res;
                m_tcp->SendData(clientId, (char*)&blockRq, sizeof(blockRq));

                if(info->m_nPos >= info->m_nFileSize || res < MAX_CONTENT_SIZE)
                {
                    fclose(info->pFile);
                    delete info;
                    info = nullptr;
                    break;
                }
            }
        }
    }
}

void TcpKernel::AddHotRq(int clientId, char *buff, int len)
{
    printf("AddHotRq, client = %d\n", clientId);
    STRU_ADDHOT_RQ rq = *(STRU_ADDHOT_RQ*)buff;
    cout << rq.m_rtmp;
    ////////////////set vodeo hot//////////////////
    char sql[1024] = "";
    sprintf(sql, "update t_VideoInfo set hotdegree = hotdegree + 1 where rtmp = '%s';", rq.m_rtmp);
    if(!m_sql->UpdataMysql(sql))
    {
        cout << "updata error" << endl;
    }

    ////////////////set user lable/////////////////


    list<string> videoidList;
    sprintf(buff, "select videoid from t_VideoInfo where rtmp = '%s';", rq.m_rtmp);
    if(!m_sql->SelectMysql(buff, 1, videoidList))
    {
        cout << "error select" << endl;
        exit(0);
    }
    redisReply* rly = (redisReply*)redisCommand(m_redisContext, "smembers videoid:%d:items", videoidList.front().c_str());
    videoidList.clear();
    for(int i = 0; i < (int)rly->elements; i++)
    {
        videoidList.push_back(rly->element[i]->str);
    }
    freeReplyObject(rly);
    MyRedisComment_updataUserLableByLable(rq.id, videoidList);
}

void TcpKernel::OutLineRq(int clientId, char *buff, int len)
{
    printf("OutLineRq, client = %d\n", clientId);

    STRU_OUTLINE_RQ rq = *(STRU_OUTLINE_RQ*)buff;
    int id = rq.id;
    //clear aes
    auto aesIte = mapIpToAES.find(clientId);
    if(aesIte != mapIpToAES.end())
    {
        delete aesIte->second;
        mapIpToAES.erase(aesIte);
    }
    //clear file
    auto infoIte = m_mapUsertoFileInfo.find(id);
    if(infoIte != m_mapUsertoFileInfo.end())
    {
        if(infoIte->second->pFile)
            fclose(infoIte->second->pFile);
        delete[] infoIte->second;
        m_mapUsertoFileInfo.erase(infoIte);
    }


}
#include<fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
void TcpKernel::GetFileList(list<FileInfo*>& fileList, int id)
{
    char sql[1024] = {0};
    sprintf(sql,"select count(videoid) from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from t_UserRecv where t_UserRecv.userId = %d ) order by hotdegree desc limit 0,10;", id);
    list<string> reList;
    int nCount;
    if(!m_sql->SelectMysql(sql, 1, reList))
    {
        cout << 1 <<endl;
        printf("select error\n");
        return;
    }

    nCount = atoi(reList.front().c_str());
    if(nCount == 0)
    {
        sprintf(sql, "delete from t_UserRecv where userId = %d", id);
        if(!m_sql->UpdataMysql(sql))
        {
            printf("updata error\n");
            return;
        }
    }
    reList.clear();
    sprintf(sql,"select videoid ,picName , picPath , rtmp from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from t_UserRecv where t_UserRecv.userId = %d ) order by hotdegree desc limit 0,10;", id);
    if(!m_sql->SelectMysql(sql, 4, reList))
    {
        cout << 2 <<endl;
        printf("select error\n");
        return ;
    }

    nCount = 1;
    int len = reList.size() / 4;
    for(int i = 0; i < len; i++)
    {
        FileInfo* info = new FileInfo;
        info->m_VideoID = atoi(reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szGifName, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szGifPath, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szRtmp, reList.front().c_str());
        reList.pop_front();

        info->m_nFileID = nCount++;
        info->pFile = fopen(info->m_szGifPath, "r");
        info->m_nPos = 0;

        fseek(info->pFile, 0, SEEK_END);
        info->m_nFileSize = ftell(info->pFile);
        fseek(info->pFile, 0, SEEK_SET);
        fclose(info->pFile);
        info->pFile = NULL;

        fileList.push_back(info);


        sprintf(sql, "insert into t_UserRecv values(%d ,%d);", id, info->m_VideoID);

        if(!m_sql->UpdataMysql(sql))
        {
            printf("insert error\n");
            return;
        }

    }
}

list<FileInfo *> TcpKernel::GetFileListByFileId(const list<string> &fileIdList,int id)
{
    char sql[1024] = {0};
    list<string> reList;
    list<FileInfo*> re;
    for(const string& s: fileIdList)
    {
        list<string> temp;
        sprintf(sql,"select videoid ,picName , picPath , rtmp from t_VideoInfo where t_VideoInfo.videoId = %s order by hotdegree desc limit 0,%d;", s.c_str(), RECOMMEND_MAX);
        if(!m_sql->SelectMysql(sql, 4, temp))
        {
            cout << "select error" <<endl;
            exit(0);
        }
        for(string& s: temp)
        {
            reList.push_back(s);
        }
    }
    int len = reList.size() / 4;
    int nCount = 0;
    for(int i = 0; i < len; i++)
    {
        FileInfo* info = new FileInfo;
        info->m_VideoID = atoi(reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szGifName, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szGifPath, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szRtmp, reList.front().c_str());
        reList.pop_front();

        info->m_nFileID = nCount++;
        info->pFile = fopen(info->m_szGifPath, "r");
        info->m_nPos = 0;

        fseek(info->pFile, 0, SEEK_END);
        info->m_nFileSize = ftell(info->pFile);
        fseek(info->pFile, 0, SEEK_SET);
        fclose(info->pFile);
        info->pFile = NULL;

        re.push_back(info);


        sprintf(sql, "insert into t_UserRecv values(%d ,%d);", id, info->m_VideoID);

        if(!m_sql->UpdataMysql(sql))
        {
            printf("insert error\n");
            exit(0);
        }
    }
    return re;
}
void TcpKernel::GetHaveUploadVideoList(list<FileInfo*>& fileList, int id)
{
    char sql[1024] = {0};
    sprintf(sql,"select videoid , videoName, picName, videoPath, picPath , rtmp , hotdegree from t_VideoInfo where userId = %d;", id);
    list<string> reList;

    if(!m_sql->SelectMysql(sql, 6, reList))
    {
        printf("select error\n");
        return;
    }

    int nCount = 1;
    int len = reList.size() / 6;
    for(int i = 0; i < len; i++)
    {
        FileInfo* info = new FileInfo;
        info->m_VideoID = atoi(reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szFileName, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szGifName, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szFilePath, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szGifPath, reList.front().c_str());
        reList.pop_front();

        strcpy(info->m_szRtmp, reList.front().c_str());
        reList.pop_front();

        info->m_nFileID = nCount++;
        info->pFile = fopen(info->m_szGifPath, "r");
        info->m_nPos = 0;
        fseek(info->pFile, 0, SEEK_END);
        info->m_nFileSize = ftell(info->pFile);
        fseek(info->pFile, 0, SEEK_SET);
        fclose(info->pFile);
        info->pFile = NULL;

        fileList.push_back(info);
    }
}
list<string> TcpKernel::GetVideoIdByRedis(int id)
{
    cout << __FUNCTION__<< endl;
    char buf[1024] = "";
    /////////////////////find user id///////////////////

    vector<string> userLableList;
    //    redisReply* reply = (redisReply*)redisCommand(m_redisContext, "ZRANGE userid:%d:items 0 -1", id);
    //    for(int j = 0; j < reply->elements; ++j) {
    //        //qDebug()<<reply->element[j]->str;
    //        userLableList.push_back(reply->element[j]->str);
    //    }
    //    freeReplyObject(reply);
    char buff[100] = "";
    sprintf(buff, "userid:%d:items", id);
    userLableList = Recommend(m_redisContext, buff);
    if(userLableList.size() == 0)
        return list<string>();
    ////////////////////find video id//////////////////
    for(string& s: userLableList)
    {
        redisReply* reply = (redisReply*)redisCommand(m_redisContext, "sadd videotemp:%d %s", id, s.c_str()); //create temp set
        freeReplyObject(reply);
    }
    list<string> re;
    int offset = 0 * RECOMMEND_MAX;
    while(1)
    {
        sprintf(buf, "select videoid from t_VideoInfo limit %d offset %d;", RECOMMEND_MAX, offset);
        list<string> videoList;
        if(!m_sql->SelectMysql(buf,1, videoList))
        {
            cout << "select videoid error" << endl;
            exit(0);
        }
        redisReply* reply;
        for(string& s:videoList)
        {
            reply = (redisReply*)redisCommand(m_redisContext, "sinter videotemp:%d videoid:%s:items", id, s.c_str());
            if(reply->elements > 0)
            {
                re.push_back(s);
                offset++;
            }
            freeReplyObject(reply);
        }
        if(videoList.size() < RECOMMEND_MAX || re.size() >= RECOMMEND_MAX)
            break;
    }
    freeReplyObject((redisReply*)redisCommand(m_redisContext, "spop videotemp:%d %d", id, DEF_HOBBY_MAX));
    return re;
}

void TcpKernel::MyRedisComment_updataUserLableByLable(int id, list<string> &lableList)
{
    cout << __FUNCTION__ << endl;
    char buf[100] = "";
    //redisReply* replay = nullptr;
    for(auto& s: lableList)
    {
        //        sprintf(buf, "ZSCORE userid:U%d:items %s", id, s.c_str());

        //        //search key
        //        replay = (redisReply*)redisCommand(m_redisContext, buf);

        //        //add count of key
        //        if(replay->len > 0)
        //        {
        if(s.size() + sizeof("ZINCRBY userid:2147483547:items 1 ") > sizeof(buf))
        {
            cout << "error: too long lable" << endl;
            break;
        }
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "ZINCRBY userid:%d:items 1 %s", id, s.c_str());
        freeReplyObject(redisCommand(m_redisContext, buf));
        //        }
    }

}
