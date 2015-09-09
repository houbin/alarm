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
    int JsonParseBeacon(string &dev_id);
    int JsonParseLogin(string &dev_id, string &auth_data);

public:
    bool JsonJoinCommon(string method, int ret = 0);
    string JsonJoinBeaconRes(int ret = 0);
    string JsonJoinDeviceStateNotice(string dev_id, int state, string time);
    string JsonJoinLoginRes(int ret = 0);

    bool RestructJsonStringToDev(std::string json_in_string, std::string &json_out_string, int &send_cnt, std::string &dev_id);
    string JsonJoinPushMsgRes(int send_cnt, int result);
    

    //for test
    string JsonJoinUserLogin();

private:
    bool JsonParseField(const string &field);

private:
	std::string json_string_;
	JSONNode in_;
	JSONNode out_;

	int send_cnt_;
    string method_;
};

#endif /* JSON_OPT_H_ */

