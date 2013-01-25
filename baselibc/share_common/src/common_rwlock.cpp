#include "common_rwlock.h"

#ifdef WIN32
#define  _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <assert.h>

#ifndef _MULTI_THREAD
//! @一个空读写锁
class rwlock_impl
{
    friend class rw_lockc;
    rwlock_impl(int & status) { status = 0; }
    ~rwlock_impl(){ }

    int rdlock() { return 0; }

    int wrlock() { return 0; }

    int rdunlock() { return 0; }

    int wrunlock() { return 0; }
};

#else

#ifdef WIN32

#define IMPL    1

#if (IMPL == 1)

class rwlock_impl
{
    friend class rw_lockc;
    rwlock_impl(int & status)
        : num_reader_(0), event_no_writer_(NULL), event_no_reader_(NULL)
    {
        event_no_writer_ = ::CreateEvent(
            NULL,   // no security
            TRUE,   // manual-reset
            TRUE,   // signaled initially
            NULL);  // unnamed
        event_no_reader_ = ::CreateEvent(
            NULL,   // no security
            TRUE,   // manual-reset
            TRUE,   // signaled initially
            NULL);  // unnamed
        ::InitializeCriticalSection(
            &mutex_write_);
        ::InitializeCriticalSection(
            &mutex_read_);
        if (event_no_writer_ == NULL 
            || event_no_reader_ == NULL)
            status = -1;
    }

    ~rwlock_impl(void)
    {
        ::DeleteCriticalSection(&mutex_read_);
        ::DeleteCriticalSection(&mutex_write_);
        if (event_no_reader_ != NULL)
            ::CloseHandle(event_no_reader_);
        if (event_no_writer_ != NULL)
            ::CloseHandle(event_no_writer_);
    }

    int rdlock()
    {
        bool bLoop = true;
        while(bLoop) {
            // 等待写线程退出
            DWORD result = ::WaitForSingleObject(event_no_writer_, INFINITE);
            if (result != WAIT_OBJECT_0)
                return -1;
            IncrementReaderCount();
            // 再次检查，双保险
            if (::WaitForSingleObject(event_no_writer_, 0) != WAIT_OBJECT_0) {
                // 如果有写线程进入，读线程作出让步
                DecrementReaderCount();
            } else {
                bLoop = false;
            }
        }

        return 0;
    }

    int rdunlock()
    {
        DecrementReaderCount();
        return 0;
    }

    int wrlock()
    {
        // 加锁，只让一个写线程同时执行这段代码
        ::EnterCriticalSection(&mutex_write_);
        // 等待其他的写线程退出
        ::WaitForSingleObject(event_no_writer_, INFINITE);
        // 一旦写线程将要进入，就清除event_no_writer_，阻止后续读线程进入（即使已经有读线程）
        ::ResetEvent(event_no_writer_);
        // 等待已经进入的读线程都退出
        ::WaitForSingleObject(event_no_reader_, INFINITE);
        ::LeaveCriticalSection(&mutex_write_);
        return 0;
    }

    int wrunlock()
    {
        ::SetEvent(event_no_writer_);
        return 0;
    }

    int IncrementReaderCount()
    {
        ::EnterCriticalSection(&mutex_read_);
        num_reader_++;
        ::ResetEvent(event_no_reader_);
        ::LeaveCriticalSection(&mutex_read_);
        return 0;
    }

    void DecrementReaderCount()
    {
        ::EnterCriticalSection(&mutex_read_);
        num_reader_--;
        if (num_reader_ <= 0)
            SetEvent(event_no_reader_);
        ::LeaveCriticalSection(&mutex_read_);
    }

private:
    HANDLE event_no_writer_; ///< 没有写者的信号

    HANDLE event_no_reader_; ///< 没有读者的信号

    int num_reader_; ///< 读计数

    CRITICAL_SECTION mutex_write_; ///< 保护获取写锁的锁

    CRITICAL_SECTION mutex_read_;///< 保护读计数的锁
};

#else // IMPL = 1

