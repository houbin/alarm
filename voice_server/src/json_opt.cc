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
	mid_ = 0;
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
    if (method_ == "")
    {
        return -1;
    }

    method = method_;
    return 0;
}

int CJsonOpt::GetMid()
{
    return mid_;
}

bool CJsonOpt::VerifyJsonField(const string &field)
{
    return VerifyJsonField(in_, field);
}

bool CJsonOpt::VerifyJsonField(JSONNode &node, const string &field)
{
    if (node.find(field) != node.end())
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

    if (!(VerifyJsonField(JK_MESSAGE_ID) && VerifyJsonField(JK_METHOD)))
    {
        return false;
    }

    mid_ = in_[JK_MESSAGE_ID].as_int();
    method_ = in_[JK_METHOD].as_string();

    return true;
}

int CJsonOpt::JsonParseGetVoiceServerAddr(string &client_ip, string &dev_ip)
{
    if (!VerifyJsonField(JK_PARAM))
    {
        return -ERROR_NO_PARAM;
    }

    JSONNode param_node;
    param_node = in_[JK_PARAM].as_node();
    param_node.preparse();

    if (!VerifyJsonField(param_node, JK_CLIENT_IP) || !VerifyJsonField(param_node, JK_DEV_IP))
    {
        return -ERROR_NO_CLIENT_OR_DEV_IP;
    }

    client_ip = param_node[JK_CLIENT_IP].as_string();
    dev_ip = param_node[JK_DEV_IP].as_string();

    return 0;
}

bool CJsonOpt::JsonJoinCommon(string method, int ret)
{
    out_.push_back(JSONNode(JK_MESSAGE_ID, mid_));
    out_.push_back(JSONNode(JK_METHOD, method));
    out_.push_back(JSONNode(JK_RESULT, ret));

    return true;
}

string CJsonOpt::JsonJoinGetVoiceServerAddrResp(int ret, string &server_ip, int server_port)
{
    JsonJoinCommon(METHOD_ON_GET_VOICESERVER_ADDR, ret);

    JSONNode param_node;
    param_node.set_name(JK_PARAM);
    param_node.push_back(JSONNode(JK_IP, server_ip));
    param_node.push_back(JSONNode(JK_PORT, server_port));
    out_.push_back(param_node);

    return out_.write();
}


