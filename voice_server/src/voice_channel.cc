#include "voice_channel.h"
#include "../../public/error_code.h"

int32_t VoiceChannelManager::ConnectByClient(string client_id, ClientVcInfo client_vc_info)
{
    LOG_DEBUG(g_logger, "connect by client, client id %s, dev_id %s", client_id.c_str(), client_vc_info.dev_id.c_str());

    Mutex::Locker lock(mutex_);

    map<string, ClientVcInfo>::iterator iter = client_vc_map_.find(client_id);
    if(iter != client_vc_map_.end())
    {
        LOG_ERROR(g_logger, "find the same client_id in voice channel, client is %s", client_id.c_str());
        
        bufferevent_free(client_vc_info.client_bev);
        client_vc_map_.erase(client_id);
    }

    client_vc_map_.insert(pair<string, ClientVcInfo>(client_id, client_vc_info));
    LOG_DEBUG(g_logger, "connect by client ok, client id %s, dev_id %s", client_id.c_str(), client_vc_info.dev_id.c_str());

    return 0;
}

int32_t VoiceChannelManager::ConnectByDev(string dev_id, DevVcInfo dev_vc_info)
{
    LOG_DEBUG(g_logger, "connect by dev, dev_id %s, client_id %s", dev_id.c_str(), dev_vc_info.client_id.c_str());
    
    Mutex::Locker lock(mutex_);

    pair<multimap<string, DevVcInfo>::iterator, multimap<string, DevVcInfo>::iterator> ret;
    ret = dev_vc_map_.equal_range(dev_id);

    multimap<string, DevVcInfo>::iterator iter;
    for(iter = ret.first; iter != ret.second; iter++)
    {
        if (iter->second.client_id == dev_vc_info.client_id)
        {
            // close this event
            bufferevent_free(iter->second.dev_bev);
            break;
        }
    }

    dev_vc_map_.insert(pair<string, DevVcInfo>(dev_id, dev_vc_info));
    LOG_DEBUG(g_logger, "connect by dev ok, dev_id %s, client_id %s", dev_id.c_str(), dev_vc_info.client_id.c_str());

    return 0;
}

int32_t VoiceChannelManager::DisconnectByClient(string client_id, string dev_id)
{
    LOG_DEBUG(g_logger, "disconnect by client, client id %s, dev_id %s", client_id.c_str(), dev_id.c_str());

    Mutex::Locker lock(mutex_);

    // close client connection
    map<string, ClientVcInfo>::iterator iter = client_vc_map_.find(client_id);
    if(iter != client_vc_map_.end())
    {
        ClientVcInfo client_vc_info = iter->second;
        string find_dev_id = client_vc_info.dev_id;
        if (find_dev_id == dev_id)
        {
            LOG_DEBUG(g_logger, "find client voice channel, clean it, client id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
            client_vc_map_.erase(client_id);
            // 该通道需要等待回复消息发送完成，才能释放
        }
    }

    // close dev connection
    pair<multimap<string, DevVcInfo>::iterator, multimap<string, DevVcInfo>::iterator> ret;
    ret = dev_vc_map_.equal_range(dev_id);
    multimap<string, DevVcInfo>::iterator dev_iter;
    for(dev_iter = ret.first; dev_iter != ret.second; dev_iter++)
    {
        if (dev_iter->second.client_id == client_id)
        {
            // close this event
            LOG_DEBUG(g_logger, "find relevent dev voice channel, clean it, client id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
            bufferevent_free(dev_iter->second.dev_bev);
            dev_vc_map_.erase(dev_iter);
            break;
        }
    }

    LOG_DEBUG(g_logger, "disconnect by client ok, client id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
    return 0;
}

int32_t VoiceChannelManager::DisconnectByDev(string dev_id, string client_id)
{
    LOG_DEBUG(g_logger, "disconnect by client, dev_id %s, client_id %s", dev_id.c_str(), client_id.c_str());

    Mutex::Locker lock(mutex_);

    // close dev connection
    pair<multimap<string, DevVcInfo>::iterator, multimap<string, DevVcInfo>::iterator> ret;
    ret = dev_vc_map_.equal_range(dev_id);
    multimap<string, DevVcInfo>::iterator dev_iter;
    for(dev_iter = ret.first; dev_iter != ret.second; dev_iter++)
    {
        if (dev_iter->second.client_id == client_id)
        {
            // close this event
            LOG_DEBUG(g_logger, "find dev voice channel, clean it, client id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
            dev_vc_map_.erase(dev_iter);
            // 该通道需要等待回复设备消息后，才能释放
            break;
        }
    }

    // close client connection
    map<string, ClientVcInfo>::iterator client_iter = client_vc_map_.find(client_id);
    if (client_iter != client_vc_map_.end())
    {
        ClientVcInfo client_vc_info = client_iter->second;
        if (client_vc_info.dev_id == dev_id)
        {
            LOG_DEBUG(g_logger, "find relevent voice channel, clean it, client id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
            bufferevent_free(client_vc_info.client_bev);
            client_vc_map_.erase(client_iter);
        }
    }

    LOG_DEBUG(g_logger, "disconnect by client ok, dev_id %s, client_id %s", dev_id.c_str(), client_id.c_str());

    return 0;
}

int32_t VoiceChannelManager::RouteClientToDev(string client_id, string dev_id, int &dev_fd)
{
    Mutex::Locker lock(mutex_);

    LOG_DEBUG(g_logger, "route client to dev, client_id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
    map<string, ClientVcInfo>::iterator client_iter = client_vc_map_.find(client_id);
    if (client_iter == client_vc_map_.end())
    {
        LOG_ERROR(g_logger, "can't find client voice channel, client_id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
        return -ERROR_CLIENT_VC_INVALID;
    }

    ClientVcInfo client_vc_info = client_iter->second;
    if (client_vc_info.dev_id != dev_id)
    {
        LOG_ERROR(g_logger, "can't find client voice channel, client_id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
        return -ERROR_CLIENT_VC_INVALID;
    }

    pair<multimap<string, DevVcInfo>::iterator, multimap<string, DevVcInfo>::iterator> ret;
    ret = dev_vc_map_.equal_range(dev_id);
    multimap<string, DevVcInfo>::iterator dev_iter;
    for(dev_iter = ret.first; dev_iter != ret.second; dev_iter++)
    {
        DevVcInfo dev_vc_info = dev_iter->second;
        if (dev_vc_info.client_id == client_id)
        {
            break;
        }
    }

    if (dev_iter == ret.second)
    {
        LOG_ERROR(g_logger, "can't find dev voice channel, client_id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
        return -ERROR_DEV_VC_INVALID;
    }
    else
    {
        LOG_ERROR(g_logger, "find dev voice channel ok, client_id %s, dev_id %s", client_id.c_str(), dev_id.c_str());
        dev_fd = dev_iter->second.dev_fd;
    }

    return 0;
}


