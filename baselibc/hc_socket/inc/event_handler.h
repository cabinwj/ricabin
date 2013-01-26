//! @file event_handler.h
#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include "hc_types.h"
#include "hc_list.h"
#include "hc_thread_mutex.h"

#include "socket.h"
#include "reactor.h"
#include "net_event.h"
#include "net_package.h"


//! @class event_handler
//! @brief �¼�������
class event_handler
{
public:
    //! @enum event_mask_t
    //! @brief �¼�����
    typedef enum event_mask_type {
        EM_NULL    = 0,
        EM_READ    = 1 << 0,    //!< �ɶ��¼�
        EM_WRITE   = 1 << 1,    //!< ��д�¼�

        EM_ALL     = EM_READ | EM_WRITE,
    } ev_mask_t;

    enum {
        HANDLER_TABLE_SIZE = 100003
    };

public:
    //! list hash node
    list_head m_hash_item_;
    //! list timeout node
    list_head m_timeout_item_;
    //! list notify close node
    list_head m_notify_close_item_;

    //! event mask �¼���ע��Ϣ
    uint32_t m_ev_mask_;

    //! ����ͨ��id, ������Ǽ���ͨ�����˴�Ϊ0
    uint32_t m_listen_net_id_;
    //! ����ͨ��id, ���û����ͨ��������ͨ���˴�Ϊ0
    uint32_t m_net_id_;
    //! ��ʱ��ʱ��, 0��ʾû�����ó�ʱ
    time_t m_timeout_;


    //! ��Ӧ��
    reactor* m_reactor_;
    //! �û��Զ������� �����ӵ��¼�������ָ��
    void* m_user_data_;

    //! socket��� Descriptor(m_socket_);
    Socket m_socket_;

public:
    event_handler();
    virtual ~event_handler();

public:
    //! �����
    //! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
    virtual int handle_input() = 0;
    //! ����д
    //! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
    virtual int handle_output() = 0;
    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(net_event::net_ev_t event_type) = 0;
    //! �ύ��������
    //! @param netpkg �����͵�����
    virtual int post_package(net_package* netpkg) = 0;

public:
    //! ���ó�ʱ
    //! @param timeout ��ʱʱ��(��)
    static void set_timeout(event_handler* eh, time_t timeout);
    //! ��ʱ������ⳬʱ����
    static void on_timer(time_t now);
    //! ֪ͨ�ر����Ӵ���
    static void on_notify_close();

public:
    //! ��ǰ���¼���
    static int m_current_count_;
    //! hash list head table
    static list_head m_hash_bucket_[HANDLER_TABLE_SIZE];
    //! timeout list head
    static list_head m_timeout_list_head_;
    //! notify close list head
    static list_head m_notify_close_list_head_;

    //! �߳���, ���������� handler_list
    static threadc_mutex m_mutex_;

public:
    static void init_hash_table();
    static void clear_hash_table();
    static event_handler* get_handler(uint32_t tunnel_id);
    static void push_handler(event_handler* eh, uint32_t tunnel_id);
    static void remove_handler(event_handler* eh);
    static void remove_handler(uint32_t tunnel_id);

public:
    static void init_timeout_list();
    static void clear_timeout_list();
    static event_handler* get_handler_from_timeout_list(uint32_t tunnel_id);
    static void push_handler_to_timeout_list(event_handler* eh);
    static void remove_handler_from_timeout_list(event_handler* eh);
    static void remove_handler_from_timeout_list(uint32_t tunnel_id);

public:
    static void init_notify_close_list();
    static void clear_notify_close_list();
    static event_handler* get_handler_from_notify_close_list(uint32_t tunnel_id);
    static void push_handler_to_notify_close_list(event_handler* eh);
    static void remove_handler_from_notify_close_list(event_handler* eh);
    static void remove_handler_from_notify_close_list(uint32_t tunnel_id);

public:
    //! net_event ����������㱻���䲢��ʼ��, ��Ӧ�ò㱻ʹ�ò�����, Ϊ���Ч��ʹ��Ԥ��������
    static net_event_pool* m_net_ev_pool_;
    //! net_package ����������㱻���䲢��ʼ��, ��Ӧ�ò㱻ʹ�ò�����, Ϊ���Ч��ʹ��Ԥ��������
    static net_package_pool* m_net_pkg_pool_;
};

#endif // _EVENT_HANDLER_H_
