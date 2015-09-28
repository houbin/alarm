#ifndef TCP_SERVER_WORKER_H_
#define TCP_SERVER_WORKER_H_

#include <deque>
#include "../util/thread.h"
#include "global.h"

using namespace util;
using namespace std;

enum
{
    D_TIMEOUT,
    D_CLOSE,
    D_ERROR
};

struct ConnectionInfo;
class Master;
class Dispatcher;

class Worker : public Thread
{
public:
    Worker(int i, uint32_t worker_conn_count, int read_timeout, int write_timeout, Master *master);
    ~Worker();

    int32_t Init();
    int32_t Start();
    void *Entry();
    void Wait();

    void Shutdown();

    int GetId();
    int GetNotifiedWFd();
    void RecvNotifiedCb(int fd, short event, void *arg);

    int32_t PutConnInfo(ConnectionInfo *conn_info);
    string GetDataProtocol();
    Dispatcher* GetDispatcher();

private:
    int id_;

    struct event_base *base_;

    int notified_wfd_;
    int notified_rfd_;
    struct event *notified_event_;

    uint32_t worker_conn_count_;
    uint32_t current_conn_count_;

    int read_timeout_;
    int write_timeout_;

    Master *master_;

    Mutex conn_info_queue_mutex_;
    deque<ConnectionInfo*> conn_info_queue_;
};

#endif

