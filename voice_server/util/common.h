#ifndef UTIL_COMMON_H_
#define UTIL_COMMON_H_

namespace util
{

void safe_close(int fd);

bool set_socket_noblock(const int sock_fd);

}

#endif

