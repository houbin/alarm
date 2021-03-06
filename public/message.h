#ifndef MESSAGE_H
#define MESSAGE_H

#define TOKEN_LENGTH            5
#define TOKEN_STR                "@#1$6"

/* JK(JSON KEY) */
#define PROTO_VERSION            "1.0"
#define JK_MESSAGE_ID            "mid"
#define JK_CLIENT_TYPE          "ct"
#define JK_PROTO_VERSION        "pv"
#define JK_PARAM                "param"
#define JK_ID                   "id"
#define JK_LOGIC_PROCESS_TYPE    "lpt"
#define JK_RESULT                "rt"
#define JK_CLINET_SFD            "cfd"
#define JK_CLINET_SFD_ID        "cfdid"
#define JK_SOFT_VERSION            "softver"
#define JK_SOFT_VERSION_ADDR    "softveraddr"
#define JK_SOFT_VERSION_DESP    "softverdesp"

#define JK_SESSION_ID            "sid"

#define JK_CUSTOM_TYPE            "custom_type"
#define JK_USERNAME                "username"
#define JK_NEW_USERNAME            "newusername"
#define JK_PASSWORD                "password"
#define JK_NEW_PASSWORD            "newpass"
#define JK_PEER_USERNAME        "pusername"
#define JK_MAIL_OR_PHONE        "mop"
#define JK_MAIL                    "mail"
#define JK_PHONE                "phone"
#define JK_NICKNAME                "nkname"
#define JK_USER_OTHER_INFO      "uoi"
#define JK_SECURITY_MAIL        "sm"
#define JK_CLIENT_LOGIN_INFO    "cli"
#define JK_CLIENT_LOGIN_PLATFORM "clp"
#define JK_LOGIN_MOBILE_ID        "lmi"
#define JK_FEEDBACK                "fb"
#define JK_LANGUAGE_TYPE        "langt"
#define JK_TERMINAL_TYPE        "tt"

#define JK_USER_ONLINE_STATUS   "uls"

#define JK_IM_SERVER_NO         "isn"
#define JK_RELAY_MESSAGE        "rm"
#define JK_RELAY_MESSAGE_GUID   "rmg"
#define JK_RELAY_MESSAGE_TIMESTAMP "rmt"

#define JK_CUSTOM_STRING        "cs"

/*从IM服务器发送到转发服务器的消息类型，是请求中转的消息 还是  中转消息的发送结果（用于确认是否删除消息队列中的已存储消息）*/
#define JK_P2RELAY_MESSAGE_TYPE  "p2rmt"

#define JK_ONLINE_SERVER_NO        "osn"
#define JK_ONLINE_SERVER_FD        "osf"
#define JK_ONLINE_SERVER_FD_ID    "osfi"

#define JK_CREATE_TIME            "ct"


#define JK_PUSH_MESSAGE_TYPE    "pmt"

#define JK_OPTION                "option"
#define JK_CUSTOM_STRING        "cs"

#define JK_ACCOUNT_LIVE_INFO     "ali"
#define JK_ALARM_FLAG            "af"

#define JK_CLIENT_VERSION        "cv"
#define JK_UPDATE_FILE_INFO        "ufi"
#define JK_FILE_VERSION            "fv"
#define JK_FILE_URL                "fu"
#define JK_FILE_SIZE            "fs"
#define JK_FILE_DESCRIPTION        "fd"
#define JK_FILE_CHECKSUM        "fc"

