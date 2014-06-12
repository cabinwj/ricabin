#include "net_thread.h"

#include "hc_log.h"

#include "ireactor.h"
#include "net_messenger.h"
#include "net_package.h"
#include "net_handler.h"


net_thread::net_thread(net_messenger* nm, ireactor* nrc)
{
    m_is_run_ = false;
    m_notify_stop_ = false;
    m_net_messenger_ = nm;
    m_reactor_ = nrc;
}

net_thread::~net_thread()
{
}

int net_thread::start()
{
    if (false != m_is_run_)
    {
        return -1;
    }

    m_notify_stop_ = false;
    int rc = activate();
    if (0 != rc)
    {
        return -1;
    }

    m_is_run_ = true;
    return 0;
}

int net_thread::stop()
{
    m_notify_stop_ = true;
    wait();

    m_is_run_ = false;
    return 0;
}

int net_thread::svc()
{
    while (m_notify_stop_ != true)
    {
        while (true)
        {
            net_package* netpkg = m_net_messenger_->pop_net_send_package();
            if (NULL == netpkg)
            {
                break;
            }

            ihandler* eh = net_handler::select_handler(netpkg->handler_id());
            if (NULL == eh)
            {
                netpkg->Destroy();
                continue;
            }

            eh->post_package(netpkg);
        }

        // network event loop
        int event_count = m_reactor_->run_reactor_event_loop();
        // epoll错误, 线程退出
        if (event_count < 0)
        {
            LOG(FATAL)("net_thread::svc, epoll error, thread exist.");
            m_is_run_ = false;
            m_net_messenger_->reactor_exception();
            return -1;
        }
        // 睡眠通过epoll_wait 的 timeout 来实现
    }

    return 0;
}

