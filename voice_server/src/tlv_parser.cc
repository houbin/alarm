#include "connection.h"
#include "tlv_parser.h"
#include "../../public/error_code.h"

TlvItem tlv_item_list[] =
{
    { TYPE_MID,         parse_TYPE_MID_func, post_parse_TYPE_MID_func },
    //{ TYPE_CLIENT_ID,   parse_TYPE_CLIENT_ID_func },
    //{ TYPE_DEV_ID,      parse_TYPE_DEV_ID_func },

    { TYPE_CLIENT_BUILD_VC,         parse_TYPE_CLIENT_BUILD_VC_func, post_parse_TYPE_CLIENT_BUILD_VC_func },
    { TYPE_CLIENT_START_SEND,       parse_TYPE_CLIENT_START_SEND_func, post_parse_TYPE_CLIENT_START_SEND_func },
    { TYPE_CLIENT_SEND_VOICE_DATA,  parse_TYPE_CLIENT_SEND_VOICE_DATA_func, post_parse_TYPE_CLIENT_SEND_VOICE_DATA_func },
    { TYPE_CLIENT_FREE_VC,          parse_TYPE_CLIENT_FREE_VC_func, post_parse_TYPE_CLIENT_FREE_VC_func },

    { TYPE_DEV_BUILD_VC,    parse_TYPE_DEV_BUILD_VC_func, post_parse_TYPE_DEV_BUILD_VC_func },
    { TYPE_DEV_FREE_VC,     parse_TYPE_DEV_FREE_VC_func, post_parse_TYPE_DEV_FREE_VC_func }
};

int32_t TlvParser::ParsedData(Slice &s, ParsedData &parsed_data)
{
    const char *data = s.data();
    size_t size = s.size();

    if (size >= CONN_BUFFER_LEN)
    {
        LOG_ERROR(g_logger, "tlv param invalid, data ptr is %p, size is %d", data, size);
        return -ERROR_INVALID_PARAM;
    }

    int count = sizeof(tlv_item_list) / sizeof(struct TlvItem);

    uint32_t type;
    uint32_t len;
    bool found = false;
    char *parsing_ptr = data;
    while (parsing_ptr < (data + size) && (parsing_ptr + kMsgHeaderSize) < (data + size))
    {
        type = DecodeFixed32(parsing_ptr);
        len = DecodeFixed32(parsing_ptr + 4);

        int i = 0;
        for (; i < count; i++)
        {
            TlvItem item = tlv_item_list[i];
            if (item.type != type)
                continue;

            found = true;
            fParseTlvItemFunc func = item.func;
            assert(func != NULL);
            int ret = func(parsing_ptr + kMsgHeaderSize, len, parsed_data);
            if (ret != 0)
            {
                return ret;
            }

            fPostParseFunc post_func = item.func;
            if (post_func)
            {
                // 检查解析参数是否合法
                ret = post_func(parsed_data);
                if (ret != 0)
                {
                    return ret;
                }
            }
        }

        if (!found)
        {
            LOG_ERROR(g_logger, "type not found, and type is %d", type);
        }

        return -ERROR_
    }

    int temp_len = 0;
    while (temp_len < size && (temp_len + kMsgHeaderSize) < )
}

