#ifndef _NET_EVENT_H_
#define _NET_EVENT_H_

#include "hc_types.h"
#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_object_holder.h"
#include "hc_sync_queue.h"

#include "address.h"

class Address;
class net_package;

//! @class net_event
//! @brief �����¼���
class net_event : public non_copyable, public destroyable
{
public:
    //! @enum net_event_type
    //! @brief �����¼�����
    typedef enum net_event_type {
        NE_NULL            = 0,         //!< ���¼�

        // �ײ��¼����׸��ϲ�
        NE_CONNECTED       = 1,         //!< tcp���ӳɹ�
        NE_ACCEPT          = 2,         //!< tcp��������
        NE_DATA            = 3,         //!< ���ݵ���

        NE_CLOSE           = 4,         //!< tcp���ӹر�
        NE_EXCEPTION       = 5,         //!< �����쳣

        // Ӧ�ò�֪ͨ�ײ�رգ��ײ�رպ����׸��ϲ�
        NE_TIMEOUT         = 6,         //!< tcp���ӳ�ʱ
        NE_NOTIFY_CLOSE    = 7,         //!< tcp֪ͨ�ر�
    } net_ev_t;

public:
    net_event();
    virtual ~net_event();

public:
    //! �����¼�����
    int16_t m_net_ev_t_;
    //! ����ͨ��id / ����ͨ��id
    int32_t m_net_id_;

    //! ���ݰ��������洫���������ݣ����಻������䡣
    net_package* m_net_package_;
    //! ���¼�������ָ��
    void* m_user_data_;
    //! �Զ˵�ַ
    Address m_remote_addr_;

public:
    //! net_event ����������㱻���䲢��ʼ��, ��Ӧ�ò㱻ʹ�ò�����, Ϊ���Ч��ʹ��Ԥ��������
    static object_holder<net_event>* m_pool_;
};

//! �����¼�������
typedef void (*net_event_callback_t)(net_event& ne);

//! �����¼�����
//! ͨ�Ų�����¼�������˶���, Ӧ�ò�ȡ���¼����ͷ�
//! �˶����̰߳�ȫ
typedef sync_queue<net_event*> net_event_queue;

#endif // _NET_EVENT_H_
