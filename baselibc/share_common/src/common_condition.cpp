#include "common_condition.h"

#ifdef WIN32
#define NOMINMAX
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#include <assert.h>

#ifndef _MULTI_THREAD
//! @一个空条件锁
class condition_impl
{
    friend class conditionc;
    condition_impl(int& status) { }
    ~condition_impl() { }

    int lock() { return 0; }
    int unlock() { return 0; }
    int wait() { return 0; }
    int timed_wait(int msec) { return 0; }
    int signal() { return 0; }
    int broadcast() { return 0; }
};
#else
#ifdef WIN32
class condition_impl
{
    friend class conditionc;
    condition_impl(int & status)
        : m_sema_(NULL)
        , m_waiters_done_(NULL)
        , m_mutex_(NULL)
    {
        m_waiters_count_ = 0;
        m_was_broadcast_ = 0;
        ::InitializeCriticalSection(
            &m_waiters_count_lock_);
        m_sema_ = ::CreateSemaphore(
            NULL,       // no security
            0,          // initially 0
            0x7fffffff, // max count
            NULL); // unnamed
        m_waiters_done_ = CreateEvent (
            NULL,  // no security
            FALSE, // auto-reset
            FALSE, // non-signaled initially
            NULL); // unnamed
        m_mutex_ = ::CreateMutex(
            NULL,  // no security
            FALSE, // notinitial owner 
            NULL); // unnamed
        if (m_sema_ == NULL 
            || m_waiters_done_ == NULL 
            || m_mutex_ == NULL)
            status = -1;
    }

    ~condition_impl()
    {
        if (m_mutex_ != NULL)
            ::CloseHandle(m_mutex_);
        if (m_waiters_done_ != NULL)
            ::CloseHandle(m_waiters_done_);
        if (m_sema_ != NULL)
            ::CloseHandle(m_sema_);
        ::DeleteCriticalSection(&m_waiters_count_lock_);
    }

    int lock()
    {
        return ::WaitForSingleObject(m_mutex_, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
    }

    int unlock()
    {
        return ::ReleaseMutex(m_mutex_) ? 0 : -1;
    }

    int wait()
    {
        return timed_wait(INFINITE);
    }

    int timed_wait(int msec)
    {
        // Avoid race conditions.
        ::EnterCriticalSection (&m_waiters_count_lock_);
        m_waiters_count_++;
        ::LeaveCriticalSection (&m_waiters_count_lock_);

        // This call atomically releases the mutex and waits on the
        // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
        // are called by another thread.
        // 假设等待信号的操作时间比较长，其他的获取锁的操作应该都比较短
        ::SignalObjectAndWait (m_mutex_, m_sema_, msec, FALSE);

        // Reacquire lock to avoid race conditions.
        ::EnterCriticalSection (&m_waiters_count_lock_);

        // We're no longer waiting...
        m_waiters_count_--;

        // Check to see if we're the last waiter after <pthread_cond_broadcast>.
        int last_waiter = m_was_broadcast_ && m_waiters_count_ == 0;

        ::LeaveCriticalSection (&m_waiters_count_lock_);

        // If we're the last waiter thread during this particular broadcast
        // then let all the other threads proceed.
        if (last_waiter)
            // This call atomically signals the <waiters_done_> event and waits until
            // it can acquire the <external_mutex>.  This is required to ensure fairness. 
            ::SignalObjectAndWait (m_waiters_done_, m_mutex_, INFINITE, FALSE);
        else
            // Always regain the external mutex since that's the guarantee we
            // give to our callers. 
            ::WaitForSingleObject (m_mutex_, INFINITE);
        return 0;
    }

    int signal()
    {
        EnterCriticalSection (&m_waiters_count_lock_);
        int have_waiters = m_waiters_count_ > 0;
        LeaveCriticalSection (&m_waiters_count_lock_);

        // If there aren't any waiters, then this is a no-op.  
        if (have_waiters)
            ReleaseSemaphore (m_sema_, 1, 0);
        return 0;
    }

    int broadcast()
    {
        // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
        // consistent relative to each other.
        EnterCriticalSection (&m_waiters_count_lock_);
        int have_waiters = 0;

        if (m_waiters_count_ > 0) {
            // We are broadcasting, even if there is just one waiter...
            // Record that we are broadcasting, which helps optimize
            // <pthread_cond_wait> for the non-broadcast case.
            m_was_broadcast_ = 1;
            have_waiters = 1;
        }

        if (have_waiters) {
            // Wake up all the waiters atomically.
            ReleaseSemaphore (m_sema_, m_waiters_count_, 0);

            LeaveCriticalSection (&m_waiters_count_lock_);

            // Wait for all the awakened threads to acquire the counting
            // semaphore. 
            WaitForSingleObject(m_waiters_done_, INFINITE);
            // This assignment is okay, even without the <waiters_count_lock_> held 
            // because no other waiter threads can wake up to access it.
            m_was_broadcast_ = 0;
        }
        else
            LeaveCriticalSection (&m_waiters_count_lock_);
        return 0;
    }

    int m_waiters_count_;
    // Number of waiting threads.

    CRITICAL_SECTION m_waiters_count_lock_;
    // Serialize access to <waiters_count_>.

    HANDLE m_sema_;
    // Semaphore used to queue up threads waiting for the condition to
    // become signaled. 

    HANDLE m_waiters_done_;
    // An auto-reset event used by the broadcast/signal thread to wait
    // for all the waiting thread(s) to wake up and be released from the
    // semaphore. 

    size_t m_was_broadcast_;
    // Keeps track of whether we were broadcasting or signaling.  This
    // allows us to optimize the code if we're just signaling.

    HANDLE m_mutex_;
};
#else
class condition_impl
{
    friend class conditionc;
    condition_impl(int & status)
        : m_init_status_(0)
    {
        if (::pthread_mutex_init(&m_mutex_, NULL) == 0)
            m_init_status_ |= 1;
        if (::pthread_cond_init(&m_thread_cond_, NULL) == 0)
            m_init_status_ |= 2;
        if (m_init_status_ != 3)
            status = -1;
    }

