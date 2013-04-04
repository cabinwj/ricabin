#ifndef _HC_OBJECT_ALLOCATOR_H_
#define _HC_OBJECT_ALLOCATOR_H_

#include "hc_allocator.h"
#include "hc_thread_mutex.h"

#include <list>
#include <vector>

template <class T>
class object_allocator
{
public:
    virtual ~object_allocator() {}
    virtual T* Create() = 0;
    virtual void Destroy(T* object) = 0;
};

template <class T>
class object_single_allocator : public object_allocator<T>
{
public:
    object_single_allocator(allocatorc* alloc = allocatorc::Instance()) : m_allocator_(alloc)
    {
        if ( NULL == m_allocator_ )
        {
            m_allocator_ = allocatorc::Instance(); 
        }
    }

public:
    virtual T* Create()
    {
        void* object = m_allocator_->Allocate(sizeof(T));
        if ( NULL == object )
        {
            return NULL;
        }

        return new (object)T;
    }

    virtual void Destroy(T* object)
    {
        object->~T();
        m_allocator_->Deallocate(object);
    }

private:
    allocatorc* m_allocator_;
};


template <class T>
class object_pool_allocator : public object_allocator<T>
{
public:
    object_pool_allocator(size_t init_cnt, size_t max_cnt = 0, size_t inc_cnt = 0,
                          allocatorc* alloc = allocatorc::Instance())
        : m_allocator_(alloc), m_current_count_(0),
        m_max_count_(max_cnt), m_increase_count_(inc_cnt)
    {
        if ( NULL == m_allocator_ )
        {
            m_allocator_ = allocatorc::Instance(); 
        }

        if ( m_max_count_ < init_cnt )
        {
            m_max_count_ = init_cnt;
            m_increase_count_ = 0;
        }

        increase_objects(init_cnt);
    }

    virtual ~object_pool_allocator()
    {
        typename::std::list<T*>::iterator lpos;
        for ( lpos = m_object_list_.begin(); lpos != m_object_list_.end(); ++lpos )
        {
            (*lpos)->~T();
        }

        typename::std::vector<T*>::iterator vpos;
        for ( vpos = m_chunk_list_.begin(); vpos != m_chunk_list_.end(); ++vpos )
        {
            m_allocator_->Deallocate(*vpos);
        }
    }

    inline void max_count(size_t max_cnt)
    {
        if ( m_current_count_ >= max_cnt )
        {
            m_max_count_ = m_current_count_;
        }
        else
        {
            m_max_count_ = max_cnt;
        }
    }

    inline size_t max_count() const { return m_max_count_; }

    inline void increase_count(size_t inc_count) { m_increase_count_ = inc_count; }

    inline size_t increase_count() const { return m_increase_count_; }

    inline size_t current_count() const { return m_current_count_; }

    inline bool is_init() const { return m_current_count_ > 0; }

    inline bool is_full() const { return m_current_count_ >= m_max_count_; }

    virtual T* Create()
    {
        threadc_mutex_guard lock(&m_mutex_);

        if ( m_object_list_.empty() )
        {
            if ( increase_objects(m_increase_count_) != 0 )
            {
                return NULL;
            }
        }

        T* object = *(m_object_list_.begin());
        m_object_list_.pop_front();

        return object;
    }

    virtual void Destroy(T* object)
    {
        threadc_mutex_guard lock(&m_mutex_);

        if ( NULL == object )
        {
            return;
        }

        m_object_list_.push_front(object);
    }

private:
    int increase_objects(size_t inc_count)
    {
        if ( m_current_count_ >= m_max_count_ )
        {
            return -1;
        }

        if ( m_current_count_ + inc_count > m_max_count_ )
        {
            inc_count = m_max_count_ - m_current_count_;
        }

        T* chunk = (T*)m_allocator_->Allocate(sizeof(T)*inc_count);
        if ( NULL == chunk )
        {
            return -1;
        }

        for ( size_t i=0; i<inc_count; ++i )
        {
            T* p = new((void*)(chunk+i))T;
            m_object_list_.push_back(p);
        }

        m_chunk_list_.push_back(chunk);
        m_current_count_ += inc_count;
        return 0;
    }

private:
    allocatorc* m_allocator_;
    threadc_mutex m_mutex_;

    size_t m_current_count_;
    size_t m_max_count_;
    size_t m_increase_count_;

    std::list<T*> m_object_list_;
    std::vector<T*> m_chunk_list_;
};

#endif // _HC_OBJECT_ALLOCATOR_H_
