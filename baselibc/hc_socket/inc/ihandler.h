#ifndef _SOCK_HANDLER_H_
#define _SOCK_HANDLER_H_

#include "hc_os.h"
#include "address.h"

class net_package;
class ihandler;
class ireactor;

//! �¼�������
class ihandler
{
public:
    //! �¼�����
    enum ev_mask_t {
        EM_NULL    = 0,
        EM_READ    = 1 << 0,    //!< �ɶ��¼�
        EM_WRITE   = 1 << 1,    //!< ��д�¼�

        EM_ALL     = EM_READ | EM_WRITE,
    };

public:
    virtual ~ihandler() { }

public:
    //! ��
    virtual int open(const Address& dst_addr, int recv_bufsize, int send_bufsize) = 0;
    //! �ر�
    virtual void close() = 0;
    //! ��ȡ �����ʾ��ͨ��id��
    virtual int32_t handler_o() = 0;
    //! ���� �����ʾ��ͨ��id��
    virtual void handler_o(int32_t net_id) = 0;
    //! ��ȡ socket Descriptor ������
    virtual Descriptor descriptor_o() = 0;
    //! ���� socket Descriptor ������
    virtual void descriptor_o(Descriptor sock) = 0;
    //! ��ȡ event mask �¼���ע��Ϣ
    virtual int32_t event_mask_o() = 0;
    //! ��ȡ event mask �¼���ע��Ϣ
    virtual void event_mask_o(int32_t ev_mask) = 0;
    //! ��ȡ ��Ӧ��
    virtual ireactor* reactor_o() = 0;
    //! ���� ��Ӧ��
    virtual void reactor_o(ireactor* react) = 0;

    //! �����
    //! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
    virtual int handle_input() = 0;
    //! ����д
    //! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
    virtual int handle_output() = 0;
    //! ���ӹر�  event_type = net_event::NE_CLOSE
    //! �����쳣  event_type = net_event::NE_EXCEPTION
    //! ��ʱ      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(int16_t event_type) = 0;
    //! �ύ��������
    //! @param netpkg �����͵�����
    virtual int post_package(net_package* netpkg) = 0;
};

#endif // _SOCK_HANDLER_H_
