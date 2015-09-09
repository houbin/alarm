#ifndef MESSAGE_REPLY_QUEUE_H_
#define MESSAGE_REPLY_QUEUE_H_

#include <string>
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include "list"
#include "context.h"

using namespace std;
using namespace util;

struct ReplyMsg
{
    string guid;
    string reply_msg;

    Context *ct;
};

class MsgReplyQueue : public Thread
{
private:
    Mutex queue_mutex_;
    Cond queue_cond_;
    list<ReplyMsg> reply_msg_queue_;

    bool stop_;
public:
    MsgReplyQueue();
    ~MsgReplyQueue();

    int SubmitMsg(ReplyMsg &reply_msg);

    int Start();
    void *Entry();
    int Stop();
};

#endif

