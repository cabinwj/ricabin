//! @file sock_connector.h
#ifndef _SOCK_CONNECTOR_H_
#define _SOCK_CONNECTOR_H_

#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_object_holder.h"

#include "ihandler.h"
#include "socket.h"
#include "address.h"

class ireactor;
class ipacket_splitter;
class net_messenger;
class net_package;

//! @class sock_connector
//! @brief tcp连接处理类
class sock_connector : public ihandler, public non_copyable, public destroyable
{
public:
    //! 构造函数
    sock_connector();
    //! 析构函数
    virtual ~sock_connector();

public:
    //! @param net_messenger 网络管理器
    //! @param packet_splitter 拆包器
    int init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id);

public:
    //! 创建tcp连接器 连接服务器(异步连接)
    //! @return 结果, 0:成功, -1失败
    virtual int open(const Address& remote_addr, int recv_bufsize, int send_bufsize);

    //! 关闭通道
    virtual void close();

public:
    //! 获取网络标示（通道id）
    virtual int32_t handler_o();
    //! 设置网络标示（通道id）
    virtual void handler_o(int32_t net_id);

    //! 获取 socket Descriptor 描述符
    virtual Descriptor descriptor_o();
    //! 设置 socket Descriptor 描述符
    virtual void descriptor_o(Descriptor sock);
    //! 获取 event mask 事件关注信息
    virtual int32_t event_mask_o();
    //! 获取 event mask 事件关注信息
    virtual void event_mask_o(int32_t ev_mask);
    //! 获取 反应器
    virtual ireactor* reactor_o();
    //! 设置 反应器
    virtual void reactor_o(ireactor* react);

    //! 处理读
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_input();
    //! 处理写
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_output();
    //! 连接关闭  event_type = net_event::NE_CLOSE
    //! 连接异常  event_type = net_event::NE_EXCEPTION
    //! 超时      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(int16_t evt);

    //! 提交发送任务
    //! @param post_packet 待发送的任务
    virtual int post_package(net_package* send_packet);

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

public:
    //! connector套接字
    static object_holder<sock_connector>* m_pool_;
};

#endif // _SOCK_CONNECTOR_H_
