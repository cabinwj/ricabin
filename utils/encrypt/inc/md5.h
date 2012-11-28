//! @file md5.h
//! @brief MD5摘要算法


#ifndef _MD5_H_
#define _MD5_H_


namespace utils {


//! MD5摘要
//! @param data 输入数据
//! @param data_len 输入数据长度
//! @param digest MD5摘要
//!
//! data 和 digest 可以指向同一缓冲区
void Md5(const char *data, int data_len, char digest[16]);


} // namepsace utils


#endif // _MD5_H_
