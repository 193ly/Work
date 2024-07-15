#include "myredis.h"

double cosine_similarity(redisContext *context, const string &user1, const string &user2)
{
    std::unordered_map<std::string, double> m1, m2;
    redisReply* reply = (redisReply*)redisCommand(context, "ZRANGE %s 0 -1 WITHSCORES", user1.c_str());
    for(int j = 0; j < reply->elements; j += 2) {
        //qDebug()<<reply->element[j]->str;
        m1[reply->element[j]->str] = atof(reply->element[j+1]->str);
    }
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(context, "ZRANGE %s 0 -1 WITHSCORES", user2.c_str());
    for(int j = 0; j < reply->elements; j += 2) {
        //qDebug()<<reply->element[j]->str;
        m2[reply->element[j]->str] = atof(reply->element[j+1]->str);
    }
    freeReplyObject(reply);
    // 计算余弦相似度
    double dot_product = 0.0, norm1 = 0.0, norm2 = 0.0;
    /*if( m1.size() > m2.size() ) //以少的作为基准
         {
         for(const std::pair<std::string, double>& p : m2) {
         if(m1.count(p.first)) {
         dot_product += p.second * m1[p.first];
         norm2 += m1[p.first] * m1[p.first];
         }
         norm1 += p.second * p.second;
         }
         }else*/
    {
        //以m1 为基准
        for(const std::pair<std::string, double>& p : m1) {
            if(m2.count(p.first)) {
                dot_product += p.second * m2[p.first];
            }
        }
        for(auto& p : m1)
        {
            norm1 += p.second * p.second;
        }
        for(auto& p : m2)
        {
            norm2 += p.second * p.second;
        }
    }
    if( norm1 * norm2 == 0 ) return 0;
    return dot_product / (double)sqrt(norm1 * norm2);
}
std::vector<string> FindDifferentSet(redisContext *context, const std::string &user1, const std::string &user2)
{
    std::vector<std::string> recommendations;
    redisReply* reply = (redisReply*)redisCommand(context, "ZUNIONSTORE tmp 2 %s %s WEIGHTS -1 1 AGGREGATE MIN",
                                                  user1.c_str(), user2.c_str()); //以最小值作为结果分值合并 , 那么为负的就是U1的评分, 正的是相似用户的评分
    freeReplyObject(reply);
    // reply = (redisReply*)redisCommand(context, "ZDIFFSTORE tmp tmp %s", "userid:U1:items"); //需要redis6.2.0以上版本
    // freeReplyObject(reply);
    reply = (redisReply*)redisCommand(context, "zrevrangebyscore tmp +INF 0 limit 0 10 ");
    for (int i = 0; i < reply->elements; i += 2) {
        std::string item = reply->element[i]->str;
        recommendations.push_back( item );
    }
    freeReplyObject(reply);
    return recommendations;
}
vector<string> Recommend(redisContext* context, const std::string &user1)
{
    // 计算与用户U1相似度最高的用户
    std::vector<std::pair<std::string, double>> similarities;
    redisReply* reply = (redisReply*)redisCommand(context, "KEYS userid:*:items");
    for (int i = 0; i < reply->elements; i++) {
        std::string user = reply->element[i]->str;
        if (user != user1) {
            double similarity = cosine_similarity(context, user1, user);
            cout << similarity << endl;
            similarities.push_back(std::make_pair(user, similarity));
        }
    }
    if(similarities.size() == 0)
        return {};
    std::sort(similarities.begin(), similarities.end(), [](const std::pair<std::string, double>& a, const std::pair<std::string,
              double>& b) { return a.second > b.second; });
    std::string similar_user = similarities.front().first;
    freeReplyObject(reply);
    // 计算推荐商品
    vector<string> recommend = FindDifferentSet(context, user1, similar_user.c_str());

    //添加自身的标签
    reply = (redisReply*)redisCommand(context, "zrange %s 0 -1", user1.c_str());
    for(int i = 0; i < (int)reply->elements; i++)
    {
        recommend.push_back(reply->element[i]->str);
    }
    return recommend;
}
