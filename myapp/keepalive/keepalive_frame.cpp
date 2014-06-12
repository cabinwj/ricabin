#include "keepalive_frame.h"

#include "hc_log.h"
#include "hc_bitmap.h"

#include "net_messenger.h"
#include "packet_splitter.h"

#include "protocol.h"
#include "base_frame.h"
#include "session_list.h"
#include "json_configure.h"
#include "keepalive_list.h"

#include "net_event_handler.h"

#include <string>
#include <iostream>
#include <stdlib.h>

#ifdef WIN32
static const char* json_relative_path[] = { "keepalive.json", "online_data", };
#else
static const char* json_relative_path[] = { "/keepalive.json", "/online_data", };
#endif

static char work_path[max_file_path_length];
static char json_full_path[max_file_path_length];

keepalive_frame::keepalive_frame()
{
}

keepalive_frame::~keepalive_frame()
{
}

int keepalive_frame::start(bool is_daemon)
{
    // 安装信号处理函数
    signal_handler();

    bzero(work_path, sizeof(work_path));
    bzero(json_full_path, sizeof(json_full_path));

#ifndef WIN32
    if (!getcwd(work_path, max_file_path_length - strlen(json_relative_path[0]) - 1))
    {
        std::cout << "keepalive_frame::start() get current work path (getcwd) error!" << std::endl;
        exit(-1);
    }
    std::cout << "keepalive_frame::start() current work path: " << work_path << std::endl;
#endif

    strncpy(json_full_path, work_path, strlen(work_path));
    strncat(json_full_path, json_relative_path[0], strlen(json_relative_path[0]));

    json_configure& json_conf = CREATE_JSON_CONFIG_INSTANCE();
    json_conf.load_mylog(json_full_path);

    LOG_INIT(json_conf.m_log_.m_path_, json_conf.m_log_.m_file_size_, json_conf.m_log_.m_level_);
    LOG_OPEN();

    if (is_daemon)
    {
        init_daemon(1, 0);
        LOG(INFO)("keepalive_frame::start() daemon start.");
    }

    json_conf.load_myconf(json_full_path);

    // 初始化底层框架
    int rc = init_environment();
    if (0 != rc)
    {
        LOG(ERROR)("keepalive_frame::start() init environment failed.");
        return -1;
    }
    LOG(INFO)("keepalive_frame::start() init environment success.");

    session_list::init();
    keepalive::init();

    // accept tcp server
    //memcpy(net_conf.m_ip_,
    //       json_conf.m_configure_[json_conf.m_use_index_].m_accept_set_.m_ip_,
    //       sizeof(net_conf.m_ip_));

    //net_conf.m_port_ = json_conf.m_configure_[json_conf.m_use_index_].m_accept_set_.m_port_;

    configure_t myconf = json_conf.m_configure_[json_conf.m_use_index_];
    net_messenger* mynetmgr = net_messenger::Instance();
    for (int idx_x = 0; idx_x < myconf.m_accept_set_.m_count_; idx_x++)
    {
        accept_t myaccept = myconf.m_accept_set_.m_array_[idx_x];
        conn_param_t* connprm = new conn_param_t;
        connprm->m_proto_type_ = SOCKET_PROTO_TCP;
        connprm->m_is_listen_ = 1;
        connprm->m_net_event_cb_ = net_event_handler;
        connprm->m_socket_send_buffer_size_ = myconf.m_common_.m_socket_send_buffer_size_;
        connprm->m_socket_recv_buffer_size_ = myconf.m_common_.m_socket_recv_buffer_size_;
        connprm->m_connect_out_uin_ = 0;     // connect
        connprm->m_connect_out_timeout_ = 0; // connect
        connprm->m_keepalive_interval_ = 0;  // connect
        connprm->m_keepalive_timeout_ = myaccept.m_keepalive_timeout_;  // accept

        ipacket_splitter* bin_split = new binary4_splitter();
        int32_t accept_net_id = mynetmgr->create_tcp_server(myaccept.m_ip_, myaccept.m_port_, bin_split, (void*)connprm,
                    (int)connprm->m_socket_recv_buffer_size_, (int)connprm->m_socket_send_buffer_size_);

        if (0 == accept_net_id)
        {
            delete connprm;
            connprm = NULL;
            delete bin_split;
            bin_split = NULL;
            LOG(ERROR)("create network error. accept_net_id:%u", accept_net_id);
            return 0;
        }

        LOG(INFO)("create network success. accept_net_id:%u", accept_net_id);
    }

    for (int idx_y = 0; idx_y < myconf.m_connect_set_.m_count_; idx_y++)
    {
        connect_t myconnect = myconf.m_connect_set_.m_array_[idx_y];
        conn_param_t* connprm = new conn_param_t;
        connprm->m_proto_type_ = SOCKET_PROTO_TCP;
        connprm->m_is_listen_ = 1;
        connprm->m_net_event_cb_ = net_event_handler;
        connprm->m_socket_send_buffer_size_ = myconf.m_common_.m_socket_send_buffer_size_;
        connprm->m_socket_recv_buffer_size_ = myconf.m_common_.m_socket_recv_buffer_size_;
        connprm->m_connect_out_uin_ = myconnect.m_module_id_;               // connect
        connprm->m_connect_out_timeout_ = myconnect.m_connect_out_timeout_; // connect
        connprm->m_keepalive_interval_ = myconnect.m_keepalive_interval_;   // connect
        connprm->m_keepalive_timeout_ = 0;   // accept

        ipacket_splitter* bin_split = new binary4_splitter();
        int32_t connect_net_id = mynetmgr->create_tcp_client(myconnect.m_ip_, myconnect.m_port_, bin_split, (void*)connprm,
                    (int)connprm->m_socket_recv_buffer_size_, (int)connprm->m_socket_send_buffer_size_);

        if (0 == connect_net_id)
        {
            delete connprm;
            connprm = NULL;
            delete bin_split;
            bin_split = NULL;
            LOG(ERROR)("create network error. connect_net_id:%u", connect_net_id);
            return -1;
        }

        LOG(INFO)("create network success. connect_net_id:%u", connect_net_id);
    }    

    return 0;
}

void keepalive_frame::run()
{
    while (true)
    {
        run_environment_once();

        if (g_run_status.is_setted(run_status_reload))
        {
            LOG(WARN)("run_status_reload.");
            g_run_status.unset(run_status_reload);
            json_configure& json_conf = CREATE_JSON_CONFIG_INSTANCE();
            json_conf.load_myconf(json_full_path);
        }
        else if (g_run_status.is_setted(run_status_exit))
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

        // 定时操作
        // 定时统计数据

        //检查dbagent 断线重连
        //定时更新状态
        //定时保存
    }
}

