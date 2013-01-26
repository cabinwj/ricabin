#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include "hc_os.h"
#include <string>

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN       16       /* for IPv4 dotted-decimal */
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN      46       /* for IPv6 hex string */
#endif

//! @Internet域地址的封装类
class Address
{
public:
    /// Map Port type to system-dependant port type
    typedef in_port_t Port;

//public:
//    /// Can be used when no IP-address should be specified
//    static const char ANY_ADDRESS_IP[INET_ADDRSTRLEN + 1];

public:
    explicit Address(const char* ip = NULL, Port port = 0);

    explicit Address(uint32_t ip, Port port = 0);

    /** Construct an address from a classic sockaddr_in.
        * @b No lookup is made.
        * @param address Address that should be copied.
        */
    Address(const sockaddr_in& address);

    Address(const Address& address);

    /** Cast the Address into sockaddr_in.
        * The first found IP is used to create the sockaddr_in.
        * @return Socket-Address in classic format.
        * @exception Exception An exception is thrown when there are no valid IPs.
        */
    operator sockaddr_in() const;

    /** Assignment of sockaddr_in
        * @param address Address that should be copied.
        */
    void operator=(const sockaddr_in& address);

    void operator=(const Address& address);

    bool operator==(const Address& address);

    // net-order
    Port get_net_port() const { return port_; }

    // host-order
    Port get_host_port() const;

    //! ip 点分十进制表示法
    /// ASCII strings (what humans prefer to use)
    /// a dotted-decimal string (e.g., "202.96.209.133") 
    std::string get_string_ip() const;

    // net-order
    inline unsigned int get_net_ip() const { return sin_address_.s_addr; }  

    // host-order
    unsigned int get_host_ip() const;

    void clear();

private:
    /**
    struct in_addr{
        in_addr_t      s_addr;        /// 32-bit IPv4 address network byte ordered
    }

    struct sockaddr_in{
        uint8_t        sin_len;       /// length of structure (16)
        sa_family_t    sin_family;    /// AF_INET
        in_port_t      sin_port;      /// 16-bit TCP or UDP port number network byte ordered
        struct in_addr sin_addr;      /// 32-bit IPv4 address network byte ordered
        char           sin_zero[8];   /// unused
    }
    */
    //! 网络地址
    /// The ipnumber in netbyteorder
    in_addr sin_address_;

    //! 端口(网络字节)
    /// The portnumber in netbyteorder
    Port port_;
};

#endif
