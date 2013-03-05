#include "sock_connector.h"
#include "sock_stream.h"
#include "net_id_guard.h"

#include "hc_log.h"


// class sock_connector
object_guard<sock_connector>* sock_connector::m_pool_ = new \
     object_guard<sock_connector>( \
     new object_single_allocator<sock_connector>(new_allocator::Instance()));

sock_connector::sock_connector() : m_net_manager_(NULL), m_packet_splitter_(NULL)
{
    m_listen_net_id_ = 0;
    m_net_id_ = 0;
    m_user_data_ = NULL;
}

int sock_connector::init(net_manager* nm, packet_splitter* ps, void* user_data,
                          uint32_t listen_net_id, uint32_t net_id)
{
    m_net_manager_ =  nm;
    m_packet_splitter_ = ps;

    m_listen_net_id_ = listen_net_id;
    m_net_id_ = net_id; 

    m_user_data_ = user_data;

    return 0;
}

sock_connector::~sock_connector()
{
    if ( 0 != m_net_id_ )
    {
        m_net_manager_->release_net_id(m_net_id_);
    }
}

int sock_connector::create_tcp_client(const Address& remote_addr, int timeout, int netbufsize)
{
    int rc = m_socket_.open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::create_tcp_client error, open error, errno:%d", error_no());
        return -1;
    }

    Socket::setnonblock(Descriptor(m_socket_));
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&netbufsize), sizeof(netbufsize));
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&netbufsize), sizeof(netbufsize));

    rc = m_socket_.connect(remote_addr);
    // NM_EINPROGRESS == error_no() 非阻塞connect,表示已经进入连接过程
    if ((0 != rc) && (error_no() != SYS_EINPROGRESS) && (error_no() != SYS_EALREADY) && (error_no() != SYS_EWOULDBLOCK))
    {
        LOG(ERROR)("sock_connector::create_tcp_client error, connect error, remote_addr<0x%08X:%d>, errno:%d", remote_addr.get_net_ip(), remote_addr.get_net_port(), error_no());
        m_socket_.close();
        return -1;
    }

    m_remote_addr_ = remote_addr;

    event_handler::sync_timeout(this, timeout);

    return 0;
}

void sock_connector::close_tcp_client()
{
    m_socket_.close();
}

int sock_connector::handle_input()
{
    LOG(ERROR)("sock_connector::handle_input, errno:%d", error_no());
    return -1;
}

int sock_connector::handle_output()
{
    int error = -1;
    socklen_t len = sizeof(error);

    int rc = Socket::getsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &len);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::handle_output, getsockopt error, errno:%d", error_no());
        return -2;
    }

    if (0 != error)
    {
        LOG(ERROR)("sock_connector::handle_output, getsockopt value:%d, errno:%d", error, error_no());
        return -2;
    }

    // connected
    rc = m_reactor_->disable_handler(this, event_handler::EM_WRITE);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::handle_output, disable_handler error");
        return -2;
    }

    // cancel timeout
    event_handler::sync_timeout(this, 0);

    // get socket local addr
    Address local_addr;
    rc = m_socket_.getsockname(local_addr);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::handle_output, getsockname error");
        return -2;
    }

    // create stream
    sock_stream* stream = sock_stream::m_pool_->Create();
    if ( NULL == stream )
    {
        LOG(ERROR)("assert: sock_connector::handle_output, new sock_stream is NULL");
        return -1;
    }

    stream->init(m_listen_net_id_, m_net_id_, m_net_manager_, m_packet_splitter_,
                 m_remote_addr_, Descriptor(m_socket_), m_user_data_);

    // NOTICE sock_stream创建成功后, m_net_id_, m_socket的生命周期由sock_stream管理
    m_reactor_->disable_handler(this, event_handler::EM_ALL);
    m_listen_net_id_ = 0;
    m_net_id_ = 0;  // 保证该m_net_id_交由sock_stream后，在本sock_connector中不会因析构而被释放归还给该通道。
    m_socket_ = INVALID_SOCKET; // 保证该socket，交由sock_stream后，在本sock_connector中不会因析构而被close()。

    rc = m_reactor_->enable_handler(stream, event_handler::EM_READ | event_handler::EM_WRITE);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::handle_output, enable_handler error");
        stream->close_stream();
        stream->Destroy();
        return -2;
    }

    // throw a net event
    net_event* netev = event_handler::m_net_ev_pool_->Create();
    if ( NULL == netev )
    {
        LOG(ERROR)("assert: sock_connector::handle_output, new netev is NULL");
        return -1;
    }

    netev->m_net_ev_t_ = net_event::NE_CONNECTED;
    netev->m_listen_net_id_ = stream->m_listen_net_id_;
    netev->m_net_id_ = stream->m_net_id_;
    netev->m_user_data_ = m_user_data_;
    netev->m_remote_addr_ = m_remote_addr_;

    m_net_manager_->push_event(netev);

    // NOTICE
    this->Destroy();
    return 0;
}

int sock_connector::handle_close(net_event::net_ev_t evt)
{
    switch (evt)
    {
    case net_event::NE_CLOSE:
    case net_event::NE_NOTIFY_CLOSE:
    case net_event::NE_EXCEPTION:
    case net_event::NE_TIMEOUT: {

        event_handler::remove_handler(this);

        m_socket_.close();

        net_event* netev = event_handler::m_net_ev_pool_->Create();
        if ( NULL == netev )
        {
            LOG(ERROR)("assert: sock_connector::handle_close error, new netev is NULL");
            this->Destroy();
            return -1;
        }

        netev->m_net_ev_t_ = evt;
        netev->m_listen_net_id_ = m_listen_net_id_;
        netev->m_net_id_ = m_net_id_;
        netev->m_user_data_ = m_user_data_;
        netev->m_remote_addr_ = m_remote_addr_;

        m_net_manager_->push_event(netev);

        this->Destroy();

    }break;
    default:
        this->Destroy();
        return -1;
    }

    

    return 0;
}

int sock_connector::post_package(net_package* netpkg)
{
    LOG(ERROR)("sock_connector::post_package error, can't send on this socket");
    netpkg->Destroy();
    return 0;
}

