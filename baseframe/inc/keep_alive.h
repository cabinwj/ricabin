#ifndef _CORO_KEEP_ALIVE_H_
#define _CORO_KEEP_ALIVE_H_

#include "common_types.h"
#include "common_list.h"

#include "protocol.h"


typedef struct alive_type
{
    list_head m_list_item_;             // ����accept or connect����
    list_head m_hash_item_;             // ����hash��

    uint32_t m_net_id_;                 // netid
    uint32_t m_listen_net_id_;          // ����netid ��Ϊ���ʾ�� accept������

    time_t m_next_alive_time_;          // �´η���alive��ʱ��
    time_t m_next_recv_alive_time_;     // �´�Ӧ���յ�alive��Ӧ��ʱ��

    uint16_t m_keep_alive_interval_;    // ����alive�ļ��ʱ��
    uint16_t m_keep_alive_timeout_;     // ����ʱ���ղ���alive�ͳ�ʱ�Ͽ�
} alive_t;

typedef struct listen_alive_type
{
    list_head m_listen_item_;           // ���ڼ�������
    list_head m_accept_list_;           // �����ϵ�����

    uint32_t m_listen_net_id_;          // ������netid
} listen_alive_t;

class keep_alive
{
public:
    enum {
        HASH_BUCKET_COUNT_ = 1000003,
    };

public:
    static int32_t hash_func(uint32_t key) { return (int32_t)(key % HASH_BUCKET_COUNT_); }

public:
    static void init(uint32_t uin);
    static void on_connected(uint32_t connect_net_id, uint16_t keep_alive_interval, uint16_t keep_alive_timeout);
    static void on_accepted(uint32_t listen_net_id, uint32_t accepted_net_id, uint16_t keep_alive_timeout);
    static void on_recv_alive(uint32_t net_id);
    static void on_disconnect(uint32_t net_id);
    static void on_check_alive(uint32_t now);
    static void on_send_alive(uint32_t net_id, message_t msg_t);

public:
    static uint32_t m_local_uin_;   // server id

public:
    static list_head m_net_id_hash_list_[HASH_BUCKET_COUNT_];
    static list_head m_connect_list_;   // ���ӳ�ȥ��net����
    static list_head m_listen_list_;    // ������net����
};

#endif
