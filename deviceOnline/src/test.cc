#include <unistd.h>
#include "device_alive.h"
#include "thread.h"
#include "json_opt.h"
#include "test.h"
#include "defines.h"
#include "../../public/utils.h"
#include "global_settings.h"


void *TestThread::Entry()
{
    int i = 0;
    for (; i < 1000000; i++)
    {
        CJsonOpt json_opt;
        string msg_info = json_opt.JsonJoinUserLogin();
        string url = utils::G<CGlobalSettings>().httpserver_url_;
        SendMsg send_msg(url, msg_info);
        g_http_client->SubmitMsg(send_msg);
        sleep(5);
    }

    return 0;
}

