#include <unistd.h>
#include <errno.h>
#include "../util/common.h"
#include "worker.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "connection.h"
#include "global.h"
#include "../util/slice.h"
#include "../include/utils.h"
#include "tlv_define.h"
#include "master.h"
#include "dispatcher.h"

using namespace util;

// master的连接分配通知
void RecvNotifiedCb(int fd, short event, void *arg)
{
    assert(arg != NULL);
    Worker *worker = (Worker *)arg;
    worker->RecvNotifiedCb(fd, event, arg);

    return;
}

void CloseConn(ConnectionInfo *conn_info, uint32_t op_type)
{
    assert(conn_info != NULL);

    Worker *worker = conn_info->worker;
    string data_protocol = worker->GetDataProtocol();
    Dispatcher *dispatcher = worker->GetDispatcher();

    switch(op_type)
    {
        case D_TIMEOUT:
            dispatcher->HandleTimeout(conn_info);
            break;
        case D_CLOSE:
            dispatcher->HandleClose(conn_info);
            break;
        case D_ERROR:
            dispatcher->HandleError(conn_info);
            break;
        default:
            LOG_ERROR(g_logger, "invalid op type");
            break;
    }

    conn_info->in_buffer_len = 0;
    bufferevent_free(conn_info->buffer_event);

    delete conn_info;
}

void CloseErrorConn(ConnectionInfo *conn_info)
{
    assert(conn_info != NULL);
    CloseConn(conn_info, D_ERROR);
}

void ClientTcpReadCb(struct bufferevent *bev, void *arg)
{
    int ret = 0;
    //uint32_t msg_length = 0;

    ConnectionInfo *conn_info = (ConnectionInfo *)arg;
    assert(conn_info != NULL);

    Worker *worker = conn_info->worker;
    string data_protocol = worker->GetDataProtocol();
    Dispatcher *dispatcher = worker->GetDispatcher();

    while (true)
    {
        // read util no data or full buffer
        uint32_t left_size = 0;
        while (true)
        {
            left_size = CONN_BUFFER_LEN - conn_info->in_buffer_len;
            if (left_size == 0)
                break;

            ret = bufferevent_read(bev, conn_info->in_buffer + conn_info->in_buffer_len, CONN_BUFFER_LEN - conn_info->in_buffer_len);
            if (ret <= 0)
                break;
            conn_info->in_buffer_len += ret;
        }

        // 处理所有信息
        while (true)
        {
            if (data_protocol == DATA_PROTOCOL_JSON)
            {
                // json协议是以\r\n为消息边界
                string recv_str(conn_info->in_buffer, conn_info->in_buffer_len);
                vector<string> json_data_vec;
                if (utils::FindCRLF(recv_str))
                {
                    utils::SplitData(recv_str, CRLF, json_data_vec);
                    unsigned int i = 0;
                    for(; i < json_data_vec.size(); i++)
                    {
                        ret = dispatcher->HandleMsg(conn_info, json_data_vec[i]);
                        if (ret != 0)
                        {
                            CloseErrorConn(conn_info);
                            return;
                        }
                    }
                    int len = recv_str.find_last_of(CRLF) + 1;
                    memmove(conn_info->in_buffer, conn_info->in_buffer + len, CONN_BUFFER_LEN - len);
                    conn_info->in_buffer_len -= len;

                    if (left_size == 0)
                    {
                        break;
                    }
                    else
                        return;
                }
            }
            else if (data_protocol == DATA_PROTOCOL_TLV)
            {
                // tlv是自己根据消息长度划分消息
                if (conn_info->in_buffer_len < kMsgHeaderSize)
                {
                    if (left_size == 0)
                        break;
                    else
                        return;
                }

                uint32_t type = DecodeFixed32(conn_info->in_buffer);
                uint32_t length = DecodeFixed32(conn_info->in_buffer + 4);
                uint32_t msg_length = DecodeFixed32(conn_info->in_buffer + 8);
                if (type != TYPE_LENGTH || length != 4 || msg_length > CONN_BUFFER_LEN || msg_length < kMsgHeaderSize)
                {
                    // 请求包不合法
                    LOG_ERROR(g_logger, "pkt invalid, type is %u, length is %u, msg length is %u", type, length, msg_length);
                    CloseErrorConn(conn_info);
                    return;
                }

                if (conn_info->in_buffer_len < msg_length)
                {
                    if (left_size == 0)
                        break;
                    else
                        continue;
                }

                // 处理数据
                //Slice tlv_data(conn_info->in_buffer + 12, msg_length - 12);
                string tlv_data(conn_info->in_buffer + 12, msg_length - 12);
                ret = dispatcher->HandleMsg(conn_info, tlv_data);
                if (ret != 0)
                {
                    CloseErrorConn(conn_info);
                    return;
                }

                // 处理下一个协议包，或者继续读
                conn_info->in_buffer_len -= msg_length;
                if (conn_info == 0)
                {
                    if (left_size == 0)
                        break;
                    else
                        return;
                }

                memmove(conn_info->in_buffer, conn_info->in_buffer + msg_length, conn_info->in_buffer_len);
            }
            else
            {
                LOG_ERROR(g_logger, "data protocol invalid");
            }
        }
    }

    return;
}

