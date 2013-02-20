#ifndef _HC_ARRAY_H_
#define _HC_ARRAY_H_

/*
** �򵥷�װ��ʵ����һ����������
** �Ż���ɾ��ָ��Ԫ��ʱ��ʱ�临�Ӷ�ΪO(n)
** �ο����ϼ�����ʵ�ַ�ʽ,��л!
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

    // �ṩ�±������������������һ��ʹ��
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

   //ȡ��Ԫ��
    reference front()
    {
        return elems_[0];
    }

    const_reference front() const
    {
        return elems_[0];
    }

    // ȡβԪ��
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

    //�õ�ԭʼ����
    const T* data() const
    {
        return elems_;
    }  
   
    T* c_array()
    {
        return elems_;
    }

    // �������һ��
    iterator push_back(const T& t)
    {
        assert(size_ <= capacity() && "array<> push_back size_ > capacity()");
        elems_[size_++] = t;
        return elems_ + size_;
    }
    
    //��ͷ������һ��Ԫ��
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

    // ɾ��һ��Ԫ��,���ҷ�����һ������,������end
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

    // ɾ��ָ��λ�õ�Ԫ��(λ��Ϊiterator��ʽ)
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

    // ɾ��ָ��λ�õ�Ԫ��(λ��Ϊindex��ʽ)
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

    //��Χɾ��
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

    //���
    void clear()
    {
        size_ = 0;
    }

private:
    T elems_[count];
    size_t size_;
};

#endif  // _HC_ARRAY_H_
