//! @file sock_stream.h
#ifndef _SOCK_STREAM_H_
#define _SOCK_STREAM_H_

#include "hc_block_buffer.h"
#include "hc_non_copyable.h"
#include "hc_destroyable.h"

#include "ihandler.h"
#include "socket.h"
#include "address.h"
#include "net_config.h"
#include "net_package.h"

class ireactor;
class ipacket_splitter;
class net_messenger;
class net_package;
class net_event;

//! @class sock_stream
//! @brief tcp通道处理类
class sock_stream : public ihandler, public non_copyable, public destroyable
{
public:
    //! 构造函数
    //! @param listen_net_id 监听通道id
    //! @param net_id 消息通道id
    //! @param net_messenger 网络管理器
    //! @param packet_splitter 拆包器
    //! @param socket 套接字
    sock_stream();
    virtual ~sock_stream();

public:
    int init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id, const Address& remote_addr, Descriptor socket);

public:
    virtual int open(const Address& local_addr, int recv_bufsize, int send_bufsize);
    //! 关闭连接
    virtual void close();

public:
    //! 获取网络标示（通道id）
    virtual int32_t handler_o();
    //! 设置网络标示（通道id）
    virtual void handler_o(int32_t net_id);

    //! 获取 socket Descriptor 描述符
    virtual Descriptor descriptor_o();
    //! 设置 socket Descriptor 描述符
    virtual void descriptor_o(Descriptor sock) ;
    //! 获取 event mask 事件关注信息
    virtual int32_t event_mask_o();
    //! 获取 event mask 事件关注信息
    virtual void event_mask_o(int32_t ev_mask);
    //! 获取 反应器
    virtual ireactor* reactor_o();
    //! 设置 反应器
    virtual void reactor_o(ireactor* react);

    //! 读
    //! @return 处理结果 0:处理正常, -1: 连接被对方关闭, -2:连接异常
    virtual int handle_input();
    //! 写
    //! @return 处理结果 0:处理正常, -1: 连接异常
    virtual int handle_output();
    //! 连接关闭  event_type = net_event::NE_CLOSE
    //! 连接异常  event_type = net_event::NE_EXCEPTION
    //! 超时      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(int16_t evt);

    //! 提交发送任务
    virtual int post_package(net_package* netpkg);

public:
    friend class sock_acceptor;
    friend class sock_connector;

private:
    int send_package();

private:
    //! event mask 事件关注信息
    int16_t m_ev_mask_;
    //! 连接通道id
    int32_t m_net_id_;

    //! 反应器
    ireactor* m_reactor_;
    //! 用户自定义数据 该连接的事件处理函数指针
    void* m_user_data_;

    //! socket句柄 Descriptor(m_socket_);
    Socket m_socket_;

    //! 网络管理器
    net_messenger* m_net_messenger_;
    //! 拆包器
    ipacket_splitter* m_packet_splitter_;
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
    static object_holder<sock_stream>* m_pool_;
};

#endif // _SOCK_STREAM_H_
