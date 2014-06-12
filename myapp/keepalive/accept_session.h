#ifndef _ACCEPT_SESSION_H_
#define _ACCEPT_SESSION_H_

#include "net_event.h"
#include "net_node.h"


class accept_session : public net_node
{
public:
    accept_session();
    virtual ~accept_session();

public:
    virtual void on_ne_data(net_event& ne);
};


#endif

