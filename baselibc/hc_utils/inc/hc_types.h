//! ģ���õ��ĸ�������������Ͷ���
#ifndef _HC_TYPES_H_
#define _HC_TYPES_H_

#include <stddef.h>                // for size_t
#include <stdint.h>

#ifdef WIN32    // for windows
#define CHECK_FORMAT(i, j)
#define bzero(a,b) (memset((a),0,(b)))
#else            // for linux(gcc)
#define CHECK_FORMAT(i, j) __attribute__((format(printf, i, j)))
#endif

#ifdef WIN32    // for windows
#include <winsock2.h>
//typedef __int8                  int8_t;
//typedef __int16                 int16_t;
//typedef __int32                 int32_t;
//typedef __int64                 int64_t;
//
//typedef unsigned __int8         uint8_t;
//typedef unsigned __int16        uint16_t;
//typedef unsigned __int32        uint32_t;
//typedef unsigned __int64        uint64_t;

typedef int                        ssize_t;
typedef int                        socklen_t;
typedef uint32_t                   in_addr_t;
typedef uint16_t                   in_port_t;

#else            // for linux

#include <getopt.h>
#include <unistd.h>                // for ssize_t and socklen_t
#include <netinet/in.h>            // for in_addr_t and in_port_t

#endif

//#ifdef _WIN64
//  typedef long int                 intptr_t;
//  typedef unsigned long int            uintptr_t;
//#else
//  typedef int                    intptr_t;
//  typedef unsigned int            uintptr_t;
//#endif

enum
{//seconds in xxx
    MINUTE = 60,        //one minute
    HOUR   = 60*MINUTE, //one hour
    DAY    = 24*HOUR,   //one day
    YEAR   = 365*DAY    //one year
};

// ϵͳ���
enum
{
    //httpͷ: POST /51.game HTTP/1.1\r\nContent-Length:65535\r\n\r\n
    http_head_length           = 49,        /**<HTTPͷ�Ĺ̶����� <*/

    max_ip_address_length      = 32,        /**<IP��ַ ����ֽڳ��� <*/

    max_datetime_string_length = 32,        /**<����ʱ���ַ�������󳤶� <*/
    max_file_name_length       = 255,       /**<�ļ�����󳤶� <*/
    max_file_path_length       = 255,       /**<·������󳤶� <*/
};

typedef struct ip_port_type
{
    char m_ip_[max_ip_address_length];
    uint16_t m_port_;
}ip_port_t;

// Ӧ�����
enum
{
    max_player_name_length    = 32,         /**<����û����ֳ��� <*/
    max_player_id_length      = 32,         /**<�û��ַ������ʺŵĳ��� <*/

    max_face_version_length   = 32,         /**<�û�ͷ��汾�ų��� <*/
    max_face_url_length       = 96,         /**<�û�ͷ���URL���� <*/
    max_user_zone_info_length = 40,         /**<�û�������Ϣ���� <*/
    max_user_nick_name_length = 32,         /**<�û��ǳƵĳ��� <*/

    max_url_length            = 64,         /**<URL��ַ�ĳ��� <*/
};

void init_daemon(int8_t nochdir, int8_t noclose);

//! ˯�ߣ���Ϊ����
void sleep_ms(unsigned long _ms);


#endif // _HC_TYPES_H_
