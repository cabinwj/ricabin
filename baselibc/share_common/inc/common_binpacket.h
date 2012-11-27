#ifndef _COMMON_BIN_PACKET_H_
#define _COMMON_BIN_PACKET_H_

#include "common_types.h"
#include "common_type_trait.h"

#include <string>
#include <string.h>


#define BS_MAKE64(lo, hi) ((uint64_t)((uint64_t)(lo) | (((uint64_t)(hi)) << 32)))
#define BS_LO32(x)  ((uint32_t)((x) & 0x00000000ffffffffULL))
#define BS_HI32(x)  ((uint32_t)(((x) & 0xffffffff00000000ULL) >> 32))


//! 写入流时 将data_所指内存中的字符串写入流中, 同时会写入结束0。
//! 读取流时 先读流中的长度，如果流中len的长度足够，并且len<=len_，将流中的len长度的字符串拷贝到data_所指内存中，修正len_=len。
// character string 字符串 
typedef struct copy_cnstr{
    copy_cnstr(char* data, uint32_t len):len_(len),data_(data){}
    uint32_t len_; //!< 字符串长度
    char* data_;   //!< 字符串指针
}copy_cnstr;   // N 表示读时只能从 D=true (从左到右) 的包中读取

//! 写入流时 将data_所指内存中的字符串写入流中, 同时会写入结束0
//! 读取流时 先读流中的长度，如果流中len的长度足够, 将data_指向流中内存块，不发生拷贝，修正len_=len。
// character string 
typedef struct nocopy_cnstr{
    nocopy_cnstr():len_(0),data_(NULL){}
    uint32_t len_; //!< 字符串长度
    char* data_;   //!< 字符串指针
}nocopy_cnstr;    // N 表示读时只能从 D=true (从左到右) 的包中读取

//! 写入流时 将data_所指内存中的数据写入流中。
//! 读取流时 如果len_>0，将data_指向流中内存块，不发生拷贝，修正len_为数据实际长度。
// binary string  二进制内存块
typedef struct nocopy_bstr{
    nocopy_bstr():len_(0),data_(NULL){}
    uint32_t len_; //!< 字符串长度(不包括结束0)
    char* data_;   //!< 字符串指针
}nocopy_bstr;

//! 写入流时 将data_所指内存中的数据写入流中。
//! 读取流时 先读流中的长度，如果流中len的长度足够, 将data_指向流中内存块，不发生拷贝，修正len_=len。
// binary string  二进制内存块
typedef struct nocopy_bnstr{
    nocopy_bnstr():len_(0),data_(NULL){}
    uint32_t len_; //!< 字符串长度(不包括结束0)
    char* data_;   //!< 字符串指针
}nocopy_bnstr;    // N 表示读时只能从 D=true (从左到右) 的包中读取

// D = true ,数据包从左往右  默认方式
// D = false 数据包从右往左
template <bool D = true>
class binary_packet
{
public:
    binary_packet(char* begin, char* end):m_good_(true),m_eof_(false),m_begin_(begin),
                                      m_end_(end),m_current_(begin),m_head_len_(0)
    {
        if (!D) m_current_ = m_end_;
    }

    binary_packet(char* begin, uint32_t len):m_good_(true),m_eof_(false),m_begin_(begin),
                                         m_end_(begin+len),m_current_(begin),m_head_len_(0)
    {
        if (!D) m_current_ = m_end_;
    }

    ~binary_packet(){}

public:
    uint32_t size()
    {
        return (m_end_ - m_begin_);
    }

    void offset(uint32_t len)
    {
        if (!m_good_) return;
        if (D)
        {
            if ( m_end_ - m_current_ < static_cast<int32_t>(len) )
            {
                m_good_ = false;
                return;
            }
            m_current_ += len;
            if (m_current_ == m_end_)
            {
                m_eof_ = true;
            }
        }
        else
        {
            if (m_current_ - m_begin_ < static_cast<int32_t>(len))
            {
                m_good_ = false;
                return;
            }
            m_current_ -= len;
        }
    }

    void offset_head(uint32_t len)
    {
        if (!m_good_) return;
        if (D)
        {
            if ( m_end_ - m_begin_ < static_cast<int32_t>(len) || m_current_ != m_begin_ )
            {
                m_good_ = false;
                return;
            }
            m_head_len_ = len;
            m_current_ += m_head_len_;
            if (m_current_ == m_end_)
            {
                m_eof_ = true;
            }
        }
        else
        {
            if ( m_end_ - m_begin_ < static_cast<int32_t>(len) )
            {
                m_good_ = false;
                return;
            }
            m_head_len_ = len;
        }
    }

