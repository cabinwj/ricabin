//! �߳���
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
//! @brief ������
class threadc_mutex;

#ifndef _MULTI_THREAD
//! @brief �ջ�����
class threadc_mutex
{
public:
    threadc_mutex() { }
    ~threadc_mutex() { }

    //! ����
    //! @return 0:�ɹ�, <0:ʧ��
    int acquire() { return 0; }
    //! ����
    //! @return 0:�ɹ�, <0:ʧ��
    int release() { return 0; }
};
#else
//! @brief �߳���
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

    //! ����
    //! @return 0:�ɹ�, <0:ʧ��
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

    //! ����
    //! @return 0:�ɹ�, <0:ʧ��
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
    //! �߳���
#ifdef WIN32
    CRITICAL_SECTION m_section_;
#else
    pthread_mutex_t m_mutex_;
#endif
};
#endif

//! @class threadc_mutex_guard
//! @brief �߳���������
//! �����ڹ��캯������, ������������
class threadc_mutex_guard
{
public:
    //! ���캯��
    //! @param mutex �õ����߳���
    threadc_mutex_guard(threadc_mutex* mutex) : m_mutex_(mutex)
    {
        m_mutex_->acquire();
    }

    //! ��������
    ~threadc_mutex_guard()
    {
        m_mutex_->release();
    }

private:
    threadc_mutex* m_mutex_;
};

#endif // _HC_THREAD_MUTEX_H_
