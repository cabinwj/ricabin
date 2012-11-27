#ifndef _COMMON_BLOCK_BUFFER_H_
#define _COMMON_BLOCK_BUFFER_H_

#include <string.h>
#include <assert.h>

//! @class block_buffer
//! @brief 数据块缓冲
//!
//! 模板参数 BUFFER_LENGTH 是缓冲区长度
template <int BUFFER_LENGTH>
class block_buffer
{
public:
    block_buffer();
    ~block_buffer();

public:
    //! 取读指针
    char* rd_ptr();

    //! 读指针前移
    void rd_ptr(int n);

    //! 取写指针
    char* wr_ptr();

    //! 写指针前移
    void wr_ptr(int n);

    //! 可用数据长度
    int data_length();

    //! 剩余空间长度
    int space_length();

    //! 实际长度
    int length();

    //! 回收空间
    void recycle();

    //! 扩展buffer空间
    void extend(int new_length);

private:
    //! 缓冲区
    char* m_buffer_;

    //! 读位置
    int m_read_index_;

    //! 写位置
    int m_write_index_;

    //! 当前buffer实际大小
    int m_length_;
};


template <int BUFFER_LENGTH>
block_buffer<BUFFER_LENGTH>::block_buffer()
{
    m_read_index_ = 0;
    m_write_index_ = 0;
    m_buffer_ = new char[BUFFER_LENGTH];
    m_length_ = BUFFER_LENGTH;
}

template <int BUFFER_LENGTH>
block_buffer<BUFFER_LENGTH>::~block_buffer()
{
    delete[] m_buffer_;
}

template <int BUFFER_LENGTH>
char* block_buffer<BUFFER_LENGTH>::rd_ptr()
{
    return m_buffer_ + m_read_index_;
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::rd_ptr(int n)
{
    assert((m_write_index_ - m_read_index_) >= n);
    m_read_index_ += n;
}

template <int BUFFER_LENGTH>
char* block_buffer<BUFFER_LENGTH>::wr_ptr()
{
    return m_buffer_ + m_write_index_;
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::wr_ptr(int n)
{
    assert((m_length_ - m_write_index_) >= n);
    m_write_index_ += n;
}

template <int BUFFER_LENGTH>
int block_buffer<BUFFER_LENGTH>::data_length()
{
    return (m_write_index_ - m_read_index_);
}

template <int BUFFER_LENGTH>
int block_buffer<BUFFER_LENGTH>::space_length()
{
    return (m_length_ - m_write_index_);
}

template <int BUFFER_LENGTH>
int block_buffer<BUFFER_LENGTH>::length()
{
    return m_length_;
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::recycle()
{
    int data_len = m_write_index_ - m_read_index_;
    if (0 == data_len) {
        m_write_index_ = 0;
        m_read_index_ = 0;
    }
    else {
        memmove(m_buffer_, m_buffer_ + m_read_index_, data_len);
        m_read_index_ = 0;
        m_write_index_ = data_len;
    }
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::extend(int new_length)
{
    int data_len = m_write_index_ - m_read_index_;
    char* ext_buffer = new char[new_length+data_len];

    if(data_len == 0)
    {
        m_write_index_ = 0;
        m_read_index_ = 0;
    }
    else
    {
        memmove(ext_buffer, m_buffer_ + m_read_index_, data_len);
        m_read_index_ = 0;
        m_write_index_ = data_len;
    }

    delete[] m_buffer_;
    m_buffer_ = ext_buffer;
    m_length_ = new_length+data_len;
}

#endif // _COMMON_BLOCK_BUFFER_H_
