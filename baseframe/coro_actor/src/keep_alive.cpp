#include "keep_alive.h"

#include "hc_allocator.h"
#include "hc_binpacket.h"
#include "hc_stack_trace.h"

#include "coroutine.h"
#include "scheduler.h"

#include "event_handler.h"
#include "net_package.h"
#include "net_manager.h"

#include "protocol.h"
#include "entity_types.h"
#include "coro_hash.h"


list_head keep_alive::m_net_id_hash_list_[HASH_BUCKET_COUNT_];
list_head keep_alive::m_connect_list_;
list_head keep_alive::m_listen_list_;

uint32_t keep_alive::m_local_uin_ = 0;

void keep_alive::init(uint32_t uin)
{
    STACK_TRACE_LOG();

    INIT_LIST_HEAD(&m_connect_list_);
    INIT_LIST_HEAD(&m_listen_list_);

    for (int i =0; i < HASH_BUCKET_COUNT_; i++)
    {
        INIT_LIST_HEAD(&m_net_id_hash_list_[i]);
    }

    m_local_uin_ = uin;
}

void keep_alive::on_connected(uint32_t connect_net_id, uint16_t keep_alive_interval, uint16_t keep_alive_timeout)
{
    STACK_TRACE_LOG();

    alive_t* alive = new alive_t;
    alive->m_net_id_ = connect_net_id;
    alive->m_listen_net_id_ = 0;
    alive->m_keep_alive_interval_ = keep_alive_interval;
    alive->m_keep_alive_timeout_ = keep_alive_timeout;
    alive->m_next_alive_time_ = time(NULL) + keep_alive_interval;
    alive->m_next_recv_alive_time_ = time(NULL) + keep_alive_timeout;

    // add 2 connect list
    list_add_tail(&alive->m_list_item_, &m_connect_list_);
    // add 2 hash list
    int32_t npos = keep_alive::hash_func(alive->m_net_id_);
    list_add_tail(&alive->m_hash_item_, &m_net_id_hash_list_[npos]);
}

void keep_alive::on_accepted(uint32_t listen_net_id, uint32_t accepted_net_id, uint16_t keep_alive_timeout)
{
    STACK_TRACE_LOG();

    alive_t* alive = new alive_t;
    alive->m_net_id_ = accepted_net_id;
    alive->m_listen_net_id_ = listen_net_id;
    alive->m_keep_alive_interval_ = 0;
    alive->m_keep_alive_timeout_ = keep_alive_timeout;
    alive->m_next_alive_time_ = 0;
    alive->m_next_recv_alive_time_ = time(NULL) + keep_alive_timeout;

    list_head* pos;
    listen_alive_t* listen = NULL;
    list_for_each(pos, &m_listen_list_)
    {
        listen_alive_t* tmp = list_entry(pos, listen_alive_t, m_listen_item_);
        if (tmp->m_listen_net_id_ == listen_net_id)
        {
            listen = tmp;
            break;
        }
    }

    if (NULL == listen)
    {
        listen = new listen_alive_t;
        listen->m_listen_net_id_ = listen_net_id;
        INIT_LIST_HEAD(&listen->m_accept_list_);
        // add 2 listen list
        list_add_tail(&listen->m_listen_item_, &m_listen_list_);
    }

    // add 2 accept list
    list_add_tail(&alive->m_list_item_, &listen->m_accept_list_);
    // add 2 hash list
    int32_t npos = keep_alive::hash_func(alive->m_net_id_);
    list_add_tail(&alive->m_hash_item_, &m_net_id_hash_list_[npos]);
}

void keep_alive::on_recv_alive(uint32_t net_id)
{
    STACK_TRACE_LOG();

    list_head* pos;
    alive_t* alive = NULL;
    int32_t npos = keep_alive::hash_func(net_id);
    list_for_each(pos, &m_net_id_hash_list_[npos])
    {
        alive_t* tmp = list_entry(pos, alive_t, m_hash_item_);
        if (tmp->m_net_id_ == net_id)
        {
            alive = tmp;
            break;
        }
    }

    if (NULL == alive)
    {
        return;
    }

    list_del(&alive->m_list_item_);
    alive->m_next_recv_alive_time_ = time(NULL) + alive->m_keep_alive_timeout_;

    if (0 != alive->m_listen_net_id_)
    {
        uint32_t listen_net_id = alive->m_listen_net_id_;
        list_for_each(pos, &m_listen_list_)
        {
            listen_alive_t* listen = list_entry(pos, listen_alive_t, m_listen_item_);
            if (listen->m_listen_net_id_ == listen_net_id)
            {
                // add 2 accept list
                list_add_tail(&alive->m_list_item_, &listen->m_accept_list_);
            }
        }
    }
    else
    {
        // add 2 connect list
        list_add_tail(&alive->m_list_item_, &m_connect_list_);
    }
}

