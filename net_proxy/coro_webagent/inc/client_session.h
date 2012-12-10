#ifndef _CLIENT_SESSION_H_
#define _CLIENT_SESSION_H_

#include "net_event.h"
#include "net_package.h"

#include "net_node.h"

class client_session : public net_node
{
public:
    client_session();
    virtual ~client_session();

//public:
//    signature_t m_signature_;   // ����ǩ��
//    conn_hdr_t m_conn_hdr_;     // cs��ͷ

public:
    virtual void on_ne_data(net_event& ne);

public:
    void process_message(net_package*& np);
};

#endif

