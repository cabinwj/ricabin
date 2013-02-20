#ifndef _HC_BITMAP_H_
#define _HC_BITMAP_H_

#include "hc_types.h"

#include <string.h>


class bitmap32
{
public:
    enum {
        all_bits = (uint32_t)0xffffffff,
    };

public:
    bitmap32(uint32_t bit) : m_bit32_(bit) { }
    bitmap32() : m_bit32_(0) { }
    ~bitmap32() { }

public:
    //clear all bits
    void clear() { m_bit32_ = 0; }
    //is the bit setted ?
    bool is_setted(uint32_t bit) { if (m_bit32_ & bit) return true; else return false; }
    //set the bit
    void set(uint32_t bit) { m_bit32_ |= bit; }
    //clear the bit
    void unset(uint32_t bit) { m_bit32_ &= (bit ^ all_bits); }
    //just set all bits
    void allbits() { m_bit32_ = all_bits; }

protected:
    uint32_t m_bit32_;
};


//bitmap container
template<int bitmap_size>
class ebitmap
{
public:
    enum {
        bytesize = (int32_t)(8*sizeof(unsigned char)),
    };

    enum {
        max_bitmap_size = bitmap_size,
    };

public:
    //copy constructor
    ebitmap<bitmap_size>& operator =(const ebitmap<bitmap_size>& rhs);

public:
    //clear all bits
    int32_t clear();
    //is the bits[idx] setted ?
    int32_t is_setted(int32_t idx);
    //set the bit of idx
    int32_t set(int32_t idx);
    //clear the bit of idx
    int32_t unset(int32_t idx);
    //just set all bits
    int32_t allbits();

private:
    unsigned char array_[max_bitmap_size];
};


template<int bitmap_size>
ebitmap<bitmap_size>& ebitmap<bitmap_size>::operator =(const ebitmap<bitmap_size>& rhs)
{
    if (this == &rhs)
    {
        return (*this);
    }

    memcpy((void *)array_, (const void *)rhs.array_, sizeof(array_));
    return (*this);
}

template<int bitmap_size>
int32_t ebitmap<bitmap_size>::clear()
{
    memset((void *)array_, 0, sizeof(array_));
    return 0;
}

template<int bitmap_size>
int32_t ebitmap<bitmap_size>::is_setted(int32_t idx)
{
    if ( (idx > (bitmap_size * bytesize)) || (idx < 0))
    {
        return 0;
    }

    unsigned char magic = 0x80;
    int32_t setcion_idx = idx / bytesize;
    int32_t bit_idx = idx % bytesize;

    if ( array_[setcion_idx] & (magic >> bit_idx) )
    {
        return 1;
    }

    return 0;
}

template<int bitmap_size>
int32_t ebitmap<bitmap_size>::set(int32_t idx)
{
    if ( (idx > (bitmap_size * bytesize)) || (idx < 0))
    {
        return 0;
    }

    unsigned char magic = 0x80;
    int setcion_idx = idx / bytesize;
    int bit_idx = idx % bytesize;

    array_[setcion_idx] |= (magic >> bit_idx);

    return 0;
}

template<int bitmap_size>
int32_t ebitmap<bitmap_size>::unset(int32_t idx)
{
    if ( (idx > (bitmap_size * bytesize)) || (idx < 0))
    {
        return 0;
    }

    unsigned char magic = 0x80;
    int setcion_idx = idx / bytesize;
    int bit_idx = idx % bytesize;

    array_[setcion_idx] &= (0xff ^ (magic >> bit_idx));

    return 0;
}

template<int bitmap_size>
int32_t ebitmap<bitmap_size>::allbits()
{
    memset(array_, 0xff, sizeof(array_));
    return 0;
}

#endif // _HC_BITMAP_H_