    uint32_t remain_length()
    {
        if (D)
        {
            return (m_end_ - m_current_);
        }
        else
        {
            return (m_current_ - m_begin_);
        }
    }

    uint32_t length()
    {
        if (D)
        {
            return (m_current_ - m_begin_);
        }
        else
        {
            return (m_end_ - m_current_);
        }
    }

    void rollback()
    {
        if (D)
        {
            m_current_ = m_begin_ + m_head_len_;
        }
        else
        {
            m_current_ = m_end_;
        }
    }

    void clear()
    {
        if (D)
        {
            m_current_ = m_begin_;
        }
        else
        {
            m_current_ = m_end_;
        }
        m_head_len_ = 0;
    }

    void set_pkglen(uint32_t len)
    {
        if (!m_good_) return;
        if (D)
        {
            if ( m_end_ - m_begin_ < static_cast<int32_t>(len) ||
                    m_current_ - m_begin_ > static_cast<int32_t>(len) )
            {
                m_good_ = false;
                return;
            }
            m_end_ = m_begin_ + len;
        }
        else
        {
            if ( m_end_ - m_begin_ < static_cast<int32_t>(len) ||
                    m_end_ - m_current_ > static_cast<int32_t>(len) )
            {
                m_good_ = false;
                return;
            }
            m_begin_ = m_end_ - len;
        }
    }

    char* get_data()
    {
        if (D)
        {
            return m_begin_;
        }
        else
        {
            return m_current_;
        }
    }

    char* get_current()
    {
        return m_current_;
    }

    bool good()
    {
        return m_good_;
    }

    bool eof()
    {
        return m_eof_;
    }

public:
    bool m_good_;
    bool m_eof_;
    char* m_begin_;
    char* m_end_;
    char* m_current_;
    uint32_t m_head_len_;
};

template<bool D=true>
class binary_input_packet : public binary_packet<D>
{
public:
    binary_input_packet(char* begin, char* end):binary_packet<D>( begin, end){}
    binary_input_packet(char* begin, uint32_t len):binary_packet<D>( begin, len){}
    ~binary_input_packet(){}

public:
    //拆包
    int read(char* buf, int n = 1)
    {
        if ( !this->m_good_ || this->m_eof_ ) return 0;
        if ( n <= 0 ) return 0;
        if (D)
        {
            n = (this->m_end_ - this->m_current_) > n ? n : (this->m_end_ - this->m_current_);
            memcpy(buf, this->m_current_, n);
            this->m_current_ += n;
            if ( this->m_end_ == this->m_current_ )
            {
                this->m_eof_ = true;
            }
        }
        else
        {
            n = (this->m_current_ - this->m_begin_) > n ? n : (this->m_current_ - this->m_begin_);
            this->m_current_ -= n;
            memcpy(buf, this->m_current_, n);
            if (this->m_current_ == this->m_begin_)
            {
                this->m_eof_ = true;
            }
        }
        return n;
    }

