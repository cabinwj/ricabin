#include "frame_base.h"

#include "hc_list.h"
#include "hc_log.h"
#include "hc_bitmap.h"
#include "hc_unique_sequence.h"

#include "net_manager.h"
#include "packet_splitter.h"

#include "protocol.h"
#include "entity_types.h"
#include "coro_hash.h"
#include "coro_timer_heap.h"
#include "keep_alive.h"
#include "session_list.h"


#ifndef WIN32
#include <sys/signal.h>
#endif

bitmap32   g_run_status;

void init_daemon( int nochdir, int noclose )
{
#ifndef WIN32
    daemon(nochdir, noclose);

    //ignore signals
    signal(SIGINT,  SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
}

int init_environment(uint32_t uin)
{
    srand((uint32_t)time(NULL));
    g_run_status.set(run_status_reset);
    coro_hash::init();
    coro_scheduler::init();
    keep_alive::init(uin);

    int rc = net_manager::Instance()->start();
    if (0 != rc)
    {
        LOG(ERROR)("init_environment() start net manager error");
        return -1;
    }

    return 0;
}

void run_environment_once()
{
    if (g_run_status.is_setted(run_status_exit))
    {
        net_manager::Instance()->stop();
    }

    coro_scheduler::schedule();
    timer_min_heap::scan_timer();
    keep_alive::on_check_alive((uint32_t)time(NULL));

    // 处理网络事件
    net_event* netev = net_manager::Instance()->pop_event();
    if (NULL == netev)
    {
        return;
    }

    LOG(TRACE)("run_environment_once() dispatch net event net<%d:%d:%d>", netev->m_listen_net_id_, netev->m_net_id_, netev->m_net_ev_t_);

    conn_param_t* cp = (conn_param_t*)(netev->m_user_data_);

    switch (netev->m_net_ev_t_)
    {
    case net_event::NE_CONNECTED: {

        if (cp->m_keep_alive_interval_ > 0)
        {
            keep_alive::on_connected(netev->m_net_id_, cp->m_keep_alive_interval_, cp->m_keep_alive_timeout_);
        }

        cp->m_net_event_handler_cb_(*netev);
        netev->Destroy();

    } break;
    case net_event::NE_ACCEPT: {

        cp->acquire();

        if (cp->m_keep_alive_timeout_ > 0)
        {
            keep_alive::on_accepted(netev->m_listen_net_id_, netev->m_net_id_, cp->m_keep_alive_timeout_);
        }

        cp->m_net_event_handler_cb_(*netev);
        netev->Destroy();

    } break;
    case net_event::NE_DATA: {

        if (PROTOCOL_ASYNC == cp->m_app_proto_)
        {
            binary_input_packet<true> inpkg(netev->m_net_package_->get_data(), netev->m_net_package_->size());
            net_hdr_t phdr;
            inpkg.offset_head(sizeof(net_hdr_t));
            inpkg.get_head(phdr);

            // 心跳包
            if (0 == phdr.m_message_id_)
            {
                LOG(INFO)("run_environment_once() receive keep alive heart beat. net<%u:%u:%u>", netev->m_listen_net_id_, netev->m_net_id_, netev->m_net_ev_t_);
                // 正好只有一个包头的长度
                if ( (!inpkg.good()) || (!inpkg.eof())) return;

                keep_alive::on_recv_alive(netev->m_net_id_);

                // 此处的心跳包，如果是请求，则要回应。以避免不间断的来回发。
                if ((message_async_syn == phdr.m_message_type_))
                {
                    // send alive response here
                    keep_alive::on_send_alive(netev->m_net_id_, message_async_ack);

                    net_node* client = session_list::get_net_node_by_net_id(netev->m_net_id_);
                    if (NULL == client)
                    {
                        LOG(ERROR)("run_environment_once() session_list::get_net_node_by_net_id return NULL.");
                        return;
                    }

                    // 心跳包
                    if ((message_async_syn == phdr.m_message_type_) && (0 != phdr.m_reserved_) && (0 == phdr.m_request_sequence_) &&
                        (message_alive == phdr.m_control_type_) && (netev->m_net_id_ == phdr.m_client_net_id_) &&
                        (phdr.m_reserved_ == phdr.m_client_uin_) && (phdr.m_reserved_ == phdr.m_from_uin_) && (0 == phdr.m_to_uin_))
                    {
                        client->m_remote_uin_ = phdr.m_reserved_;
                        client->m_is_trans_.set(is_trans_accept_server);
                        session_list::append_uin_to_list(client);
                    }
                }
            }
            else if ((message_async_ack == phdr.m_message_type_) && (0 != phdr.m_message_id_))
            {
                LOG(INFO)("run_environment_once() dispatch net event have async ack sequence:%u, net<%u:%u:%u>", phdr.m_reserved_, netev->m_listen_net_id_, netev->m_net_id_, netev->m_net_ev_t_);

                coroutine* coro = coro_hash::get_coro(phdr.m_reserved_);
                if ( NULL != coro )
                {
                    coro->set_io_result(netev);
                    coro_scheduler::switch_to(coro);
                }

                netev->Destroy();
            }
            else // if ( message_common == type  ||  message_async_syn == type)
            {
                cp->m_net_event_handler_cb_(*netev);
                netev->Destroy();
            }
        }
        else if (PROTOCOL_SIMPLE == cp->m_app_proto_)
        {
            cp->m_net_event_handler_cb_(*netev);
            netev->Destroy();
        }
        else if (PROTOCOL_TEXT4 == cp->m_app_proto_)
        {
            cp->m_net_event_handler_cb_(*netev);
            netev->Destroy();
        }
        else if (PROTOCOL_TEXT6 == cp->m_app_proto_)
        {
            cp->m_net_event_handler_cb_(*netev);
            netev->Destroy();
        }

    } break;

    case net_event::NE_CLOSE:
    case net_event::NE_EXCEPTION: {

        // 唤醒挂起的所有协程
        coro_hash::on_awaken_coro((uint32_t)time(NULL));

        if (cp->m_keep_alive_interval_ || cp->m_keep_alive_timeout_)
        {
            keep_alive::on_disconnect(netev->m_net_id_);
        }

        cp->m_net_event_handler_cb_(*netev);
        netev->Destroy();
        cp->release();

    } break;

    case net_event::NE_TIMEOUT:
        // 应用层主动关闭
    case net_event::NE_NOTIFY_CLOSE: {

        cp->m_net_event_handler_cb_(*netev);
        netev->Destroy();
        cp->release();

    } break;

    default: {
        // do nothing
        netev->Destroy();
        cp->release();
    } break;
    }
}

void run_environment()
{
    while (true)
    {
        run_environment_once();
        sleep_ms(10);
    }
}

void stop_environment()
{
    g_run_status.set(run_status_exit);
}

uint32_t create_network(const net_conf_t& nconf, net_event_callback_t handler)
{
    conn_param_t* connprm = new conn_param_t;
    connprm->m_app_proto_ = nconf.m_app_proto_;

    connprm->m_net_event_handler_cb_ = handler;

    connprm->m_remote_uin_ = nconf.m_connect_out_uin_;

    connprm->m_reconnect_interval_ = nconf.m_reconnect_interval_;   // connect
    connprm->m_keep_alive_interval_ = nconf.m_keep_alive_interval_; // connect
    connprm->m_keep_alive_timeout_ = nconf.m_keep_alive_timeout_;

    packet_splitter* bin_split = NULL;
    if (SOCKET_PROTO_TCP == nconf.m_proto_type_)
    {
        switch (connprm->m_app_proto_)
        {
        case PROTOCOL_ASYNC:
            bin_split = binary4_splitter::Instance();
            break;

        case PROTOCOL_SIMPLE:
            bin_split = binary2_splitter::Instance();
            break;

        case PROTOCOL_TEXT4:
        case PROTOCOL_TEXT6:
            break;

        default:
            break;
        }
    }

    uint32_t net_id = 0;

    switch (connprm->m_app_proto_)
    {
    case PROTOCOL_ASYNC:
    case PROTOCOL_SIMPLE: {
        if (SOCKET_PROTO_TCP == nconf.m_proto_type_)
        {
            if (nconf.m_is_listen_)
            {
                net_id = net_manager::Instance()->create_tcp_server(nconf.m_ipport_.m_ip_, nconf.m_ipport_.m_port_, bin_split,
                                                                    (void*)connprm, (int)nconf.m_socket_buffer_size_);
                if (0 == net_id)
                {
                    LOG(ERROR)("create network create tcp server error");
                    delete connprm;
                    connprm = NULL;
                    return 0;
                }
            }
            else
            {
                net_id = net_manager::Instance()->create_tcp_client(nconf.m_ipport_.m_ip_, nconf.m_ipport_.m_port_, bin_split,
                                                                    (void*)connprm, nconf.m_connect_timeout_, (int)nconf.m_socket_buffer_size_);
                if (0 == net_id)
                {
                    LOG(ERROR)("create network create tcp connect server error");
                    delete connprm;
                    connprm = NULL;
                    return 0;
                }
            }
        }
    } break;

    case PROTOCOL_TEXT4:
    case PROTOCOL_TEXT6:
        delete connprm;
        connprm = NULL;
        return 0;

    default:
        delete connprm;
        connprm = NULL;
        return 0;
    }

    return net_id;
}

void release_tunnel(uint32_t net_id)
{
    LOG(INFO)("release_tunnel() notify close. net_id:%u", net_id);
    net_manager::Instance()->notify_close(net_id);

    // 唤醒挂起的所有协程
    coro_hash::on_awaken_coro((uint32_t)time(NULL));

    // 删除心跳信息
    keep_alive::on_disconnect(net_id);
}

int send_async_package(uint32_t net_id, net_package*& np, int timeout, net_event*& ne)
{
    uint32_t async_sequence = unique_sequence32::Instance()->sequence();
    LOG(TRACE)("send_async_package() net id:%u, sequence id:%u", net_id, async_sequence);

    // 组异步请求包，跳转到包头中的type位置开始写
    binary_output_packet<true> outpkg(np->get_data(), np->size());

    outpkg.offset(sizeof(uint32_t) + sizeof(uint16_t));
    outpkg << (uint16_t)message_async_syn << async_sequence;

    int rc = net_manager::Instance()->send_package(net_id, np);
    if (0 != rc)
    {
        // 发送失败
        return -1;
    }

    // 包发出去后，等待回应(相当于阻塞操作)
    coroutine* coro = coro_scheduler::current_coro();
    // 当前协程放入异步请求阻塞协程的任务队列。
    coro_hash::push_coro(net_id, async_sequence, coro);

    // 设置定时器(与协程关联)
    coro_timer* ctimer = new coro_timer(timeout, coro);
    timer_min_heap::enable_timer(ctimer);

    // 切回主协程
    coro_scheduler::yield();

    // 异步回应超时
    if (-1 == ctimer->min_heap_idx())
    {
        // 超时，移出阻塞协程的任务队列
        coro_hash::remove_coro(coro);
        return -2;
    }

    // 从io result中拿event，if (NULL == event)表示 socket close or exception
    ne = (net_event*)coro_scheduler::current_coro()->get_io_result();
    timer_min_heap::disable_timer(ctimer);

    // 移出阻塞协程的任务队列
    coro_hash::remove_coro(coro);

    return 0;
}

void coro_sleep(int32_t timeout)
{
    coroutine* coro = coro_scheduler::current_coro();
    // 用堆内存，销毁由coro timer heap负责
    coro_timer* corotimer = new coro_timer(timeout, coro);

    timer_min_heap::enable_timer(corotimer);
    coro_scheduler::yield();
}

