//! @file epoll_reactor.h
#ifndef _EPOLL_REACTOR_H_
#define _EPOLL_REACTOR_H_

#ifndef WIN32

#include "hc_os.h"
#include "ireactor.h"
#include "ihandler.h"


class ihandler;

//! @class epoll_reactor
//! @brief EPOLL�汾�ķ�Ӧ��
class epoll_reactor : public ireactor
{
public:
    epoll_reactor();
    virtual ~epoll_reactor();

public:
    //! ��reactor
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int open_reactor();

    //! �ر�reactor
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int close_reactor();

    //! �¼���ѭ��
    //! @return >=0:�����¼��ĸ���, <0:��Ӧ������
    virtual int run_reactor_event_loop();

    //! ֹͣ�¼���ѭ������������ע��Ĵ�����
    virtual int end_reactor_event_loop();

    //! ע��һ���¼�����
    //! @param event_handler �¼�������
    //! @param masks Ҫ�������¼�
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int enable_handler(ihandler* eh, uint32_t masks);

    //! ɾ��һ���¼�����
    //! @param event_handler �¼�������
    //! @param masks Ҫɾ�����¼�
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int disable_handler(ihandler* eh, uint32_t masks);

private:
    //! �������(20��)
    static const int MAX_HANDLER = 200000;
    //! epoll������
    int m_epfd;
    //! epoll���ص��¼�
    epoll_event* m_events;
};
#endif

#endif // _EPOLL_REACTOR_H_
