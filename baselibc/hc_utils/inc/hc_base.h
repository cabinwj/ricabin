#ifndef _COMMON_BASE_H_
#define _COMMON_BASE_H_

#include "hc_types.h"

enum
{
    error = -1,

    success = 0,
    fail    = 1,
    queue_buffer_is_not_enough = 2,   /**< ���пռ䲻��*/
    queue_is_empty             = 3,   /**< ����Ϊ��*/
};

enum
{//seconds in xxx
    MINUTE = 60,        //one minute
    HOUR   = 60*MINUTE, //one hour
    DAY    = 24*HOUR,   //one day
    YEAR   = 365*DAY    //one year
};

enum
{
    //httpͷ: POST /51.game HTTP/1.1\r\nContent-Length:65535\r\n\r\n
    http_head_length           = 49,       //httpͷ�Ĺ̶�����

    max_ip_address_length      = 30,       //

    max_datetime_string_length = 32,     /**<����ʱ���ַ�������󳤶� <*/
    max_file_name_length       = 255,       /**<�ļ�����󳤶�<*/
    max_file_path_length       = 255,       /**<·������󳤶�*/
};

typedef struct ip_port_type
{
    char m_ip_[max_ip_address_length];
    int16_t m_port_;
}ip_port_t;

enum
{
    max_player_name_length    = 32,         /**< ����û����ֳ���*/
    max_player_id_length      = 32,         /**< �û��ַ������ʺŵĳ���*/

    max_face_version_length   = 32,         /* �û�ͷ��汾�ų���*/
    max_face_url_length       = 96,         /* �û�ͷ���URL����*/
    max_user_zone_info_length = 40,         /* �û�������Ϣ����*/
    max_user_nick_name_length = 32,         /* �û��ǳƵĳ���*/

    max_url_length            = 64,         /*URL��ַ�ĳ���*/
};

void init_daemon(int8_t nochdir, int8_t noclose);

#endif // _COMMON_BASE_H_
