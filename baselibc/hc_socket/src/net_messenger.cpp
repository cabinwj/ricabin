#include "net_messenger.h"

#include "hc_log.h"

#include "ihandler.h"
#include "sock_connector.h"
#include "sock_acceptor.h"
#include "sock_stream.h"
#include "net_handler.h"
#include "net_thread.h"
#include "net_config.h"

// class net_messenger
net_messenger::net_messenger()
{
    m_status_ = 0;
    m_net_thread_ = NULL;
    //! 队列容量为100万
    m_net_event_queue_ = new net_event_queue(MAX_NET_EVENT_COUNT);
    //! 队列容量为100万
    m_net_send_packet_queue_ = new net_package_queue(MAX_NET_SEND_TASK_COUNT);
}

net_messenger::~net_messenger()
{
    if ( NULL != m_net_thread_ )
    {
        delete m_net_thread_;
        m_net_thread_ = NULL;
    }

    if ( NULL != m_net_event_queue_ )
    {
        delete m_net_event_queue_;
        m_net_event_queue_ = NULL;
    }

    if ( NULL != m_net_send_packet_queue_ )
    {
        delete m_net_send_packet_queue_;
        m_net_send_packet_queue_ = NULL;
    }
}

net_messenger* net_messenger::Instance()
{
    static net_messenger __net_messenger;
    return &__net_messenger;
}

int net_messenger::start()
{
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( 0 != err )
    {
        return 1;
    }

    if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 )
    {
        WSACleanup( );
        return -1; 
    }
#endif 

    if ( 0 != m_status_ )
    {
        LOG(ERROR)("net_messenger::start error, alreay started");
        return -1;
    }

    int rc = m_reactor_.open_reactor();
    if (0 != rc)
    {
        LOG(ERROR)("net_messenger::start error, open reactor error");
        return -1;
    }

    m_net_thread_ = new net_thread(this, &m_reactor_);

    rc = m_net_thread_->start();
    if (0 != rc)
    {
        LOG(ERROR)("net_messenger::start error, start thread error");
        delete m_net_thread_;
        m_net_thread_ = NULL;
        m_reactor_.close_reactor();
        return -1;
    }

    m_status_ = 1;
    return 0;
}

int net_messenger::stop()
{
    if (0 == m_status_)
    {
        LOG(ERROR)("net_messenger::stop error, not started");
        return -1;
    }

    int rc = m_net_thread_->stop();
    if (0 != rc)
    {
        LOG(ERROR)("net_messenger::stop error, stop thread error");
        return -1;
    }

    delete m_net_thread_;
    m_net_thread_ = NULL;

    m_reactor_.end_reactor_event_loop();
    m_reactor_.close_reactor();

    //清除未发送分发完的数据包
    net_package* netpkg = NULL;
    while (!m_net_send_packet_queue_->pop(netpkg, true))
    {
        netpkg->Destroy();
    }

    //清除未提交给用户的网络事件
    net_event* netev = NULL;
    while (!m_net_event_queue_->pop(netev, true))
    {
        netev->Destroy();
    }

    m_status_ = 0;
    return 0;
}

int32_t net_messenger::create_tcp_client(const Address& remote_addr, ipacket_splitter* ps, void* user_data,
                                         int recv_bufsize, int send_bufsize)
{
    if (1 != m_status_)
    {
        LOG(WARN)("net_messenger::create_tcp_client error, not started");
        return 0;
    }

    sock_connector* connector = sock_connector::m_pool_->Create();
    if ( NULL == connector )
    {
        LOG(ERROR)("assert: net_messenger::create_tcp_client new sock_connector is NULL");
        return 0;
    }

    connector->init(this, ps, user_data, this->acquire_net_id(net_id_holder::ID_TCP_CONNECTOR));

    int32_t net_id = connector->handler_o();

    int rc = connector->open(remote_addr, recv_bufsize, send_bufsize);
    if (0 != rc)
    {
        LOG(WARN)("net_messenger::create_tcp_client error, open error");
        connector->close();
        connector->Destroy();
        return 0;
    }

    rc = m_reactor_.enable_handler(connector, ihandler::EM_WRITE);
    if (0 != rc)
    {
        LOG(WARN)("net_messenger::create_tcp_client error, enable_handler error");
        connector->close();
        connector->Destroy();
        return 0;
    }

    return net_id;
}

