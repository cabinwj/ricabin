#ifndef _REACTOR_H_
#define _REACTOR_H_

#include "hc_types.h"

class ihandler;

//! @class ireactor
//! @brief ��Ӧ��ģ��
class ireactor
{
public:
    virtual ~ireactor() { }

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
    virtual int enable_handler(ihandler* eh, int16_t masks) = 0;

    //! ɾ��һ���¼�����
    //! @param event_handler �¼�������
    //! @param masks Ҫɾ�����¼�
    //! @return 0:�ɹ�, <0:ʧ��
    virtual int disable_handler(ihandler* eh, int16_t masks) = 0;
};

#endif // _REACTOR_H_
