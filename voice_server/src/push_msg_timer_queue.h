#ifndef PUSH_MSG_TIMER_QUEUE_H_
#define PUSH_MSG_TIMER_QUEUE_H_

#include <map>
#include "../util/mutex.h"
#include "../util/cond.h"
#include "../uitl/utime.h"
#include "../util/thread.h"
#include "../util/atomic.h"
#include "../util/coding.h"
#include "global.h"
#include "tlv_define.h"
#include "connection.h"

using namespace util;
using namespace std;

AtomicUInt32 g_push_msg_mid = 0;

struct PushMsgInfo
{
    uint32_t recv_mid_;
    int recv_cfd_;

    uint32_t push_mid_;
    uint32_t response_type_;
};

class PushMsgContxt : public Context
{
public:
    uint32_t recv_mid_;
    int recv_cfd_;

    uint32_t push_mid_;
    uint32_t response_type_;

    PushMsgContxt(uint32_t recv_mid, int recv_cfd, uint32_t push_mid, uint32_t response_type) : 
        recv_mid_(recv_mid), recv_cfd_(recv_cfd), push_mid_(push_mid), response_type_(response_type) { }
    ~PushMsgContxt() { }

    uint32_t GetPushMid() { return push_mid_; }

    void EncodeRespMsg(string *resp_msg, uint32_t msg_len)
    {
        PutFixed32(resp_msg, TYPE_LENGTH);
        PutFixed32(resp_msg, 4);
        PutFixed32(resp_msg, msg_len);
    
        PutFixed32(resp_msg, TYPE_MID);
        PutFixed32(resp_msg, 4);
        PutFixed32(resp_msg, recv_mid_);

        PutFixed32(resp_msg, response_type_);
        PutFixed32(resp_msg, 4);
        PutFixed32(resp_msg, -ERROR_RECV_PUSH_MSG_RESP_TIMEOUT);
    }

    void Finish(int ret)
    {
        string msg;
        uint32_t msg_len = 3 * (kMsgHeaderSize + 4);

        EncodeRespMsg(&msg, msg_len);

        if (!SocketOperate::WriteSfd(recv_cfd_, msg.c_str(), msg.size()))
        {
            LOG_ERROR(g_logger, "send response msg error, mid is %u, type is %u", recv_mid_, response_type_);
        } 

        return;
    }
};

class PushMsgTimerQueue : public Thread
{
private:
    Mutex mutex_;
    Cond cond_;

    multimap<UTime, PushMsgContxt*> schedule_;
    map<uint32_t, multimap<UTime, PushMsgContxt*>::iterator> events_;

    bool stop_;

public:
    PushMsgTimerQueue();
    ~PushMsgTimerQueue();
    
    int32_t AddEventAfter(double seconds, uint32_t mid, PushMsgContext *push_msg_ct);
    int32_t AddEventAt(UTime t when, uint32_t mid, PushMsgContext *ct);
    int32_t CancelEvent(uint32_t mid);
    int32_t CancelEventUnlocked(uint32_t mid);
    int32_t GetAndCancelEvent(uint32_t mid, PushMsgInfo &push_msg_info);

    void Start();
    void Shutdown()

    void *Entry();
};

#endif
