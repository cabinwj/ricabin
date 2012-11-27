//! @file thread_mutex.h
//! @brief 线程锁
#ifndef _THREAD_MUTEX_H_
#define _THREAD_MUTEX_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #include <pthread.h>
#endif

//! @class threadc_mutex
#ifndef _MULTI_THREAD
//! @一个空互斥锁
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
    threadc_mutex();
    ~threadc_mutex();

    //! 加锁
    //! @return 0:成功, <0:失败
    int acquire();

    //! 解锁
    //! @return 0:成功, <0:失败
    int release();

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
//!
//! 此类在构造函数加锁, 析构函数解锁
class threadc_mutex_guard
{
public:
    //! 构造函数
    //! @param mutex 用到的线程锁
    threadc_mutex_guard(threadc_mutex& mutex);

    //! 析构函数
    ~threadc_mutex_guard();

private:
    threadc_mutex& m_mutex_;
};

#endif // _THREAD_MUTEX_H_
