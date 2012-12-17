#ifndef _STATUSD_USER_LIST_H_
#define _STATUSD_USER_LIST_H_

#include "hash_list.h"

class user_node : public hash_node
{
public:
    user_node(uint32_t uid);
    ~user_node();

public:
    uint16_t m_online_status_;
    uint32_t m_client_net_id_;
    uint32_t m_client_ip_;
    uint32_t m_connector_id_;
    uint32_t m_logic_id_;
    time_t m_login_time_;
    time_t m_last_logout_time_;
    time_t m_last_recv_status_time_;

public:
    static list_head m_user_list_head_;
    static list_head* m_user_list_iter_;
    static uint32_t m_user_count_;
};


class user_list : public hash_list
{
public:
    enum
    {
        USER_HASH_BUCKET_COUNT = 100003,
    };

public:
    static void init();
    static user_node* get(uint32_t key);
    static void put(user_node* p);

public:
    static user_node* iter_user();
    static void print_hash();

public:
    static list_head m_user_hash_bucket_[USER_HASH_BUCKET_COUNT];
};

#endif
