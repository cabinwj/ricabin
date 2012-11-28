//! @file xtea.h
//! @brief XTEA�㷨
#ifndef _XTEA_H_
#define _XTEA_H_

namespace utils {
//! XTEA�㷨
class XTEA
{
public:
	//! ����
	//! @param key ��Կ
	//! @param in ��������
	//! @param in_len �������ݳ���
	//! @param out ���������
	//! @param out_buf_len �������������
	//! @param out_len �������ݳ���
	//!
	//! in �� out ����ָ��ͬһ������
	static void encipher(const char key[16], const char *in, int in_len, char *out, int out_buf_len, int &out_len);

	//! ����
	//! @param key ��Կ
	//! @param in ��������
	//! @param in_len �������ݳ���
	//! @param out ���������
	//! @param out_buf_len �������������
	//! @param out_len �������ݳ���
	//!
	//! in �� out ����ָ��ͬһ������
	static void decipher(const char key[16], const char *in, int in_len, char *out, int out_buf_len, int &out_len);
};
} // namepsace utils

#endif // _XTEA_H_
