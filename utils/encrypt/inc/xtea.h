//! @file xtea.h
//! @brief XTEA算法
#ifndef _XTEA_H_
#define _XTEA_H_

namespace utils {
//! XTEA算法
class XTEA
{
public:
	//! 加密
	//! @param key 密钥
	//! @param in 输入数据
	//! @param in_len 输入数据长度
	//! @param out 输出缓冲区
	//! @param out_buf_len 输出缓冲区长度
	//! @param out_len 加密数据长度
	//!
	//! in 和 out 可以指向同一缓冲区
	static void encipher(const char key[16], const char *in, int in_len, char *out, int out_buf_len, int &out_len);

	//! 解密
	//! @param key 密钥
	//! @param in 输入数据
	//! @param in_len 输入数据长度
	//! @param out 输出缓冲区
	//! @param out_buf_len 输出缓冲区长度
	//! @param out_len 解密数据长度
	//!
	//! in 和 out 可以指向同一缓冲区
	static void decipher(const char key[16], const char *in, int in_len, char *out, int out_buf_len, int &out_len);
};
} // namepsace utils

#endif // _XTEA_H_
