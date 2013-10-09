//! @file sock_acceptor.h
#ifndef _SOCK_ACCEPTOR_H_
#define _SOCK_ACCEPTOR_H_

#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_object_holder.h"

#include "ihandler.h"
#include "socket.h"
#include "address.h"

class ireactor;
class ipacket_splitter;
class net_messenger;
class net_package;

//! @class sock_acceptor
//! @brief tcp����������
class sock_acceptor : public ihandler, public non_copyable, public destroyable
{
public:
    //! ���캯��
    //! @param net_messenger ���������
    //! @param packet_splitter �����
    sock_acceptor();
    //! ��������
    virtual ~sock_acceptor();

public:
    int init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id);

public:
    //! ����tcp������
    //! @param local_addr tcp���ؼ�����ַ
    //! @return ���, 0:�ɹ�, -1ʧ��
    virtual int open(const Address& local_addr, int recv_bufsize, int send_bufsize);

    //! �ر�ͨ��
    virtual void close();

public:
    //! ��ȡ�����ʾ��ͨ��id��
    virtual int32_t handler_o();
    //! ���������ʾ��ͨ��id��
    virtual void handler_o(int32_t net_id);

    //! ��ȡ socket Descriptor ������
    virtual Descriptor descriptor_o();
    //! ���� socket Descriptor ������
    virtual void descriptor_o(Descriptor sock);
    //! ��ȡ event mask �¼���ע��Ϣ
    virtual int32_t event_mask_o();
    //! ��ȡ event mask �¼���ע��Ϣ
    virtual void event_mask_o(int32_t ev_mask);
    //! ��ȡ ��Ӧ��
    virtual ireactor* reactor_o();
    //! ���� ��Ӧ��
    virtual void reactor_o(ireactor* react);

    //! �����
    //! @return ������ 0:����������-1: ���ӱ��رգ�-2:�����쳣
    virtual int handle_input();
    //! ����д
    //! @return ������ 0:����������-1: ���ӱ��رգ�-2:�����쳣
    virtual int handle_output();
    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(int16_t evt);

    //! �ύ��������
    //! @param post_packet �����͵�����
    virtual int post_package(net_package* netpkg);

private:
    //! event mask �¼���ע��Ϣ
    int16_t m_ev_mask_;
    //! ����ͨ��id
    int32_t m_net_id_;

    //! ��Ӧ��
    ireactor* m_reactor_;
    //! �û��Զ������� �����ӵ��¼�������ָ��
    void* m_user_data_;

    //! socket��� Descriptor(m_socket_);
    Socket m_socket_;

    //! ���������
    net_messenger* m_net_messenger_;
    //! �����
    ipacket_splitter* m_packet_splitter_;
    //! ���ص�ַ
    Address m_local_addr_;

public:
    //! accept�׽��ֳ�10��
    static object_holder<sock_acceptor>* m_pool_;
};

#endif // _SOCK_ACCEPTOR_H_
