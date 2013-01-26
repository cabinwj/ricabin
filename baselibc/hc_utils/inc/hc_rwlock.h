#ifndef _COMMON_RWLOCK_H_
#define _COMMON_RWLOCK_H_

/**
  * ��д���Ŀ�ƽ̨ʵ��
  */
class rwlock_impl;
class rw_lockc
{
public:
    rw_lockc();
    ~rw_lockc();

public:
    /// ֧���Զ��ͷŵĶ���
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

    /// ֧���Զ��ͷŵ�д��
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
    // ��ʼ��
    int init();

    /// ��ȡ����
    int read_lock();

    /// ��ȡд��
    int write_lock();

    /// �ͷŶ���
    int read_unlock();

    /// �ͷ�д��
    int write_unlock();

private:
    rwlock_impl* impl_;
};

/// ֧���Զ��ͷŵĶ���
class read_lock_guard
{
public:
    //! ���캯��
    //! @param lock �õ��Ķ�д��
    read_lock_guard(rw_lockc& lock) : m_lock_(lock)
    {
        m_lock_.read_lock();
    }

    //! ��������
    ~read_lock_guard()
    {
        m_lock_.read_unlock();
    }

private:
    rw_lockc& m_lock_;
};

/// ֧���Զ��ͷŵ�д��
class write_lock_guard
{
public:
    //! ���캯��
    //! @param lock �õ��Ķ�д��
    write_lock_guard(rw_lockc& lock) : m_lock_(lock)
    {
        m_lock_.write_lock();
    }

    //! ��������
    ~write_lock_guard()
    {
        m_lock_.write_unlock();
    }

private:
    rw_lockc& m_lock_;
};


#endif // _COMMON_RWLOCK_H_
