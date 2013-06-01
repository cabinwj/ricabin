#include "coro_hash.h"
#include "hc_stack_trace.h"

#include "scheduler.h"


static inline int hashfn(uint32_t sequence)
{
    return (sequence % coro_hash::HASH_BUCKET_COUNT);
}

coro_hash::coro_hash_head coro_hash::m_hash_bucket_[HASH_BUCKET_COUNT];

void coro_hash::init()
{
    STACK_TRACE_LOG();

    for (int i = 0; i < HASH_BUCKET_COUNT; i++)
    {
        INIT_LIST_HEAD(&m_hash_bucket_[i].m_net_head_);
        INIT_LIST_HEAD(&m_hash_bucket_[i].m_sequence_head_);
    }
}

void coro_hash::clear()
{
    STACK_TRACE_LOG();

    list_head* pos;
    list_head* n;
    list_head* corolist_head;
    list_head* coro_head;
    for (int index = 0; index < HASH_BUCKET_COUNT; ++index)
    {
        coro_head = &m_hash_bucket_[index].m_sequence_head_;
        if ( !list_empty(coro_head) )
        {
            list_for_each_safe(pos, n, coro_head)
            {
                coroutine* coro = list_entry(pos, coroutine, m_sequence_item_);
                delete coro;
            }
        }
    }

    for (int index = 0; index < HASH_BUCKET_COUNT; ++index)
    {
        corolist_head = &m_hash_bucket_[index].m_net_head_;
        if ( !list_empty(corolist_head) )
        {
            list_for_each_safe(pos, n, corolist_head)
            {
                coro_list_t* corolist = list_entry(pos, coro_list_t, m_hash_item_);
                delete corolist;
            }
        }
    }
}

coro_list_t* coro_hash::get_coro_list(uint32_t net_id)
{
    STACK_TRACE_LOG();

    list_head* pos;
    coro_list_t* corolist = NULL;
    list_for_each(pos, &m_hash_bucket_[hashfn(net_id)].m_net_head_)
    {
        corolist = list_entry(pos, coro_list_t, m_hash_item_);
        if (corolist->m_net_id_ == net_id)
        {
            return corolist;
        }
    }

    return NULL;
}

coroutine* coro_hash::get_coro(uint32_t sequence_id)
{
    STACK_TRACE_LOG();

    list_head* pos;
    coroutine* coro = NULL;
    list_for_each(pos, &m_hash_bucket_[hashfn(sequence_id)].m_sequence_head_)
    {
        coro = list_entry(pos, coroutine, m_sequence_item_);
        if (coro->get_sequence_id() == sequence_id)
        {
            return coro;
        }
    }

    return NULL;
}

void coro_hash::push_coro(uint32_t net_id, uint32_t sequence_id, coroutine* coro)
{
    STACK_TRACE_LOG();

    coro->set_sequence_id(sequence_id);

    list_add_tail(&coro->m_sequence_item_, &m_hash_bucket_[hashfn(sequence_id)].m_sequence_head_);

    coro_list_t* corolist = get_coro_list(net_id);
    if (NULL == corolist)
    {
        corolist = new coro_list_t;
        corolist->m_net_id_ = net_id;
        INIT_LIST_HEAD(&corolist->m_coro_list_);
        list_add_tail(&corolist->m_hash_item_, &m_hash_bucket_[hashfn(net_id)].m_net_head_);
    }

    list_add_tail(&coro->m_net_item_ ,&corolist->m_coro_list_);
}

void coro_hash::remove_coro(coroutine* coro)
{
    STACK_TRACE_LOG();

    list_del(&coro->m_sequence_item_);
    list_del(&coro->m_net_item_);
}


void coro_hash::on_awaken_coro(uint32_t net_id)
{
    STACK_TRACE_LOG();

    coro_list_t* corolist = coro_hash::get_coro_list(net_id);
    if (NULL != corolist)
    {
        list_head* pos;
        list_head* next;
        coroutine* coro = NULL;
        list_for_each_safe(pos, next, &corolist->m_coro_list_)
        {
            coro = list_entry(pos, coroutine, m_net_item_);
            // socket close or exception 将io result 置NULL，切回挂起协程
            coro->set_io_result(NULL);
            coro_scheduler::switch_to(coro);
        }

        list_del(&corolist->m_hash_item_);
        delete corolist;
    }
}
