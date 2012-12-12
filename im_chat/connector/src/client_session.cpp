#include "client_session.h"

#include "common_allocator.h"
#include "common_binpacket.h"

#include "net_manager.h"
#include "net_package.h"

#include "frame_base.h"
#include "entity_types.h"
#include "session_list.h"
#include "keep_alive.h"
#include "xml_configure.h"

#include "connect_session.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/time.h>
#include <unistd.h>
#endif

class connect_session;

client_session::client_session()
{
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_net_hash_item_);

    m_net_id_ = 0;
    m_listen_net_id_ = 0;

    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);

    memset(&m_signature_, 0, sizeof(signature_t));
}

client_session::~client_session()
{
}

// net event handler
void client_session::on_ne_data(net_event& ne)
{
    LOG(INFO)("client_session::on_ne_data() recv data from net<%u:%u:%u>, remote_addr<0x%08X:%d>", ne.m_listen_net_id_, ne.m_net_id_, ne.m_net_ev_t_, ne.m_remote_addr_.get_net_ip(), ne.m_remote_addr_.get_net_port());

    struct timeval process_begin;
    gettimeofday(&process_begin, NULL);

    bool is_recved_data = m_is_trans_.is_setted(is_trans_recved_data);

    binary_input_packet<true> inpkg(ne.m_net_package_->get_data(), ne.m_net_package_->size());

    //inpkg.offset_head(sizeof(proto_hdr_t));
    //inpkg.get_head(m_conn_hdr_);

    // ��һ���ֶ�(packet_len�ֶ�) �ڶ����ֶ�(client_uin�ֶ�) �������ֶ�(option_len: option���ȵ��ֶ�)
    inpkg >> m_conn_hdr_.m_packet_len_ >> m_conn_hdr_.m_client_uin_ >> m_conn_hdr_.m_option_len_;
    // ��֤���ݰ���
    if (inpkg.size() != m_conn_hdr_.m_packet_len_)
    {
        LOG(ERROR)("client_session::on_net_data() packet_len is invalue");
        return;
    }

    if ((m_conn_hdr_.m_option_len_ < 0) || (m_conn_hdr_.m_option_len_ > max_option_length))
    {
        LOG(ERROR)("client_session::on_ne_data() option length is invalue");
        if (is_recved_data) { clear_current_net(close_reason_data_error); } //��Ҫ�ϱ�Service
        else { clear_current_net(close_reason_invalid_signature); }            
        return;
    }

    // ���ĸ��ֶ�(m_option_�ֶ�) �������ĸ��ֶΣ���ѡ��option ��Ϊ��ҵ� signature
    memset(m_conn_hdr_.m_option_, 0, sizeof(m_conn_hdr_.m_option_));
    inpkg.read(m_conn_hdr_.m_option_, m_conn_hdr_.m_option_len_);

    // ������ֶ�(message_id�ֶ�) �������ֶ�(message_type�ֶ�) ���߸��ֶ�(player_id�ֶ�)
    inpkg >> m_conn_hdr_.m_message_id_ >> m_conn_hdr_.m_message_type_ >> m_conn_hdr_.m_player_id_;

    // get crypt code offset from code
    // ��sequence��ʼ�������ܲ��֡�       
    uint32_t crypt_code_offset = min_cs_header_length + m_conn_hdr_.m_option_len_; 
    // ע����ԷǼ��ܲ��ֵ�ƫ�ƽ��м��
    if (crypt_code_offset <= 0 || crypt_code_offset >= inpkg.size()) //���ƹ��� ���ߣ�
    {
        LOG(ERROR)("client_session::on_ne_data() crypt_code_offset is invalue.");
        if (is_recved_data) { clear_current_net(close_reason_data_error); } //��Ҫ�ϱ�Service
        else { clear_current_net(close_reason_invalid_signature); } 
        return;
    }

    // ��һ���յ��Է�����������(first packet) �������֤ǩ��
    if (!is_recved_data)
    {
        // �����Ѿ���peer�յ�������
        m_is_trans_.set(is_trans_recved_data);

        if (0 == m_conn_hdr_.m_option_len_)
        {
            LOG(ERROR)("client_session::on_ne_data() recv first package but option length is 0");
            clear_current_net(close_reason_invalid_signature);                        
            return;
        }

        // ���ܳ���ҵ�signature
        if (0 != decrypt_signature(m_conn_hdr_.m_option_, m_conn_hdr_.m_option_len_, m_signature_))
        {
            LOG(ERROR)("client_session::on_ne_data() decrypt signature is invalue.");
            clear_current_net(close_reason_invalid_signature); 
            return;
        }

        // ��֤��ҵ�UIN�Ƿ���ǩ���е�UINһ����
        if (m_signature_.m_client_uin_ != m_conn_hdr_.m_client_uin_)
        {
            LOG(ERROR)("client_session::on_ne_data() client uin error, m_signature_.m_client_uin_=%d,client_uin=%d", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
            clear_current_net(close_reason_invalid_signature); 
            return;
        }
            
        // ����UIN��������
        m_remote_uin_ = m_conn_hdr_.m_client_uin_;

        // ���ǩ������Ч���� ��Ӧ���ߣ�
        time_t now = time(NULL);
        if (now - (time_t)m_signature_.m_timestamp_ >= max_signature_valid_period)
        {
            LOG(ERROR)("client_session::on_ne_data() the signature of client uin[%d] was expired! so discarded package!", m_conn_hdr_.m_client_uin_);
            clear_current_net(close_reason_invalid_signature); 
            return;
        }

        // ���뵽 uin hash list
        session_list::append_uin_to_list(this);
    }
    else
    {
        // ��֤��ҵ�UIN�Ƿ���ǩ���е�UINһ����
        if (m_signature_.m_client_uin_ != m_conn_hdr_.m_client_uin_)
        {
            LOG(ERROR)("client_session::on_ne_data() client uin error, m_signature_.m_client_uin_=%d,client_uin=%d. so discarded package!", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
            return;
        }
    }

    // �����Ѿ���peer�յ����ݵ�ʱ����
    m_last_recv_timestamp_ = (uint32_t)time(NULL); 

    // ����������CSЭ�鲿�֡�
    // ��session key������
    int32_t out_length = (max_client_pkg_size + 100 - crypt_code_offset); 
    int rc = decrypt_buffer(m_signature_.m_session_key_,
                            (inpkg.get_data() + crypt_code_offset),
                            (m_conn_hdr_.m_packet_len_ - crypt_code_offset),
                            (inpkg.get_data() + crypt_code_offset),
                            out_length);
    if (0 == rc)
    {
        LOG(ERROR)("client_session::on_ne_data() decry_buffer error, m_signature_.m_client_uin_=%d,client_uin=%d,", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
        if (is_recved_data) { clear_current_net(close_reason_data_error); } //��Ҫ�ϱ�Service
        else { clear_current_net(close_reason_invalid_signature); } 
        return;
    }

    // get sequence inpkg current ����       
    inpkg >> m_conn_hdr_.m_request_sequence_;

    xml_configure& condxml = GET_XML_CONFIG_INSTANCE();

    uint32_t ent_id = condxml.get_server_entity_id(m_conn_hdr_.m_client_uin_, pf_entity_router); 
    connect_session* conn = (connect_session*)session_list::get_net_node_by_uin(ent_id, is_trans_connect_out); 
    if (NULL == conn)
    {
        LOG(ERROR)("client_session::on_ne_data() get_net_node_by_uin error, m_signature_.m_client_uin_=%d,client_uin=%d,", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
        return;
    }

    // ����CSЭ������ݰ���
    uint32_t decrypt_packet_len = out_length + sizeof(net_hdr_t) - 4; // sizeof(m_conn_hdr_.m_request_sequence_);

    // ��net_header �����ǩ��(ʹ�������ֽ������)���ݸ���˵�service?
    net_hdr_t net_hdr;
    net_hdr.m_packet_len_ = decrypt_packet_len;
    net_hdr.m_message_id_ = m_conn_hdr_.m_message_id_;
    net_hdr.m_message_type_ = m_conn_hdr_.m_message_type_;
    net_hdr.m_request_sequence_ = m_conn_hdr_.m_request_sequence_;
    net_hdr.m_control_type_ = is_recved_data ? message_common : message_start_request;
    net_hdr.m_client_net_id_ = m_net_id_;
    net_hdr.m_client_uin_ = m_conn_hdr_.m_client_uin_;
    net_hdr.m_from_uin_ = condxml.m_net_xml_[condxml.m_use_index_].m_common_.m_entity_id_;
    net_hdr.m_to_uin_ = condxml.get_server_entity_id(m_conn_hdr_.m_client_uin_, pf_entity_logic);  // �˴�Ҫ����Ŀ�����ͻ���Ψһ��ʶ

    // �����µ� �����
    net_package* np = event_handler::m_net_pkg_pool_->Create();
    if ( NULL == np )
    {
        LOG(ERROR)("assert: client_session::on_ne_data() error. new np is NULL");
        return;
    }

    np->allocator_data_block(new_allocator::Instance(), decrypt_packet_len);

    // ��� �µ������
    binary_output_packet<true> outpkg(np->get_data(), np->capacity());
    outpkg.offset_head(sizeof(net_hdr_t));
    outpkg.set_head(net_hdr);

    np->offset_cursor(decrypt_packet_len);

    // �ж�message_id����Ϣ��Χ�Ϸ��ԣ�
    switch (net_hdr.m_message_id_)
    {
    case C_KEEPALIVE:
    case C_LOGIN:
    case C_LOGIN_OVER:
    case C_LOGOUT:
    case C_FORCE_LOGOUT:
    case C_MODIFY_USER_INFO:
    case C_GET_FRIEND_STATUS:
    case C_GET_FRIEND_GROUP:
    case C_GET_FRIEND_LIST:
    case C_ADD_FRIEND_GROUP:
    case C_REMOVE_FRIEND_GROUP:
    case C_ADD_FRIEND:
    case C_REMOVE_FRIEND:
    case C_MODIFY_FRIEND_GROUP:
    case C_MODIFY_FRIEND_GROUP_NAME:
    case C_GET_FRIEND_INFO:
    case C_AGREE_ADD_FRIEND:
    case C_REFUSE_ADD_FRIEND:
    case C_GET_OFFLINE_MSG:
    case C_REMOVE_OFFLINE_MSG:
    case C_CHAT_MSG:
    case C_CHAT_MSG_ACK:
    case C_FRIEND_STATUS_CHANGE_NOTICE:
    case C_FRIEND_RELATION_CHANGE_NOTICE:
    case C_P2P_TRANSPARENT:
    case C_PICTURE_CHANGE_NOTICE:
    case C_PROFILE_CHANGE_NOTICE:
        LOG(INFO)("client_session::on_ne_data() send_package, m_signature_.m_client_uin_=%d,client_uin=%d,", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
        rc = net_manager::Instance()->send_package(conn->m_net_id_, np);
        if (0 != rc)
        {
            np->Destroy();
        }

        // ��ʶ��conn��ǰ�����ϣ���������Ѿ�����������
        conn->m_is_trans_.set(is_trans_sended_data);
        
        break;
    default:
        break;
    }

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("client_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}


void client_session::clear_current_net(uint16_t close_reason)
{
    LOG(INFO)("client_session::clear_current_net() client uin:%u net<%u:%u>, remote_addr<0x%08X:%d>", m_remote_uin_, m_listen_net_id_, m_net_id_, m_remote_addr_.get_net_ip(), m_remote_addr_.get_net_port());

    xml_configure& condxml = GET_XML_CONFIG_INSTANCE();

    // �������ĶϿ����ӣ���֪ͨҵ����������ж�
    if ( ( close_reason_service != close_reason) 
        && (close_reason_invalid_connect != close_reason)
        && (close_reason_invalid_signature != close_reason) )
    {
        // �γ��ڲ���Ϣ
        net_hdr_t net_hdr;
        net_hdr.m_packet_len_ = sizeof(net_hdr_t);
        net_hdr.m_message_id_ = S_NOTIFY_EACH_OTHER;
        net_hdr.m_message_type_ = message_common;
        net_hdr.m_request_sequence_ = 0;
        net_hdr.m_control_type_ = message_disconnect_notify;
        net_hdr.m_client_net_id_ = m_net_id_;
        net_hdr.m_client_uin_ = m_remote_uin_;
        net_hdr.m_from_uin_ = keep_alive::m_local_uin_; // condxml.m_net_xml_[condxml.m_use_index_].m_common_.m_entity_id_;
        net_hdr.m_to_uin_ = condxml.get_server_entity_id(m_remote_uin_, pf_entity_logic);  // �˴�Ҫ����Ŀ�����ͻ���Ψһ��ʶ

        uint32_t entity_id = condxml.get_server_entity_id(m_remote_uin_, pf_entity_router); 
        connect_session* conn = (connect_session*)session_list::get_net_node_by_uin(entity_id, is_trans_connect_out); 
        if (NULL == conn)
        {
            LOG(ERROR)("client_session::clear_current_net() get_net_node_by_uin error, m_signature_.m_client_uin_=%d,client_uin=%d,", m_signature_.m_client_uin_, m_remote_uin_);
            release_tunnel(m_net_id_);
            return;
        }

        // �����µ� �����
        net_package* np = event_handler::m_net_pkg_pool_->Create();
        if ( NULL == np )
        {
            LOG(ERROR)("assert: client_session::on_ne_data() error. new np is NULL");
            release_tunnel(m_net_id_);
            return;
        }

        np->allocator_data_block(new_allocator::Instance(), net_hdr.m_packet_len_);

        // ��� �µ������
        binary_output_packet<true> outpkg(np->get_data(), np->capacity());
        outpkg.offset_head(sizeof(net_hdr_t));
        outpkg.set_head(net_hdr);

        LOG(INFO)("client_session::clear_current_net() send_package, m_signature_.m_client_uin_=%d,client_uin=%d,", m_signature_.m_client_uin_, m_remote_uin_);
        int rc = net_manager::Instance()->send_package(conn->m_net_id_, np);
        if (0 != rc)
        {
            np->Destroy();
            release_tunnel(m_net_id_);
            return;
        }
    }

    // close and reinit 
    release_tunnel(m_net_id_);

    return;
}

