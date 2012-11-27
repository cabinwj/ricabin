#ifndef _COMMON_SYNC_QUEUE_H_
#define _COMMON_SYNC_QUEUE_H_

#include "common_condition.h"
#include "common_os.h"

#include <list>

template<class T>
class sync_queue
{
public:
    enum {
        DEFAULT_LIMIT = 1000
    };

public:
    sync_queue(size_t limit = DEFAULT_LIMIT): m_queue_limit_(limit)
    {
        m_connditionc_.init();
    }

    virtual ~sync_queue()
    {
    }

public:
    virtual size_t size()
    {
        m_connditionc_.lock();
        size_t size = m_queue_list_.size();
        m_connditionc_.unlock();
        return size;
    }

    int push(const T& value)
    {
        m_connditionc_.lock();
        if ( m_queue_list_.size() > m_queue_limit_ )
        {
            m_connditionc_.unlock();
            return -1;
        }

        m_queue_list_.push_back(value);

        m_connditionc_.broadcast();
        m_connditionc_.unlock();
        return 0;
    }

    int pop(T& value, bool block)
    {
        m_connditionc_.lock();
        while (true)
        {
            if (!m_queue_list_.empty())
            {
                value = m_queue_list_.front();
                m_queue_list_.pop_front();
                m_connditionc_.unlock();
                return 0;
            }

            if (!block)
            {
                m_connditionc_.unlock();
                return -1;
            }

            m_connditionc_.wait();
        }
    }

private:
    conditionc m_connditionc_;

    std::list<T> m_queue_list_;
    size_t  m_queue_limit_;
};

#endif
