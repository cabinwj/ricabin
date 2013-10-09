#ifndef _NET_ID_HOLDER_H_
#define _NET_ID_HOLDER_H_

#include "hc_types.h"
#include "hc_sync_queue.h"


//! @class net_id_holder
//! @brief ����ͨ��id������
//!
//! ����ά�������㹻������ͨ��id, ʵ�����й����в����ܳ���id�þ������
class net_id_holder
{
public:
    //! @enum net_id_type
    //! @brief ����ͨ��id����
    typedef enum net_id_type
    {
        ID_TCP_CONNECTOR,
        ID_TCP_ACCEPTOR,
        ID_TCP_STREAM
    } net_id_t;

    typedef enum id_interval_type {
        TCP_CONNECTOR_BEGIN = 100001,
        TCP_CONNECTOR_END = 200000,

        TCP_ACCEPTOR_BEGIN = 200001,
        TCP_ACCEPTOR_END = 300000,

        TCP_STREAM_BEGIN = 300001,
        TCP_STREAM_END = 1000000
    } id_interval_t;

public:
    net_id_holder();
    ~net_id_holder();

    //! ����һ������ͨ��id
    //! @param type Ҫ�����ͨ������
    //! @return ͨ��id 0:ʧ��,id�þ�,  >0:�ɹ�
    int32_t acquire(net_id_holder::net_id_t type);

    //! �ͷ�����ͨ��id
    //! @param id ����ͨ��id
    void release(int32_t id);

private:
    //! ����ͨ��id����
    typedef sync_queue<int32_t> net_id_queue;

    //! ����ͨ��tcp_connector_id����10��
    net_id_queue* m_tcp_connector_id_queue_;
    //! ����ͨ��tcp_acceptor_id����10��
    net_id_queue* m_tcp_acceptor_id_queue_;
    //! ����ͨ��tcp_acceptor_id����70��
    net_id_queue* m_tcp_stream_id_queue_;
};


#endif // _NET_ID_HOLDER_H_
