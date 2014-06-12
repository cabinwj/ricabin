#include "session_list.h"
#include "hc_stack_trace.h"
#include "net_node.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif


// net_info_list
list_head session_list::m_net_hash_bucket_[session_list::HASH_BUCKET_COUNT_];
list_head session_list::m_uin_hash_bucket_[session_list::HASH_BUCKET_COUNT_];
list_head session_list::m_acceptc_list_;
list_head session_list::m_accepts_list_;
list_head session_list::m_connect_list_;

int32_t session_list::hash_func(int32_t key)
{
    return (int32_t)(key % session_list::HASH_BUCKET_COUNT_);
}

void session_list::init()
{
    STACK_TRACE_LOG();

    INIT_LIST_HEAD(&m_acceptc_list_);
    INIT_LIST_HEAD(&m_connect_list_);
    INIT_LIST_HEAD(&m_accepts_list_);

    for (int i =0; i < session_list::HASH_BUCKET_COUNT_; i++)
    {
        INIT_LIST_HEAD(&m_net_hash_bucket_[i]);
        INIT_LIST_HEAD(&m_uin_hash_bucket_[i]);
    }
}

void session_list::on_connected(net_node* conn)
{
    STACK_TRACE_LOG();

    if (NULL == conn)
    {
        return;
    }

    // add 2 connect list
    list_add_tail(&conn->m_list_item_, &m_connect_list_);
    // add 2 hash list
    int32_t npos = session_list::hash_func(conn->m_net_id_);
    list_add_tail(&conn->m_net_hash_item_, &m_net_hash_bucket_[npos]);
}

void session_list::on_accepted(net_node* client)
{
    STACK_TRACE_LOG();

    if (NULL == client)
    {
        return;
    }

    // add 2 hash list
    int32_t npos = session_list::hash_func(client->m_net_id_);
    list_add_tail(&client->m_net_hash_item_, &m_net_hash_bucket_[npos]);
}

void session_list::on_disconnect(net_node* conn)
{
    STACK_TRACE_LOG();

    if (NULL == conn)
    {
        return;
    }

    // delete from accept or connect list
    list_del(&conn->m_list_item_);
    // delete from hash list
    list_del(&conn->m_net_hash_item_);

    //// delete from uin hash list
    //list_del(&conn->m_uin_hash_item_);

    if (conn->m_is_trans_.is_setted(is_trans_accept_client))
    {
        // delete from uin hash list
        list_del(&conn->m_uin_hash_item_);
    }
}

void session_list::append_uin_to_list(net_node* client)
{
    STACK_TRACE_LOG();

    if (NULL == client)
    {
        return;
    }

    if (client->m_is_trans_.is_setted(is_trans_accept_server))
    {
        // add 2 accept server list
        list_add_tail(&client->m_list_item_, &m_accepts_list_);
    }
    
    if (client->m_is_trans_.is_setted(is_trans_accept_client))
    {
        // add 2 accept client list
        list_add_tail(&client->m_list_item_, &m_acceptc_list_);

        // add 2 uin hash list 
        int32_t npos = session_list::hash_func(client->m_remote_uin_);
        list_add_tail(&client->m_uin_hash_item_, &m_uin_hash_bucket_[npos]);
    }
}

void session_list::on_disconnect_by_net_id(uint32_t net_id)
{
    STACK_TRACE_LOG();

    list_head* pos = NULL;
    net_node* alive = NULL;
    int32_t npos = session_list::hash_func(net_id);
    if ( list_empty(&m_net_hash_bucket_[npos]) )
    {
        return;
    }

    list_for_each(pos, &m_net_hash_bucket_[npos])
    {
        alive = list_entry(pos, net_node, m_net_hash_item_);
        if (alive->m_net_id_ == net_id)
        {
            break;
        }
    }

    if (NULL == alive)
    {
        return;
    }

    session_list::on_disconnect(alive);
}

net_node* session_list::net_node_by_net_id_o(uint32_t net_id)
{
    STACK_TRACE_LOG();

    int32_t npos = session_list::hash_func(net_id);

    list_head* head = &m_net_hash_bucket_[npos];
    if ( list_empty(head) )
    {
        return NULL;
    }

    list_head* pos = NULL;
    list_head* n = NULL;
    net_node* client = NULL;
    list_for_each_safe(pos, n, head)
    {
        client = list_entry(pos, net_node, m_net_hash_item_);
        if (client->m_net_id_ == net_id)
        {
            return client;
        }
    }

    return NULL;
}

net_node* session_list::net_node_by_uin_o(uint32_t uin, is_trans_t is_trans)
{
    STACK_TRACE_LOG();

    switch (is_trans)
    {
    case is_trans_accept_client: {

        int32_t npos = session_list::hash_func(uin);
        list_head* head = &m_uin_hash_bucket_[npos];
        if ( list_empty(head) )
        {
            return NULL;
        }

        list_head* pos = NULL;
        list_head* n = NULL;
        net_node* client = NULL;
        list_for_each_safe(pos, n, head)
        {
            client = list_entry(pos, net_node, m_uin_hash_item_);
            if (client->m_remote_uin_ == uin)  // clients->m_is_trans_.is_setted(is_trans)?
            {
                return client;
            }
        }

    } break;

    case is_trans_accept_server: {

        if ( list_empty(&m_accepts_list_) )
        {
            return NULL;
        }

        list_head* pos = NULL;
        list_head* n = NULL;
        net_node* client = NULL;
        list_for_each_safe(pos, n, &m_accepts_list_)
        {
            client = list_entry(pos, net_node, m_uin_hash_item_);
            if (client->m_remote_uin_ == uin)  // clients->m_is_trans_.is_setted(is_trans)?
            {
                return client;
            }
        }

    } break;

    case is_trans_connect_out: {

        if ( list_empty(&m_connect_list_) )
        {
            return NULL;
        }

        list_head* pos = NULL;
        list_head* n = NULL;
        net_node* client = NULL;
        list_for_each_safe(pos, n, &m_connect_list_)
        {
            client = list_entry(pos, net_node, m_uin_hash_item_);
            if (client->m_remote_uin_ == uin)  // clients->m_is_trans_.is_setted(is_trans)?
            {
                return client;
            }
        }

    } break;

    default:
        break;
    }

    return NULL;
}

