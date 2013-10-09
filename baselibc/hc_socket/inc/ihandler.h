#ifndef _SOCK_HANDLER_H_
#define _SOCK_HANDLER_H_

#include "hc_os.h"
#include "address.h"

class net_package;
class ihandler;
class ireactor;

//! 事件处理器
class ihandler
{
public:
    //! 事件类型
    enum ev_mask_t {
        EM_NULL    = 0,
        EM_READ    = 1 << 0,    //!< 可读事件
        EM_WRITE   = 1 << 1,    //!< 可写事件

        EM_ALL     = EM_READ | EM_WRITE,
    };

public:
    virtual ~ihandler() { }

public:
    //! 打开
    virtual int open(const Address& dst_addr, int recv_bufsize, int send_bufsize) = 0;
    //! 关闭
    virtual void close() = 0;
    //! 获取 网络标示（通道id）
    virtual int32_t handler_o() = 0;
    //! 设置 网络标示（通道id）
    virtual void handler_o(int32_t net_id) = 0;
    //! 获取 socket Descriptor 描述符
    virtual Descriptor descriptor_o() = 0;
    //! 设置 socket Descriptor 描述符
    virtual void descriptor_o(Descriptor sock) = 0;
    //! 获取 event mask 事件关注信息
    virtual int32_t event_mask_o() = 0;
    //! 获取 event mask 事件关注信息
    virtual void event_mask_o(int32_t ev_mask) = 0;
    //! 获取 反应器
    virtual ireactor* reactor_o() = 0;
    //! 设置 反应器
    virtual void reactor_o(ireactor* react) = 0;

    //! 处理读
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_input() = 0;
    //! 处理写
    //! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
    virtual int handle_output() = 0;
    //! 连接关闭  event_type = net_event::NE_CLOSE
    //! 连接异常  event_type = net_event::NE_EXCEPTION
    //! 超时      event_type = net_event::NE_TIMEOUT
    virtual int handle_close(int16_t event_type) = 0;
    //! 提交发送任务
    //! @param netpkg 待发送的任务
    virtual int post_package(net_package* netpkg) = 0;
};

#endif // _SOCK_HANDLER_H_
