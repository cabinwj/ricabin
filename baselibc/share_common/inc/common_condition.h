/// @file condition.h
#ifndef _COMMON_CONDITION_H_
#define _COMMON_CONDITION_H_


#ifndef _MULTI_THREAD
class condition_impl;

/**
    ���������Ŀ�ƽ̨ʵ��
    �ڲ��������߳���mutex

    - �����ߣ�
    <CODE>
        conditionc & cond;
        while (true) {
            cond.lock();
            cond.wait();
            // ��ȡ��Ʒ
            cond.unlock();
            // ��������
        }
    </CODE>

    - �����ߣ�
    <CODE>
        conditionc & cond;
        cond.lock();
        // �ύ��Ʒ
        cond.signal();
        // ����cond.broadcast();
        cond.unlock(); // Win32ƽ̨���������ں����ͷ�
    </CODE>
    */
class conditionc
{
public:
    conditionc() { }

    ~conditionc() { }

public:
    // ��ʼ��
    int init() { return 0; }

    /// ��ȡ��
    int lock() { return 0; }

    /// �ͷ���
    int unlock() { return 0; }

    /// �ȴ���������������
    int wait() { return 0; }

    /// �ȴ���������������ʱ
    int timed_wait(int msec) { return 0; }

    /// �����źţ�����һ���ȴ��߳�
    int signal() { return 0; }

    /// �㲥�źţ��������еȴ��߳�
    int broadcast() { return 0; }

private:
    condition_impl* m_condition_impl_;
};
#else

class condition_impl;

/**
    ���������Ŀ�ƽ̨ʵ��
    �ڲ��������߳���mutex

    - �����ߣ�
    <CODE>
        conditionc & cond;
        while (true) {
            cond.lock();
            cond.wait();
            // ��ȡ��Ʒ
            cond.unlock();
            // ��������
        }
    </CODE>

    - �����ߣ�
    <CODE>
        conditionc & cond;
        cond.lock();
        // �ύ��Ʒ
        cond.signal();
        // ����cond.broadcast();
        cond.unlock(); // Win32ƽ̨���������ں����ͷ�
    </CODE>
    */
class conditionc
{
public:
    conditionc();

    ~conditionc();

public:
    // ��ʼ��
    int init();

    /// ��ȡ��
    int lock();

    /// �ͷ���
    int unlock();

    /// �ȴ���������������
    int wait();

    /// �ȴ���������������ʱ
    int timed_wait(int msec);

    /// �����źţ�����һ���ȴ��߳�
    int signal();

    /// �㲥�źţ��������еȴ��߳�
    int broadcast();

private:
    condition_impl* m_condition_impl_;
};

#endif

#endif // _COMMON_CONDITION_H_
