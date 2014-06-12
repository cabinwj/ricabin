#ifndef _JSON_CONFIGURE_H_
#define _JSON_CONFIGURE_H_

#include "hc_singleton.h"
#include "hc_types.h"

#if defined(_MSC_VER)
#include <hash_map>
#include <hash_set>
using stdext::hash_map;
using stdext::hash_set;
#elif defined(__GNUC__)
#include <ext/hash_map>
#include <ext/hash_set>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;
#endif

#include "protocol.h"


enum
{
    max_module_label_length = 64,
    max_module_type_count   = 16,       //服务类型的个数，仅能为16种不同类型的服务进程作数据交互。
    max_module_id_count     = 8,        //每类服务最多只配8个服务进程（对应不同的ip）。
    max_module_count        = 16 * 8 * 10,
};

struct common_t
{
    uint16_t m_zone_id_;
    uint32_t m_module_id_; // uin
    uint16_t m_socket_send_buffer_size_;
    uint16_t m_socket_recv_buffer_size_;
};

struct log_t
{
    char m_path_[max_file_path_length];
    uint32_t m_level_;
    uint32_t m_file_size_;
};

struct accept_t
{
    uint32_t m_module_id_; // uin
    uint16_t m_keepalive_timeout_;
    char m_ip_[max_ip_address_length];
    uint16_t m_port_;
};

struct accept_set_t
{
    uint16_t m_count_;
    accept_t m_array_[max_module_type_count];
};

struct connect_t
{
    uint32_t m_module_id_; // uin
    uint16_t m_keepalive_interval_;
    uint16_t m_connect_out_timeout_;
    char m_ip_[max_ip_address_length];
    uint16_t m_port_;
};

struct connect_set_t
{
    uint16_t m_count_;
    connect_t m_array_[max_module_type_count];
};

struct configure_t
{
    common_t m_common_;
    accept_set_t m_accept_set_;
    connect_set_t m_connect_set_;
};

struct module_t
{
    uint32_t m_module_id_;
    char m_ip_[max_ip_address_length];
    uint16_t m_port_;
};

struct module_set_t
{
    uint16_t m_count_;
    module_t m_array_[max_module_count];
};


class json_configure
{
public:
    enum
    {
        master            = (int16_t)0,
        slave             = (int16_t)1,

        max_configure_count,
    };

public:
    log_t m_log_;
    module_set_t m_module_set_;
    int16_t m_use_index_;
    configure_t m_configure_[max_configure_count];

public:
    json_configure();
    ~json_configure() {}

public:
    //加载配置文件
    void load_mylog(const char* json_file_name);
    //加载配置文件
    void load_myconf(const char* json_file_name);
    //加载配置文件
    void load_module(const char* json_file_name);
};

#define CREATE_JSON_CONFIG_INSTANCE  object_singleton<json_configure>::Instance
#define GET_JSON_CONFIG_INSTANCE     object_singleton<json_configure>::Instance
#define DESTROY_JSON_CONFIG_INSTANCE object_singleton<json_configure>::Instance

#endif
