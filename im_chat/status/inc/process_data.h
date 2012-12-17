#ifndef _STATUSD_PROCESS_DATA_H_
#define _STATUSD_PROCESS_DATA_H_

#include "net_package.h"

#include "entity_types.h"


class process_data
{
public:
    process_data(uint32_t net_id, net_package*& packet, net_hdr_t& net_hdr)
        : m_current_net_id_(net_id), m_package_(packet), m_net_hdr_(net_hdr) {}
    virtual ~process_data(){}

public:
    bool process() { return execute(); }

public:
    virtual bool execute() = 0;

protected:
    uint32_t m_current_net_id_;
    net_package* m_package_;
    net_hdr_t& m_net_hdr_;
};


class report_user_status : public process_data
{
public:
    report_user_status(uint32_t net_id, net_package*& packet, net_hdr_t& net_hdr)
        : process_data(net_id, packet, net_hdr){}
    ~report_user_status(){}

public:
    bool execute();
};

class get_user_status : public process_data
{
public:
    get_user_status(uint32_t net_id, net_package*& packet, net_hdr_t& net_hdr)
        : process_data(net_id, packet, net_hdr){}
    ~get_user_status(){}

public:
    bool execute();
};

#endif
