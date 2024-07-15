#include "myaes.h"
#include<math.h>

MyAES::MyAES()
{
    ase_key_init();
}
int MyAES::aes_encrypt(const char* in,  char* out,int len, char* key)//, int olen)可能会设置buf长度
{
    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
    for(int i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
        iv[i]=0;
    AES_KEY aes;
    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
    //int len=strlen(in);//这里的长度是char*in的长度，但是如果in中间包含'\0'字符的话

    //那么就只会加密前面'\0'前面的一段，所以，这个len可以作为参数传进来，记录in的长度

    //至于解密也是一个道理，光以'\0'来判断字符串长度，确有不妥，后面都是一个道理。
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_ENCRYPT);
    return 1;
}

//int MyAES::aes_encryptWithoutBase(const char *in, char *out, int len, char *key)
//{
//    if(!in || !key || !out) return 0;
//    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
//    for(int i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
//        iv[i]=0;
//    AES_KEY aes;
//    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
//    {
//        return 0;
//    }
//    //int len=strlen(in);//这里的长度是char*in的长度，但是如果in中间包含'\0'字符的话

//    //那么就只会加密前面'\0'前面的一段，所以，这个len可以作为参数传进来，记录in的长度

//    //至于解密也是一个道理，光以'\0'来判断字符串长度，确有不妥，后面都是一个道理。
//    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_ENCRYPT);
//    return 1;
//}
int MyAES::aes_decrypt(const char* in, char* out, int len, char* key)
{
    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
    for(int i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
        iv[i]=0;
    AES_KEY aes;
    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
    //   int len=strlen(in);
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_DECRYPT);
    return 1;
}

//int MyAES::aes_decryptWithoutBase(const char *in, char *out, int len, char *key)
//{
//    if(!in || !key || !out) return 0;
//    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
//    for(int i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
//        iv[i]=0;
//    AES_KEY aes;
//    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
//    {
//        return 0;
//    }
//    //   int len=strlen(in);
//    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_DECRYPT);
//    return 1;
//}
#include<random>
#include<time.h>
void MyAES::ase_key_init()
{
    srand((unsigned int)time(nullptr));
    for(int i = 0; i < AES_BLOCK_SIZE; ++i)
    {
        key[i] = rand() % 256;
    }
}
#include<string.h>

int MyAES::Base64Encode(const unsigned char* in, int len, char* out_base64)
{
    if (!in || len < 0 || !out_base64)
    {
        return 0;
    }

    //创建内存源
    auto mem_bio = BIO_new(BIO_s_mem());
    if (!mem_bio) return 0;
    // base64 filter
    auto b64_bio = BIO_new(BIO_f_base64());//这个接口在头文件 evp.h

    if (!b64_bio)
    {
        BIO_free(mem_bio); //释放申请成功的空间
        return 0;
    }

    // 形成BIO链表
    //b64-mem
    // 形成链表, 往base64内存写数据, 进行编码，结果会传递到链表的下一个节点, 到mem中读取结果
    BIO_push(b64_bio, mem_bio);  // 2个链表（从链表头部，代表整个链表)

    //超过长度不还行
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);

    //write 是编码 3字节 => 4字节 不足3字节补充0 和等于号
    //编码数据每64直接会加一个\n换行符号,并且结尾时也有换行符号

    int re = BIO_write(b64_bio, in, len); //将数据写入到链表头
    if (re < 0)
    {
        // 写入失败, 清空整个链表节点
        BIO_free_all(b64_bio);
        return 0;
    }
    // 刷新缓存， 写入链表的mem
    BIO_flush(b64_bio);

    // 从链表源内存读取
    BUF_MEM* p_data = nullptr; // 需要引入
    BIO_get_mem_ptr(b64_bio, &p_data); //拿到编码数据了
    int out_size = 0;
    if (p_data)
    {
        memcpy(out_base64, p_data->data, p_data->length);
        out_size = p_data->length;
    }
    //执行完后， 清理空间
    BIO_free_all(b64_bio);
    return out_size;
}


int MyAES::Base64Decode(const char* in, int len, unsigned char* out_data)
{
    if (!in || len <= 0 || !out_data)
    {
        return 0;
    }

    // 内存源
    auto mem_bio = BIO_new_mem_buf(in, len);
    if (!mem_bio)
    {
        return 0;
    }
    // base64 过滤器
    auto b64_bio = BIO_new(BIO_f_base64());
    if (!b64_bio)
    {
        BIO_free(mem_bio);
        return 0;
    }

    //形成BIO链条
    BIO_push(b64_bio, mem_bio);
    //取消默认读取换行符号做结束的操作
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);
    //读取 解码 4字节转3字节
    size_t size = 0;
    size =  BIO_read(b64_bio, out_data, len);

    //BIO_read_ex(b64_bio, out_data, len, &size);
    BIO_free_all(b64_bio);
    return size;
}
