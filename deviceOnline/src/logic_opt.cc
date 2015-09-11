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
#include "device_alive.h"

CLogicOpt::CLogicOpt(conn* c)
{
	conn_ = c;
	result_ = 0;
    is_online_ = false;
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
    ReplyMsg reply_msg;

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

    if (method == METHOD_DEVICE_LOGIN)
    {
        DeviceLogin();
        goto SEND_RESPONSE;
    }
    else if (method == METHOD_KEEP_ONLINE)
    {
        DeviceBeacon();
        goto SEND_RESPONSE;
    }
    else
    {
		LOG4CXX_ERROR(g_logger, "CLogicOpt::StartLogicOpt method invalid");
        return;
    }

SEND_RESPONSE:

    // 转义\r\n为\\r\\n
    string response_msg = utils::ReplaceString(responseToClient_, "\\r\\n", "\\\\r\\\\n");
    response_msg.append("\r\n");
    if (!SocketOperate::WriteSfd(conn_->sfd, response_msg.c_str(), response_msg.length()))
    {
        LOG4CXX_ERROR(g_logger, "SocketOperate::WriteSfd error, fd " << conn_->sfd << ", reply_msg " << responseToClient_);
        ret = -ERROR_PUSH_MESSAGE;
    }

    return;
}

int CLogicOpt::DeviceLogin()
{
    int ret = 0;
    deque<int> auth_data;

	LOG4CXX_TRACE(g_logger, "CLogicOpt::DeviceLogin enter");

    ret = jsonOpt_ptr_->JsonParseLogin(dev_id_, auth_data);
    if (ret != 0)
    {
	    LOG4CXX_ERROR(g_logger, "CLogicOpt::DeviceLogin parse login error, ret is " << ret);
        goto out;
    }

    if (!conn_)
    {
        LOG4CXX_ERROR(g_logger, "DeviceLogin conn_ is NULL");
        ret = -ERROR_DEVICE_CONNECT;
        goto out;
    }

    // TODO: 认证设备的合法性

    if (!conn_->is_login)
    {
        conn_->is_login = true;
        conn_->dev_id = dev_id_;

        // 更新设备在redis中的信息
        ret = CLogicOpt::SetDeviceFdCache(conn_->dev_id, conn_->sfd);
        if (ret != 0)
        {
	        LOG4CXX_ERROR(g_logger, "DeviceBeacon set fd cache failed");
            ret = -ERROR_SET_DEVICE_FD_CACHE;
            goto out;
        }

        struct sockaddr_in addr;
        socklen_t len = sizeof(struct sockaddr);
        ret = getsockname(conn_->sfd, (struct sockaddr *)&addr, &len);
        if (ret != 0)
        {
	        LOG4CXX_ERROR(g_logger, "DeviceBeacon getsockname error");
            goto out;
        }
        string addr_str(inet_ntoa(addr.sin_addr));
        ret = CLogicOpt::SetDeviceAddrCache(conn_->dev_id, addr_str);
        if (ret != 0)
        {
	        LOG4CXX_ERROR(g_logger, "DeviceBeacon getsockname error");
            goto out;
        }

        // 通知设备上线
        time_t now;
        time(&now);
        string time_info(ctime(&now));
        CJsonOpt json_opt;
        string msg_info = json_opt.JsonJoinDeviceStateNotice(conn_->dev_id, 1, time_info);
        SendMsg send_msg(msg_info);
        g_http_client->SubmitMsg(send_msg);

	    LOG4CXX_TRACE(g_logger, "DeviceBeacon ok, dev_id is " << conn_->dev_id << ", sfd is " << conn_->sfd);
    }

out:
    responseToClient_ = jsonOpt_ptr_->JsonJoinLoginRes(ret);
    return ret;
}

int CLogicOpt::DeviceBeacon()
{
    int ret = 0;
    string dev_id;

	LOG4CXX_TRACE(g_logger, "CLogicOpt::DeviceBeacon enter");

    ret = jsonOpt_ptr_->JsonParseBeacon(dev_id);
    if (ret != 0)
    {
	    LOG4CXX_ERROR(g_logger, "CLogicOpt::StartLogicOpt:JsonParseBecon failed");
        goto out;
    }

    if (!conn_)
    {
	    LOG4CXX_ERROR(g_logger, "DeviceBeacon device conn_ is null");
        ret = -ERROR_DEVICE_CONNECT;
        goto out;
    }

    if (!conn_->is_login)
    {
	    LOG4CXX_ERROR(g_logger, "DeviceBeacon device conn_ is null");
        ret = -ERROR_DEVICE_NOT_LOGIN;
        goto out;
    }

    if (!conn_->is_online)
    {
        conn_->is_online = true;
    }

out:
    responseToClient_ = jsonOpt_ptr_->JsonJoinBeaconRes(ret);
    return ret;
}

int CLogicOpt::SetDeviceFdCache(string dev_id, int fd)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    if(!redis_opt.Hset(dev_id, REDIS_FIELD_FD, fd))
    {
        ret = -ERROR_SET_DEVICE_FD_CACHE;
        goto out;
    }

out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::GetDeviceFdFromCache(string dev_id, int &fd)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    string fd_str;
    if(!redis_opt.Hget(dev_id, REDIS_FIELD_FD, fd_str))
    {
        ret = -ERROR_GET_DEVICE_FD_FROM_CACHE;
        goto out;
    }

    fd = atoi(fd_str.c_str());
out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::RemoveDeviceFdFromCache(string dev_id)
{
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    redis_opt.Hdel(dev_id, REDIS_FIELD_FD);

	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return 0;
}

int CLogicOpt::SetDeviceAddrCache(string dev_id, string addr)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    if(!redis_opt.Hset(dev_id, REDIS_FIELD_ADDR, addr))
    {
        ret = -ERROR_SET_DEVICE_ADDR_CACHE;
        goto out;
    }

out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::GetDeviceAddrFromCache(string dev_id, string &addr)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    if(!redis_opt.Hget(dev_id, REDIS_FIELD_ADDR, addr))
    {
        ret = -ERROR_GET_DEVICE_ADDR_FROM_CACHE;
        goto out;
    }

out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::RemoveDeviceAddrFromCache(string dev_id)
{
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    redis_opt.Hdel(dev_id, REDIS_FIELD_ADDR);

	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return 0;
}

