#ifndef PUSH_MSG_TIMER_QUEUE_H_
#define PUSH_MSG_TIMER_QUEUE_H_

#include <map>
#include "../util/mutex.h"
#include "../util/cond.h"
#include "../util/utime.h"
#include "../util/thread.h"
#include "../util/atomic.h"
#include "../util/coding.h"
#include "../util/context.h"
#include "../include/socket_wrapper.h"
#include "../../public/error_code.h"
#include "global.h"
#include "tlv_define.h"

using namespace std;
using namespace util;

extern AtomicUInt32 g_push_msg_mid;

struct PushMsgInfo
{
    uint32_t recv_mid_;
    int recv_cfd_;

    uint32_t push_mid_;
    uint32_t response_type_;
};

class PushMsgContext : public Context
{
public:
    uint32_t recv_mid_;
    int recv_cfd_;

    uint32_t push_mid_;
    uint32_t response_type_;

    PushMsgContext(uint32_t recv_mid, int recv_cfd, uint32_t push_mid, uint32_t response_type) : 
        recv_mid_(recv_mid), recv_cfd_(recv_cfd), push_mid_(push_mid), response_type_(response_type) { }
    ~PushMsgContext() { }

    uint32_t GetPushMid() { return push_mid_; }

    void EncodeRespMsg(string *resp_msg, uint32_t msg_len);
    void Finish(int ret);
};

class PushMsgTimerQueue : public Thread
{
private:
    Mutex mutex_;
    Cond cond_;

    multimap<UTime, PushMsgContext*> schedule_;
    map<uint32_t, multimap<UTime, PushMsgContext*>::iterator> events_;

    bool stop_;

public:
    PushMsgTimerQueue();
    ~PushMsgTimerQueue();
    
    int32_t AddEventAfter(double seconds, uint32_t mid, PushMsgContext *push_msg_ct);
    int32_t AddEventAt(UTime when, uint32_t mid, PushMsgContext *ct);
    int32_t CancelEvent(uint32_t mid);
    int32_t CancelEventUnlocked(uint32_t mid);
    int32_t GetAndCancelEvent(uint32_t mid, PushMsgInfo &push_msg_info);

    void Start();
    void Shutdown();
    void *Entry();
};

#endif

