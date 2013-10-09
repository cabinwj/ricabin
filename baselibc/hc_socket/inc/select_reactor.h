//! @file select_reactor.h
#ifndef _SELECT_REACTOR_H_
#define _SELECT_REACTOR_H_

#include "hc_os.h"
#include "ireactor.h"
#include "ihandler.h"

class ihandler;
class ireactor;

//! @class select_reactor
//! @brief Select�汾�ķ�Ӧ��
class select_reactor : public ireactor
{
public:
    select_reactor();
    virtual ~select_reactor();

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
    virtual int enable_handler(ihandler* eh, int16_t masks);

    //! ɾ��һ���¼�����
    //! @param event_handler �¼�������
    //! @param masks Ҫɾ�����¼�
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int disable_handler(ihandler* eh, int16_t masks);

private:
    fd_set m_read_set_;
    fd_set m_write_set_;
    fd_set m_exception_set_;
};

#endif // _SELECT_REACTOR_H_