/* 自动更新 */
#define JK_APP_CURRENT_VERSION    "crtver"
#define JK_APP_VERSION            "appver"
#define JK_APP_VERSION_FULL        "appfullver"
#define JK_APP_VERSION_URL        "appverurl"
#define JK_APP_VERSION_DESC        "appverdesc"
#define JK_APP_CLIENT_TYPE      "appclit"
/* 设备服务相关 */
#define JK_DEVICES_CHANGE       "dc"
#define JK_DEVICE_INFO            "dinfo"
#define JK_DEVICE_LIST            "dlist"
#define JK_SERVER_TYPE          "srvtype"
#define JK_DEVICE_GUID            "dguid"
#define JK_DEVICE_TYPE            "dtype"
#define JK_DEVICE_SUB_TYPE        "dstype"
#define JK_DEVICE_SUB_TYPE_INT    "dstypeint"
#define JK_DEVICE_USERNAME        "dusername"
#define JK_DEVICE_PASSWORD        "dpassword"
#define JK_DEVICE_NAME            "dname"
#define JK_DEVICE_IP            "dip"
#define JK_DEVICE_PORT            "dport"
#define JK_DEVICE_NET_STATE        "dnst"
#define JK_NET_STORAGE_SWITCH    "netss"
#define JK_TF_STORAGE_SWITCH    "tfss"
#define JK_ALARM_SWITCH            "aswitch"
#define JK_ALARM_VIDEO_FTP        "avftp"
#define JK_ALARM_SNAP_FTP        "asnapftp"
#define JK_ALARM_FTP_ACC        "aftpacc"
#define JK_ALARM_FTP_PWD        "aftppwd"
#define JK_ALARM_TIME            "atime"
#define JK_PIC_FTP_BIG            "dpicb"
#define JK_PIC_FTP_SMALL        "dpics"
#define JK_PIC_FTP_ACC            "dpicacc"
#define JK_PIC_FTP_PWD            "dpicpwd"
#define JK_PIC_UPLOAD_TIMEING    "dpicut"
#define JK_VIDEO_FLUENCY        "dvfluency"
#define JK_VIDEO_LINK_TYPE        "dvlt"
#define JK_DEVICE_VIDEO_USERNAME    "dvusername"
#define JK_DEVICE_VIDEO_PASSWORD    "dvpassword"
#define JK_DEVICE_VIDEO_IP        "dvip"
#define JK_DEVICE_VIDEO_PORT    "dvport"
#define JK_DEVICE_VIDEO_TCP        "dvtcp"
#define JK_DEVICE_SOFT_VERSION    "dsv"
#define JK_DEVICE_BABY_MODE        "dbbm"
#define JK_DEVICE_FULL_ALARM_MODE    "dfam"
#define JK_DEVICE_RESET_FLAG    "drf"
#define JK_DEVICE_VERIFY        "dverify"
#define JK_DEVICE_CHANNEL_SUM    "dcs"
#define JK_DEVICE_CHANNEL_NO    "dcn"
#define JK_DEVICE_CHANNEL_NAME    "dcname"
#define JK_CHANNEL_LIST            "clist"
#define JK_DEVICE_WIFI_FLAG        "dwifi"
#define JK_DEVICE_RELATION_NUM    "drn"

#define JK_DEVICE_NAME_RESULT            "dnamers"
#define JK_NET_STORAGE_SWITCH_RESULT    "netssrs"
#define JK_TF_STORAGE_SWITCH_RESULT        "tfssrs"
#define JK_ALARM_SWITCH_RESULT            "aswitchrs"
#define JK_ALARM_VIDEO_FTP_RESULT        "avftprs"
#define JK_ALARM_SNAP_FTP_RESULT        "asnapftprs"
#define JK_ALARM_FTP_ACC_RESULT            "aftpaccrs"
#define JK_ALARM_FTP_PWD_RESULT            "aftppwdrs"
#define JK_ALARM_TIME_RESULT            "atimers"
#define JK_PIC_FTP_BIG_RESULT            "dpicbrs"
#define JK_PIC_FTP_SMALL_RESULT            "dpicsrs"
#define JK_PIC_FTP_ACC_RESULT            "dpicaccrs"
#define JK_PIC_FTP_PWD_RESULT            "dpicpwdrs"
#define JK_PIC_UPLOAD_TIMEING_RESULT    "dpicutrs"
#define JK_VIDEO_FLUENCY_RESULT            "dvfluencyrs"
#define JK_DEVICE_BABY_MODE_RESULT        "dbbmrs"
#define JK_DEVICE_FULL_ALARM_MODE_RESULT    "dfamrs"

#define JK_DEVICE_HUMITURE_FLAG        "dhflag"
#define JK_DEVICE_TEMPERATURE        "dtem"
#define JK_DEVICE_HUMIDNESS            "dhum"
#define JK_DEVICE_TIMESTAMP            "dts"
#define JK_DEVICE_HUMITURE_LIST        "dhlist"
#define JK_DEVICE_HUMITURE_DATE        "dhdate"        
#define JK_DEVICE_HUMITURE_HOUR        "dhour"
#define JK_DEVICE_HUMITURE_NUM        "dhnum"
#define JK_DEVICE_ENV_SCORE            "descore"    // 综合环境健康指数评分
#define JK_DEVICE_HUMITURE_SCORE    "dhscore"    // 温湿度评分
#define JK_DEVICE_HUMITURE_TOP        "dhtop"
#define JK_DEVICE_HUMITURE_LAST_SCORE    "dhlscore"
#define JK_DEVICE_HUMITURE_LAST_TOP        "dhltop"
#define JK_DEVICE_HUMITURE_RATIO    "dhratio"
#define JK_DEVICE_HUMITURE_ASSESSMENT    "dhass"
#define JK_DEVICES_ONLINE_STATUS    "dsls"
#define JK_DEVICE_IM_ONLINE_STATUS    "dimols"
#define JK_ONLINE_STATUS            "ols"
#define JK_DEVICES_PIC            "dspic"
#define JK_DEMO_POINT_SERVER    "dps"
#define JK_DEVICE_BIND_TYPE        "dbt"

