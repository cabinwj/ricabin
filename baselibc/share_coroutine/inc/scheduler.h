#ifndef _CORO_SCHEDULER_H_
#define _CORO_SCHEDULER_H_

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#include <winsock2.h>
#endif

extern "C" {
#include "coro.h"
}

#include "common_list.h"
#include "coroutine.h"

class coroutine;

// NOTE: 协程的调度总是 一般的协程 和 主协程 之间的依次切换。

class coro_scheduler
{
public:
    // 初始化 协程任务队列
    static void init();

    // 从任务队列中遍历协程
    static void schedule();

    // 从主协程切出去，当前协程切到协程next
    static void switch_to(coroutine* next);

    // 从当前协程切回主协程
    static void yield();

    // 将当前协程coro列入任务调度队列
    static void push_coro(coroutine* coro);

    // 获取当前协程
    static coroutine* current_coro(){ return m_current_coro_; }

    // 任务调度队列中的协程数
    static int32_t coro_count() { return m_coro_count_; }

    // 活跃协程数(未僵死的)
    static int32_t active_coro_count() { return m_active_coro_count_; }

private:
    // 任务队列列表
    static list_head m_tasks_list_;

    // 任务队列中协程的个数
    static int32_t m_coro_count_;

    // 活跃协程数(未僵死的)
    static int32_t m_active_coro_count_;

    // 当前协程
    static coroutine* m_current_coro_;

    // 主协程“现场”
#ifdef WIN32
    static LPVOID m_main_fiber_;
#else
    static coro_context* m_main_coro_context_;
#endif
};

#endif // _CORO_SCHEDULER_H_
