#ifndef _COMMON_THREAD_POOL_H_
#define _COMMON_THREAD_POOL_H_

#include "common_os.h"
#include "common_types.h"
#include "common_non_copyable.h"
#include "common_thread.h"
#include "common_thread_mutex.h"
#include <list>

class threadc;
class job_threadc;

// NOTICE: threadc_pool �������ڶ���߳��б�ͬʱ�ٿ�
class threadc_pool : public non_copyable
{
public:
    threadc_pool();
    virtual ~threadc_pool();

public:
    int start(size_t count);
    void stop(size_t count);
    void stop_all();
    size_t thread_count();

protected:
    virtual bool run_once();
    virtual void stop_threads(size_t stopcount);

protected:
    threadc* create_thread();
    void destroy_thread(threadc* thread);

protected:
    friend class job_threadc;

protected:
    typedef std::list<threadc*> thread_pointer_list;

protected:
    thread_pointer_list m_live_threads_;
    thread_pointer_list m_dead_threads_;

    volatile size_t    m_stop_count_;

    //! �߳���, ���������� handler_list
    threadc_mutex m_mutex_;
};

#endif // _COMMON_THREAD_POOL_H_
