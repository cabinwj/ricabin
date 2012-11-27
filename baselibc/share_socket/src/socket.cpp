#include "socket.h"

Socket::Socket() : descriptor_(INVALID_SOCKET)
{
}

Socket::Socket(Descriptor sfd) : descriptor_(sfd)
{
}

Socket::Socket(Family family, Type type, Protocol protocol)
{
    open(family, type, protocol);
}

Socket::~Socket()
{
    if (is_opened())
    {
        ::close(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

int Socket::open(Family family, Type type, Protocol protocol)
{
    if (is_opened())
    {
        LOG(ERROR)("Socket::open() socket error, socket already opened");
        return -1;
    }

    descriptor_ = socket(family, type, protocol);

    if ((INVALID_SOCKET == descriptor_) || (0 > descriptor_))
    {
        LOG(ERROR)("Socket::open() socket error, open socket error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

void Socket::close()
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::close() socket error, socket not opened");
        return;
    }

    ::close(descriptor_);

    descriptor_ = INVALID_SOCKET;
}

int Socket::bind(const sockaddr_in& addr)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::bind() socket error, socket not opened");
        return -1;
    }

    const sockaddr* socketAddress = reinterpret_cast<const sockaddr*>(&addr);

    int error = ::bind(descriptor_, socketAddress, sizeof(addr));

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::bind() socket error, bind socket error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

int Socket::bind(const Address& addr)
{
    return bind(static_cast<const sockaddr_in>(addr));
}

int Socket::connect(const sockaddr_in& addr)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::connect() socket error, socket not opened");
        return -1;
    }

    const sockaddr* socketAddress = reinterpret_cast<const sockaddr*>(&addr);

    int error = ::connect(descriptor_, socketAddress, sizeof(addr));

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::connect() socket error, connect socket error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

int Socket::connect(const Address& addr)
{
    return connect(static_cast<const sockaddr_in>(addr));
}

int Socket::send(const char* data, size_t size, int flags)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::send() socket error, socket not opened");
        return -1;
    }

    int bytes = -1;
    do{

        bytes = ::send(descriptor_, data, size, flags);

    }while( bytes<0 && SYS_EINTR == error_no() );

    if (-1 == bytes)
    {
        LOG(ERROR)("Socket::send() socket error, send error, errno:%d", error_no());
    }

    return bytes;
}

int Socket::sendto(const sockaddr_in& addr, const char* data, size_t size, int flags)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::sendto() socket error, socket not opened");
        return -1;
    }

    const sockaddr* socketAddress = reinterpret_cast<const sockaddr*>(&addr);

    int bytes = -1;
    do{

        bytes = ::sendto(descriptor_, data, size, flags, socketAddress, sizeof(addr));

    }while( bytes<0 && SYS_EINTR == error_no() );

    if (-1 == bytes)
    {
        LOG(ERROR)("Socket::sendto() socket error, sendto error, errno:%d", error_no());
    }

    return bytes;
}

int Socket::sendto(const Address& addr, const char* data, size_t size, int flags)
{
    return sendto(sockaddr_in(addr),data,size,flags);
}

int Socket::recv(char* data, size_t size, int flags)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::recv() socket error, socket not opened");
        return -1;
    }

    int bytes = -1;
    do{

        bytes = ::recv(descriptor_, data, size, flags);

    }while( bytes<0 && SYS_EINTR == error_no() );

    if (-1 == bytes)
    {
        LOG(ERROR)("Socket::recv() socket error, recv error, errno:%d", error_no());
    }

    return bytes;
}

int Socket::recvfrom(sockaddr_in& addr, char* data, size_t size, int flags)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::recvfrom() socket error, socket not opened");
        return -1;
    }

    socklen_t addrLen = sizeof(addr);
    sockaddr* socketAddress = reinterpret_cast<sockaddr*>(&addr);

    int bytes = -1;
    do{

        bytes = ::recvfrom(descriptor_, data, size, flags, socketAddress, &addrLen);

    }while( bytes<0 && SYS_EINTR == error_no() );

    if (-1 == bytes)
    {
        LOG(ERROR)("Socket::recvfrom() socket error, recvfrom error, errno:%d", error_no());
    }

    return bytes;
}

int Socket::recvfrom(Address& addr, char* data, size_t size, int flags)
{
    sockaddr_in saddr;
    int bytes = recvfrom(saddr,data,size,flags);
    addr = saddr;
    return bytes;
}

int Socket::listen(int queuelen)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::listen() socket error, socket not opened");
        return -1;
    }

    int error = ::listen(descriptor_, queuelen);

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::listen() socket error, listen error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

Descriptor Socket::accept(sockaddr_in& addr)
{
    Descriptor socketfd = INVALID_SOCKET;

    if (!is_opened())
    {
        LOG(ERROR)("Socket::accept() socket error, socket not opened");
        return socketfd;
    }

    socklen_t addrLen = sizeof(addr);
    sockaddr* socketAddress = reinterpret_cast<sockaddr*>(&addr);

    do{

        socketfd = ::accept(descriptor_, socketAddress, &addrLen);

    }while(((INVALID_SOCKET == socketfd) || (0 > socketfd)) &&(SYS_ECONNABORTED == error_no() || SYS_EINTR == error_no() ));

    if ((INVALID_SOCKET == socketfd) || (0 > socketfd))
    {
        LOG(ERROR)("Socket::accept() socket error, accept error, errno:%d", error_no());
    }

    return socketfd;
}

