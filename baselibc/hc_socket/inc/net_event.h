#ifndef _NET_EVENT_H_
#define _NET_EVENT_H_

#include "hc_types.h"
#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_object_guard.h"
#include "hc_stack_trace.h"

#include "net_package.h"
#include "address.h"


//! @class net_event
//! @brief 网络事件类
class net_event : public non_copyable, public destroyable
{
public:
    //! @enum net_event_type
    //! @brief 网络事件类型
    typedef enum net_event_type {
        NE_NULL,                 //!< 空事件

        // 底层事件，抛给上层
        NE_CONNECTED,            //!< tcp连接成功
        NE_ACCEPT,               //!< tcp接受连接
        NE_DATA,                 //!< 数据到达

        NE_CLOSE,                //!< tcp连接关闭
        NE_EXCEPTION,            //!< 连接异常

        NE_TIMEOUT,              //!< tcp连接超时

        // 应用层通知底层关闭，底层关闭后，再抛给上层
        NE_NOTIFY_CLOSE,         //!< tcp通知关闭
    } net_ev_t;

public:
    net_event() : m_net_ev_t_(net_event::NE_NULL),
                  m_listen_net_id_(0), m_net_id_(0),
                  m_net_package_(NULL), m_user_data_(NULL)
    {
        STACK_TRACE_LOG();
    }

    virtual ~net_event()
    {
        STACK_TRACE_LOG();

        if ( NULL != m_net_package_ )
        {
            m_net_package_->Destroy();
            m_net_package_ = NULL;
        }
    }

public:
    //! 网络事件类型
    net_ev_t m_net_ev_t_;
    //! 监听通道id, 如果不是监听通道，此处为0
    uint32_t m_listen_net_id_; 
    //! 连接通道id, 如果没有新通道产生，通道此处为0
    uint32_t m_net_id_;

    //! 数据包：从外面传进来的数据，本类不负责分配。
    net_package* m_net_package_;
    //! 该事件处理函数指针
    void* m_user_data_;
    //! 对端地址
    Address m_remote_addr_;
};


//! 网络事件处理器
typedef void (*net_event_callback_t)(net_event& ne);

typedef object_guard<net_event> net_event_pool;

//! 网络事件队列
//! 通信层产生事件并放入此队列, 应用层取出事件并释放
//! 此队列线程安全
typedef sync_queue<net_event*> net_event_queue;

#endif // _NET_EVENT_H_
