#ifndef TCP_SERVER_WORKER_H_
#define TCP_SERVER_WORKER_H_

#include "../util/logger.h"
#include "connection.h"
#include "master.h"
#include <deque>

using namespace util;

namespace tcpserver
{

class Master;
class Worker : public Thread
{
public:
    Worker(Logger *logger, int i, uint32_t worker_conn_count, int read_timeout, int write_timeout, Master *master);
    ~Worker();

    int32_t Init();
    int32_t Start();
    void *Entry();

    int GetId();
    int GetNotifiedWFd();
    void RecvNotifiedCb(int fd, short event, void *arg);

    string GetDataProtocol();
    DispatchQueue* GetDispatchQueue();

private:
    Logger *logger_;
    int id_;

    struct event_base *base_;

    int notified_wfd_;
    int notified_rfd_;
    struct event *notified_event_;

    uint32_t worker_conn_count_;
    uint32_t current_conn_count_;

    int read_timeout_;
    int write_timeout_;

    Master master_;

    Mutex conn_info_queue_mutex_;
    deque<ConnectionInfo*> conn_info_queue_;
};

}

#endif