void keep_alive::on_disconnect(uint32_t net_id)
{
    STACK_TRACE_LOG();

    list_head* pos;
    alive_t* alive = NULL;
    int32_t npos = keep_alive::hash_func(net_id);
    list_for_each(pos, &m_net_id_hash_list_[npos])
    {
        alive_t* tmp = list_entry(pos, alive_t, m_hash_item_);
        if(tmp->m_net_id_ == net_id)
        {
            alive = tmp;
            break;
        }
    }

    if (NULL == alive)
    {
        return;
    }

    // delete from accept or connect list
    list_del(&alive->m_list_item_);
    // delete from hash list
    list_del(&alive->m_hash_item_);

    if (0 != alive->m_listen_net_id_)
    {
        uint32_t listen_net_id = alive->m_listen_net_id_;
        list_for_each(pos, &m_listen_list_)
        {
            listen_alive_t* listen = list_entry(pos, listen_alive_t, m_listen_item_);
            if (listen->m_listen_net_id_ == listen_net_id)
            {
                if (list_empty(&listen->m_accept_list_))
                {
                    // delete from listen list
                    list_del(&listen->m_listen_item_);
                    delete listen;
                    break;
                }
            }
        }
    }

    delete alive;
}

void keep_alive::on_check_alive(uint32_t now)
{
    STACK_TRACE_LOG();

    list_head* pos;
    list_head* next;
    list_for_each_safe(pos, next, &m_connect_list_)
    {
        alive_t* alive = list_entry(pos, alive_t, m_list_item_);
        // 应该发心跳包了
        if (now >= alive->m_next_alive_time_)
        {
            alive->m_next_alive_time_ = now + alive->m_keep_alive_interval_;

            // send alive request here
            keep_alive::on_send_alive(alive->m_net_id_, message_async_syn);
        }

        // 对方心跳回应超时了
        if (now > alive->m_next_recv_alive_time_)
        {
            //1.跑完这个超时连接相关的挂起协程，关闭socket
            LOG(INFO)("keep_alive::on_check_alive() recv keep alive time out and notify close. net_id:%u", alive->m_net_id_);

            net_manager::Instance()->notify_close(alive->m_net_id_);
            // 唤醒挂起的所有协程
            coro_hash::on_awaken_coro(now);

            //2.删除 对每个建立的连接进行管理的超时的alive结构
            list_del(&alive->m_list_item_);
            list_del(&alive->m_hash_item_);
            delete alive;
        }
    }

    list_for_each_safe(pos, next, &m_listen_list_)
    {
        listen_alive_t* listen = list_entry(pos, listen_alive_t, m_listen_item_);
        list_head* pos2;
        list_head* next2;
        list_for_each_safe(pos2, next2, &listen->m_accept_list_)
        {
            alive_t* alive = list_entry(pos2, alive_t, m_list_item_);
            if (now <= alive->m_next_recv_alive_time_)
            {
                break;
            }

            // 对方心跳超时了
            //1.跑完这个超时连接相关的挂起协程，关闭socket
            LOG(INFO)("keep_alive::on_check_alive() recv keep alive time out and notify close. net_id:%u, listen_net_id:%u", alive->m_net_id_, alive->m_listen_net_id_);

            net_manager::Instance()->notify_close(alive->m_net_id_);
            // 唤醒挂起的所有协程
            coro_hash::on_awaken_coro(now);

            //2.删除 对每个建立的连接进行管理的超时的alive结构
            list_del(&alive->m_list_item_);
            list_del(&alive->m_hash_item_);

            if (list_empty(&listen->m_accept_list_))
            {
                list_del(&listen->m_listen_item_);
                delete listen;
            }

            delete alive;
        }
    }
}

void keep_alive::on_send_alive(uint32_t net_id, message_t msg_t)
{
    STACK_TRACE_LOG();

    LOG(INFO)("keep_alive::on_send_alive() net<%d:%d:%d>", net_id, m_local_uin_, msg_t);
    net_package* np = event_handler::m_net_pkg_pool_->Create();
    if ( NULL == np )
    {
        LOG(ERROR)("assert: keep_alive::on_send_alive() error, new np is NULL");
        return;
    }

    uint32_t real_pkglen = sizeof(net_hdr_t);
    np->allocator_data_block(new_allocator::Instance(), real_pkglen);

    binary_output_packet<true> outpkg(np->get_data(), np->capacity());

    net_hdr_t phdr_req;
    phdr_req.m_packet_len_ = real_pkglen;
    phdr_req.m_message_id_ = 0;
    phdr_req.m_message_type_ = msg_t;
    phdr_req.m_reserved_ = m_local_uin_;
    phdr_req.m_request_sequence_ = 0;
    phdr_req.m_control_type_ = message_alive;
    phdr_req.m_client_net_id_ = net_id;
    phdr_req.m_client_uin_ = m_local_uin_;
    phdr_req.m_from_uin_ = m_local_uin_;
    phdr_req.m_to_uin_ = 0;

    outpkg.offset_head(real_pkglen);
    outpkg.set_head(phdr_req);

    np->offset_cursor(real_pkglen);

    int rc = net_manager::Instance()->send_package(net_id, np);
    if (0 != rc)
    {
        np->Destroy();
    }
}

