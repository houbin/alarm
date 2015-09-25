#include "json_opt.h"
#include "dispatcher.h"
#include "../../public/error_code.h"
#include "../../public/message.h"
#include "global.h"
#include "tlv_define.h"
#include "../include/socket_wrapper.h"

Dispatcher::Dispatcher()
{

}

Dispatcher::~Dispatcher()
{

}

int32_t Dispatcher::HandleConnect(ConnectionInfo *conn_info)
{
    LOG_DEBUG(g_logger, "connect, client ip:port is %s:%u", conn_info->cip, conn_info->cport);

    return 0;
}

int32_t Dispatcher::HandleTimeout(ConnectionInfo *conn_info)
{
    LOG_DEBUG(g_logger, "connect timeout, client ip:port is %s:%u", conn_info->cip, conn_info->cport);
    HandleClose(conn_info);

    return 0;
}

int32_t Dispatcher::HandleClose(ConnectionInfo *conn_info)
{
    LOG_DEBUG(g_logger, "close connect, client ip:port is %s:%u", conn_info->cip, conn_info->cport);

    return 0;
}

int32_t Dispatcher::HandleError(ConnectionInfo *conn_info)
{
    LOG_DEBUG(g_logger, "connect error, client ip:port is %s:%u", conn_info->cip, conn_info->cport);
    HandleClose(conn_info);

    return 0;
}


/*
 * json dispatcher
 *
 * */
JsonDispatcher::JsonDispatcher()
{

}

JsonDispatcher::~JsonDispatcher()
{

}

int32_t JsonDispatcher::HandleMsg(ConnectionInfo *conn_info, string &msg_info)
{
    LOG_DEBUG(g_logger, "handle json msg, client ip:port is %s:%u, msg is %s", conn_info->cip, conn_info->cport, msg_info);

    int32_t ret;
    CJsonOpt json_opt;
    json_opt.setJsonString(msg_info);

    if (!json_opt.JsonParseCommon())
    {
        LOG_ERROR(g_logger, "json parse common error");
        return -ERROR_PARSE_MSG;
    }

    string method = json_opt.GetMethod();
    if (method == METHOD_GET_VOICESERVER_ADDR)
    {
        ret = HandleGetVoiceServerAddr(conn_info, json_opt);
        if (ret != 0)
        {
            LOG_ERROR(g_logger, "error is %s", ret);
            return ret;
        }
    }
    else
    {
        LOG_ERROR(g_logger, "invalid json method, method is %s", method);
    }

    LOG_DEBUG(g_logger, "handle json msg end, client ip:port is %s:%u, msg is %s", conn_info->cip, conn_info->cport, msg_info);

    return 0;
}

int32_t JsonDispatcher::HandleGetVoiceServerAddr(ConnectionInfo *conn_info, CJsonOpt &json_opt)
{
    int ret = 0;
    int server_port = 0;
    string client_ip;
    string dev_ip;
    string server_ip;

    ret = json_opt.JsonParseGetVoiceServerAddr(client_ip, dev_ip);
    if (ret != 0)
    {
        goto out;
    }

    // get voice server addr
    server_ip = g_config->Read("public_addr", "123.56.101.53");
    server_port = g_config->Read("tlv_listen_port", 15040);

out:
    string resp_str = json_opt.JsonJoinGetVoiceServerAddrResp(ret, server_ip, server_port);
    string response_msg = utils::ReplaceString(resp_str, "\\r\\n", "\\\\r\\\\n");
    response_msg.append("\r\n");
    if (!SocketOperate::WriteSfd(conn_info->cfd, response_msg.c_str(), response_msg.length()))
    {
        LOG_ERROR(g_logger, "write reponse msg error, fd is %d, reponse msg is %s", conn_info->cfd, response_ms);
        ret = -ERROR_PUSH_MESSAGE;
    }

    return ret;
}

/*
 * tlv dispatcher
 *
 *
 * */
TlvDispatcher::TlvDispatcher()
{

}

TlvDispatcher::~TlvDispatcher()
{

}

