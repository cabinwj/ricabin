#include "common_thread_mutex.h"


#ifdef _MULTI_THREAD
// class threadc_mutex
threadc_mutex::threadc_mutex()
{
#ifdef WIN32
    InitializeCriticalSection(&m_section_);
#else
    pthread_mutex_init(&m_mutex_, NULL);
#endif
}

threadc_mutex::~threadc_mutex()
{
#ifdef WIN32
    DeleteCriticalSection(&m_section_);
#else
    pthread_mutex_destroy(&m_mutex_);
#endif
}

int threadc_mutex::acquire()
{
#ifdef WIN32
    EnterCriticalSection(&m_section_);
#else
    int rc = pthread_mutex_lock(&m_mutex_);
    if (0 != rc) {
        return -1;
    }
#endif
    return 0;
}

int threadc_mutex::release()
{
#ifdef WIN32
    LeaveCriticalSection(&m_section_);
#else
    int rc = pthread_mutex_unlock(&m_mutex_);
    if (0 != rc) {
        return -1;
    }
#endif
    return 0;
}
#endif

// class threadc_mutex_guard
threadc_mutex_guard::threadc_mutex_guard(threadc_mutex& mutex) : m_mutex_(mutex)
{
    m_mutex_.acquire();
}

threadc_mutex_guard::~threadc_mutex_guard()
{
    m_mutex_.release();
}

