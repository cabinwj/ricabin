//! @file epoll_reactor.h
#ifndef _EPOLL_REACTOR_H_
#define _EPOLL_REACTOR_H_

#ifndef WIN32

#include "hc_os.h"
#include "ireactor.h"
#include "ihandler.h"


class ihandler;

//! @class epoll_reactor
//! @brief EPOLL版本的反应器
class epoll_reactor : public ireactor
{
public:
    epoll_reactor();
    virtual ~epoll_reactor();

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
    virtual int enable_handler(ihandler* eh, uint32_t masks);

    //! 删除一个事件监听
    //! @param event_handler 事件处理器
    //! @param masks 要删除的事件
    //! @return 0:成功, <0:失败
    virtual int disable_handler(ihandler* eh, uint32_t masks);

private:
    //! 最大句柄数(20万)
    static const int MAX_HANDLER = 200000;
    //! epoll描述符
    int m_epfd;
    //! epoll返回的事件
    epoll_event* m_events;
};
#endif

#endif // _EPOLL_REACTOR_H_
