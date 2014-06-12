#include "net_event_handler.h"

#include "hc_log.h"
#include "hc_binpacket_tlv.h"
#include "net_package.h"

#include "session_list.h"

#include "entity_types.h"
#include "keepalive_list.h"
#include "accept_session.h"
//#include "connect_session.h"


void net_event_handler(net_event& netev)
{
    LOG(INFO)("net_event_handler() dispatch net<%d:%d>", netev.m_net_id_, netev.m_net_ev_t_);

    conn_param_t* cp = (conn_param_t*)(netev.m_user_data_);

    switch (netev.m_net_ev_t_)
    {
        // ��������ȥ������
    case net_event::NE_CONNECTED: {

        //net_node* conn = new connect_session;
        //conn->m_net_id_ = ne.m_net_id_;
        //conn->m_listen_net_id_ = 0;
        //conn->m_remote_uin_ = cp->m_remote_uin_;
        //conn->m_is_trans_.set(is_trans_connect_out);

        //session_list::on_connected(conn);

        if (cp->m_keepalive_interval_ > 0)
        {
            keepalive::on_connected(netev.m_net_id_, cp->m_keepalive_interval_, cp->m_keepalive_timeout_);
        }

        keepalive::on_send_alive(netev.m_net_id_, message_async_syn);

    } break;
        // ����������������
    case net_event::NE_ACCEPT: {

        if (cp->m_keepalive_timeout_ > 0)
        {
            keepalive::on_accepted(netev.m_net_id_, cp->m_keepalive_timeout_);
        }

        // ����ֻ��client ��������
        net_node* accept = new accept_session;
        accept->m_net_id_ = netev.m_net_id_;
        accept->m_last_recv_timestamp_ = (uint32_t)time(NULL);
        accept->m_remote_addr_ = netev.m_remote_addr_;

        session_list::on_accepted(accept); 

    } break;
        // �û��Լ�ʵ��
    case net_event::NE_DATA: {

        input_binpacket inpkg(netev.m_net_package_->data_o(), netev.m_net_package_->size());
        net_hdr_t phdr;
        inpkg.read((char*)(&phdr), sizeof(net_hdr_t));

        // ��������ע��������֮�������������Ϊ����ͻ�Ӧ message_async_syn/ message_async_ack��
        if ( 0 == phdr.m_message_id_ )
        {
            LOG(INFO)("net_event_handler() receive keep alive heart beat. net<%u:%u>", netev.m_net_id_, netev.m_net_ev_t_);
            // ����ֻ��һ����ͷ�ĳ���
            if (!inpkg.is_init()) return;

            keepalive::on_recv_alive(netev.m_net_id_);

            // �˴����������������������Ҫ��Ӧ���Ա��ⲻ��ϵ����ط���
            if ( message_syn == phdr.m_message_type_ )
            {
                // send alive response here
                keepalive::on_send_alive(netev.m_net_id_, message_ack);

                net_node* session = session_list::net_node_by_net_id_o(netev.m_net_id_);
                if ( NULL == session )
                {
                    LOG(ERROR)("net_event_handler() session_list::get_net_node_by_net_id return NULL.");
                    return;
                }

                if ( (message_alive == phdr.m_control_type_) && 
                        (netev.m_net_id_ == phdr.m_client_net_id_) &&
                        (0 != phdr.m_reserved_ ) &&
                        (phdr.m_reserved_ == phdr.m_client_uin_) &&
                        (phdr.m_reserved_ == phdr.m_from_uin_) )
                {
                    // ������ server ������������������client�������� remote_uin �������б�
                    session->m_remote_uin_ = phdr.m_reserved_;
                    session->m_is_trans_.set(is_trans_accept_server);
                    session_list::append_uin_to_list(session);
                }
            }
        }

        net_node* session = session_list::net_node_by_net_id_o(netev.m_net_id_);
        if (NULL == session)
        {
            LOG(ERROR)("net_event_handler() session_list::net_node_by_net_id_o return NULL.");
            return;
        }

        if (NULL != netev.m_net_package_)
        {
            session->on_ne_data(netev);
        }

    } break;

    case net_event::NE_CLOSE:
    case net_event::NE_EXCEPTION: {

        if (cp->m_keepalive_interval_ || cp->m_keepalive_timeout_)
        {
            keepalive::on_disconnect(netev.m_net_id_);
        }

    } break;
        // �ж��Ƿ�Ҫ���� ��������ȥ������
    case net_event::NE_TIMEOUT:
        // Ӧ�ò������ر� �ж��Ƿ�Ҫ����
    case net_event::NE_NOTIFY_CLOSE: {

        net_node* client = session_list::net_node_by_net_id_o(netev.m_net_id_);
        if (NULL == client)
        {
            LOG(ERROR)("net_event_handler() session_list::net_node_by_net_id_o return NULL.");
            return;
        }

        session_list::on_disconnect_by_net_id(netev.m_net_id_);

    } break;

    default: {
        LOG(ERROR)("net_event_handler() error. unknown");
    } break;

    }
}

