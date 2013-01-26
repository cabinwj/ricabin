#include "scheduler.h"
#include "coroutine.h"

#include "hc_log.h"

list_head coro_scheduler::m_tasks_list_;
int32_t coro_scheduler::m_coro_count_ = 0;
int32_t coro_scheduler::m_active_coro_count_ = 0;
coroutine* coro_scheduler::m_current_coro_ = NULL;

#ifdef WIN32
LPVOID coro_scheduler::m_main_fiber_;
#else
coro_context* coro_scheduler::m_main_coro_context_;
#endif

void coro_scheduler::init()
{
    INIT_LIST_HEAD(&m_tasks_list_);
    m_coro_count_ = 0;
    m_current_coro_ = NULL;

#ifdef WIN32
    m_main_fiber_ = ConvertThreadToFiber(NULL);
#else
    m_main_coro_context_ = new coro_context;
#endif
}

void coro_scheduler::schedule()
{
    list_head* pos;
    list_head* next;
    coroutine* coro = NULL;
    list_for_each_safe(pos, next, &m_tasks_list_)
    {
        coro = list_entry(pos, coroutine, m_tasks_item_);

#ifdef WIN32
        LOG(TRACE)("schedule coroutine: %p", coro->m_fiber_);
#else
        LOG(TRACE)("schedule coroutine: %p", coro->m_coro_context_);
#endif

        // �Ƴ��������
        list_del(&coro->m_tasks_item_);

        // ���õ�ǰЭ�����Ƴ��������״̬
        coro->set_scheduling(false);

        --m_coro_count_;

        // ���ֽ�������ֱ��ɾ��
        if ( coro->is_zombie() )
        {
            --m_active_coro_count_;
            coro->release();
            continue;
        }

        // Э�̵��ȣ�����Э���г�ȥ
        switch_to(coro);
    }
}

void coro_scheduler::switch_to(coroutine* next)
{
    m_current_coro_ = next;

#ifdef WIN32
    LOG(TRACE)("switch from main coroutine: %p to coroutine: %p", m_main_fiber_, m_current_coro_->m_fiber_);
#else
    LOG(TRACE)("switch from main coroutine: %p to coroutine: %p", m_main_coro_context_, m_current_coro_->m_coro_context_);
#endif

#ifdef WIN32
    SwitchToFiber(m_current_coro_->m_fiber_);
#else
    coro_transfer(m_main_coro_context_, m_current_coro_->m_coro_context_);
#endif
}

void coro_scheduler::push_coro(coroutine* coro)
{
    // ������������У��򲻲����������
    if ( coro->is_scheduling() )
    {
#ifdef WIN32
        LOG(TRACE)("push coroutine: %p but already in", coro->m_fiber_);
#else
        LOG(TRACE)("push coroutine: %p but already in", coro->m_coro_context_);
#endif
        return;
    }

    // �����������
    list_add_tail(&coro->m_tasks_item_, &m_tasks_list_);

    // ���õ�ǰЭ�������������״̬
    coro->set_scheduling(true);

    ++m_coro_count_;

    if ( !coro->is_zombie() )
    {
        ++m_active_coro_count_;
    }

#ifdef WIN32
    LOG(TRACE)("push coroutine: %p success", coro->m_fiber_);
#else
    LOG(TRACE)("push coroutine: %p success", coro->m_coro_context_);
#endif
}

void coro_scheduler::yield()
{
#ifdef WIN32
    LOG(TRACE)("switch from coroutine: %p to main coroutine: %p", m_current_coro_->m_fiber_, m_main_fiber_);
#else
    LOG(TRACE)("switch from coroutine: %p to main coroutine: %p", m_current_coro_->m_coro_context_, m_main_coro_context_);
#endif

#ifdef WIN32
    SwitchToFiber(m_main_fiber_);
#else
    coro_transfer(m_current_coro_->m_coro_context_, m_main_coro_context_);
#endif
}
