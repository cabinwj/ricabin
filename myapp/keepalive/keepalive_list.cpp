#include "keepalive_list.h"

#include "hc_log.h"
#include "hc_allocator.h"
#include "hc_binpacket_tlv.h"

#include "ihandler.h"
#include "net_package.h"
#include "net_messenger.h"

#include "protocol.h"
#include "entity_types.h"


list_head keepalive::m_net_hash_list_[HASH_BUCKET_COUNT_];
list_head keepalive::m_alive_list_;

void keepalive::init()
{
    INIT_LIST_HEAD(&m_alive_list_);

    for (int i =0; i < HASH_BUCKET_COUNT_; i++)
    {
        INIT_LIST_HEAD(&m_net_hash_list_[i]);
    }
}

void keepalive::on_connected(uint32_t connect_net_id, uint16_t keepalive_interval, uint16_t keepalive_timeout)
{
    alive_t* alive = new alive_t;
    alive->m_net_id_ = connect_net_id;
    alive->m_keepalive_interval_ = keepalive_interval;
    alive->m_keepalive_timeout_ = keepalive_timeout;
    alive->m_next_alive_time_ = time(NULL) + keepalive_interval;
    alive->m_next_recv_alive_time_ = time(NULL) + keepalive_timeout;

    // add 2 connect list
    list_add_tail(&alive->m_list_item_, &m_alive_list_);
    // add 2 hash list
    int32_t npos = keepalive::hash_func(alive->m_net_id_);
    list_add_tail(&alive->m_hash_item_, &m_net_hash_list_[npos]);
}

void keepalive::on_accepted(uint32_t accepted_net_id, uint16_t keepalive_timeout)
{
    alive_t* alive = new alive_t;
    alive->m_net_id_ = accepted_net_id;
    alive->m_keepalive_interval_ = 0;
    alive->m_keepalive_timeout_ = keepalive_timeout;
    alive->m_next_alive_time_ = 0;
    alive->m_next_recv_alive_time_ = time(NULL) + keepalive_timeout;

    // add 2 accept list
    list_add_tail(&alive->m_list_item_, &m_alive_list_);
    // add 2 hash list
    int32_t npos = keepalive::hash_func(alive->m_net_id_);
    list_add_tail(&alive->m_hash_item_, &m_net_hash_list_[npos]);
}

void keepalive::on_recv_alive(uint32_t net_id)
{
    list_head* pos = NULL;
    alive_t* alive = NULL;
    int32_t npos = keepalive::hash_func(net_id);
    list_for_each(pos, &m_net_hash_list_[npos])
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
    alive->m_next_recv_alive_time_ = time(NULL) + alive->m_keepalive_timeout_;

    // add 2 connect list
    list_add_tail(&alive->m_list_item_, &m_alive_list_);
}

void keepalive::on_disconnect(uint32_t net_id)
{
    list_head* pos = NULL;
    alive_t* alive = NULL;
    int32_t npos = keepalive::hash_func(net_id);
    if ( list_empty(&m_net_hash_list_[npos]) )
    {
        return;
    }

    list_for_each(pos, &m_net_hash_list_[npos])
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

    // delete from accept or connect list
    list_del(&alive->m_list_item_);
    // delete from hash list
    list_del(&alive->m_hash_item_);

    delete alive;
}

void keepalive::on_check_alive(uint32_t now)
{
    list_head* pos = NULL;
    list_head* next = NULL;
    if ( list_empty(&m_alive_list_) )
    {
        return;
    }

    list_for_each_safe(pos, next, &m_alive_list_)
    {
        alive_t* alive = list_entry(pos, alive_t, m_list_item_);
        // 应该发心跳包了
        if ( (now >= alive->m_next_alive_time_) && (0 != alive->m_next_alive_time_))
        {
            alive->m_next_alive_time_ = now + alive->m_keepalive_interval_;

            // send alive request here
            keepalive::on_send_alive(alive->m_net_id_, message_async_syn);
        }

        // 对方发送/回应心跳超时了
        if ( (now > alive->m_next_recv_alive_time_) && (0 != alive->m_next_recv_alive_time_))
        {
            //1.跑完这个超时连接相关的挂起协程，关闭socket
            LOG(INFO)("keepalive::on_check_alive() recv keep alive time out and notify close. net_id:%u", alive->m_net_id_);

            net_messenger::Instance()->notify_close(alive->m_net_id_);
            //// 唤醒挂起的所有协程
            //coro_hash::on_awaken_coro(now);

            //2.删除 对每个建立的连接进行管理的超时的alive结构
            list_del(&alive->m_list_item_);
            list_del(&alive->m_hash_item_);
            delete alive;
        }
    }
}

void keepalive::on_send_alive(uint32_t net_id, message_t msg_t)
{
    //LOG(INFO)("keepalive::on_send_alive() net<%d:%d:%d>", net_id, m_local_uin_, msg_t);
    net_package* np = net_package::m_pool_->Create();
    if ( NULL == np )
    {
        LOG(ERROR)("assert: keepalive::on_send_alive() error, new np is NULL");
        return;
    }

    uint32_t head_len = sizeof(net_hdr_t);
    np->allocate_data_block(new_allocator::Instance(), head_len);

    output_binpacket outpkg(np->data_o(), np->capacity());

    net_hdr_t phdr_req;
    phdr_req.m_packet_len_ = head_len;
    phdr_req.m_message_id_ = 0;
    phdr_req.m_message_type_ = msg_t;
    //phdr_req.m_reserved_ = m_local_uin_;
    phdr_req.m_request_sequence_ = 0;
    phdr_req.m_control_type_ = message_alive;
    phdr_req.m_client_net_id_ = net_id;
    //phdr_req.m_client_uin_ = m_local_uin_;
    //phdr_req.m_from_uin_ = m_local_uin_;
    phdr_req.m_to_uin_ = 0;

    outpkg.offset_absolute(head_len);
    outpkg.write((char*)(&phdr_req), head_len); 

    np->offset_cursor(head_len);

    int rc = net_messenger::Instance()->send_package(net_id, np);
    if (0 != rc)
    {
        np->Destroy();
    }
}