Descriptor Socket::accept(Address& addr)
{
    sockaddr_in address = addr;
    Descriptor socket = accept(address);
    addr = address;
    return socket;
}

int Socket::getsockname(sockaddr_in& addr)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::getsockname() socket error, socket not opened");
        return -1;
    }

    socklen_t addrLen = sizeof(addr);
    sockaddr* socketAddress = reinterpret_cast<sockaddr*>(&addr);

    int error = ::getsockname(descriptor_, socketAddress, &addrLen);

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::getsockname() socket error, getsockname error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

int Socket::getsockname(Address& addr)
{
    sockaddr_in address = addr;
    int rc = getsockname(address);
    addr = address;
    return rc;
}

int Socket::getpeername(sockaddr_in& addr)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::getpeername() socket error, socket not opened");
        return -1;
    }

    socklen_t addrLen = sizeof(addr);
    sockaddr* socketAddress = reinterpret_cast<sockaddr*>(&addr);

    int error = ::getpeername(descriptor_, socketAddress, &addrLen);

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::getpeername() socket error, getpeername error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

int Socket::getpeername(Address& addr)
{
    sockaddr_in address = addr;
    int rc = getpeername(address);
    addr = address;
    return rc;
}

int Socket::getsockopt(Descriptor descriptor, int level, int optname, void* optval, socklen_t* optlen)
{
    if ((INVALID_SOCKET == descriptor) || (0 > descriptor) )
    {
        LOG(ERROR)("Socket::getsockopt() socket error, socket not opened");
        return -1;
    }

    int error = ::getsockopt(descriptor, level, optname, static_cast<char*>(optval), optlen);

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::getsockopt() socket error, getsockopt error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

void Socket::setsockopt(Descriptor descriptor, int level, int optname, const void* optval, socklen_t optlen)
{
    if ((INVALID_SOCKET == descriptor) || (0 > descriptor) )
    {
        LOG(ERROR)("Socket::setsockopt() socket error, socket not opened");
        return;
    }

    int error = ::setsockopt(descriptor, level, optname, static_cast<const char*>(optval), optlen);

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::setsockopt() socket error, setsockopt error, errno:%d", error_no());
    }
}

//! 设置为非阻塞模式
void Socket::setnonblock(Descriptor socket)
{
    int error = 0;
#ifndef WIN32
    int val = ::fcntl(socket, F_GETFL, NULL);
    if (val < 0)
    {
        LOG(ERROR)("Socket::setnonblock() socket error, getsocket flags error, errno:%d", error_no());
        return;
    }
    error = ::fcntl(socket, F_SETFL, val | O_NONBLOCK);
    if (-1 == error)
    {
        LOG(ERROR)("Socket::setnonblock() socket error, setnonblock error, errno:%d", error_no());
    }
    //return error > -1 ? 0 : error;
#else
    int mode = 1;
    error = ioctlsocket(socket, FIONBIO, (u_long FAR*)&mode);
    if (0 != error)
    {
        LOG(ERROR)("Socket::setnonblock() socket error, setnonblock error, errno:%d", error_no());
    }
#endif
}

#ifndef WIN32
void Socket::fcntl(Descriptor descriptor,int flag)
{
    if ((INVALID_SOCKET == descriptor) || (0 > descriptor) )
    {
        LOG(ERROR)("Socket::fcntl() socket error, socket not opened");
        return;
    }

    int val = ::fcntl(descriptor, F_GETFL, 0);
    int error = ::fcntl(descriptor, F_SETFL, val | flag);

    if (SOCKET_ERROR == error)
    {
        LOG(ERROR)("Socket::fcntl() socket error, fcntl error, errno:%d", error_no());
    }
}
#endif

int Socket::shutdown(shutdown_t how)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::shutdown() socket error, socket not opened");
        return -1;
    }

    int error = ::shutdown(descriptor_, how);

    if (SOCKET_ERROR == error && SYS_ENOTCONN != error_no())
    {
        LOG(ERROR)("Socket::shutdown() socket error, shutdown error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

bool Socket::wait(bool& read, bool& write, bool& exception, int seconds, int useconds)
{
    if (!is_opened())
    {
        LOG(ERROR)("Socket::wait() socket error, socket not opened");
        return false;
    }

    struct timeval time;
    time.tv_sec  = seconds;
    time.tv_usec = useconds;

    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;

    if (read)
    {
        FD_ZERO(&readfds);
        FD_SET(descriptor_, &readfds);
    }

    if (write)
    {
        FD_ZERO(&writefds);
        FD_SET(descriptor_, &writefds);
    }

    if (exception)
    {
        FD_ZERO(&exceptfds);
        FD_SET(descriptor_, &exceptfds);
    }

    int ret = 0;

    do{

        ret = select(descriptor_+1,(read  ? &readfds : NULL), (write ? &writefds : NULL), (exception ? &exceptfds :NULL), &time);

    }while( ret < 0 && SYS_EINTR == error_no() );

    if ( ret <=0 ) return false;

    if (read)
    {
        read = (FD_ISSET(descriptor_, &readfds));
    }

    if (write)
    {
        write = (FD_ISSET(descriptor_, &writefds));
    }

    if (exception)
    {
        exception = (FD_ISSET(descriptor_, &exceptfds));
    }

    return true;
}

bool Socket::can_read()
{
    bool r = true;
    bool w = false;
    bool e = true;
    return wait(r,w,e) && r;
}

bool Socket::can_write()
{
    bool r = false;
    bool w = true;
    bool e = true;
    return wait(r,w,e) && w;
}

