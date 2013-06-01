#include "coro_timer_heap.h"
#include "hc_stack_trace.h"


min_heap timer_min_heap::m_min_heap_;


int timer_min_heap::enable_timer(min_helem* timer)
{
    STACK_TRACE_LOG();

    return m_min_heap_.push(timer);
}

int timer_min_heap::disable_timer(min_helem* timer)
{
    STACK_TRACE_LOG();

    return m_min_heap_.erase(timer);
}

void timer_min_heap::scan_timer()
{
    STACK_TRACE_LOG();

    while (true)
    {
        min_helem* timer = m_min_heap_.pop();
        if (NULL != timer)
        {
            return;
        }

        // 回调，换醒异步阻塞协程
        timer->Call();

        delete timer;
        timer = NULL;
    }
}

