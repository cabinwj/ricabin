#include "connect_session.h"

#include "common_allocator.h"
#include "common_binpacket.h"

#include "net_manager.h"
#include "net_package.h"

#include "frame_base.h"
#include "entity_types.h"
#include "session_list.h"
#include "xml_configure.h"

#include "client_session.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/time.h>
#include <unistd.h>
#endif

class client_session;

connect_session::connect_session()
{
    INIT_LIST_HEAD(&m_net_hash_item_);
    INIT_LIST_HEAD(&m_list_item_);

    m_listen_net_id_ = 0;
    m_net_id_ = 0;

    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);
}

connect_session::~connect_session()
{
}


void connect_session::on_ne_data(net_event& ne)
{
    LOG(INFO)("connect_session::on_ne_data() recv data from net<%u:%u:%u>, remote_addr<0x%08X:%d>", ne.m_listen_net_id_, ne.m_net_id_, ne.m_net_ev_t_, ne.m_remote_addr_.get_net_ip(), ne.m_remote_addr_.get_net_port());

    struct timeval process_begin;
    gettimeofday(&process_begin, NULL);

    binary_input_packet<true> inpkg(ne.m_net_package_->get_data(), ne.m_net_package_->size());

    net_hdr_t net_hdr;
    inpkg.offset_head(sizeof(net_hdr_t));
    inpkg.get_head(net_hdr);

    // 要判断前端是否断了连接。
    client_session* client = (client_session*)session_list::get_net_node_by_net_id(net_hdr.m_client_net_id_);
    if (NULL == client)
    {
        LOG(WARN)("connect_session::on_net_data() get_net_node_by_net_id() error. client_uin=%u, net=%u", net_hdr.m_client_uin_, net_hdr.m_client_net_id_);

        //若不需要通知后台关闭client的连接，则直接返回
        if (net_hdr.m_control_type_ == message_force_close_connect)
        {
            LOG(WARN)("connect_session::on_net_data() froce close connect. client_uin=%u, net=%u", net_hdr.m_client_uin_, net_hdr.m_client_net_id_);
            return;
        }

        client_session::inner_message_notify(net_hdr.m_client_net_id_, net_hdr.m_client_uin_,
                                             net_hdr.m_to_uin_, net_hdr.m_from_uin_, m_net_id_);
        return;
    }

    // 普通的中转包或者关闭当前连接的包
    switch (net_hdr.m_control_type_)
    {
        // 普通回应包
    case message_common: {

        // 防止net_id失效或已被复用
        if (client->m_remote_uin_ != net_hdr.m_client_uin_)
        {
            LOG(WARN)("connect_session::on_net_data() net_hdr.m_client_uin_(%u) has be reuse. now clients uin(%u)", net_hdr.m_client_uin_, client->m_remote_uin_);
            xml_configure& condxml = GET_XML_CONFIG_INSTANCE();
            client_session::inner_message_notify(net_hdr.m_client_net_id_, net_hdr.m_client_uin_,
                                                 condxml.m_net_xml_[condxml.m_use_index_].m_common_.m_entity_id_,
                                                 condxml.get_server_entity_id(net_hdr.m_client_uin_, pf_entity_logic),
                                                 m_net_id_);
            return;
        }

        // 这里多了一层copy
        char code_buffer[max_client_pkg_size + 100];
        // 先将包加密的部分直接拷贝出来(去掉net_header)，留下一个sequence长度，从sequence开始，属加密部分。
        inpkg.read(code_buffer + 4, (net_hdr.m_packet_len_ - sizeof(net_hdr_t)));
        // 回填sequence
        *(uint32_t*)(code_buffer) = htonl(net_hdr.m_request_sequence_);
   
        // 用session key来加密
        int32_t out_length = (max_client_pkg_size + 100); 
        int rc = encrypt_buffer(client->m_signature_.m_session_key_, (code_buffer),
                                (net_hdr.m_packet_len_ - sizeof(net_hdr_t) + 4), (code_buffer), out_length);
        if (0 == rc)
        {
            LOG(ERROR)("connect_session::on_ne_data() encrypt_buffer error, client_uin=%d,from_uin=%d,to_uin=%d", net_hdr.m_client_uin_, net_hdr.m_from_uin_, net_hdr.m_to_uin_);
            return;
        }

        // 修正CS协议的数据包长
        client->m_conn_hdr_.m_packet_len_ = out_length + min_cs_header_length + client->m_conn_hdr_.m_option_len_;

        // 分配新的 cs回应包
        net_package* np = event_handler::m_net_pkg_pool_->Create();
        if ( NULL == np )
        {
            LOG(ERROR)("assert: client_session::on_ne_data() error. new np is NULL");
            return;
        }

        np->allocator_data_block(new_allocator::Instance(), client->m_conn_hdr_.m_packet_len_);

        // 组包 新的请求包
        binary_output_packet<true> outpkg(np->get_data(), np->capacity());
        outpkg << client->m_conn_hdr_.m_packet_len_ << client->m_conn_hdr_.m_client_uin_
               << client->m_conn_hdr_.m_option_len_;

        outpkg.write(client->m_conn_hdr_.m_option_, client->m_conn_hdr_.m_option_len_);

        outpkg << client->m_conn_hdr_.m_message_id_ << client->m_conn_hdr_.m_message_type_
               << client->m_conn_hdr_.m_player_id_;

        outpkg.write(code_buffer, out_length);

        np->offset_cursor(client->m_conn_hdr_.m_packet_len_);

        LOG(INFO)("connect_session::on_ne_data() send_package, client_uin=%d,from_uin=%d,to_uin=%d", net_hdr.m_client_uin_, net_hdr.m_from_uin_, net_hdr.m_to_uin_);
        rc = net_manager::Instance()->send_package(client->m_net_id_, np);
        if (0 != rc)
        {
            np->Destroy();
        }

        //标识在client当前连接上，服务进程已经发送了数据
        client->m_is_trans_.set(is_trans_sended_data);

    } break;

    	// 若service进程主动要求connector server关闭连接，则不用通知service 进行
    case message_force_close_connect: {

        LOG(INFO)("connect_session::on_ne_data() service request 2 close client...");
        client->clear_current_net(close_reason_service);

    } break;

    default:
        LOG(WARN)("connect_session::on_ne_data() send invalid package 2.. so ignore(discard) it!");
        break;
    }

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("connect_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}

