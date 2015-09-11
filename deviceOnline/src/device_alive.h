#ifndef DEVICE_ALIVE_H_
#define DEVICE_ALIVE_H_

#include "message_reply_queue.h"
#include "http_client.h"

extern MsgReplyQueue *g_msg_reply_queue;
extern HttpClient *g_http_client;

extern void ClearDeviceCache();
extern int StartHttpClient();

#endif
