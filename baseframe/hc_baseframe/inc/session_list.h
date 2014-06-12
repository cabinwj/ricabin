#ifndef _SESSION_LIST_H_
#define _SESSION_LIST_H_

#include "hc_list.h"

#include "protocol.h"

class net_node;

class session_list
{
public:
    enum {
        HASH_BUCKET_COUNT_ = 1000003,
    };

public:
    static int32_t hash_func(int32_t key);

public:
    static void init();

    static void on_connected(net_node* conn);
    static void on_accepted(net_node* client);

    static void append_uin_to_list(net_node* client);

    static void on_disconnect_by_net_id(uint32_t net_id);

    static void on_disconnect(net_node* conn);

public:
    static net_node* net_node_by_net_id_o(uint32_t net_id);
    static net_node* net_node_by_uin_o(uint32_t uin, is_trans_t is_trans);

public:
    static list_head m_net_hash_bucket_[HASH_BUCKET_COUNT_];
    static list_head m_uin_hash_bucket_[HASH_BUCKET_COUNT_];

    static list_head m_connect_list_;         // 连接出去的net链表
    static list_head m_acceptc_list_;         // 监听client的net链表
    static list_head m_accepts_list_;         // 监听server的net链表
};

#endif

