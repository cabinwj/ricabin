//! @file sock_connector.h
#ifndef _SOCK_CONNECTOR_H_
#define _SOCK_CONNECTOR_H_

#include "event_handler.h"
#include "packet_splitter.h"
#include "net_manager.h"


//! @class sock_connector
//! @brief tcp连接处理类
class sock_connector : public event_handler, public non_copyable, public destroyable
{
public:
    //! 构造函数
    //! @param net_manager 网络管理器
    //! @param packet_splitter 拆包器
    sock_connector();
    //! 析构函数
    virtual ~sock_connector();

public:
    int init(net_manager* nm, packet_splitter* ps, void* user_data,
             uint32_t listen_net_id, uint32_t net_id);

public:
    //! 创建tcp连接器 连接服务器(异步连接)
    //! @param remote_addr 服务端地址
    //! @param timeout 连接超时
    //! @return 结果, 0:成功, -1失败
    int create_tcp_client(const Address& remote_addr, int timeout, int netbufsize);

    //! 关闭通道
    void close_tcp_client();

    //! 处理读
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_input();

    //! 处理写
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_output();

    //! 连接关闭  event_type = net_event::NE_CLOSE
    //! 连接异常  event_type = net_event::NE_EXCEPTION
    //! 超时      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t evt);

    //! 提交发送任务
    //! @param post_packet 待发送的任务
    virtual int post_package(net_package* send_packet);

private:
    //! 网络管理器
    net_manager* m_net_manager_;
    //! 拆包器
    packet_splitter* m_packet_splitter_;
    //! 对方远程地址
    Address m_remote_addr_;

public:
    //! connector套接字
    static object_guard<sock_connector>* m_pool_;
};

#endif // _SOCK_CONNECTOR_H_
