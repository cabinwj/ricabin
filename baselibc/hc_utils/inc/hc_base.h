#ifndef _COMMON_BASE_H_
#define _COMMON_BASE_H_

#include "hc_types.h"

enum
{
    error = -1,

    success = 0,
    fail    = 1,
    queue_buffer_is_not_enough = 2,   /**< 队列空间不够*/
    queue_is_empty             = 3,   /**< 队列为空*/
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
    //http头: POST /51.game HTTP/1.1\r\nContent-Length:65535\r\n\r\n
    http_head_length           = 49,       //http头的固定长度

    max_ip_address_length      = 30,       //

    max_datetime_string_length = 32,     /**<日期时间字符串的最大长度 <*/
    max_file_name_length       = 255,       /**<文件名最大长度<*/
    max_file_path_length       = 255,       /**<路径的最大长度*/
};

typedef struct ip_port_type
{
    char m_ip_[max_ip_address_length];
    int16_t m_port_;
}ip_port_t;

enum
{
    max_player_name_length    = 32,         /**< 最大用户名字长度*/
    max_player_id_length      = 32,         /**< 用户字符串型帐号的长度*/

    max_face_version_length   = 32,         /* 用户头像版本号长度*/
    max_face_url_length       = 96,         /* 用户头像的URL长度*/
    max_user_zone_info_length = 40,         /* 用户地区信息长度*/
    max_user_nick_name_length = 32,         /* 用户昵称的长度*/

    max_url_length            = 64,         /*URL地址的长度*/
};

void init_daemon(int8_t nochdir, int8_t noclose);

#endif // _COMMON_BASE_H_
