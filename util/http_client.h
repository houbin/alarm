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

extern int g_http_client_mid;

class SendMsg
{
public:
    string url_;
    string msg_info_;
    Context *ct_;

    SendMsg(string url, string msg_info, Context *ct = NULL) : url_(url), msg_info_(msg_info) {}
    ~SendMsg() {}
};

class HttpClient : public Thread
{
public:
    HttpClient();
    ~HttpClient();

    int SubmitMsg(SendMsg msg);

    int Init();
    int Start();
    void *Entry();
    int Stop();

    int ParseUrl(string url, string &scheme, string &host, int &port, string &uri);
    int MakeHttpReq(SendMsg send_msg, struct evhttp_request **req);

private:
    // 需要发送的消息，如上下线消息
    Mutex mutex_;
    Cond cond_;
    list<SendMsg> dispatch_queue_;

    bool stop_;

    struct event_base *base_;
};

#endif

