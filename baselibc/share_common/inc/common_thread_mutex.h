//! @file thread_mutex.h
//! @brief �߳���
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
//! @һ���ջ�����
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
    threadc_mutex();
    ~threadc_mutex();

    //! ����
    //! @return 0:�ɹ�, <0:ʧ��
    int acquire();

    //! ����
    //! @return 0:�ɹ�, <0:ʧ��
    int release();

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
//!
//! �����ڹ��캯������, ������������
class threadc_mutex_guard
{
public:
    //! ���캯��
    //! @param mutex �õ����߳���
    threadc_mutex_guard(threadc_mutex& mutex);

    //! ��������
    ~threadc_mutex_guard();

private:
    threadc_mutex& m_mutex_;
};

#endif // _THREAD_MUTEX_H_
