#include "TcpClientMediator.h"

#include"TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
    //new 网络对象
    m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator()  //使用时, 父类指针指向子类, 使用虚析构
{
    if( m_pNet )
    {
        delete m_pNet;
        m_pNet = NULL;
    }
}
//初始化网络
bool TcpClientMediator::OpenNet(const char *szBufIP, unsigned short port)
{

    strcpy_s( m_szBufIP, sizeof(m_szBufIP), szBufIP);
    m_port = port;

    if(m_pNet->InitNet(szBufIP , port))
        return SendAES(); //连接的同时发送秘钥
    else
        return false;
}
//关闭网络
void TcpClientMediator::CloseNet()
{
    m_pNet->UnInitNet();
}
#include"net/packdef.h"
//bool TcpClientMediator::SendKey(unsigned int lSendIP, char *buf, int nlen)
//{
//    //    //对公钥进行 Base64编码 可以用于传输存储
//    //    std::string strPublic = PUBLIC_KEY;
//    //    //打印编码后的公钥
//    //    std::cout << "base64后的公钥:" << strPublic.c_str()<< std::endl;
//    //    // 公钥 Base64解码 获得公钥
//    //    Rsa_PublicKey publickey =  CMyRsa::RSA_DecodePublicKey( strPublic );

//    //    //RSA 利用公钥对明文加密
//    //    string plaintext(buf);
//    //    std::string ciphertext = CMyRsa::RSA_OAEP_Encrypt_Public(plaintext ,  publickey );
//    //    std::cout << "Ciphertext: " << ciphertext.c_str()<< std::endl;

//    //    Q_UNUSED(lSendIP);
//    //    if( IsConnected() )
//    //        return m_pNet->SendData( 0 , (char*)ciphertext.c_str(), ciphertext.size());
//    //    else
//    //    {
//    //        m_pNet->UnInitNet();
//    //        delete m_pNet;
//    //        m_pNet = new TcpClient(this);
//    //        if( this->OpenNet( m_szBufIP , m_port ) )
//    //        {
//    //            return m_pNet->SendData( 0,(char*)ciphertext.c_str(),ciphertext.size());
//    //        }
//    //        else
//    //        {
//    //            return -1;
//    //        }
//    //    }
//}
#include<iostream>
#include<math.h>
#include<QDebug>
using namespace std;
//发送 : 同时兼容tcp udp 
bool TcpClientMediator::SendData( unsigned int lSendIP , char* buf , int nlen )
{
    Q_UNUSED(lSendIP);
    //std::string cipherText = CMyAes::AES_CBC_Encryption(key, iv, buf);
    int lenBase = ceil((double)nlen / 3) * 4;
    char* base_out = new char[lenBase]{0};
    lenBase = MyAES::Base64Encode((unsigned char*)buf, nlen, base_out);
    //qDebug() << "buse_out =" <<  QString(base_out);
    int len = lenBase % AES_GROUP_BLOCK == 0 ? lenBase : (lenBase / AES_GROUP_BLOCK + 1) * AES_GROUP_BLOCK;
    char* en = new char[len]{0};
    aes->aes_encrypt(base_out, en, lenBase, aes->key);
    //qDebug() << "en = " <<  QString(en);

    delete[] base_out;
    base_out = nullptr;

    int re = false;
    if( IsConnected() )
    {
        re = m_pNet->SendData( 0,(char*)en,len);

        //        char* base_out1 = new char[len]{0};
        //        MyAES::aes_decrypt(en, base_out1, len, aes->key);
        //        printf("base_out1 = %s\n", base_out1);
        //        int i = strcmp(base_out, base_out1);
        //        int lastLen = strlen(base_out1);
        //        if(lastLen > len)
        //            lastLen = len;
        //        char* de = new char[lastLen]{0};
        //        int trueLen = MyAES::Base64Decode(base_out1, lastLen, (unsigned char*)de);

        //        delete[] base_out1;
        //        base_out1 = nullptr;
        //        return true;
    }
    else
    {
        m_pNet->UnInitNet();
        delete m_pNet;
        m_pNet = new TcpClient(this);
        if( this->OpenNet( m_szBufIP , m_port ) )
        {
            /*

            */
            re = m_pNet->SendData( 0,(char*)en,len);
        }
        else
        {
            re = false;
        }
    }
    delete[] en;
    en = nullptr;

    return re;
}
#include<iostream>
//处理 
void TcpClientMediator::DealData(unsigned int lSendIP , char* buf , int nlen )
{
    cout << endl << __FUNCTION__ << "  buf " << buf << endl;

    char* base_out1 = new char[nlen]{0};
    MyAES::aes_decrypt(buf, base_out1, nlen, aes->key);
    //printf("base_out1 = %s\n", base_out1);
    int baseLen = strlen(base_out1);
    if(baseLen > nlen)
    {
        baseLen = nlen;
    }
    char* de = new char[baseLen]{0};
    baseLen = MyAES::Base64Decode(base_out1, baseLen, (unsigned char*)de);
    delete[] base_out1;

    memcpy(buf, de, baseLen);
    delete[] de;
    //将这几个参数 以信号的形式发送出去  -- IKernel类 的槽函数处理 -- 注意空间回收
    //std::cout<< lSendIP << ":" << buf <<std::endl;
    Q_EMIT SIG_ReadyData( lSendIP , buf , baseLen);
}

bool TcpClientMediator::SendAES()
{
    if(!aes)
    {
        aes = new MyAES();
    }
    char tmp[AES_BLOCK_SIZE] = "";
    memcpy(tmp, aes->key, AES_BLOCK_SIZE);
    cout << "aes = " << aes->key << endl;

    MyRSA rsa ;
    string en = rsa.rsaPubEncrypt(aes->key);
    cout << "en = " <<en << endl << endl;
    string de = rsa.rsaPriDecrypt(en);
    cout <<"de = " <<de << endl;
    if(!m_pNet->SendData(0, (char*)en.c_str(), en.size()))
        return false;
}

void TcpClientMediator::disConnect()
{
    Q_EMIT SIG_disConnect();
}

bool TcpClientMediator::IsConnected()
{
    return m_pNet->IsConnected();
}

