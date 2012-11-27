//! @file sock_stream.h
#ifndef _SOCK_STREAM_H_
#define _SOCK_STREAM_H_

#include "common_block_buffer.h"

#include "event_handler.h"
#include "packet_splitter.h"
#include "net_manager.h"

#include "config.h"


//! @class sock_stream
//! @brief tcp通道处理类
class sock_stream : public event_handler, public non_copyable, public destroyable
{
public:
    //! 构造函数
    //! @param listen_net_id 监听通道id
    //! @param net_id 消息通道id
    //! @param net_manager 网络管理器
    //! @param packet_splitter 拆包器
    //! @param socket 套接字
    sock_stream();
    virtual ~sock_stream();

public:
    int init(uint32_t listen_net_id, uint32_t net_id, net_manager* nm,
             packet_splitter* ps, const Address& remote_addr, Descriptor socket, void* user_data);

public:
    //! 关闭连接
    void close_stream();

    //! 读
    //! @return 处理结果 0:处理正常, -1: 连接被对方关闭, -2:连接异常
    virtual int handle_input();

    //! 写
    //! @return 处理结果 0:处理正常, -1: 连接异常
    virtual int handle_output();

    //! 连接关闭  event_type = net_event::NE_CLOSE
    //! 连接异常  event_type = net_event::NE_EXCEPTION
    //! 超时      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t evt);

    //! 提交发送任务
    virtual int post_package(net_package* netpkg);

public:
    friend class sock_acceptor;
    friend class sock_connector;

private:
    int send_package();

private:
    //! 网络管理器
    net_manager* m_net_manager_;
    //! 拆包器
    packet_splitter* m_packet_splitter_;
    //! 对方远程地址
    Address m_remote_addr_;
    //! 接受缓存
    block_buffer<RECV_BUFFER_LENGTH> m_recv_buffer_;
    //! 通道待发送任务队列
    //! 此队列线程安全
    //! 队列容量为1万
    net_package_queue* m_socket_send_packet_queue_;
    //! 当前正在发送的任务
    net_package* m_send_netpkg_;
    //! 当前正在发送的任务已经发送的字节数
    int m_send_netpkglen_;
    //! 当前数据包缓存
    net_event* m_net_event_;
    //! 当前数据包剩下多少长度未接收
    int m_remain_len_;

public:
    //! stream套接字池70万, 为提高效率使用预分配对象池
    static object_guard<sock_stream>* m_pool_;
};

#endif // _SOCK_STREAM_H_
