#include "connection.h"

using namespace util;

namespace tcpserver
{

void TlvMsg::Encode(char *buffer)
{
    char *temp = buffer;
    EncodeFixed32(temp, kMsgMagicCode);
    temp += 4;
    EncodeFixed32(temp, tag);
    temp += 4;
    EncodeFixed32(temp, length);
    temp += 4;
}

void TlvMsg::Decode(char *buffer)
{
    char *temp = buffer;

    magic_code = DecodeFixed32(temp);
    temp += 4;
    tag = DecodeFixed32(temp);
    temp += 4; length = DecodeFixed32(temp);
    temp += 4;
}

}

