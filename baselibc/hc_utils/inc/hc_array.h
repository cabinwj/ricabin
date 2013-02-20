#ifndef _HC_ARRAY_H_
#define _HC_ARRAY_H_

/*
** 简单封装并实现了一个定长数组
** 优化：删除指定元素时的时间复杂度为O(n)
** 参考网上及其他实现方式,感谢!
*/

#include <cstddef>
#include <algorithm>
#include <assert.h>

template <typename T, size_t count >
class object_array
{
public:
    enum
    {
        __CAPACITY__ = count,
    };

public:
    // some type definition
    typedef T value_type;

    typedef T * iterator;
    typedef T & reference;

    typedef T * const const_iterator;
    typedef const T &  const_reference;
        
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::reverse_iterator < iterator > reverse_iterator;
    typedef std::reverse_iterator < const_iterator > const_reverse_iterator;

public:
    iterator begin()
    {
        return elems_;
    }

    const_iterator begin() const
    {
        return elems_;
    }

    iterator end()
    {
        return elems_ + size_;
    }

    const_iterator end() const
    {
        return elems_ + size_;
    }  
   
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    // 提供下标操作符，可以像数组一样使用
    reference operator[] (size_type i)
    {
        assert(i < size_ && "operator[] out of range");
        return elems_[i];
    }

    const_reference operator[] (size_type i) const
    {
        assert(i < size_ && "operator[] out of range");
        return elems_[i];
    }

   //取首元素
    reference front()
    {
        return elems_[0];
    }

    const_reference front() const
    {
        return elems_[0];
    }

    // 取尾元素
    reference back()
    {
        return elems_[size_ - 1];
    }

    const_reference back() const
    {
        return elems_[size_ - 1];
    }

    size_type size()
    {
        return size_;
    }

    bool empty()
    {
        return 0 == size_;
    }

    bool full()
    {
        return __CAPACITY__ <= size_;
    }

    //得到原始数组
    const T* data() const
    {
        return elems_;
    }  
   
    T* c_array()
    {
        return elems_;
    }

    // 后面插入一个
    iterator push_back(const T& t)
    {
        assert(size_ <= capacity() && "array<> push_back size_ > capacity()");
        elems_[size_++] = t;
        return elems_ + size_;
    }
    
    //在头部插入一个元素
    iterator push_front(const T& t)
    {
        assert(size_ <= capacity() && "array<> push_front size_ > capacity()");
        for (int i = size_; i > 0; i--)
        {
            elems_[i] = elems_[i-1];
        }
        elems_[0] = t;
        size_++;
        return &elems_[0];
    }

    // 删除一个元素,并且返回下一个索引,可能是end
    iterator quick_erase(iterator pos)
    {
        iterator endPos = end();
        if (pos != endPos)
        {
            iterator nextPos = pos + 1;
            if (nextPos != endPos)
            {
                // id not the last one, should be replace by the last
                * pos = back();
            }
            --size_;
        }
        return pos;
    }

    // 删除指定位置的元素(位置为iterator形式)
    iterator erase(iterator pos)
    {
        iterator endPos = end();
        if (pos != endPos)
        {
            iterator nextPos = pos + 1;
            if (nextPos != endPos)
            {
                std::copy(nextPos, endPos, pos);
            }
            --size_;
        }
        return pos;
    }

    // 删除指定位置的元素(位置为index形式)
    void erase(size_type pos)
    {
        assert(pos < size_ && "erase out of range");
        size_type next = pos + 1;
        if (next < size_)
        {
            std::copy(elems_ + next, elems_ + size_, elems_ + pos);
        }
        --size_;
    }

    //范围删除
    iterator erase(iterator first, iterator last)
    {
        iterator __i(std::copy(last, end(), first));
        size_ -= (last - first);
        return first;
    }

    size_t capacity()
    {
        return count;
    }

    //清空
    void clear()
    {
        size_ = 0;
    }

private:
    T elems_[count];
    size_t size_;
};

#endif  // _HC_ARRAY_H_
