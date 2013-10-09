#include "sock_stream.h"

#include "hc_log.h"
#include "hc_stack_trace.h"

#include "packet_splitter.h"
#include "net_id_holder.h"
#include "net_messenger.h"
#include "net_handler.h"

// class sock_stream
//! stream套接字池70万
object_holder<sock_stream>* sock_stream::m_pool_ = new \
    object_holder<sock_stream>( \
    new object_pool_allocator<sock_stream>(SOCK_STREAM_POOL_COUNT, \
                                           (net_id_holder::TCP_STREAM_END - net_id_holder::TCP_STREAM_BEGIN + 1), \
                                           SOCK_STREAM_POOL_COUNT, \
                                           new_allocator::Instance()));

sock_stream::sock_stream() : m_ev_mask_(0), m_net_id_(0),
    m_reactor_(NULL), m_user_data_(NULL), m_net_messenger_(NULL), m_packet_splitter_(NULL),
    m_send_netpkg_(NULL), m_send_netpkglen_(0), m_net_event_(NULL), m_remain_len_(0),
    m_socket_send_packet_queue_(NULL)
{
    STACK_TRACE_LOG();
}

int sock_stream::init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id, const Address& remote_addr, Descriptor socket)
{
    STACK_TRACE_LOG();

    m_net_messenger_ = (NULL == nm) ? net_messenger::Instance() : nm;
    m_packet_splitter_ = (NULL == ps) ? ipacket_splitter::Instance() : ps;

    m_remote_addr_ = remote_addr;

    m_net_id_ = net_id;

    m_socket_ = socket;

    m_user_data_ = user_data;
    
    m_send_netpkg_ = NULL;
    m_send_netpkglen_ = 0;

    m_net_event_ = NULL;
    m_remain_len_ = 0;

    //! 通道待发送任务队列
    //! 此队列线程安全
    //! 队列容量为1万
    m_socket_send_packet_queue_ = new net_package_queue(MAX_SOCKET_SEND_TASK_COUNT);

    return 0;
}

sock_stream::~sock_stream()
{
    STACK_TRACE_LOG();

    if (0 != m_net_id_)
    {
        m_net_messenger_->release_net_id(m_net_id_);
    }

    //清除未发完的数据包
    if (NULL != m_send_netpkg_)
    {
        m_send_netpkg_->Destroy();
        m_send_netpkg_ = NULL;
    }
    
    while (!m_socket_send_packet_queue_->pop(m_send_netpkg_, true))
    {
        m_send_netpkg_->Destroy();
    }
}

int sock_stream::open(const Address& local_addr, int recv_bufsize, int send_bufsize)
{
    STACK_TRACE_LOG();
    return 0;
}

void sock_stream::close()
{
    STACK_TRACE_LOG();
    m_socket_.close();
}

int32_t sock_stream::handler_o()
{
    return m_net_id_;
}

void sock_stream::handler_o(int32_t net_id)
{
    m_net_id_ = net_id;
}

Descriptor sock_stream::descriptor_o()
{
    return Descriptor(m_socket_);
}

void sock_stream::descriptor_o(Descriptor sock)
{
    m_socket_ = sock;
}

int32_t sock_stream::event_mask_o()
{
    return m_ev_mask_;
}

void sock_stream::event_mask_o(int32_t ev_mask)
{
    m_ev_mask_ = ev_mask;
}

ireactor* sock_stream::reactor_o()
{
    return m_reactor_;
}

void sock_stream::reactor_o(ireactor* react)
{
    m_reactor_ = react;
}

