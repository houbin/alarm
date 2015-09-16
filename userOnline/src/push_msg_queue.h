#ifndef PUSH_MSG_QUEUE_H_
#define PUSH_MSG_QUEUE_H_

#include <string>
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include "list"
#include "context.h"
#include <map>

using namespace std;
using namespace util;

class WaitFinishPushMsgQueue;
extern WaitFinishPushMsgQueue g_wait_finish_push_msg_queue;

struct PushMsg
{
    int push_mid;

    string guid;
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

    int SubmitWaitFinishPushMsg(int push_mid, PushMsg push_msg);
    int FinishPushMsg(int push_mid, int ret);
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

