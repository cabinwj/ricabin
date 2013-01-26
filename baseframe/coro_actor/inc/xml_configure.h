#ifndef _XML_CONFIGURE_H_
#define _XML_CONFIGURE_H_

#include "hc_singleton.h"
#include "hc_types.h"
#include "hc_base.h"

#include "address.h"
#include "tinyxml.h"

#include "protocol.h"
#include "entity_types.h"


struct common_t
{
    //int16_t m_zone_id_;
    uint32_t m_entity_id_; // uin
};

struct log_t
{
    char m_log_path_[max_file_path_length];
    uint32_t m_log_level_;
    uint32_t m_file_size_;
};

struct listen_t
{
    ip_port_t m_ip_port_;
    int16_t m_alive_timestamp_;
    int16_t m_recv_first_packet_timestamp_;
};

struct entity_file_t
{
    int16_t m_ent_file_count_;
    char m_ent_file_[max_connect_entity_type_count][max_file_name_length];
};

struct entity_t
{
    int16_t m_ent_id_count_;
    uint32_t m_ent_type_;
    ip_port_t m_array_[max_connect_entity_id_count];
};

struct entity_set
{
    int16_t m_ent_type_count_;
    entity_t m_ent_array_[max_connect_entity_type_count];
};

struct net_xml_t
{
    common_t m_common_;
    log_t m_log_;
    listen_t m_listen_;
    entity_file_t m_ent_file_;
    entity_set m_ent_set_;
};


class xml_configure
{
public:
    enum
    {
        master            = (int16_t)0,
        slave             = (int16_t)1,

        max_net_conf_count,
    };

public:
    int16_t m_use_index_;
    net_xml_t m_net_xml_[max_net_conf_count];

public:
    xml_configure();
    explicit xml_configure(const char* xml_file_name);

    ~xml_configure() {}

public:
    void load_all(const char* xml_file_name);

    uint32_t get_entity_id_by_addr(const Address& laddr);
    int16_t get_entity_count_by_entity_type(const uint32_t pfent);

    uint32_t get_server_entity_id(const uint32_t uin, uint32_t pfet);

private:
    TiXmlElement* m_root_elem_;
};


#define CREATE_XML_CONFIG_INSTANCE  object_singleton<xml_configure>::Instance
#define GET_XML_CONFIG_INSTANCE     object_singleton<xml_configure>::Instance
#define DESTROY_XML_CONFIG_INSTANCE object_singleton<xml_configure>::Instance


#endif