    template<class T>
    void get_head(T& value)
    {
        if (!this->m_good_) return;
        if ( sizeof(T) != this->m_head_len_ )
        {
            this->m_good_ = false;
            return;
        }
        if (D)
        {
            value = *reinterpret_cast<T*>(this->m_begin_);
        }
        else
        {
            value = *reinterpret_cast<T*>(this->m_current_);
            this->m_current_ -= sizeof(T);
        }
    }

public:
    template<class T>
friend binary_input_packet& operator>>(binary_input_packet& in, T& value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        if (D)
        {
            if (in.m_end_ - in.m_current_ < static_cast<int32_t>(sizeof(T)))
            {
                in.m_good_ = false;
                return in;
            }
        }
        else
        {
            if (in.m_current_ - in.m_begin_ < static_cast<int32_t>(sizeof(T)))
            {
                in.m_good_ = false;
                return in;
            }
        }

        switch(sizeof(T))
        {
        case 1:
            {
                if (D) value = *reinterpret_cast<T*>(in.m_current_++);
                else  value = *reinterpret_cast<T*>(--in.m_current_);
            }break;
        case 2:
            {
                if ( type_test<T>::IS_SHORT || type_test<T>::IS_UNSIGNED_SHORT )
                {
                    if (D) value = ntohs(*reinterpret_cast<T*>(in.m_current_));
                    else  value = ntohs(*reinterpret_cast<T*>((in.m_current_-2)));
                }
                else
                {
                    if (D) value = *reinterpret_cast<T*>(in.m_current_);
                    else  value = *reinterpret_cast<T*>((in.m_current_-2));
                }
                if (D) in.m_current_ += 2;
                else   in.m_current_ -= 2;
            }break;
        case 4:
            {
                if ( type_test<T>::IS_INT || type_test<T>::IS_UNSIGNED_INT )
                {
                    if (D) value = ntohl(*reinterpret_cast<T*>(in.m_current_));
                    else   value = ntohl(*reinterpret_cast<T*>((in.m_current_-4)));
                }
                else
                {
                    if (D) value = *reinterpret_cast<T*>(in.m_current_);
                    else  value = *reinterpret_cast<T*>((in.m_current_-4));
                }
                if (D) in.m_current_ += 4;
                else  in.m_current_ -= 4;
            }break;
        default:
            {
                if (D) value = *reinterpret_cast<T*>(in.m_current_);
                else  value = *reinterpret_cast<T*>((in.m_current_ - sizeof(T)));

                if (D) in.m_current_ += sizeof(T);
                else  in.m_current_ -= sizeof(T);
            }break;
        }
        if (D)
        {
            if (in.m_current_ == in.m_end_) in.m_eof_ = true;
        }
        else
        {
            if (in.m_current_ == in.m_begin_) in.m_eof_ = true;
        }
        return in;
    }

friend binary_input_packet& operator>>(binary_input_packet& in, std::string& value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        if (!D) return in;
        uint32_t len = 0;
        in >> len;
        if ( in.m_end_ - in.m_current_ < static_cast<int32_t>(len) || 1 > len )
        {
            in.m_good_ = false;
            return in;
        }
        char* end = in.m_current_ + len - 1;
        value = string(in.m_current_,end);
        in.m_current_ += len;
        if (in.m_current_ == in.m_end_) in.m_eof_ = true;
        return in;
    }

friend binary_input_packet& operator>>(binary_input_packet& in, char* value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        if (!D) return in;
        uint32_t len = 0;
        in >> len;
        if ( in.m_end_ - in.m_current_ < static_cast<int32_t>(len) || 1 > len )
        {
            in.m_good_ = false;
            return in;
        }
        memcpy(value, in.m_current_, len);
        in.m_current_ += len;
        if (in.m_current_ == in.m_end_) in.m_eof_ = true;
        return in;
    }

friend binary_input_packet& operator>>(binary_input_packet& in, copy_cnstr& value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        if (!D) return in;
        uint32_t len;
        in >> len;
        if ( in.m_end_ - in.m_current_ < static_cast<int32_t>(len) || value.len_ < len )
        {
            in.m_good_ = false;
            return in;
        }
        value.len_ = len;
        memcpy(value.data_, in.m_current_, value.len_);
        in.m_current_ += len;
        if (in.m_current_ == in.m_end_) in.m_eof_ = true;
        return in;
    }

friend binary_input_packet& operator>>(binary_input_packet& in, nocopy_cnstr& value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        if (!D) return in;
        uint32_t len;
        in >> len;
        if ( in.m_end_ - in.m_current_ < static_cast<int32_t>(len) )
        {
            in.m_good_ = false;
            return in;
        }
        value.len_ = len;
        value.data_ = in.m_current_;
        in.m_current_ += len;
        if (in.m_current_ == in.m_end_) in.m_eof_ = true;
        return in;
    }

friend binary_input_packet& operator>>(binary_input_packet& in, nocopy_bstr& value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        int& n = value.len_;
        if ( n <= 0 ) return in;
        if (D)
        {
            n = (in.m_end_ - in.m_current_) > n ? n : (in.m_end_ - in.m_current_);
            value.data_ = in.m_current_;
            in.m_current_ += n;
            if ( in.m_end_ == in.m_current_ ) in.m_eof_ = true;
        }
        else
        {
            n = (in.m_current_ - in.m_begin_) > n ? n : (in.m_current_ - in.m_begin_);
            in.m_current_ -= n;
            value.data_ = in.m_current_;
            if (in.m_current_ == in.m_begin_) in.m_eof_ = true;
        }
        return in;
    }

friend binary_input_packet& operator>>(binary_input_packet& in, nocopy_bnstr& value)
    {
        if (!in.m_good_ || in.m_eof_) return in;
        if (!D) return in;
        uint32_t len;
        in >> len;
        if ( in.m_end_ - in.m_current_ < static_cast<int32_t>(len) )
        {
            in.m_good_ = false;
            return in;
        }
        value.len_ = len;
        value.data_ = in.m_current_;
        in.m_current_ += len;
        if (in.m_current_ == in.m_end_) in.m_eof_ = true;
        return in;
    }
};

