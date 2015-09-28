#ifndef TCP_SERVER_CONNECTION_H_
#define TCP_SERVER_CONNECTION_H_

#include <event.h>
#include <string>
#include "tlv_define.h"

class Worker;
struct ConnectionInfo
{
    uint64_t conn_id;
    int cfd;
    
    std::string cip;
    uint16_t cport;

    struct bufferevent *buffer_event;
    char in_buffer[CONN_BUFFER_LEN];
    uint16_t in_buffer_len;
    //char out_buffer[CONN_BUFFER_LEN];
    //uint16_t out_buffer_len;
    Worker *worker;

    int guid_type; // client or dev
    string guid; // client or dev guid
};

#endif

