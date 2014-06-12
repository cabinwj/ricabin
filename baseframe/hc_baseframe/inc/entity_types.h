#ifndef _ENTITY_TYPES_H_
#define _ENTITY_TYPES_H_

#include "hc_types.h"

#include "protocol.h"


// ƽ̨ϵͳ�е�ʵ������ (��16λ��ʾ���ͣ���16λ��ʾid��һ����������256̨)
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
    //max_connect_entity_type_count   = 16,       //���������͵ĸ���������������������Ϊ16�ֲ�ͬ���͵ķ����������ݽ�����
    //max_connect_entity_id_count     = 16,       //ÿ������������ֻ��16��������̣���Ӧ��ͬ��ip����

    max_open_port_count             = 8,        //�������򿪵Ķ˿������Ŀ
    max_socket_count                = 0xfff0,   //֧�ֵ����������

    max_client_pkg_size             = 10*1024,
    max_package_count_per_sending   = 512,      //

    server_bitmap_length            = 16,       // ��������ʶ�ĳ���

    //��ǩ����֤�����Ϣ
    max_signature_length            = 256,

    auth_service_key_length         = 16,        //����֤������Լ������Կ����
    crypt_key_length                = 16,        //��ͻ��˵�ͨѶʹ�õļ�����Կ��

    max_signature_valid_period      = 43200,     //(12*60*60) 12Сʱ
    min_signature_valid_period      = 3600,      //60*60 1Сʱ

    max_encrypt_key_length          = 16,        //������Կ�ĳ���

    min_cs_header_length            = 18,        //cs head(������option��ռ�õ�buff���ȣ�Ҳ������sequence)

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

// message_type 0x01-0x10Ϊ�ײ㱣��
enum
{
    //����: client  --> server, client�״η�����Ϣ��server��client��һ�η�����Ϣ
    message_start_request                = (uint16_t)0x11,

    //����: client <--  server, server�����ر�client������
    message_force_close_connect          = (uint16_t)0x12,

    //����: client  --> server, client��������
    message_disconnect_notify            = (uint16_t)0x13,

    //����: client  --> server, client�������ߣ�����������MAC��ַhash
    message_disconnect_by_mac            = (uint16_t)0x14,
};

// close_reason_type
enum
{
    close_reason_service               = (uint16_t)0x21,    //service�����ر�
    close_reason_client                = (uint16_t)0x22,    //client�����ر�
    close_reason_io_error              = (uint16_t)0x23,    //IO��д����
    close_reason_data_error            = (uint16_t)0x24,    //�޷�ʶ���յ��ͻ��˵���Ϣ����
    close_reason_queue_error           = (uint16_t)0x25,    //�޷�ʶ���յ��ͻ��˵���Ϣ����
    close_reason_recv_buff_error       = (uint16_t)0x26,    //���ջ������(��)
    close_reason_send_buff_error       = (uint16_t)0x27,    //���ͻ������(��)

    close_reason_client_timeout        = (uint16_t)0x28,    //�ͻ��˳�ʱ(�Ѿ���service���̽���������)
    close_reason_channel_error         = (uint16_t)0x29,    //д�ڴ�ܵ�����
    close_reason_app_error             = (uint16_t)0x2a,    //���̴���
    close_reason_system_error          = (uint16_t)0x2b,    //ϵͳ����
    close_reason_invalid_connect       = (uint16_t)0x2c,    //������Ч��û������server�й����ݽ���������Ҫ֪ͨservice���̽�����������

    close_reason_invalid_signature     = (uint16_t)0x2d,    //��Ч������ǩ�� 
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

    uint16_t m_control_type_;   // �����ʹ���ʽ
    uint16_t m_proto_type_;     // Э���ʾ Ӧ��Э�����ͣ�0 �����ƣ�1 4�ֽ��ı���2 6�ֽ��ı���

    uint32_t m_reserved_;       // Э��sequence��ʾ/������uin��ʾ
    uint32_t m_request_sequence_;// client������������

    uint32_t m_client_net_id_;  // net ͨ��id
    uint32_t m_client_uin_;     // client uin
    uint32_t m_from_uin_;
    uint32_t m_to_uin_;
};

struct conn_hdr_t
{
    uint32_t m_packet_len_;
    uint32_t m_client_uin_;
    uint16_t m_proto_type_;     // Э���ʾ Ӧ��Э�����ͣ�0 �����ƣ�1 4�ֽ��ı���2 6�ֽ��ı���
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
    // client �� connector ֮���Э��  1000��
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
    // server �� server ֮���Э��  6000��
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
