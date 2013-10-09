//! @file net_messenger.h
#ifndef _NET_MESSENGER_H_
#define _NET_MESSENGER_H_

#include "hc_types.h"
#include "hc_sync_queue.h"

#include "address.h"
#include "net_package.h"
#include "net_event.h"
#include "net_id_holder.h"


#ifdef WIN32    // for windows
  #include "select_reactor.h"
  //! windows平台下使用select
  typedef select_reactor net_reactor;
#define NETBUF_SIZE 8192
#else            // for linux
  //! linux平台下使用epoll
  #include "epoll_reactor.h"
  typedef epoll_reactor net_reactor;
#define NETBUF_SIZE 16384
#endif

class ipacket_splitter;
class sock_connector;
class sock_acceptor;
class sock_stream;
class net_package;
class net_thread;
class net_event;

//! @class net_messenger
//! @brief 网络管理器
class net_messenger
{
public:
    net_messenger();
    ~net_messenger();

public:
    static net_messenger* Instance();

public:
    //! 启动网络管理器线程
    //! @return 0:成功, -1失败
    int start();

    //! 停止网络管理器线程
    //! @return 0:成功, -1失败
    int stop();

    //! 创建一个tcp client通道
    //! @param remote_addr tcp对端地址
    //! @param packet_splitter tcp拆包器
    //! @param user_data 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    int32_t create_tcp_client(const Address& remote_addr, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! 创建一个tcp client通道
    //! @param remote_ip 对端ip
    //! @param remote_port 对端port
    //! @param packet_splitter tcp拆包器
    //! @param user_data 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    int32_t create_tcp_client(const char *remote_ip, int remote_port, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! 创建一个tcp server通道
    //! @param local_addr tcp本地监听地址
    //! @param packet_splitter tcp拆包器
    //! @param user_data 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    int32_t create_tcp_server(const Address& local_addr, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! 创建一个tcp server通道
    //! @param local_ip tcp本地监听ip
    //! @param local_port tcp本地监听port
    //! @param packet_splitter tcp拆包器
    //! @param user_data 用户定义的数据，在netevent中返回给用户，一般会包含该连接的事件处理函数指针
    //! @param netbufsize 网络底层缓冲区大小
    //! @return 通道id, >0:成功, 0失败
    int32_t create_tcp_server(const char* local_ip, int local_port, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! 通知删除网络通道
    //! @param id 通道id
    //! @param evt net_event_type
    //! @return 0:成功 <0:失败
    int notify_close(int32_t net_id, int16_t evt = net_event::NE_NOTIFY_CLOSE);

    //! 发送数据包
    //! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
    //! @param id 通道id
    //! @param packet 数据包
    //! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
    int send_package(int32_t net_id, net_package* netpkg);

    //! 取网络事件
    //! @warning 客户端应循环调用此函数获取网络事件, 并在处理完成每个网络事件后将其释放
    //! @return 网络事件, 没有网络事件时返回NULL
    net_event* pop_event();

    //! 获取未处理的发送事件的数量
    //! @return 发送事件的数量
    int net_send_package_count();

    //! 获取未处理的接收事件的数量
    //! @return 接收事件的数量
    int net_event_count();

    //! 分配net_id
    //! @return net_id
    int32_t acquire_net_id(net_id_holder::net_id_t type);

    //! 释放net_id
    //! @param net_id
    void release_net_id(int32_t net_id);

    //! 获取反应器
    //! @return 反应器指针
    net_reactor* reactor_pointer();

    //! 放入一个网络事件
    //! @param net_event 网络事件
    int push_event(net_event* netev);

    //! 取发送任务
    //! @return 发送任务, 没有发送任务时返回NULL
    net_package* pop_net_send_package();

    //! 网络故障(reactor故障)
    void reactor_exception();

public:
    friend class net_thread;

private:
    //! 运行状态 0:停止, 1:运行, 2:异常
    int m_status_;
    //! 网络管理器线程
    net_thread* m_net_thread_;
    //! 通道id管理器
    net_id_holder m_net_id_holder_;
    //! 反应器
    net_reactor m_reactor_;

    //! 网络事件队列，队列容量为100万
    net_event_queue* m_net_event_queue_;
    //! 待发送任务队列，队列容量为100万
    net_package_queue* m_net_send_packet_queue_;
};

#endif // _NET_MESSENGER_H_
