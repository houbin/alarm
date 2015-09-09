#include "http_client.h"
#include "defines.h"
#include "../../public/error_code.h"

void http_request_done(struct evhttp_request *req, void *arg)
{
    LOG4CXX_INFO(g_logger, "receive http response");

    if (req == NULL)
    {
        LOG4CXX_ERROR(g_logger, "receive http response, req = NULL");
        return;
    }

    struct evhttp_connection *conn = (struct evhttp_connection*)arg;

    LOG4CXX_ERROR(g_logger, "Response line: " << evhttp_request_get_response_code(req) << ", " << evhttp_request_get_response_code_line(req));

    char buffer[512] = {0};
    int n = 0;
    while ((n = evbuffer_remove(evhttp_request_get_input_buffer(req), buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[n] = 0;
        LOG4CXX_INFO(g_logger, "receive http response, content is " << buffer);
        // TODO: handle this msg
    }

    evhttp_connection_free(conn);

    return;
}

HttpClient::HttpClient(string url) : mutex_("HttpClient"), stop_(false), url_(url), host_(NULL), port_(-1), base_(NULL)
{ 
    memset(uri_, 0, 256);
}

HttpClient::~HttpClient()
{
    event_base_free(base_);
    base_ = NULL;
}

int HttpClient::SubmitMsg(SendMsg send_msg)
{
    Mutex::Locker lock(mutex_);
    if (stop_)
        return 0;

    dispatch_queue_.push_back(send_msg);
    cond_.Signal();

    return 0;
}

int HttpClient::Init()
{
    const char *scheme = NULL;
    const char *path = NULL;
    const char *query = NULL;
    struct evhttp_uri *http_uri = NULL;

    http_uri = evhttp_uri_parse(url_.c_str());
    if (http_uri == NULL)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_uri_parse error, url is " << url_);
        return -ERROR_HTTP_PARSE_URL;
    }

    scheme = evhttp_uri_get_scheme(http_uri);
    if (scheme == NULL)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_uri_get_scheme error, url is " << url_);
        return -ERROR_HTTP_GET_SCHEME;
    }

    if (strcasecmp(scheme, "https") == 0)
    {
        LOG4CXX_ERROR(g_logger, "scheme is https, not support");
        return -ERROR_HTTP_NOT_SUPPORT_HTTPS;
    }

    host_ = evhttp_uri_get_host(http_uri);
    if (host_ == NULL)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_uri_get_host error, url is " << url_);
        return -ERROR_HTTP_GET_HOST;
    }

    port_ = evhttp_uri_get_port(http_uri);
    if (port_ == -1)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_uri_get_port error, url is " << url_);
        return -ERROR_HTTP_GET_PORT;
    }

    path = evhttp_uri_get_path(http_uri);
    if (path == NULL)
    {
        path = "/";
    }

    query = evhttp_uri_get_query(http_uri);
    if (query == NULL)
    {
        snprintf(uri_, sizeof(uri_) - 1, "%s", path);
    }
    else
    {
        snprintf(uri_, sizeof(uri_) - 1, "%s?%s", path, query);
    }
    uri_[sizeof(uri_) - 1] = '\0';

    base_ = event_base_new();
    if (base_ == NULL)
    {
        LOG4CXX_ERROR(g_logger, "event_base_new error");
        return -ERROR_HTTP_NEW_BASE;
    }

    return 0;
}

int HttpClient::Start()
{
    Create();

    return 0;
}

int HttpClient::MakeHttpReq(SendMsg send_msg, struct evhttp_request **req)
{
    struct evhttp_connection *conn = NULL;
    struct bufferevent *bev = NULL;
    
    bev = bufferevent_socket_new(base_, -1, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        LOG4CXX_ERROR(g_logger, "bufferevent_socket_new error");
        return -ERROR_HTTP_NEW_BUFFEREVENT;
    }

    conn = evhttp_connection_base_bufferevent_new(base_, NULL, bev, host_, port_);
    if (conn == NULL)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_connection_base_bufferevent_new error");
        return -ERROR_HTTP_NEW_EVHTTP_CONNECTION;
    }

    *req = evhttp_request_new(http_request_done, conn);
    if (*req == NULL)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_request_new error");
        return -ERROR_HTTP_NEW_REQUEST;
    }

    struct evkeyvalq *output_headers = NULL;
    output_headers = evhttp_request_get_output_headers(*req);
    evhttp_add_header(output_headers, "Host", host_);
    evhttp_add_header(output_headers, "Content-Type", "application/json");
    evhttp_add_header(output_headers, "Connection", "close");

    struct evbuffer *output_buffer = NULL;
    output_buffer = evhttp_request_get_output_buffer(*req);
    evbuffer_add(output_buffer, send_msg.msg_info_.c_str(), send_msg.msg_info_.size());

    char msg_len[8] = {0};
    snprintf(msg_len, 7, "%zu", send_msg.msg_info_.size());
    evhttp_add_header(output_headers, "Content-Length", msg_len);

    int ret = evhttp_make_request(conn, *req, EVHTTP_REQ_POST, uri_);
    if (ret != 0)
    {
        LOG4CXX_ERROR(g_logger, "evhttp_make_request error");
        return -ERROR_HTTP_MAKE_REQUEST;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 200000;
    evhttp_connection_set_timeout_tv((*req)->evcon, &tv);

    LOG4CXX_ERROR(g_logger, "dispatch this event start");
    event_base_dispatch(base_);
    LOG4CXX_ERROR(g_logger, "dispatch this event ok");

    return 0;
}

void *HttpClient::Entry()
{
    mutex_.Lock();
    while (!stop_)
    {
        while (!dispatch_queue_.empty())
        {
            SendMsg send_msg = dispatch_queue_.front();
            dispatch_queue_.pop_front();

            LOG4CXX_TRACE(g_logger, "pop http SendMsg, msg is " << send_msg.msg_info_);

            mutex_.Unlock();

            int ret = 0;
            struct evhttp_request *req = NULL;
            ret = MakeHttpReq(send_msg, &req);
            if (ret != 0)
            {
                LOG4CXX_ERROR(g_logger, "new http request error, ret " << ret);
                goto entry_continue;
            }

            LOG4CXX_TRACE(g_logger, "make http req of SendMsg, and send request");

        entry_continue:
            mutex_.Lock();
            continue;
        }

        if (stop_)
        {
            LOG4CXX_ERROR(g_logger, "http client thread stopped");
            break;
        }

        cond_.Wait(mutex_);
    }

    mutex_.Unlock();
    LOG4CXX_ERROR(g_logger, "http client thread exit");

    return NULL;
}

int HttpClient::Stop()
{
    LOG4CXX_ERROR(g_logger, "stop http client thread start");
    mutex_.Lock();
    stop_ = true;
    cond_.Signal();
    mutex_.Unlock();

    Join();
    LOG4CXX_ERROR(g_logger, "stop http client thread ok ");

    return 0;
}

