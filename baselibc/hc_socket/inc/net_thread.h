//! @file net_thread.h
#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_

#include "hc_thread.h"

class net_messenger;
class ireactor;

//! @class net_thread
//! @brief ���紦���߳�
//!
//! ���̸߳����������ݵ��շ�, ����ͨ����ʱ���, �����������ݷ��������ͨ���Ĵ����Ͷ���
class net_thread : public threadc
{
public:
    //! ���캯��
    //! @param net_messenger ���������
    //! @param reactor ��Ӧ��
    net_thread(net_messenger* nm, ireactor* nrc);
    virtual ~net_thread();

public:
    //! �����߳�
    int start();
    //! ֹͣ�߳�
    int stop();
    //! �̺߳���
    virtual int svc();

private:
    //! ����״̬
    volatile bool m_is_run_;
    //! ֹ֪ͣͨ
    volatile bool m_notify_stop_;
    //! ���������
    net_messenger* m_net_messenger_;
    //! ��Ӧ��
    ireactor* m_reactor_;
};


#endif // _NET_THREAD_H_
