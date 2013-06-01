#include "sock_acceptor.h"
#include "sock_stream.h"
#include "net_id_guard.h"

#include "hc_log.h"
#include "hc_stack_trace.h"


// class sock_acceptor
object_guard<sock_acceptor>* sock_acceptor::m_pool_ = new \
    object_guard<sock_acceptor>( \
    new object_single_allocator<sock_acceptor>(new_allocator::Instance()));

sock_acceptor::sock_acceptor() : m_net_manager_(NULL), m_packet_splitter_(NULL)
{
    STACK_TRACE_LOG();

    m_listen_net_id_ = 0;
    m_net_id_ = 0;
    m_user_data_ = NULL;
}

int sock_acceptor::init(net_manager* nm, packet_splitter* ps, void* user_data,
                         uint32_t listen_net_id, uint32_t net_id)
{
    STACK_TRACE_LOG();

    m_net_manager_ = nm;
    m_packet_splitter_ = ps;

    m_listen_net_id_ = listen_net_id;
    m_net_id_ = net_id;

    m_user_data_ = user_data;

    return 0;
}

sock_acceptor::~sock_acceptor()
{
    STACK_TRACE_LOG();

    if ( 0 != m_listen_net_id_ )
    {
        m_net_manager_->release_net_id(m_listen_net_id_);
    }
}

int sock_acceptor::create_tcp_server(const Address& local_addr, int netbufsize)
{
    STACK_TRACE_LOG();

    int rc = m_socket_.open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 != rc)
    {
        LOG(ERROR)("sock_acceptor::create_tcp_server error, open error, errno:%d", error_no());
        return -1;
    }

    Socket::setnonblock(Descriptor(m_socket_));

    //设置网络底层收发缓冲区长度
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&netbufsize), sizeof(netbufsize));
    int real_buffersize = 0;
    socklen_t nsize = sizeof(real_buffersize);
    Socket::getsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&real_buffersize), &nsize);
    if (real_buffersize != netbufsize)
    {
        LOG(WARN)("sock_acceptor::create_tcp_server error, setsockopt so_rcvbuf failed. set_size:%d real_size:%d.", netbufsize, real_buffersize);
    }    

    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&netbufsize), sizeof(netbufsize));
    real_buffersize = 0;
    Socket::getsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&real_buffersize), &nsize);
    if (real_buffersize != netbufsize)
    {
        LOG(WARN)("sock_acceptor::create_tcp_server error, setsockopt so_sndbuf failed. set_size:%d real_size:%d.", netbufsize, real_buffersize);
    }

    int val = 1;
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));

    rc = m_socket_.bind(local_addr);
    if (0 != rc)
    {
        LOG(ERROR)("sock_acceptor::create_tcp_server error, bind error, errno:%d", error_no());
        m_socket_.close();
        return -1;
    }

    rc = m_socket_.listen(10240);
    if (0 != rc)
    {
        LOG(ERROR)("sock_acceptor::create_tcp_server error, listen error, errno:%d", error_no());
        m_socket_.close();
        return -1;
    }

    return 0;
}

void sock_acceptor::close_tcp_server()
{
    STACK_TRACE_LOG();

    m_socket_.close();
}

int sock_acceptor::handle_input()
{
    STACK_TRACE_LOG();

    uint32_t count = 0;
    while (true)
    {
        Address remote_addr;
        Descriptor accept_socket = m_socket_.accept(remote_addr);
        LOG(INFO)("sock_acceptor::handle_input accept_socket:%d, accept_times:%u", accept_socket, ++count);
        if ((INVALID_SOCKET == accept_socket) || (0 > accept_socket))
        {
            // 0 == error_no(): not client connecting keep acceptor
            // The socket is marked non-blocking and no connections are present to be accepted.
            if ( 0 == error_no() || SYS_EAGAIN == error_no() || SYS_EWOULDBLOCK == error_no() || SYS_EMFILE == error_no() || ENFILE == error_no() )
            {
                return 0;
            }

            // exception
            LOG(ERROR)("sock_acceptor::handle_input, accept error, errno:%d", error_no());
            return -2;
        }

        // got a new connection
        Socket::setnonblock(accept_socket);

        m_net_id_ = m_net_manager_->acquire_net_id(net_id_guard::ID_TCP_STREAM);
        if (0 == m_net_id_)
        {
            LOG(ERROR)("sock_acceptor::handle_input, acquire net id error, errno:%d", error_no());
            return -1;
        }

        sock_stream* stream = sock_stream::m_pool_->Create();
        if ( NULL == stream )
        {
            LOG(ERROR)("assert: sock_acceptor::handle_input, new sock_stream is NULL");
            return -1;
        }

        stream->init(m_listen_net_id_, m_net_id_, m_net_manager_, m_packet_splitter_,
                     remote_addr, accept_socket, m_user_data_);
        
        //注册读写事件
        int rc = m_reactor_->enable_handler(stream, event_handler::EM_READ | event_handler::EM_WRITE);
        if (0 != rc)
        {
            LOG(ERROR)("sock_acceptor::handle_input, enable_handler MASK_READ error");
            stream->close_stream();
            stream->Destroy();
            return 0;
        }

        // throw a net event
        net_event* netev = event_handler::m_net_ev_pool_->Create();
        if ( NULL == netev )
        {
            LOG(ERROR)("assert: sock_connector::handle_input, new netev is NULL");
            return -1;
        }

        netev->m_net_ev_t_ = net_event::NE_ACCEPT;
        netev->m_listen_net_id_ = stream->m_listen_net_id_;
        netev->m_net_id_ = stream->m_net_id_;
        netev->m_user_data_ = m_user_data_;
        netev->m_remote_addr_ = remote_addr;

        m_net_manager_->push_event(netev);
    }

    return 0;
}

int sock_acceptor::handle_output()
{
    STACK_TRACE_LOG();

    LOG(ERROR)("sock_acceptor::handle_output, errno:%d", error_no());
    return -1;
}

int sock_acceptor::handle_close(net_event::net_ev_t evt)
{
    STACK_TRACE_LOG();

    switch (evt)
    {
    case net_event::NE_CLOSE:
    case net_event::NE_NOTIFY_CLOSE:
    case net_event::NE_EXCEPTION: {
        
        event_handler::remove_handler(this);

        m_socket_.close();

        net_event* netev = event_handler::m_net_ev_pool_->Create();
        if ( NULL == netev )
        {
            LOG(ERROR)("assert: sock_acceptor::handle_close error, new netev is NULL");
            this->Destroy();
            return -1;
        }

        netev->m_net_ev_t_ = evt;
        netev->m_listen_net_id_ = m_listen_net_id_;
        netev->m_net_id_ = m_net_id_;
        netev->m_user_data_ = m_user_data_;
        netev->m_remote_addr_.clear();

        m_net_manager_->push_event(netev);

        this->Destroy();

    }break;
    case net_event::NE_TIMEOUT:
        this->Destroy();
        return -1;
    default:
        this->Destroy();
        return -1;
    }

    return 0;
}

int sock_acceptor::post_package(net_package* netpkg)
{
    STACK_TRACE_LOG();

    LOG(ERROR)("sock_acceptor::post_package error, can't send at this socket");
    netpkg->Destroy();
    return 0;
}

