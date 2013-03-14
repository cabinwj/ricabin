//! @file sock_acceptor.h
#ifndef _SOCK_ACCEPTOR_H_
#define _SOCK_ACCEPTOR_H_

#include "event_handler.h"
#include "packet_splitter.h"
#include "net_manager.h"

//! @class sock_acceptor
//! @brief tcp����������
class sock_acceptor : public event_handler
{
public:
    //! ���캯��
    //! @param net_manager ���������
    //! @param packet_splitter �����
    sock_acceptor();
    //! ��������
    virtual ~sock_acceptor();

public:
    int init(net_manager* nm, packet_splitter* ps, void* user_data,
             uint32_t listen_net_id, uint32_t net_id);

public:
    //! ����tcp������
    //! @param local_addr tcp���ؼ�����ַ
    //! @return ���, 0:�ɹ�, -1ʧ��
    int create_tcp_server(const Address& local_addr, int netbufsize);

    //! �ر�ͨ��
    void close_tcp_server();

    //! �����
    //! @return ������ 0:����������-1: ���ӱ��رգ�-2:�����쳣
    virtual int handle_input();

    //! ����д
    //! @return ������ 0:����������-1: ���ӱ��رգ�-2:�����쳣
    virtual int handle_output();

    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t evt);

    //! �ύ��������
    //! @param post_packet �����͵�����
    virtual int post_package(net_package* netpkg);

private:
    //! ���������
    net_manager* m_net_manager_;
    //! �����
    packet_splitter* m_packet_splitter_;

public:
    //! accept�׽��ֳ�10��
    static object_guard<sock_acceptor>* m_pool_;
};

#endif // _SOCK_ACCEPTOR_H_
