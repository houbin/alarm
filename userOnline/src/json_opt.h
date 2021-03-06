/*
 * json_opt.h
 *
 *  Created on: Mar 6, 2013
 *      Author: yaowei
 */

#ifndef JSON_OPT_H_
#define JSON_OPT_H_

#include "libjson/libjson.h"
#include "libjson/_internal/Source/JSONNode.h"
#include "../../public/user_interface_defines.h"
#include "defines.h"

using namespace std;

class CJsonOpt
{
public:
    CJsonOpt();
    virtual ~CJsonOpt();

    void setJsonString(const std::string &msg);
    int GetMethod(string &method);
    int GetMid();
    bool VerifyJsonField(const string &field);

    bool JsonParseCommon();
    int JsonParseBeacon(string &session_id);
    int JsonParsePushMsg(string &guid);

    // 推送消息的回复消息
    int JsonParsePushMsgResp(int &ret);

public:
    bool JsonJoinCommon(string method, int ret = 0);
    string JsonJoinBeaconRes(int ret = 0);

    string JsonJoinPushMsgToClient(int push_mid);
    string JsonJoinPushMsgRes(int mid, int result);


    string JsonJoinUserLogout(int mid, string session);

    //for test
    //string JsonJoinUserLogin();

private:
    bool JsonParseField(const string &field);

private:
    std::string json_string_;
    JSONNode in_;
    JSONNode out_;

    unsigned int mid_;
    string method_;
};

#endif /* JSON_OPT_H_ */

