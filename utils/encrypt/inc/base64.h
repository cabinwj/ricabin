//! @file base64.h
//! @brief base64����


#ifndef _BASE64_H_
#define _BASE64_H_


namespace utils {


//! base64����
class Base64
{
public:
	//! ����
	//! @param in ��������
	//! @param in_len �������ݳ���
	//! @param out ���������
	//! @param out_buf_len �������������
	//! @param out_len ���������ݳ���
	//!
	//! in �� out ����ָ��ͬһ������
	static void encode(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

	//! ����
	//! @param in ��������
	//! @param in_len �������ݳ���
	//! @param out ���������
	//! @param out_buf_len �������������
	//! @param out_len ���������ݳ���
	//!
	//! in �� out ����ָ��ͬһ������
	static void decode(const char *in, int in_len, char *out, int out_buf_len, int &out_len);
};


} // namepsace utils


#endif // _BASE64_H_
