/*
 * redis_conn_pool.h
 *
 *  Created on: Apr 9, 2013
 *      Author: yaowei
 */

#ifndef REDIS_CONN_POOL_H_
#define REDIS_CONN_POOL_H_

#include <deque>
#include <hiredis/hiredis.h>
#include "defines.h"

typedef struct redisConnInfo_
{
	int max_conn_num;
	std::string ip;
	int port;
} RedisConnInfo;

class CRedisConnPool
{
public:

	static CRedisConnPool* GetInstance();

	bool Init(const RedisConnInfo& redisConnInfo);
	void Destroy();

	redisContext* GetRedisContext();
	void ReleaseRedisContext(redisContext* conn);

private:
	CRedisConnPool();
	virtual ~CRedisConnPool();

	redisContext* CreateNewRedisContext();

	void SafeRedisFree(redisContext *conn)
	{
		if (conn)
		{
			redisFree(conn);
			conn = NULL;
		}
	}

	static CRedisConnPool* redisConnPool_;
	static boost::mutex instance_mutex_;
	std::deque<redisContext*> queue_redisContext_;
	RedisConnInfo redisConnInfo_;
};

#endif /* REDIS_CONN_POOL_H_ */
