#ifndef _ENTITY_TYPES_H_
#define _ENTITY_TYPES_H_

#include "hc_types.h"

#include "protocol.h"


// 平台系统中的实体类型 (高16位表示类型，低16位表示id，一类服务可以有256台)
enum platform_entity_type
{
    pf_entity_null            = (uint32_t)0x0,

    pf_entity_admin_client    = (uint32_t)0x010000,
    pf_entity_client          = (uint32_t)0x020000,
    pf_entity_web_client      = (uint32_t)0x030000,

    pf_entity_webagent        = (uint32_t)0x040000,

    pf_entity_memcache        = (uint32_t)0x050000,
    pf_entity_dbagent         = (uint32_t)0x060000,
    pf_entity_status          = (uint32_t)0x070000,
    pf_entity_logic           = (uint32_t)0x080000,
    pf_entity_conn            = (uint32_t)0x090000,

    pf_entity_router          = (uint32_t)0x0A0000,

    // mark
    pf_entity_type_mark       = (uint32_t)0x7FF0000,
    pf_entity_id_mark         = (uint32_t)0x000FFFF,
};

enum
{
    offline_status        = (uint16_t)0,
    online_status         = (uint16_t)1,
    login_status          = (uint16_t)2,
    change_status         = (uint16_t)3,
};

enum
{
    //max_connect_entity_type_count   = 16,       //服务器类型的个数，即单个服务器仅能为16种不同类型的服务器作数据交互。
    //max_connect_entity_id_count     = 16,       //每类服务器，最多只配16个服务进程（对应不同的ip）。

    max_open_port_count             = 8,        //服务器打开的端口最大数目
    max_socket_count                = 0xfff0,   //支持的最大连接数

    max_client_pkg_size             = 10*1024,
    max_package_count_per_sending   = 512,      //

    server_bitmap_length            = 16,       // 服务器标识的长度

    //与签名认证相关信息
    max_signature_length            = 256,

    auth_service_key_length         = 16,        //与认证服务器约定的密钥长度
    crypt_key_length                = 16,        //与客户端的通讯使用的加密密钥长

    max_signature_valid_period      = 43200,     //(12*60*60) 12小时
    min_signature_valid_period      = 3600,      //60*60 1小时

    max_encrypt_key_length          = 16,        //加密密钥的长度

    min_cs_header_length            = 18,        //cs head(不包括option所占用的buff长度，也不包括sequence)

    min_option_length               = 0,
    max_option_length               = 256,

    offset_client_uin_in_cs_head    = 4,
    offset_option_lenth_in_cs_head  = 8,
    offset_option_in_cs_head        = 10,

    min_offset_message_id_in_cs_head    = 10,
    min_offset_message_type_in_cs_head  = 12,
    min_offset_player_id_in_cs_head     = 14,


    max_get_user_status_count    = 100,
};

// message_type 0x01-0x10为底层保留
enum
{
    //方向: client  --> server, client首次发送消息给server，client第一次发送消息
    message_start_request                = (uint16_t)0x11,

    //方向: client <--  server, server主动关闭client的链接
    message_force_close_connect          = (uint16_t)0x12,

    //方向: client  --> server, client主动断线
    message_disconnect_notify            = (uint16_t)0x13,

    //方向: client  --> server, client主动断线，服务器根据MAC地址hash
    message_disconnect_by_mac            = (uint16_t)0x14,
};

// close_reason_type
enum
{
    close_reason_service               = (uint16_t)0x21,    //service主动关闭
    close_reason_client                = (uint16_t)0x22,    //client主动关闭
    close_reason_io_error              = (uint16_t)0x23,    //IO读写错误
    close_reason_data_error            = (uint16_t)0x24,    //无法识别收到客户端的消息错误
    close_reason_queue_error           = (uint16_t)0x25,    //无法识别收到客户端的消息错误
    close_reason_recv_buff_error       = (uint16_t)0x26,    //接收缓冲错误(满)
    close_reason_send_buff_error       = (uint16_t)0x27,    //发送缓冲错误(满)

