#include "select_reactor.h"

#include "hc_log.h"

#include "ihandler.h"
#include "net_handler.h"
#include "net_event.h"

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

    if (0 == net_handler::current_handler_count() )
    {
        sleep_ms(10);
        return 0;
    }

    net_handler::net2handler_hashmap::iterator iter1 = net_handler::m_net2hdr_hashmap_.begin();
    for ( ; net_handler::m_net2hdr_hashmap_.end() != iter1; iter1++ )
    {
        ihandler* eh = iter1->second;
        //Descriptor fd = Descriptor(eh->m_socket_);
        Descriptor fd = eh->descriptor_o();
        //int32_t ev_mask = eh->m_ev_mask_;
        int32_t ev_mask = eh->event_mask_o();
        if ( ev_mask & ihandler::EM_READ )
        {
            FD_SET(fd, &m_read_set_);
        }
        if ( ev_mask & ihandler::EM_WRITE)
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

    max_fd++;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    // select事件
    int count = select((int)max_fd, &m_read_set_, &m_write_set_, &m_exception_set_, &timeout);
    if (count < 0)
    {
        LOG(FATAL)("select_reactor::run_reactor_event_loop error, select error, errno:%d", error_no());
        if (errno == SYS_EINTR)
        {
            return 0;
        }

        return -1;
    }

    net_handler::net2handler_hashmap::iterator iter2 = net_handler::m_net2hdr_hashmap_.begin();
    while (net_handler::m_net2hdr_hashmap_.end() != iter2)
    {
        ihandler* eh = iter2->second;
        //Descriptor fd = Descriptor(eh->m_socket_);
        Descriptor fd = eh->descriptor_o();
        //int32_t ev_mask = eh->m_ev_mask_;
        int32_t ev_mask = eh->event_mask_o();
        int32_t net_id = eh->handler_o();

        if (true)
        {
            // 连接异常
            if (FD_ISSET(fd, &m_exception_set_))
            {
                LOG(ERROR)("select_reactor::run_reactor_event_loop, got NE_EXCEPTION for net_id:%u", net_id);
                eh->handle_close(net_event::NE_EXCEPTION);
                iter2 = net_handler::m_net2hdr_hashmap_.erase(iter2);
                continue;
            }
        }

        if ( ev_mask & ihandler::EM_READ )
        {
            if (FD_ISSET(fd, &m_read_set_))
            {
                int rc = eh->handle_input();
                LOG(TRACE)("select_reactor::handle_input return:%d for net_id:%u", rc, net_id);
                // 连接关闭
                if (-1 == rc)
                {
                    eh->handle_close(net_event::NE_CLOSE);
                    iter2 = net_handler::m_net2hdr_hashmap_.erase(iter2);
                    continue;
                }
                // 连接异常
                if (-2 == rc)
                {
                    eh->handle_close(net_event::NE_EXCEPTION);
                    iter2 = net_handler::m_net2hdr_hashmap_.erase(iter2);
                    continue;
                }
            }
        }

        if ( ev_mask & ihandler::EM_WRITE )
        {
            if (FD_ISSET(fd, &m_write_set_))
            {
                int rc = eh->handle_output();
                LOG(TRACE)("select_reactor::handle_output return:%d for net_id:%u", rc, net_id);
                // 连接关闭
                if (-1 == rc)
                {
                    eh->handle_close(net_event::NE_CLOSE);
                    iter2 = net_handler::m_net2hdr_hashmap_.erase(iter2);
                    continue;
                }
                // 连接异常
                if (-2 == rc)
                {
                    eh->handle_close(net_event::NE_EXCEPTION);
                    iter2 = net_handler::m_net2hdr_hashmap_.erase(iter2);
                    continue;
                }
            }
        }

        iter2++;
    }

    return count;
}

int select_reactor::end_reactor_event_loop()
{
    net_handler::clear_all_handler();
    return close_reactor();
}

int select_reactor::enable_handler(ihandler* eh, int16_t masks)
{
    eh->reactor_o(this);
    eh->event_mask_o(eh->event_mask_o() | masks);
    //Descriptor fd = Descriptor(eh->m_socket_);
    Descriptor fd = eh->descriptor_o();
    //int32_t ev_mask = eh->m_ev_mask_;
    int32_t ev_mask = eh->event_mask_o();

    if (true)
    {
        FD_SET(fd, &m_exception_set_);
    }

    if ( ev_mask & ihandler::EM_READ )
    {
        FD_SET(fd, &m_read_set_);
    }

    if ( ev_mask & ihandler::EM_WRITE )
    {
        FD_SET(fd, &m_write_set_);
    }

    int32_t net_id = eh->handler_o();
    ihandler* ehdr = net_handler::select_handler(net_id);
    // not in. add
    if (NULL == ehdr)
    {
        net_handler::insert_handler(eh);
    }

    return 0;
}

int select_reactor::disable_handler(ihandler* eh, int16_t masks)
{
    eh->reactor_o(this);
    eh->event_mask_o(eh->event_mask_o() & ~masks);
    int32_t ev_mask = eh->event_mask_o();
    int32_t net_id = eh->handler_o();
    ihandler* ehdr = net_handler::select_handler(net_id);
    // not in. return
    if (NULL == ehdr)
    {
        return 0;
    }

    // delete
    if (0 == ev_mask)
    {
        net_handler::remove_handler(eh);
    }

    return 0;
}
