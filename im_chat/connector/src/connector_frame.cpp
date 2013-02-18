#include "connector_frame.h"

#include "hc_log.h"

#include "protocol.h"
#include "frame_base.h"
#include "session_list.h"
#include "xml_configure.h"

#include "net_event_handler.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <signal.h>
#include <sys/file.h>
#endif

#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

class connector_frame;

void sigusr1_handle(int signo)
{
    connector_frame::m_run_status_.set(run_status_reload);
#ifndef WIN32
    signal(SIGUSR1, sigusr1_handle);
#endif
}

void sigusr2_handle(int signo)
{
    connector_frame::m_run_status_.set(run_status_exit);
#ifndef WIN32
    signal(SIGUSR2, sigusr2_handle);
#endif
}

#ifdef WIN32
static const char* xml_relative_path[] = { "cond.xml", "online_data", };
#else
static const char* xml_relative_path[] = { "/cond.xml", "/online_data", };
#endif

static char work_path[max_file_path_length];
static char xml_full_path[max_file_path_length];

bitmap32 connector_frame::m_run_status_;

connector_frame::connector_frame()
{
}

connector_frame::~connector_frame()
{
}

int connector_frame::start(bool is_daemon)
{
    if (is_daemon)
    {
        init_daemon(1, 0);
        cout << "connector_frame::start() daemon start!" << endl;
    }

    bzero(work_path, sizeof(work_path));
    bzero(xml_full_path, sizeof(xml_full_path));

    // 安装信号处理函数
#ifndef WIN32
    signal(SIGUSR1, sigusr1_handle);
    signal(SIGUSR2, sigusr2_handle);

    if (!getcwd(work_path, max_file_path_length - strlen(xml_relative_path[0]) - 1))
    {
        cout << "connector_frame::start() get current work path (getcwd) error!" << endl;
        exit(0);
    }
    cout << "connector_frame::start() current work path: " << work_path << endl;
#endif

    strncpy(xml_full_path, work_path, strlen(work_path));
    strncat(xml_full_path, xml_relative_path[0], strlen(xml_relative_path[0]));

    xml_configure& condxml = CREATE_XML_CONFIG_INSTANCE();
    condxml.load_all(xml_full_path);

    LOG_INIT(condxml.m_net_xml_[condxml.m_use_index_].m_log_.m_log_path_,
             condxml.m_net_xml_[condxml.m_use_index_].m_log_.m_file_size_,
             condxml.m_net_xml_[condxml.m_use_index_].m_log_.m_log_level_);

    LOG_OPEN();

    // 初始化底层框架
    int rc = init_environment(condxml.m_net_xml_[condxml.m_use_index_].m_common_.m_entity_id_);
    if (0 != rc)
    {
        LOG(ERROR)("connector_frame::start() init environment fail.");
        return -1;
    }
    LOG(INFO)("connector_frame::start() coroutine frame actor is running.");

    session_list::init();

    // accept tcp server
    net_conf_t nconf;
    nconf.m_proto_type_ = SOCKET_PROTO_TCP;
    nconf.m_is_listen_ = 1;
    nconf.m_app_proto_ = PROTOCOL_ASYNC;
    
    memcpy(nconf.m_ipport_.m_ip_,
           condxml.m_net_xml_[condxml.m_use_index_].m_listen_.m_ip_port_.m_ip_,
           sizeof(nconf.m_ipport_.m_ip_));
    nconf.m_ipport_.m_port_ = condxml.m_net_xml_[condxml.m_use_index_].m_listen_.m_ip_port_.m_port_;
    
    nconf.m_connect_timeout_ = 0;
    nconf.m_keep_alive_interval_ = 0;
    
    nconf.m_keep_alive_timeout_ = 600;
    nconf.m_socket_buffer_size_ = 8192;
    
    nconf.m_reconnect_interval_ = 0;
    
    uint32_t listen_net_id = create_network(nconf, on_net_event_handler);
    if (0 == listen_net_id)
    {
        LOG(ERROR)("create_network() create network error. listen_net_id:%u", listen_net_id);
        return 0;
    }
    LOG(INFO)("create_network() create network success. listen_net_id:%u", listen_net_id);


    for (int16_t x = 0; x < condxml.m_net_xml_[condxml.m_use_index_].m_ent_set_.m_ent_type_count_; ++x)
    {
        int16_t entity_id_count = condxml.m_net_xml_[condxml.m_use_index_].m_ent_set_.m_ent_array_[x].m_ent_id_count_;
        for (int16_t y = 0; y < entity_id_count; ++y)
        {
            net_conf_t nconf;
            nconf.m_proto_type_ = SOCKET_PROTO_TCP;
            nconf.m_is_listen_ = 0;
            nconf.m_app_proto_ = PROTOCOL_ASYNC;

            memcpy(nconf.m_ipport_.m_ip_,
                   condxml.m_net_xml_[condxml.m_use_index_].m_ent_set_.m_ent_array_[x].m_array_[y].m_ip_,
                   sizeof(nconf.m_ipport_.m_ip_));
            nconf.m_ipport_.m_port_ = condxml.m_net_xml_[condxml.m_use_index_].m_ent_set_.m_ent_array_[x].m_array_[y].m_port_;

            nconf.m_socket_buffer_size_ = 8192;
            nconf.m_keep_alive_timeout_ = 600;

            nconf.m_connect_out_uin_ = condxml.m_net_xml_[condxml.m_use_index_].m_ent_set_.m_ent_array_[x].m_ent_type_ + ((y+1) & pf_entity_id_mark);

            nconf.m_reconnect_interval_ = 5;
            nconf.m_keep_alive_interval_ = 20;
            nconf.m_connect_timeout_ = 2;

            uint32_t net_id = create_network(nconf, on_net_event_handler);
            if(0 == net_id)
            {
                LOG(TRACE)("create_network() create network error. net_id:%u", net_id);
                return 0;
            }
            LOG(INFO)("create_network() create network success. net_id :%u", net_id);
        }
    }

    return 0;
}

void connector_frame::run()
{
    while (true)
    {
        run_environment_once();

        if (m_run_status_.is_setted(run_status_reload))
        {
            m_run_status_.unset(run_status_reload);
            LOG(INFO)("reload config...");
            xml_configure& condxml = CREATE_XML_CONFIG_INSTANCE();
            condxml.load_all(xml_full_path);
        }
        else if (m_run_status_.is_setted(run_status_exit))
        {
            LOG(INFO)("run_status_exit.");
            //if ( 1 == active_coro_count() )
            //{
            // 这里将各用户下线，存储用户信息? 保存资源信息? 卸载模块?

            stop_environment();
            break;
            //}
        }

        //coro_sleep(1000);

        on_timer();
        on_tick();
    }
}

void connector_frame::on_timer()
{

}

void connector_frame::on_tick()
{
    //检查dbagent 断线重连

    //定时更新状态

    //定时保存
}
