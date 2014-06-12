#ifndef _NET_NODE_H_
#define _NET_NODE_H_

#include "hc_list.h"
#include "hc_bitmap.h"

#include "net_event.h"
#include "address.h"


class net_node
{
public:
    net_node();
    virtual ~net_node();

public:
    virtual void on_ne_data(net_event& ne) { }

public:
    list_head m_list_item_;             // ����accept or connect����
    list_head m_net_hash_item_;         // ����hash��
    list_head m_uin_hash_item_;         // ��Ӧ�ò�����������ı�ʶuin hash ��

    uint32_t m_net_id_;                 // netid  // ����netid ��Ϊ���ʾ�� accept������   

    uint32_t m_remote_uin_;             // ��Ӧ�ò������������ID������Ψһ��ʶ��
    // �����accept client�����ӣ�uin����Ӧ�ò�������������û�uin���� ���Ա�����û�з����ʶ
    // �����accept server�����ӣ�uin���ǶԷ�����ı�ʶ���Է��ķ���ID��
    // �����connect out�����ӣ�uin���ǶԷ�����ı�ʶ���Է��ķ���ID��

    uint32_t m_last_recv_timestamp_;    // ���һ�δӶԶ˽��յ����ݵ�timestamp

    bitmap32 m_is_trans_;               // is_transmission_t
    Address m_remote_addr_;             // �Զ˵�ip
};

#endif
