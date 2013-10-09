#include "sock_acceptor.h"

#include "hc_log.h"
#include "hc_stack_trace.h"

#include "ireactor.h"
#include "sock_stream.h"
#include "packet_splitter.h"
#include "net_messenger.h"
#include "net_id_holder.h"
#include "net_handler.h"

// class sock_acceptor
object_holder<sock_acceptor>* sock_acceptor::m_pool_ = new \
    object_holder<sock_acceptor>( \
    new object_single_allocator<sock_acceptor>(new_allocator::Instance()));

sock_acceptor::sock_acceptor() : m_ev_mask_(0), m_net_id_(0),
    m_reactor_(NULL), m_user_data_(NULL), m_net_messenger_(NULL), m_packet_splitter_(NULL)
{
    STACK_TRACE_LOG();
}

int sock_acceptor::init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id)
{
    STACK_TRACE_LOG();

    m_net_messenger_ = nm;
    m_packet_splitter_ = ps;

    m_net_id_ = net_id;

    m_user_data_ = user_data;

    return 0;
}

sock_acceptor::~sock_acceptor()
{
    STACK_TRACE_LOG();

    if ( 0 != m_net_id_ )
    {
        m_net_messenger_->release_net_id(m_net_id_);
    }
}

int sock_acceptor::open(const Address& local_addr, int recv_bufsize, int send_bufsize)
{
    STACK_TRACE_LOG();

    int rc = m_socket_.open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 != rc)
    {
        LOG(ERROR)("sock_acceptor::open error, open error, errno:%d", error_no());
        return -1;
    }

    Socket::setnonblock(Descriptor(m_socket_));

    //设置网络底层收发缓冲区长度
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&recv_bufsize), sizeof(recv_bufsize));
    int real_rcvbufsize = 0;
    socklen_t nsize = sizeof(real_rcvbufsize);
    Socket::getsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&real_rcvbufsize), &nsize);
    if (real_rcvbufsize != recv_bufsize)
    {
        LOG(WARN)("sock_acceptor::open error, setsockopt so_rcvbuf failed. set_size:%d real_size:%d.", recv_bufsize, real_rcvbufsize);
    }    

    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&send_bufsize), sizeof(send_bufsize));
    int real_sndbufsize = 0;
    Socket::getsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&real_sndbufsize), &nsize);
    if (real_sndbufsize != send_bufsize)
    {
        LOG(WARN)("sock_acceptor::open error, setsockopt so_sndbuf failed. set_size:%d real_size:%d.", send_bufsize, real_sndbufsize);
    }

    int val = 1;
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));

    rc = m_socket_.bind(local_addr);
    if (0 != rc)
    {
        LOG(ERROR)("sock_acceptor::open error, bind error, errno:%d", error_no());
        m_socket_.close();
        return -1;
    }

    rc = m_socket_.listen(10240);
    if (0 != rc)
    {
        LOG(ERROR)("sock_acceptor::open error, listen error, errno:%d", error_no());
        m_socket_.close();
        return -1;
    }

    return 0;
}

void sock_acceptor::close()
{
    STACK_TRACE_LOG();
    m_socket_.close();
}

int32_t sock_acceptor::handler_o()
{
    return m_net_id_;
}

void sock_acceptor::handler_o(int32_t net_id)
{
    m_net_id_ = net_id;
}

Descriptor sock_acceptor::descriptor_o()
{
    return Descriptor(m_socket_);
}

void sock_acceptor::descriptor_o(Descriptor sock)
{
    m_socket_ = sock;
}

int32_t sock_acceptor::event_mask_o()
{
    return m_ev_mask_;
}

void sock_acceptor::event_mask_o(int32_t ev_mask)
{
    m_ev_mask_ = ev_mask;
}

ireactor* sock_acceptor::reactor_o()
{
    return m_reactor_;
}

void sock_acceptor::reactor_o(ireactor* react)
{
    m_reactor_ = react;
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

        int32_t net_id = m_net_messenger_->acquire_net_id(net_id_holder::ID_TCP_STREAM);
        if (0 == net_id)
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

        stream->init(m_net_messenger_, m_packet_splitter_, m_user_data_, net_id, remote_addr, accept_socket);
        
        //注册读写事件
        int rc = m_reactor_->enable_handler(stream, ihandler::EM_READ | ihandler::EM_WRITE);
        if (0 != rc)
        {
            LOG(ERROR)("sock_acceptor::handle_input, enable_handler MASK_READ error");
            stream->close();
            stream->Destroy();
            return 0;
        }

        // throw a net event
        net_event* netev = net_event::m_pool_->Create();
        if ( NULL == netev )
        {
            LOG(ERROR)("assert: sock_connector::handle_input, new netev is NULL");
            return -1;
        }

        netev->m_net_ev_t_ = net_event::NE_ACCEPT;
        netev->m_net_id_ = stream->m_net_id_;
        netev->m_user_data_ = m_user_data_;
        netev->m_remote_addr_ = remote_addr;

        m_net_messenger_->push_event(netev);
    }

    return 0;
}

int sock_acceptor::handle_output()
{
    STACK_TRACE_LOG();
    LOG(ERROR)("sock_acceptor::handle_output, errno:%d", error_no());
    return -1;
}

int sock_acceptor::handle_close(int16_t evt)
{
    STACK_TRACE_LOG();

    switch (evt)
    {
    case net_event::NE_CLOSE:
    case net_event::NE_NOTIFY_CLOSE:
    case net_event::NE_EXCEPTION:
    case net_event::NE_TIMEOUT: {
        
        net_handler::remove_handler(this);

        m_socket_.close();

        net_event* netev = net_event::m_pool_->Create();
        if ( NULL == netev )
        {
            LOG(ERROR)("assert: sock_acceptor::handle_close error, new netev is NULL");
            this->Destroy();
            return -1;
        }

        netev->m_net_ev_t_ = evt;
        netev->m_net_id_ = m_net_id_;
        netev->m_user_data_ = m_user_data_;
        netev->m_remote_addr_.clear();

        m_net_messenger_->push_event(netev);

        this->Destroy();

    }break;
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

