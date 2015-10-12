/*
 * global_settings.h
 *
 *  Created on: Mar 12, 2013
 *      Author: yaowei
 */

#ifndef GLOBAL_SETTINGS_H_
#define GLOBAL_SETTINGS_H_
#include <string>

class CGlobalSettings
{
public:
    int remote_listen_port_;
    unsigned int thread_num_;

    int local_listen_port_;
    int local_conn_timeout_;


    std::string redis_ip_;
    int redis_port_;

    int client_heartbeat_timeout_;

    std::string httpserver_url_;
};

#endif /* GLOBAL_SETTINGS_H_ */
