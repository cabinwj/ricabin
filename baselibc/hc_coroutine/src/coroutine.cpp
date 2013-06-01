#include "coroutine.h"
#include "scheduler.h"

#include "hc_log.h"
#include "hc_stack_trace.h"

coro_context* coro_pool::allocate(size_t size)
{
    STACK_TRACE_LOG();

    std::map<size_t, std::list<coro_context*> >::iterator it = m_coro_pool_.find(size);
    if (it == m_coro_pool_.end() || it->second.empty())
    {
        char* mem = new char[size];
        return (coro_context*)mem;
    }

    coro_context* coro = it->second.back();
    it->second.pop_back();
    return coro;
}

void coro_pool::deallocate(coro_context* coro, size_t size)
{
    STACK_TRACE_LOG();

    if (0 == size)
    {
        delete[] (char*)coro;
    }
    else
    {
        m_coro_pool_[size].push_back(coro);
    }
}

coro_pool* coro_pool::instance()
{
    static coro_pool __pool;
    return &__pool;
}

coroutine::coroutine(size_t stack_size) : m_reference_count_(1),
                                          m_is_zombie_(false), m_is_scheduling_(false), m_stack_size_(stack_size),
                                          m_sequence_id_(0),/* m_net_id_(0),*/ m_io_result_(NULL)
{
    STACK_TRACE_LOG();

    INIT_LIST_HEAD(&m_tasks_item_);
    INIT_LIST_HEAD(&m_net_item_);
    INIT_LIST_HEAD(&m_sequence_item_);

#ifdef WIN32
    m_fiber_ = CreateFiber(m_stack_size_, coroutine::proc, (void*)this);
#else
    m_coro_context_ = coro_pool::instance()->allocate(m_stack_size_);
    coro_create(m_coro_context_, &coroutine::proc, (void*)this,
                (char*)m_coro_context_ + sizeof(coro_context),
                m_stack_size_ - sizeof(coro_context));
#endif

#ifdef WIN32
    LOG(TRACE)("construct coroutine object: %p, create and start coroutine: %p", this, m_fiber_);
#else
    LOG(TRACE)("construct coroutine object: %p, create and start coroutine: %p", this, m_coro_context_);
#endif
}

coroutine::~coroutine()
{
    STACK_TRACE_LOG();

#ifdef WIN32
    LOG(TRACE)("free coroutine: %p, coro: %p", this, m_fiber_);
#else
    LOG(TRACE)("free coroutine: %p, coro: %p", this, m_coro_context_);
#endif

#ifdef WIN32
    if (m_fiber_)
    {
        DeleteFiber(m_fiber_);
    }
#else
    if (m_coro_context_)
    {
        coro_pool::instance()->deallocate(m_coro_context_, m_stack_size_);
    }
#endif
}

#ifdef WIN32
VOID WINAPI coroutine::proc(PVOID ctx)
#else
void coroutine::proc(void* ctx)
#endif
{
    STACK_TRACE_LOG();

    coroutine* coro = (coroutine*)ctx;

#ifdef WIN32
    LOG(TRACE)("coroutine proc coro: %p", coro->m_fiber_);
#else
    LOG(TRACE)("coroutine proc coro: %p", coro->m_coro_context_);
#endif

    // Call() 封装了 用户自定义的应用函数
    coro->Call();

    // 执行后，设置僵死
    coro->set_zombie(true);

    // 放入任务队列
    coro_scheduler::push_coro(coro);

    // 从当前协程切换回主协程
    coro_scheduler::yield();
}

