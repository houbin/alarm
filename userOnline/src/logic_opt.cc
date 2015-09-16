/*
 * message_opt.cc
 *
 *  Created on: Mar 18, 2013
 *      Author: yaowei
 */

#include "logic_opt.h"
#include "defines.h"
#include "json_opt.h"
#include "redis_conn_pool.h"
#include "global_settings.h"
#include "../../public/utils.h"
#include "../../public/redis_key.h"
#include "../../public/socket_wrapper.h"
#include "../../public/message.h"
#include "../../public/error_code.h"
#include "push_msg_queue.h"
#include "user_alive.h"

CLogicOpt::CLogicOpt(conn* c)
{
	conn_ = c;
	result_ = 0;
	jsonOpt_ptr_ = new CJsonOpt;
	assert(jsonOpt_ptr_ != NULL);
}

CLogicOpt::~CLogicOpt()
{
	utils::SafeDelete(jsonOpt_ptr_);
}

void CLogicOpt::StartLogicOpt(const std::string& message)
{
    int ret = 0;
	std::string custom_msg;
    string method;

	LOG4CXX_INFO(g_logger, "CLogicOperate::StartLogicOpt" << message);

	jsonOpt_ptr_->setJsonString(message);
    if (!jsonOpt_ptr_->JsonParseCommon())
    {
		LOG4CXX_ERROR(g_logger, "CLogicOperate::StartLogicOpt:JsonParseCommon failed:" << message);
        return;
    }

    ret = jsonOpt_ptr_->GetMethod(method);
    if (ret != 0)
    {
		LOG4CXX_ERROR(g_logger, "CLogicOperate::StartLogicOpt:GetMethod failed:" << message);
        return;
    }

    if (method == METHOD_KEEP_ONLINE)
    {
        UserBeacon();
        goto SEND_RESPONSE;
    }
    else
    {
        HandlePushMsgResp();
        return;
    }

SEND_RESPONSE:
    // 转义\r\n为\\r\\n
    string response_msg = utils::ReplaceString(responseToClient_, "\\r\\n", "\\\\r\\\\n");
    response_msg.append("\r\n");
    if (!SocketOperate::WriteSfd(conn_->sfd, response_msg.c_str(), response_msg.size()))
    {
        LOG4CXX_ERROR(g_logger, "send push msg reponse error, sfd " << conn_->sfd);
    }

    return;
}

int CLogicOpt::UserBeacon()
{
    int ret = 0;
    string session_id;

	LOG4CXX_TRACE(g_logger, "CLogicOpt::UserBeacon enter");

    ret = jsonOpt_ptr_->JsonParseBeacon(session_id);
    if (ret != 0)
    {
        ret = -ERROR_PARSE_BEACON;
	    LOG4CXX_ERROR(g_logger, "CLogicOpt::StartLogicOpt:JsonParseBecon failed");
        goto out;
    }

    if (!conn_)
    {
        ret = -ERROR_USER_CONNECT;
	    LOG4CXX_ERROR(g_logger, "UserBeacon connect error");
        goto out;
    }

    conn_->guid = session_id;

    // check session id
    ret = CheckSessionId(session_id);
    if (ret != 0)
    {
        conn_->is_online = false;
        CLogicOpt::RemoveGuidFdFromCache(conn_->guid);
	    LOG4CXX_ERROR(g_logger, "UserBeacon check session id error");
        goto out;
    }

    if (!conn_->is_online)
    {
        // TODO 用户上线通知
        conn_->is_online = true;
        ret = CLogicOpt::SetGuidFdCache(conn_->guid, conn_->sfd);
        if (ret != 0)
        {
            ret = -ERROR_SET_GUID_FD_CACHE;
	        LOG4CXX_ERROR(g_logger, "UserBeacon set fd cache failed");
            goto out;
        }
	    LOG4CXX_TRACE(g_logger, "UserBeacon first beacon, session is " << session_id);
    }
    else if (conn_->is_online && conn_->guid != session_id)
    {
        // 用户session id变了
        CLogicOpt::RemoveGuidFdFromCache(conn_->guid);
        ret = CLogicOpt::SetGuidFdCache(conn_->guid, conn_->sfd);
        if (ret != 0)
        {
            ret = -ERROR_SET_GUID_FD_CACHE;
	        LOG4CXX_ERROR(g_logger, "UserBeacon set fd cache failed");
            goto out;
        }
	    LOG4CXX_TRACE(g_logger, "UserBeacon session changed, before session id is " << conn_->guid << ", now session id is " << session_id);
    }

out:
    responseToClient_ = jsonOpt_ptr_->JsonJoinBeaconRes(ret);
    return ret;
}

int CLogicOpt::HandlePushMsgResp()
{
    int ret = 0;
    int msg_ret;
    int push_mid = 0;

	LOG4CXX_TRACE(g_logger, "CLogicOpt::HandlePushMsgResp enter");

    push_mid = jsonOpt_ptr_->GetMid();

    ret = jsonOpt_ptr_->JsonParsePushMsgResp(msg_ret);
    if (ret != 0)
    {
        msg_ret = -ERROR_PARSE_MSG;
	    LOG4CXX_ERROR(g_logger, "CLogicOpt::StartLogicOpt:HandlePushMsgResp failed");
    }

    g_wait_finish_push_msg_queue.FinishPushMsg(push_mid, msg_ret);

    return 0;
}

int CLogicOpt::SetGuidFdCache(string guid, int fd)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_CLIENT_INFO);

    if(!redis_opt.Set(guid, fd))
    {
        LOG4CXX_ERROR(g_logger, "redis set session fd error, session id is " << guid);
        ret = -ERROR_SET_GUID_FD_CACHE;
        goto out;
    }

out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::GetGuidFdFromCache(string guid, int &fd)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_CLIENT_INFO);

    string fd_str;
    if(!redis_opt.Get(guid, fd_str))
    {
        LOG4CXX_ERROR(g_logger, "redis get session fd error, session id is " << guid);
        ret = -ERROR_GET_USER_FD_FROM_CACHE;
        goto out;
    }

    fd = atoi(fd_str.c_str());
out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::RemoveGuidFdFromCache(string guid)
{
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_CLIENT_INFO);

    redis_opt.Del(guid);

	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return 0;
}

int CLogicOpt::CheckSessionId(string guid)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(SESSION);

    string key_str = "sess:" + guid;

    if (!redis_opt.Hexists(key_str, "username"))
    {
        LOG4CXX_ERROR(g_logger, "redis check session exist error, session id is " << guid);
        ret = -ERROR_SESSION_ID_NOT_EXIST;
        goto out;
    }

out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

