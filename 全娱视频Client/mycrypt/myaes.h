#ifndef MYAES_H
#define MYAES_H

#define AES_BITS 128
#define MSG_LEN 128
#define AES_GROUP_BLOCK 16

#include "openssl/aes.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/pem.h>
class MyAES
{
public:
    MyAES();
    //len是输入的长度
    static int aes_encrypt(const char* in,  char* out, int len/*输入的长度*/, char* key);
    //len是输出的长度
    static int aes_decrypt(const char* in,  char* out,int len/*输出的长度*/, char* key);
    void ase_key_init();
    char key[AES_BLOCK_SIZE];
    static int Base64Encode(const unsigned char *in, int len, char *out_base64);
    static int Base64Decode(const char *in, int len, unsigned char *out_data);
};
#endif // MYAES_H