// 设备升级
#define JK_UPGRADE_FILE_VERSION     "ufver"
#define JK_UPGRADE_FILE_URL         "ufurl"
#define JK_UPGRADE_FILE_DESCRIPTION "ufdes"
#define JK_UPGRADE_FILE_SIZE        "ufsize"
#define JK_UPGRADE_FILE_CHECKSUM    "ufc"
#define JK_UPGRADE_DOWNLOAD_STEP    "udstep"
#define JK_UPGRADE_WRITE_STEP       "uwstep"

// 设备报警
#define JK_ALARM_GUID            "aguid"
#define JK_ALARM_SOLUTION        "asln"
#define JK_ALARM_MESSAGE_TYPE    "amt"
#define JK_ALARM_STATUS            "astatus"
#define JK_ALARM_TYPE        "atype"
#define JK_ALARM_PIC            "apic"
#define JK_ALARM_PIC_SIZE        "apicsz"
#define JK_ALARM_VIDEO            "avd"
#define JK_ALARM_VIDEO_SZIE        "avdsz"
#define JK_ALARM_MESSAGE        "amsg"
#define JK_ALARM_TIMESTAMP        "ats"
#define JK_ALARM_TIMESTAMP_STR    "atss"
#define JK_ALARM_INDEX_START    "aistart"
#define JK_ALARM_INDEX_STOP        "aistop"
#define JK_ALARM_INFO            "ainfo"
#define JK_ALARM_ERROR            "aerror"

//设备报警-FTP模式
#define JK_ALARM_FTP_GUID            "aguid"
#define JK_ALARM_FTP_MESSAGE_TYPE    "amt"
#define JK_ALARM_FTP_DEVICE_GUID    "dguid"
#define JK_ALARM_FTP_TIMESTAMP        "ats"
#define JK_ALARM_FTP_DEVICE_NAME    "dname"
#define JK_ALARM_FTP_CHANNEL_NO        "dcn"
#define JK_ALARM_FTP_TYPE            "atype"
#define JK_ALARM_FTP_PIC            "apic"
#define JK_ALARM_FTP_PIC_SIZE        "apicsz"
#define JK_ALARM_FTP_VIDEO            "avd"
#define JK_ALARM_FTP_VIDEO_SZIE        "avdsz"

// 报警在线推送
#define JK_ALARM_SEND_ACCOUNT        "accountname"
#define JK_ALARM_SEND_GUID            "alarmguid"
#define JK_ALARM_SEND_CLOUDNUM        "cloudnum"
#define JK_ALARM_SEND_CLOUDNAME        "cloudname"
#define JK_ALARM_SEND_CLOUDCHN        "cloudchn"
#define JK_ALARM_SEND_ALARMTYPE        "alarmtype"
#define JK_ALARM_SEND_ALARMLEVEL    "alarmlevel"
#define JK_ALARM_SEND_ALARMTIME        "alarmtime"
#define JK_ALARM_SEND_PICURL        "picurl"
#define JK_ALARM_SEND_VIDEOURL        "videourl"

// 广告平台
#define JK_PRODUCT_TYPE            "prot"
#define JK_AD_VERSION            "adver"
#define    JK_AD_INFO                "adinfo"
#define JK_AD_NO                "adno"
#define JK_AD_URL                "adurl"
#define JK_AD_LINK                "adl"
#define JK_AD_URL_EN            "adurlen"
#define JK_AD_LINK_EN            "adlen"
#define JK_AD_URL_ZHT            "adurlzht"
#define JK_AD_LINK_ZHT            "adlzht"
#define JK_PORTAL_VERSION        "porver"
#define JK_PORTAL                "por"
#define JK_PORTAL_EN            "poren"
#define JK_PORTAL_ZHT            "porzht"
#define JK_PORTAL_SIZE            "psize"
#define JK_PUB_INDEX_START        "pistart"
#define JK_PUB_COUNT            "pcount"
#define JK_PUB_TIME                "ptime"
#define JK_PUB_INFO                "pinfo"
#define JK_PUB_LIST                "plist"

