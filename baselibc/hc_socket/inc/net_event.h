#ifndef _NET_EVENT_H_
#define _NET_EVENT_H_

#include "hc_types.h"
#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_object_guard.h"
#include "hc_stack_trace.h"

#include "net_package.h"
#include "address.h"


//! @class net_event
//! @brief �����¼���
class net_event : public non_copyable, public destroyable
{
public:
    //! @enum net_event_type
    //! @brief �����¼�����
    typedef enum net_event_type {
        NE_NULL,                 //!< ���¼�

        // �ײ��¼����׸��ϲ�
        NE_CONNECTED,            //!< tcp���ӳɹ�
        NE_ACCEPT,               //!< tcp��������
        NE_DATA,                 //!< ���ݵ���

        NE_CLOSE,                //!< tcp���ӹر�
        NE_EXCEPTION,            //!< �����쳣

        NE_TIMEOUT,              //!< tcp���ӳ�ʱ

        // Ӧ�ò�֪ͨ�ײ�رգ��ײ�رպ����׸��ϲ�
        NE_NOTIFY_CLOSE,         //!< tcp֪ͨ�ر�
    } net_ev_t;

public:
    net_event() : m_net_ev_t_(net_event::NE_NULL),
                  m_listen_net_id_(0), m_net_id_(0),
                  m_net_package_(NULL), m_user_data_(NULL)
    {
        STACK_TRACE_LOG();
    }

    virtual ~net_event()
    {
        STACK_TRACE_LOG();

        if ( NULL != m_net_package_ )
        {
            m_net_package_->Destroy();
            m_net_package_ = NULL;
        }
    }

public:
    //! �����¼�����
    net_ev_t m_net_ev_t_;
    //! ����ͨ��id, ������Ǽ���ͨ�����˴�Ϊ0
    uint32_t m_listen_net_id_; 
    //! ����ͨ��id, ���û����ͨ��������ͨ���˴�Ϊ0
    uint32_t m_net_id_;

    //! ���ݰ��������洫���������ݣ����಻������䡣
    net_package* m_net_package_;
    //! ���¼�������ָ��
    void* m_user_data_;
    //! �Զ˵�ַ
    Address m_remote_addr_;
};


//! �����¼�������
typedef void (*net_event_callback_t)(net_event& ne);

typedef object_guard<net_event> net_event_pool;

//! �����¼�����
//! ͨ�Ų�����¼�������˶���, Ӧ�ò�ȡ���¼����ͷ�
//! �˶����̰߳�ȫ
typedef sync_queue<net_event*> net_event_queue;

#endif // _NET_EVENT_H_
