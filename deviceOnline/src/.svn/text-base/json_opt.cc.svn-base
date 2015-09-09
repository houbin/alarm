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

int CJsonOpt::JsonParseLogin(string &dev_id, string &auth_data)
{
    if (!VerifyJsonField(JK_PARAM))
    {
        return -1;
    }

    JSONNode param_node = in_[JK_PARAM].as_node();
    dev_id = param_node[JK_DEV_ID].as_string();
    auth_data = param_node[JK_AUTH_DATA].as_string();

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

bool CJsonOpt::RestructJsonStringToDev(string json_in_string, string &json_out_string, int &send_cnt, string &dev_id)
{
    try
    {
        out_ = libjson::parse(json_in_string.c_str());
    }
    catch(...)
    {
        LOG4CXX_WARN(g_logger, "CJsonOpt::RestructJsonStringToDev:parse failed. string = " << json_in_string);
        return false;
    }

    if (!(VerifyJsonField(JK_SEND_CNT) && (VerifyJsonField(JK_DEV_ID))))
    {
        return false;
    }

    send_cnt = out_[JK_SEND_CNT].as_int();
    dev_id = out_[JK_DEV_ID].as_string();
    out_.pop_back(JK_SEND_CNT);
    out_.pop_back(JK_METHOD);
    out_.pop_back(JK_DEV_ID);

    return true;
}

string CJsonOpt::JsonJoinPushMsgRes(int send_cnt, int result)
{
    out_.push_back(JSONNode(JK_SEND_CNT, send_cnt));
    out_.push_back(JSONNode(JK_METHOD, METHOD_ON_PUSH_MSG));
    out_.push_back(JSONNode(JK_RESULT, result));

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
    JsonJoinCommon(METHOD_ON_DEVICE_LOGIN, ret);

    time_t now;
    time(&now);
    string time_str(ctime(&now));

    JSONNode result_node;
    result_node.set_name(JK_RESULT);
    result_node.push_back(JSONNode(JK_TM_STR, time_str));
    
    out_.push_back(result_node);

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

