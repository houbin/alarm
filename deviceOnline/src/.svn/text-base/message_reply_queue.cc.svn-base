#include "message_reply_queue.h"
#include "redis_opt.h"
#include "../../public/socket_wrapper.h"
#include "../../public/utils.h"
#include "../../public/error_code.h"
#include "logic_opt.h"

MsgReplyQueue::MsgReplyQueue()
: queue_mutex_("MsgReplyQueue::Mutex"), stop_(false)
{

}

MsgReplyQueue::~MsgReplyQueue()
{

}

int MsgReplyQueue::SubmitMsg(ReplyMsg &reply_msg)
{
    Mutex::Locker lock(queue_mutex_);

    reply_msg_queue_.push_back(reply_msg);
    queue_cond_.Signal();

    return 0;
}

int MsgReplyQueue::Start()
{
    Create();
    return 0;
}

void *MsgReplyQueue::Entry()
{
    int ret = 0;
    queue_mutex_.Lock();

    while (!stop_)
    {
        while (!reply_msg_queue_.empty())
        {
            string response_msg;
            ReplyMsg reply_msg = reply_msg_queue_.front();
            reply_msg_queue_.pop_front();

            queue_mutex_.Unlock();

            LOG4CXX_TRACE(g_logger, "MsgReplyQueue pop one reply msg, dev id  " << reply_msg.dev_id << ", reply msg " << reply_msg.reply_msg);

            int fd = -1;
            ret = CLogicOpt::GetDeviceFdFromCache(reply_msg.dev_id, fd);
            if (ret != 0) 
            {
                LOG4CXX_ERROR(g_logger, "CRedisOpt::GetDeviceFdFromCache error, dev_id " << reply_msg.dev_id << ", ret %d" << ret);
                ret = -ERROR_GET_DEVICE_FD_FROM_CACHE;
                goto entry_continue;
            }

            if (fd < 0)
            {
                LOG4CXX_ERROR(g_logger, "MsgReplyQueue fd " << fd);
                ret = -ERROR_FD_INVALID;
                goto entry_continue;
            }

            // TODO: 转义\r\n为\\r\\n
            response_msg = utils::ReplaceString(reply_msg.reply_msg, "\\r\\n", "\\\\r\\\\n");
            response_msg.append("\r\n");
            if (!SocketOperate::WriteSfd(fd, response_msg.c_str(), response_msg.length()))
            {
                LOG4CXX_ERROR(g_logger, "SocketOperate::WriteSfd error, fd " << fd << " reply_msg " << reply_msg.reply_msg);
                SocketOperate::CloseSocket(fd);
                ret = -ERROR_PUSH_MESSAGE;
                goto entry_continue;
            }
        
        entry_continue:
            Context *ct = reply_msg.ct;
            if (ct)
            {
                ct->Complete(ret);
            }
            
            queue_mutex_.Lock();
            continue;
        }

        if (stop_)
        {
            LOG4CXX_INFO(g_logger, "MsgReplyQueue stopped");
            break;
        }

        queue_cond_.Wait(queue_mutex_);
    }

    queue_mutex_.Unlock();
    LOG4CXX_INFO(g_logger, "MsgReplyQueue thread exit");

    return NULL;
}

int MsgReplyQueue::Stop()
{
    LOG4CXX_INFO(g_logger, "MsgReplyQueue stop thread");

    queue_mutex_.Lock();
    stop_ = true;
    queue_cond_.Signal();
    queue_mutex_.Unlock();

    Join();

    LOG4CXX_INFO(g_logger, "MsgReplyQueue stop thread ok");

    return 0;
}