int32_t TlvDispatcher::HandleMsg(ConnectionInfo *conn_info, Slice &s)
{
    LOG_DEBUG(g_logger, "handle tlv msg, client ip:port is %s:%u", conn_info->cip, conn_info->cport);

    int32_t ret;
    char *msg_data = s.data();
    size_t msg_size = s.size();

    char *parsing_ptr = msg_data;

    // 解析mid
    uint32_t mid_type = DecodeFixed32(parsing_ptr);
    uint32_t mid_len = DecodeFixed32(parsing_ptr + 4);
    uint32_t mid = DecodeFixed32(parsing_ptr + 8);
    if (mid_type != TYPE_MID || mid_len != 4)
    {
        LOG_ERROR(g_logger, "parse mid error");
        return -ERROR_TLV_PARSE_MID;
    }
    parsing_ptr = parsing_ptr + kMsgHeaderSize + mid_len;

    uint32_t cmd = DecodeFixed32(parsing_ptr);
    uint32_t len = DecodeFixed32(parsing_ptr + 4);

    if (msg_len != kMsgHeaderSize + mid_len + kMsgHeaderSize + len)
    {
        LOG_ERROR(g_logger, "parse cmd length error");
        return -ERROR_TLV_PARSE_CMD;
    }

    Slice cmd_value(parsing_ptr + kMsgHeaderSize, len);
    switch(cmd)
    {
        case TYPE_CLIENT_BUILD_VC:
            ret = HandleClientBuildVc(conn_info, mid, cmd_value);
            break;

        case TYPE_CLIENT_START_SEND:
            Handle
        case TYPE_CLIENT_SEND_VOICE_DATA:
        case TYPE_CLIENT_FREE_VC:
        case TYPE_DEV_BUILD_VC:
        case TYPE_DEV_FREE_VC:
        default:
            break;
    }

    if (ret == 0)
    {
        LOG_DEBUG(g_logger, "tlv HandleMsg ok");
    }
    else
    {
        LOG_DEBUG(g_logger, "tlv handle msg error, ret %d", ret);
    }

    return ret;
}

void TlvDispatcher::EncodeCommonTlv(string *resp_msg, uint32_t msg_len, uint32_t mid)
{
    PutFixed32(resp_msg, TYPE_LENGTH);
    PutFixed32(resp_msg, 4);
    PutFixed32(resp_msg, msg_len);

    PutFixed32(resp_msg, TYPE_MID);
    PutFixed32(resp_msg, 4);
    PutFixed32(resp_msg, mid);
}

void TlvDispatcher::EncodeRet(string *resp_msg, uint32_t on_cmd, int32_t ret)
{
    PutFixed32(resp_msg, on_cmd);
    PutFixed32(resp_msg, 4);
    PutFixed32(resp_msg, ret);
}

int32_t TlvDispatcher::SendMsg(int fd, string msg)
{
    if (!SocketOperate::WriteSfd(fd, msg.c_str(), msg.size()))
    {
        return -ERROR_TLV_SEND_MSG;
    } 

    return 0;
}

int32_t TlvDispatcher::ForwardMsgToDev(int push_fd, uint32_t push_mid, string &push_msg, PushMsgContext *ct)
{
    int32_t ret = 0;

    LOG_DEBUG(g_logger, "forward msg to dev, mid is %u", push_mid);

    ret = SendMsg(push_fd, push_msg)
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "send msg error, mid %u", push_mid);
        return ret;
    }

    double timeout = g_config->Read("tlv_push_msg_timeout", 5.0);
    push_msg_timer_queue_.AddEventAfter(timeout, push_mid, ct);

    LOG_DEBUG(g_logger, "forward msg to dev ok, mid is %u", push_mid);

    return 0;
}

void TlvDispatcher::PackClientBuildVcRespMsg(uint32_t mid, int32_t ret, string &resp_msg)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize + 4;

    EncodeCommonTlv(&resp_msg, msg_len, mid);
    EncodeRet(&resp_msg, TYPE_ON_CLIENT_BUILD_VC, ret);
}

int32_t TlvDispatcher::HandleClientBuildVc(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    int32_t ret;
    uint32_t type;
    uint32_t len;
    string client_id;
    string dev_id;

    LOG_DEBUG(g_logger, "client build voice channel, and mid is %u", mid);
    
    int size = cmd_value.size();
    char *data = cmd_value.data();

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of client_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_CLIENT_ID;
        goto SEND_RESPONSE;
    }
    client_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of dev_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_DEV_ID;
        goto SEND_RESPONSE;
    }
    dev_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    conn_info->guid_type = GUID_CLIENT;
    conn_info->guid = client_id;

    LOG_DEBUG(g_logger, "mid is %u, client_id is %s, dev_id is %s", mid, client_id, dev_id);

    ClientVcInfo client_vc_info;
    client_vc_info.client_id = client_id;
    client_vc_info.client_fd = conn_info->cfd;
    client_vc_info.client_bev = conn_info->buffer_event;
    client_vc_info.dev_id = dev_id;
    ret = vc_manager_.ConnectByClient(client_id, client_vc_info);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "connect by client error, ret is %d", ret);
        goto SEND_RESPONSE;
    }

    LOG_DEBUG(g_logger, "client build voice channel ok, and mid is %u, client id is %s, dev id is %s", 
                        mid, client_id, dev_id);

