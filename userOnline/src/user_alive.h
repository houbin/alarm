#ifndef DEVICE_ALIVE_H_
#define DEVICE_ALIVE_H_

#include "push_msg_queue.h"
//#include "http_client.h"

extern PushMsgQueue *g_msg_push_queue;
//extern HttpClient *g_http_client;

extern void ClearGuidFdCache();
//extern int StartHttpClient();

#endif