int sock_stream::handle_input()
{
    STACK_TRACE_LOG();

    LOG(INFO)("sock_stream::handle_input, net<%u>, remote_addr<0x%08X:%d>", m_net_id_, m_remote_addr_.net_ip(), m_remote_addr_.net_port());
    // TODO: 检查此处的处理逻辑，是否会造成由于单个连接而拖累整个服务
    while (true)
    {
        int rc = m_socket_.recv(m_recv_buffer_.wr_ptr(), m_recv_buffer_.space_length(), 0);
        // close
        if (0 == rc)
        {
            LOG(INFO)("sock_stream::handle_input, close by peer");
            return -1;
        }
        // error
        if (0 > rc)
        {
            if (SYS_EAGAIN == error_no())
            {
                LOG(WARN)("sock_stream::handle_input, recv EAGAIN");
                return 0;
            }
            // exception
            else
            {
                LOG(ERROR)("sock_stream::handle_input, recv error, errno:%d", error_no());
                return -2;
            }
        }

        // 读出数据 if (0 < rc)
        m_recv_buffer_.wr_ptr(rc);

        LOG(TRACE)("rb_data_len:%d, rb_space_len:%d, remain_len:%d, rc:%d", m_recv_buffer_.data_length(), m_recv_buffer_.space_length(), m_remain_len_, rc);

        // 该数据包已经分片接收过数据的分片，但数据不完整
        if (NULL != m_net_event_)
        {
            // 已经接收到分片数据包，但不能拼接成完整的数据包，则继续等待
            if (m_recv_buffer_.data_length() < m_remain_len_)
            {
                m_net_event_->m_net_package_->data_o(m_recv_buffer_.rd_ptr(), m_recv_buffer_.data_length());
                m_remain_len_ -= m_recv_buffer_.data_length();
                m_recv_buffer_.rd_ptr(m_recv_buffer_.data_length());
                m_recv_buffer_.recycle();

                continue;
            }

            // 已经接收到的分片数据包，并可以合并为完整的数据包
            m_net_event_->m_net_package_->data_o(m_recv_buffer_.rd_ptr(), m_remain_len_);

            m_recv_buffer_.rd_ptr(m_remain_len_);
            m_remain_len_ = 0;

            m_net_messenger_->push_event(m_net_event_);
            m_net_event_ = NULL;

            continue;
        }

        int real_pkglen = 0;

        // 循环拆包
        while (true)
        {
            real_pkglen = 0;
            int split_result = m_packet_splitter_->split(m_recv_buffer_.rd_ptr(), m_recv_buffer_.data_length(), real_pkglen);
            // 协议错，可能是非法数据 
            if (-1 == split_result)
            {
                LOG(ERROR)("sock_stream::handle_input, split error, split_result:%d", split_result);
                return -2;
            }
            // 包分片
            if (0 == split_result)
            {
                if (real_pkglen > m_recv_buffer_.length())
                {
                    m_net_event_ = net_event::m_pool_->Create();
                    if ( NULL == m_net_event_ )
                    {
                        LOG(ERROR)("assert: sock_stream::handle_input, new m_net_event_ is NULL");
                        return -1;
                    }

                    m_net_event_->m_net_ev_t_ = net_event::NE_DATA;
                    m_net_event_->m_net_id_ = m_net_id_;
                    m_net_event_->m_user_data_ = m_user_data_;
                    m_net_event_->m_remote_addr_ = m_remote_addr_;

                    m_net_event_->m_net_package_ = net_package::m_pool_->Create();
                    if ( NULL == m_net_event_->m_net_package_ )
                    {
                        LOG(ERROR)("assert: sock_stream::handle_input, new m_net_event_->m_net_package_ is NULL");
                        return -1;
                    }

                    m_net_event_->m_net_package_->allocate_data_block(new_allocator::Instance(), real_pkglen);

                    m_net_event_->m_net_package_->data_o(m_recv_buffer_.rd_ptr(), m_recv_buffer_.data_length());

                    m_recv_buffer_.rd_ptr(m_recv_buffer_.data_length());

                    m_remain_len_ = real_pkglen - m_recv_buffer_.data_length();
                }

                break;
            }

            // 包完整 if (1 == split_result)  // got a packet                                        
            net_event* netev = net_event::m_pool_->Create();
            if ( NULL == netev )
            {
                LOG(ERROR)("assert: sock_stream::handle_input, new netev is NULL");
                return -1;
            }

            netev->m_net_ev_t_ = net_event::NE_DATA;
            netev->m_net_id_ = m_net_id_;
            netev->m_user_data_ = m_user_data_;
            netev->m_remote_addr_ = m_remote_addr_;

            netev->m_net_package_ = net_package::m_pool_->Create();
            if ( NULL == m_net_event_->m_net_package_ )
            {
                LOG(ERROR)("assert: sock_stream::handle_input, new netev->m_net_package_ is NULL");
                return -1;
            }

            netev->m_net_package_->allocate_data_block(new_allocator::Instance(), real_pkglen);

            netev->m_net_package_->data_o(m_recv_buffer_.rd_ptr(), real_pkglen);

            m_recv_buffer_.rd_ptr(real_pkglen);

            LOG(TRACE)("rb_data_len:%d, rb_space_len:%d, remain_len:%d", m_recv_buffer_.data_length(), m_recv_buffer_.space_length(), m_remain_len_);

            m_net_messenger_->push_event(netev);
            m_net_event_ = NULL;

            // 缓冲区里的数据只有一个包 buf is empty, no packet in buf
            if (0 == m_recv_buffer_.data_length())
            {
                break;
            }
        }

        if ((m_recv_buffer_.data_length() == 0) || (m_recv_buffer_.space_length() == 0)
            || (m_recv_buffer_.data_length() + m_recv_buffer_.space_length()) < real_pkglen)
        {
            m_recv_buffer_.recycle();
        }
    }

    return 0;
}

