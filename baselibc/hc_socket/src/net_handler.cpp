#include "net_handler.h"

#include "hc_stack_trace.h"

#include "ihandler.h"
#include "net_event.h"
#include "net_id_holder.h"
#include "sock_connector.h"

net_handler::net2handler_hashmap net_handler::m_net2hdr_hashmap_;

ihandler* net_handler::select_handler(int32_t net_id)
{
    STACK_TRACE_LOG();
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

    m_net2hdr_hashmap_.insert(std::make_pair(ish->handler_o(), ish));
}

void net_handler::remove_handler(ihandler* ish)
{
    STACK_TRACE_LOG();
    if ( NULL == ish )
    {
        return;
    }

    ////delete from connect timeout list
    //if ( ( ish->handler_o() >= net_id_holder::TCP_CONNECTOR_BEGIN ) 
    //    && ( ish->handler_o() <= net_id_holder::TCP_ACCEPTOR_END ) )
    //{
    //    sock_connector::sync_timeout((sock_connector*)ish, 0);
    //}

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
    net2handler_hashmap::iterator iter = m_net2hdr_hashmap_.find(net_id);
    if ( m_net2hdr_hashmap_.end() != iter )
    {
        //ihandler* ish = iter->second;
        //if ( NULL != ish )
        //{
        //    //delete from connect timeout list
        //    if ( ( ish->handler_o() >= net_id_holder::TCP_CONNECTOR_BEGIN ) 
        //        && ( ish->handler_o() <= net_id_holder::TCP_ACCEPTOR_END ) )
        //    {
        //        sock_connector::sync_timeout((sock_connector*)ish, 0);
        //    }
        //}

        //delete from hash list
        m_net2hdr_hashmap_.erase(iter);
    }
}

void net_handler::clear_all_handler()
{
    STACK_TRACE_LOG();
    net2handler_hashmap::iterator iter = m_net2hdr_hashmap_.begin();
    for ( ; m_net2hdr_hashmap_.end() != iter; iter++ )
    {
        ihandler* ish = iter->second;
        iter->second = NULL;
        //delete from hash list
        //m_net2hdr_hashmap_.erase(iter);

        if ( NULL != ish )
        {
            ////delete from connect timeout list
            //if ( ( ish->handler_o() >= net_id_holder::TCP_CONNECTOR_BEGIN ) 
            //    && ( ish->handler_o() <= net_id_holder::TCP_ACCEPTOR_END ) )
            //{
            //    sock_connector::sync_timeout((sock_connector*)ish, 0);
            //}

            //delete hash from list, delete timeout from list and close, delete handler
            ish->handle_close(net_event::NE_NOTIFY_CLOSE);
        }
    }
}

int32_t net_handler::current_handler_count()
{
    return m_net2hdr_hashmap_.size();
}


