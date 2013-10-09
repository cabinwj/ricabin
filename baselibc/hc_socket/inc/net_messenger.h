//! @file net_messenger.h
#ifndef _NET_MESSENGER_H_
#define _NET_MESSENGER_H_

#include "hc_types.h"
#include "hc_sync_queue.h"

#include "address.h"
#include "net_package.h"
#include "net_event.h"
#include "net_id_holder.h"


#ifdef WIN32    // for windows
  #include "select_reactor.h"
  //! windowsƽ̨��ʹ��select
  typedef select_reactor net_reactor;
#define NETBUF_SIZE 8192
#else            // for linux
  //! linuxƽ̨��ʹ��epoll
  #include "epoll_reactor.h"
  typedef epoll_reactor net_reactor;
#define NETBUF_SIZE 16384
#endif

class ipacket_splitter;
class sock_connector;
class sock_acceptor;
class sock_stream;
class net_package;
class net_thread;
class net_event;

//! @class net_messenger
//! @brief ���������
class net_messenger
{
public:
    net_messenger();
    ~net_messenger();

public:
    static net_messenger* Instance();

public:
    //! ��������������߳�
    //! @return 0:�ɹ�, -1ʧ��
    int start();

    //! ֹͣ����������߳�
    //! @return 0:�ɹ�, -1ʧ��
    int stop();

    //! ����һ��tcp clientͨ��
    //! @param remote_addr tcp�Զ˵�ַ
    //! @param packet_splitter tcp�����
    //! @param user_data �û���������ݣ���netevent�з��ظ��û���һ�����������ӵ��¼�������ָ��
    //! @param netbufsize ����ײ㻺������С
    //! @return ͨ��id, >0:�ɹ�, 0ʧ��
    int32_t create_tcp_client(const Address& remote_addr, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! ����һ��tcp clientͨ��
    //! @param remote_ip �Զ�ip
    //! @param remote_port �Զ�port
    //! @param packet_splitter tcp�����
    //! @param user_data �û���������ݣ���netevent�з��ظ��û���һ�����������ӵ��¼�������ָ��
    //! @param netbufsize ����ײ㻺������С
    //! @return ͨ��id, >0:�ɹ�, 0ʧ��
    int32_t create_tcp_client(const char *remote_ip, int remote_port, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! ����һ��tcp serverͨ��
    //! @param local_addr tcp���ؼ�����ַ
    //! @param packet_splitter tcp�����
    //! @param user_data �û���������ݣ���netevent�з��ظ��û���һ�����������ӵ��¼�������ָ��
    //! @param netbufsize ����ײ㻺������С
    //! @return ͨ��id, >0:�ɹ�, 0ʧ��
    int32_t create_tcp_server(const Address& local_addr, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! ����һ��tcp serverͨ��
    //! @param local_ip tcp���ؼ���ip
    //! @param local_port tcp���ؼ���port
    //! @param packet_splitter tcp�����
    //! @param user_data �û���������ݣ���netevent�з��ظ��û���һ�����������ӵ��¼�������ָ��
    //! @param netbufsize ����ײ㻺������С
    //! @return ͨ��id, >0:�ɹ�, 0ʧ��
    int32_t create_tcp_server(const char* local_ip, int local_port, ipacket_splitter* ps, void* user_data,
                              int recv_bufsize = NETBUF_SIZE, int send_bufsize = NETBUF_SIZE);

    //! ֪ͨɾ������ͨ��
    //! @param id ͨ��id
    //! @param evt net_event_type
    //! @return 0:�ɹ� <0:ʧ��
    int notify_close(int32_t net_id, int16_t evt = net_event::NE_NOTIFY_CLOSE);

    //! �������ݰ�
    //! @warning ����˵��óɹ�, packet����������������ͷ�; ����˵���ʧ��, packet���ɵ��÷������ͷ�
    //! @param id ͨ��id
    //! @param packet ���ݰ�
    //! @return ���ͽ��, 0:�ɹ�, -1:ʧ��(ͨ��������), -2:ʧ��(��������)
    int send_package(int32_t net_id, net_package* netpkg);

    //! ȡ�����¼�
    //! @warning �ͻ���Ӧѭ�����ô˺�����ȡ�����¼�, ���ڴ������ÿ�������¼������ͷ�
    //! @return �����¼�, û�������¼�ʱ����NULL
    net_event* pop_event();

    //! ��ȡδ����ķ����¼�������
    //! @return �����¼�������
    int net_send_package_count();

    //! ��ȡδ����Ľ����¼�������
    //! @return �����¼�������
    int net_event_count();

    //! ����net_id
    //! @return net_id
    int32_t acquire_net_id(net_id_holder::net_id_t type);

    //! �ͷ�net_id
    //! @param net_id
    void release_net_id(int32_t net_id);

    //! ��ȡ��Ӧ��
    //! @return ��Ӧ��ָ��
    net_reactor* reactor_pointer();

    //! ����һ�������¼�
    //! @param net_event �����¼�
    int push_event(net_event* netev);

    //! ȡ��������
    //! @return ��������, û�з�������ʱ����NULL
    net_package* pop_net_send_package();

    //! �������(reactor����)
    void reactor_exception();

public:
    friend class net_thread;

private:
    //! ����״̬ 0:ֹͣ, 1:����, 2:�쳣
    int m_status_;
    //! ����������߳�
    net_thread* m_net_thread_;
    //! ͨ��id������
    net_id_holder m_net_id_holder_;
    //! ��Ӧ��
    net_reactor m_reactor_;

    //! �����¼����У���������Ϊ100��
    net_event_queue* m_net_event_queue_;
    //! ������������У���������Ϊ100��
    net_package_queue* m_net_send_packet_queue_;
};

#endif // _NET_MESSENGER_H_
