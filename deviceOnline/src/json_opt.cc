/*
 * json_pt.cc
 *
 *  Created on: Mar 6, 2013
 *      Author: yaowei
 */

#include <time.h>
#include "json_opt.h"
#include "../../public/message.h"
#include "../../public/utils.h"
#include "../../public/error_code.h"

using namespace std;

CJsonOpt::CJsonOpt()
{
	send_cnt_ = 0;
}

CJsonOpt::~CJsonOpt()
{
}

void CJsonOpt::setJsonString(const std::string &msg)
{
	out_ = JSONNode();
	json_string_ = msg;
}

int CJsonOpt::GetMethod(string &method)
{
    method = method_;
    return 0;
}

int CJsonOpt::GetSendCnt()
{
    return send_cnt_;
}

bool CJsonOpt::VerifyJsonField(const string &field)
{
    if (in_.find(field) != in_.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CJsonOpt::JsonParseCommon()
{
    try
    {
        in_ = libjson::parse(json_string_);
    }
    catch(...)
    {
        return false;
    }

    if (!(VerifyJsonField(JK_SEND_CNT) && VerifyJsonField(JK_METHOD)))
    {
        return false;
    }

    send_cnt_ = in_[JK_SEND_CNT].as_int();
    method_ = in_[JK_METHOD].as_string();

    return true;
}

int CJsonOpt::JsonParseBeacon(string &dev_id)
{
    if (!VerifyJsonField(JK_PARAM))
    {
        return -1;
    }

    JSONNode param_node = in_[JK_PARAM].as_node();
    dev_id = param_node[JK_DEV_ID].as_string();

    return 0;
}

#define AUTH_DATA_SIZE 13
int CJsonOpt::JsonParseLogin(string &dev_id, deque<int> &auth_data)
{
    if (!VerifyJsonField(JK_PARAM))
    {
        return -1;
    }

    JSONNode param_node = in_[JK_PARAM].as_node();
    param_node.preparse();

    dev_id = param_node[JK_DEV_ID].as_string();

    JSONNode auth_data_node(JSON_ARRAY);
    auth_data_node = param_node[JK_AUTH_DATA].as_node();

    int size = auth_data_node.size();
    if (size != AUTH_DATA_SIZE)
    {
        return -ERROR_AUTH_DATA_SIZE;
    }

    int i = 0;
    for (; i < size; i++)
    {
        int temp = auth_data_node[i].as_int();
        auth_data.push_back(temp);
    }


    return 0;
}

int CJsonOpt::JsonParsePushMsg(string &dev_id)
{
    if (!(VerifyJsonField(JK_PARAM) && VerifyJsonField(JK_DEV_ID)))
    {
        return -1;
    }

    dev_id = in_[JK_DEV_ID].as_string();
    out_ = in_[JK_PARAM].as_node();

    return 0;
}

bool CJsonOpt::JsonJoinCommon(string method, int ret)
{
    out_.push_back(JSONNode(JK_SEND_CNT, send_cnt_));
    out_.push_back(JSONNode(JK_METHOD, method));

    JSONNode result_node(JSON_NODE);
    result_node.set_name(JK_ERROR);
    result_node.push_back(JSONNode(JK_ERRORCODE, ret));
    out_.push_back(result_node);

    return true;
}

string CJsonOpt::JsonJoinBeaconRes(int ret)
{
    JsonJoinCommon(METHOD_KEEP_ONLINE, ret);

    return out_.write();
}

string CJsonOpt::JsonJoinDeviceStateNotice(string dev_id, int state, string time)
{
    out_.push_back(JSONNode(JK_METHOD, METHOD_DEVICE_STATE_NOTICE));

    JSONNode param_node;
    param_node.set_name(JK_PARAM);
    param_node.push_back(JSONNode(JK_DEV_ID, dev_id));
    param_node.push_back(JSONNode(JK_STATE, state));
    param_node.push_back(JSONNode(JK_TIME, time));

    out_.push_back(param_node);
    
    return out_.write();
}

string CJsonOpt::JsonJoinLoginRes(int ret)
{
    time_t now;
    struct tm *local_time;
    char buffer[16] = {0};

    JsonJoinCommon(METHOD_DEVICE_LOGIN, ret);

    time(&now);
    local_time = localtime(&now);
    sprintf(buffer, "%04d%02d%02d%02d%02d%02d", local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
                    local_time->tm_hour, local_time->tm_min, local_time->tm_sec);


    string time_str(buffer);

    JSONNode result_node;
    result_node.set_name(JK_RESULT);
    result_node.push_back(JSONNode(JK_TM_STR, time_str));
    
    out_.push_back(result_node);

    return out_.write();
}

string CJsonOpt::JsonJoinPushMsgToDev()
{
    return out_.write();
}

string CJsonOpt::JsonJoinPushMsgRes(int send_cnt, int result)
{
    out_.push_back(JSONNode(JK_SEND_CNT, send_cnt));
    out_.push_back(JSONNode(JK_METHOD, METHOD_ON_PUSH_MSG));
    out_.push_back(JSONNode(JK_RESULT, result));

    return out_.write();
}

// for test
string CJsonOpt::JsonJoinUserLogin()
{
    out_.push_back(JSONNode("mid", "123"));
    out_.push_back(JSONNode("method", "login"));

    JSONNode param_node;
    param_node.set_name("param");
    param_node.push_back(JSONNode("user", "jovision_admin"));
    param_node.push_back(JSONNode("pwd", "0jeovHYiZ02r"));
    param_node.push_back(JSONNode("phone_id", "12345678"));

    out_.push_back(param_node);

    return out_.write();
}

