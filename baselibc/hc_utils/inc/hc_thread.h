//! �߳�
#ifndef _HC_THREAD_H_
#define _HC_THREAD_H_

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

//! @class threadc
//! �߳���
class threadc
{
public:
    threadc();
    virtual ~threadc();

public:
    //! �����߳�
    int activate();

    //! �ȴ��߳�
    int wait();

    //! �̺߳���
    virtual int svc() = 0;

private:
    //! �̺߳���
#ifdef WIN32
    static unsigned __stdcall thread_proc(void* param);
#else
    static void* thread_proc(void* param);
#endif

private:
    //! �߳�id
#ifdef WIN32
    HANDLE m_thread_;
#else
    pthread_t m_thread_;
#endif
};


#endif // _HC_THREAD_H_
