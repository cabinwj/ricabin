#ifndef _NET_ID_HOLDER_H_
#define _NET_ID_HOLDER_H_

#include "hc_types.h"
#include "hc_sync_queue.h"


//! @class net_id_holder
//! @brief 网络通道id管理器
//!
//! 此类维护数量足够的网络通道id, 实际运行过程中不可能出现id用尽的情况
class net_id_holder
{
public:
    //! @enum net_id_type
    //! @brief 网络通道id分类
    typedef enum net_id_type
    {
        ID_TCP_CONNECTOR,
        ID_TCP_ACCEPTOR,
        ID_TCP_STREAM
    } net_id_t;

    typedef enum id_interval_type {
        TCP_CONNECTOR_BEGIN = 100001,
        TCP_CONNECTOR_END = 200000,

        TCP_ACCEPTOR_BEGIN = 200001,
        TCP_ACCEPTOR_END = 300000,

        TCP_STREAM_BEGIN = 300001,
        TCP_STREAM_END = 1000000
    } id_interval_t;

public:
    net_id_holder();
    ~net_id_holder();

    //! 分配一个网络通道id
    //! @param type 要分配的通道类型
    //! @return 通道id 0:失败,id用尽,  >0:成功
    int32_t acquire(net_id_holder::net_id_t type);

    //! 释放网络通道id
    //! @param id 网络通道id
    void release(int32_t id);

private:
    //! 网络通道id队列
    typedef sync_queue<int32_t> net_id_queue;

    //! 网络通道tcp_connector_id队列10万
    net_id_queue* m_tcp_connector_id_queue_;
    //! 网络通道tcp_acceptor_id队列10万
    net_id_queue* m_tcp_acceptor_id_queue_;
    //! 网络通道tcp_acceptor_id队列70万
    net_id_queue* m_tcp_stream_id_queue_;
};


#endif // _NET_ID_HOLDER_H_