int sock_stream::handle_output()
{
    STACK_TRACE_LOG();

    LOG(INFO)("sock_stream::handle_output, net<%u>, remote_addr<0x%08X:%d>", m_net_id_, m_remote_addr_.net_ip(), m_remote_addr_.net_port());
#ifdef WIN32
    if (NULL == m_send_netpkg_ && 0 == m_socket_send_packet_queue_->size())
    {
        m_net_messenger_->reactor_pointer()->disable_handler(this, ihandler::EM_WRITE);
    }
#endif

    int rc = 0;
    if (NULL != m_send_netpkg_)
    {
        rc = send_package();
        // send eagain
        if (-3 == rc)
        {
#ifdef WIN32
            m_net_messenger_->reactor_pointer()->enable_handler(this, ihandler::EM_WRITE);
#endif
            //LOG(WARN)("sock_stream::handle_output, send EAGAIN");
            return 0;
        }
        // 连接异常( rc = -2)
        // 连接关闭( rc = -1)
        else if ((-2 == rc) || (-1 == rc))
        {
            rc = m_reactor_->disable_handler(this, ihandler::EM_WRITE);
            // 设置reactor失败，认为socket异常
            if (0 != rc)
            {
                LOG(ERROR)("sock_stream::handle_output, disable_handler error");
                return -2;
            }

            return rc;
        }
    }

    while (true)
    {
        int rc = m_socket_send_packet_queue_->pop(m_send_netpkg_, false);
        // 发送队列已空
        if (0 != rc)
        {
            LOG(TRACE)("sock_stream::handle_output, socket send packet queue is empty");
            break;
        }

        rc = send_package();
        // send eagain
        if (-3 == rc)
        {
#ifdef WIN32
            m_net_messenger_->reactor_pointer()->enable_handler(this, ihandler::EM_WRITE);
#endif
            //LOG(WARN)("sock_stream::handle_output, send EAGAIN");
            return 0;
        }
        // 连接异常( rc = -2)
        // 连接关闭( rc = -1)
        else if ((-2 == rc) || (-1 == rc))
        {
            rc = m_reactor_->disable_handler(this, ihandler::EM_WRITE);
            // 设置reactor失败，认为socket异常
            if (0 != rc)
            {
                LOG(ERROR)("sock_stream::handle_output, disable_handler error");
                return -2;
            }

            return rc;
        }
    }

    rc = m_reactor_->disable_handler(this, ihandler::EM_WRITE);
    // 设置reactor失败,认为socket异常
    if (0 != rc)
    {
        LOG(ERROR)("sock_stream::handle_output, disable_handler error");
        return -2;
    }

    return 0;
}

int sock_stream::handle_close(int16_t evt)
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
            LOG(ERROR)("assert: sock_stream::handle_close error, new netev is NULL");
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