int32_t net_messenger::create_tcp_client(const char* remote_ip, int remote_port, ipacket_splitter* ps, void* user_data,
                                         int recv_bufsize, int send_bufsize)
{
    Address remote_addr(remote_ip, remote_port);
    return create_tcp_client(remote_addr, ps, user_data, recv_bufsize, send_bufsize);
}

int32_t net_messenger::create_tcp_server(const Address& local_addr, ipacket_splitter* ps, void* user_data,
                                         int recv_bufsize, int send_bufsize)
{
    if (1 != m_status_)
    {
        LOG(WARN)("net_messenger::create_tcp_server error, not started");
        return 0;
    }

    sock_acceptor* acceptor = sock_acceptor::m_pool_->Create();
    if ( NULL == acceptor )
    {
        LOG(ERROR)("assert: net_messenger::create_tcp_server new sock_acceptor is NULL");
        return 0;
    }

    acceptor->init(this, ps, user_data, this->acquire_net_id(net_id_holder::ID_TCP_ACCEPTOR));

    int32_t net_id = acceptor->handler_o();

    int rc = acceptor->open(local_addr, recv_bufsize, send_bufsize);
    if (0 != rc)
    {
        LOG(WARN)("net_messenger::create_tcp_server error, open error");
        acceptor->Destroy();
        return 0;
    }

    rc = m_reactor_.enable_handler(acceptor, ihandler::EM_READ);
    if (0 != rc)
    {
        LOG(WARN)("net_messenger::create_tcp_server error, enable_handler error");
        acceptor->close();
        acceptor->Destroy();
        return 0;
    }

    return net_id;
}

int32_t net_messenger::create_tcp_server(const char* local_ip, int local_port, ipacket_splitter* ps, void* user_data,
                                         int recv_bufsize, int send_bufsize)
{
    Address local_addr(local_ip, local_port);
    return create_tcp_server(local_addr, ps, user_data, recv_bufsize, send_bufsize);
}

int net_messenger::notify_close(int32_t net_id, int16_t evt)
{
    ihandler* eh = net_handler::select_handler(net_id);
    if ( NULL != eh )
    {
        // delete timeout from list, delete hash from list and close, and delete handler
        eh->handle_close(evt);
    }

    return 0;
}

int net_messenger::send_package(int32_t net_id, net_package* netpkg)
{
    if (1 != m_status_)
    {
        LOG(WARN)("net_messenger::send_package error, not started");
        return -1;
    }

    if ((NULL == netpkg) || (0 == netpkg->length()))
    {
        LOG(WARN)("net_messenger::send_package error, netpkg is NULL or netpkg length is 0");
        return -1;
    }

    ihandler* eh = net_handler::select_handler(net_id);
    // 通道不存在
    if (NULL == eh)
    {
        LOG(WARN)("assert: net_messenger::send_package error, net id no exist, net id: %u", net_id);
        return -1;
    }

    netpkg->handler_id(net_id);

    int rc = m_net_send_packet_queue_->push(netpkg);
    // 队列用尽
    if (0 != rc)
    {
        LOG(WARN)("net_messenger::send_package error, net send packet queue is full");
        return -2;
    }

    return 0;
}

net_event* net_messenger::pop_event()
{
    net_event* netev = NULL;
    int rc = m_net_event_queue_->pop(netev, false);
    if (0 != rc)
    {
        return NULL;
    }

    return netev;
}

net_reactor* net_messenger::reactor_pointer()
{
    return &m_reactor_;
}

int net_messenger::push_event(net_event* netev)
{
    int rc = m_net_event_queue_->push(netev);
    // 队列用尽
    if (0 != rc)
    {
        LOG(ERROR)("net_messenger::push_event error, net event queue is full, net<%d:%d>", netev->m_net_id_, netev->m_net_ev_t_);
        netev->Destroy();
    }

    return rc;
}

int net_messenger::net_event_count()
{
    return m_net_event_queue_->size();
}

int32_t net_messenger::acquire_net_id(net_id_holder::net_id_t type)
{
    return m_net_id_holder_.acquire(type);
}

void net_messenger::release_net_id(int32_t id)
{
    m_net_id_holder_.release(id);
}

int net_messenger::net_send_package_count()
{
    return m_net_send_packet_queue_->size();
}

net_package* net_messenger::pop_net_send_package()
{
    net_package* netpkg = NULL;
    int rc = m_net_send_packet_queue_->pop(netpkg, false);
    if (0 != rc)
    {
        return NULL;
    }

    return netpkg;
}

void net_messenger::reactor_exception()
{
    m_status_ = 2;
    LOG(FATAL)("net_messenger::reactor_exception(), epoll error, thread exist");
}

