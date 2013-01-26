//! @file config.h
//! @brief ϵͳ�����������
#ifndef _CONFIG_H_
#define _CONFIG_H_


//! @def MAX_PACKET_LENGTH
//! @brief �������󳤶�
//!
//! Ŀǰϵͳʹ�õİ���С��������2048, �˴������ݻ����С��Ϊ2048,
//! ������Э��ʱע������Ȳ�Ҫ�����˴�����
#ifndef MAX_PACKET_LENGTH
#define MAX_PACKET_LENGTH 2048
#endif


//! @def MAX_NET_EVENT_COUNT
//! @brief ��������¼�����
//!
//! ʵ��δ���������¼�����MAX_NET_EVENT_COUNTʱ����ֶ�ʧ�����¼������
#ifndef MAX_NET_EVENT_COUNT
#define MAX_NET_EVENT_COUNT 1024000
#endif


//! @def MAX_NET_SEND_TASK_COUNT
//! @brief ���ȫ�ִ������������
//!
//! ʵ�ʴ����������������ʱ����ֶ�ʧ���Ͱ������
#ifndef MAX_NET_SEND_TASK_COUNT
#define MAX_NET_SEND_TASK_COUNT 1024000
#endif


//! @def MAX_SOCKET_SEND_TASK_COUNT
//! @brief ��󵥸�socket�������������
//!
//! ʵ�ʴ����������������ʱ����ֶ�ʧ���Ͱ������
#define MAX_SOCKET_SEND_TASK_COUNT 10240


//! @def RECV_BUFFER_LENGTH
//! @brief ���ܻ��泤��
#define RECV_BUFFER_LENGTH 20480


//! @def NET_EVENT_POOL_COUNT
//! @brief net_event������л���Ķ������
#define NET_EVENT_POOL_COUNT 20000


//! @def NET_PACKET_POOL_COUNT
//! @brief net_packet������л���Ķ������
#define NET_PACKET_POOL_COUNT 20000


//! @def SOCK_STREAM_POOL_COUNT
//! @brief sock_stream������л���Ķ������
#define SOCK_STREAM_POOL_COUNT 10000


#endif // _CONFIG_H_