// 增值服务
#define JK_STREAMING_MEDIA_SERVER    "smsrv"
#define JK_STREAMING_MEDIA_TIME        "smt"
#define JK_STREAMING_MEDIA_CHANNELS    "smcs"
#define JK_STREAMING_MEDIA_FLAG        "sm"
#define JK_STREAMING_MEDIA_SHARE    "sms"
#define JK_CLOUD_STORAGE_FLAG        "csf"
#define JK_CLOUD_STORAGE_TIME        "cst"
#define JK_CLOUD_STORAGE_CHANNELS    "cscs"
#define JK_CLOUD_STORAGE_HOST        "cshost"
#define JK_CLOUD_STORAGE_ID            "csid"
#define JK_CLOUD_STORAGE_KEY        "cskey"
#define JK_CLOUD_STORAGE_SPACE        "csspace"
#define JK_CLOUD_STORAGE_TYPE        "cstype"
#define JK_SHARED_SERVER            "ssrv"
#define JK_SHARE_CHANNELS            "scs"
#define JK_RTMP_PORT                "rtmp"
#define JK_HLS_PORT                    "hls"
#define JK_STREAMING_MEDIA_SUPPORT    "smspt"
#define JK_CLOUD_STORAGE_SUPPORT    "csspt"
#define JK_CLOUD_STORAGE_RESULT     "csrs"

/* 报警服务器的消息类型 */
typedef enum alarm_client_messageid
{
    MID_RESPONSE_PUSHALARM            = 1000,        /* 告警服务器向客户端推送报警信息 */
    MID_REQUEST_ALARMPICURL            = 1001,        /* 客户端获取报警图片的url地址 */
    MID_RESPONSE_ALARMPICURL        = 1002,        /* 服务器向客户端发送报警图片的url地址 */
    MID_REQUEST_ALARMVIDEOURL        = 1003,        /* 客户端获取报警视频的url地址 */
    MID_RESPONSE_ALARMVIDEOURL        = 1004,        /* 服务器向客户端发送报警视频的url地址 */
    MID_REQUEST_ALARMHISTORY        = 1005,        /* 客户端获取报警的历史记录 */
    MID_RESPONSE_ALARMHISTORY        = 1006,        /* 服务器向客户端发送报警的历史记录 */
    MID_REQUEST_REMOVEALARM            = 1007,        /* 客户端发送删除报警信息 */
    MID_RESPONSE_REMOVEALARM        = 1008,        /* 服务器向客户端发发送删除报警信息的结果 */
    MID_RESPONSE_PUSHALARMCOUNT        = 1009        /* 告警服务器向客户端推送某一时间段的报警条数 */
} alarm_client_messageid_t;

/** Message Type */
enum AccountSystemMessageType
{
    IS_USER_EXIST = 1000,
    IS_USER_EXIST_RESPONSE = 1001,

    USER_REGISTER = 1002,
    USER_REGISTER_RESPONSE = 1003,

    LOGIN = 1004,
    LOGIN_RESPONSE = 1005,

    LOGOUT = 1006,
    LOGOUT_RESPONSE = 1007,

    MODIFY_USERPASS = 1008,
    MODIFY_USERPASS_RESPONSE = 1009,

    RESET_PASSWORD_NOSESSION = 1010,
    RESET_PASSWORD_NOSESSION_RESPONSE = 1011,

    SEND_FEEDBACK_TO_MAIL = 1012,
    SEND_FEEDBACK_TO_MAIL_RESPONSE = 1013,

    SEND_RESET_PASSWORD_MAIL = 1014,
    SEND_RESET_PASSWORD_MAIL_RESPONSE = 1015,

    GET_USER_DETAIL_INFO = 1016,
    GET_USER_DETAIL_INFO_RESPONSE = 1017,

    VERIFY_USERPASS = 1018,
    VERIFY_USERPASS_RESPONSE = 1019,

    GET_ALARM_FLAG    = 1020,
    GET_ALARM_FLAG_RESPONSE = 1021,

    SET_ALARM_FLAG = 1022,
    SET_ALARM_FLAG_RESPONSE = 1023,

    BIND_MAIL_OR_PHONE = 1024,
    BIND_MAIL_OR_PHONE_RESPONSE = 1025,

