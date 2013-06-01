#include "session_list.h"
#include "hc_stack_trace.h"


#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif


// net_info_list
list_head session_list::m_net_hash_bucket_[session_list::HASH_BUCKET_COUNT_];
list_head session_list::m_uin_hash_bucket_[session_list::HASH_BUCKET_COUNT_];
list_head session_list::m_listen_list_;
list_head session_list::m_connect_list_;
list_head session_list::m_accepts_list_;
list_head session_list::m_reconnect_list_;

void session_list::init()
{
    STACK_TRACE_LOG();

    INIT_LIST_HEAD(&m_listen_list_);
    INIT_LIST_HEAD(&m_connect_list_);
    INIT_LIST_HEAD(&m_accepts_list_);
    INIT_LIST_HEAD(&m_reconnect_list_);

    for (int i =0; i < session_list::HASH_BUCKET_COUNT_; i++)
    {
        INIT_LIST_HEAD(&m_net_hash_bucket_[i]);
        INIT_LIST_HEAD(&m_uin_hash_bucket_[i]);
    }
}

void session_list::on_connected(net_node* conn)
{
    STACK_TRACE_LOG();

    // add 2 connect list
    list_add_tail(&conn->m_list_item_, &m_connect_list_);
    // add 2 hash list
    int32_t net_npos = session_list::hash_func(conn->m_net_id_);
    list_add_tail(&conn->m_net_hash_item_, &m_net_hash_bucket_[net_npos]);
}

void session_list::on_accepted(net_node* client)
{
    STACK_TRACE_LOG();

    // add 2 hash list
    int32_t npos = session_list::hash_func(client->m_net_id_);
    list_add_tail(&client->m_net_hash_item_, &m_net_hash_bucket_[npos]);
}

void session_list::on_disconnect(net_node* client)
{
    STACK_TRACE_LOG();

    if (NULL == client)
    {
        return;
    }

    // delete from accept or connect list
    list_del(&client->m_list_item_);
    // delete from hash list
    list_del(&client->m_net_hash_item_);

    if (client->m_is_trans_.is_setted(is_trans_accept_client))
    {
        // delete from uin hash list
        list_del(&client->m_uin_hash_item_);

        // check listen list
        list_head* pos = NULL;
        if (0 != client->m_listen_net_id_)
        {
            uint32_t listen_net_id = client->m_listen_net_id_;
            list_for_each(pos, &m_listen_list_)
            {
                listen_node* listen = list_entry(pos, listen_node, m_listen_item_);
                if (listen->m_listen_net_id_ == listen_net_id)
                {
                    if (list_empty(&listen->m_accept_list_))
                    {
                        // delete from listen list
                        list_del(&listen->m_listen_item_);
                        delete listen;
                        break;
                    }
                }
            }
        }
    }

    delete client;
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
        // add 2 accepts_list
        list_add_tail(&client->m_list_item_, &m_accepts_list_);
    }
    
    if (client->m_is_trans_.is_setted(is_trans_accept_client))
    {
        list_head* pos = NULL;
        listen_node* listen = NULL;
        list_for_each(pos, &m_listen_list_)
        {
            listen = list_entry(pos, listen_node, m_listen_item_);
            if (listen->m_listen_net_id_ == client->m_listen_net_id_)
            {
                break;
            }
        }

        if (NULL == listen)
        {
            listen = new listen_node;
            listen->m_listen_net_id_ = client->m_listen_net_id_;
            INIT_LIST_HEAD(&listen->m_listen_item_);
            INIT_LIST_HEAD(&listen->m_accept_list_);
            // add 2 listen list
            list_add_tail(&listen->m_listen_item_, &m_listen_list_);
        }

        // add 2 accept list
        list_add_tail(&client->m_list_item_, &listen->m_accept_list_);

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

net_node* session_list::get_net_node_by_net_id(uint32_t net_id)
{
    STACK_TRACE_LOG();

    int32_t npos = session_list::hash_func(net_id);

    list_head* head = &m_net_hash_bucket_[npos];
    if ( list_empty(head) )
    {
        return NULL;
    }

    list_head* pos;
    list_head* n;
    net_node* clients = NULL;
    list_for_each_safe(pos, n, head)
    {
        clients = list_entry(pos, net_node, m_net_hash_item_);
        if (clients->m_net_id_ == net_id)
        {
            return clients;
        }
    }

    return NULL;
}

net_node* session_list::get_net_node_by_uin(uint32_t uin, is_trans_t is_trans)
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

        list_head* pos;
        list_head* n;
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

        list_head* pos;
        list_head* n;
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

        list_head* pos;
        list_head* n;
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


void session_list::append_reconnect_list(uint32_t uin, uint16_t reconnect_interval)
{
    STACK_TRACE_LOG();

    reconn_node* reconn = new reconn_node;
    reconn->m_next_reconnect_time_ = (uint32_t)time(NULL) + reconnect_interval;
    reconn->m_uin_ = uin;
    INIT_LIST_HEAD(&reconn->m_list_item_);
    // add 2 reconnect list
    list_add_tail(&reconn->m_list_item_, &m_reconnect_list_);
}