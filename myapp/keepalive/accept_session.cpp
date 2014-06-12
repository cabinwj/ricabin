#include "accept_session.h"

#include "hc_log.h"
#include "hc_allocator.h"
#include "hc_binpacket_tlv.h"

#include "net_messenger.h"
#include "net_package.h"

#include "protocol.h"
#include "base_frame.h"
#include "session_list.h"
#include "entity_types.h"


accept_session::accept_session()
{
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_net_hash_item_);
    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_net_id_ = 0;
    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);
}

accept_session::~accept_session()
{
}

void accept_session::on_ne_data(net_event& ne)
{
    LOG(INFO)("accept_session::on_ne_data() recv data from net<%u:%u>, remote_addr<0x%08X:%d>", ne.m_net_id_, ne.m_net_ev_t_, ne.m_remote_addr_.net_ip(), ne.m_remote_addr_.net_port());

    struct timeval process_begin;
    gettimeofday(&process_begin, NULL);

    input_binpacket inpkg(ne.m_net_package_->data_o(), ne.m_net_package_->size());

    net_hdr_t net_hdr;
    inpkg.read((char*)&net_hdr, sizeof(net_hdr_t));

    // 验证数据包长
    if (inpkg.size() != net_hdr.m_packet_len_)
    {
        LOG(ERROR)("accept_session::on_net_data() packet_len is invalue.");
        return;
    }

    accept_session* conn = (accept_session*)session_list::net_node_by_uin_o(net_hdr.m_to_uin_, is_trans_accept_server); 
    if (NULL == conn)
    {
        LOG(ERROR)("accept_session::on_ne_data() session_list::get_net_node_by_uin error, client_uin=%d,from_uin=%d,to_uin=%d", net_hdr.m_client_uin_, net_hdr.m_from_uin_, net_hdr.m_to_uin_);
        return;
    }

    LOG(INFO)("accept_session::on_ne_data() send_package, m_signature_.m_client_uin_=%d,client_uin=%d,", net_hdr.m_client_uin_, net_hdr.m_client_uin_);
    int rc = net_messenger::Instance()->send_package(conn->m_net_id_, ne.m_net_package_);
    if (0 == rc)
    {
        // 避免上层net_event析构时把package也析构（此时的包可能还有队列中，没有发送出去。）
        ne.m_net_package_ = NULL;        
    }

    // 标识在conn当前连接上，服务进程已经发送了数据
    conn->m_is_trans_.set(is_trans_sended_data);

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("accept_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}


