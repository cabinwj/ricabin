#ifndef _REACTOR_H_
#define _REACTOR_H_

#include "common_types.h"

class event_handler;

//! @class reactor
//! @brief ��Ӧ��ģ��
class reactor
{
public:
    virtual ~reactor() { }

public:
    //! ��reactor
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int open_reactor() = 0;

    //! �ر�reactor
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int close_reactor() = 0;

    //! �¼���ѭ��
    //! @return >=0:�����¼��ĸ���, <0:��Ӧ������
    virtual int run_reactor_event_loop() = 0;

    //! ֹͣ�¼���ѭ������������ע��Ĵ�����
    virtual int end_reactor_event_loop() = 0;

    //! ע��һ���¼�����
    //! @param event_handler �¼�������
    //! @param masks Ҫ�������¼�
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int enable_handler(event_handler* eh, uint32_t masks) = 0;

    //! ɾ��һ���¼�����
    //! @param event_handler �¼�������
    //! @param masks Ҫɾ�����¼�
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int disable_handler(event_handler* eh, uint32_t masks) = 0;
};

#endif // _REACTOR_H_
