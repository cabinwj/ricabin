#include "select_reactor.h"

#include "event_handler.h"
#include "hc_log.h"
#include "hc_list.h"

// class select_reactor
time_t select_reactor::m_last_scan_time_ = time(NULL);


select_reactor::select_reactor()
{
    FD_ZERO(&m_read_set_);
    FD_ZERO(&m_write_set_);
    FD_ZERO(&m_exception_set_);
}

select_reactor::~select_reactor()
{
}

int select_reactor::open_reactor()
{
    FD_ZERO(&m_read_set_);
    FD_ZERO(&m_write_set_);
    FD_ZERO(&m_exception_set_);
    return 0;
}

int select_reactor::close_reactor()
{
    return 0;
}

int select_reactor::run_reactor_event_loop()
{
    Descriptor max_fd = 0;

    FD_ZERO(&m_read_set_);
    FD_ZERO(&m_write_set_);
    FD_ZERO(&m_exception_set_);

    if (0 == event_handler::m_current_count_)
    {
        sleep_ms(10);
        return 0;
    }

    list_head* head;
    list_head* pos, *n;
    for (int index = 0; index < event_handler::HANDLER_TABLE_SIZE; index++)
    {
        head = &event_handler::m_hash_bucket_[index];
        list_for_each(pos, head)
        {
            event_handler* eh = list_entry(pos, event_handler, m_hash_item_);
            Descriptor fd = Descriptor(eh->m_socket_);
            if (eh->m_ev_mask_ & event_handler::EM_READ)
            {
                FD_SET(fd, &m_read_set_);
            }
            if (eh->m_ev_mask_ & event_handler::EM_WRITE)
            {
                FD_SET(fd, &m_write_set_);
            }
            if (true)
            {
                FD_SET(fd, &m_exception_set_);
            }
            if (fd > max_fd)
            {
                max_fd = fd;
            }
        }
    }

    max_fd++;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    // select事件
    int count = select((int)max_fd, &m_read_set_, &m_write_set_, &m_exception_set_, &timeout);
    if (count < 0)
    {
        LOG(FATAL)("select_reactor::run_reactor_event_loop error, select error, errno:%d", error_no());
        if (errno == EINTR)
        {
            return 0;
        }

        return -1;
    }

    for (int index = 0; index < event_handler::HANDLER_TABLE_SIZE; index++)
    {
        head = &event_handler::m_hash_bucket_[index];
        list_for_each_safe(pos, n, head)
        {
            event_handler* eh = list_entry(pos, event_handler, m_hash_item_);
            Descriptor fd = Descriptor(eh->m_socket_);
            uint32_t tunnel_id = eh->m_net_id_;

            if (true)
            {
                // 连接异常
                if (FD_ISSET(fd, &m_exception_set_))
                {
                    LOG(ERROR)("select_reactor::run_reactor_event_loop, got NE_EXCEPTION for tunnel_id:%u", tunnel_id);
                    eh->handle_close(net_event::NE_EXCEPTION);
                    continue;
                }
            }

            if (eh->m_ev_mask_ & event_handler::EM_READ)
            {
                if (FD_ISSET(fd, &m_read_set_))
                {
                    int rc = eh->handle_input();
                    LOG(TRACE)("select_reactor::handle_input return:%d for tunnel_id:%u", rc, tunnel_id);
                    // 连接关闭
                    if (-1 == rc)
                    {
                        eh->handle_close(net_event::NE_CLOSE);
                        continue;
                    }
                    // 连接异常
                    if (-2 == rc)
                    {
                        eh->handle_close(net_event::NE_EXCEPTION);
                        continue;
                    }
                }
            }

            if (eh->m_ev_mask_ & event_handler::EM_WRITE)
            {
                if (FD_ISSET(fd, &m_write_set_))
                {
                    int rc = eh->handle_output();
                    LOG(TRACE)("select_reactor::handle_output return:%d for tunnel_id:%u", rc, tunnel_id);
                    // 连接关闭
                    if (-1 == rc)
                    {
                        eh->handle_close(net_event::NE_CLOSE);
                        continue;
                    }
                    // 连接异常
                    if (-2 == rc)
                    {
                        eh->handle_close(net_event::NE_EXCEPTION);
                        continue;
                    }
                }
            }
        }
    }

    // 处理用户通知关闭的处理器和超时的处理器
    time_t current_time = time(NULL);
    // 定时处理，避免每次都跑，影响系统性能 1s
    if (current_time - select_reactor::m_last_scan_time_ > 1)
    {
        select_reactor::m_last_scan_time_ = current_time;
        // 定时器处理 检测超时
        event_handler::scan_timer(current_time);
    }

    return count;
}

int select_reactor::end_reactor_event_loop()
{
    event_handler::clear_hash_table();
    return close_reactor();
}

int select_reactor::enable_handler(event_handler* eh, uint32_t masks)
{
    eh->m_reactor_ = this;
    eh->m_ev_mask_ |= masks;

    if (true)
    {
        FD_SET(Descriptor(eh->m_socket_), &m_exception_set_);
    }

    if (eh->m_ev_mask_ & event_handler::EM_READ)
    {
        FD_SET(Descriptor(eh->m_socket_), &m_read_set_);
    }

    if (eh->m_ev_mask_ & event_handler::EM_WRITE)
    {
        FD_SET(Descriptor(eh->m_socket_), &m_write_set_);
    }


    uint32_t tunnel_id = eh->m_net_id_;
    event_handler* ehdr = event_handler::hunt_handler(tunnel_id);
    // not in. add
    if (NULL == ehdr)
    {
        event_handler::push_handler(eh, eh->m_net_id_);
    }

    return 0;
}

int select_reactor::disable_handler(event_handler* eh, uint32_t masks)
{
    eh->m_reactor_ = this;
    eh->m_ev_mask_ &= ~masks;

    uint32_t tunnel_id = eh->m_net_id_;
    event_handler* ehdr = event_handler::hunt_handler(tunnel_id);
    // not in. return
    if (NULL == ehdr)
    {
        return 0;
    }

    // delete
    if (0 == eh->m_ev_mask_)
    {
        event_handler::remove_handler(eh);
    }

    return 0;
}
