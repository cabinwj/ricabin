#include "hc_thread_pool.h"

#include <algorithm>

/**
* implement job_threadc
*/
class job_threadc : public threadc
{
public:
    job_threadc(threadc_pool* pool);

public:
    virtual int svc();

private:
    threadc_pool* m_thread_pool_;
};

job_threadc::job_threadc(threadc_pool* pool_) : m_thread_pool_(pool_) {}

int job_threadc::svc()
{
    while ( true )
    {
        bool rc = m_thread_pool_->run_once();
        if (!rc) // need to exit
        {
            break;
        }
    }
    m_thread_pool_->destroy_thread(this);

    return 0;
}

/**
    * implement threadc_pool
    */
threadc_pool::threadc_pool() : m_stop_count_(0)
{
}

threadc_pool::~threadc_pool()
{
}

threadc* threadc_pool::create_thread()
{
    return new job_threadc(this);
}

void threadc_pool::destroy_thread(threadc* thread)
{
    threadc_mutex_guard lock(&m_mutex_);

    thread_pointer_list::iterator iter = find(m_live_threads_.begin(),
                                             m_live_threads_.end(), thread);
    if ( iter != m_live_threads_.end() )
    {
        m_live_threads_.erase(iter);
    }

    m_dead_threads_.push_back(thread);
}

int threadc_pool::start(size_t count)
{
    size_t live_count = m_live_threads_.size();
    if ( live_count < count )
    {
        while ( live_count < count )
        {
            threadc* thread = create_thread();
            if ( 0 != thread->activate() )
            {
                return -1;
            }
            m_live_threads_.push_back(thread);
            ++live_count;
        }
    }
    else if ( live_count > count )
    {
        stop(live_count-count);
    }
    return 0;
}

void threadc_pool::stop(size_t count)
{
    if ( 0 == count )
    {
        return;
    }
    size_t stop_count = ::std::min(count, m_live_threads_.size());
    stop_threads(stop_count);
    // wait for exiting of all stop_count thread
    while ( true )
    {
        sleep_ms(20);

        threadc_mutex_guard lock(&m_mutex_);

        if ( stop_count <= m_dead_threads_.size() )
        {
            break;
        }
    }

    for ( thread_pointer_list::iterator iter = m_dead_threads_.begin();
            iter != m_dead_threads_.end(); ++iter )
    {
        (*iter)->wait();
        delete *iter;
    }

    m_dead_threads_.clear();
    return;
}

void threadc_pool::stop_all()
{
    stop(m_live_threads_.size());
}

size_t threadc_pool::thread_count()
{
    return m_live_threads_.size();
}

bool threadc_pool::run_once()
{
    while ( true )
    {
        sleep_ms(1);
        if ( m_stop_count_ > 0 )
        {
            threadc_mutex_guard lock(&m_mutex_);

            if ( this->m_stop_count_ > 0 )
            {
                --m_stop_count_;
                break;
            }
        }
    }
    return false;
}

void threadc_pool::stop_threads(size_t stop_count)
{
    this->m_stop_count_ = stop_count;
}

