#ifndef _HC_PROTOCOL_H_
#define _HC_PROTOCOL_H_

#include "hc_types.h"
#include "net_event.h"


enum message_t
{
    message_invalid                      = (uint16_t)0,

    // 0x01-0x10Ϊ�ײ㱣�� 
    message_alive                        = (uint16_t)0x01,   // ������
    message_async_syn                    = (uint16_t)0x02,   // Э���첽����request ��Э��ר�ã�
    message_async_ack                    = (uint16_t)0x03,   // Э���첽��Ӧresponse��Э��ר�ã�
    message_common                       = (uint16_t)0x04,   // ��ͨ��common
    message_syn                          = (uint16_t)0x05,   // ����request
    message_ack                          = (uint16_t)0x06,   // ��Ӧresponse
    message_ntf                          = (uint16_t)0x07,   // ֪ͨnotify
};


enum proto_t
{
    PROTOCOL_IN = (int16_t)0,    //�첽����Э��(��������Ӧ����ͨ��) �����޷�����֮��ͨѶ��
    PROTOCOL_EX = (int16_t)1,    //��������Э��
} ;


enum socket_porto_t
{
    SOCKET_PROTO_TCP = (int16_t)0,
    SOCKET_PROTO_UDP = (int16_t)1
};


enum run_status_t
{
    run_status_reset          = (int8_t)0x0,
    run_status_reload         = (int8_t)0x01,
    run_status_exit           = (int8_t)0x02
};


enum is_trans_t
{
    is_trans_null             = 1 <<  0,
    is_trans_sended_data      = 1 <<  1,  //��ʶ�Ƿ�����Զ˷��͹�����
    is_trans_recved_data      = 1 <<  2,  //��ʶ�Ƿ��յ����Զ˵�����
    is_trans_reconnect        = 1 <<  3,  //��ʶ�Ƿ���Ҫ���������ߣ�
    is_trans_connect_out      = 1 <<  4,  //��ʶ�Ƿ�Ϊ����ȥ������
    is_trans_accept_client    = 1 <<  5,  //��ʶ�Ƿ�Ϊaccept from client
    is_trans_accept_server    = 1 <<  6,  //��ʶ�Ƿ�Ϊaccept from server
};


struct net_conf_t
{
    int16_t m_proto_type_;           // tcp or udp
    int16_t m_is_listen_;            // �Ǽ�����������ȥ��
    int16_t m_app_proto_;            // Ӧ��Э�����ͣ�0 �����ƣ�1 4�ֽ��ı���2 6�ֽ��ı���

    ip_port_t m_ipport_;

    uint32_t m_socket_buffer_size_; // socket���͡����ջ�������С
    uint32_t m_connect_out_uin_;    // ��������ȥ��socket��Ч: ����ȥ�ķ����ʶ���Զ˵ķ�����id

    int16_t m_keep_alive_timeout_;  // ���û�յ������������κΰ��������Ͽ�����
    int16_t m_reconnect_interval_;  // ��������ȥ��socket��Ч: �Զ������ļ��ʱ�䣬����Ϊ0���Զ����� (��λ��s)
    int16_t m_keep_alive_interval_; // ��������ȥ��socket��Ч: ������������Ϊ0��������  (��λ��s)
    int16_t m_connect_timeout_;     // ��������ȥ��socket��Ч����������ĳ�ʱʱ��

    //// ��û������
    //int32_t m_max_wait_for_first_package_time_;       //socket�����ϵȴ���һ���������ʱ����(Ҳ�������ӽ�����,�ȴ����յ�һ�����ļ��)(��λ��s)
    //int32_t m_check_socket_interval_;                 //����������ӵ�ʱ����(��λ��s)
    //int32_t m_log_statistic_interval_;                //ͳ�Ƽ��

    //int32_t m_signature_valid_period_;                //����ǩ����Чʱ��
} ;

//class conn_param_t
struct conn_param_t
{
//public:
    conn_param_t() : m_reference_count_(1) {}

    void acquire() { ++m_reference_count_; }
    void release() { --m_reference_count_; if (0 == m_reference_count_) delete this; }

//public:
    uint32_t m_reference_count_;
    uint32_t m_remote_uin_;         // ��������ȥ��socket��Ч: ����ȥ�ķ����ʶ���Զ˵ķ�����id

    net_event_callback_t m_net_event_handler_cb_;

    int16_t m_app_proto_;
    int16_t m_keep_alive_timeout_;  // ���û�յ������������κΰ��������Ͽ�����
    int16_t m_reconnect_interval_;  // ��������ȥ��socket��Ч: �Զ������ļ��ʱ�䣬����Ϊ0���Զ�����
    int16_t m_keep_alive_interval_; // ��������ȥ��socket��Ч: ������������Ϊ0��������         
};

#endif