int sock_stream::post_package(net_package* netpkg)
{
    STACK_TRACE_LOG();

    LOG(INFO)("sock_stream::post_package, net<%u>, remote_addr<0x%08X:%d>", m_net_id_, m_remote_addr_.net_ip(), m_remote_addr_.net_port());
    if ((NULL == netpkg) || (0 == netpkg->length()))
    {
        LOG(TRACE)("sock_stream::post_package netpkg is NULL or netpkg length is 0");
        return 0;
    }

    // 当前没有待发任务, 直接发送
    if (NULL == m_send_netpkg_)
    {
        m_send_netpkg_ = netpkg;
    }
    // 网络忙, 入队列待发或当前队列里没有包
    else
    {
        int rc = m_socket_send_packet_queue_->push(netpkg);
        // 队列用尽
        if (0 != rc)
        {            
            LOG(ERROR)("sock_stream::post_package error, socket send packet queue is full, netpkg discard");
            netpkg->Destroy();
            return 0;
        }
    }

    int rc = send_package();
    // 连接关闭
    if (-1 == rc)
    {
        m_reactor_->disable_handler(this, ihandler::EM_ALL);
        handle_close(net_event::NE_CLOSE);
        return -1;
    }
    // 连接异常
    else if (-2 == rc)
    {
        m_reactor_->disable_handler(this, ihandler::EM_ALL);
        handle_close(net_event::NE_EXCEPTION);
        return -2;
    }
    // write eagain
    else if (-3 == rc)
    {
        //LOG(WARN)("sock_stream::post_package, send EAGAIN");
        rc = m_reactor_->enable_handler(this, ihandler::EM_WRITE);
        // 设置reactor失败,认为socket异常
        if (0 != rc)
        {
            LOG(ERROR)("sock_stream::post_package error, enable_handler error");
            handle_close(net_event::NE_EXCEPTION);
            return -2;
        }
    }

    return 0;
}

int sock_stream::send_package()
{
    STACK_TRACE_LOG();

    LOG(INFO)("sock_stream::send_package, net<%u>, remote_addr<0x%08X:%d>", m_net_id_, m_remote_addr_.net_ip(), m_remote_addr_.net_port());
    if ((NULL == m_send_netpkg_) || (0 == m_send_netpkg_->length()))
    {
        LOG(TRACE)("sock_stream::send_package m_send_netpkg_ is NULL or m_send_netpkg_ length is 0");
        return 0;
    }

    int remain_len = m_send_netpkg_->length() - m_send_netpkglen_;
    char* current = m_send_netpkg_->data_o();
    int32_t send_times = 0;
    int rc = 0;
    while (remain_len > 0)
    {
        rc = m_socket_.send(current + m_send_netpkglen_, remain_len, 0);
        // 发送成功
        if (rc > 0)
        {
            m_send_netpkglen_ += rc;
            remain_len -= rc;
        }
        // send warn
        else if (0 > rc)
        {
            if (SYS_EAGAIN == error_no()) //数据未写完，需要等待后续写入
            {
                //#ifdef WIN32
                //                    m_net_messenger_->m_reactor_.enable_handler(this, event_handler::EM_WRITE);
                //#endif
                LOG(WARN)("sock_stream::send_package, send EAGAIN");
                return -3;
            }

            LOG(ERROR)("sock_stream::send_package error, send error, errno:%d", error_no());
            m_send_netpkg_->Destroy();
            m_send_netpkg_ = NULL;
            m_send_netpkglen_ = 0;
            return -2;
        }
        // close  不能关闭连接
        else if (0 == rc)
        {
            LOG(WARN)("sock_stream::send_package, close by peer");
            m_send_netpkg_->Destroy();
            m_send_netpkg_ = NULL;
            m_send_netpkglen_ = 0;
            return -1;  
        }

        ++send_times;
    }

    if (send_times > 1 && remain_len == 0)    //全部传输完，并且传输次数超过两次
    {
        LOG(WARN)("sock_stream::send_package, net_id:%d, send data bytes(%d), need %d times.", m_net_id_, remain_len, send_times);
    }

    m_send_netpkg_->Destroy();
    m_send_netpkg_ = NULL;
    m_send_netpkglen_ = 0;

    return 0;
}
