#ifndef _CORO_TIMER_HEAP_
#define _CORO_TIMER_HEAP_

#include "common_min_heap.h"

#include "coroutine.h"
#include "scheduler.h"

#include "protocol.h"


class coro_timer : public min_helem
{
public:
    coro_timer(int32_t timeout, coroutine* coro) : m_coro_(coro)
    {
        weight(timeout);
        m_coro_->add_reference();
    }

    virtual ~coro_timer()
    {
        m_coro_->release();
    }

public:
    virtual void Call()
    {
        if ( !m_coro_->is_zombie() )
        {
            coro_scheduler::switch_to(m_coro_);
        }
    }

private:
    coroutine* m_coro_;
};

class timer_min_heap
{
public:
    static int enable_timer(min_helem* timer);
    static int disable_timer(min_helem* timer);
    static void scan_timer();

private:
    static min_heap m_min_heap_;
};

#endif // _CORO_TIMER_HEAP_
