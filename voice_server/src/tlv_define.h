#ifndef TLV_DEFINE_H_
#define TLV_DEFINE_H_

enum
{
    TYPE_LENGTH             = 0,
    TYPE_MID                = 1,
    TYPE_CLIENT_ID          = 2,
    TYPE_DEV_ID             = 3,
    TYPE_HEARTBEAT          = 4,
    TYPE_ON_HEARTBEAT       = 5,
    
    TYPE_CLIENT_BUILD_VC            = 1000,
    TYPE_ON_CLIENT_BUILD_VC         = 1001,
    TYPE_CLIENT_START_SEND          = 1002,
    TYPE_ON_CLIENT_START_SEND       = 1003,
    TYPE_CLIENT_SEND_VOICE_DATA     = 1004,
    TYPE_ON_CLIENT_SEND_VOICE_DATA  = 1005,
    TYPE_CLIENT_FREE_VC             = 1006,
    TYPE_ON_CLIENT_FREE_VC          = 1007,
    TYPE_PUSH_CLIENT_START_SEND     = 1008,
    TYPE_ON_PUSH_CLIENT_START_SEND  = 1009,
    TYPE_PUSH_CLIENT_VOICE_DATA     = 1010,
    TYPE_ON_PUSH_CLIENT_VOICE_DATA  = 1012,
    
    TYPE_DEV_BUILD_VC               = 2000,
    TYPE_ON_DEV_BUILD_VC            = 2001,
 // TYPE_DEV_SEND_VOICE_DATA        = 2002,
 // TYPE_ON_DEV_SEND_VOICE_DATA     = 2003,
    TYPE_DEV_FREE_VC                = 2004,
    TYPE_ON_DEV_FREE_VC             = 2005,
    TYPE_MAX_NUMBER
};

enum MsgType
{
    MSG_TYPE_PRINT = 0,
    MSG_TYPE_ECHO = 1,
    MSG_TYPE_PINGPONG = 2
};

#define CONN_BUFFER_LEN (10 * 1024)
#define MAX_MSG_LENGTH 2048
//static const uint32_t kMsgMagicCode = 0x49564f4a;
#define kMsgHeaderSize (4+4)

#define MAX_GUID_LEN 256

enum
{
    GUID_CLIENT = 1,
    GUID_DEV = 2,
};

#endif
