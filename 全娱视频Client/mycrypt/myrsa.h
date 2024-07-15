#ifndef RSA_H
#define RSA_H

#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#define BEGIN_RSA_PUBLIC_KEY    "BEGIN RSA PUBLIC KEY"
#define BEGIN_RSA_PRIVATE_KEY   "BEGIN RSA PRIVATE KEY"
#define BEGIN_PUBLIC_KEY        "BEGIN PUBLIC KEY"
#define BEGIN_PRIVATE_KEY       "BEGIN PRIVATE KEY"
#define KEY_LENGTH              1024
#include<stdlib.h>
using namespace std;
class MyRSA
{
private:
public:
    MyRSA();
    static MyRSA* GetRSA();
    static bool rsa_key_init();
    static string rsaPubEncrypt(const string&strPlainData);

    static string rsaPriDecrypt(const string&strDecryptData);
    static string private_key;
    static string public_key;
};

#endif // RSA_H