template <bool D=true>
class binary_output_packet : public binary_packet<D>
{
public:
    binary_output_packet(char* begin, char* end):binary_packet<D>( begin, end){}
    binary_output_packet(char* begin, uint32_t len):binary_packet<D>( begin, len){}
    ~binary_output_packet(){}

public:
    // 组包
    binary_output_packet& write(char* buf, int n = 1)
    {
        if (!this->m_good_ || this->m_eof_) return *this;
        if (D)
        {
            if (n <= 0 || this->m_end_ - this->m_current_ < n )
            {
                this->m_good_ = false;
                return *this;
            }
            memcpy(this->m_current_, buf, n);
            this->m_current_ += n;
        }
        else
        {
            if (n <= 0 || this->m_current_ - this->m_begin_ < n )
            {
                this->m_good_ = false;
                return *this;
            }
            this->m_current_ -= n;
            memcpy(this->m_current_, buf, n);
        }
        if (this->m_current_ == this->m_begin_) this->m_eof_ = true;
        return *this;
    }

    template<class T>
    void set_head(T value)
    {
        if (!this->m_good_) return;
        if ( sizeof(T) != this->m_head_len_ )
        {
            this->m_good_ = false;
            return;
        }
        if (D)
        {
            *reinterpret_cast<T*>(this->m_begin_) = value;
        }
        else
        {
            *reinterpret_cast<T*>(this->m_current_ - sizeof(T)) = value;
            this->m_current_ -= sizeof(T);
        }
    }

public:
    template<class T>
friend binary_output_packet& operator<<(binary_output_packet& out, T value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        if (D)
        {
            if (out.m_end_ - out.m_current_ < static_cast<int32_t>(sizeof(T)))
            {
                out.m_good_ = false;
                return out;
            }
        }
        else
        {
            if (out.m_current_ - out.m_begin_ < static_cast<int32_t>(sizeof(T)))
            {
                out.m_good_ = false;
                return out;
            }
        }
        switch(sizeof(T))
        {
        case 1:
            {
                if (D) *reinterpret_cast<T*>(out.m_current_++) = value;
                else  *reinterpret_cast<T*>(--out.m_current_) = value;
            }break;
        case 2:
            {
                if ( type_test<T>::IS_SHORT || type_test<T>::IS_UNSIGNED_SHORT )
                {
                    if (D) *reinterpret_cast<T*>(out.m_current_) = htons(value);
                    else  *reinterpret_cast<T*>((out.m_current_-2)) = htons(value);
                }
                else
                {
                    if (D) *reinterpret_cast<T*>(out.m_current_) = value;
                    else  *reinterpret_cast<T*>((out.m_current_-2)) = value;
                }
                if (D) out.m_current_ += 2;
                else  out.m_current_ -= 2;
            }break;
        case 4:
            {
                if ( type_test<T>::IS_INT || type_test<T>::IS_UNSIGNED_INT )
                {
                    if (D) *reinterpret_cast<T*>(out.m_current_) = htonl(value);
                    else  *reinterpret_cast<T*>((out.m_current_-4)) = htonl(value);
                }
                else
                {
                    if (D) *reinterpret_cast<T*>(out.m_current_) = value;
                    else  *reinterpret_cast<T*>((out.m_current_-4)) = value;
                }
                if (D) out.m_current_ += 4;
                else  out.m_current_ -= 4;
            }break;
        default:
            {
                if (D) *reinterpret_cast<T*>(out.m_current_) = value;
                else  *reinterpret_cast<T*>((out.m_current_-sizeof(T))) = value;

                if (D) out.m_current_ += sizeof(T);
                else  out.m_current_ -= sizeof(T);
            }break;
        }
        if (D)
        {
            if ( out.m_current_ == out.m_end_ ) out.m_eof_ = true;
        }
        else
        {
            if ( out.m_current_ == out.m_begin_ ) out.m_eof_ = true;
        }
        return out;
    }

friend binary_output_packet& operator<<(binary_output_packet& out, std::string value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        uint32_t len = value.length();
        if (D)
        {
            out << static_cast<uint32_t>(len+1);
            if ( out.m_end_ - out.m_current_ < static_cast<int32_t>(len+1) )
            {
                out.m_good_ = false;
                return out;
            }
            memcpy(out.m_current_,value.c_str(),len);
            out.m_current_ += len;
            *(out.m_current_) = static_cast<char>(0);
            out.m_current_ += 1;
        }
        else
        {
            if ( out.m_current_ - out.m_begin_ < static_cast<int32_t>(len+1) )
            {
                out.m_good_ = false;
                return out;
            }
            out << static_cast<char>(0);
            out.m_current_ -= len;
            memcpy(out.m_current_, value.c_str(), len);
            out << static_cast<uint32_t>(len+1);
        }
        if (D)
        {
            if ( out.m_current_ == out.m_end_ ) out.m_eof_ = true;
        }
        else
        {
            if ( out.m_current_ == out.m_begin_ ) out.m_eof_ = true;
        }
        return out;
    }

friend binary_output_packet& operator<<(binary_output_packet& out, char* value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        uint32_t len = strlen(value);
        if (D)
        {
            out << static_cast<uint32_t>(len+1);
            if ( out.m_end_ - out.m_current_ < (len+1) )
            {
                out.m_good_ = false;
                return out;
            }
            memcpy(out.m_current_,value,len);
            *(out.m_current_+len) = static_cast<char>(0);
            out.m_current_ += static_cast<uint32_t>(len+1);
        }
        else
        {
            if ( out.m_current_ - out.m_begin_ < (len+1) )
            {
                out.m_good_ = false;
                return out;
            }
            out << static_cast<char>(0);
            out.m_current_ -= len;
            memcpy(out.m_current_,value,len);
            out << static_cast<uint32_t>(len+1);
        }
        if (D)
        {
            if ( out.m_current_ == out.m_end_ ) out.m_eof_ = true;
        }
        else
        {
            if ( out.m_current_ == out.m_begin_ ) out.m_eof_ = true;
        }
        return out;
    }

friend binary_output_packet& operator<<(binary_output_packet& out, copy_cnstr value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        if (D)
        {
            if ( out.m_end_ - out.m_current_ < static_cast<int32_t>(value.len_+1) + sizeof(value.len_))
            {
                out.m_good_ = false;
                return out;
            }
            out << static_cast<uint32_t>(value.len_+1);
            memcpy(out.m_current_, value.data_, value.len_);
            *(out.m_current_ + value.len_) = static_cast<char>(0);
            out.m_current_ += static_cast<uint32_t>(value.len_+1);
        }
        else
        {
            if ( out.m_current_ - out.m_begin_ < static_cast<int32_t>(value.len_+1) )
            {
                out.m_good_ = false;
                return out;
            }
            out << static_cast<char>(0);
            out.m_current_ -= value.len_;
            memcpy(out.m_current_, value.data_, value.len_);
            out << static_cast<uint32_t>(value.len_+1);
        }
        if (D)
        {
            if ( out.m_current_ == out.m_end_ ) out.m_eof_ = true;
        }
        else
        {
            if ( out.m_current_ == out.m_begin_ ) out.m_eof_ = true;
        }
        return out;
    }

friend binary_output_packet& operator<<(binary_output_packet& out, nocopy_cnstr value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        if (D)
        {
            if ( out.m_end_ - out.m_current_ < static_cast<int32_t>(value.len_+1) + sizeof(value.len_) )
            {
                out.m_good_ = false;
                return out;
            }
            out << static_cast<uint32_t>(value.len_+1);
            memcpy(out.m_current_,value.data_,value.len_);
            *(out.m_current_+value.len_) = static_cast<char>(0);
            out.m_current_ += static_cast<uint32_t>(value.len_+1);
        }
        else
        {
            if ( out.m_current_ - out.m_begin_ < static_cast<int32_t>(value.len_+1) )
            {
                out.m_good_ = false;
                return out;
            }
            out << static_cast<char>(0);
            out.m_current_ -= value.len_;
            memcpy(out.m_current_, value.data_, value.len_);
            out << static_cast<uint32_t>(value.len_+1);
        }
        if (D)
        {
            if ( out.m_current_ == out.m_end_ ) out.m_eof_ = true;
        }
        else
        {
            if ( out.m_current_ == out.m_begin_ ) out.m_eof_ = true;
        }
        return out;
    }

friend binary_output_packet& operator<<(binary_output_packet& out, nocopy_bstr value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        if (D)
        {
            out << value.len_;
            out.write(value.data_, value.len_);
        }
        else
        {
            out.write(value.data_, value.len_);
            out << value.len_;
        }
        return out;        
    }

friend binary_output_packet& operator<<(binary_output_packet& out, nocopy_bnstr value)
    {
        if (!out.m_good_ || out.m_eof_) return out;
        if (D)
        {
            out << value.len_;
            out.write(value.data_, value.len_);
        }
        else
        {
            out.write(value.data_, value.len_);
            out << value.len_;
        }
        return out;
    }
};

#endif
