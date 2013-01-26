#ifndef _CORO_HASH_H_
#define _CORO_HASH_H_

#include "coroutine.h"
#include "hc_list.h"
#include "hc_types.h"

typedef struct coro_list_type
{
    list_head m_hash_item_;
    list_head m_coro_list_;
    uint32_t m_net_id_;
} coro_list_t;


// 维护异步请求的列表（阻塞任务队列列表）
class coro_hash
{
public:
    enum {
        HASH_BUCKET_COUNT  = 500009,
    };

public:
    static void init();
    static void clear();

    static coro_list_t* get_coro_list(uint32_t net_id);
    static coroutine* get_coro(uint32_t sequence_id);

    static void push_coro(uint32_t net_id, uint32_t sequence_id, coroutine* coro);
    static void remove_coro(coroutine* coro);

    static void on_awaken_coro(uint32_t net_id);

public:
    struct coro_hash_head
    {
        list_head m_sequence_head_;
        list_head m_net_head_;
    };

    static coro_hash_head m_hash_bucket_[HASH_BUCKET_COUNT];
};

#endif // _CORO_HASH_H_
