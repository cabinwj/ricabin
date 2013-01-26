#include "net_event_handler.h"

#include "hc_log.h"

#include "net_package.h"

#include "entity_types.h"
#include "keep_alive.h"
#include "net_node.h"
#include "xml_configure.h"
#include "session_list.h"

#include "client_session.h"
#include "connect_session.h"


#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif


void on_net_event_handler(net_event& ne)
{
    LOG(INFO)("on_net_event_handler() dispatch net<%d:%d:%d>", ne.m_listen_net_id_, ne.m_net_id_, ne.m_net_ev_t_);

    conn_param_t* cp = (conn_param_t*)(ne.m_user_data_);

    switch (ne.m_net_ev_t_)
    {
        // 仅对连出去的连接
    case net_event::NE_CONNECTED: {

        net_node* conn = new connect_session;
        conn->m_net_id_ = ne.m_net_id_;
        conn->m_listen_net_id_ = 0;
        conn->m_remote_uin_ = cp->m_remote_uin_;
        conn->m_is_trans_.set(is_trans_connect_out);

        session_list::on_connected(conn);

        keep_alive::on_send_alive(conn->m_net_id_, message_async_syn);

    } break;
        // 仅对连进来的连接
    case net_event::NE_ACCEPT: {

        // 这里只有client 才连进来
        net_node* client = new client_session;

        client->m_net_id_ = ne.m_net_id_;
        client->m_listen_net_id_ = ne.m_listen_net_id_;
        client->m_is_trans_.set(is_trans_accept_client);

        client->m_last_recv_timestamp_ = (uint32_t)time(NULL);
        client->m_remote_addr_ = ne.m_remote_addr_;

        session_list::on_accepted(client); 

    } break;
        // 用户自己实现
    case net_event::NE_DATA: {

        net_node* client = session_list::get_net_node_by_net_id(ne.m_net_id_);
        if (NULL == client)
        {
            LOG(ERROR)("on_net_event_handler() session_list::get_net_node_by_net_id return NULL.");
            return;
        }

        if (NULL != ne.m_net_package_)
        {
            client->on_ne_data(ne);
        }

    } break;

    case net_event::NE_CLOSE:
    case net_event::NE_EXCEPTION:
        // 判断是否要重连 仅对连出去的连接
    case net_event::NE_TIMEOUT:
        // 应用层主动关闭 判断是否要重连
    case net_event::NE_NOTIFY_CLOSE: {

        net_node* client = session_list::get_net_node_by_net_id(ne.m_net_id_);
        if (NULL == client)
        {
            LOG(ERROR)("on_net_event_handler() session_list::get_net_node_by_net_id return NULL.");
            return;
        }

        if (client->m_is_trans_.is_setted(is_trans_connect_out) && (0 != cp->m_reconnect_interval_))
        {
            session_list::append_reconnect_list(client->m_remote_uin_, cp->m_reconnect_interval_);
        }

        session_list::on_disconnect_by_net_id(ne.m_net_id_);

    } break;

    default: {
        LOG(ERROR)("on_net_event_handler() error, unknown");
    } break;

    }
}