    ~condition_impl()
    {
        if (m_init_status_ & 2)
            ::pthread_cond_destroy(&m_thread_cond_);
        if (m_init_status_ & 1)
            ::pthread_mutex_destroy(&m_mutex_);
    }

    int lock()
    {
        return ::pthread_mutex_lock(&m_mutex_);
    }

    int unlock()
    {
        return ::pthread_mutex_unlock(&m_mutex_);
    }

    int wait()
    {
        return ::pthread_cond_wait(&m_thread_cond_, &m_mutex_);
    }

    int timed_wait(int msec)
    {
        struct timespec ts;
        // 这里clock_gettime从rt库导出，链接时需要指定“-l rt”
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += msec / 1000;
        ts.tv_nsec += (msec % 1000) * 1000 * 1000;
        if (ts.tv_nsec >= 1000 * 1000 * 1000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000 * 1000 * 1000;
        }
        return ::pthread_cond_timedwait(&m_thread_cond_, &m_mutex_, &ts);
    }

    int signal()
    {
        return ::pthread_cond_signal(&m_thread_cond_);
    }

    int broadcast()
    {
        return ::pthread_cond_broadcast(&m_thread_cond_);
    }

    pthread_mutex_t m_mutex_;
    pthread_cond_t m_thread_cond_;
    int m_init_status_;
};
#endif
#endif

conditionc::conditionc()
    : m_condition_impl_(NULL)
{
}

conditionc::~conditionc()
{
    if (m_condition_impl_)
        delete m_condition_impl_;
}

int conditionc::init()
{
    int status = 0;
    m_condition_impl_ = new condition_impl(status);
    assert(m_condition_impl_);
    return status;
}

int conditionc::lock()
{
    return m_condition_impl_->lock();
}

int conditionc::unlock()
{
    return m_condition_impl_->unlock();
}

int conditionc::wait()
{
    return m_condition_impl_->wait();
}

int conditionc::timed_wait(int msec)
{
    return m_condition_impl_->timed_wait(msec);
}

int conditionc::signal()
{
    return m_condition_impl_->signal();
}

int conditionc::broadcast()
{
    return m_condition_impl_->broadcast();
}