    REPORT_CLIENT_PLATFORM_INFO = 1026,
    REPORT_CLIENT_PLATFORM_INFO_RESPONSE = 1027,

    RESET_USERNAME_PASSWORD = 1028,
    RESET_USERNAME_PASSWORD_RESPONSE = 1029,

    JUDGE_USER_PASSWORD_STRENGTH = 1030,
    JUDGE_USER_PASSWORD_STRENGTH_RESPONSE = 1031,

    RESET_PASSWORD_BY_MOBILE = 1032,
    RESET_PASSWORD_BY_MOBILE_RESPONSE = 1033,

    SET_ACCOUNT_INFO = 1034,
    SET_ACCOUNT_INFO_RESPONSE = 1035,

    GET_ACCOUNT_INFO = 1036,
    GET_ACCOUNT_INFO_RESPONSE = 1037,

    GET_ACCOUNT_MAILPHONE_NOSESSEION = 1038,
    GET_ACCOUNT_MAILPHONE_NOSESSEION_RESPONSE = 1039,

    RESET_PASSWORD=1040,
    RESET_PASSWORD_RESPONSE=1041
};

enum AutoUpdateSystemMessageType
{
    GET_SOFT_VERSION = 5000,
    GET_SOFT_VERSION_RESPONSE = 5001,
};

enum IMServerMessageType
{
    USER_ONLINE = 3000,
    USER_ONLINE_RESPONSE = 3001,

    GET_LIVE_STATUS = 3002,
    GET_LIVE_STATUS_RESPONSE = 3003,

    SET_ONLINE_STATUS = 3004,
    SET_ONLINE_STATUS_RESPONSE = 3005,

    PUSH_DEVICE_MODIFY_INFO = 3006,
    PUSH_DEVICE_MODIFY_INFO_RESPONSE = 3007,

    PUSH_DEVICE_UPDATE_CMD = 3012,
    PUSH_DEVICE_UPDATE_CMD_RESPONSE = 3013,

    PUSH_DEVICE_CANCEL_CMD = 3014,
    PUSH_DEVICE_CANCEL_CMD_RESPONSE = 3015,

    GET_UPDATE_DOWNLOAD_STEP = 3016,
    GET_UPDATE_DOWNLOAD_STEP_RESPONSE = 3017,

    GET_UPDATE_WRITE_STEP = 3018,
    GET_UPDATE_WRITE_STEP_RESPONSE = 3019,

    PUSH_DEVICE_REBOOT_CMD = 3020,
    PUSH_DEVICE_REBOOT_CMD_RESPONSE = 3021,

    PUSH_DEVICE_MODIFY_PASSWORD = 3022,
    PUSH_DEVICE_MODIFY_PASSWORD_RESPONSE = 3023,


    PUSH_STREAMINGMEDIA_OPEN = 3024,
    PUSH_STREAMINGMEDIA_OPEN_RESPONSE = 3025,

    PUSH_STREAMINGMEDIA_CLOSE = 3026,
    PUSH_STREAMINGMEDIA_CLOSE_RESPONSE = 3027,

    PUSH_SHARE_OPEN = 3028,
    PUSH_SHARE_OPEN_RESPONSE = 3029,

    PUSH_SHARE_CLOSE = 3030,
    PUSH_SHARE_CLOSE_RESPONSE = 3031,

    PUSH_DEV_USER_OFFLINE = 3032,
    PUSH_DEV_USER_OFFLINE_RESPONSE = 3033,

    PUSH_CLOUD_STORAGE_OPEN = 3034,
    PUSH_CLOUD_STORAGE_OPEN_RESPONSE = 3035,
};

enum P2RelayMessageType
{
    HEATBEAT_DETECT = 4000,
    HEATBEAT_DETECT_RESPONSE = 4001,
};


enum AlarmMsgType
{
    GET_ALARM_INFO = 6000,
    GET_ALARM_INFO_RESPONSE = 6001,

    DEL_ALARM_INFO = 6002,
    DEL_ALARM_INFO_RESPONSE = 6003,

    CLEAN_ALARM_INFO = 6004,
    CLEAN_ALARM_INFO_RESPONSE = 6005,
};


enum IM2RelayMessageType
{
    NOTIFY_OFFLINE = 4300,
    RELAY_NOTIFY_OFFLINE = 4301,

    RELAY_DEVICE_MODIFY_INFO = 4302,
    RELAY_DEVICE_MODIFY_RESULT = 4303,

    RELAY_DEVICE_UPDATE_CMD = 4304,
    RELAY_DEVICE_UPDATE_CMD_RESULT = 4305,

