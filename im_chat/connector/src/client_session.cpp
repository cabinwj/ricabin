#include "client_session.h"

#include "hc_allocator.h"
#include "hc_binpacket.h"

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

    bzero(&m_signature_, sizeof(signature_t));
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

    // 第一个字段(packet_len字段) 第二个字段(client_uin字段) 第三个字段(option_len: option长度的字段)
    inpkg >> m_conn_hdr_.m_packet_len_ >> m_conn_hdr_.m_client_uin_ >> m_conn_hdr_.m_option_len_;
    // 验证数据包长
    if (inpkg.size() != m_conn_hdr_.m_packet_len_)
    {
        LOG(ERROR)("client_session::on_net_data() error. packet_len is invalue.");
        return;
    }

    if ((m_conn_hdr_.m_option_len_ < 0) || (m_conn_hdr_.m_option_len_ > max_option_length))
    {
        LOG(ERROR)("client_session::on_ne_data() error. option_len is invalue.");
        if (is_recved_data) { clear_current_net(close_reason_data_error); } //需要上报Service
        else { clear_current_net(close_reason_invalid_signature); }            
        return;
    }

    // 第四个字段(m_option_字段) 读出第四个字段（可选）option 现为玩家的 signature
    bzero(m_conn_hdr_.m_option_, sizeof(m_conn_hdr_.m_option_));
    inpkg.read(m_conn_hdr_.m_option_, m_conn_hdr_.m_option_len_);

    // 第五个字段(message_id字段) 第六个字段(message_type字段) 第七个字段(player_id字段)
    inpkg >> m_conn_hdr_.m_message_id_ >> m_conn_hdr_.m_message_type_ >> m_conn_hdr_.m_player_id_;

    // get crypt code offset from code
    // 从sequence开始，属加密部分。       
    uint32_t crypt_code_offset = min_cs_header_length + m_conn_hdr_.m_option_len_; 
    // 注：须对非加密部分的偏移进行检查
    if (crypt_code_offset <= 0 || crypt_code_offset >= inpkg.size()) //疑似攻击 断线？
    {
        LOG(ERROR)("client_session::on_ne_data() error. crypt_code_offset is invalue.");
        if (is_recved_data) { clear_current_net(close_reason_data_error); } //需要上报Service
        else { clear_current_net(close_reason_invalid_signature); } 
        return;
    }

    // 第一次收到对方过来的数据(first packet) 解包，验证签名
    if (!is_recved_data)
    {
        // 设置已经从peer收到数据了
        m_is_trans_.set(is_trans_recved_data);

        if (0 == m_conn_hdr_.m_option_len_)
        {
            LOG(ERROR)("client_session::on_ne_data() error. the first package option_len is 0.");
            clear_current_net(close_reason_invalid_signature);                        
            return;
        }

        // 解密出玩家的signature
        if (0 != decrypt_signature(m_conn_hdr_.m_option_, m_conn_hdr_.m_option_len_, m_signature_))
        {
            LOG(ERROR)("client_session::on_ne_data() error. can not decrypt signature.");
            clear_current_net(close_reason_invalid_signature); 
            return;
        }

        // 验证玩家的UIN是否与签名中的UIN一致性
        if (m_signature_.m_client_uin_ != m_conn_hdr_.m_client_uin_)
        {
            LOG(ERROR)("client_session::on_ne_data() error. m_signature_.m_client_uin_(%d) != client_uin(%d)", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
            clear_current_net(close_reason_invalid_signature); 
            return;
        }
            
        // 保存UIN到连接上
        m_remote_uin_ = m_conn_hdr_.m_client_uin_;

        // 检查签名的有效期限 回应断线？
        time_t now = time(NULL);
        if (now - (time_t)m_signature_.m_timestamp_ >= max_signature_valid_period)
        {
            LOG(ERROR)("client_session::on_ne_data() error. the signature of client_uin(%d) was expired.", m_conn_hdr_.m_client_uin_);
            clear_current_net(close_reason_invalid_signature);
            return;
        }

        // 加入到 uin hash list
        session_list::append_uin_to_list(this);
    }
    else
    {
        // 验证玩家的UIN是否与签名中的UIN一致性
        if (m_signature_.m_client_uin_ != m_conn_hdr_.m_client_uin_)
        {
            LOG(ERROR)("client_session::on_ne_data() error. m_signature_.m_client_uin_(%d) != client_uin(%d)", m_signature_.m_client_uin_, m_conn_hdr_.m_client_uin_);
            return;
        }
    }

    // 设置已经从peer收到数据的时间了
    m_last_recv_timestamp_ = (uint32_t)time(NULL);

    LOG(INFO)("client_session::on_ne_data() m_signature_.m_client_uin_=%d", m_signature_.m_client_uin_);

    // 接下来处理CS协议部分。
    // 用session key来解密
    int32_t out_length = (max_client_pkg_size + 100 - crypt_code_offset); 
    int rc = decrypt_buffer(m_signature_.m_session_key_,
                            (inpkg.get_data() + crypt_code_offset),
                            (m_conn_hdr_.m_packet_len_ - crypt_code_offset),
                            (inpkg.get_data() + crypt_code_offset),
                            out_length);
    if (0 == rc)
    {
        LOG(ERROR)("client_session::on_ne_data() error. can not decrypt buffer.");
        if (is_recved_data) { clear_current_net(close_reason_data_error); } //需要上报Service
        else { clear_current_net(close_reason_invalid_signature); } 
        return;
    }

    // get sequence inpkg current 不变       
    inpkg >> m_conn_hdr_.m_request_sequence_;

    xml_configure& condxml = GET_XML_CONFIG_INSTANCE();

    uint32_t ent_id = condxml.get_server_entity_id(m_conn_hdr_.m_client_uin_, pf_entity_router); 
    connect_session* conn = (connect_session*)session_list::get_net_node_by_uin(ent_id, is_trans_connect_out); 
    if (NULL == conn)
    {
        LOG(ERROR)("assert: client_session::on_ne_data() error. get conn by uin is NULL.");
        return;
    }

    // 修正CS协议的数据包长
    uint32_t decrypt_packet_len = out_length + sizeof(net_hdr_t) - 4; // sizeof(m_conn_hdr_.m_request_sequence_);

    // 在net_header 将玩家签名(使用网络字节序编码)传递给后端的service?
    net_hdr_t net_hdr;
    net_hdr.m_packet_len_ = decrypt_packet_len;
    net_hdr.m_message_id_ = m_conn_hdr_.m_message_id_;
    net_hdr.m_message_type_ = m_conn_hdr_.m_message_type_;
    net_hdr.m_request_sequence_ = m_conn_hdr_.m_request_sequence_;
    net_hdr.m_control_type_ = is_recved_data ? message_common : message_start_request;
    net_hdr.m_client_net_id_ = m_net_id_;
    net_hdr.m_client_uin_ = m_conn_hdr_.m_client_uin_;
    net_hdr.m_from_uin_ = condxml.m_net_xml_[condxml.m_use_index_].m_common_.m_entity_id_;
    net_hdr.m_to_uin_ = condxml.get_server_entity_id(m_conn_hdr_.m_client_uin_, pf_entity_logic);  // 此处要给定目的类型或是唯一标识

    // 分配新的 请求包
    net_package* np = event_handler::m_net_pkg_pool_->Create();
    if ( NULL == np )
    {
        LOG(ERROR)("assert: client_session::on_ne_data() error. new np is NULL");
        return;
    }

    np->allocator_data_block(new_allocator::Instance(), decrypt_packet_len);

    // 组包 新的请求包
    binary_output_packet<true> outpkg(np->get_data(), np->capacity());
    outpkg.offset_head(sizeof(net_hdr_t));
    outpkg.set_head(net_hdr);
    outpkg.write(inpkg.get_data() + crypt_code_offset + 4, out_length - 4);

    np->offset_cursor(decrypt_packet_len);

    LOG(INFO)("client_session::on_ne_data() client_uin=%d, message_id=%d, request_sequence=%d", m_conn_hdr_.m_client_uin_, m_conn_hdr_.m_message_id_, m_conn_hdr_.m_request_sequence_);

    // 判断message_id的消息范围合法性？
    switch (net_hdr.m_message_id_)
    {
    case C_LOGOUT: {
        rc = net_manager::Instance()->send_package(conn->m_net_id_, np);
        if (0 != rc)
        {
            np->Destroy();
        }

        // 标识在conn当前连接上，服务进程已经发送了数据
        conn->m_is_trans_.set(is_trans_sended_data);

        // 整理当前通道上未经client确认的数据。

        // 释放当前通道
        release_tunnel(m_net_id_);
    } break;
    case C_CHAT_MSG_ACK:
    case C_RECV_ADD_FRIEND_INVITE_ACK: {
        // 整理当前通道上未经client确认的数据。
    } break;
    case C_LOGIN:
    case C_KEEPALIVE: {

    } break;

        // just head
    case C_LOGIN_OVER:
    case C_GET_FRIEND_GROUP:
    case C_GET_FRIEND_LIST:
        // head + body
    case C_MODIFY_USER_INFO:
    case C_GET_FRIEND_STATUS:
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
    case C_P2P_TRANSPARENT:
    case C_CHANGE_STATUS:
    case C_CHANGE_PICTURE:
    case C_CHANGE_PROFILE: {
        rc = net_manager::Instance()->send_package(conn->m_net_id_, np);
        if (0 != rc)
        {
            np->Destroy();
        }

        // 标识在conn当前连接上，服务进程已经发送了数据
        conn->m_is_trans_.set(is_trans_sended_data);
        
    } break;
    default:
        break;
    }

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("client_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}

void client_session::inner_message_notify(uint32_t client_net_id, uint32_t client_uin,
                                          uint32_t from_uin, uint32_t to_uin, uint32_t net_id)
{
    // 形成内部消息
    net_hdr_t net_hdr;
    net_hdr.m_packet_len_ = sizeof(net_hdr_t);
    net_hdr.m_message_id_ = 0;
    net_hdr.m_message_type_ = 0;
    net_hdr.m_request_sequence_ = 0;
    net_hdr.m_control_type_ = message_disconnect_notify;
    net_hdr.m_client_net_id_ = client_net_id;
    net_hdr.m_client_uin_ = client_uin;
    net_hdr.m_from_uin_ = from_uin;
    net_hdr.m_to_uin_ = to_uin;  // 此处要给定目的类型或是唯一标识

    // 分配新的 请求包
    net_package* np = event_handler::m_net_pkg_pool_->Create();
    if ( NULL == np )
    {
        LOG(ERROR)("assert: client_session::inner_message_notify() error. new np is NULL");
        return;
    }

    np->allocator_data_block(new_allocator::Instance(), net_hdr.m_packet_len_);

    // 组包 新的请求包
    binary_output_packet<true> outpkg(np->get_data(), np->capacity());
    outpkg.offset_head(sizeof(net_hdr_t));
    outpkg.set_head(net_hdr);

    LOG(INFO)("client_session::inner_message_notify() send_package, client_uin=%d, from_uin=%d, to_uin=%d", client_uin, from_uin, to_uin);
    int rc = net_manager::Instance()->send_package(net_id, np);
    if (0 != rc)
    {
        np->Destroy();
    }
}

void client_session::clear_current_net(uint16_t close_reason)
{
    LOG(INFO)("client_session::clear_current_net() client uin:%u net<%u:%u>, remote_addr<0x%08X:%d>", m_remote_uin_, m_listen_net_id_, m_net_id_, m_remote_addr_.get_net_ip(), m_remote_addr_.get_net_port());

    // 非正常的断开连接，则通知业务进程连接中断
    if ( ( close_reason_service != close_reason) 
        && (close_reason_invalid_connect != close_reason)
        && (close_reason_invalid_signature != close_reason) )
    {
        xml_configure& condxml = GET_XML_CONFIG_INSTANCE();
        uint32_t entity_id = condxml.get_server_entity_id(m_remote_uin_, pf_entity_router); 
        connect_session* conn = (connect_session*)session_list::get_net_node_by_uin(entity_id, is_trans_connect_out); 
        if (NULL != conn)
        {
            LOG(INFO)("client_session::clear_current_net() inner_message_notify.");
            client_session::inner_message_notify(m_net_id_, m_remote_uin_, keep_alive::m_local_uin_,
                                                 condxml.get_server_entity_id(m_remote_uin_, pf_entity_logic),
                                                 conn->m_net_id_);
        }
    }

    // close and reinit 
    release_tunnel(m_net_id_);

    return;
}

