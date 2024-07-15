#define _CRT_SECURE_NO_WARNINGS 1
#include<QFile>
#include<QIODevice>
#include "myrsa.h"
#include<string.h>
#define uchar unsigned char
MyRSA::MyRSA()
{
    rsa_key_init();
}
string MyRSA::private_key = "";
string MyRSA::public_key = "";
MyRSA* MyRSA::GetRSA()
{
    static MyRSA rsa;
    return &rsa;
}
bool MyRSA::rsa_key_init()
{
    QFile file1("./public_key.pem");
    file1.open(QIODevice::ReadOnly);
    public_key = file1.readAll().toStdString();
    file1.close();

    QFile file2("./private_key.pem");
    file2.open(QIODevice::ReadOnly);
    private_key = file2.readAll().toStdString();
    file2.close();
}
string MyRSA::rsaPubEncrypt(const string&str)
{
    char* ciphertext = NULL;
    int len = 0;
    int ret = 0;

    BIO* pKeyBio = BIO_new_mem_buf((uchar*)public_key .c_str(), public_key.size());
    if (pKeyBio == NULL) {
        return "";
    }

    //*注解1
    //rsa = PEM_read_RSAPublicKey(file, NULL, NULL, NULL);
    RSA* rsa = RSA_new();
    rsa = PEM_read_bio_RSAPublicKey(pKeyBio, &rsa, NULL, NULL);
    if (rsa == NULL) {
        ERR_print_errors_fp(stdout);
        return std::string();
    }

    len = RSA_size(rsa);
    ciphertext = (char*)malloc(len + 1);
    if (ciphertext == NULL) {
        RSA_free(rsa);
        return std::string();
    }
    memset(ciphertext, 0, len + 1);

    //*注解2
    ret = RSA_public_encrypt(str.length(), (unsigned char*)str.c_str(), (unsigned char*)ciphertext, rsa, RSA_PKCS1_PADDING);
    if (ret < 0) {
        ERR_print_errors_fp(stdout);
        free(ciphertext);
        RSA_free(rsa);
        return std::string();
    }

    //*注解3
    //std::string s(ciphertext);    //不能使用这个构造函数，有的密文使用这个构造函数构造出的string会缺失部分数据，导致无法解密
    std::string s(ciphertext, ret);

    free(ciphertext);
    RSA_free(rsa);
    BIO_free_all(pKeyBio);

    return s;
}
string MyRSA::rsaPriDecrypt(const std::string& str)
{
    RSA* rsa = NULL;
    char* plaintext = NULL;
    int len = 0;
    int ret = 0;


    BIO* pKeyBio = BIO_new_mem_buf((uchar*)private_key.c_str(), private_key.size());
    if (pKeyBio == nullptr)
        return "";
    //*注解4
    rsa = RSA_new();
    rsa = PEM_read_bio_RSAPrivateKey(pKeyBio, &rsa, NULL, NULL);
    if (rsa == NULL) {
        ERR_print_errors_fp(stdout);
        return std::string();
    }

    len = RSA_size(rsa);
    plaintext = (char*)malloc(len + 1);
    if (plaintext == NULL) {
        RSA_free(rsa);
        return std::string();
    }
    memset(plaintext, 0, len + 1);

    //*注解5
    ret = RSA_private_decrypt(str.length(), (unsigned char*)str.c_str(), (unsigned char*)plaintext, rsa, RSA_PKCS1_PADDING);
    if (ret < 0) {
        ERR_print_errors_fp(stdout);
        free(plaintext);
        RSA_free(rsa);
        return std::string();
    }
    std::string s(plaintext, ret);

    free(plaintext);
    RSA_free(rsa);
    BIO_free_all(pKeyBio);
    return s;
}