    RELAY_DEVICE_CANCEL_CMD = 4306,
    RELAY_DEVICE_CANCEL_CMD_RESULT = 4307,

    RELAY_GET_DEVICE_UPDATE_STEP = 4308,
    RELAY_GET_DEVICE_UPDATE_STEP_RESULT = 4309,

    RELAY_DEVICE_REBOOT_CMD = 4310,
    RELAY_DEVICE_REBOOT_CMD_RESULT = 4311,

    RELAY_DEVICE_MODIFY_PASSWORD = 4312,
    RELAY_DEVICE_MODIFY_PASSWORD_RESULT = 4313,


    RELAY_STREAMINGMEDIA_OPEN = 4314,
    RELAY_STREAMINGMEDIA_CLOSE = 4315,

    RELAY_SHARE_OPEN = 4316,
    RELAY_SHARE_CLOSE = 4317,

    RELAY_CLOUDSTORAGE_OPEN = 4318,
    RELAY_CLOUDSTORAGE_CLOSE = 4319,

    RELAY_DEV_USER_ONLINE = 4320,
    RELAY_DEV_USER_OFFLINE = 4321,

    RELAY_CLOUD_STORAGE_OPEN = 4322,
};

enum Alarm2RelayMessageType
{
    GET_ACCOUNT_LIVE_INFO = 4600,
    GET_ACCOUNT_LIVE_INFO_RESPONSE = 4601,

    SEND_MESSAGE_TO_USER = 4602,
    SEND_MESSAGE_TO_USER_RESPONSE = 4603,

    RELAY_ALARM_MESSAGE = 4604,
    RELAY_ALARM_MESSAGE_RESPONSE = 4605,
};

/** 设备基础信息服务 */
enum MessageType_DeviceInfo
{
    DEVICE_REGISTER = 2001,
    DEVICE_REGISTER_RESPONSE = 2002,

    GET_USER_DEVICES = 2003,
    GET_USER_DEVICES_RESPONSE = 2004,

    GET_DEVICE_INFO = 2005,
    GET_DEVICE_INFO_RESPONSE = 2006,

    MODIFY_DEVICE_CONF_INFO = 2007,
    MODIFY_DEVICE_CONF_INFO_RESPONSE = 2008,

    GET_DEVICE_ONLINE_STATE = 2009,
    GET_DEVICE_ONLINE_STATE_RESPONSE = 2010,

    GET_DEVICE_PIC = 2011,
    GET_DEVICE_PIC_RESPONSE = 2012,

    MODIFY_DEVICE_INFO_VIDEO_LINK = 2013,
    MODIFY_DEVICE_INFO_VIDEO_LINK_RESPONSE = 2014,

    USER_BIND_DEVICE = 2015,
    USER_BIND_DEVICE_RESPONSE = 2016,

    USER_REMOVE_BIND_DEVICE = 2017,
    USER_REMOVE_BIND_DEVICE_RESPONSE = 2018,

    GET_USER_DEVICE_INFO = 2019,
    GET_USER_DEVICE_INFO_RESPONSE = 2020,

    GET_DEVICE_HUMITURE_STAT = 2021,
    GET_DEVICE_HUMITURE_STAT_RESPONSE = 2022,

    GET_DEVICE_HUMITURE_ONTIME = 2023,
    GET_DEVICE_HUMITURE_ONTIME_RESPONSE = 2024,

    GET_USER_DEVICES_STATUS_INFO = 2025,
    GET_USER_DEVICES_STATUS_INFO_RESPONSE = 2026,

    GET_DEVICE_HUMITURE_SCORE = 2027,
    GET_DEVICE_HUMITURE_SCORE_RESPONSE = 2028,

    GET_DEVICE_USERNAMES = 2029,
    GET_DEVICE_USERNAMES_RESPONSE = 2030,

    MODIFY_DEVICE_INFO_ADVANCED = 2031,
    MODIFY_DEVICE_INFO_ADVANCED_RESPONSE = 2032,

    GET_DEVICE_UPDATE_INFO = 2033,
    GET_DEVICE_UPDATE_INFO_RESPONSE = 2034,

    MODIFY_DEVICE_PASSWORD = 2035,
    MODIFY_DEVICE_PASSWORD_RESPONSE = 2036,

    DEVICE_SHARE = 2037,
    DEVICE_SHARE_RESPONSE = 2038,

    ADD_DEVICE_CHANNEL = 2039,
    ADD_DEVICE_CHANNEL_RESPONSE = 2040,

