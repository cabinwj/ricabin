#ifndef _REACTOR_H_
#define _REACTOR_H_

#include "common_types.h"

class event_handler;

//! @class reactor
//! @brief 反应器模型
class reactor
{
public:
    virtual ~reactor() { }

public:
    //! 打开reactor
    //! @return 0:成功, <0:失败
    virtual int open_reactor() = 0;

    //! 关闭reactor
    //! @return 0:成功, <0:失败
    virtual int close_reactor() = 0;

    //! 事件主循环
    //! @return >=0:处理事件的个数, <0:反应器错误
    virtual int run_reactor_event_loop() = 0;

    //! 停止事件主循环并清理所有注册的处理器
    virtual int end_reactor_event_loop() = 0;

    //! 注册一个事件监听
    //! @param event_handler 事件处理器
    //! @param masks 要监听的事件
    //! @return 0:成功, <0:失败
    virtual int enable_handler(event_handler* eh, uint32_t masks) = 0;

    //! 删除一个事件监听
    //! @param event_handler 事件处理器
    //! @param masks 要删除的事件
    //! @return 0:成功, <0:失败
    virtual int disable_handler(event_handler* eh, uint32_t masks) = 0;
};

#endif // _REACTOR_H_
