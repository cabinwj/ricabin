#include "address.h"
#include "hc_log.h"
#include "hc_stack_trace.h"

#include <string.h>

//const char Address::ANY_ADDRESS_IP[INET_ADDRSTRLEN + 1] = {0,};

Address::Address(const char* ip, Port port)
{
    STACK_TRACE_LOG();

    if (NULL == ip)
    {
        sin_address_.s_addr = htonl(0);
        port_ = htons(port);
        return;
    }

    in_addr address;
    bzero(&address, sizeof(address));
    // Try to convert ip-string to binary ip.
#ifndef WIN32
    if (0 == inet_aton(ip, &address))
    {
        LOG(ERROR)("Address::Address() socket error, convert ip-string to binary ip, errno:%d", error_no());
        return;
    }
    // IP could be converted to binary ip
    sin_address_ = address;
#else
    sin_address_.s_addr = inet_addr(ip);
#endif

    port_ = htons(port);
}

Address::Address(uint32_t ip, Port port)
{
    //STACK_TRACE_LOG();

    sin_address_.s_addr = htonl(ip);
    port_ = htons(port);
}

Address::Address(const sockaddr_in& address)
{
    //STACK_TRACE_LOG();

    sin_address_ = address.sin_addr;
    port_ = address.sin_port;
}

Address::Address(const Address& address)
{
    //STACK_TRACE_LOG();

    sin_address_ = address.sin_address_;
    port_ = address.port_;
}

Address::operator sockaddr_in() const
{
    //STACK_TRACE_LOG();

    sockaddr_in address;
    memset(&address, 0, sizeof(sockaddr_in));
    address.sin_family = PF_INET;
    address.sin_port = port_;
    address.sin_addr = sin_address_;
    return address;
}

void Address::operator=(const sockaddr_in& address)
{
    //STACK_TRACE_LOG();

    sin_address_ = address.sin_addr;
    port_ = address.sin_port;
}

void Address::operator=(const Address& address)
{
    //STACK_TRACE_LOG();

    sin_address_ = address.sin_address_;
    port_ = address.port_;
}

bool Address::operator==(const Address& address)
{
    //STACK_TRACE_LOG();

    return ((sin_address_.s_addr == address.sin_address_.s_addr) && (port_ == address.port_));
}

Address::Port Address::host_port() const
{
    //STACK_TRACE_LOG();

    return ntohs(port_);
}

std::string Address::dot_decimal_ip() const
{
    //STACK_TRACE_LOG();

#ifndef WIN32
    char str_ip[16];
    std::string strings_ip = (NULL == inet_ntop(AF_INET, &sin_address_, str_ip, sizeof(str_ip)))? "0.0.0.0" : str_ip;
    return strings_ip;
#else
    std::string strings_ip(inet_ntoa(sin_address_), 16);
    return strings_ip;
#endif
}

unsigned int Address::host_ip() const
{
    //STACK_TRACE_LOG();

    return ntohl(sin_address_.s_addr);
}

void Address::clear()
{
    //STACK_TRACE_LOG();

    sin_address_.s_addr = 0;
    port_ = 0;
}
