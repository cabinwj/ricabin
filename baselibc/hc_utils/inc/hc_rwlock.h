#ifndef _COMMON_RWLOCK_H_
#define _COMMON_RWLOCK_H_

/**
  * 读写锁的跨平台实现
  */
class rwlock_impl;
class rw_lockc
{
public:
    rw_lockc();
    ~rw_lockc();

public:
    /// 支持自动释放的读锁
    class read_lockc
    {
    public:
        read_lockc(rw_lockc& lock)
            : m_lock_(lock)
        {
            m_lock_.read_lock();
        }

        ~read_lockc()
        {
            m_lock_.read_unlock();
        }

    private:
        rw_lockc& m_lock_;
    };

    /// 支持自动释放的写锁
    class write_lockc
    {
    public:
        write_lockc(rw_lockc& lock)
            : m_lock_(lock)
        {
            m_lock_.write_lock();
        }

        ~write_lockc()
        {
            m_lock_.write_unlock();
        }

    private:
        rw_lockc& m_lock_;
    };

public:
    read_lockc read_locker()
    {
        return read_lockc(*this);
    }

    write_lockc write_locker()
    {
        return write_lockc(*this);
    }

public:
    // 初始化
    int init();

    /// 获取读锁
    int read_lock();

    /// 获取写锁
    int write_lock();

    /// 释放读锁
    int read_unlock();

    /// 释放写锁
    int write_unlock();

private:
    rwlock_impl* impl_;
};

/// 支持自动释放的读锁
class read_lock_guard
{
public:
    //! 构造函数
    //! @param lock 用到的读写锁
    read_lock_guard(rw_lockc& lock) : m_lock_(lock)
    {
        m_lock_.read_lock();
    }

    //! 析构函数
    ~read_lock_guard()
    {
        m_lock_.read_unlock();
    }

private:
    rw_lockc& m_lock_;
};

/// 支持自动释放的写锁
class write_lock_guard
{
public:
    //! 构造函数
    //! @param lock 用到的读写锁
    write_lock_guard(rw_lockc& lock) : m_lock_(lock)
    {
        m_lock_.write_lock();
    }

    //! 析构函数
    ~write_lock_guard()
    {
        m_lock_.write_unlock();
    }

private:
    rw_lockc& m_lock_;
};


#endif // _COMMON_RWLOCK_H_
