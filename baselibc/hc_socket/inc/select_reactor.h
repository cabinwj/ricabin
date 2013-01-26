//! @file select_reactor.h
#ifndef _SELECT_REACTOR_H_
#define _SELECT_REACTOR_H_

#include "hc_os.h"
#include "reactor.h"
#include "event_handler.h"

class event_handler;

//! @class select_reactor
//! @brief Select版本的反应器
class select_reactor : public reactor
{
public:
    select_reactor();
    virtual ~select_reactor();

public:
    //! 打开reactor
    //! @return 0:成功, <0:失败
    virtual int open_reactor();

    //! 关闭reactor
    //! @return 0:成功, <0:失败
    virtual int close_reactor();

    //! 事件主循环
    //! @return >=0:处理事件的个数, <0:反应器错误
    virtual int run_reactor_event_loop();

    //! 停止事件主循环并清理所有注册的处理器
    virtual int end_reactor_event_loop();

    //! 注册一个事件监听
    //! @param event_handler 事件处理器
    //! @param masks 要监听的事件
    //! @return 0:成功, <0:失败
    virtual int enable_handler(event_handler* eh, uint32_t masks);

    //! 删除一个事件监听
    //! @param event_handler 事件处理器
    //! @param masks 要删除的事件
    //! @return 0:成功, <0:失败
    virtual int disable_handler(event_handler* eh, uint32_t masks);

private:
    fd_set m_read_set_;
    fd_set m_write_set_;
    fd_set m_exception_set_;

private:
    //! 上一次扫描超时的时间
    static time_t m_last_scan_time_;
};

#endif // _SELECT_REACTOR_H_
