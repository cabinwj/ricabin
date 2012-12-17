#include "accept_session.h"

#include "common_allocator.h"
#include "common_binpacket.h"

#include "net_manager.h"
#include "net_package.h"

#include "frame_base.h"
#include "entity_types.h"
#include "session_list.h"
#include "xml_configure.h"

#include "process_data.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/time.h>
#include <unistd.h>
#endif

accept_session::accept_session()
{
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_net_hash_item_);
    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_net_id_ = 0;
    m_listen_net_id_ = 0;

    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);
}

accept_session::~accept_session()
{
}


void accept_session::on_ne_data(net_event& ne)
{
    LOG(INFO)("accept_session::on_ne_data() recv data from net<%u:%u:%u>, remote_addr<0x%08X:%d>", ne.m_listen_net_id_, ne.m_net_id_, ne.m_net_ev_t_, ne.m_remote_addr_.get_net_ip(), ne.m_remote_addr_.get_net_port());

    struct timeval process_begin;
    gettimeofday(&process_begin, NULL);

    binary_input_packet<true> inpkg(ne.m_net_package_->get_data(), ne.m_net_package_->size());

    net_hdr_t net_hdr;
    inpkg.offset_head(sizeof(net_hdr_t));
    inpkg.get_head(net_hdr);

    // 验证数据包长
    if (inpkg.size() != net_hdr.m_packet_len_)
    {
        LOG(ERROR)("accept_session::on_net_data() packet_len is invalue.");
        return;
    }

    int rc = 0;
    // 判断协议，处理逻辑，此处目前只有获得用户状态和用户提交状态。
    // 判断message_id的消息范围合法性？
    switch (net_hdr.m_message_id_)
    {
    case IMD_STATUSD_REPORT_USER_STATUS: {
        report_user_status obj(m_net_id_, ne.m_net_package_, net_hdr);
        rc = obj.execute();
    } break;
    case IMD_STATUSD_GET_USER_STATUS: {
        get_user_status obj(m_net_id_, ne.m_net_package_, net_hdr);
        rc = obj.execute();
    } break;
    default:
        LOG(WARN)("accept_session::on_net_data() Unknown message id. client_uin=%d,from_uin=%d,to_uin=%d", net_hdr.m_client_uin_, net_hdr.m_from_uin_, net_hdr.m_to_uin_);
        break;
    }

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("accept_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}


