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

// NOTE: Э�̵ĵ������� һ���Э�� �� ��Э�� ֮��������л���

class coro_scheduler
{
public:
    // ��ʼ�� Э���������
    static void init();

    // ����������б���Э��
    static void schedule();

    // ����Э���г�ȥ����ǰЭ���е�Э��next
    static void switch_to(coroutine* next);

    // �ӵ�ǰЭ���л���Э��
    static void yield();

    // ����ǰЭ��coro����������ȶ���
    static void push_coro(coroutine* coro);

    // ��ȡ��ǰЭ��
    static coroutine* current_coro(){ return m_current_coro_; }

    // ������ȶ����е�Э����
    static int32_t coro_count() { return m_coro_count_; }

    // ��ԾЭ����(δ������)
    static int32_t active_coro_count() { return m_active_coro_count_; }

private:
    // ��������б�
    static list_head m_tasks_list_;

    // ���������Э�̵ĸ���
    static int32_t m_coro_count_;

    // ��ԾЭ����(δ������)
    static int32_t m_active_coro_count_;

    // ��ǰЭ��
    static coroutine* m_current_coro_;

    // ��Э�̡��ֳ���
#ifdef WIN32
    static LPVOID m_main_fiber_;
#else
    static coro_context* m_main_coro_context_;
#endif
};

#endif // _CORO_SCHEDULER_H_
