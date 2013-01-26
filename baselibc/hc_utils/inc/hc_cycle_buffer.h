//! @file cycle_buffer.h
#ifndef _COMMON_CYCLE_BUFFER_H_
#define _COMMON_CYCLE_BUFFER_H_


#include "hc_thread_mutex.h"

//! @class cycle_buffer
//! @brief ѭ������
//!
//! ģ����� T ��ѭ���������д�ŵ���������, ��д��Ͷ�ȡʱ���ᷢ������, ���� T ����̫��
//! ģ����� MAX_COUNT ��ѭ��������������ŵ�Ԫ�ظ���
//! ���̷߳��ʴ���ʱҪȷ������д��ֻ��һ���߳�
template <typename T, int MAX_COUNT>
class cycle_buffer
{
public:
    cycle_buffer();
    ~cycle_buffer();

    //! ��ȡ����
    //! @param data ����������
    //! @return ��ȡ���, true:��ȡ�ɹ�, false:��ȡʧ��,����Ϊ��
    bool read(T& data);

    //! д������
    //! @param data Ҫд�������
    //! @return д����, true:д��ɹ�, false:д��ʧ��,��������
    bool write(const T& data);

    //! ��ȡ��ǰbuffer�еĽ�����
    int size();

private:
    //! ����Ԫ�ؽ��
    T m_datas_[MAX_COUNT];

    //volatile int m_size_;
    //! ���±�
    volatile int m_read_index_;

    //! д�±�
    volatile int m_write_index_;

    //! �߳���, ����������
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

    //����m_write_index, m_read_index��Ϊvolatile,�˴�������޶ȵĵõ���ȷ������ֵ
    int tmp_write_index = m_write_index_;
    int tmp_read_index = m_read_index_;

    return tmp_write_index >= tmp_read_index ? (tmp_write_index-tmp_read_index): tmp_write_index+(MAX_COUNT-tmp_read_index);
}


#endif // _COMMON_CYCLE_BUFFER_H_
