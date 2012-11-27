#ifndef _COMMON_OS_H_
#define _COMMON_OS_H_

#include "common_types.h"

#include <sys/types.h>
#include <stdlib.h>
#include <new>
#include <time.h>


#ifdef WIN32    // for windows
#include <winsock2.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/timeb.h>

typedef SOCKET Descriptor;  // unsigned int
#define bzero(a,b) (memset((a),0,(b)))

#define F_OK 0
#define W_OK 2
#define R_OK 4

// shutdown()
#define SHUT_RD   SD_RECEIVE       ///< No more data can be read.
#define SHUT_WR   SD_SEND          ///< No more data can be written.
#define SHUT_RDWR SD_BOTH          ///< No data transport in both directions.

#define SOL_IP  IPPROTO_IP
#define SOL_TCP IPPROTO_TCP

//! 创建文件夹
inline int mkdir(const char *dirname, int)
{
    return _mkdir(dirname);
}

//! 检测文件
inline int access(const char *path, int mode)
{
    return _access(path, mode);
}

//! 关闭socket
inline int close(Descriptor socket)
{
    return closesocket(socket);
}

inline int gettimeofday(struct timeval* tv, void* tzp)
{
    struct _timeb tmb;

    if (NULL == tv)
    {
        return -1;
    }

    /* XXXX
        * _ftime is not the greatest interface here; GetSystemTimeAsFileTime
        * would give us better resolution, whereas something cobbled together
        * with GetTickCount could maybe give us monotonic behavior.
        *
        * Either way, I think this value might be skewed to ignore the
        * timezone, and just return local time.  That's not so good.
        */
    _ftime_s(&tmb);

    tv->tv_sec = (long) tmb.time;
    tv->tv_usec = ((int) tmb.millitm) * 1000;

    return 0;
}



#define SYS_EINTR               WSAEINTR           //10004 同标准C
#define SYS_EBADF               WSAEBADF           //10009 同标准C
#define SYS_EACCES              WSAEACCES          //10013 同标准C
#define SYS_EFAULT              WSAEFAULT          //10014 同标准C
#define SYS_EINVAL              WSAEINVAL          //10022 同标准C
#define SYS_EMFILE              WSAEMFILE          //10024 同标准C


#define SYS_EAGAIN              WSAEWOULDBLOCK     //<! 无数据可读或可写
#define SYS_EWOULDBLOCK         WSAEWOULDBLOCK     //10035 同BSD

#define SYS_EINPROGRESS         WSAEINPROGRESS     //10036
                                                        //当一个阻塞函数正在进行时，
                                                        //调用任何Windows Sockets API函数均返回此错误
#define SYS_EALREADY            WSAEALREADY        //10037
#define SYS_ENOTSOCK            WSAENOTSOCK        //10038 同BSD
#define SYS_EDESTADDRREQ        WSAEDESTADDRREQ    //10039 同BSD
#define SYS_EMSGSIZE            WSAEMSGSIZE        //10040 同BSD
#define SYS_EPROTOTYPE          WSAEPROTOTYPE      //10041 同BSD
#define SYS_ENOPROTOOPT         WSAENOPROTOOPT     //10042 同BSD
#define SYS_EPROTONOSUPPORT     WSAEPROTONOSUPPORT //10043 同BSD
#define SYS_ESOCKTNOSUPPORT     WSAESOCKTNOSUPPORT //10044 同BSD
#define SYS_EOPNOTSUPP          WSAEOPNOTSUPP      //10045 同BSD
#define SYS_EPFNOSUPPORT        WSAEPFNOSUPPORT    //10046 同BSD
#define SYS_EAFNOSUPPORT        WSAEAFNOSUPPORT    //10047 同BSD
#define SYS_EADDRINUSE          WSAEADDRINUSE      //10048 同BSD
#define SYS_EADDRNOTAVAIL       WSAEADDRNOTAVAIL   //10049 同BSD
#define SYS_ENETDOWN            WSAENETDOWN        //10050 同BSD
                                                           //任何时候只要Windows Sockets实现检测到网络子系统失败，
                                                           //它就报告此错误。
