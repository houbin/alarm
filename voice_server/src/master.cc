#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>
#include "../util/common.h"
#include "global.h"
#include "connection.h"
#include "master.h"

using namespace util;

namespace tcpserver
{

void AcceptCb(int fd, short event, void *arg)
{
    assert(arg != NULL);
    Master *master = (Master*)arg;
    master->AcceptCb(fd, event, (void *)master);

    return;
}

Master::Master(Logger *logger, string name, Dispatcher *dispatcher, MasterOption &master_option)
: logger_(logger), name_(name), master_option_(master_option)
{
    dispatch_queue_ = new DispatchQueue(logger, dispatcher);
}

Master::~Master()
{
    safe_close(listen_fd_);
}

int32_t Master::Init()
{
    int32_t ret = 0;

    main_base_ = event_base_new();
    if (main_base_ == NULL)
    {
        LOG_ERROR(logger_, "event_base_new error");
        return -1;
    }

    int i = 0;
    for (i; i < master_option.worker_count_; i++)
    {
        Worker *worker = NULL;

        worker = new Worker(logger_, i, master_option.worker_conn_count_, 
                        master_option.read_timeout_, master_option.write_timeout_, this);
        if (worker == NULL)
        {
            LOG_ERROR(logger_, "new Worker %d error", i);
            return -1;
        }

        ret = worker->Init();
        if (ret != 0)
        {
            LOG_ERROR(logger_, "Worker %d init error, ret %d", i, ret);
            return ret;
        }

        ret = worker->Start();
        if (ret != 0)
        {
            LOG_ERROR(logger_, "worker %d start error, ret %d", i, ret);
            return ret;
        }

        workers_.push_back(worker);
    }

    ret = OpenServerSocket();
    if (ret != 0)
    {
        LOG_ERROR(logger_, "open server socket error, ret %d", ret);
        return ret;
    }

    listen_event_ = event_new(main_base_, listen_fd_, EV_READ|EV_PERSIST, tcpserver::AcceptCb, (void*)this);
    if (listen_event_ == NULL)
    {
        LOG_ERROR(logger_, "cannot new event");
        return -1;
    }

    ret = event_add(listen_event_, NULL);
    if (ret != 0)
    {
        LOG_ERROR(logger_, "event_add error");
        return -1;
    }

    return 0;
}

int32_t Master::Start()
{
    Create();

    return 0;
}

void *Master::Entry()
{
    event_base_dispatch(main_base_);

    LOG_FATAL(logger_, "event_base_dispatch return");

    return NULL;
}

void Master::Wait()
{
    Join();

    return;
}

int32_t Master::OpenServerSocket()
{
    int ret;
    struct sockaddr_in listen_addr;

    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0)
    {
        LOG_ERROR(logger_, "create socket error, ret %d, msg %s", -errno, strerror(errno));
        return -errno;
    }

    // set non-blocking
    bool bRet = set_socket_noblock(listen_fd_);
    if (!bRet)
    {
        LOG_ERROR(logger_, "cannot set nonblocking, listen fd %d", listen_fd_);
        safe_close(listen_fd_);
        return -1;
    }

    // set SO_REUSEADDR
    int flag_reuseaddr = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &flag_reuseaddr, sizeof(flag_reuseaddr));

    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = ntohs(master_option.listen_port_);
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(listen_fd_, (struct sockaddr*)&listen_addr, sizeof(listen_addr));
    if (ret != 0)
    {
        LOG_ERROR(logger_, "bind error, ret %d, msg %s", -errno, strerror(errno));
        safe_close(listen_fd_);
        return -errno;
    }

    ret = listen(listen_fd_, 1024);
    if (ret != 0)
    {
        LOG_ERROR(logger_, "listen error, ret %d, msg %s", -errno, strerror(errno));
        safe_close(listen_fd_);
        return -errno;
    }

    return 0;
}

void Master::AcceptCb(int fd, short event, void *arg)
{
    int cfd;
    int32_t ret = 0;
    struct sockaddr_in client_addr;

    LOG_DEBUG(logger_, "master accept event, fd %d, event %d", fd, event);

    cfd = AcceptClient(&client_addr);
    if (cfd < 0)
    {
        LOG_ERROR(logger, "accept client error");
        return;
    }

    Worker *worker = NULL;
    ret = PickOneWorker(&worker);
    if (ret != 0)
    {
        LOG_ERROR(logger, "no worker available");
        return;
    }

    ++conn_count_;
    ConnectionInfo *conn_info = new ConnectionInfo;
    conn_info->conn_id = master_option.conn_count_;
    conn_info->cfd = cfd;
    conn_info->cip = (uint32_t)client_addr.sin_addr.s_addr;
    conn_info->cport = ntohs(client_addr.sin_port);
    conn_info->Worker = worker;

    LOG_DEBUG(logger_, "accept client, conn_id %" PRIu64 ", cfd %d, ip %s, port %d", conn_info->conn_id, cfd,
                        inet_ntoa(client_addr.sin_addr), conn_info->cport);

    // notify worker
    worker->PutConnInfo(conn_info);
    int notified_wfd = worker->GetNotifiedWFd();
    write(notified_wfd, "c", 1);

    LOG_DEBUG(logger_, "conn id %" PRIu64 ", put to worker %d", conn_info->conn_id, worker->GetId());

    return;
}

int Master::AcceptClient(struct sockaddr_in *client_addr)
{
    int ret = 0;
    int cfd = -1;

    do
    {
        socklen_t len = sizeof(struct sockaddr_in);
        cfd = accept(listen_fd_, (struct sockaddr*)client_addr, &len);
        if (cfd < 0)
        {
            if (errno == EINTR)
                continue;

            LOG_ERROR(logger_, "accept error, ret %d, msg %s", -errno, strerror(errno));
            break;
        }

        bool bRet = set_socket_noblock(cfd);
        if (!bRet)
        {
            LOG_ERROR(logger_, "cannot set nonblocking, cfd %d", cfd);
            safe_close(cfd);
            cfd = -1;
        }
        return cfd;
    }while(false);

    return -1;
}

int32_t Master::PickOneWorker(Worker **worker)
{
    assert(worker != NULL);

    *worker = workers_[conn_count_ % worker_count_];

    return 0;
}

string Master::GetDataProtocol()
{
    return master_option_.data_protocol_;
}

DispatchQueue *Master::GetDispatchQueue()
{
    return dispatch_queue_;
}

}

