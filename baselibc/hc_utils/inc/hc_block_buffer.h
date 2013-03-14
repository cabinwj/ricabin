#ifndef _HC_BLOCK_BUFFER_H_
#define _HC_BLOCK_BUFFER_H_

#include <string.h>
//#include <assert.h>

//! @class block_buffer
//! @brief ���ݿ黺��
//!
//! ģ����� BUFFER_LENGTH �ǻ���������
template <int BUFFER_LENGTH>
class block_buffer
{
public:
    block_buffer();
    ~block_buffer();

public:
    //! ȡ��ָ��
    char* rd_ptr();
    //! ��ָ��ǰ��
    void rd_ptr(int n);
    //! ȡдָ��
    char* wr_ptr();
    //! дָ��ǰ��
    void wr_ptr(int n);

    //! �������ݳ���
    int data_length();
    //! ʣ��ռ䳤��
    int space_length();

    //! ʵ�ʳ���
    int length();
    //! ���տռ�
    void recycle();

    //! ��չbuffer�ռ�
    void extend(int new_len);

private:
    //! ������
    char* m_buffer_;
    //! ��λ��
    int m_rd_ndx_;
    //! дλ��
    int m_wr_ndx_;
    //! ��ǰbufferʵ�ʴ�С
    int m_length_;
};


template <int BUFFER_LENGTH>
block_buffer<BUFFER_LENGTH>::block_buffer()
{
    m_rd_ndx_ = 0;
    m_wr_ndx_ = 0;
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
    return m_buffer_ + m_rd_ndx_;
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::rd_ptr(int n)
{
    //assert((m_wr_ndx_ - m_rd_ndx_) >= n);
    m_rd_ndx_ += n;
}

template <int BUFFER_LENGTH>
char* block_buffer<BUFFER_LENGTH>::wr_ptr()
{
    return m_buffer_ + m_wr_ndx_;
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::wr_ptr(int n)
{
    //assert((m_length_ - m_wr_ndx_) >= n);
    m_wr_ndx_ += n;
}

template <int BUFFER_LENGTH>
int block_buffer<BUFFER_LENGTH>::data_length()
{
    return (m_wr_ndx_ - m_rd_ndx_);
}

template <int BUFFER_LENGTH>
int block_buffer<BUFFER_LENGTH>::space_length()
{
    return (m_length_ - m_wr_ndx_);
}

template <int BUFFER_LENGTH>
int block_buffer<BUFFER_LENGTH>::length()
{
    return m_length_;
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::recycle()
{
    int data_len = m_wr_ndx_ - m_rd_ndx_;
    if ( 0 == data_len )
    {
        m_wr_ndx_ = 0;
        m_rd_ndx_ = 0;
    }
    else
    {
        memmove(m_buffer_, m_buffer_ + m_rd_ndx_, data_len);
        m_rd_ndx_ = 0;
        m_wr_ndx_ = data_len;
    }
}

template <int BUFFER_LENGTH>
void block_buffer<BUFFER_LENGTH>::extend(int new_len)
{
    int data_len = m_wr_ndx_ - m_rd_ndx_;
    char* ext_buffer = new char[new_len + data_len];

    if ( 0 == data_len )
    {
        m_wr_ndx_ = 0;
        m_rd_ndx_ = 0;
    }
    else
    {
        memmove(ext_buffer, m_buffer_ + m_rd_ndx_, data_len);
        m_rd_ndx_ = 0;
        m_wr_ndx_ = data_len;
    }

    delete[] m_buffer_;
    m_buffer_ = ext_buffer;
    m_length_ = new_len + data_len;
}

#endif // _HC_BLOCK_BUFFER_H_
