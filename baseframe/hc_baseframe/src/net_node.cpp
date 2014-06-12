#include "net_node.h"

#include "protocol.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#endif


net_node::net_node()
{
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_net_hash_item_);
    INIT_LIST_HEAD(&m_uin_hash_item_);

    m_net_id_ = 0;

    m_remote_uin_ = 0;

    m_is_trans_.set(is_trans_null);
    m_last_recv_timestamp_ = (uint32_t)time(NULL);
}

net_node::~net_node()
{
}

