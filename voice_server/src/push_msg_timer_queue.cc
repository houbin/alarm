#include "push_msg_timer_queue.h"
#include "../../public/error_code.h"

AtomicUInt32 g_push_msg_mid;

void PushMsgContext::EncodeRespMsg(string *resp_msg, uint32_t msg_len)
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

void PushMsgContext::Finish(int ret)
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

PushMsgTimerQueue::PushMsgTimerQueue()
: mutex_("PushMsgTimerQueue::Mutex"), stop_(false)
{

}

PushMsgTimerQueue::~PushMsgTimerQueue()
{

}

typedef std::multimap < UTime, PushMsgContext *> scheduled_map_t;
typedef std::map < uint32_t, scheduled_map_t::iterator > event_lookup_map_t;

int32_t PushMsgTimerQueue::AddEventAfter(double seconds, uint32_t mid, PushMsgContext *ct)
{
    LOG_DEBUG(g_logger, "add event after %f, mid is %u", seconds, mid);

    UTime when = GetClockNow();
    when += seconds;

    return AddEventAt(when, mid, ct);
}

int32_t PushMsgTimerQueue::AddEventAt(UTime when, uint32_t mid, PushMsgContext *ct)
{
    LOG_DEBUG(g_logger, "add event at %d.%d", when.tv_sec, when.tv_nsec);
    Mutex::Locker lock(mutex_);

    scheduled_map_t::value_type s_val(when, ct);
    scheduled_map_t::iterator iter = schedule_.insert(s_val);

    event_lookup_map_t::value_type e_val(mid, iter);
    pair<event_lookup_map_t::iterator, bool> rval = events_.insert(e_val);
    assert(rval.second);

    if (iter == schedule_.begin())
        cond_.Signal();

    return 0;
}

int32_t PushMsgTimerQueue::CancelEvent(uint32_t mid)
{
    Mutex::Locker lock(mutex_);
    return CancelEventUnlocked(mid);
}

int32_t PushMsgTimerQueue::CancelEventUnlocked(uint32_t mid)
{
    LOG_DEBUG(g_logger, "cancel event, mid is %u", mid);

    event_lookup_map_t::iterator e_iter = events_.find(mid);
    if (e_iter == events_.end())
    {
        LOG_DEBUG(g_logger, "can't find this event, mid is %u", mid);
        return 0;
    }

    PushMsgContext *ct = e_iter->second->second;
    schedule_.erase(e_iter->second);
    events_.erase(mid);

    if (ct)
        delete ct;
    
    return 0;
}

int32_t PushMsgTimerQueue::GetAndCancelEvent(uint32_t mid, PushMsgInfo &push_msg_info)
{
    LOG_DEBUG(g_logger, "get push msg info, mid %u", mid);
    Mutex::Locker lock(mutex_);

    event_lookup_map_t::iterator e_iter = events_.find(mid);
    if (e_iter == events_.end())
    {
        LOG_INFO(g_logger, "can't find event, mid is %u", mid);
        return -ERROR_ITEM_NOT_EXIST;
    }

    scheduled_map_t::iterator s_iter = e_iter->second;
    PushMsgContext *ct = s_iter->second;
    push_msg_info.recv_mid_ = ct->recv_mid_;
    push_msg_info.recv_cfd_ = ct->recv_cfd_;
    push_msg_info.push_mid_ = ct->push_mid_;
    push_msg_info.response_type_ = ct->response_type_;

    CancelEventUnlocked(mid);

    return 0;
}

void PushMsgTimerQueue::Start()
{
    Create();
}

void PushMsgTimerQueue::Shutdown()
{
    LOG_INFO(g_logger, "push msg timer queue shutdown");

    Mutex::Locker lock(mutex_);
    stop_ = true;
    cond_.Signal();

    return;
}

void *PushMsgTimerQueue::Entry()
{
    mutex_.Lock();

    while (!stop_)
    {
        UTime now = GetClockNow();
        while(!schedule_.empty())
        {
            scheduled_map_t::iterator s_iter = schedule_.begin();
            if (s_iter->first > now)
            {
                break;
            }
            
            PushMsgContext *ct = s_iter->second;
            uint32_t mid = ct->GetPushMid();
            schedule_.erase(s_iter);
            events_.erase(mid);

            mutex_.Unlock();
            if (ct)
                ct->Complete(0);
            mutex_.Lock();
        }

        if (stop_)
            break;

        if (schedule_.empty())
            cond_.Wait(mutex_);
        else
            cond_.WaitUtil(mutex_, schedule_.begin()->first);

        LOG_DEBUG(g_logger, "push msg timer awake");
    }
    
    mutex_.Unlock();

    LOG_DEBUG(g_logger, "push msg timer exiting");
    return 0;
}

