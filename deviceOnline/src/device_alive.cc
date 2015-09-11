//============================================================================
// Name        : deviceOnline.cpp
// Author      : yaocoder
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <signal.h>
#include "defines.h"
#include "init_configure.h"
#include "master_thread.h"
#include "redis_conn_pool.h"
#include "global_settings.h"
#include "../../public/utils.h"
#include "../../public/config_file.h"
#include "redis_opt.h"
#include "device_alive.h"
#include "test.h"
#include "local_transport.h"

static void InitConfigure();
static void SettingsAndPrint();
static void InitRedis();
static void Run();
static void SigUsr(int signo);

MsgReplyQueue *g_msg_reply_queue = NULL;
HttpClient *g_http_client = NULL;

TestThread *g_test_thread = NULL;

int main(int argc, char **argv)
{
	/* process arguments */
	int c;
	std::string version = std::string("1.0.387");
	while (-1 != (c = getopt(argc, argv, "v" /* 获取程序版本号，配合svn */
	)))
	{
		switch (c)
		{
		case 'v':
			printf("The version is %s\n", version.c_str());
			return EXIT_SUCCESS;
		default:
			break;
		}
	}

	InitConfigure();

	SettingsAndPrint();

	if(signal(SIGUSR1, SigUsr) == SIG_ERR)
	{
		LOG4CXX_FATAL(g_logger, "Configure signal failed.");
		exit(EXIT_FAILURE);
	}

	InitRedis();

	if (daemon(1, 0) == -1)
	{
		LOG4CXX_FATAL(g_logger, "daemon failed.");
	}
    
    // clear redis data
    ClearDeviceFdCache();

    // start http client
    int ret = 0;
    ret = StartHttpClient();
    if (ret != 0)
    {
        LOG4CXX_ERROR(g_logger, "start http client error, ret " << ret);
        return -1;
    }
    
    // start test thread
    //g_test_thread = new TestThread;
    //g_test_thread->Create();
    //
    // start local_transport
    CLocalTransport *local_transport = CLocalTransport::GetInstance();
    local_transport->SetupLocalTransport();

	Run();

	return EXIT_SUCCESS;
}

void ClearDeviceFdCache()
{
    bool bRet;
    long long cursor = 0;
    long long next_cursor = 0;
    long long keys_sum_count = 0;

    redisContext* redis_con = CRedisConnPool::GetInstance()->GetRedisContext();
    CRedisOpt redis_opt;
    redis_opt.SetRedisContext(redis_con);
    redis_opt.SelectDB(DEVICE);

    while (true)
    {
        vector<string> vec_keys;
        bRet = redis_opt.Scan(cursor, vec_keys, next_cursor);
        if (!bRet)
        {
            LOG4CXX_ERROR(g_logger, "redis_opt scan error, cursor " << cursor << ", next_cursor " << next_cursor);
            break;
        }

        vector<string>::iterator iter = vec_keys.begin();
        for (; iter != vec_keys.end(); iter++)
        {
            LOG4CXX_ERROR(g_logger, "redis_opt set " << *iter << " -1");
            redis_opt.Del(*iter);
        }

        cursor = next_cursor;
        keys_sum_count += vec_keys.size();

        if (cursor == 0)
        {
            LOG4CXX_INFO(g_logger, "redis_opt scan over, sum of keys " << keys_sum_count);
            break;
        }
    }

	CRedisConnPool::GetInstance()->ReleaseRedisContext(redis_con);

    return;
}

int StartHttpClient()
{
    int ret = 0;

    string url = utils::G<CGlobalSettings>().httpserver_url_;

    g_http_client = new HttpClient(url);
    if (g_http_client == NULL)
    {
        return -1;
    }

    ret = g_http_client->Init();
    if (ret != 0)
    {
		LOG4CXX_FATAL(g_logger, "http client init failed.");
        return ret;
    }

    g_http_client->Start();

    return 0;
}

void Run()
{
    // start message reply queue
    g_msg_reply_queue = new MsgReplyQueue();
    g_msg_reply_queue->Start();

	CMasterThread masterThread;
	if(!masterThread.InitMasterThread())
	{
		LOG4CXX_FATAL(g_logger, "InitMasterThread failed.");
		exit(EXIT_FAILURE);
	}

	masterThread.Run();
}

void SigUsr(int signo)
{
	if(signo == SIGUSR1)
	{
		/* 重新加载应用配置文件（仅仅是连接超时时间），log4cxx日志配置文件*/
		InitConfigure();
		SettingsAndPrint();
		LOG4CXX_INFO(g_logger, "reload configure.");
		return;
	}
}

void InitConfigure()
{
	CInitConfig initConfObj;
	initConfObj.SetConfigFilePath(std::string("/etc/jovision/alarm/conf/"));
	std::string project_name = "deviceOnline";
	initConfObj.InitLog4cxx(project_name);
	if (!initConfObj.LoadConfiguration(project_name))
	{
		LOG4CXX_FATAL(g_logger, "LoadConfiguration failed.");
		exit(EXIT_FAILURE);
	}
}

void InitRedis()
{
	RedisConnInfo redisConnInfo;
	redisConnInfo.max_conn_num = 4;
	redisConnInfo.ip = utils::G<ConfigFile>().read<std::string> ("redis.ip", "127.0.0.1");
	redisConnInfo.port = utils::G<ConfigFile>().read<int> ("redis.port", 6379);
	if (!CRedisConnPool::GetInstance()->Init(redisConnInfo))
	{
		LOG4CXX_FATAL(g_logger, "Init redisConnPool failed.");
		exit(EXIT_FAILURE);
	}
}

void SettingsAndPrint()
{
	utils::G<CGlobalSettings>().remote_listen_port_ = utils::G<ConfigFile>().read<int> ("remote.listen.port", 15030);
	utils::G<CGlobalSettings>().thread_num_= utils::G<ConfigFile>().read<int> ("worker.thread.num", 4);
	utils::G<CGlobalSettings>().client_heartbeat_timeout_ = utils::G<ConfigFile>().read<int>("client.timeout.s", 70);
	utils::G<CGlobalSettings>().local_listen_port_ = utils::G<ConfigFile>().read<int>("local.listen.port", 15031);
	utils::G<CGlobalSettings>().local_conn_timeout_ = utils::G<ConfigFile>().read<int>("local.conn.timeout", 5);
	utils::G<CGlobalSettings>().httpserver_url_ = utils::G<ConfigFile>().read<string>("httpserver.url", "http://172.16.27.219:8081/netalarm-rs/rsapi/userauth/login");

	LOG4CXX_INFO(g_logger, "******deviceOnline.remote.listen.port = " << utils::G<CGlobalSettings>().remote_listen_port_ << "******");
	LOG4CXX_INFO(g_logger, "******deviceOnline.worker.thread.num = "  << utils::G<CGlobalSettings>().thread_num_ << "******");
	LOG4CXX_INFO(g_logger, "******deviceOnline.client.timeout.s = "   << utils::G<CGlobalSettings>().client_heartbeat_timeout_ << "******");
	LOG4CXX_INFO(g_logger, "******deviceOnline.httpserver.url  = "   << utils::G<CGlobalSettings>().httpserver_url_ << "******");
}

