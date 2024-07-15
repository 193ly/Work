#ifndef MYREDIS_H
#define MYREDIS_H
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <hiredis/hiredis.h>
#include <unordered_map>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
class MyRedis
{
public:

    MyRedis(){}

    ~MyRedis()
    {
        this->_connect = NULL;
        this->_reply = NULL;
    }

    bool connect(std::string host, int port)
    {
        this->_connect = redisConnect(host.c_str(), port);
        if(this->_connect != NULL && this->_connect->err)
        {
            printf("connect error: %s\n", this->_connect->errstr);
            return 0;
        }
        return 1;
    }

    std::string get(std::string key)
    {
        this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
        std::string str = this->_reply->str;
        freeReplyObject(this->_reply);
        return str;
    }

    void set(std::string key, std::string value)
    {
        redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
    }

private:

    redisContext* _connect;
    redisReply* _reply;

};
std::vector<string> FindDifferentSet(redisContext *context, const std::string &user1, const std::string &user2);
double cosine_similarity(redisContext *context, const std::string &user1, const std::string &user2);
vector<string> Recommend(redisContext* context, const std::string &user1);
#endif // MYREDIS_H
