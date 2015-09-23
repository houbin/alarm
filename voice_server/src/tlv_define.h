#ifndef TLV_DEFINE_H_
#define TLV_DEFINE_H_

enum
{
    TYPE_LENGTH             = 0,
    TYPE_MID                = 1,
    TYPE_CLIENT_ID          = 2,
    TYPE_DEV_ID             = 3,
    
    TYPE_CLIENT_BUILD_VC            = 1000,
    TYPE_ON_CLIENT_BUILD_VC         = 1001,
    TYPE_CLIENT_START_SEND          = 1002,
    TYPE_ON_CLIENT_START_SEND       = 1003,
    TYPE_CLIENT_SEND_VOICE_DATA     = 1004,
    TYPE_ON_CLIENT_SEND_VOICE_DATA  = 1005,
    TYPE_CLIENT_FREE_VC             = 1006,
    TYPE_ON_CLIENT_FREE_VC          = 1007,
    
    TYPE_DEV_BUILD_VC               = 2000,
    TYPE_ON_DEV_BUILD_VC            = 2001,
 // TYPE_DEV_SEND_VOICE_DATA        = 2002,
 // TYPE_ON_DEV_SEND_VOICE_DATA     = 2003,
    TYPE_DEV_FREE_VC                = 2004,
    TYPE_ON_DEV_FREE_VC             = 2005,
    TYPE_MAX_NUMBER
};

#endif
