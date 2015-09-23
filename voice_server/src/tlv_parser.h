#ifndef TLV_PARSER_H_
#define TLV_PARSER_H_

#include <stdint.h>
#include <string>
#include "global.h"
#include "tlv_define.h"

using namespace std;

struct ParsedData
{
    uint32_t mid;
    bool have_mid;

    uint32_t method_id;
    bool have_method_id;

    union
    {
        struct VcInfo
        {
            string client_id;
            bool have_client_id;

            string dev_id;
            bool have_dev_id;
        }S_vc_info;

        struct VoiceData
        {
            uint32_t len;
            bool have_len;

            char *voice_data;
            bool have_voice_data;
        }S_voice_data;
    };
}

typedef int32_t (*fParseTlvItemFunc)(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
typedef int32_t (*fPostParseFunc)(ParsedData &parsed_data);

// parse tlv
int32_t parse_TYPE_MID_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
//int32_t parse_TYPE_CLIENT_ID_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
//int32_t parse_TYPE_DEV_ID_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
int32_t parse_TYPE_CLIENT_BUILD_VC_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
int32_t parse_TYPE_CLIENT_START_SEND_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
int32_t parse_TYPE_CLIENT_SEND_VOICE_DATA_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
int32_t parse_TYPE_CLIENT_FREE_VC_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
int32_t parse_TYPE_DEV_BUILD_VC_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);
int32_t parse_TYPE_DEV_FREE_VC_func(cont char *tlv_data, uint32_t size, ParsedData &parsed_data);

// post func after parsed tlv
int32_t post_parse_TYPE_MID_func(ParsedData &parsed_data);
//int32_t post_parse_TYPE_CLIENT_ID_func(ParsedData &parsed_data);
//int32_t post_parse_TYPE_DEV_ID_func(ParsedData &parsed_data);
int32_t post_parse_TYPE_CLIENT_BUILD_VC_func(ParsedData &parsed_data);
int32_t post_parse_TYPE_CLIENT_START_SEND_func(ParsedData &parsed_data);
int32_t post_parse_TYPE_CLIENT_SEND_VOICE_DATA_func(ParsedData &parsed_data);
int32_t post_parse_TYPE_CLIENT_FREE_VC_func(ParsedData &parsed_data);
int32_t post_parse_TYPE_DEV_BUILD_VC_func(ParsedData &parsed_data);
int32_t post_parse_TYPE_DEV_FREE_VC_func(ParsedData &parsed_data);

struct TlvItem
{
    uint32_t type;
    fParseTlvItemFunc func;
    fPostParseFunc post_func;
};

class TlvParser
{
private:

public:
    TlvParser() {}
    ~TlvParser() {}

    int32_t ParseMsg(Slice &s, ParsedData &parsed_data);
};

#endif
