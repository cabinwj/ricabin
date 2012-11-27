#ifndef _COMMON_MINI_HEAP_
#define _COMMON_MINI_HEAP_

#include "common_types.h"
#include "common_allocator.h"


class min_helem
{
public:
    min_helem() : m_weight_(0), m_min_heap_idx_(-1) { }
    virtual ~min_helem() { }

public:
    virtual void Call() = 0;

public:
    void weight(uint32_t w) { m_weight_ = w; }
    uint32_t weight() { return m_weight_; }
    void min_heap_idx(int32_t min_hidx) { m_min_heap_idx_ = min_hidx; }
    int32_t min_heap_idx() { return m_min_heap_idx_; }

private:
    uint32_t m_weight_;
    int32_t m_min_heap_idx_;
};


class min_heap
{
public:
    min_heap() : m_array_(NULL), m_count_(0), m_capacity_(0) {}
    virtual ~min_heap() { if (NULL != m_array_) allocatorc::Instance()->Deallocate(m_array_); }

public:
    int push(min_helem* helem);
    min_helem* pop();
    int erase(min_helem* helem);

    int reserve();

private:
    void shift_up(int32_t hole_idx, min_helem* helem);
    void shift_down(int32_t hole_idx, min_helem* helem);

private:
    min_helem** m_array_;
    int32_t m_count_;
    int32_t m_capacity_;
};

#endif
