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

class CJsonOpt;
class CRedisOpt;
class CLogicOpt
{
public:
	explicit CLogicOpt(conn* c);
	virtual ~CLogicOpt();

	void StartLogicOpt(const std::string& message);

    static int SetDeviceFdCache(std::string dev_id, int fd);
    static int GetDeviceFdFromCache(std::string dev_id, int &fd);
    static int RemoveDeviceFdFromCache(std::string dev_id);

    static int SetDeviceAddrCache(std::string dev_id, std::string public_addr, std::string private_addr);
    static int GetDeviceAddrFromCache(std::string dev_id, std::string &public_addr, std::string &private_addr);
    static int RemoveDeviceAddrFromCache(std::string dev_id);

    static int RemoveDeviceFromCache(std::string dev_id);
private:

    int DeviceLogin();
	int DeviceBeacon();

private:

	void SendToClient();

private:

	CJsonOpt*	jsonOpt_ptr_;

	int 		result_;
	conn* 		conn_;
	std::string responseToClient_;

    bool is_online_;
	std::string dev_id_;
};

#endif /* MESSAGE_OPT_H_ */
