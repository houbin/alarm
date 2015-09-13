#ifndef DEVICE_ALIVE_H_
#define DEVICE_ALIVE_H_

#include "push_msg_queue.h"
#include "http_client.h"

extern PushMsgQueue *g_push_msg_queue;
extern HttpClient *g_http_client;

extern void ClearDeviceCache();
extern int StartHttpClient();

#endif