    DELETE_DEVICE_CHANNEL = 2041,
    DELETE_DEVICE_CHANNEL_RESPONSE = 2042,

    GET_DEVICE_CHANNEL = 2043,
    GET_DEVICE_CHANNEL_RESPONSE = 2044,

    MODIFY_DEVICE_CHANNEL_NAME = 2045,
    MODIFY_DEVICE_CHANNEL_NAME_RESPONSE = 2046,

    GET_DEVICE_RELATION_NUM = 2047,
    GET_DEVICE_RELATION_NUM_RESPONSE = 2048,

    MODIFY_DEVICE_WIFI_FLAG = 2047,
    MODIFY_DEVICE_WIFI_FLAG_RESPONSE = 2048,

    SET_AP_CONF_FLAG = 2049,
    SET_AP_CONF_FLAG_RESPONSE = 2050,

    GET_USER_CHANNELS = 2049,
    GET_USER_CHANNELS_RESPONSE = 2050,

    CHECK_DEVICE_BIND_STATE = 2051,
    CHECK_DEVICE_BIND_STATE_RESPONSE = 2052,

    REPORT_DEVICE_CLOUDSEE_ONLINE = 2053,
    REPORT_DEVICE_CLOUDSEE_ONLINE_RESPONSE = 2054,

    REPORT_DEVICE_RESET = 2055,
    REPORT_DEVICE_RESET_RESPONSE = 2056,

    GET_DEMO_POINT = 2057,
    GET_DEMO_POINT_RESPONSE = 2058,

    GET_DEMO_POINT_SERVER = 2059,
    GET_DEMO_POINT_SERVER_RESPONSE = 2060,
};
/** 设备在线服务 */
enum MessageType_DeviceOnline
{
    DEVICE_ONLINE = 2201,
    DEVICE_ONLINE_RESPONSE = 2202,

    DEVICE_HEARTBEAT = 2203,
    DEVICE_HEARTBEAT_RESPONSE = 2204,

    DEVICE_OFFLINE = 2205,
    DEVICE_OFFLINE_RESPONSE = 2206,

    DEVICE_REPORT_HUMITURE = 2207,
    DEVICE_REPORT_HUMITURE_RESPONSE = 2208,

    PUSH_DEVICE_MODIFY_RESULT = 2209,
    PUSH_DEVICE_MODIFY_RESULT_RESPONSE = 2210,

    PUSH_DEVICE_UPDATE_CMD_RESULT = 2211,
    PUSH_DEVICE_UPDATE_CMD_RESULT_RESPONSE = 2212,

    PUSH_DEVICE_CANCEL_CMD_RESULT = 2213,
    PUSH_DEVICE_CANCEL_CMD_RESULT_RESPONSE = 2214,

    GET_DEVICE_UPDATE_STEP_RESULT = 2215,
    GET_DEVICE_UPDATE_STEP_RESULT_RESPONSE = 2216,

    PUSH_DEVICE_MODIFY_PASSWORD_RESULT = 2217,
    PUSH_DEVICE_MODIFY_PASSWORD_RESULT_RESPONSE = 2218,

    PUSH_ALARM_MESSAGE = 2219,
    PUSH_ALARM_MESSAGE_RESPONSE = 2220,

    PUSH_ALARM_MESSAGE_FTP = 2221,
    PUSH_ALARM_MESSAGE_FTP_RESPONSE = 2222,

    PUSH_ALARM_MESSAGE_CLOUD = 2223,
    PUSH_ALARM_MESSAGE_CLOUD_RESPONSE = 2224,

    DEVICE_REGISTER_ONLINE = 2225,
    DEVICE_REGISTER_ONLINE_RESPONSE = 2226,

    PUSH_CLOUD_STORAGE_OPEN_RESULT = 2227,
    PUSH_CLOUD_STORAGE_OPEN_RESULT_RESPONSE = 2228,
};


enum MessageType_ADPublishSys
{
    GET_AD_INFO = 5500,
    GET_AD_INFO_RESPONSE = 5501,

    GET_PORTAL = 5502,
    GET_PORTAL_RESPONSE = 5503,

    GET_PUBLISH_INFO = 5504,
    GET_PUBLISH_INFO_RESPONSE = 5505,
};

enum MessageType_VAS
{
    STREAMING_MEDIA_OPEN = 5200,
    STREAMING_MEDIA_OPEN_RESPONSE = 5201,

