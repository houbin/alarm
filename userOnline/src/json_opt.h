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
    bool VerifyJsonField(const string &field);

    bool JsonParseCommon();
    int JsonParseBeacon(string &session_id);

public:
    bool JsonJoinCommon(string method, int ret = 0);
    string JsonJoinBeaconRes(int ret = 0);

    bool RestructJsonStringToRemote(string json_in_string, string &json_out_string, int &mid, string &guid);

    string JsonJoinPushMsgRes(int mid, int result);

    //for test
    //string JsonJoinUserLogin();

private:
    bool JsonParseField(const string &field);

private:
	std::string json_string_;
	JSONNode in_;
	JSONNode out_;

	int mid_;
    string method_;
};

#endif /* JSON_OPT_H_ */

