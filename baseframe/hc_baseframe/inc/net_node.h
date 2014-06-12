#ifndef _NET_NODE_H_
#define _NET_NODE_H_

#include "hc_list.h"
#include "hc_bitmap.h"

#include "net_event.h"
#include "address.h"


class net_node
{
public:
    net_node();
    virtual ~net_node();

public:
    virtual void on_ne_data(net_event& ne) { }

public:
    list_head m_list_item_;             // 用于accept or connect链表
    list_head m_net_hash_item_;         // 用于hash表
    list_head m_uin_hash_item_;         // 与应用层领域相关联的标识uin hash 表

    uint32_t m_net_id_;                 // netid  // 监听netid 不为零表示是 accept上来的   

    uint32_t m_remote_uin_;             // 与应用层领域相关联的ID，用于唯一标识。
    // 如果是accept client的连接，uin就是应用层领域相关联（用户uin）。 而对本连接没有服务标识
    // 如果是accept server的连接，uin就是对方服务的标识（对方的服务ID）
    // 如果是connect out的连接，uin就是对方服务的标识（对方的服务ID）

    uint32_t m_last_recv_timestamp_;    // 最近一次从对端接收到数据的timestamp

    bitmap32 m_is_trans_;               // is_transmission_t
    Address m_remote_addr_;             // 对端的ip
};

#endif
