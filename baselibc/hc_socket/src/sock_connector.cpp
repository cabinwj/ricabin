#include "sock_connector.h"

#include "hc_log.h"
#include "hc_stack_trace.h"

#include "sock_stream.h"
#include "packet_splitter.h"
#include "net_id_holder.h"
#include "net_messenger.h"
#include "net_handler.h"

#define CONNECTION_TIMEOUT        {3, 0}
#define CONNECTION_RETRY_INTERVAL    {3, 0}

// class sock_connector
object_holder<sock_connector>* sock_connector::m_pool_ = new \
     object_holder<sock_connector>( \
     new object_single_allocator<sock_connector>(new_allocator::Instance()));

sock_connector::sock_connector() : m_ev_mask_(0), m_net_id_(0),
    m_reactor_(NULL), m_user_data_(NULL), m_net_messenger_(NULL), m_packet_splitter_(NULL)
{
    STACK_TRACE_LOG();
}

int sock_connector::init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id)
{
    STACK_TRACE_LOG();

    m_net_messenger_ =  nm;
    m_packet_splitter_ = ps;

    m_net_id_ = net_id; 

    m_user_data_ = user_data;

    return 0;
}

sock_connector::~sock_connector()
{
    STACK_TRACE_LOG();

    if ( 0 != m_net_id_ )
    {
        m_net_messenger_->release_net_id(m_net_id_);
    }
}

int sock_connector::open(const Address& remote_addr, int recv_bufsize, int send_bufsize)
{
    STACK_TRACE_LOG();

    int rc = m_socket_.open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::open error, open error, errno:%d", error_no());
        return -1;
    }

    Socket::setnonblock(Descriptor(m_socket_));
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&recv_bufsize), sizeof(recv_bufsize));
    Socket::setsockopt(Descriptor(m_socket_), SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&send_bufsize), sizeof(send_bufsize));

    rc = m_socket_.connect(remote_addr);
    // NM_EINPROGRESS == error_no() 非阻塞connect,表示已经进入连接过程
    if ((0 != rc) && (error_no() != SYS_EINPROGRESS) && (error_no() != SYS_EALREADY) && (error_no() != SYS_EWOULDBLOCK))
    {
        LOG(ERROR)("sock_connector::open error, connect error, remote_addr<0x%08X:%d>, errno:%d", remote_addr.net_ip(), remote_addr.net_port(), error_no());
        m_socket_.close();
        return -1;
    }

    m_remote_addr_ = remote_addr;

    return 0;
}

void sock_connector::close()
{
    STACK_TRACE_LOG();
    m_socket_.close();
}

int32_t sock_connector::handler_o()
{
    return m_net_id_;
}

void sock_connector::handler_o(int32_t net_id)
{
    m_net_id_ = net_id;
}

Descriptor sock_connector::descriptor_o()
{
    return Descriptor(m_socket_);
}

void sock_connector::descriptor_o(Descriptor sock)
{
    m_socket_ = sock;
}

int32_t sock_connector::event_mask_o()
{
    return m_ev_mask_;
}

void sock_connector::event_mask_o(int32_t ev_mask)
{
    m_ev_mask_ = ev_mask;
}

void sock_connector::reactor_o(ireactor* react)
{
    m_reactor_ = react;
}

ireactor* sock_connector::reactor_o()
{
    return m_reactor_;
}

int sock_connector::handle_input()
{
    STACK_TRACE_LOG();
    LOG(ERROR)("sock_connector::handle_input, errno:%d", error_no());
    return -1;
}

int sock_connector::handle_output()
{
    STACK_TRACE_LOG();

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
    rc = m_reactor_->disable_handler(this, ihandler::EM_WRITE);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::handle_output, disable_handler error");
        return -2;
    }

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

    stream->init(m_net_messenger_, m_packet_splitter_, m_user_data_, m_net_id_, m_remote_addr_, Descriptor(m_socket_));

    // NOTICE sock_stream创建成功后, m_net_id_, m_socket的生命周期由sock_stream管理
    m_reactor_->disable_handler(this, ihandler::EM_ALL);
    m_net_id_ = 0;  // 保证该m_net_id_交由sock_stream后，在本sock_connector中不会因析构而被释放归还给该通道。
    m_socket_ = INVALID_SOCKET; // 保证该socket，交由sock_stream后，在本sock_connector中不会因析构而被close()。

    rc = m_reactor_->enable_handler(stream, ihandler::EM_READ | ihandler::EM_WRITE);
    if (0 != rc)
    {
        LOG(ERROR)("sock_connector::handle_output, enable_handler error");
        stream->close();
        stream->Destroy();
        return -2;
    }

    // throw a net event
    net_event* netev = net_event::m_pool_->Create();
    if ( NULL == netev )
    {
        LOG(ERROR)("assert: sock_connector::handle_output, new netev is NULL");
        return -1;
    }

    netev->m_net_ev_t_ = net_event::NE_CONNECTED;
    netev->m_net_id_ = stream->m_net_id_;
    netev->m_user_data_ = m_user_data_;
    netev->m_remote_addr_ = m_remote_addr_;

    m_net_messenger_->push_event(netev);

    // NOTICE
    this->Destroy();
    return 0;
}

int sock_connector::handle_close(int16_t evt)
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
            LOG(ERROR)("assert: sock_connector::handle_close error, new netev is NULL");
            this->Destroy();
            return -1;
        }

        netev->m_net_ev_t_ = evt;
        netev->m_net_id_ = m_net_id_;
        netev->m_user_data_ = m_user_data_;
        netev->m_remote_addr_ = m_remote_addr_;

        m_net_messenger_->push_event(netev);

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
    STACK_TRACE_LOG();
    LOG(ERROR)("sock_connector::post_package error, can't send on this socket");
    netpkg->Destroy();
    return 0;
}

