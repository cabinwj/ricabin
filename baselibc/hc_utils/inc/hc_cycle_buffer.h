//! @file cycle_buffer.h
#ifndef _COMMON_CYCLE_BUFFER_H_
#define _COMMON_CYCLE_BUFFER_H_


#include "hc_thread_mutex.h"

//! @class cycle_buffer
//! @brief 循环缓冲
//!
//! 模板参数 T 是循环缓冲区中存放的数据类型, 在写入和读取时都会发生拷贝, 所以 T 不宜太大
//! 模板参数 MAX_COUNT 是循环缓冲区中最多存放的元素个数
//! 多线程访问此类时要确保读和写各只有一个线程
template <typename T, int MAX_COUNT>
class cycle_buffer
{
public:
    cycle_buffer();
    ~cycle_buffer();

    //! 读取数据
    //! @param data 读到的数据
    //! @return 读取结果, true:读取成功, false:读取失败,缓存为空
    bool read(T& data);

    //! 写入数据
    //! @param data 要写入的数据
    //! @return 写入结果, true:写入成功, false:写入失败,缓存已满
    bool write(const T& data);

    //! 获取当前buffer中的结点个数
    int size();

private:
    //! 缓冲元素结点
    T m_datas_[MAX_COUNT];

    //volatile int m_size_;
    //! 读下标
    volatile int m_read_index_;

    //! 写下标
    volatile int m_write_index_;

    //! 线程锁, 保护处理器
    threadc_mutex m_mutex_;
};


template <typename T, int MAX_COUNT>
cycle_buffer<T, MAX_COUNT>::cycle_buffer()
{
    m_read_index_ = 0;
    m_write_index_ = 0;
}


template <typename T, int MAX_COUNT>
cycle_buffer<T, MAX_COUNT>::~cycle_buffer()
{
}


template <typename T, int MAX_COUNT>
bool cycle_buffer<T, MAX_COUNT>::read(T& data)
{
    threadc_mutex_guard lock(m_mutex_);

    if (m_read_index_ == m_write_index_) {
        return false;
    }

    data = m_datas_[m_read_index_];

    if (m_read_index_ < MAX_COUNT - 1) {
        m_read_index_++;
    }
    else {
        m_read_index_ = 0;
    }

    //m_size_--;

    return true;
}


template <typename T, int MAX_COUNT>
bool cycle_buffer<T, MAX_COUNT>::write(const T& data)
{
    threadc_mutex_guard lock(m_mutex_);

    if (m_read_index_ == 0)
    {
        if (m_write_index_ == (MAX_COUNT - 1)) {
            return false;
        }
    }
    else
    {
        if (m_write_index_ == (m_read_index_ - 1)) {
            return false;
        }
    }

    m_datas_[m_write_index_] = data;

    if (m_write_index_ < MAX_COUNT - 1) {
        m_write_index_++;
    }
    else {
        m_write_index_ = 0;
    }

    //m_size_++;

    return true;
}

template <typename T, int MAX_COUNT>
int cycle_buffer<T, MAX_COUNT>::size()
{
    threadc_mutex_guard lock(m_mutex_);

    //由于m_write_index, m_read_index均为volatile,此处能最大限度的得到正确的容量值
    int tmp_write_index = m_write_index_;
    int tmp_read_index = m_read_index_;

    return tmp_write_index >= tmp_read_index ? (tmp_write_index-tmp_read_index): tmp_write_index+(MAX_COUNT-tmp_read_index);
}


#endif // _COMMON_CYCLE_BUFFER_H_
