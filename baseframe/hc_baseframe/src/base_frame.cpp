#include "base_frame.h"

#include "hc_log.h"

#include "packet_splitter.h"
#include "net_messenger.h"

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

void sigusr1_handle(int signo)
{
    g_run_status.set(run_status_reload);
#ifndef WIN32
    signal(SIGUSR1, sigusr1_handle);
#endif
}

void sigusr2_handle(int signo)
{
    g_run_status.set(run_status_exit);
#ifndef WIN32
    signal(SIGUSR2, sigusr2_handle);
#endif
}

void signal_handler()
{
#ifndef WIN32
    signal(SIGUSR1, sigusr1_handle);
    signal(SIGUSR2, sigusr2_handle);
#endif
}

int init_environment()
{
    srand((uint32_t)time(NULL));
    g_run_status.set(run_status_reset);
    int rc = net_messenger::Instance()->start();
    if (0 != rc)
    {
        LOG(ERROR)("init_environment() start net manager error");
        return -1;
    }

    return 0;
}

void run_environment_once()
{
    // 处理网络事件
    net_event* netev = net_messenger::Instance()->pop_event();
    if (NULL == netev)
    {
        return;
    }

    LOG(TRACE)("run_environment_once() dispatch net event. net<%d:%d>", netev->m_net_id_, netev->m_net_ev_t_);

    conn_param_t* cp = (conn_param_t*)(netev->m_user_data_);

    switch (netev->m_net_ev_t_)
    {
    case net_event::NE_CONNECTED: {

        cp->m_net_event_cb_(*netev);
        netev->Destroy();

    } break;
    case net_event::NE_ACCEPT: {

        cp->acquire();
        cp->m_net_event_cb_(*netev);
        netev->Destroy();

    } break;
    case net_event::NE_DATA: {

        cp->m_net_event_cb_(*netev);
        netev->Destroy();

    } break;

    case net_event::NE_CLOSE:
    case net_event::NE_EXCEPTION: {

        cp->m_net_event_cb_(*netev);
        netev->Destroy();
        cp->release();

    } break;

    case net_event::NE_TIMEOUT:
        // 应用层主动关闭
    case net_event::NE_NOTIFY_CLOSE: {

        cp->m_net_event_cb_(*netev);
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

void stop_environment()
{
    net_messenger::Instance()->stop();
}

void release_tunnel(uint32_t net_id)
{
    net_messenger::Instance()->notify_close(net_id);
}
