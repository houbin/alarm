#include <unistd.h>
#include <errno.h>
#include "../util/common.h"
#include "worker.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "connection.h"
#include "global.h"

using namespace util;

namespace tcpserver
{

// master的连接分配通知
void RecvNotifiedCb(int fd, short event, void *arg)
{
    assert(arg != NULL);
    Worker *worker = (Worker *)arg;
    worker->RecvNotifiedCb(fd, event, arg);

    return;
}

void CloseConn(ConnectionInfo *conn_info)
{
    conn_info->in_buffer_len = 0;

    bufferevent_free(conn_info->buffer_event);
    safe_close(conn_info->cfd);

    delete conn_info;
}

void CloseErrorConn(ConnectionInfo *conn_info)
{
    assert(conn_info != NULL);
    CloseConn(conn_info);
}

int DelimitJsonData(string recv_str, vector<string> &json_data_vec)
{
    // json数据以"\r\n"为边界
}

int ParseJsonData()


void ClientTcpReadCb(struct bufferevent *bev, void *arg)
{
    int ret = 0;
    uint32_t msg_length = 0;

    ConnectionInfo *conn_info = (ConnectionInfo *)arg;
    assert(conn_info != NULL);

    Worker *worker = conn_info->worker;
    string data_protocol = worker->GetDataProtocol();
    DispatchQueue *dispatch_queue = worker->GetDispatchQueue();

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
                vector<string> json_data_str;
                if (utils::FindCRLF(str_recv))
                {
                    utils::SplitData(str_recv, CRLF, json_data_vec);
                    int i = 0;
                    for(; i < json_data_vec.size(); i++)
                    {
                        QueueItem item(PRIORITY_DEFAULT, json_data_vec[i]);
                        dispatch_queue->Enqueue(item);
                    }
                    int len = str_recv.find_last_of(CRLF) + 1;
                    memmove(conn_info->in_buffer, conn_info->in_buffer + len, DATA_BUFFER_SIZE - len);
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
                int left_len = conn_info->in_buffer_len
                while(left_size > 0)
                {
                    if (left_size < kMsgHeaderSize)
                    {
                        break;
                    }

                    uint32_t type = DecodeFixed32(conn_info->in_buffer);
                    uint32_t length = DecodeFixed32(conn_info->in_buffer + 4);
                    if (length > DATA_BUFFER_SIZE)
                    {
                        LOG_ERROR(g_logger, "tlv length is large than DATA_BUFFER_SIZE, type is %u, length is %u", type, length);
                        conn_info->in_buffer_len = 0;
                        left_size = 0;
                        break;
                    }

                    if (length > left_size)
                    {
                        // data not ok
                        break;
                    }

                    string tlv_date(conn_info->in_buffer, length);
                    QueueItem item(PRIORITY_DEFAULT, tlv_date);
                    dispatch_queue->Enqueue(item);

                    left_size = conn_info->in_buffer_len - length;
                    memmove(conn_info->in_buffer, conn_info->in_buffer + length, left_size);
                    conn_info->in_buffer_len = left_size;
                }
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

    struct in_addr client_addr;
    client_addr.s_addr = conn_info->cip;
    LOG_DEBUG(logger, "client tcp error, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd,
                            inet_ntoa(client_addr), conn_info->cport);

    if (event & BEV_EVENT_TIMEOUT)
    {
        LOG_WARN(logger, "client tcp event timeout, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd,
                            inet_ntoa(client_addr), conn_info->cport);
    }
    else if (event & BEV_EVENT_EOF)
    {
        LOG_WARN(logger, "client tcp event eof, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd,
                            inet_ntoa(client_addr), conn_info->cport);
    
    }
    else if (event & BEV_EVENT_ERROR)
    {
        int error_code = EVUTIL_SOCKET_ERROR();
        LOG_DEBUG(logger, "client tcp event error, error code %d, msg %s, conn id %" PRIu64 ", cfd %d, ip %s, port %u", 
                    error_code, evutil_socket_error_to_string(error_code), conn_info->conn_id, conn_info->cfd, 
                    inet_ntoa(client_addr), conn_info->cport);
    }

    bufferevent_free(conn_info->buffer_event);
    safe_close(conn_info->cfd);

    return;
}

Worker::Worker(Logger *logger, int i, uint32_t worker_conn_count, int read_timeout, int write_timeout, Master *master)
: logger_(logger), id_(i), worker_conn_count_(worker_conn_count), read_timeout_(read_timeout), 
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
        LOG_ERROR(logger_, "create pipe error, error %d, msg %d", -errno, strerror(errno));
        return -1;
    }

    notified_rfd_ = fds[0];
    notified_wfd_ = fds[1];

    base_ = event_base_new();
    if(base_ == NULL)
    {
        LOG_ERROR(logger_, "event_base_new error");
        return -1;
    }

    notified_event_ = event_new(base_, notified_rfd_, EV_READ|EV_PERSIST, tcpserver::RecvNotifiedCb, (void *)this);
    if (notified_event_ == NULL)
    {
        LOG_ERROR(logger_, "event_new error");
        return -1;
    }

    ret = event_add(notified_event_, NULL);
    if (ret != 0)
    {
        LOG_ERROR(logger_, "event_add error");
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
    int ret = 0;
    
    event_base_dispatch(base_);

    LOG_INFO(logger_, "event_base_dispatch return");

    return NULL;
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
    int ret = 0;
    char buf[1] = {0};

    if (read(fd, buf, 1) != 1)
    {
        LOG_ERROR(logger_, "read master -> worker notify error");
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

    struct in_addr client_addr;
    client_addr.s_addr = conn_info->cip;
    LOG_DEBUG(logger_, "pop conn info, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd,
                inet_ntoa(client_addr), conn_info->cport);

    conn_info->buffer_event = bufferevent_socket_new(base_, conn_info->cfd, BEV_OPT_CLOSE_ON_FREE);
    if (conn_info->buffer_event == NULL)
    {
        int error_code = EVUTIL_SOCKET_ERROR();
        LOG_ERROR(logger_, "bufferevent_socket_new error, error %d, msg %s", error_code, evutil_socket_error_to_string(error_code));
        return;
    }

    bufferevent_setcb(conn_info->buffer_event, tcpserver::ClientTcpReadCb, NULL, ClientTcpErrorCb, (void *)conn_info);
    bufferevent_enable(conn_info->buffer_event, EV_READ|EV_PERSIST);

    conn_info->in_buffer_len = 0;
    memset(conn_info->in_buffer, 0, sizeof(conn_info->in_buffer));
    conn_info->out_buffer_len = 0;
    memset(conn_info->out_buffer, 0, sizeof(conn_info->out_buffer));

    //test sleep 10s
    //sleep(10);
    LOG_DEBUG(logger_, "add client to worker, conn id %" PRIu64 ", cfd %d, ip %s, port %u", conn_info->conn_id, conn_info->cfd, 
                        inet_ntoa(client_addr), conn_info->cport);
    
    return;
}

int32_t Worker::PutConnInfo(ConnectionInfo *conn_info)
{
    LOG_DEBUG(logger_, "put conn info, conn id %" PRIu64 "", conn_info->conn_id);
    Mutex::Locker lock(conn_info_queue_mutex_);
    conn_info_queue_.push_back(conn_info);

    return 0;
}

string Worker::GetDataProtocol()
{
    return master_->GetDataProtocol();
}

DispatchQueue* Worker::GetDispatchQueue()
{
    return master_->GetDispatchQueue();
}

}

