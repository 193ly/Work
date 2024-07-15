#include "clogic.h"

void CLogic::setNetPackMap()
{
//    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
//    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
//    NetPackMap(DEF_PACK_UPLOAD_RQ) = &TcpKernel::UploadBlockRq;
//    NetPackMap(DEF_PACK_DOWNLOAD_RQ) = &TcpKernel::DownloadRq;
}

#define _DEF_COUT_FUNC_    cout << "clientfd:"<< clientfd << __func__ << endl;

////注册
//void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
//{
//    //cout << "clientfd:"<< clientfd << __func__ << endl;
//    _DEF_COUT_FUNC_
   
//}

////登录
//void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
//{
////    cout << "clientfd:"<< clientfd << __func__ << endl;
//    _DEF_COUT_FUNC_

//    STRU_LOGIN_RS rs;
//    SendData( clientfd , (char*)&rs , sizeof rs );
//}
