#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <stdint.h>
#include <string>
#include "global.h"
#include "json_opt.h"
#include "connection.h"
#include "voice_channel.h"
#include "push_msg_timer_queue.h"

using namespace std;

class PushMsgContext;
struct ConnectionInfo;

class Dispatcher
{
private:
    string data_protocol_;

public:
    Dispatcher(string data_protocol);
    ~Dispatcher();

    int32_t HandleConnect(ConnectionInfo *conn_info);
    int32_t HandleTimeout(ConnectionInfo *conn_info);
    int32_t HandleClose(ConnectionInfo *conn_info);
    int32_t HandleError(ConnectionInfo *conn_info);
    virtual int32_t HandleMsg(ConnectionInfo *conn_info, string &msg_info) = 0;
    //virtual int32_t HandleMsg(ConnectionInfo *conn_info, Slice &s) = 0;
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
    VoiceChannelManager vc_manager_;
    PushMsgTimerQueue push_msg_timer_queue_;

public:
    TlvDispatcher();
    ~TlvDispatcher();
    int32_t HandleMsg(ConnectionInfo *conn_info, string &tlv_data);

    void EncodeCommonTlv(string *resp_msg, uint32_t msg_len, uint32_t mid);
    void EncodeRet(string *resp_msg, uint32_t on_cmd, int32_t ret);

    int32_t SendMsg(int fd, string msg);
    int32_t ForwardMsgToDev(int push_fd, uint32_t push_mid, string &push_msg, PushMsgContext *ct);

    void PackHeartBeatRespMsg(uint32_t mid, int32_t ret, string &resp_msg);
    int32_t HandleHeartbeat(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackClientBuildVcRespMsg(uint32_t mid, int32_t ret, string &string_msg);
    int32_t HandleClientBuildVc(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackStartSendPushMsg(string &push_msg, uint32_t &push_mid);
    int32_t HandleClientStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackStartSendRespMsg(uint32_t mid, string &resp_msg, int32_t ret);
    int32_t HandleOnPushStartSend(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackSendVoicePushMsg(string &push_msg, uint32_t &push_mid, Slice &s);
    int32_t HandleClientSendVoice(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackClientSendVoiceRespMsg(uint32_t mid, string &resp_msg, int32_t ret);
    int32_t HandleOnPushClientVoice(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackClientFreeVcRespMsg(uint32_t mid, string &resp_msg, int32_t ret);
    int32_t HandleClientFreeVc(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    // dev protocol
    void PackDevBuildVcRespMsg(uint32_t mid, string &resp_msg, int32_t ret);
    int32_t HandleDevBuildVc(ConnectionInfo *conn_info, uint32_t mid, Slice &s);

    void PackDevFreeVcRespMsg(uint32_t mid, string &resp_msg, int32_t ret);
    int32_t HandleDevFreeVc(ConnectionInfo *conn_info, uint32_t mid, Slice &s);
};

#endif