    close_reason_client_timeout        = (uint16_t)0x28,    //客户端超时(已经与service进程交换过数据)
    close_reason_channel_error         = (uint16_t)0x29,    //写内存管道错误
    close_reason_app_error             = (uint16_t)0x2a,    //进程错误
    close_reason_system_error          = (uint16_t)0x2b,    //系统错误
    close_reason_invalid_connect       = (uint16_t)0x2c,    //连接无效（没有与后端server有过数据交换，不需要通知service进程进行清理动作）

    close_reason_invalid_signature     = (uint16_t)0x2d,    //无效的数字签名 
};

struct signature_t
{
    char m_session_key_[max_encrypt_key_length];
    char m_service_bitmap_[server_bitmap_length];
    uint32_t m_client_uin_;
    uint32_t m_timestamp_;
};

struct net_hdr_t
{
    uint32_t m_packet_len_;
    uint16_t m_message_id_;
    uint16_t m_message_type_;   // message_t

    uint16_t m_control_type_;   // 操作和处理方式
    uint16_t m_proto_type_;     // 协议标示 应用协议类型（0 二进制，1 4字节文本，2 6字节文本）

    uint32_t m_reserved_;       // 协程sequence标示/服务器uin标示
    uint32_t m_request_sequence_;// client包的请求序列

    uint32_t m_client_net_id_;  // net 通道id
    uint32_t m_client_uin_;     // client uin
    uint32_t m_from_uin_;
    uint32_t m_to_uin_;
};

struct conn_hdr_t
{
    uint32_t m_packet_len_;
    uint32_t m_client_uin_;
    uint16_t m_proto_type_;     // 协议标示 应用协议类型（0 二进制，1 4字节文本，2 6字节文本）
    uint16_t m_option_len_;     // 0 <= option_ <= 256 
    char m_option_[max_signature_length];

    uint16_t m_message_id_;
    uint16_t m_message_type_;
    uint32_t m_player_id_;
    uint32_t m_request_sequence_;    
};

struct socket_statistics_t 
{
    uint32_t m_socket_count_;
};

typedef enum client_protocol_type
{
    // client 与 connector 之间的协议  1000起
    C_KEEPALIVE                   = (uint16_t)1000,

    C_LOGIN                       = (uint16_t)1001,
    C_LOGIN_OVER                  = (uint16_t)1002,
    C_LOGOUT                      = (uint16_t)1003,
    C_FORCE_LOGOUT                = (uint16_t)1004,

    C_MODIFY_USER_INFO            = (uint16_t)1101,
    C_MODIFY_FRIEND_GROUP         = (uint16_t)1102,
    C_MODIFY_FRIEND_GROUP_NAME    = (uint16_t)1103,
    C_GET_FRIEND_GROUP            = (uint16_t)1104,
    C_GET_FRIEND_LIST             = (uint16_t)1105,
    C_GET_FRIEND_INFO             = (uint16_t)1106,
    C_ADD_FRIEND_GROUP            = (uint16_t)1107,
    C_REMOVE_FRIEND_GROUP         = (uint16_t)1108,
    C_REMOVE_FRIEND               = (uint16_t)1109,
    C_GET_OFFLINE_MSG             = (uint16_t)1110,
    C_REMOVE_OFFLINE_MSG          = (uint16_t)1111,

    C_CHANGE_STATUS               = (uint16_t)1201,
    C_CHANGE_PICTURE              = (uint16_t)1202,
    C_CHANGE_PROFILE              = (uint16_t)1203,

    C_CHAT_MSG                      = (uint16_t)1301,
    C_CHAT_MSG_ACK                  = (uint16_t)1302,
    C_P2P_TRANSPARENT               = (uint16_t)1303,

    C_GET_FRIEND_STATUS             = (uint16_t)1401,

    C_ADD_FRIEND                    = (uint16_t)1501,
    C_REFUSE_ADD_FRIEND             = (uint16_t)1502,
    C_AGREE_ADD_FRIEND              = (uint16_t)1503,
    C_RECV_ADD_FRIEND_INVITE_ACK    = (uint16_t)1504,

    C_FRIEND_STATUS_CHANGE_NOTICE   = (uint16_t)1601,
    C_FRIEND_RELATION_CHANGE_NOTICE = (uint16_t)1602,
    C_PICTURE_CHANGE_NOTICE         = (uint16_t)1603,
    C_PROFILE_CHANGE_NOTICE         = (uint16_t)1604,

} c_proto_t;

