#ifndef _KEEPALIVE_LIST_H_
#define _KEEPALIVE_LIST_H_

#include "hc_list.h"

#include "protocol.h"


typedef struct alive_type
{
    alive_type() : m_net_id_(0), m_next_alive_time_(0), m_next_recv_alive_time_(0),
        m_keepalive_interval_(0), m_keepalive_timeout_(0)
    {
        INIT_LIST_HEAD(&m_list_item_);
        INIT_LIST_HEAD(&m_hash_item_);
    }

    list_head m_list_item_;             // 用于accept or connect链表
    list_head m_hash_item_;             // 用于hash表

    uint32_t m_net_id_;                 // netid

    time_t m_next_alive_time_;          // 下次发送alive的时间
    time_t m_next_recv_alive_time_;     // 下次应该收到alive回应的时间

    uint16_t m_keepalive_interval_;    // 发送alive的间隔时间
    uint16_t m_keepalive_timeout_;     // 多少时间收不到alive就超时断开
} alive_t;

class keepalive
{
public:
    enum {
        HASH_BUCKET_COUNT_ = 1000003,
    };

public:
    static int32_t hash_func(uint32_t key) { return (int32_t)(key % HASH_BUCKET_COUNT_); }

public:
    static void init();
    static void on_connected(uint32_t connect_net_id, uint16_t keepalive_interval, uint16_t keepalive_timeout);
    static void on_accepted(uint32_t accepted_net_id, uint16_t keepalive_timeout);
    static void on_recv_alive(uint32_t net_id);
    static void on_disconnect(uint32_t net_id);
    static void on_check_alive(uint32_t now);
    static void on_send_alive(uint32_t net_id, message_t msg_t);

public:
    static list_head m_net_hash_list_[HASH_BUCKET_COUNT_];
    static list_head m_alive_list_;   // 连接出去的net链表
};

#endif