class rwlock_impl
{
    friend class rw_lockc;
    rwlock_impl(int & status)
        : num_reader_(0)
        , mutex_access_(NULL)
        , mutex_write_(NULL)
        , event_write_(NULL)
        , event_read_(NULL)
    {
        mutex_access_ = ::CreateMutex(NULL, FALSE, NULL);
        mutex_write_ = ::CreateMutex(NULL, FALSE, NULL);
        event_write_ = ::CreateEvent(NULL, TRUE, TRUE, NULL);
        event_read_ = ::CreateEvent(NULL, TRUE, TRUE, NULL);
        if (mutex_access_ == NULL 
            || mutex_write_ == NULL 
            || event_write_ == NULL 
            || event_read_ == NULL)
            status = -1;
    }

    ~rwlock_impl()
    {
        if (event_read_ != NULL)
            ::CloseHandle(event_read_);
        if (event_write_ != NULL)
            ::CloseHandle(event_write_);
        if (mutex_write_ != NULL)
            ::CloseHandle(mutex_write_);
        if (mutex_access_ != NULL)
            ::CloseHandle(mutex_access_);
    }

    int rdlock()
    {
        HANDLE h[] = {mutex_access_, event_read_};
        DWORD result = ::WaitForMultipleObjects(2, h, TRUE, INFINITE);
        if (result != WAIT_OBJECT_0 + 1)
            return -1;
        ++num_reader_;
        ResetEvent(event_write_);
        ReleaseMutex(mutex_access_);
        return 0;
    }

    int wrlock()
    {
        HANDLE h[] = {mutex_access_, event_write_, mutex_write_};
        DWORD result = ::WaitForMultipleObjects(3, h, TRUE, INFINITE);
        if (result != WAIT_OBJECT_0 + 2)
            return -1;
        assert(num_reader_ == 0);
        ResetEvent(event_read_);
        ReleaseMutex(mutex_access_);
        return 0;
    }

    int rdunlock()
    {
        DWORD result = ::WaitForSingleObject(mutex_access_, INFINITE);
        if (result != WAIT_OBJECT_0)
            return -1;
        assert(num_reader_ > 0);
        --num_reader_;
        if (num_reader_ == 0)
            SetEvent(event_write_);
        ReleaseMutex(mutex_access_);
        return true;
    }

    int wrunlock()
    {
        DWORD result = ::WaitForSingleObject(mutex_access_, INFINITE);
        if (result != WAIT_OBJECT_0)
            return -1;
        assert(num_reader_ == 0);
        ReleaseMutex(mutex_write_);
        SetEvent(event_read_);
        SetEvent(event_write_);
        ReleaseMutex(mutex_access_);
        return true;
    }

    HANDLE mutex_access_;
    HANDLE mutex_write_;
    HANDLE event_write_;
    HANDLE event_read_;
    int num_reader_;
};

#endif // IMPL == 1

#else

class rwlock_impl
{
    friend class rw_lockc;
    rwlock_impl(int & status)
        : status_(0)
    {
        status_ = status = ::pthread_rwlock_init(&lock_, NULL);
    }

    ~rwlock_impl()
    {
        if (status_)
            ::pthread_rwlock_destroy(&lock_);
    }

    int rdlock()
    {
        return ::pthread_rwlock_rdlock(&lock_);
    }

    int wrlock()
    {
        return ::pthread_rwlock_wrlock(&lock_);
    }

    int rdunlock()
    {
        return ::pthread_rwlock_unlock(&lock_);
    }

    int wrunlock()
    {
        return ::pthread_rwlock_unlock(&lock_);
    }

    pthread_rwlock_t lock_;
    int status_;
};

#endif
#endif

rw_lockc::rw_lockc()
    : impl_(NULL)
{
}

rw_lockc::~rw_lockc()
{
    if (impl_)
        delete impl_;
}

int rw_lockc::init()
{
    int status = 0;
    impl_ = new rwlock_impl(status);
    return status;
}

int rw_lockc::read_lock()
{
    return impl_->rdlock();
}

int rw_lockc::write_lock()
{
    return impl_->wrlock();
}

int rw_lockc::read_unlock()
{
    return impl_->rdunlock();
}

int rw_lockc::write_unlock()
{
    return impl_->wrunlock();
}

