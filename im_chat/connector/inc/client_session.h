#ifndef _CLIENT_SESSION_H_
#define _CLIENT_SESSION_H_

#include "net_event.h"

#include "entity_types.h"
#include "net_node.h"


class client_session : public net_node
{
public:
    client_session();
    virtual ~client_session();

public:                                                
    signature_t m_signature_;   // 数字签名
    conn_hdr_t m_conn_hdr_;     // cs包头

public:
    virtual void on_ne_data(net_event& ne);

public:
    void clear_current_net(uint16_t close_reason);

public:
    static void inner_message_notify(uint32_t client_net_id, uint32_t client_uin,
                                     uint32_t from_uin, uint32_t to_uin, uint32_t net_id);
};

#endif