SEND_RESPONSE:
    string resp_msg;
    PackClientBuildVcRespMsg(mid, ret, &resp_msg);
    ret = SendMsg(conn_info->cfd, resp_msg);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "send response msg of client build voice channle error, ret is %d, mid is %u", ret, mid);
        return -ERROR_TLV_SEND_MSG;
    } 

    return 0;
}

void TlvDispatcher::PackStartSendPushMsg(string &push_msg, uint32_t &push_mid)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize;

    push_mid = g_push_msg_mid.incrementAndGet();
    EncodeCommonTlv(&push_msg, msg_len, push_mid);

    PutFixed32(&push_msg, TYPE_PUSH_CLIENT_START_SEND);
    PutFixed32(&push_msg, 0);

   return;
}

int32_t TlvDispatcher::HandleClientStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    int32_t ret = 0;
    int dev_fd = -1;
    string dev_id;
    uint32_t push_mid = 0;
    string push_msg;
    PushMsgContext *ct = NULL;

    LOG_DEBUG(g_logger, "client start to send, and mid is %u", mid);

    char *data = cmd_value.data();
    int size = cmd_value.size();

    if (size != 0)
    {
        LOG_ERROR(g_logger, "cmd value of client start send error");
        ret = -ERROR_TLV_LEN_NOT_0;
        goto SEND_RESPONSE;
    }

    // 转发到client
    if (conn_info->guid_type != GUID_CLIENT || conn_info->guid == "")
    {
        LOG_ERROR(g_logger, "client not build voice channel, mid %u, guid type %d, guid %s", mid, 
                        conn_info->guid_type, conn_info->guid);
        ret = -ERROR_CLIENT_NOT_BUILD_VC;
        goto SEND_RESPONSE;
    }

    ret = vc_manager_.RouteClientToDev(conn_info->guid, dev_id, dev_fd);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "can't find dev for client, mid %u, guid type %d, guid %s", mid, 
                        conn_info->guid_type, conn_info->guid);
        ret = -ERROR_CANNOT_FIND_DEV_FOR_CLIENT;
        goto SEND_RESPONSE;
    }

    PackStartSendPushMsg(push_msg, push_mid);

    ct = new PushMsgContext(mid, conn_info->cfd, dev_fd, push_mid, TYPE_ON_CLIENT_START_SEND);
    ret = ForwardMsgToDev(dev_fd, push_mid, push_msg, ct);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "forward msg to dev error, client_id %s, dev_id %s, ret %d", conn_info->guid, 
                        dev_id, ret);
        goto SEND_RESPONSE;
    }

    LOG_DEBUG(g_logger, "forward client start send msg to dev ok, mid is %u, client_id %s, dev_id %s",
                        mid, conn_info->guid, dev_id);
    return 0;

SEND_RESPONSE:
    string resp_msg;
    PackStartSendRespMsg(mid, resp_msg, ret);
    
    // 发送回复消息，失败也不需要关闭connection
    SendMsg(resp_msg);
    return 0;
}

void TlvDispatcher::PackStartSendRespMsg(uint32_t mid, string &resp_msg, int32_t ret)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize;

    EncodeCommonTlv(&push_msg, msg_len, mid);

    PutFixed32(&push_msg, TYPE_ON_CLIENT_START_SEND);
    PutFixed32(&push_msg, ret);

    return;
}

int32_t TlvDispatcher::HandleOnPushStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    int32_t ret;
    LOG_DEBUG(g_logger, "handle on push start send, and mid is %u", mid);
    
    int size = cmd_value.size();
    char *data = cmd_value.data();

    int32_t on_push_ret = DecodeFixed32(data);

    // 取消等待推送消息的回复消息的超时事件
    PushMsgInfo push_msg_info;
    ret = push_msg_timer_queue_.GetAndCancelEvent(mid, push_msg_info);
    if (ret != 0)
    {
        LOG_INFO(g_logger, "get and cancel event error, ret %d, mid %u", ret, mid);
        return 0;
    }

    string resp_msg;
    PackStartSendRespMsg(push_msg_info.recv_mid, resp_msg, on_push_ret);
    ret = SendMsg(push_msg_info.recv_cfd, resp_msg);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "send client start send response msg error, mid %u, ret %d", 
                            push_msg_info.recv_mid, ret);
        // 设备端回复消息后，需要回复client消息，如果发送失败后返回错误的话，会关闭设备的connection
    }

    return 0;
}

