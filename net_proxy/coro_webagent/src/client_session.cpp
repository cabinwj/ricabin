#include "client_session.h"

#include "net_manager.h"

#include "protocol.h"
#include "frame_base.h"
#include "keep_alive.h"
#include "session_list.h"
#include "xml_configure.h"

#include "connect_session.h"


#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/time.h>
#include <unistd.h>
#endif


client_session::client_session()
{
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_net_hash_item_);
    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_net_id_ = 0;
    m_listen_net_id_ = 0;

    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);

    //memset(&m_signature_, 0, sizeof(signature_t));
}

client_session::~client_session()
{
}


// net event handler
void client_session::on_ne_data(net_event& ne)
{
    LOG(INFO)("client_session::on_ne_data() recv data from net<%u:%u:%u>, remote_addr<0x%08X:%d>", ne.m_listen_net_id_, ne.m_net_id_, ne.m_net_ev_t_, ne.m_remote_addr_.net_ip(), ne.m_remote_addr_.net_port());

    struct timeval process_begin;
    gettimeofday(&process_begin, NULL);

    m_is_trans_.is_setted(is_trans_recved_data);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);

    //对于每个到来的数据包，创建一个协程处理用户请求
    SPAWN(&client_session::process_message, *this, ne.m_net_package_);

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("client_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}


void client_session::process_message(net_package*& np)
{
    // web端请求
    binary_input_packet<true> inpkg(np->get_data(), np->size());
    net_hdr_t phdr;
    inpkg.offset_head(sizeof(net_hdr_t));
    inpkg.get_head(phdr);

    if (!inpkg.good())
    {
        LOG(ERROR)("client_session::process_message() packet error.");
        return;
    }

    switch (phdr.m_message_id_)
    {
    case C_GET_FRIEND_INFO:
    case C_GET_FRIEND_LIST: {

        xml_configure& condxml = GET_XML_CONFIG_INSTANCE();

        uint32_t ent_id = condxml.get_server_entity_id(phdr.m_client_uin_, pf_entity_router);
        connect_session* conn = (connect_session*)session_list::get_net_node_by_uin(ent_id, is_trans_connect_out); 
        if (NULL == conn)
        {
            LOG(ERROR)("client_session::process_message, session_list::get_net_node_by_uin return NULL");
            return;
        }

        net_event* ne = NULL;
        int rc = send_async_package(conn->m_net_id_, np, 1000, ne);
        if (0 != rc)
        {
            LOG(ERROR)("client_session::process_message, send_async_package error, rc:%d", rc);
            return;
        }

        // 异步回应包为空 回应？
        if (NULL == ne->m_net_package_) 
        {
            LOG(ERROR)("client_session::process_message, send_async_package return netpkg is NULL");
            return;
        }

        rc = net_manager::Instance()->send_package(m_net_id_, ne->m_net_package_);
        if (0 == rc)
        {
            // 避免上层net_event析构时把package也析构（此时的包可能还有队列中，没有发送出去。）
            ne->m_net_package_ = NULL;
        }

    } break;
    default:
        break;
    }
}

