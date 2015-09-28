#ifndef TCP_SERVER_MASTER_H_
#define TCP_SERVER_MASTER_H_

#include <stdint.h>
#include <vector>
#include <event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../util/thread.h"
#include "../util/mutex.h"
#include "../util/cond.h"
#include "global.h"
#include "dispatcher.h"

using namespace util;
using namespace std;

class Worker;

#define DATA_PROTOCOL_JSON "json"
#define DATA_PROTOCOL_TLV "tlv"

struct MasterOption
{
    string data_protocol_; // 数据传输的协议
    int listen_port_; // master侦听的端口
    uint16_t worker_count_; // master挂的worker数量
    uint32_t worker_conn_count_; // 每个worker连接的数目,暂时没有使用
    int read_timeout_; // 连接的读数据超时
    int write_timeout_; // 连接的写数据超时
};

class Master : public Thread
{
public:
    Master(string name, MasterOption &master_option);
    ~Master();

    int32_t Init();
    int32_t Start();
    void *Entry();
    void Wait();

    void Shutdown();

    int32_t OpenServerSocket();
    void AcceptCb(int fd, short event, void *arg);
    int AcceptClient(struct sockaddr_in *client_addr);
    int32_t PickOneWorker(Worker **worker);

    string GetDataProtocol();
    Dispatcher* GetDispatcher();

private:
    string name_;

    int listen_fd_;

    struct event_base *main_base_;
    struct event *listen_event_;

    vector<Worker*> workers_;
    
    MasterOption master_option_;

    Dispatcher *dispatcher_;

    bool stop_;

    uint64_t conn_count_; //记录连接次数,包含已经断开的
};

#endif 

