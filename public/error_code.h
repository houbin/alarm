#ifndef PUBLIC_ERROR_CODE_H_
#define PUBLIC_ERROR_CODE_H_


// ���ؾ��������
#define ERROR_PARSE_MESSAGE_TYPE        10000
#define ERROR_BAD_MESSAGE_TYPE          10001
#define ERROR_PARSE_LOAD_REPORT         10002
#define ERROR_REFRESH_LOAD_ITEM         10003
#define ERROR_PARSE_METHOD              10004
#define ERROR_BAD_METHOD                10005
#define ERROR_PARSE_CLIENT_TYPE         10006
#define ERROR_CANNOT_FIND_VALID_SERVER  10007

// �豸������ʼ������
#define ERROR_PARSE_BEACON              11000
#define ERROR_SET_DEVICE_FD_CACHE       11001
#define ERROR_METHOD_INVALID            11002
#define ERROR_GET_DEVICE_FD_FROM_CACHE  11003
#define ERROR_FD_INVALID                11005
#define ERROR_PUSH_MESSAGE              11006
#define ERROR_HTTP_PARSE_URL            11007
#define ERROR_HTTP_GET_SCHEME           11008
#define ERROR_HTTP_GET_HOST             11009
#define ERROR_HTTP_GET_PORT             11010
#define ERROR_HTTP_NOT_SUPPORT_HTTPS    11011
#define ERROR_HTTP_NEW_BASE             11012
#define ERROR_HTTP_NEW_BUFFEREVENT      11012
#define ERROR_HTTP_NEW_EVHTTP_CONNECTION 11013
#define ERROR_HTTP_NEW_REQUEST          11014
#define ERROR_HTTP_MAKE_REQUEST         11015

#define ERROR_DEVICE_LOGIN              11016
#define ERROR_DEVICE_CONNECT            11017
#define ERROR_DEVICE_DOUBLE_LOGIN       11018
#define ERROR_DEVICE_NOT_LOGIN          11019
#define ERROR_DEVICE_ID_NOT_THE_SAME    11020
#define ERROR_AUTH_DATA_SIZE            11021
#define ERROR_SET_DEVICE_ADDR_CACHE     11022
#define ERROR_GET_DEVICE_ADDR_FROM_CACHE 11023
#define ERROR_GET_SOCKETNAME            11024

// pc client ���ߴ�����
#define ERROR_USER_CONNECT              12000
#define ERROR_SET_GUID_FD_CACHE         12001
#define ERROR_GET_USER_FD_FROM_CACHE    12002
#define ERROR_SESSION_ID_NOT_EXIST      12003
#define ERROR_PARSE_MSG                 12004

// voice server ������
#define ERROR_QUEUE_ITEM_PRIORITY_INVALID 13000
#define ERROR_PARSE_GET_VOICESERVER_ADDR 13001
#define ERROR_NO_CLIENT_OR_DEV_IP       13002
#define ERROR_NO_PARAM                  13003
#define ERROR_TLV_DATA_SIZE_TOO_LARGE   13004
#define ERROR_TLV_PARSE_MID             13005
#define ERROR_TLV_PARSE_CMD             13006
#define ERROR_TLV_PARSE_DEV_ID          13007
#define ERROR_TLV_PARSE_CLIENT_ID       13008
#define ERROR_TLV_SEND_MSG              13009
#define ERROR_TLV_LEN_NOT_0             13010


#endif

