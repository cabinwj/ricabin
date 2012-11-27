/// @file condition.h
#ifndef _COMMON_CONDITION_H_
#define _COMMON_CONDITION_H_


#ifndef _MULTI_THREAD
class condition_impl;

/**
    条件变量的跨平台实现
    内部集成了线程锁mutex

    - 消费者：
    <CODE>
        conditionc & cond;
        while (true) {
            cond.lock();
            cond.wait();
            // 获取产品
            cond.unlock();
            // 其他处理
        }
    </CODE>

    - 生产者：
    <CODE>
        conditionc & cond;
        cond.lock();
        // 提交产品
        cond.signal();
        // 或者cond.broadcast();
        cond.unlock(); // Win32平台，锁必须在后面释放
    </CODE>
    */
class conditionc
{
public:
    conditionc() { }

    ~conditionc() { }

public:
    // 初始化
    int init() { return 0; }

    /// 获取锁
    int lock() { return 0; }

    /// 释放锁
    int unlock() { return 0; }

    /// 等待条件变量，死等
    int wait() { return 0; }

    /// 等待条件变量，带超时
    int timed_wait(int msec) { return 0; }

    /// 单播信号，唤醒一个等待线程
    int signal() { return 0; }

    /// 广播信号，唤醒所有等待线程
    int broadcast() { return 0; }

private:
    condition_impl* m_condition_impl_;
};
#else

class condition_impl;

/**
    条件变量的跨平台实现
    内部集成了线程锁mutex

    - 消费者：
    <CODE>
        conditionc & cond;
        while (true) {
            cond.lock();
            cond.wait();
            // 获取产品
            cond.unlock();
            // 其他处理
        }
    </CODE>

    - 生产者：
    <CODE>
        conditionc & cond;
        cond.lock();
        // 提交产品
        cond.signal();
        // 或者cond.broadcast();
        cond.unlock(); // Win32平台，锁必须在后面释放
    </CODE>
    */
class conditionc
{
public:
    conditionc();

    ~conditionc();

public:
    // 初始化
    int init();

    /// 获取锁
    int lock();

    /// 释放锁
    int unlock();

    /// 等待条件变量，死等
    int wait();

    /// 等待条件变量，带超时
    int timed_wait(int msec);

    /// 单播信号，唤醒一个等待线程
    int signal();

    /// 广播信号，唤醒所有等待线程
    int broadcast();

private:
    condition_impl* m_condition_impl_;
};

#endif

#endif // _COMMON_CONDITION_H_
