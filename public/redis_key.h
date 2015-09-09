#ifndef REDIS_KEY_H
#define REDIS_KEY_H

#include <string>

enum DB_NUMBER
{
	SESSION = 0,
	STATUS  = 2,
	MESSAGE = 3,
	DEVICE = 4,
	HUMITURE = 5,
	DEVICE_USER = 6,
    CLIENT_USER = 7
};

static inline std::string int2str(int v)
{
	std::stringstream ss;
	ss << v;
	return ss.str();
}


#define ONLINE_FLAG ":onlineflag"
inline std::string RedisKeyUserOnlineFlag(const std::string& username)
{ return username + std::string(ONLINE_FLAG);}

#define CLIENT_LOGIN_INFO 	":loginclientinfo"
#define KEY_LOGIN_PLATFORM 	":platform"
#define KEY_MOBILE_ID		":moblieid"
#define KEY_LANGUAGE_TYPE	":langt"
#define KEY_ALARM_FLAG		":alarm"
inline std::string RedisKeyClientLoginInfo(const std::string& username)
{ return username + std::string(CLIENT_LOGIN_INFO);}

#define ONLINE_SERVER_INFO ":s_info"
#define KEY_ONLINE_SERVER_NO ":s_no"
#define KEY_ONLINE_SERVER_FD ":s_fd"
#define KEY_ONLINE_SERVER_FD_ID ":s_fd_id"
inline std::string RedisKeyUserOnlineServerInfo(const std::string& username)
{ return username + std::string(ONLINE_SERVER_INFO);}

#define REDIS_DEVICE_LIST   ":devicelist"
inline std::string RedisKeyDevicelist(const std::string& username) 
{ return username + std::string(REDIS_DEVICE_LIST);}

#define ONLINE_FLAG ":onlineflag"
inline std::string RedisKeyDeviceOnlineFlag(const std::string& device_guid) 
{ return device_guid + std::string(ONLINE_FLAG);}

#define ONLINE_SERVER_INFO ":s_info"
#define KEY_ONLINE_SERVER_NO ":s_no"
#define KEY_ONLINE_SERVER_FD ":s_fd"
#define KEY_ONLINE_SERVER_FD_ID ":s_fd_id"
inline std::string RedisKeyDeviceOnlineServerInfo(const std::string& device_guid) 
{ return device_guid + std::string(ONLINE_SERVER_INFO);}


#define REDIS_GROUP_ID(id)   id + std::string(":groupid")

inline std::string RedisKeySfd2Device(const int server_no, const int sfd) 
{return int2str(server_no) + std::string("&") +int2str(sfd);}

#define REDIS_KEY_DEVICE_CHANGE ":dc"
inline std::string RedisKeyDeviceChange(const std::string& device_guid) 
{ return device_guid + std::string(REDIS_KEY_DEVICE_CHANGE);}

#define REDIS_ALL_DEVICES_RELATED_USER ":adru"
inline std::string RedisKeyAllDevicesRelatedUser(const std::string& username) 
{ return username + std::string(REDIS_ALL_DEVICES_RELATED_USER);}

inline std::string RedisKeyDeviceHumiture(const std::string& device_guid, const std::string& timestamp)
{ return device_guid + std::string("&") + timestamp; }
#define REDIS_HASH_FEILD_TEMPERATURE	"temperature"
#define REDIS_HASH_FEILD_HUMIDNESS		"humidity"

#define REDIS_LATEST_HUMITURE_KEY ":latesthumiturekey"
inline std::string RedisKeyLatestDeviceHumitureKey(const std::string& device_guid)
{ return device_guid + std::string(REDIS_LATEST_HUMITURE_KEY); }

inline std::string RedisKeyDeviceApConfFlag(const std::string& device_guid)
{ return device_guid + std::string(":apconf"); }

#define REDIS_DEVICE_USER_KEY ":deviceuser"
inline std::string RedisKeyDeviceUserKey(const std::string& device_guid)
{ return device_guid + std::string(REDIS_DEVICE_USER_KEY); }

#endif