typedef enum server_protocol_type
{
    // server 与 server 之间的协议  6000起
    S_KEEPALIVE                   = (uint16_t)6000,

    S_LOGIN                       = (uint16_t)6001,
    S_LOGIN_OVER                  = (uint16_t)6002,
    S_LOGOUT                      = (uint16_t)6003,
    S_FORCE_LOGOUT                = (uint16_t)6004,

    S_MODIFY_USER_INFO            = (uint16_t)6101,
    S_MODIFY_FRIEND_GROUP         = (uint16_t)6102,
    S_MODIFY_FRIEND_GROUP_NAME    = (uint16_t)6103,
    S_GET_FRIEND_GROUP            = (uint16_t)6104,
    S_GET_FRIEND_LIST             = (uint16_t)6105,
    S_GET_FRIEND_INFO             = (uint16_t)6106,
    S_ADD_FRIEND_GROUP            = (uint16_t)6107,
    S_REMOVE_FRIEND_GROUP         = (uint16_t)6108,
    S_REMOVE_FRIEND               = (uint16_t)6109,
    S_GET_OFFLINE_MSG             = (uint16_t)6110,
    S_REMOVE_OFFLINE_MSG          = (uint16_t)6111,

    S_CHANGE_STATUS                 = (uint16_t)6201,

    S_FRIEND_CHAT_MSG               = (uint16_t)6301,
    S_FRIEND_CHAT_MSG_ACK           = (uint16_t)6302,
    S_P2P_TRANSPARENT               = (uint16_t)6303,

    S_GET_FRIEND_STATUS             = (uint16_t)6401,

    S_ADD_FRIEND                    = (uint16_t)6501,
    S_REFUSE_ADD_FRIEND             = (uint16_t)6502,
    S_AGREE_ADD_FRIEND              = (uint16_t)6503,

    S_FRIEND_STATUS_CHANGE_NOTICE   = (uint16_t)6601,
    S_FRIEND_RELATION_CHANGE_NOTICE = (uint16_t)6602,
    S_PICTURE_CHANGE_NOTICE         = (uint16_t)6603,
    S_PROFILE_CHANGE_NOTICE         = (uint16_t)6604,

    // ss
    COND_IMD_ADD_OFFLINE_CHAT_MSG   = (uint16_t)10001,
    COND_IMD_ADD_OFFLINE_ADD_FRIEND = (uint16_t)10002,
    COND_IMD_RECV_ADD_FRIEND        = (uint16_t)10003,
    COND_IMD_USER_TIMEOUT           = (uint16_t)10004,

    IMD_DBAGENTD_ADD_OFFLINE_ADD_FRIEND    = (uint16_t)10101,
    IMD_DBAGENTD_ADD_OFFLINE_CHAT_MSG      = (uint16_t)10102,
    IMD_DBAGENTD_REMOVE_OFFLINE_ADD_FRIEND = (uint16_t)10103,

    IMD_IMD_GET_FRIEND_STATUS             = (uint16_t)10201,
    IMD_IMD_FRIEND_STATUS_CHANGE          = (uint16_t)10202,
    IMD_IMD_FRIEND_RELATION_CHANGE        = (uint16_t)10203,
    IMD_IMD_FRIEND_INFO_CHANGE            = (uint16_t)10204,
    IMD_IMD_RECV_ADD_FRIEND               = (uint16_t)10205,
    IMD_IMD_CHAT_MSG                      = (uint16_t)10206,
    IMD_IMD_RECV_OFFLIEN_ADD_FRIEND       = (uint16_t)10207,
    IMD_IMD_RECV_OFFLINE_CHAT_MSG         = (uint16_t)10208,
    IMD_IMD_FRIEND_PICTURE_CHANGE         = (uint16_t)10209,
    IMD_IMD_FRIEND_PROFILE_CHANGE         = (uint16_t)10210,

    IMD_STATUSD_GET_USER_STATUS           = (uint16_t)10301,
    IMD_STATUSD_REPORT_USER_STATUS        = (uint16_t)10302,

} s_proto_t;

#endif   // _ENTITY_TYPES_H_
