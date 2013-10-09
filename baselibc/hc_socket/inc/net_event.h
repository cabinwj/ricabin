#ifndef _NET_EVENT_H_
#define _NET_EVENT_H_

#include "hc_types.h"
#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_object_holder.h"
#include "hc_sync_queue.h"

#include "address.h"

class Address;
class net_package;

//! @class net_event
//! @brief 网络事件类
class net_event : public non_copyable, public destroyable
{
public:
    //! @enum net_event_type
    //! @brief 网络事件类型
    typedef enum net_event_type {
        NE_NULL            = 0,         //!< 空事件

        // 底层事件，抛给上层
        NE_CONNECTED       = 1,         //!< tcp连接成功
        NE_ACCEPT          = 2,         //!< tcp接受连接
        NE_DATA            = 3,         //!< 数据到达

        NE_CLOSE           = 4,         //!< tcp连接关闭
        NE_EXCEPTION       = 5,         //!< 连接异常

        // 应用层通知底层关闭，底层关闭后，再抛给上层
        NE_TIMEOUT         = 6,         //!< tcp连接超时
        NE_NOTIFY_CLOSE    = 7,         //!< tcp通知关闭
    } net_ev_t;

public:
    net_event();
    virtual ~net_event();

public:
    //! 网络事件类型
    int16_t m_net_ev_t_;
    //! 监听通道id / 连接通道id
    int32_t m_net_id_;

    //! 数据包：从外面传进来的数据，本类不负责分配。
    net_package* m_net_package_;
    //! 该事件处理函数指针
    void* m_user_data_;
    //! 对端地址
    Address m_remote_addr_;

public:
    //! net_event 此类在网络层被分配并初始化, 在应用层被使用并销毁, 为提高效率使用预分配对象池
    static object_holder<net_event>* m_pool_;
};

//! 网络事件处理器
typedef void (*net_event_callback_t)(net_event& ne);

//! 网络事件队列
//! 通信层产生事件并放入此队列, 应用层取出事件并释放
//! 此队列线程安全
typedef sync_queue<net_event*> net_event_queue;

#endif // _NET_EVENT_H_
