#include <stdio.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <signal.h>
#include "../util/config.h"
#include "master.h"

using namespace std;

Config *g_config = NULL;
Logger *g_logger = NULL;
Master *json_master = NULL;
Master *tlv_master = NULL;

void run_daemon()
{
    pid_t pid;
    
    pid = fork();
    if (pid > 0)
    {
        // this is parent process
        exit(1);
    }
    else if (pid != 0)
    {
        exit(0);
    }

    // this is child process
    setsid();
    return;
}

int run_master()
{
    MasterOption json_master_option;
    json_master_option.data_protocol_ = "json";
    json_master_option.listen_port_ = g_config->Read("json_listen_port", 15050);
    json_master_option.worker_count_ = 4;
    json_master_option.worker_conn_count_ = 2500;
    json_master_option.read_timeout_ = g_config->Read("json_read_timeout", 30);
    json_master_option.write_timeout_ = g_config->Read("json_write_timeout", 0);
    json_master = new Master(g_logger, "json_master", json_master_option);

    MasterOption tlv_master_option;
    tlv_master_option.data_protocol_ = "tlv";
    tlv_master_option.listen_port_ = g_config->Read("tlv_listen_port", 15040);
    tlv_master_option.worker_count_ = 4;
    tlv_master_option.worker_conn_count_ = 2500;
    tlv_master_option.read_timeout_ = g_config->Read("tlv_read_timeout", 30);
    tlv_master_option.write_timeout_ = g_config->Read("tlv_write_timeout", 0);
    json_master = new Master(g_logger, "tlv_master", tlv_master_option);

    LOG_INFO(g_logger, "==================== config ====================");
    LOG_INFO(g_logger, "json listen port %u", json_master_option.listen_port_);
    LOG_INFO(g_logger, "json read timeout %u", json_master_option.read_timeout_);
    LOG_INFO(g_logger, "json write timeout %u", json_master_option.write_timeout_);
    LOG_INFO(g_logger, "tlv listen port %u", tlv_master_option.listen_port_);
    LOG_INFO(g_logger, "tlv read timeout %u", tlv_master_option.read_timeout_);
    LOG_INFO(g_logger, "tlv write timeout %u", tlv_master_option.write_timeout_);
    LOG_INFO(g_logger, "==================== config ====================");

    json_master->Init();
    json_master->Start();
    tlv_master->Init();
    tlv_master->Start();
    
    return 0;
}

int wait_master()
{
    json_master->Wait();
    tlv_master->Wait();
    return 0;
}

int shutdown_master()
{
    json_master->Shutdown();
    tlv_master->Shutdown();
    return 0;
}

int is_file_exist(string path)
{
    if (path == "")
    {
        return -1;
    }

    if (access(path.c_str(), F_OK) != 0)
    {
        return -1;
    }

    return 0;
}

void handle_signal(int signum)
{
    if (signum == SIGUSR1)
    {
        string config_path("/etc/jovision/voice_server/voice_server.conf")
        ret = is_file_exist(config_path.c_str());
        if (ret != 0)
        {
            fprintf(stderr, "config file not exist\n");
            return;
        }

        g_config->ReadFile(config_path.c_str());
        int log_level = g_config->Read("log_level", util::INFO);
        g_logger->SetLogLevel(log_level);
    }
    else if (signum == SIGUSR2)
    {
        shutdown_master();
    }

    return;
}

void register_signal(int signum)
{
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_signal;
    act.sa_flags = 0;

    sigaction(signum, &act, NULL);
    return;
}

int main()
{
    int ret = 0;
    FILE *f = NULL;
    string log_path;
    int log_level;
    string config_path("/etc/jovision/voice_server/voice_server.conf")
    
    ret = is_file_exist(config_path.c_str());
    if (ret != 0)
    {
        fprintf(stderr, "config file not exist\n");
        return -1;
    }


    g_config = new Config(config_path);
    log_path = g_config->Read("log_path", "/var/log/voice_server/");
    log_level = g_config->Read("log_level", util::INFO);

    g_logger = new Logger(log_path);
    g_logger->SetLogLevel(log_level);

    run_daemon();

    LOG_INFO(g_logger, "run voice server......")

    run_master();
    wait_master();

    delete json_master;
    delete tlv_master;

    LOG_INFO(g_logger, "end voice server.....");

    return 0;
}

