#ifndef MYAES_H
#define MYAES_H

#define AES_BITS 128
#define MSG_LEN 128
#define AES_GROUP_BLOCK 16

#include "openssl/aes.h"
#include <iostream>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/pem.h>
using namespace std;
class MyAES
{
public:
    MyAES();
    static int aes_encrypt(const char* in,  char* out, int len, char* key);
//    static int aes_encryptWithoutBase(const char* in, char* out, int len, char* key);
    static int aes_decrypt(const char* in,  char* out,int len, char* key);
//    static int aes_decryptWithoutBase(const char* in,  char* out,int len, char* key);
    static int Base64Decode(const char* in, int len, unsigned char* out_data);
    static int Base64Encode(const unsigned char* in, int len, char* out_base64);

    void ase_key_init();
    char key[AES_BLOCK_SIZE] = "";
};
#endif // MYAES_H