#define SYS_ENETUNREACH         WSAENETUNREACH     //10051 同BSD
#define SYS_ENETRESET           WSAENETRESET       //10052 同BSD
#define SYS_ECONNABORTED        WSAECONNABORTED    //10053 同BSD accept返回前连接夭折
                                                        //[ECONNABORTED] A connection arrived, but it was closed while waiting on the listen queue.
                                                        // 该错误被描述为“software caused connection abort”， 即“软件引起的连接中止”。
                                                        // 原因在于当服务和客户进程在完成用于 TCP 连接的“三次握手”后，客户 TCP 却发送了一个 RST （复位）分节，
                                                        // 在服务进程看来，就在该连接已由 TCP 排队，等着服务进程调用 accept 的时候 RST 却到达了。
#define SYS_ECONNRESET          WSAECONNRESET      //10054 同BSD
#define SYS_ENOBUFS             WSAENOBUFS         //10055 同BSD
#define SYS_EISCONN             WSAEISCONN         //10056 同BSD
#define SYS_ENOTCONN            WSAENOTCONN        //10057 同BSD
#define SYS_ESHUTDOWN           WSAESHUTDOWN       //10058 同BSD
#define SYS_ETOOMANYREFS        WSAETOOMANYREFS    //10059 同BSD
#define SYS_ETIMEDOUT           WSAETIMEDOUT       //10060 同BSD
#define SYS_ECONNREFUSED        WSAECONNREFUSED    //10061 同BSD
#define SYS_ELOOP               WSAELOOP           //10062 同BSD
#define SYS_ENAMETOOLONG        WSAENAMETOOLONG    //10063 同BSD
#define SYS_EHOSTDOWN           WSAEHOSTDOWN       //10064 同BSD
#define SYS_EHOSTUNREACH        WSAEHOSTUNREACH    //10065 同BSD

#define SYS_HOST_NOT_FOUND      WSAHOST_NOT_FOUND  //11001 同BSD
#define SYS_TRY_AGAIN           WSATRY_AGAIN       //11002 同BSD
#define SYS_NO_RECOVERY         WSANO_RECOVERY     //11003 同BSD
#define SYS_NO_DATA             WSANO_DATA         //11004 同BSD



#else             // for linux

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>


#define SYS_EINTR               EINTR           //10004 同标准C
#define SYS_EBADF               EBADF           //10009 同标准C
#define SYS_EACCES              EACCES          //10013 同标准C
#define SYS_EFAULT              EFAULT          //10014 同标准C
#define SYS_EINVAL              EINVAL          //10022 同标准C
#define SYS_EMFILE              EMFILE          //10024 同标准C


#define SYS_EAGAIN              EWOULDBLOCK     //<! 无数据可读或可写
#define SYS_EWOULDBLOCK         EWOULDBLOCK     //10035 同BSD

#define SYS_EINPROGRESS         EINPROGRESS     //10036
                                                        //当一个阻塞函数正在进行时，
                                                        //调用任何Windows Sockets API函数均返回此错误
