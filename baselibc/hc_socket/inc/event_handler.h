//! @file event_handler.h
#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include "hc_types.h"
#include "hc_list.h"
#include "hc_thread_mutex.h"

#include "socket.h"
#include "reactor.h"
#include "net_event.h"
#include "net_package.h"


//! @class event_handler
//! @brief 事件处理器
class event_handler
{
public:
    //! @enum event_mask_t
    //! @brief 事件类型
    typedef enum event_mask_type {
        EM_NULL    = 0,
        EM_READ    = 1 << 0,    //!< 可读事件
        EM_WRITE   = 1 << 1,    //!< 可写事件

        EM_ALL     = EM_READ | EM_WRITE,
    } ev_mask_t;

    enum {
        HANDLER_TABLE_SIZE = 100003
    };

public:
    //! list hash node
    list_head m_hash_item_;
    //! list timeout node
    list_head m_timeout_item_;
    //! list notify close node
    list_head m_notify_close_item_;

    //! event mask 事件关注信息
    uint32_t m_ev_mask_;

    //! 监听通道id, 如果不是监听通道，此处为0
    uint32_t m_listen_net_id_;
    //! 连接通道id, 如果没有新通道产生，通道此处为0
    uint32_t m_net_id_;
    //! 超时的时间, 0表示没有设置超时
    time_t m_timeout_;


    //! 反应器
    reactor* m_reactor_;
    //! 用户自定义数据 该连接的事件处理函数指针
    void* m_user_data_;

    //! socket句柄 Descriptor(m_socket_);
    Socket m_socket_;

public:
    event_handler();
    virtual ~event_handler();

public:
    //! 处理读
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_input() = 0;
    //! 处理写
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_output() = 0;
    //! 连接关闭  event_type = net_event::NE_CLOSE
    //! 连接异常  event_type = net_event::NE_EXCEPTION
    //! 超时      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t event_type) = 0;
    //! 提交发送任务
    //! @param netpkg 待发送的任务
    virtual int post_package(net_package* netpkg) = 0;

public:
    //! 设置超时
    //! @param timeout 超时时间(秒)
    static void set_timeout(event_handler* eh, time_t timeout);
    //! 定时器，检测超时处理
    static void on_timer(time_t now);
    //! 通知关闭连接处理
    static void on_notify_close();

public:
    //! 当前的事件数
    static int m_current_count_;
    //! hash list head table
    static list_head m_hash_bucket_[HANDLER_TABLE_SIZE];
    //! timeout list head
    static list_head m_timeout_list_head_;
    //! notify close list head
    static list_head m_notify_close_list_head_;

    //! 线程锁, 保护处理器 handler_list
    static threadc_mutex m_mutex_;

public:
    static void init_hash_table();
    static void clear_hash_table();
    static event_handler* get_handler(uint32_t tunnel_id);
    static void push_handler(event_handler* eh, uint32_t tunnel_id);
    static void remove_handler(event_handler* eh);
    static void remove_handler(uint32_t tunnel_id);

public:
    static void init_timeout_list();
    static void clear_timeout_list();
    static event_handler* get_handler_from_timeout_list(uint32_t tunnel_id);
    static void push_handler_to_timeout_list(event_handler* eh);
    static void remove_handler_from_timeout_list(event_handler* eh);
    static void remove_handler_from_timeout_list(uint32_t tunnel_id);

public:
    static void init_notify_close_list();
    static void clear_notify_close_list();
    static event_handler* get_handler_from_notify_close_list(uint32_t tunnel_id);
    static void push_handler_to_notify_close_list(event_handler* eh);
    static void remove_handler_from_notify_close_list(event_handler* eh);
    static void remove_handler_from_notify_close_list(uint32_t tunnel_id);

public:
    //! net_event 此类在网络层被分配并初始化, 在应用层被使用并销毁, 为提高效率使用预分配对象池
    static net_event_pool* m_net_ev_pool_;
    //! net_package 此类在网络层被分配并初始化, 在应用层被使用并销毁, 为提高效率使用预分配对象池
    static net_package_pool* m_net_pkg_pool_;
};

#endif // _EVENT_HANDLER_H_