    STREAMING_MEDIA_SHARE = 5202,
    STREAMING_MEDIA_SHARE_RESPONSE = 5203,

    GET_DEVICE_VAS_INFO = 5204,
    GET_DEVICE_VAS_INFO_RESPONSE = 5205,

    CLOUD_STORAGE_OPEN = 5206,
    CLOUD_STORAGE_OPEN_RESPONSE = 5207,

    GET_CLOUD_STORAGE_INFO = 5208,
    GET_CLOUD_STORAGE_INFO_RESPONSE = 5209,

    GET_VAS_SUPPORT = 5210,
    GET_VAS_SUPPORT_RESPONSE = 5211,
};

// add by houbin
enum MessageType_LoadReport
{
    LOAD_REPORT_REQ = 3000,
    LOAD_REPORT_RES = 3001,

    LOAD_SERVER_LIST_REQ = 3002,
    LOAD_SERVER_LIST_RES = 3003
};


enum TcpConnectFlag
{
    SHORT_CONNECTION,
    PERSIST_CONNECTION,
};

enum LogicProcessType
{
    ACCOUNT_BUSINESS_PROCESS = 0,
    DEV_INFO_PRO = 1,
    ALARM_PROCESS = 2,
    UPDATE_PROCESS = 4,
    DEV_INFO_HOMECLOUD = 5,

    IM_SERVER_DIRECT = 6,
    IM_SERVER_RELAY = 7,
    IM_SERVER_RELAY_REQUEST = 8,
    IM_DEV_DIRECT = 9,

    ALARM_SERVER_RELAY = 10,
    ALARM_INFO_PROCESS = 11,
};


/* ============= 联网报警自定义 =========== */
/*
 *   key of json
 */
// 联网报警新定义的消息共有格式
#define JK_MESSAGE_TYPE     "method"
#define JK_METHOD           "method"
#define JK_LOGIN            "login"
#define JK_SET_PHONEID      "set_phoneid"

// 上报负载信息
#define JK_SERVICE_TYPE     "service_type"
#define JK_LOAD_NUMBER      "load_number"
#define JK_CLIENT_TYPE      "ct"
#define JK_PARAM            "param"
#define JK_NETGATE_LIST     "list"
#define JK_ONLINESRV_LIST   "list_online"
#define JK_SERVER_IP        "server_host"
#define JK_SERVER_PORT      "server_port"
#define JK_ID               "id"
#define JK_USER             "user"
#define JK_PWD              "pwd"
#define JK_PHONE_ID         "phone_id"
#define JK_SESSION          "session"

// 设备上线
#define JK_SEND_CNT         "sentcnt"
#define JK_DEV_ID           "dev_id"
#define JK_ERROR            "error"
#define JK_ERRORCODE        "errorcode"
#define JK_STATE            "state"
#define JK_TIME             "time"
#define JK_TM_STR           "tm"
#define JK_AUTH_DATA        "data"

// 用户上线
#define JK_SESSION          "session"

// 语音转发服务器
#define JK_CLIENT_IP        "client_ip"
#define JK_DEV_IP           "dev_ip"
#define JK_IP               "ip"
#define JK_PORT             "port"


/*
 *   value of json
 */
#define METHOD_GET_SERVERS              "get_servers"
#define METHOD_ON_GET_SERVERS           "on_get_servers"
#define METHOD_KEEP_ONLINE              "keep_online"
#define METHOD_PUSH_MSG                 "push_msg"
#define METHOD_ON_PUSH_MSG              "on_push_msg"
#define METHOD_DEVICE_STATE_NOTICE      "device_state_notice"
#define METHOD_DEVICE_LOGIN             "login"
#define METHOD_ON_DEVICE_LOGIN          "on_login"
#define METHOD_SET_STREAMSERVER_ADDR    "set_streamserver_addr"
#define METHOD_GET_DEV_IP               "get_dev_ip"
#define METHOD_ON_GET_DEV_IP            "on_get_dev_ip"
#define METHOD_LOGOUT                   "logout"
#define METHOD_ON_LOGOUT                "on_logout"
#define METHOD_GET_VOICESERVER_ADDR     "get_voiceserver_addr"
#define METHOD_ON_GET_VOICESERVER_ADDR  "on_get_voiceserver_addr"


enum ClientType
{
    CLIENT_TYPE_DEV = 1,
    CLIENT_TYPE_PC  = 2,
    CLIENT_TYPE_APP = 3
};

enum ServerType
{
    SERVER_NETGATE=1,
    SERVER_ONLINE=2
};

#endif

