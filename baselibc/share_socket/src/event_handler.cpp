#include "event_handler.h"
#include "config.h"

// class event_handler
int event_handler::m_current_count_ = 0;
list_head event_handler::m_hash_bucket_[HANDLER_TABLE_SIZE];
list_head event_handler::m_timeout_list_head_;
list_head event_handler::m_notify_close_list_head_;
threadc_mutex event_handler::m_mutex_;

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
    INIT_LIST_HEAD(&m_notify_close_item_);
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

    event_handler::init_timeout_list();
    event_handler::init_notify_close_list();

    m_current_count_ = 0;
}

void event_handler::clear_hash_table()
{
    threadc_mutex_guard lock(m_mutex_);

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
            // delete notify close from list
            list_del(&eh->m_notify_close_item_);
            // delete hash from list
            list_del(pos);
            // delete hash from list, delete timeout from list, delete notify close list and close, delete handler
            eh->handle_close(net_event::NE_NOTIFY_CLOSE);

            m_current_count_--;
        }
    }

    m_current_count_ = 0;
}

event_handler* event_handler::get_handler(uint32_t tunnel_id)
{
    //threadc_mutex_guard lock(m_mutex_);

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

    threadc_mutex_guard lock(m_mutex_);

    list_head* head = &m_hash_bucket_[npos];
    list_add_tail(&eh->m_hash_item_, head);
    m_current_count_++;
}

void event_handler::remove_handler(event_handler* eh)
{
    threadc_mutex_guard lock(m_mutex_);

    // delete timeout from list
    list_del(&eh->m_timeout_item_);
    // delete notify close from list
    list_del(&eh->m_notify_close_item_);
    // delete hash from list
    list_del(&eh->m_hash_item_);

    m_current_count_--;
}

void event_handler::remove_handler(uint32_t tunnel_id)
{
    int npos = hash_func(tunnel_id);
    if (npos >= HANDLER_TABLE_SIZE || npos < 0)
    {
        return;
    }

    list_head* head = &m_hash_bucket_[npos];
    if ( list_empty(head) )
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

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
            // delete notify close from list
            list_del(&eh->m_notify_close_item_);
            // delete hash from list
            list_del(pos);
            m_current_count_--;
            break;
        }
    }
}


// timeout list
void event_handler::init_timeout_list()
{
    INIT_LIST_HEAD(&m_timeout_list_head_);
}

void event_handler::clear_timeout_list()
{
    list_head* head = &m_timeout_list_head_;
    if ( list_empty(head) )
    {
        return;
    }

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_timeout_item_);
        event_handler::remove_handler_from_timeout_list(eh);
    }
}

event_handler* event_handler::get_handler_from_timeout_list(uint32_t tunnel_id)
{
    list_head* head = &m_timeout_list_head_;
    if ( list_empty(head) )
    {
        return NULL;
    }

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_timeout_item_);
        if (eh->m_net_id_ == tunnel_id)
        {
            return eh;
        }
    }

    return NULL;
}

void event_handler::push_handler_to_timeout_list(event_handler* eh)
{
    if (NULL == eh)
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

    list_del(&eh->m_timeout_item_);
    list_head* head = &m_timeout_list_head_;
    list_add_tail(&eh->m_timeout_item_, head);
}

void event_handler::remove_handler_from_timeout_list(event_handler* eh)
{
    if (NULL == eh)
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

    list_del(&eh->m_timeout_item_);
}

void event_handler::remove_handler_from_timeout_list(uint32_t tunnel_id)
{
    list_head* head = &m_timeout_list_head_;
    if ( list_empty(head) )
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_timeout_item_);
        if (eh->m_net_id_ == tunnel_id)
        {
            list_del(pos);
            break;
        }
    }
}


// notify close list
void event_handler::init_notify_close_list()
{
    INIT_LIST_HEAD(&m_notify_close_list_head_);
}

void event_handler::clear_notify_close_list()
{
    list_head* head = &m_notify_close_list_head_;
    if ( list_empty(head) )
    {
        return;
    }

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_notify_close_item_);
        event_handler::remove_handler_from_notify_close_list(eh);
    }
}

event_handler* event_handler::get_handler_from_notify_close_list(uint32_t tunnel_id)
{
    list_head* head = &m_notify_close_list_head_;
    if ( list_empty(head) )
    {
        return NULL;
    }

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_notify_close_item_);
        if (eh->m_net_id_ == tunnel_id)
        {
            return eh;
        }
    }

    return NULL;
}

void event_handler::push_handler_to_notify_close_list(event_handler* eh)
{
    if (NULL == eh)
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

    list_del(&eh->m_notify_close_item_);
    list_head* head = &m_notify_close_list_head_;
    list_add_tail(&eh->m_notify_close_item_, head);
}

void event_handler::remove_handler_from_notify_close_list(event_handler* eh)
{
    if (NULL == eh)
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

    list_del(&eh->m_notify_close_item_);
}

void event_handler::remove_handler_from_notify_close_list(uint32_t tunnel_id)
{
    list_head* head = &m_notify_close_list_head_;
    if ( list_empty(head) )
    {
        return;
    }

    threadc_mutex_guard lock(m_mutex_);

    list_head* pos;
    list_head* n;
    event_handler* eh = NULL;
    list_for_each_safe(pos, n, head)
    {
        eh = list_entry(pos, event_handler, m_notify_close_item_);
        if (eh->m_net_id_ == tunnel_id)
        {
            list_del(pos);
            break;
        }
    }
}


// set / cancel time out and notify close
void event_handler::set_timeout(event_handler* eh, time_t timeout)
{
    if (NULL == eh)
    {
        return;
    }

    if ( timeout > 0 )
    {
        eh->m_timeout_ = time(NULL) + timeout;
        event_handler::push_handler_to_timeout_list(eh);
    }
    else
    {
        eh->m_timeout_ = 0;
        event_handler::remove_handler_from_timeout_list(eh);
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

void event_handler::on_notify_close()
{
    list_head* head = &m_notify_close_list_head_;
    if ( list_empty(head) )
    {
        return;
    }

    list_head* pos;
    list_head* n;
    list_for_each_safe(pos, n, head)
    {
        event_handler* eh = list_entry(pos, event_handler, m_notify_close_item_);
        // delete timeout from list, delete hash from list and close, and delete handler
        eh->handle_close(net_event::NE_NOTIFY_CLOSE);
    }
}
