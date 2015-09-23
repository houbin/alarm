/**
 * @created:	   		2012/05/18
 *
 * @file				socket_wrapper.h
 *
 * @author  			wei yao <yaocoder@gmail.com>
 *
 * @version 			1.0
 *
 * @LICENSE
 *
 * @brief				对socket操作的封装处理
 *
 */
#ifndef SOCKET_WRAPPER_H__
#define SOCKET_WRAPPER_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

#define		BACKLOG     50

namespace SocketOperate
{
	/**
	  * @brief		设置socket为非阻塞
	  * @param [in]
	  * @return If successful true is returned, if not false is returned.
	 */
	static inline bool SetSocketNoBlock(const int sock_fd)
	{
		int flags = fcntl(sock_fd, F_GETFL, 0);

		if(-1 == flags) {
			return false;
		}

		flags |= O_NONBLOCK;

		if(-1 == fcntl(sock_fd, F_SETFL, flags)) {
			return false;
		}

		return true;
	}

	/**
	  * @brief			关闭socket
	  * @param [in]
	  * @return If successful true is returned, if not false is returned.
	 */
	static inline bool	CloseSocket(const int sock_fd)
	{
		if(sock_fd  < 0)
			return true;

		if(shutdown(sock_fd, SHUT_WR) < 0)
		{
			//log
		}

		if(close(sock_fd) < 0)
			return false;
		return true;
	}

	static inline bool WriteSfd(const int sfd, const char* buf, const int buf_len)
	{
		int msg_len = buf_len, send_ret = 0;
		const char *send_data = buf;
		time_t first_time = 0;

		while (msg_len > 0)
		{
			send_ret = send(sfd, send_data, msg_len, MSG_NOSIGNAL);
			//发送成功，则把相应标志位设置为0
			if (send_ret >= msg_len)
			{
				break;
			}
			//没有发送完，则继续发送
			else if (send_ret > 0)
			{
				msg_len -= send_ret;
				send_data += send_ret;
				continue;
			}
			//发送失败
			else
			{
				//系统调用失败或发送缓冲区满需要继续发送该消息
				if ((errno == EINTR) || (errno == EAGAIN))
				{
					if (first_time == 0)
					{
						first_time = time(NULL);
					}
					else
					{
						if ((time(NULL) - first_time) > 3)
						{
							return false;
						}
					}
					continue;
				}
			}

			return false;
		}

		return true;
	}

	/**
	  * @brief			通过sockaddr_in结构信息得到主机port
	  * @param [in]
	  * @return			int 主机类型 port
	 */
	static inline int GetHostPort(const sockaddr_in sock_addr_in)
	{
        return ntohs(sock_addr_in.sin_port);
	}

	/**
	  * @brief			通过sockaddr_in结构信息得到主机ip
	  * @param [in]
	  * @return			string 主机类型 ip地址
	 */
	static inline std::string GetHostAddr(const sockaddr_in* sock_addr_in)
	{
		char ip_buf[32];
		std::string stradd = ::inet_ntop(AF_INET, &sock_addr_in->sin_addr, ip_buf, sizeof(ip_buf));
        return stradd;
	}

};
#endif
