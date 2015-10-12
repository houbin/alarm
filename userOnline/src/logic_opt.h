/*
 * message_opt.h
 *
 *  Created on: Mar 18, 2013
 *      Author: yaowei
 */

#ifndef MESSAGE_OPT_H_
#define MESSAGE_OPT_H_

#include "defines.h"
#include "redis_opt.h"
#include "../../public/message.h"
#include "../../public/user_interface_defines.h"

using namespace std;

class CJsonOpt;
class CRedisOpt;
class CLogicOpt
{
public:
    explicit CLogicOpt(conn* c);
    virtual ~CLogicOpt();

    void StartLogicOpt(const std::string& message);
    int CheckSessionId(string guid);

    static int SetGuidFdCache(std::string guid, int fd);
    static int GetGuidFdFromCache(std::string guid, int &fd);
    static int RemoveGuidFdFromCache(std::string guid);
    
private:

    int UserBeacon();
    int HandlePushMsgResp();

private:

    CJsonOpt*    jsonOpt_ptr_;

    int         result_;
    conn*         conn_;
    std::string responseToClient_;
};

#endif /* MESSAGE_OPT_H_ */
