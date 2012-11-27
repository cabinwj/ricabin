//! @file sock_stream.h
#ifndef _SOCK_STREAM_H_
#define _SOCK_STREAM_H_

#include "common_block_buffer.h"

#include "event_handler.h"
#include "packet_splitter.h"
#include "net_manager.h"

#include "config.h"


//! @class sock_stream
//! @brief tcpͨ��������
class sock_stream : public event_handler, public non_copyable, public destroyable
{
public:
    //! ���캯��
    //! @param listen_net_id ����ͨ��id
    //! @param net_id ��Ϣͨ��id
    //! @param net_manager ���������
    //! @param packet_splitter �����
    //! @param socket �׽���
    sock_stream();
    virtual ~sock_stream();

public:
    int init(uint32_t listen_net_id, uint32_t net_id, net_manager* nm,
             packet_splitter* ps, const Address& remote_addr, Descriptor socket, void* user_data);

public:
    //! �ر�����
    void close_stream();

    //! ��
    //! @return ������ 0:��������, -1: ���ӱ��Է��ر�, -2:�����쳣
    virtual int handle_input();

    //! д
    //! @return ������ 0:��������, -1: �����쳣
    virtual int handle_output();

    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t evt);

    //! �ύ��������
    virtual int post_package(net_package* netpkg);

public:
    friend class sock_acceptor;
    friend class sock_connector;

private:
    int send_package();

private:
    //! ���������
    net_manager* m_net_manager_;
    //! �����
    packet_splitter* m_packet_splitter_;
    //! �Է�Զ�̵�ַ
    Address m_remote_addr_;
    //! ���ܻ���
    block_buffer<RECV_BUFFER_LENGTH> m_recv_buffer_;
    //! ͨ���������������
    //! �˶����̰߳�ȫ
    //! ��������Ϊ1��
    net_package_queue* m_socket_send_packet_queue_;
    //! ��ǰ���ڷ��͵�����
    net_package* m_send_netpkg_;
    //! ��ǰ���ڷ��͵������Ѿ����͵��ֽ���
    int m_send_netpkglen_;
    //! ��ǰ���ݰ�����
    net_event* m_net_event_;
    //! ��ǰ���ݰ�ʣ�¶��ٳ���δ����
    int m_remain_len_;

public:
    //! stream�׽��ֳ�70��, Ϊ���Ч��ʹ��Ԥ��������
    static object_guard<sock_stream>* m_pool_;
};

#endif // _SOCK_STREAM_H_
