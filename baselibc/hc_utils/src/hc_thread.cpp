#include "hc_thread.h"
#include "hc_types.h"
#include "hc_stack_trace.h"

// class threadc
threadc::threadc()
{
    STACK_TRACE_LOG();
}

threadc::~threadc()
{
    STACK_TRACE_LOG();
}

int threadc::activate()
{
    STACK_TRACE_LOG();

#ifdef WIN32
    m_thread_ = (HANDLE)_beginthreadex(NULL, 0, thread_proc, (void*)this, 0, NULL);
    if (0L == m_thread_) {
        return -1;
    }
#else
    int rc = pthread_create(&m_thread_, NULL, thread_proc, (void*)this);
    if (0 != rc) {
        return rc;
    }
#endif
    return 0;
}

int threadc::wait()
{
    STACK_TRACE_LOG();

#ifdef WIN32
    WaitForSingleObject(m_thread_, INFINITE);
    CloseHandle(m_thread_);
#else
    void *value_ptr;
    int rc = pthread_join(m_thread_, &value_ptr);
    if (0 != rc) {
        return rc;
    }
#endif
    return 0;
}

#ifdef WIN32
unsigned __stdcall threadc::thread_proc(void* param)
{
    STACK_TRACE_LOG();

    threadc* pthis = (threadc*)param;

    int code = pthis->svc();

    return (unsigned)code;
}
#else
void* threadc::thread_proc(void* param)
{
    STACK_TRACE_LOG();

    threadc* pthis = (threadc*)param;

    intptr_t code = pthis->svc();

    return (void*)code;
}
#endif
