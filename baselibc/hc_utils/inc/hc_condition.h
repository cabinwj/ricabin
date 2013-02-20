/// @file condition.h
#ifndef _HC_CONDITION_H_
#define _HC_CONDITION_H_

/**
    ���������Ŀ�ƽ̨ʵ��
    �ڲ��������߳���mutex

    - �����ߣ�
    <CODE>
        conditionc& cond;
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
        conditionc& cond;
        cond.lock();
        // �ύ��Ʒ
        cond.signal();
        // ����cond.broadcast();
        cond.unlock(); // Win32ƽ̨���������ں����ͷ�
    </CODE>
 */

class condition_impl;
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

#endif // _HC_CONDITION_H_
