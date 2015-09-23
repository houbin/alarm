#ifndef VOICE_CHANNEL_
#define VOICE_CHANNEL_

#include <map>
#include "global.h"

using namespace std;
using namespace util;

struct VoiceChannel
{
    string client_id;
    int client_fd;
    bool is_client_connect;

    string dev_id;
    int dev_fd;
    bool is_dev_connect;
};

class VoiceChannelMap
{
private:
    Mutex mutex_;
    map<string, VoiceChannel> vc_map_; // client -> voice channel

public:
    VoiceChannelMap() : mutex_("VoiceChannelMap::Mutex") { }
    ~VoiceChannelMap() { }

    int32_t ConnectByDev(string dev_id, string client_id, int dev_fd);
    int32_t ConnectByClient(string dev_id, string client_id, int client_fd);
    int32_t Disconnect(string dev_id, string client_id);

    int32_t RouteMsgToDev(string dev_id, string client_id, slice &msg);
};

#endif
