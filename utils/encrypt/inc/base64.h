//! @file base64.h
//! @brief base64编码


#ifndef _BASE64_H_
#define _BASE64_H_


namespace utils {


//! base64编码
class Base64
{
public:
	//! 编码
	//! @param in 输入数据
	//! @param in_len 输入数据长度
	//! @param out 输出缓冲区
	//! @param out_buf_len 输出缓冲区长度
	//! @param out_len 编码后的数据长度
	//!
	//! in 和 out 可以指向同一缓冲区
	static void encode(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

	//! 解码
	//! @param in 输入数据
	//! @param in_len 输入数据长度
	//! @param out 输出缓冲区
	//! @param out_buf_len 输出缓冲区长度
	//! @param out_len 解码后的数据长度
	//!
	//! in 和 out 可以指向同一缓冲区
	static void decode(const char *in, int in_len, char *out, int out_buf_len, int &out_len);
};


} // namepsace utils


#endif // _BASE64_H_
