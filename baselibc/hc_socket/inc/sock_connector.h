//! @file sock_connector.h
#ifndef _SOCK_CONNECTOR_H_
#define _SOCK_CONNECTOR_H_

#include "event_handler.h"
#include "packet_splitter.h"
#include "net_manager.h"


//! @class sock_connector
//! @brief tcp���Ӵ�����
class sock_connector : public event_handler, public non_copyable, public destroyable
{
public:
    //! ���캯��
    //! @param net_manager ���������
    //! @param packet_splitter �����
    sock_connector();
    //! ��������
    virtual ~sock_connector();

public:
    int init(net_manager* nm, packet_splitter* ps, void* user_data,
             uint32_t listen_net_id, uint32_t net_id);

public:
    //! ����tcp������ ���ӷ�����(�첽����)
    //! @param remote_addr ����˵�ַ
    //! @param timeout ���ӳ�ʱ
    //! @return ���, 0:�ɹ�, -1ʧ��
    int create_tcp_client(const Address& remote_addr, int timeout, int netbufsize);

    //! �ر�ͨ��
    void close_tcp_client();

    //! �����
    //! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
    virtual int handle_input();

    //! ����д
    //! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
    virtual int handle_output();

    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t evt);

    //! �ύ��������
    //! @param post_packet �����͵�����
    virtual int post_package(net_package* send_packet);

private:
    //! ���������
    net_manager* m_net_manager_;
    //! �����
    packet_splitter* m_packet_splitter_;
    //! �Է�Զ�̵�ַ
    Address m_remote_addr_;

public:
    //! connector�׽���
    static object_guard<sock_connector>* m_pool_;
};

#endif // _SOCK_CONNECTOR_H_
