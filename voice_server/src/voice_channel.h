#ifndef VOICE_CHANNEL_
#define VOICE_CHANNEL_

#include <map>
#include "global.h"
#include <event.h>

using namespace std;
using namespace util;

struct ClientVcInfo
{
    string client_id;
    int client_fd;
    struct bufferevent *client_bev;

    string dev_id;
};

struct DevVcInfo
{
    string dev_id;
    int dev_fd;
    struct bufferevent *dev_bev;

    string client_id;
};

class VoiceChannelManager
{
private:
    Mutex mutex_;
    map<string, ClientVcInfo> client_vc_map_; // client -> ClientVcInfo
    multimap<string, DevVcInfo> dev_vc_map_; // dev -> DevVcInfo

public:
    VoiceChannelManager() : mutex_("VoiceChannelManager::Mutex") { }
    ~VoiceChannelManager() { }

    int32_t ConnectByDev(string dev_id, DevVcInfo dev_vc_info);
    int32_t ConnectByClient(string client_id, ClientVcInfo client_vc_info);
    int32_t DisconnectByDev(string dev_id, string client_id);
    int32_t DisconnectByClient(string client_id);

    int32_t RouteClientToDev(string client_id, string dev_id, int &dev_fd);
};

#endif