#define SYS_EALREADY            EALREADY        //10037
#define SYS_ENOTSOCK            ENOTSOCK        //10038 同BSD
#define SYS_EDESTADDRREQ        EDESTADDRREQ    //10039 同BSD
#define SYS_EMSGSIZE            EMSGSIZE        //10040 同BSD
#define SYS_EPROTOTYPE          EPROTOTYPE      //10041 同BSD
#define SYS_ENOPROTOOPT         ENOPROTOOPT     //10042 同BSD
#define SYS_EPROTONOSUPPORT     EPROTONOSUPPORT //10043 同BSD
#define SYS_ESOCKTNOSUPPORT     ESOCKTNOSUPPORT //10044 同BSD
#define SYS_EOPNOTSUPP          EOPNOTSUPP      //10045 同BSD
#define SYS_EPFNOSUPPORT        EPFNOSUPPORT    //10046 同BSD
#define SYS_EAFNOSUPPORT        EAFNOSUPPORT    //10047 同BSD
#define SYS_EADDRINUSE          EADDRINUSE      //10048 同BSD
#define SYS_EADDRNOTAVAIL       EADDRNOTAVAIL   //10049 同BSD
#define SYS_ENETDOWN            ENETDOWN        //10050 同BSD
                                                        //任何时候只要Windows Sockets实现检测到网络子系统失败，
                                                        //它就报告此错误。
#define SYS_ENETUNREACH         ENETUNREACH     //10051 同BSD
#define SYS_ENETRESET           ENETRESET       //10052 同BSD
#define SYS_ECONNABORTED        ECONNABORTED    //10053 同BSD accept返回前连接夭折
                                                        //[ECONNABORTED] A connection arrived, but it was closed while waiting on the listen queue.
                                                        // 该错误被描述为“software caused connection abort”， 即“软件引起的连接中止”。
                                                        // 原因在于当服务和客户进程在完成用于 TCP 连接的“三次握手”后，客户 TCP 却发送了一个 RST （复位）分节，
                                                        // 在服务进程看来，就在该连接已由 TCP 排队，等着服务进程调用 accept 的时候 RST 却到达了。
#define SYS_ECONNRESET          ECONNRESET      //10054 同BSD
#define SYS_ENOBUFS             ENOBUFS         //10055 同BSD
#define SYS_EISCONN             EISCONN         //10056 同BSD
#define SYS_ENOTCONN            ENOTCONN        //10057 同BSD
#define SYS_ESHUTDOWN           ESHUTDOWN       //10058 同BSD
#define SYS_ETOOMANYREFS        ETOOMANYREFS    //10059 同BSD
#define SYS_ETIMEDOUT           ETIMEDOUT       //10060 同BSD
#define SYS_ECONNREFUSED        ECONNREFUSED    //10061 同BSD
#define SYS_ELOOP               ELOOP           //10062 同BSD
#define SYS_ENAMETOOLONG        ENAMETOOLONG    //10063 同BSD
#define SYS_EHOSTDOWN           EHOSTDOWN       //10064 同BSD
#define SYS_EHOSTUNREACH        EHOSTUNREACH    //10065 同BSD

#define SYS_HOST_NOT_FOUND      HOST_NOT_FOUND  //11001 同BSD
#define SYS_TRY_AGAIN           TRY_AGAIN       //11002 同BSD
#define SYS_NO_RECOVERY         NO_RECOVERY     //11003 同BSD
#define SYS_NO_DATA             NO_DATA         //11004 同BSD

#define INVALID_SOCKET -1   //无效socket
#define SOCKET_ERROR -1

typedef int Descriptor;

// from winsock.h winsock2.h
/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0


#endif

//! 睡眠，改为毫秒
inline void sleep_ms(unsigned long _ms)
{
#ifndef WIN32
    struct timespec req;
    req.tv_sec = (int)_ms/1000;
    req.tv_nsec = (_ms - req.tv_sec*1000)*1000000L;
    nanosleep(&req, NULL);
#else
    Sleep(_ms);
#endif
}

//! 错误码
inline int error_no()
{
#ifndef WIN32
    return errno;
#else
    return WSAGetLastError();
#endif
}

typedef int Family;
typedef int Type;
typedef int Protocol;

/// How to shutdown the connection.
typedef enum shutdown_t
{
    READ       = SHUT_RD,       ///< No more data can be read.
    WRITE      = SHUT_WR,       ///< No more data can be written.
    READ_WRITE = SHUT_RDWR      ///< No data transport in both directions.
}shutdown_t;

#endif // _COMMON_OS_H_

