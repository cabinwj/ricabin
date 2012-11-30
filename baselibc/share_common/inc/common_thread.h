//! @file thread.h
//! @brief 线程
#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

//! @class threadc
//! @brief 线程类
class threadc
{
public:
    threadc();
    virtual ~threadc();

public:
    //! 启动线程
    int activate();

    //! 等待线程
    int wait();

    //! 线程函数
    virtual int svc() = 0;

private:
    //! 线程函数
#ifdef WIN32
    static unsigned __stdcall thread_proc(void* param);
#else
    static void* thread_proc(void* param);
#endif

private:
    //! 线程id
#ifdef WIN32
    HANDLE m_thread_;
#else
    pthread_t m_thread_;
#endif
};


#endif // _THREAD_H_
