#ifndef _SESSION_LIST_H_
#define _SESSION_LIST_H_

#include "common_list.h"

#include "protocol.h"
#include "net_node.h"


class session_list
{
public:
    enum {
        HASH_BUCKET_COUNT_ = 1000003,
    };

public:
    static int32_t hash_func(uint32_t key) { return (int32_t)(key % HASH_BUCKET_COUNT_); }

public:
    static void init();

    static void on_connected(net_node* conn);
    static void on_accepted(net_node* client);

    static void append_uin_to_list(net_node* client);

    static void on_disconnect_by_net_id(uint32_t net_id);

    static void on_disconnect(net_node* conn);

    static void append_reconnect_list(uint32_t uin, uint16_t reconnect_interval);

public:
    static net_node* get_net_node_by_net_id(uint32_t net_id);
    static net_node* get_net_node_by_uin(uint32_t uin, is_trans_t is_trans);

public:
    static list_head m_net_hash_bucket_[HASH_BUCKET_COUNT_];
    static list_head m_uin_hash_bucket_[HASH_BUCKET_COUNT_];

    static list_head m_listen_list_;          // 监听的net链表
    static list_head m_connect_list_;         // 连接出去的net链表
    static list_head m_accepts_list_;         // 从其它服务器上连过来的
    static list_head m_reconnect_list_;       // 等待重连的net链表
};

#endif

