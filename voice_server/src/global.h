#ifndef TCPSERVER_GLOBAL_H_
#define TCPSERVER_GLOBAL_H_

#include "../util/logger.h"
#include "../util/config.h"

extern util::Logger *g_logger;
extern util::Config *g_config;


#define DATA_PROTOCOL_JSON "json"
#define DATA_PROTOCOL_TLV "tlv"

#endif
