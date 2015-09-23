#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <stdint.h>
#include <string>
#include "global.h"
#include "json_opt.h"
#include "connection.h"
#include "voice_channel.h"

using namespace std;

uint32_t g_push_msg_mid = 0;

struct PushMsgInfo
{
    uint32_t recv_mid;
    uint32_t cfd;
};

class Dispatcher
{
public:
    Dispatcher(string data_protocol);
    ~Dispatcher();

    int32_t HandleConnect(ConnectionInfo *conn_info);
    int32_t HandleTimeout(ConnectionInfo *conn_info);
    int32_t HandleClose(ConnectionInfo *conn_info);
    int32_t HandleError(ConnectionInfo *conn_info);
    virtual int32_t HandleMsg(ConnectionInfo *conn_info, string &msg_info);
    virtual int32_t HandleMsg(ConnectionInfo *conn_info, Slice &s);
};

class JsonDispatcher : public Dispatcher
{
public:
    JsonDispatcher();
    ~JsonDispatcher();
    int32_t HandleMsg(ConnectionInfo *conn_info, string &msg_info);
    int32_t HandleGetVoiceServerAddr(ConnectionInfo *conn_info, CJsonOpt &json_opt);
};

class TlvDispatcher : public Dispatcher
{
private:
    VoiceChannelMap voice_channel_map_;

    map<uint32_t, PushMsgInfo> wait_finish_push_msg_map_;

public:
    TlvDispatcher();
    ~TlvDispatcher();
    int32_t HandleMsg(ConnectionInfo *conn_info, Slice &s);

    int32_t EncodeCommonTlv(string *resp_msg, uint32_t msg_len, uint32_t mid);
    int32_t EncodeRet(string *resp_msg, uint32_t on_cmd, int32_t ret);

    int32_t HandleClientBuildVc(ConnectionInfo *conn_info, uint32_t mid, Slice &s);
    int32_t ResponseClientBuildVc(ConnectionInfo *conn_info, uint32_t mid, int32_t ret);

    int32_t HandleClientStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &s);
    int32_t ResponseClientStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &s); 
};

#endif

