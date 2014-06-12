//! @file blow_fish.h
//! @brief BlowFish�㷨
#ifndef _BLOW_FISH_H_
#define _BLOW_FISH_H_

#include <stddef.h>                // for size_t
#include <stdint.h>


namespace utils {
//! ��ʼ����
//! @param key ��Կ
//! @param key_len ��Կ����
//! @param key_pbox ��
//! @param key_sbox ��
void BlowFish_Initialize(char *key, int key_len, uint32_t key_pbox[18], uint32_t key_sbox[4][256]);


//! ����
//! @param key_pbox ��
//! @param key_sbox ��
//! @param in ��������
//! @param in_len �������ݳ���
//! @param out ���������
//! @param out_buf_len �������������
//! @param out_len �������ݳ���
//!
//! in �� out ����ָ��ͬһ������
void BlowFish_Encipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], const char *in, int in_len, char *out, int out_buf_len, int &out_len);


//! ����
//! @param key_pbox ��
//! @param key_sbox ��
//! @param in ��������
//! @param in_len �������ݳ���
//! @param out ���������
//! @param out_buf_len �������������
//! @param out_len �������ݳ���
//!
//! in �� out ����ָ��ͬһ������
void BlowFish_Decipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], const char *in, int in_len, char *out, int out_buf_len, int &out_len);




//! BlowFish�㷨
class BlowFish
{
public:
    //! ��ʼ����
    //! @param key ��Կ
    //! @param key_len ��Կ����
    void initialize(char *key, int key_len);

    //! ����
    //! @param in ��������
    //! @param in_len �������ݳ���
    //! @param out ���������
    //! @param out_buf_len �������������
    //! @param out_len �������ݳ���
    //!
    //! in �� out ����ָ��ͬһ������
    void encipher(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

    //! ����
    //! @param in ��������
    //! @param in_len �������ݳ���
    //! @param out ���������
    //! @param out_buf_len �������������
    //! @param out_len �������ݳ���
    //!
    //! in �� out ����ָ��ͬһ������
    void decipher(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

private:
    uint32_t m_key_pbox[18];
    uint32_t m_key_sbox[4][256];
};


} // namepsace utils


#endif // _BLOW_FISH_H_