void TlvDispatcher::PackSendVoicePushMsg(string &push_msg, uint32_t &push_mid, Slice &data)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize + data.size();

    push_mid = g_push_msg_mid.incrementAndGet();
    EncodeCommonTlv(&push_msg, msg_len, push_mid);

    PutFixed32(&push_msg, TYPE_PUSH_CLIENT_VOICE_DATA);
    PutFixed32(&push_msg, data.size());
    push_msg.append(data.data(), data.size());
}

int32_t TlvDispatcher::HandleClientSendVoice(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    int32_t ret = 0;
    int dev_fd;
    string dev_id;
    PushMsgContext *ct = NULL;
    uint32_t push_mid;
    string push_msg;

    LOG_DEBUG(g_logger, "client send voice data, and mid is %u", mid);

    char *data = cmd_value.data();
    int size = cmd_value.size();

    // 转发到client
    if (conn_info->guid_type != GUID_CLIENT || conn_info->guid == "")
    {
        LOG_ERROR(g_logger, "client not build voice channel, mid %u, guid type %d, guid %s", mid, 
                        conn_info->guid_type, conn_info->guid);
        ret = -ERROR_CLIENT_NOT_BUILD_VC;
        goto SEND_RESPONSE;
    }

    ret = vc_manager_.RouteClientToDev(conn_info->guid, dev_id, dev_fd);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "can't find dev for client, mid %u, guid type %d, guid %s", mid, 
                        conn_info->guid_type, conn_info->guid);
        ret = -ERROR_CANNOT_FIND_DEV_FOR_CLIENT;
        goto SEND_RESPONSE;
    }

    PackSendVoicePushMsg(push_msg, push_mid, cmd_value);
    ct = new PushMsgContext(mid, conn_info->cfd, dev_fd, push_mid, TYPE_ON_CLIENT_SEND_VOICE_DATA);
    assert(ct != NULL);
    ret = ForwardMsgToDev(dev_fd, push_mid, push_msg, ct);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "forward msg to dev error, client_id %s, dev_id %s, ret %d", conn_info->guid, 
                        dev_id, ret);
        goto SEND_RESPONSE;
    }

    LOG_DEBUG(g_logger, "forward client send voice msg to dev ok, mid is %u, client_id %s, dev_id %s",
                        mid, conn_info->guid, dev_id);
    return 0;

SEND_RESPONSE:
    LOG_DEBUG(g_logger, "forward client send voice msg to dev error, mid is %u, client_id %s",
                        mid, conn_info->guid);
    string resp_msg;
    PackClientSendVoiceRespMsg(mid, resp_msg, ret);
    SendMsg(conn_info->cfd, resp_msg);

    // 发送失败，也不返回错误，因为返回错误会关闭连接
    return 0;
}

void TlvDispatcher::PackClientSendVoiceRespMsg(uint32_t mid, string &resp_msg, int32_t ret)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize;

    EncodeCommonTlv(&push_msg, msg_len, mid);

    PutFixed32(&push_msg, TYPE_ON_CLIENT_SEND_VOICE_DATA);
    PutFixed32(&push_msg, ret);

    return;
}

int32_t TlvDispatcher::HandleOnPushClientVoice(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    int32_t ret;
    LOG_DEBUG(g_logger, "handle on push send voice msg, and mid is %u", mid);
    
    int size = cmd_value.size();
    char *data = cmd_value.data();

    int32_t on_push_ret = DecodeFixed32(data);

    // 取消等待推送消息的回复消息的超时事件
    PushMsgInfo push_msg_info;
    ret = push_msg_timer_queue_.GetAndCancelEvent(mid, push_msg_info);
    if (ret != 0)
    {
        LOG_INFO(g_logger, "get and cancel event error, ret %d, mid %u", ret, mid);
        return 0;
    }

    string resp_msg;
    PackClientSendVoiceRespMsg(push_msg_info.recv_mid, resp_msg, on_push_ret);
    ret = SendMsg(push_msg_info.recv_cfd, resp_msg);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "send client start send response msg error, mid %u, ret %d", 
                            push_msg_info.recv_mid, ret);
        // 这里如果发送失败，不要返回错误，因为这里是设备端的connection，返回错误会把设备端的conn关闭
    }

    return 0;
}

