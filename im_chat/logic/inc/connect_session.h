#ifndef _CONNECT_SESSION_H_
#define _CONNECT_SESSION_H_

#include "net_event.h"
#include "net_node.h"


class connect_session : public net_node
{
public:
    connect_session();
    virtual ~connect_session();

public:
    virtual void on_ne_data(net_event& ne);

private:
    bool process_connector_message(net_event& ne);
    bool process_logic_message(net_event& ne);
    bool process_dbagent_message(net_event& ne);
};

#endif
