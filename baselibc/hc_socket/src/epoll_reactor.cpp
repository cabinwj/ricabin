#include "epoll_reactor.h"

#ifndef WIN32
#include "event_handler.h"
#include "hc_log.h"
#include "hc_list.h"

// class epoll_reactor
time_t epoll_reactor::m_last_scan_time_ = time(NULL);

epoll_reactor::epoll_reactor()
{
    m_epfd = -1;
    m_events = new epoll_event[MAX_HANDLER * 2];
}

epoll_reactor::~epoll_reactor()
{
    delete [] m_events;
}

int epoll_reactor::open_reactor()
{
    m_epfd = epoll_create(MAX_HANDLER);
    if (-1 == m_epfd)
    {
        LOG(FATAL)("epoll_reactor::open_reactor error, epoll_create error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

int epoll_reactor::close_reactor()
{
    if (-1 != m_epfd)
    {
        close(m_epfd);
        m_epfd = -1;
    }

    return 0;
}

int epoll_reactor::run_reactor_event_loop()
{
    if (-1 == m_epfd)
    {
        return -1;
    }

    // epoll事件
    int count = epoll_wait(m_epfd, m_events, MAX_HANDLER, 10);
    if (count < 0)
    {
        LOG(FATAL)("epoll_reactor::run_reactor_event_loop error, epoll_wait error, errno:%d", error_no());
        if (error_no() == SYS_EINTR)
        {
            return 0;
        }

        return -1;
    }

    for (int i = 0; i < count; i++)
    {
        event_handler* eh = (event_handler*)m_events[i].data.ptr;
        uint32_t tunnel_id = eh->m_net_id_;

        // 异常
        if (m_events[i].events & EPOLLERR)
        {
            LOG(ERROR)("epoll_reactor::run_reactor_event_loop, got EPOLLERR for tunnel_id:%d", tunnel_id);
            eh->handle_close(net_event::NE_EXCEPTION);
            continue;
        }

        // 关闭
        if (m_events[i].events & EPOLLHUP)
        {
            LOG(ERROR)("epoll_reactor::run_reactor_event_loop, got EPOLLHUP for tunnel_id:%d", tunnel_id);
            eh->handle_close(net_event::NE_CLOSE);
            continue;
        }

        // 可读
        if (m_events[i].events & EPOLLIN)
        {
            int rc = eh->handle_input();
            LOG(TRACE)("epoll_reactor::handle_input return:%d for tunnel_id:%u", rc, tunnel_id);
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

        // 可写
        if (m_events[i].events & EPOLLOUT)
        {
            int rc = eh->handle_output();
            LOG(TRACE)("epoll_reactor::handle_output return:%d for tunnel_id:%u", rc, tunnel_id);
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

    // 处理用户通知关闭的处理器和超时的处理器
    time_t current_time = time(NULL);
    // 定时处理，避免每次都跑，影响系统性能 1s
    if (current_time - epoll_reactor::m_last_scan_time_ > 1)
    {
        epoll_reactor::m_last_scan_time_ = current_time;
        // 定时器处理 检测超时
        event_handler::scan_timer(current_time);
    }

    return count;
}

int epoll_reactor::end_reactor_event_loop()
{
    event_handler::clear_hash_table();
    return close_reactor();
}

int epoll_reactor::enable_handler(event_handler* eh, uint32_t masks)
{
    if (-1 == m_epfd)
    {
        return -1;
    }

    eh->m_reactor_ = this;
    eh->m_ev_mask_ |= masks;

    struct epoll_event ep_ev;
    ep_ev.events = 0;
    ep_ev.data.ptr = (void*)eh;

    ep_ev.events |= EPOLLET;

    if (eh->m_ev_mask_ & event_handler::EM_READ)
    {
        ep_ev.events |= EPOLLIN;
    }

    if (eh->m_ev_mask_ & event_handler::EM_WRITE)
    {
        ep_ev.events |= EPOLLOUT;
    }

    int rc = 0;
    uint32_t tunnel_id = eh->m_net_id_;
    event_handler* ehdr = event_handler::hunt_handler(tunnel_id);
    // event_handler is not in. add?
    if (NULL == ehdr)
    {
        // event_mask unset
        if (0 == eh->m_ev_mask_)
        {
            return -1;
        }

        // add
        rc = epoll_ctl(m_epfd, EPOLL_CTL_ADD, Descriptor(eh->m_socket_), &ep_ev);
        if (0 != rc)
        {
            LOG(FATAL)("epoll_reactor::enable_handler error, epoll_ctl EPOLL_CTL_ADD error, errno:%d", error_no());
            return -1;
        }

        event_handler::push_handler(eh, eh->m_net_id_);

        return 0;
    }

    // event_handler already in. modify (event_mask mask already modify: pointer same instance)
    rc = epoll_ctl(m_epfd, EPOLL_CTL_MOD, Descriptor(eh->m_socket_), &ep_ev);
    if (0 != rc)
    {
        LOG(FATAL)("epoll_reactor::enable_handler error, epoll_ctl EPOLL_CTL_MOD error, errno:%d", error_no());
        return -1;
    }

    return 0;
}

int epoll_reactor::disable_handler(event_handler* eh, uint32_t masks)
{
    if (-1 == m_epfd)
    {
        return -1;
    }

    eh->m_reactor_ = this;
    eh->m_ev_mask_ &= ~masks;

    struct epoll_event ep_ev;
    ep_ev.events = 0;
    ep_ev.data.ptr = (void*)eh;

    ep_ev.events |= EPOLLET;

    if (eh->m_ev_mask_ & event_handler::EM_READ)
    {
        ep_ev.events |= EPOLLIN;
    }

    if (eh->m_ev_mask_ & event_handler::EM_WRITE)
    {
        ep_ev.events |= EPOLLOUT;
    }

    int rc = 0;
    uint32_t tunnel_id = eh->m_net_id_;
    event_handler* ehdr = event_handler::hunt_handler(tunnel_id);
    // event_handler is not in. do nothing
    if (NULL == ehdr)
    {
        return 0;
    }

    // event_handler already in. modify?
    // event_mask clear, delete
    if (0 == eh->m_ev_mask_)
    {
        rc = epoll_ctl(m_epfd, EPOLL_CTL_DEL, Descriptor(eh->m_socket_), &ep_ev);
        if (0 != rc)
        {
            LOG(FATAL)("epoll_reactor::disable_handler error, epoll_ctl EPOLL_CTL_DEL error, errno:%d", error_no());
            return -1;
        }

        event_handler::remove_handler(eh);
    }
    // event_mask change, modify
    else
    {
        rc = epoll_ctl(m_epfd, EPOLL_CTL_MOD, Descriptor(eh->m_socket_), &ep_ev);
        if (0 != rc)
        {
            LOG(FATAL)("epoll_reactor::disable_handler error, epoll_ctl EPOLL_CTL_MOD error, errno:%d", error_no());
            return -1;
        }
    }

    return 0;
}

#endif
