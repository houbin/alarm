/*
 * CRedisOpt.h
 *
 *  Created on: 2012-11-5
 *      Author: yaowei
 */

#ifndef CREDISOPT_H_
#define CREDISOPT_H_

#include <set>
#include <hiredis/hiredis.h>
#include "defines.h"

class CRedisOpt {
public:
    CRedisOpt();
    virtual ~CRedisOpt();

    void SetRedisContext(redisContext* conn) { conn_ = conn;}

    bool SelectDB(int db_num);

    /* string 类型操作 */
    bool Set(const std::string& key, const std::string& value);
    bool Set(const std::string& key, const int value);
    bool Set(const std::string& key, const long value);
    bool Get(const std::string& key, std::string& value);
    bool MGet(const std::vector<std::string>& vec_key, const std::string& extend_keyname, std::vector<std::string>& vec_value);
    bool Incr(const std::string& key);

    /* list 类型操作 */
    bool LPush(const std::string& key, const std::vector<std::string>& vec_value);
    bool LPush(const std::string& key, const std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool LPop(const std::string& key);
    bool LLen(const std::string& key, int& len);
    bool LRange(const std::string& key, std::vector<std::string>& vec_value);

    /* set 类型操作 */
    bool SAdd(const std::string& key, const std::vector<std::string>& vec_value);
    bool SAdd(const std::string& key, const std::string& value);
    bool SAdd(const std::string& key, const std::set<std::string>& set_value);
    bool SMembers(const std::string& key, std::vector<std::string>& vec_value);

    /* map 类型操作 */
    bool Hset(const std::string& key, const std::string& field, const std::string& value);
    bool Hset(const std::string& key, const std::string& field, const int value);
    bool Hget(const std::string& key, const std::string& field, std::string& value);
    bool Hdel(const std::string& key, const std::string& field);
    bool Hexists(const std::string &key, const std::string &field);
    bool Hvals(const std::string& key, std::vector<std::string>& vec_value);
    bool HLen(const std::string& key, int& len);

    bool Del(const std::string& key);

    bool Expire(const std::string& key, const int sec);

    bool Exists(const std::string& key);

    // need version of redis not lower than 2.8.
    // The newest stable version the better.
    bool Scan(long long cursor, std::vector<std::string> &vec_keys, long long &next_cursor);
    
private:

    void SafeFreeReplyObject(redisReply *reply)
    {
        if(reply)
        {
            freeReplyObject(reply);
            reply = NULL;
        }
    }

    void SafeFreeRedisContext(redisContext     *conn)
    {
        if(conn)
        {
            redisFree(conn);
            conn = NULL;
        }
    }

    redisContext         *conn_;
    redisReply          *reply_;
};

#endif /* CCACHED_H_ */
