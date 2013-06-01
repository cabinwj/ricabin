#include "connect_session.h"

#include "net_manager.h"

#include "protocol.h"
#include "frame_base.h"
#include "entity_types.h"
#include "session_list.h"
#include "xml_configure.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/time.h>
#include <unistd.h>
#endif


connect_session::connect_session()
{
    INIT_LIST_HEAD(&m_net_hash_item_);
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_listen_net_id_ = 0;
    m_net_id_ = 0;

    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);
}

connect_session::~connect_session()
{
}


void connect_session::on_ne_data(net_event& ne)
{
    LOG(INFO)("connect_session::on_net_data() recv data from net<%u:%u:%u>, remote_addr<0x%08X:%d>", ne.m_listen_net_id_, ne.m_net_id_, ne.m_net_ev_t_, ne.m_remote_addr_.net_ip(), ne.m_remote_addr_.net_port());

    struct timeval process_begin;
    gettimeofday(&process_begin, NULL);

    //m_is_trans_.is_setted(is_trans_recved_data);
    //m_last_recv_timestamp_ = (uint32_t)time(NULL);

    ////对于每个到来的数据包，创建一个协程处理用户请求
    //SPAWN(&client_session::process_message, *this, ne.m_net_package_);

    struct timeval process_end;
    gettimeofday(&process_end, NULL);
    int32_t interval = (process_end.tv_sec - process_begin.tv_sec)*1000 + (process_end.tv_usec - process_begin.tv_usec)/1000;
    LOG(INFO)("connect_session::on_ne_data() dispatch message cost time %06d (ms).", interval);

    return;
}
