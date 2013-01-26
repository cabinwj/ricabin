//! @file config.h
//! @brief 系统各项参数定义
#ifndef _CONFIG_H_
#define _CONFIG_H_


//! @def MAX_PACKET_LENGTH
//! @brief 网络包最大长度
//!
//! 目前系统使用的包大小都不超过2048, 此处包数据缓存大小设为2048,
//! 开发新协议时注意包长度不要超过此处限制
#ifndef MAX_PACKET_LENGTH
#define MAX_PACKET_LENGTH 2048
#endif


//! @def MAX_NET_EVENT_COUNT
//! @brief 最大网络事件个数
//!
//! 实际未处理网络事件超过MAX_NET_EVENT_COUNT时会出现丢失网络事件的情况
#ifndef MAX_NET_EVENT_COUNT
#define MAX_NET_EVENT_COUNT 1024000
#endif


//! @def MAX_NET_SEND_TASK_COUNT
//! @brief 最大全局待发送任务个数
//!
//! 实际待发送任务个数超过时会出现丢失发送包的情况
#ifndef MAX_NET_SEND_TASK_COUNT
#define MAX_NET_SEND_TASK_COUNT 1024000
#endif


//! @def MAX_SOCKET_SEND_TASK_COUNT
//! @brief 最大单个socket待发送任务个数
//!
//! 实际待发送任务个数超过时会出现丢失发送包的情况
#define MAX_SOCKET_SEND_TASK_COUNT 10240


//! @def RECV_BUFFER_LENGTH
//! @brief 接受缓存长度
#define RECV_BUFFER_LENGTH 20480


//! @def NET_EVENT_POOL_COUNT
//! @brief net_event对象池中缓存的对象个数
#define NET_EVENT_POOL_COUNT 20000


//! @def NET_PACKET_POOL_COUNT
//! @brief net_packet对象池中缓存的对象个数
#define NET_PACKET_POOL_COUNT 20000


//! @def SOCK_STREAM_POOL_COUNT
//! @brief sock_stream对象池中缓存的对象个数
#define SOCK_STREAM_POOL_COUNT 10000


#endif // _CONFIG_H_
