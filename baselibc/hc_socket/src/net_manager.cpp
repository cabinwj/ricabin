#include "net_manager.h"

#include "hc_log.h"

#include "sock_connector.h"
#include "sock_acceptor.h"
#include "sock_stream.h"


// class net_manager
net_manager::net_manager()
{
    m_status_ = 0;
    m_net_thread_ = NULL;
    //! 队列容量为100万
    m_net_event_queue_ = new net_event_queue(MAX_NET_EVENT_COUNT);
    //! 队列容量为100万
    m_net_send_packet_queue_ = new net_package_queue(MAX_NET_SEND_TASK_COUNT);
}

net_manager::~net_manager()
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

net_manager* net_manager::Instance()
{
    static net_manager __net_manager;
    return &__net_manager;
}

int net_manager::start()
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
        LOG(ERROR)("net_manager::start error, alreay started");
        return -1;
    }

    event_handler::init_hash_table();

    int rc = m_reactor_.open_reactor();
    if (0 != rc)
    {
        LOG(ERROR)("net_manager::start error, open reactor error");
        return -1;
    }

    m_net_thread_ = new net_thread(this, &m_reactor_);

    rc = m_net_thread_->start();
    if (0 != rc)
    {
        LOG(ERROR)("net_manager::start error, start thread error");
        delete m_net_thread_;
        m_net_thread_ = NULL;
        m_reactor_.close_reactor();
        return -1;
    }

    m_status_ = 1;
    return 0;
}

