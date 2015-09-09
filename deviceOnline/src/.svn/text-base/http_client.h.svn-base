#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <string>
#include <list>
#include <evhttp.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/bufferevent.h>
#include "cond.h"
#include "mutex.h"
#include "thread.h"
#include "context.h"
#include "defines.h"

using namespace std;
using namespace util;

class SendMsg
{
public:
    string msg_info_;
    Context *ct_;

    SendMsg(string msg_info, Context *ct = NULL) : msg_info_(msg_info), ct_(ct)
    {}

    ~SendMsg(){}
};

class HttpClient : public Thread
{
public:
    HttpClient(string url);
    ~HttpClient();

    int SubmitMsg(SendMsg msg);

    int Init();
    int Start();
    void *Entry();
    int Stop();

    int MakeHttpReq(SendMsg send_msg, struct evhttp_request **req);
    //int GetEventBase(struct event_base **base);

private:
    // 需要发送的消息，如上下线消息
    Mutex mutex_;
    Cond cond_;
    list<SendMsg> dispatch_queue_;

    bool stop_;

    string url_;
    const char *host_;
    int port_;
    char uri_[256];

    struct event_base *base_;
//    struct evhttp_connection *conn_;

/*
public:
    struct DispatchThread : public Thread
    {
        HttpClient *http_client_;
        DispatchThread(HttpClient *http_client) : http_client_(http_client) { }

        void *Entry()
        {
            int ret;
            LOG4CXX_ERROR(g_logger, "enter dispatch thread entry");

            struct event_base *base = NULL;
            ret = http_client_->GetEventBase(&base);
            assert (ret == 0);

            LOG4CXX_ERROR(g_logger, "start dispatch thread entry");
            event_base_dispatch(base);
            LOG4CXX_ERROR(g_logger, "end dispatch thread entry");

            return NULL;
        }

        int Stop()
        {
            int ret;
            struct event_base *base = NULL;
            ret = http_client_->GetEventBase(&base);
            assert (ret == 0);

            event_base_loopbreak(base);

            Join();

            return 0;
        }
    }dispatch_thread;
*/
};

#endif

