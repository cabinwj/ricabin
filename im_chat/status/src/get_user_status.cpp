#include "process_data.h"
#include "user_list.h"
#include "xml_configure.h"
#include "keep_alive.h"
#include "event_handler.h"
#include "net_manager.h"

#include "common_log.h"
#include <vector>


bool get_user_status::execute()
{
    LOG(INFO)("get_user_status::execute(). client_uin=%d,from_uin=%d,to_uin=%d", m_net_hdr_.m_client_uin_, m_net_hdr_.m_from_uin_, m_net_hdr_.m_to_uin_);
    binary_input_packet<true> inpkg(m_package_->get_data(), m_package_->size());

    net_hdr_t net_hdr;
    inpkg.offset_head(sizeof(net_hdr_t));
    inpkg.get_head(net_hdr);

    uint16_t count = 0;
    inpkg >> count;
    if (!inpkg.good())
    {
        LOG(ERROR)("report_user_status::execute() package invalue.");
        return false;
    }

    std::vector<uint32_t> uin_array;
    uint32_t user_uin = 0;
    for (int i = 0; i < count; i++)
    {
        inpkg >> user_uin;
        uin_array.push_back(user_uin);
    }

    if (!inpkg.good() || !inpkg.eof())
    {
        LOG(ERROR)("report_user_status::execute() package invalue.");
        return false;
    }

    net_hdr.m_packet_len_ = sizeof(net_hdr_t) + sizeof(count) + (count * sizeof(uint32_t) + sizeof(uint16_t));
    net_hdr.m_message_type_ = message_response;
    net_hdr.m_from_uin_ = m_net_hdr_.m_to_uin_;
    net_hdr.m_to_uin_ = m_net_hdr_.m_from_uin_;  // 此处要给定目的类型或是唯一标识

    // 分配新的 请求包
    net_package* np = event_handler::m_net_pkg_pool_->Create();
    if ( NULL == np )
    {
        LOG(ERROR)("assert: report_user_status::execute() error. new np is NULL");
        return false;
    }

    np->allocator_data_block(new_allocator::Instance(), net_hdr.m_packet_len_);

    // 组包 新的请求包
    binary_output_packet<true> outpkg(np->get_data(), np->capacity());
    outpkg.offset_head(sizeof(net_hdr_t));
    outpkg.set_head(net_hdr);

    std::vector<uint32_t>::const_iterator con_iter = uin_array.begin();
    for ( ; con_iter != uin_array.end(); con_iter++)
    {
        user_node* user = user_list::get(*con_iter);
        if ( NULL == user ) { outpkg << *con_iter << offline_status; }
        else { outpkg << *con_iter << user->m_online_status_; }
    }

    np->offset_cursor(net_hdr.m_packet_len_);

    // 原通道返回
    LOG(INFO)("report_user_status::execute() send_package, client_uin=%d,from_uin=%d,to_uin=%d", m_net_hdr_.m_client_uin_, m_net_hdr_.m_from_uin_, m_net_hdr_.m_to_uin_);
    int rc = net_manager::Instance()->send_package(m_current_net_id_, np);
    if (0 != rc)
    {
        np->Destroy();
    }

    return true;
}