void TlvDispatcher::PackClientFreeVcRespMsg(uint32_t mid, string &resp_msg, int32_t ret)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize + 4;

    EncodeCommonTlv(&resp_msg, msg_len, mid);
    EncodeRet(&resp_msg, TYPE_ON_CLIENT_FREE_VC, ret);
}

int32_t TlvDispatcher::HandleClientFreeVc(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    uint32_t type;
    uint32_t len;
    string client_id;
    string dev_id;

    LOG_DEBUG(g_logger, "client free voice channle, mid is %u", mid);

    char *data = cmd_value.data();
    size_t size = cmd_value.size();

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of client_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_CLIENT_ID;
        goto SEND_RESPONSE;
    }
    client_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of dev_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_DEV_ID;
        goto SEND_RESPONSE;
    }
    dev_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    ret = vc_manager_.DisconnectByClient(client_id);

SEND_RESPONSE:
    string resp_msg;
    PackClientFreeVcRespMsg(mid, resp_msg, ret);

    return ret;
}

void TlvDispatcher::PackDevBuildVcRespMsg(uint32_t mid, string &resp_msg, int32_t ret)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize + 4;

    EncodeCommonTlv(&resp_msg, msg_len, mid);
    EncodeRet(&resp_msg, TYPE_ON_DEV_BUILD_VC, ret);
}

int32_t TlvDispatcher::HandleDevBuildVc(ConnectionInfo *conn_info, uint32_t mid, Slice &s)
{
    int32_t ret;
    uint32_t type;
    uint32_t len;
    string client_id;
    string dev_id;

    LOG_DEBUG(g_logger, "device build voice channel, and mid is %u", mid);
    
    int size = cmd_value.size();
    char *data = cmd_value.data();

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of client_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_CLIENT_ID;
        goto SEND_RESPONSE;
    }
    client_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of dev_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_DEV_ID;
        goto SEND_RESPONSE;
    }
    dev_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    conn_info->guid_type = GUID_DEV;
    conn_info->guid = dev_id;

    LOG_DEBUG(g_logger, "mid is %u, client_id is %s, dev_id is %s", mid, client_id, dev_id);

    DevVcInfo dev_vc_info;
    dev_vc_info.dev_id = dev_id;
    dev_vc_info.dev_fd = conn_info->cfd;
    dev_vc_info.dev_bev = conn_info->buffer_event;
    dev_vc_info.client_id = client_id;
    ret = vc_manager_.ConnectByDev(dev_id, dev_vc_info);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "connect by dev error, ret is %d", ret);
        goto SEND_RESPONSE;
    }

    LOG_DEBUG(g_logger, "client build voice channel ok, and mid is %u, client id is %s, dev id is %s", 
                        mid, client_id, dev_id);

SEND_RESPONSE:
    string resp_msg;
    PackClientBuildVcRespMsg(mid, ret, &resp_msg);
    ret = SendMsg(conn_info->cfd, resp_msg);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "send response msg of dev build voice channle error, ret is %d, mid is %u", ret, mid);
        return -ERROR_TLV_SEND_MSG;
    } 

    return 0;
}

void TlvDispatcher::PackDevFreeVcRespMsg(uint32_t mid, string &resp_msg, int32_t ret)
{
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize + 4;

    EncodeCommonTlv(&resp_msg, msg_len, mid);
    EncodeRet(&resp_msg, TYPE_ON_DEV_FREE_VC, ret);
}

int32_t TlvDispatcher::HandleDevFreeVc(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    uint32_t type;
    uint32_t len;
    string client_id;
    string dev_id;

    LOG_DEBUG(g_logger, "dev free voice channle, mid is %u", mid);

    char *data = cmd_value.data();
    size_t size = cmd_value.size();

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of client_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_CLIENT_ID;
        goto SEND_RESPONSE;
    }
    client_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of dev_id error, because length is bigger than %d", MAX_GUID_LEN);
        ret = -ERROR_TLV_PARSE_DEV_ID;
        goto SEND_RESPONSE;
    }
    dev_id.assign(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    ret = vc_manager_.DisconnectByDev(dev_id, client_id);

SEND_RESPONSE:
    string resp_msg;
    PackClientFreeVcRespMsg(mid, resp_msg, ret);

    return ret;
}




