#include "sock_stream.h"
#include "net_id_guard.h"

#include "common_log.h"


// class sock_stream
//! stream�׽��ֳ�70��
object_guard<sock_stream>* sock_stream::m_pool_ = new \
    object_guard<sock_stream>( \
    new object_pool_allocator<sock_stream>(SOCK_STREAM_POOL_COUNT, \
                                           (net_id_guard::TCP_STREAM_END - net_id_guard::TCP_STREAM_BEGIN + 1), \
                                           SOCK_STREAM_POOL_COUNT, \
                                           new_allocator::Instance()));

sock_stream::sock_stream() : m_net_manager_(NULL), m_packet_splitter_(NULL)
{
    m_listen_net_id_ = 0;
    m_net_id_ = 0;
    m_user_data_ = NULL;

    m_send_netpkg_ = NULL;
    m_send_netpkglen_ = 0;

    m_net_event_ = NULL;
    m_remain_len_ = 0;

    m_socket_send_packet_queue_ = NULL;
}

int sock_stream::init(uint32_t listen_net_id, uint32_t net_id,
                      net_manager* nm, packet_splitter* ps, const Address& remote_addr,
                      Descriptor socket, void* user_data)
{
    m_net_manager_ = (NULL == nm) ? net_manager::Instance() : nm;
    m_packet_splitter_ = (NULL == ps) ? packet_splitter::Instance() : ps;

    m_remote_addr_ = remote_addr;

    m_listen_net_id_ = listen_net_id;
    m_net_id_ = net_id;

    m_socket_ = socket;

    m_user_data_ = user_data;
    
    m_send_netpkg_ = NULL;
    m_send_netpkglen_ = 0;

    m_net_event_ = NULL;
    m_remain_len_ = 0;

    //! ͨ���������������
    //! �˶����̰߳�ȫ
    //! ��������Ϊ1��
    m_socket_send_packet_queue_ = new net_package_queue(MAX_SOCKET_SEND_TASK_COUNT);

    return 0;
}

