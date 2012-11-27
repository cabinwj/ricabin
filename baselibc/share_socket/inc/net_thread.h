//! @file net_thread.h
#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_

#include "common_thread.h"
#include "reactor.h"
#include "net_manager.h"

class net_manager;
class reactor;


//! @class net_thread
//! @brief ���紦���߳�
//!
//! ���̸߳����������ݵ��շ�, ����ͨ����ʱ���, �����������ݷ��������ͨ���Ĵ����Ͷ���
class net_thread : public threadc
{
public:
    //! ���캯��
    //! @param net_manager ���������
    //! @param reactor ��Ӧ��
    net_thread(net_manager* nm, reactor* rctr);
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
    net_manager* m_net_manager_;

    //! ��Ӧ��
    reactor* m_reactor_;
};


#endif // _NET_THREAD_H_
