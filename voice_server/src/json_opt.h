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
#include <deque>

using namespace std;

class CJsonOpt
{
public:
	CJsonOpt();
	virtual ~CJsonOpt();

	void setJsonString(const std::string &msg);
    int GetMid();
    int GetMethod(string &method);
    bool VerifyJsonField(const string &field);
    bool VerifyJsonField(JSONNode &node, const string &field);

    bool JsonParseCommon();
    int JsonParseGetVoiceServerAddr(string &client_ip, string &dev_ip);

public:
    bool JsonJoinCommon(string method, int ret = 0);
    string JsonJoinGetVoiceServerAddrResp(int ret, string &server_ip, int server_port);

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