sock_stream::~sock_stream()
{
    if (0 != m_net_id_)
    {
        m_net_manager_->release_net_id(m_net_id_);
    }

    //���δ��������ݰ�
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

void sock_stream::close_stream()
{
    m_socket_.close();
}

int sock_stream::handle_input()
{
    LOG(INFO)("sock_stream::handle_input, net<%u:%u>, remote_addr<0x%08X:%d>", m_listen_net_id_, m_net_id_, m_remote_addr_.get_net_ip(), m_remote_addr_.get_net_port());
    // TODO: ���˴��Ĵ����߼����Ƿ��������ڵ������Ӷ�������������
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

        // �������� if (0 < rc)
        m_recv_buffer_.wr_ptr(rc);

        LOG(TRACE)("rb_data_len:%d, rb_space_len:%d, remain_len:%d, rc:%d", m_recv_buffer_.data_length(), m_recv_buffer_.space_length(), m_remain_len_, rc);

        // �����ݰ��Ѿ���Ƭ���չ����ݵķ�Ƭ�������ݲ�����
        if (NULL != m_net_event_)
        {
            // �Ѿ����յ���Ƭ���ݰ���������ƴ�ӳ����������ݰ���������ȴ�
            if (m_recv_buffer_.data_length() < m_remain_len_)
            {
                m_net_event_->m_net_package_->set_data(m_recv_buffer_.rd_ptr(), m_recv_buffer_.data_length());
                m_remain_len_ -= m_recv_buffer_.data_length();
                m_recv_buffer_.rd_ptr(m_recv_buffer_.data_length());
                m_recv_buffer_.recycle();

                continue;
            }

            // �Ѿ����յ��ķ�Ƭ���ݰ��������Ժϲ�Ϊ���������ݰ�
            m_net_event_->m_net_package_->set_data(m_recv_buffer_.rd_ptr(), m_remain_len_);

            m_recv_buffer_.rd_ptr(m_remain_len_);
            m_remain_len_ = 0;

            m_net_manager_->push_event(m_net_event_);
            m_net_event_ = NULL;

            continue;
        }

        int real_pkglen = 0;

        // ѭ�����
        while (true)
        {
            real_pkglen = 0;
            int split_result = m_packet_splitter_->split(m_recv_buffer_.rd_ptr(), m_recv_buffer_.data_length(), real_pkglen);
            // Э��������ǷǷ����� 
            if (-1 == split_result)
            {
                LOG(ERROR)("sock_stream::handle_input, split error, split_result:%d", split_result);
                return -2;
            }
            // ����Ƭ
            if (0 == split_result)
            {
                if (real_pkglen > m_recv_buffer_.length())
                {
                    m_net_event_ = event_handler::m_net_ev_pool_->Create();
                    if ( NULL == m_net_event_ )
                    {
                        LOG(ERROR)("assert: sock_stream::handle_input, new m_net_event_ is NULL");
                        return -1;
                    }

                    m_net_event_->m_net_ev_t_ = net_event::NE_DATA;
                    m_net_event_->m_listen_net_id_ = m_listen_net_id_;
                    m_net_event_->m_net_id_ = m_net_id_;
                    m_net_event_->m_user_data_ = m_user_data_;
                    m_net_event_->m_remote_addr_ = m_remote_addr_;

                    m_net_event_->m_net_package_ = event_handler::m_net_pkg_pool_->Create();
                    if ( NULL == m_net_event_->m_net_package_ )
                    {
                        LOG(ERROR)("assert: sock_stream::handle_input, new m_net_event_->m_net_package_ is NULL");
                        return -1;
                    }

                    m_net_event_->m_net_package_->allocator_data_block(new_allocator::Instance(), real_pkglen);

                    m_net_event_->m_net_package_->set_data(m_recv_buffer_.rd_ptr(), m_recv_buffer_.data_length());

                    m_recv_buffer_.rd_ptr(m_recv_buffer_.data_length());

                    m_remain_len_ = real_pkglen - m_recv_buffer_.data_length();
                }

                break;
            }

            // ������ if (1 == split_result)  // got a packet                                        
            net_event* netev = event_handler::m_net_ev_pool_->Create();
            if ( NULL == netev )
            {
                LOG(ERROR)("assert: sock_stream::handle_input, new netev is NULL");
                return -1;
            }

            netev->m_net_ev_t_ = net_event::NE_DATA;
            netev->m_listen_net_id_ = m_listen_net_id_;
            netev->m_net_id_ = m_net_id_;
            netev->m_user_data_ = m_user_data_;
            netev->m_remote_addr_ = m_remote_addr_;

            netev->m_net_package_ = event_handler::m_net_pkg_pool_->Create();
            if ( NULL == m_net_event_->m_net_package_ )
            {
                LOG(ERROR)("assert: sock_stream::handle_input, new netev->m_net_package_ is NULL");
                return -1;
            }

            netev->m_net_package_->allocator_data_block(new_allocator::Instance(), real_pkglen);

            netev->m_net_package_->set_data(m_recv_buffer_.rd_ptr(), real_pkglen);

            m_recv_buffer_.rd_ptr(real_pkglen);

            LOG(TRACE)("rb_data_len:%d, rb_space_len:%d, remain_len:%d", m_recv_buffer_.data_length(), m_recv_buffer_.space_length(), m_remain_len_);

            m_net_manager_->push_event(netev);
            m_net_event_ = NULL;

            // �������������ֻ��һ���� buf is empty, no packet in buf
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
    LOG(INFO)("sock_stream::handle_output, net<%u:%u>, remote_addr<0x%08X:%d>", m_listen_net_id_, m_net_id_, m_remote_addr_.get_net_ip(), m_remote_addr_.get_net_port());
#ifdef WIN32
    if (NULL == m_send_netpkg_ && 0 == m_socket_send_packet_queue_->size())
    {
        m_net_manager_->reactor_pointer()->disable_handler(this, event_handler::EM_WRITE);
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
            m_net_manager_->reactor_pointer()->enable_handler(this, event_handler::EM_WRITE);
#endif
            //LOG(WARN)("sock_stream::handle_output, send EAGAIN");
            return 0;
        }
        // �����쳣( rc = -2)
        // ���ӹر�( rc = -1)
        else if ((-2 == rc) || (-1 == rc))
        {
            rc = m_reactor_->disable_handler(this, event_handler::EM_WRITE);
            // ����reactorʧ�ܣ���Ϊsocket�쳣
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
        // ���Ͷ����ѿ�
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
            m_net_manager_->reactor_pointer()->enable_handler(this, event_handler::EM_WRITE);
#endif
            //LOG(WARN)("sock_stream::handle_output, send EAGAIN");
            return 0;
        }
        // �����쳣( rc = -2)
        // ���ӹر�( rc = -1)
        else if ((-2 == rc) || (-1 == rc))
        {
            rc = m_reactor_->disable_handler(this, event_handler::EM_WRITE);
            // ����reactorʧ�ܣ���Ϊsocket�쳣
            if (0 != rc)
            {
                LOG(ERROR)("sock_stream::handle_output, disable_handler error");
                return -2;
            }

            return rc;
        }
    }

    rc = m_reactor_->disable_handler(this, event_handler::EM_WRITE);
    // ����reactorʧ��,��Ϊsocket�쳣
    if (0 != rc)
    {
        LOG(ERROR)("sock_stream::handle_output, disable_handler error");
        return -2;
    }

    return 0;
}

int sock_stream::handle_close(net_event::net_ev_t evt)
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
            LOG(ERROR)("assert: sock_stream::handle_close error, new netev is NULL");
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

int sock_stream::post_package(net_package* netpkg)
{
    LOG(INFO)("sock_stream::post_package, net<%u:%u>, remote_addr<0x%08X:%d>", m_listen_net_id_, m_net_id_, m_remote_addr_.get_net_ip(), m_remote_addr_.get_net_port());
    if ((NULL == netpkg) || (0 == netpkg->length()))
    {
        LOG(TRACE)("sock_stream::post_package netpkg is NULL or netpkg length is 0");
        return 0;
    }

    // ��ǰû�д�������, ֱ�ӷ���
    if (NULL == m_send_netpkg_)
    {
        m_send_netpkg_ = netpkg;
    }
    // ����æ, ����д�����ǰ������û�а�
    else
    {
        int rc = m_socket_send_packet_queue_->push(netpkg);
        // �����þ�
        if (0 != rc)
        {            
            LOG(ERROR)("sock_stream::post_package error, socket send packet queue is full, netpkg discard");
            netpkg->Destroy();
            return 0;
        }
    }

    int rc = send_package();
    // ���ӹر�
    if (-1 == rc)
    {
        m_reactor_->disable_handler(this, event_handler::EM_ALL);
        handle_close(net_event::NE_CLOSE);
        return -1;
    }
    // �����쳣
    else if (-2 == rc)
    {
        m_reactor_->disable_handler(this, event_handler::EM_ALL);
        handle_close(net_event::NE_EXCEPTION);
        return -2;
    }
    // write eagain
    else if (-3 == rc)
    {
        //LOG(WARN)("sock_stream::post_package, send EAGAIN");
        rc = m_reactor_->enable_handler(this, event_handler::EM_WRITE);
        // ����reactorʧ��,��Ϊsocket�쳣
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
    LOG(INFO)("sock_stream::send_package, net<%u:%u>, remote_addr<0x%08X:%d>", m_listen_net_id_, m_net_id_, m_remote_addr_.get_net_ip(), m_remote_addr_.get_net_port());
    if ((NULL == m_send_netpkg_) || (0 == m_send_netpkg_->length()))
    {
        LOG(TRACE)("sock_stream::send_package m_send_netpkg_ is NULL or m_send_netpkg_ length is 0");
        return 0;
    }

    int remain_len = m_send_netpkg_->length() - m_send_netpkglen_;
    char* current = m_send_netpkg_->get_data();
    int32_t send_times = 0;
    int rc = 0;
    while (remain_len > 0)
    {
        rc = m_socket_.send(current + m_send_netpkglen_, remain_len, 0);
        // ���ͳɹ�
        if (rc > 0)
        {
            m_send_netpkglen_ += rc;
            remain_len -= rc;
        }
        // send warn
        else if (0 > rc)
        {
            if (SYS_EAGAIN == error_no()) //����δд�꣬��Ҫ�ȴ�����д��
            {
                //#ifdef WIN32
                //                    m_net_manager_->m_reactor_.enable_handler(this, event_handler::EM_WRITE);
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
        // close  ���ܹر�����
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

    if (send_times > 1 && remain_len == 0)    //ȫ�������꣬���Ҵ��������������
    {
        LOG(WARN)("sock_stream::send_package, net_id:%d, send data bytes(%d), need %d times.", m_net_id_, remain_len, send_times);
    }

    m_send_netpkg_->Destroy();
    m_send_netpkg_ = NULL;
    m_send_netpkglen_ = 0;

    return 0;
}