void ClientTcpErrorCb(struct bufferevent *bev, short event, void *arg)
{
    assert(arg != NULL);
    ConnectionInfo *conn_info = (ConnectionInfo *)arg;
    uint32_t op_type;

    if (event & BEV_EVENT_TIMEOUT)
    {
        LOG_WARN(g_logger, "client tcp event timeout, conn id %" PRIu64 ", cfd %d, ip %s, port %u", 
        conn_info->conn_id, conn_info->cfd, conn_info->cip.c_str(), conn_info->cport);
        op_type = D_TIMEOUT;
    }
    else if (event & BEV_EVENT_EOF)
    {
        LOG_WARN(g_logger, "client tcp event eof, conn id %" PRIu64 ", cfd %d, ip %s, port %u", 
        conn_info->conn_id, conn_info->cfd, conn_info->cip.c_str(), conn_info->cport);
        op_type = D_CLOSE;
    }
    else if (event & BEV_EVENT_ERROR)
    {
        int error_code = EVUTIL_SOCKET_ERROR();
        LOG_DEBUG(g_logger, "client tcp event error, error code %d, msg %s, conn id %" PRIu64 ", cfd %d, ip %s, port %u", 
                    error_code, evutil_socket_error_to_string(error_code), conn_info->conn_id, conn_info->cfd, 
                    conn_info->cip.c_str(), conn_info->cport);
        op_type = D_ERROR;
    }

    CloseConn(conn_info, op_type);

    return;
}

Worker::Worker(int i, uint32_t worker_conn_count, int read_timeout, int write_timeout, Master *master)
: id_(i), worker_conn_count_(worker_conn_count), read_timeout_(read_timeout), 
write_timeout_(write_timeout), master_(master), conn_info_queue_mutex_("Worker::ConnectionInfoQueueMutex")
{
    base_ = NULL;
    notified_wfd_ = -1;
    notified_rfd_ = -1;
    notified_event_ = NULL;
}

Worker::~Worker()
{
    
}

int32_t Worker::Init()
{
    int ret = 0;
    int fds[2];

    ret = pipe(fds);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "create pipe error, error %d, msg %d", -errno, strerror(errno));
        return -1;
    }

    notified_rfd_ = fds[0];
    notified_wfd_ = fds[1];

    base_ = event_base_new();
    if(base_ == NULL)
    {
        LOG_ERROR(g_logger, "event_base_new error");
        return -1;
    }

    notified_event_ = event_new(base_, notified_rfd_, EV_READ|EV_PERSIST, ::RecvNotifiedCb, (void *)this);
    if (notified_event_ == NULL)
    {
        LOG_ERROR(g_logger, "event_new error");
        return -1;
    }

    ret = event_add(notified_event_, NULL);
    if (ret != 0)
    {
        LOG_ERROR(g_logger, "event_add error");
        return -1;
    }

    return 0;
}

