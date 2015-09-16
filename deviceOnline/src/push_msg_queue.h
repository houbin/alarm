#ifndef PUSH_MSG_QUEUE_
#define PUSH_MSG_QUEUE_

#include <string>
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include "list"
#include "context.h"
#include <map>
#include "libjson/_internal/Source/JSONNode.h"

using namespace std;
using namespace util;

class WaitFinishPushMsgQueue;
extern WaitFinishPushMsgQueue g_wait_finish_push_msg_queue;

struct PushMsg
{
    int push_cnt;    //推送出去的消息号

    string dev_id;
    string push_msg;

    Context *ct;
};

class WaitFinishPushMsgQueue
{
private:
    Mutex queue_mutex_;
    map<int, PushMsg> wait_finish_msg_queue_;

public:
    WaitFinishPushMsgQueue();
    ~WaitFinishPushMsgQueue();

    int SubmitWaitFinishPushMsg(int push_cnt, PushMsg push_msg);
    int FinishPushMsg(int push_cnt, int ret, JSONNode &param_node);
};

class PushMsgQueue : public Thread
{
private:
    Mutex queue_mutex_;
    Cond queue_cond_;
    list<PushMsg> push_msg_queue_;

    bool stop_;
public:
    PushMsgQueue();
    ~PushMsgQueue();

    int SubmitMsg(PushMsg &push_msg);

    int Start();
    void *Entry();
    int Stop();
};

#endif

