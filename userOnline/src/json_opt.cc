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

    if (!(VerifyJsonField(JK_MESSAGE_ID) && VerifyJsonField(JK_METHOD)))
    {
        return false;
    }

    mid_ = in_[JK_MESSAGE_ID].as_int();
    method_ = in_[JK_METHOD].as_string();

    return true;
}

int CJsonOpt::JsonParseBeacon(string &session_id)
{
    if (!VerifyJsonField(JK_PARAM))
    {
        return -1;
    }

    JSONNode param_node = in_[JK_PARAM].as_node();
    session_id = param_node[JK_SESSION].as_string();

    return 0;
}

bool CJsonOpt::JsonJoinCommon(string method, int ret)
{
    out_.push_back(JSONNode(JK_MESSAGE_ID, mid_));
    out_.push_back(JSONNode(JK_METHOD, method));
    out_.push_back(JSONNode(JK_RESULT, ret));

    return true;
}

string CJsonOpt::JsonJoinBeaconRes(int ret)
{
    JsonJoinCommon(METHOD_KEEP_ONLINE, ret);

    return out_.write();
}

bool CJsonOpt::RestructJsonStringToRemote(string json_in_string, string &json_out_string, int &mid, string &guid)
{
    try
    {
        out_ = libjson::parse(json_in_string.c_str());
    }
    catch(...)
    {
        LOG4CXX_WARN(g_logger, "CJsonOpt::RestructJsonStringToRemote::parse failed. string = " << json_in_string);
        return false;
    }

    if (!(VerifyJsonField(JK_MESSAGE_ID) && (VerifyJsonField(JK_SESSION))))
    {
        return false;
    }

    mid = out_[JK_MESSAGE_ID].as_int();
    guid = out_[JK_SESSION].as_string();
    out_.pop_back(JK_SEND_CNT);
    out_.pop_back(JK_METHOD);
    out_.pop_back(JK_DEV_ID);

    return true;
}

string CJsonOpt::JsonJoinPushMsgRes(int mid, int result)
{
    out_.push_back(JSONNode(JK_MESSAGE_ID, mid));
    out_.push_back(JSONNode(JK_METHOD, METHOD_ON_PUSH_MSG));
    out_.push_back(JSONNode(JK_RESULT, result));

    return out_.write();
}

