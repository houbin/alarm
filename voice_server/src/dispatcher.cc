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
            ret = ResponseClientBuildVc(conn_info, mid, ret);
            if (ret != 0)
            {
                return ret;
            }
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

    LOG_DEBUG(g_logger, "tlv HandleMsg ok");
    return 0;
}

void TlvDispatcher::EncodeCommonTlv(string *resp_msg, uint32_t msg_len, uint32_t mid)
{
    PutFixed32(resp_msg, TYPE_LENGTH);
    PutFixed32(resp_msg, 4);
    PutFixed32(resp_msg, msg_len);

    PutFixed32(resp_msg, TYPE_MID);
    PutFixed32(resp_msg, 4);
    PutFixed32(resp_msg, mid);

    return;
}

void TlvDispatcher::EncodeRet(string *resp_msg, uint32_t on_cmd, int32_t ret)
{
    PutFixed32(resp_msg, on_cmd);
    PutFixed32(resp_msg, 4);
    PutFixed32(resp_msg, ret);

    return;
}

int32_t TlvDispatcher::HandleClientBuildVc(ConnectionInfo *conn_info, uint32_t mid, Slice &cmd_value)
{
    int32_t ret;
    uint32_t type;
    uint32_t len;
    LOG_DEBUG(g_logger, "client build voice channel, and mid is %u", mid);
    
    int size = cmd_value.size();
    char *data = cmd_value.data();

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of client_id error, because length is bigger than %d", MAX_GUID_LEN);
        return -ERROR_TLV_PARSE_CLIENT_ID;
    }
    string client_id(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    type = DecodeFixed32(data);
    len = DecodeFixed32(data + 4);
    if (len > MAX_GUID_LEN)
    {
        LOG_ERROR(g_logger, "parse length of dev_id error, because length is bigger than %d", MAX_GUID_LEN);
        return -ERROR_TLV_PARSE_DEV_ID;
    }
    string dev_id(data + kMsgHeaderSize, len);
    data += kMsgHeaderSize + len;

    LOG_DEBUG(g_logger, "mid is %u, client_id is %s, dev_id is %s", mid, client_id, dev_id);

    ret = voice_channel_map_.ConnectByClient(dev_id, client_id, conn_info->cfd);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "connect by client error, ret is %d", ret);
        return ret;
    }

    LOG_DEBUG(g_logger, "client build voice channel ok, and mid is %u, client id is %s, dev id is %s", mid, client_id, dev_id);

    return 0;
}

int32_t TlvDispatcher::ResponseClientBuildVc(ConnectionInfo *conn_info, uint32_t mid, int32_t ret)
{
    LOG_DEBUG(g_logger, "reponse msg of client build vc, mid is %u, ret is %d", mid, ret);

    string resp_msg;
    uint32_t msg_len = kMsgHeaderSize + 4 + kMsgHeaderSize + 4 + kMsgHeaderSize + 4;

    EncodeCommonTlv(&resp_msg, msg_len, mid);
    EncodeRet(&resp_msg, TYPE_ON_CLIENT_BUILD_VC, ret);

    if (!SocketOperate::WriteSfd(conn_info->cfd, resp_msg.c_str(), resp_msg.size()))
    {
        LOG_ERROR(g_logger, "write msg error, mid is %u", mid);
        return -ERROR_TLV_SEND_MSG;
    } 
    LOG_DEBUG(g_logger, "reponse msg of client build vc ok, mid is %u", mid);

   return 0;
}

int32_t TlvDispatcher::HandleClientStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &s)
{
    LOG_DEBUG(g_logger, "client start to send, and mid is %u", mid);

    char *data = s.data();
    int size = s.size();

    if (size != 0)
    {
        LOG_ERROR(g_logger, "cmd value of client start send error");
        return -ERROR_TLV_LEN_NOT_0;
    }

    return 0;
}

int32_t TlvDispatcher::ResponseClient




