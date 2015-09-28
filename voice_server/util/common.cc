#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "common.h"

namespace util
{

void safe_close(int fd)
{
    if (fd >= 0)
    {
        close(fd);
    }

    return;
}

bool set_socket_noblock(const int sock_fd)
{
    int flags = fcntl(sock_fd, F_GETFL, 0);

    if(-1 == flags)
    {
        return false;
    }

    flags |= O_NONBLOCK;

    if(-1 == fcntl(sock_fd, F_SETFL, flags))
    {
        return false;
    }

    return true;
}

}

