#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "hc_types.h"
#include "hc_base.h"

#include "net_event.h"


enum message_t
{
    message_invalid                      = (uint16_t)0,

    // 0x01-0x10为底层保留 
    message_alive                        = (uint16_t)0x01,
    message_async_syn                    = (uint16_t)0x02,
    message_async_ack                    = (uint16_t)0x03,
    message_other                        = (uint16_t)0x04,
    message_request                      = (uint16_t)0x05,
    message_response                     = (uint16_t)0x06,
    message_notify                       = (uint16_t)0x07,
    message_common                       = (uint16_t)0x08,
};


enum proto_t
{
    PROTOCOL_ASYNC  = (int16_t)0,    //异步操作协议(包括请求、应答、普通包)
    PROTOCOL_TEXT4  = (int16_t)1,    //Web 4字节文本协议
    PROTOCOL_TEXT6  = (int16_t)2,    //Web 6字节文本协议
    PROTOCOL_SIMPLE = (int16_t)3,    //简单协议，只拆包，不做任何处理
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
    is_trans_sended_data      = 1 <<  1,  //标识是否有向对端发送过数据
    is_trans_recved_data      = 1 <<  2,  //标识是否收到过对端的数据
    is_trans_reconnect        = 1 <<  3,  //标识是否需要重连（断线）
    is_trans_connect_out      = 1 <<  4,  //标识是否为连出去的连接
    is_trans_accept_client    = 1 <<  5,  //标识是否为accept from client
    is_trans_accept_server    = 1 <<  6,  //标识是否为accept from server
};


struct net_conf_t
{
    int16_t m_proto_type_;           // tcp or udp
    int16_t m_is_listen_;            // 是监听还是连出去的
    int16_t m_app_proto_;            // 应用协议类型（0 二进制，1 4字节文本，2 6字节文本）

    ip_port_t m_ipport_;
    uint32_t m_socket_buffer_size_; // socket发送、接收缓冲区大小
    int16_t m_keep_alive_timeout_;  // 多久没收到心跳包（或任何包），做断开处理

    uint32_t m_connect_out_uin_;    // 仅对连出去的socket有效: 连出去的服务标识，对端的服务器id

    int16_t m_reconnect_interval_;  // 仅对连出去的socket有效: 自动重连的间隔时间，设置为0则不自动重连 (单位：s)
    int16_t m_keep_alive_interval_; // 仅对连出去的socket有效: 心跳包，设置为0无心跳包  (单位：s)
    int16_t m_connect_timeout_;     // 仅对连出去的socket有效：建立连结的超时时间

    //// 还没有用上
    //int32_t m_max_wait_for_first_package_time_;       //socket连接上等待第一个包的最大时间间隔(也就是连接建立后,等待接收第一个包的间隔)(单位：s)
    //int32_t m_check_socket_interval_;                 //检查网络连接的时间间隔(单位：s)
    //int32_t m_log_statistic_interval_;                //统计间隔

    //int32_t m_signature_valid_period_;                //数字签名有效时长

} ;


struct conn_param_t
{
    conn_param_t() : m_reference_count_(1) {}

    void acquire() { ++m_reference_count_; }
    void release() { --m_reference_count_; if (0 == m_reference_count_) delete this; }

    uint32_t m_reference_count_;

    int16_t m_app_proto_;

    uint32_t m_remote_uin_;         // 仅对连出去的socket有效: 连出去的服务标识，对端的服务器id

    net_event_handler_callback_t m_net_event_handler_cb_;

    int16_t m_keep_alive_timeout_;  // 多久没收到心跳包（或任何包），做断开处理
    int16_t m_reconnect_interval_;  // 仅对连出去的socket有效: 自动重连的间隔时间，设置为0则不自动重连
    int16_t m_keep_alive_interval_; // 仅对连出去的socket有效: 心跳包，设置为0无心跳包         
};

#endif
