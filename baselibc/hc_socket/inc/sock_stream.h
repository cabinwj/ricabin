//! @file sock_stream.h
#ifndef _SOCK_STREAM_H_
#define _SOCK_STREAM_H_

#include "hc_block_buffer.h"
#include "hc_non_copyable.h"
#include "hc_destroyable.h"

#include "ihandler.h"
#include "socket.h"
#include "address.h"
#include "net_config.h"
#include "net_package.h"

class ireactor;
class ipacket_splitter;
class net_messenger;
class net_package;
class net_event;

//! @class sock_stream
//! @brief tcpͨ��������
class sock_stream : public ihandler, public non_copyable, public destroyable
{
public:
    //! ���캯��
    //! @param listen_net_id ����ͨ��id
    //! @param net_id ��Ϣͨ��id
    //! @param net_messenger ���������
    //! @param packet_splitter �����
    //! @param socket �׽���
    sock_stream();
    virtual ~sock_stream();

public:
    int init(net_messenger* nm, ipacket_splitter* ps, void* user_data, int32_t net_id, const Address& remote_addr, Descriptor socket);

public:
    virtual int open(const Address& local_addr, int recv_bufsize, int send_bufsize);
    //! �ر�����
    virtual void close();

public:
    //! ��ȡ�����ʾ��ͨ��id��
    virtual int32_t handler_o();
    //! ���������ʾ��ͨ��id��
    virtual void handler_o(int32_t net_id);

    //! ��ȡ socket Descriptor ������
    virtual Descriptor descriptor_o();
    //! ���� socket Descriptor ������
    virtual void descriptor_o(Descriptor sock) ;
    //! ��ȡ event mask �¼���ע��Ϣ
    virtual int32_t event_mask_o();
    //! ��ȡ event mask �¼���ע��Ϣ
    virtual void event_mask_o(int32_t ev_mask);
    //! ��ȡ ��Ӧ��
    virtual ireactor* reactor_o();
    //! ���� ��Ӧ��
    virtual void reactor_o(ireactor* react);

    //! ��
    //! @return ������ 0:��������, -1: ���ӱ��Է��ر�, -2:�����쳣
    virtual int handle_input();
    //! д
    //! @return ������ 0:��������, -1: �����쳣
    virtual int handle_output();
    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(int16_t evt);

    //! �ύ��������
    virtual int post_package(net_package* netpkg);

public:
    friend class sock_acceptor;
    friend class sock_connector;

private:
    int send_package();

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
    static object_holder<sock_stream>* m_pool_;
};

#endif // _SOCK_STREAM_H_
