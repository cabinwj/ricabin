//! @file hex_string.h
//! @brief 十六进制字符串
#ifndef _COMMON_HEX_STRING_H_
#define _COMMON_HEX_STRING_H_

//! 十六进制字符串工具类
class hex_string
{
public:
    //! 二进制数据转十六进制字符串
    //! @param in 输入数据
    //! @param in_len 输入数据长度
    //! @param out 输出缓冲区
    //! @param out_buf_len 输出缓冲区长度
    //! @param out_len 输出数据长度
    //!
    //! in 和 out 可以指向同一缓冲区
    static void bin2asc(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

    //! 十六进制字符串转二进制数据
    //! @param in 输入数据
    //! @param in_len 输入数据长度
    //! @param out 输出缓冲区
    //! @param out_buf_len 输出缓冲区长度
    //! @param out_len 输出数据长度
    //!
    //! in 和 out 可以指向同一缓冲区
    static void asc2bin(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

private:
    static const char* asc_map[256];
    static unsigned char bin_map[256];
};

#endif // _COMMON_HEX_STRING_H_