int32_t Worker::Start()
{
    Create();
    return 0;
}

void* Worker::Entry()
{
    //int ret = 0;
    
    LOG_INFO(g_logger, "worker %d run", id_);

    event_base_dispatch(base_);
    event_free(notified_event_);
    event_base_free(base_);

    LOG_INFO(g_logger, "worker %d done", id_);

    return NULL;
}

void Worker::Wait()
{
    Join();

    return;
}

void Worker::Shutdown()
{
    LOG_INFO(g_logger, "worker %d try to shutdown", id_);
    event_base_loopbreak(base_);

    Wait();
    LOG_INFO(g_logger, "worker %d shutdown ok", id_);
    
    return;
}

int Worker::GetId()
{
    return id_;
}

int Worker::GetNotifiedWFd()
{
    return notified_wfd_;
}

void Worker::RecvNotifiedCb(int fd, short event, void *arg)
{
    //int ret = 0;
    char buf[1] = {0};

    if (read(fd, buf, 1) != 1)
    {
        LOG_ERROR(g_logger, "read master -> worker notify error");
        return;
    }

    ConnectionInfo *conn_info = NULL;
    {
        Mutex::Locker lock(conn_info_queue_mutex_);
        conn_info = conn_info_queue_.front();
        conn_info_queue_.pop_front();
        assert(conn_info != NULL);
        assert(conn_info->cfd > 0);
    }

    LOG_DEBUG(g_logger, "pop conn info, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd,
               conn_info->cip.c_str(), conn_info->cport);

    int be_flags = BEV_OPT_CLOSE_ON_FREE;
    conn_info->buffer_event = bufferevent_socket_new(base_, conn_info->cfd, be_flags);
    if (conn_info->buffer_event == NULL)
    {
        int error_code = EVUTIL_SOCKET_ERROR();
        LOG_ERROR(g_logger, "bufferevent_socket_new error, error %d, msg %s", error_code, evutil_socket_error_to_string(error_code));
        return;
    }

    bufferevent_setcb(conn_info->buffer_event, ::ClientTcpReadCb, NULL, ::ClientTcpErrorCb, (void *)conn_info);
    bufferevent_enable(conn_info->buffer_event, EV_READ|EV_PERSIST);
    struct timeval hb_val;
    if (GetDataProtocol() == "json")
    {
        hb_val.tv_sec = g_config->Read("json_read_timeout", 60);
    }
    else
        hb_val.tv_sec = g_config->Read("tlv_read_timeout", 60);
    hb_val.tv_usec = 0;
    bufferevent_set_timeouts(conn_info->buffer_event, &hb_val, NULL);

    conn_info->in_buffer_len = 0;
    memset(conn_info->in_buffer, 0, sizeof(conn_info->in_buffer));
    //conn_info->out_buffer_len = 0;
    //memset(conn_info->out_buffer, 0, sizeof(conn_info->out_buffer));

    //test sleep 10s
    //sleep(10);
    LOG_DEBUG(g_logger, "add client to worker, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd, 
                        conn_info->cip.c_str(), conn_info->cport);
    
    return;
}

int32_t Worker::PutConnInfo(ConnectionInfo *conn_info)
{
    LOG_DEBUG(g_logger, "put conn info, conn id %" PRIu64 "", conn_info->conn_id);
    Mutex::Locker lock(conn_info_queue_mutex_);
    conn_info_queue_.push_back(conn_info);

    return 0;
}

string Worker::GetDataProtocol()
{
    return master_->GetDataProtocol();
}

Dispatcher* Worker::GetDispatcher()
{
    return master_->GetDispatcher();
}

