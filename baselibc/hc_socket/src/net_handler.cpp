#include "net_handler.h"

#include "hc_stack_trace.h"

#include "ihandler.h"
#include "net_event.h"
#include "net_id_holder.h"
#include "sock_connector.h"

net_handler::net2handler_hashmap net_handler::m_net2hdr_hashmap_;
//rw_lockc net_handler::m_rwlock_;
//threadc_mutex net_handler::m_mutex_;

ihandler* net_handler::select_handler(int32_t net_id)
{
    STACK_TRACE_LOG();

    //write_lock_holder lock(&m_rwlock_);
    //threadc_mutex_guard lock(&m_mutex_);
    net2handler_hashmap::iterator iter = m_net2hdr_hashmap_.find(net_id);
    if ( m_net2hdr_hashmap_.end() != iter )
    {
        if ( NULL == iter->second )
        {
            m_net2hdr_hashmap_.erase(iter);
            return NULL;
        }
        
        return iter->second;
    }

    return NULL;
}

void net_handler::insert_handler(ihandler* ish)
{
    STACK_TRACE_LOG();

    if ( NULL == ish )
    {
        return;
    }

    //write_lock_holder lock(&m_rwlock_);
    //threadc_mutex_guard lock(&m_mutex_);
    m_net2hdr_hashmap_.insert(std::make_pair(ish->handler_o(), ish));
}

void net_handler::remove_handler(ihandler* ish)
{
    STACK_TRACE_LOG();
    if ( NULL == ish )
    {
        return;
    }

    //write_lock_holder lock(&m_rwlock_);
    //threadc_mutex_guard lock(&m_mutex_);
    //delete from hash list
    net2handler_hashmap::iterator iter = m_net2hdr_hashmap_.find(ish->handler_o());
    if ( m_net2hdr_hashmap_.end() != iter )
    {
        m_net2hdr_hashmap_.erase(iter);
    }
}

void net_handler::remove_handler(int32_t net_id)
{
    STACK_TRACE_LOG();

    //write_lock_holder lock(&m_rwlock_);
    //threadc_mutex_guard lock(&m_mutex_);
    net2handler_hashmap::iterator iter = m_net2hdr_hashmap_.find(net_id);
    if ( m_net2hdr_hashmap_.end() != iter )
    {
        //delete from hash list
        m_net2hdr_hashmap_.erase(iter);
    }
}

void net_handler::clear_all_handler()
{
    STACK_TRACE_LOG();

    //write_lock_holder lock(&m_rwlock_);
    //threadc_mutex_guard lock(&m_mutex_);
    net2handler_hashmap::iterator iter = m_net2hdr_hashmap_.begin();
    while ( m_net2hdr_hashmap_.end() != iter )
    {
        ihandler* ish = iter->second;
        if ( NULL != ish )
        {
            //delete hash from list, delete timeout from list and close, delete handler
            ish->handle_close(net_event::NE_NOTIFY_CLOSE);
        }

        //delete from hash list
        //iter = m_net2hdr_hashmap_.erase(iter);
        iter->second = NULL;
        iter++;
    }
}

int32_t net_handler::current_handler_count()
{
    //read_lock_holder lock(&m_rwlock_);
    //threadc_mutex_guard lock(&m_mutex_);
    return m_net2hdr_hashmap_.size();
}


