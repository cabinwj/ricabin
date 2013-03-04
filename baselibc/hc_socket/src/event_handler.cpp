#include "event_handler.h"
#include "config.h"

// class event_handler
int event_handler::m_current_count_ = 0;
list_head event_handler::m_hash_bucket_[HANDLER_TABLE_SIZE];
list_head event_handler::m_timeout_list_head_;

net_event_pool* event_handler::m_net_ev_pool_ = new \
    net_event_pool( \
    new object_pool_allocator<net_event>(NET_EVENT_POOL_COUNT, \
                                         5 * NET_EVENT_POOL_COUNT, \
                                         NET_EVENT_POOL_COUNT, \
                                         new_allocator::Instance()));

net_package_pool* event_handler::m_net_pkg_pool_ = new \
    net_package_pool( \
    new object_pool_allocator<net_package>(NET_PACKET_POOL_COUNT, \
                                           5 * NET_PACKET_POOL_COUNT, \
                                           NET_PACKET_POOL_COUNT, \
                                           new_allocator::Instance()));


// hash list
static inline uint32_t hash_func(uint32_t key)
{
    return key % event_handler::HANDLER_TABLE_SIZE;
}

event_handler::event_handler() : m_ev_mask_(0), m_net_id_(0), m_timeout_(0),
                                 m_reactor_(NULL), m_user_data_(NULL)
{
    INIT_LIST_HEAD(&m_hash_item_);
    INIT_LIST_HEAD(&m_timeout_item_);
}

event_handler::~event_handler()
{
}

void event_handler::init_hash_table()
{
    //threadc_mutex_guard lock(m_mutex_);
    for (int index = 0; index < HANDLER_TABLE_SIZE; index++)
    {
        INIT_LIST_HEAD(&m_hash_bucket_[index]);
    }

    INIT_LIST_HEAD(&m_timeout_list_head_);

    m_current_count_ = 0;
}

void event_handler::clear_hash_table()
{
    list_head* pos;
    list_head* n;
    list_head* head;
    for (int index = 0; index < HANDLER_TABLE_SIZE; index++)
    {
        head = &m_hash_bucket_[index];
        if ( list_empty(head) )
        {
            continue;
        }

        list_for_each_safe(pos, n, head)
        {
            event_handler* eh = list_entry(pos, event_handler, m_hash_item_);
            // delete timeout from list
            list_del(&eh->m_timeout_item_);
            // delete hash from list
            list_del(pos);
            // delete hash from list, delete timeout from list and close, delete handler
            eh->handle_close(net_event::NE_NOTIFY_CLOSE);

            m_current_count_--;
        }
    }

    m_current_count_ = 0;
}

event_handler* event_handler::get_handler(uint32_t tunnel_id)
{
    int npos = hash_func(tunnel_id);
    if (npos >= HANDLER_TABLE_SIZE || npos < 0)
    {
        return NULL;
    }

    list_head* head = &m_hash_bucket_[npos];
    if ( list_empty(head) )
    {
        return NULL;
    }

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_hash_item_);
        if (eh->m_net_id_ == tunnel_id)
        {
            return eh;
        }
    }

    return NULL;
}

void event_handler::push_handler(event_handler* eh, uint32_t tunnel_id)
{
    if (NULL == eh)
    {
        return;
    }

    int npos = hash_func(tunnel_id);
    if (npos >= HANDLER_TABLE_SIZE || npos < 0)
    {
        return;
    }

    list_add_tail(&eh->m_hash_item_, &m_hash_bucket_[npos]);
    m_current_count_++;
}

void event_handler::remove_handler(event_handler* eh)
{
    // delete timeout from list
    list_del(&eh->m_timeout_item_);
    // delete hash from list
    list_del(&eh->m_hash_item_);

    m_current_count_--;
}

void event_handler::remove_handler(uint32_t tunnel_id)
{
    int npos = hash_func(tunnel_id);
    if ( npos >= HANDLER_TABLE_SIZE || npos < 0 )
    {
        return;
    }

    list_head* head = &m_hash_bucket_[npos];
    if ( list_empty(head) )
    {
        return;
    }

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_hash_item_);
        if (eh->m_net_id_ == tunnel_id)
        {
            // delete timeout from list
            list_del(&eh->m_timeout_item_);
            // delete hash from list
            list_del(pos);
            m_current_count_--;
            break;
        }
    }
}

// set / cancel time out and notify close
void event_handler::sync_timeout(event_handler* eh, time_t timeout)
{
    if (NULL == eh)
    {
        return;
    }

    if ( timeout > 0 )
    {
        eh->m_timeout_ = time(NULL) + timeout;
        list_del(&eh->m_timeout_item_);
        list_add_tail(&eh->m_timeout_item_, &m_timeout_list_head_);
    }
    else
    {
        eh->m_timeout_ = 0;
        list_del(&eh->m_timeout_item_);
    }
}

void event_handler::on_timer(time_t now)
{
    list_head* head = &m_timeout_list_head_;
    if ( list_empty(head) )
    {
        return;
    }

    list_head* pos;
    list_head* n;
    list_for_each_safe(pos, n, head)
    {
        event_handler* eh = list_entry(pos, event_handler, m_timeout_item_);
        if ((0 != eh->m_timeout_) && (now >= eh->m_timeout_))
        {
            // delete timeout from list, delete hash from list and close, and delete handler
            eh->handle_close(net_event::NE_TIMEOUT);
        }
    }
}