int net_manager::stop()
{
    if (0 == m_status_)
    {
        LOG(ERROR)("net_manager::stop error, not started");
        return -1;
    }

    int rc = m_net_thread_->stop();
    if (0 != rc)
    {
        LOG(ERROR)("net_manager::stop error, stop thread error");
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

    m_reactor_.end_reactor_event_loop();

    m_status_ = 0;
    return 0;
}

uint32_t net_manager::create_tcp_client(const Address& remote_addr, packet_splitter* ps,
                                        void* user_data, int timeout, int netbufsize)
{
    if (1 != m_status_)
    {
        LOG(WARN)("net_manager::create_tcp_client error, not started");
        return 0;
    }

    sock_connector* connector = sock_connector::m_pool_->Create();
    if ( NULL == connector )
    {
        LOG(ERROR)("assert: net_manager::create_tcp_client() new sock_connector is NULL");
        return 0;
    }

    connector->init(this, ps, user_data, 0, this->acquire_net_id(net_id_guard::ID_TCP_CONNECTOR));

    uint32_t net_id = connector->m_net_id_;

    int rc = connector->create_tcp_client(remote_addr, timeout, netbufsize);
    if (0 != rc)
    {
        LOG(WARN)("net_manager::create_tcp_client error, create_tcp_client error");
        connector->close_tcp_client();
        connector->Destroy();
        return 0;
    }

    rc = m_reactor_.enable_handler(connector, event_handler::EM_WRITE);
    if (0 != rc)
    {
        LOG(WARN)("net_manager::create_tcp_client error, enable_handler error");
        connector->close_tcp_client();
        connector->Destroy();
        return 0;
    }

    return net_id;
}

uint32_t net_manager::create_tcp_client(const char* remote_ip, int remote_port, packet_splitter* ps, 
                                        void* user_data, int timeout, int netbufsize)
{
    Address remote_addr(remote_ip, remote_port);
    return create_tcp_client(remote_addr, ps, user_data, timeout, netbufsize);
}

uint32_t net_manager::create_tcp_server(const Address& local_addr, packet_splitter* ps, 
                                        void* user_data, int netbufsize)
{
    if (1 != m_status_)
    {
        LOG(WARN)("net_manager::create_tcp_server error, not started");
        return 0;
    }

    sock_acceptor* acceptor = sock_acceptor::m_pool_->Create();
    if ( NULL == acceptor )
    {
        LOG(ERROR)("assert: net_manager::create_tcp_server() new sock_acceptor is NULL");
        return 0;
    }

    acceptor->init(this, ps, user_data, this->acquire_net_id(net_id_guard::ID_TCP_ACCEPTOR), 0);

    uint32_t net_id = acceptor->m_net_id_;

    int rc = acceptor->create_tcp_server(local_addr, netbufsize);
    if (0 != rc)
    {
        LOG(WARN)("net_manager::create_tcp_server error, create_tcp_server error");
        acceptor->Destroy();
        return 0;
    }

    rc = m_reactor_.enable_handler(acceptor, event_handler::EM_READ);
    if (0 != rc)
    {
        LOG(WARN)("net_manager::create_tcp_server error, enable_handler error");
        acceptor->close_tcp_server();
        acceptor->Destroy();
        return 0;
    }

    return net_id;
}

uint32_t net_manager::create_tcp_server(const char* local_ip, int local_port, packet_splitter* ps, 
                                        void* user_data, int netbufsize)
{
    Address local_addr(local_ip, local_port);
    return create_tcp_server(local_addr, ps, user_data, netbufsize);
}

int net_manager::notify_close(uint32_t net_id)
{
    event_handler* eh = event_handler::get_handler(net_id);
    if ( NULL != eh )
    {
        // delete timeout from list, delete hash from list and close, and delete handler
        eh->handle_close(net_event::NE_NOTIFY_CLOSE);
    }

    return 0;
}

int net_manager::send_package(uint32_t net_id, net_package* netpkg)
{
    if (1 != m_status_)
    {
        LOG(WARN)("net_manager::send_packet error, not started");
        return -1;
    }

    if ((NULL == netpkg) || (0 == netpkg->length()))
    {
        LOG(WARN)("net_manager::send_packet error, netpkg is NULL or netpkg length is 0");
        return -1;
    }

    event_handler* eh = event_handler::get_handler(net_id);
    // 通道不存在
    if (NULL == eh)
    {
        LOG(WARN)("assert: net_manager::send_packet error, net id no exist, net id: %u", net_id);
        return -1;
    }

    netpkg->handler_id(net_id);

    int rc = m_net_send_packet_queue_->push(netpkg);
    // 队列用尽
    if (0 != rc)
    {
        LOG(WARN)("net_manager::send_packet error, net send packet queue is full");
        return -2;
    }

    return 0;
}

net_event* net_manager::pop_event()
{
    net_event* netev = NULL;
    int rc = m_net_event_queue_->pop(netev, false);
    if (0 != rc)
    {
        return NULL;
    }

    return netev;
}

net_reactor* net_manager::reactor_pointer()
{
    return &m_reactor_;
}

int net_manager::push_event(net_event* netev)
{
    int rc = m_net_event_queue_->push(netev);
    // 队列用尽
    if (0 != rc)
    {
        LOG(ERROR)("net_manager::put_event error, net event queue is full, net<%d:%d:%d>", netev->m_listen_net_id_, netev->m_net_id_, netev->m_net_ev_t_);
        netev->Destroy();
    }

    return rc;
}

int net_manager::net_event_count()
{
    return m_net_event_queue_->size();
}

uint32_t net_manager::acquire_net_id(net_id_guard::net_id_t type)
{
    return m_net_id_guard_.acquire(type);
}

void net_manager::release_net_id(uint32_t id)
{
    m_net_id_guard_.release(id);
}

int net_manager::net_send_package_count()
{
    return m_net_send_packet_queue_->size();
}

net_package* net_manager::pop_net_send_package()
{
    net_package* netpkg = NULL;
    int rc = m_net_send_packet_queue_->pop(netpkg, false);
    if (0 != rc)
    {
        return NULL;
    }

    return netpkg;
}

void net_manager::reactor_exception()
{
    m_status_ = 2;
    LOG(FATAL)("net_manager::reactor_exception(), epoll error, thread exist");
}

