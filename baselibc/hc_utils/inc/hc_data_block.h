#ifndef _HC_DATA_BLOCK_H_
#define _HC_DATA_BLOCK_H_

#include "hc_allocator.h"


class data_block
{
public:
    enum {
        DEFAULT_BLOCK_SIZE = 1024
    };

public:
    typedef char* iterator;
    typedef const char* const_iterator;

public:
    data_block(size_t block_size = DEFAULT_BLOCK_SIZE, allocatorc* alloc = allocatorc::Instance());
    data_block(const data_block&);
    ~data_block();

public:
    data_block& operator = (const data_block&);

public:
    void swap(data_block&);
    int resize(size_t sz);
    int increase_end(size_t sz);
    int decrease_end(size_t sz);
    int copy(iterator dest_buffer, const_iterator src_buffer, size_t sz);
    int erase(iterator b, iterator e);

public:
    inline void clear() { m_end_ = m_begin_; }
    inline iterator begin() { return m_begin_; }
    inline iterator end() { return m_end_; }
    inline const_iterator begin() const { return m_begin_; }
    inline const_iterator end() const { return m_end_; }
    inline int append(const_iterator src_buffer, size_t sz) { return copy(end(), src_buffer, sz); }
    inline bool is_init() const { return begin() != NULL; }
    inline bool is_empty() const { return begin() == end(); }
    inline bool is_full() const { return end() == m_end_of_block_; }
    inline size_t size() const { return end() - begin(); }
    inline size_t length() const { return size(); }
    inline size_t available() const { return m_end_of_block_ - end(); }
    inline size_t capacity() const { return m_end_of_block_ - begin(); }
    inline void offset_cursor(size_t len) { if (capacity() >= len) { m_end_ = m_begin_ + len; } }

public:
    allocatorc* m_allocatorc_;
    iterator m_begin_;
    iterator m_end_;
    iterator m_end_of_block_;
};
#endif // _HC_DATA_BLOCK_H_
