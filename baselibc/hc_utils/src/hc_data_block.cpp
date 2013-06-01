#include "hc_data_block.h"
#include "hc_stack_trace.h"

#include <algorithm>
#include <string.h>


data_block::data_block(size_t block_size, allocatorc* alloc)
    : m_allocatorc_(alloc), m_begin_(0), m_end_(0), m_end_of_block_(0)
{
    STACK_TRACE_LOG();

    if (NULL == m_allocatorc_)
    {
        m_allocatorc_ = allocatorc::Instance();
    }

    size_t real_size = 0;
    m_begin_ = (iterator)m_allocatorc_->Allocate(block_size, &real_size);
    if ( NULL == m_begin_ )
    {
        return;
    }
    m_end_ = m_begin_;
    m_end_of_block_ = m_begin_ + real_size;
}

data_block::data_block(const data_block& odb)
{
    STACK_TRACE_LOG();

    m_allocatorc_ = odb.m_allocatorc_;
    size_t capacity = odb.capacity();
    m_begin_ = (iterator)m_allocatorc_->Allocate(capacity);
    if ( NULL == m_begin_ )
    {
        return;
    }
    m_end_ = m_begin_;
    m_end_of_block_ = m_begin_ + capacity;

    append(odb.begin(), odb.size());
}

data_block::~data_block()
{
    STACK_TRACE_LOG();

    if ( NULL != m_begin_ )
    {
        m_allocatorc_->Deallocate(m_begin_);
    }
}

data_block& data_block::operator=(const data_block& odb)
{
    STACK_TRACE_LOG();

    data_block odb_swap(odb);
    this->swap(odb_swap);
    return *this;
}

void data_block::swap(data_block& odb)
{
    STACK_TRACE_LOG();

    ::std::swap(m_allocatorc_, odb.m_allocatorc_);
    ::std::swap(m_begin_, odb.m_begin_);
    ::std::swap(m_end_, odb.m_end_);
    ::std::swap(m_end_of_block_, odb.m_end_of_block_);
}

int data_block::resize(size_t sz)
{
    STACK_TRACE_LOG();

    // maybe it doesn't need to realloc
    if ( sz <= capacity() )
    {
        if ( begin() + sz < end() )
        {
            m_end_ = begin() + sz;
        }
        return 0;
    }

    data_block odb(sz, m_allocatorc_);
    if ( !odb.is_init() )
    {
        return -1;
    }

    if ( odb.append( begin(), ::std::min(size(), sz) ) != 0 )
    {
        return -1;
    }
    this->swap(odb);
    return 0;
}

int data_block::copy(iterator dest_buffer, const_iterator src_buffer, size_t sz)
{
    STACK_TRACE_LOG();

    // dest_buffer must be in [m_begin_, m_end_)
    ssize_t dest_offset = dest_buffer - begin();
    if ( dest_buffer < begin() ||  dest_buffer > end() )
    {
        return -1;
    }

    ssize_t src_offset = src_buffer - begin();
    size_t dest_size = dest_offset + sz;
    ssize_t inc_size = dest_size - size();
    if ( inc_size < 0 )
    {
        inc_size = 0;
    }

    if ( dest_size > capacity() )
    {
        if ( resize(dest_size) != 0 )
        {
            return -1;
        }
    }

    if ( src_offset >= 0  && (size_t)src_offset < size() )
    {
        memmove(begin()+dest_offset, begin()+src_offset, sz);
    }
    else
    {
        memcpy(begin()+dest_offset, src_buffer, sz);
    }
    return increase_end(inc_size);
}

int data_block::erase(iterator b, iterator e)
{
    STACK_TRACE_LOG();

    if ( b == e )
    {
        return 0;
    }

    if ( b > e )
    {
        return -1;
    }

    if ( b < begin() || e > end() )
    {
        return -1;
    }

    if ( b == begin() && e == end() )
    {
        clear();
        return 0;
    }

    if ( copy(b, e, end()-e) != 0 )
    {
        return -1;
    }
    return decrease_end(e-b);
}

int data_block::increase_end(size_t sz)
{
    STACK_TRACE_LOG();

    if ( sz > available() )
    {
        return -1;
    }
    m_end_ += sz;
    return 0;
}

int data_block::decrease_end(size_t sz)
{
    STACK_TRACE_LOG();

    if ( sz > size() )
    {
        return -1;
    }
    m_end_ -= sz;
    return 0;
}
