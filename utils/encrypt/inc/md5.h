//! @file md5.h
//! @brief MD5ժҪ�㷨


#ifndef _MD5_H_
#define _MD5_H_


namespace utils {


//! MD5ժҪ
//! @param data ��������
//! @param data_len �������ݳ���
//! @param digest MD5ժҪ
//!
//! data �� digest ����ָ��ͬһ������
void Md5(const char *data, int data_len, char digest[16]);


} // namepsace utils


#endif // _MD5_H_
