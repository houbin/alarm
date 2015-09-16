#include "push_msg_queue.h"
#include "redis_opt.h"
#include "../../public/socket_wrapper.h"
#include "../../public/utils.h"
#include "../../public/error_code.h"
#include "local_transport.h"
#include "logic_opt.h"

WaitFinishPushMsgQueue g_wait_finish_push_msg_queue;

WaitFinishPushMsgQueue::WaitFinishPushMsgQueue()
    : queue_mutex_("WaitFinishPushMsgQueue::Mutex")
{

}

WaitFinishPushMsgQueue::~WaitFinishPushMsgQueue()
{

}

int WaitFinishPushMsgQueue::SubmitWaitFinishPushMsg(int push_cnt, PushMsg push_msg)
{
    Mutex::Locker lock(queue_mutex_);

    // TODO:如果有push_cnt相同的怎么办呢
    wait_finish_msg_queue_.insert(pair<int, PushMsg>(push_cnt, push_msg));

    return 0;
}

int WaitFinishPushMsgQueue::FinishPushMsg(int push_cnt, int ret, JSONNode &param_node)
{
    Mutex::Locker lock(queue_mutex_);

    map<int, PushMsg>::iterator iter = wait_finish_msg_queue_.find(push_cnt);
    if (iter == wait_finish_msg_queue_.end())
    {
        return 0;
    }

    Context *ct = iter->second.ct;
    if (ct)
    {
        PushMsgRespContext *resp_ct = (PushMsgRespContext *)ct;
        resp_ct->SetParamNode(param_node);
        resp_ct->Complete(ret);
    }

    return 0;
}

PushMsgQueue::PushMsgQueue()
: queue_mutex_("PushMsgQueue::Mutex"), stop_(false)
{

}

PushMsgQueue::~PushMsgQueue()
{

}

int PushMsgQueue::SubmitMsg(PushMsg &push_msg)
{
    Mutex::Locker lock(queue_mutex_);

    push_msg_queue_.push_back(push_msg);
    queue_cond_.Signal();

    return 0;
}

int PushMsgQueue::Start()
{
    Create();
    return 0;
}

void *PushMsgQueue::Entry()
{
    int ret = 0;
    Context *ct = NULL;
    queue_mutex_.Lock();

    while (!stop_)
    {
        while (!push_msg_queue_.empty())
        {
            string response_msg;
            PushMsg push_msg = push_msg_queue_.front();
            push_msg_queue_.pop_front();

            queue_mutex_.Unlock();

            LOG4CXX_TRACE(g_logger, "PushMsgQueue pop one push msg, dev id  " << push_msg.dev_id << ", push msg " << push_msg.push_msg);

            int fd = -1;
            ret = CLogicOpt::GetDeviceFdFromCache(push_msg.dev_id, fd);
            if (ret != 0) 
            {
                LOG4CXX_ERROR(g_logger, "CRedisOpt::GetDeviceFdFromCache error, dev_id " << push_msg.dev_id << ", ret %d" << ret);
                ret = -ERROR_GET_DEVICE_FD_FROM_CACHE;
                goto entry_finish;
            }

            if (fd < 0)
            {
                LOG4CXX_ERROR(g_logger, "PushMsgQueue fd " << fd);
                ret = -ERROR_FD_INVALID;
                goto entry_finish;
            }

            // TODO: 转义\r\n为\\r\\n
            response_msg = utils::ReplaceString(push_msg.push_msg, "\\r\\n", "\\\\r\\\\n");
            response_msg.append("\r\n");
            if (!SocketOperate::WriteSfd(fd, response_msg.c_str(), response_msg.length()))
            {
                LOG4CXX_ERROR(g_logger, "SocketOperate::WriteSfd error, fd " << fd << " push_msg " << response_msg);
                ret = -ERROR_PUSH_MESSAGE;
                goto entry_finish;
            }

            g_wait_finish_push_msg_queue.SubmitWaitFinishPushMsg(push_msg.push_cnt, push_msg);
            goto entry_continue;
        
        entry_finish:
            ct = push_msg.ct;
            if (ct)
            {
                ct->Complete(ret);
            }
            
        entry_continue:
            queue_mutex_.Lock();
            continue;
        }

        if (stop_)
        {
            LOG4CXX_INFO(g_logger, "PushMsgQueue stopped");
            break;
        }

        queue_cond_.Wait(queue_mutex_);
    }

    queue_mutex_.Unlock();
    LOG4CXX_INFO(g_logger, "PushMsgQueue thread exit");

    return NULL;
}

int PushMsgQueue::Stop()
{
    LOG4CXX_INFO(g_logger, "PushMsgQueue stop thread");

    queue_mutex_.Lock();
    stop_ = true;
    queue_cond_.Signal();
    queue_mutex_.Unlock();

    Join();

    LOG4CXX_INFO(g_logger, "PushMsgQueue stop thread ok");

    return 0;
}

