#include "process_data.h"
#include "user_list.h"
#include "xml_configure.h"
#include "keep_alive.h"
#include "event_handler.h"
#include "net_manager.h"

#include "common_log.h"


bool report_user_status::execute()
{
    LOG(INFO)("get_user_status::execute(). client_uin=%d,from_uin=%d,to_uin=%d", m_net_hdr_.m_client_uin_, m_net_hdr_.m_from_uin_, m_net_hdr_.m_to_uin_);
    binary_input_packet<true> inpkg(m_package_->get_data(), m_package_->size());

    net_hdr_t net_hdr;
    inpkg.offset_head(sizeof(net_hdr_t));
    inpkg.get_head(net_hdr);

    uint16_t ostatus = 0;
    inpkg >> ostatus;
    if (!inpkg.good() || !inpkg.eof())
    {
        LOG(ERROR)("report_user_status::execute() package invalue.");
        return false;
    }

    // 判断状态？ 
    user_node* user = user_list::get(m_net_hdr_.m_client_uin_);
    if ( NULL == user )
    {
        // new user, new position(0)
        user = new user_node(m_net_hdr_.m_client_uin_);
        user_list::put(user);
    }

    // login success init user data，把client设置为online，依据协议，设置为client 或者 web 或者 phone
    user->m_online_status_ = ostatus;
    user->m_client_net_id_ = m_net_hdr_.m_client_net_id_;
    user->m_client_ip_ = 0;
    user->m_connector_id_ = 0;
    user->m_logic_id_ = 0;
    user->m_login_time_ = (uint32_t)time(NULL);
    user->m_last_logout_time_ = 0;
    user->m_last_recv_status_time_ = (uint32_t)time(NULL);

    return true;
}
