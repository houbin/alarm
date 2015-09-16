/*
 * message_opt.cc
 *
 *  Created on: Mar 18, 2013
 *      Author: yaowei
 */

#include <sys/socket.h>
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
    else // (method == METHOD_SET_STREAMSERVER_ADDR)
    {
        //其他的method认为是推送消息的回复消息
		LOG4CXX_ERROR(g_logger, "CLogicOpt::StartLogicOpt method is " << method);
        HandlePushMsgResp();
        return;
    }

SEND_RESPONSE:
    string response_msg = utils::ReplaceString(responseToClient_, "\\r\\n", "\\\\r\\\\n");
    response_msg.append("\r\n");
    if (!SocketOperate::WriteSfd(conn_->sfd, response_msg.c_str(), response_msg.length()))
    {
        LOG4CXX_ERROR(g_logger, "SocketOperate::WriteSfd error, fd " << conn_->sfd << ", push_msg " << responseToClient_);
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

        // 获取上线设备的ip
        struct sockaddr_in dev_addr;
        socklen_t len;
        memset(&dev_addr, 0, sizeof(struct sockaddr));
        ret = getpeername(conn_->sfd, (struct sockaddr*)&dev_addr, &len);
        if (ret != 0)
        {
	        LOG4CXX_ERROR(g_logger, "DeviceBeacon getpeername failed, errno is " << -errno);
            ret = -errno;
            goto out;
        }
        string dev_ip(inet_ntoa(dev_addr.sin_addr));

        // 更新设备在redis中的信息
        ret = CLogicOpt::SetDeviceFdCache(conn_->dev_id, conn_->sfd);
        if (ret != 0)
        {
	        LOG4CXX_ERROR(g_logger, "DeviceBeacon set fd cache failed");
            ret = -ERROR_SET_DEVICE_FD_CACHE;
            goto out;
        }

        string dev_public_addr = utils::G<CGlobalSettings>().public_addr_ + ":15030";
        string dev_private_addr = utils::G<CGlobalSettings>().private_addr_ + ":15031";
        ret = CLogicOpt::SetDeviceAddrCache(conn_->dev_id, dev_public_addr, dev_private_addr, dev_ip);
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
        string url = utils::G<CGlobalSettings>().httpserver_url_;
        SendMsg send_msg(url, msg_info);
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

int CLogicOpt::HandlePushMsgResp()
{
    int ret = 0;
    int msg_ret;
    int push_cnt = 0;
    string method;

    push_cnt = jsonOpt_ptr_->GetSendCnt();
    jsonOpt_ptr_->GetMethod(method);
    LOG4CXX_TRACE(g_logger, "handle push msg resp, method is " << method << ", push_cnt is " << push_cnt);

    JSONNode param_node;
    ret = jsonOpt_ptr_->JsonParsePushMsgResp(msg_ret, param_node);
    if (ret != 0)
    {
        msg_ret = -ERROR_PARSE_MSG;
	    LOG4CXX_ERROR(g_logger, "CLogicOpt::StartLogicOpt:HandlePushMsgResp failed");
    }

    g_wait_finish_push_msg_queue.FinishPushMsg(push_cnt, msg_ret, param_node);

    return 0;
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

int CLogicOpt::SetDeviceAddrCache(string dev_id, string public_addr, string private_addr, string dev_ip)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    if(!redis_opt.Hset(dev_id, REDIS_FIELD_PUBLIC_ADDR, public_addr))
    {
        ret = -ERROR_SET_DEVICE_ADDR_CACHE;
        goto out;
    }
    
    if (!redis_opt.Hset(dev_id, REDIS_FIELD_PRIVATE_ADDR, private_addr))
    {
        redis_opt.Hdel(dev_id, REDIS_FIELD_PUBLIC_ADDR);
        ret = -ERROR_SET_DEVICE_ADDR_CACHE;
        goto out;
    }

    if (!redis_opt.Hset(dev_id, REDIS_FIELD_DEV_IP, dev_ip))
    {
        redis_opt.Hdel(dev_id, REDIS_FIELD_PUBLIC_ADDR);
        redis_opt.Hdel(dev_id, REDIS_FIELD_PRIVATE_ADDR);
        ret = -ERROR_SET_DEVICE_ADDR_CACHE;
        goto out;
    }

out:
	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return ret;
}

int CLogicOpt::GetDeviceAddrFromCache(string dev_id, string &public_addr, string &private_addr, string &dev_ip)
{
    int ret = 0;
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    if(!redis_opt.Hget(dev_id, REDIS_FIELD_PUBLIC_ADDR, public_addr))
    {
        ret = -ERROR_GET_DEVICE_ADDR_FROM_CACHE;
        goto out;
    }

    if(!redis_opt.Hget(dev_id, REDIS_FIELD_PRIVATE_ADDR, private_addr))
    {
        ret = -ERROR_GET_DEVICE_ADDR_FROM_CACHE;
        goto out;
    }

    if(!redis_opt.Hget(dev_id, REDIS_FIELD_DEV_IP, dev_ip))
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

    redis_opt.Hdel(dev_id, REDIS_FIELD_PUBLIC_ADDR);
    redis_opt.Hdel(dev_id, REDIS_FIELD_PRIVATE_ADDR);
    redis_opt.Hdel(dev_id, REDIS_FIELD_DEV_IP);

	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return 0;
}

int CLogicOpt::RemoveDeviceFromCache(string dev_id)
{
    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(REDIS_DEVICE_INFO);

    redis_opt.Del(dev_id);

	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return 0;
}

