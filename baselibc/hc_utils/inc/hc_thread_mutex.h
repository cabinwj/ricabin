//! 线程锁
#ifndef _HC_THREAD_MUTEX_H_
#define _HC_THREAD_MUTEX_H_

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif


//! @class threadc_mutex
//! @brief 互斥锁
class threadc_mutex;

#ifndef _MULTI_THREAD
//! @brief 空互斥锁
class threadc_mutex
{
public:
    threadc_mutex() { }
    ~threadc_mutex() { }

    //! 加锁
    //! @return 0:成功, <0:失败
    int acquire() { return 0; }
    //! 解锁
    //! @return 0:成功, <0:失败
    int release() { return 0; }
};
#else
//! @brief 线程锁
class threadc_mutex
{
public:
    threadc_mutex()
    {
#ifdef WIN32
        InitializeCriticalSection(&m_section_);
#else
        pthread_mutex_init(&m_mutex_, NULL);
#endif
    }

    ~threadc_mutex()
    {
#ifdef WIN32
        DeleteCriticalSection(&m_section_);
#else
        pthread_mutex_destroy(&m_mutex_);
#endif
    }

    //! 加锁
    //! @return 0:成功, <0:失败
    int acquire()
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

    //! 解锁
    //! @return 0:成功, <0:失败
    int release()
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

private:
    //! 线程锁
#ifdef WIN32
    CRITICAL_SECTION m_section_;
#else
    pthread_mutex_t m_mutex_;
#endif
};
#endif

//! @class threadc_mutex_guard
//! @brief 线程锁工具类
//! 此类在构造函数加锁, 析构函数解锁
class threadc_mutex_guard
{
public:
    //! 构造函数
    //! @param mutex 用到的线程锁
    threadc_mutex_guard(threadc_mutex* mutex) : m_mutex_(mutex)
    {
        m_mutex_->acquire();
    }

    //! 析构函数
    ~threadc_mutex_guard()
    {
        m_mutex_->release();
    }

private:
    threadc_mutex* m_mutex_;
};

#endif // _HC_THREAD_MUTEX_H_
