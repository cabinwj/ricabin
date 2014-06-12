//! @file blow_fish.h
//! @brief BlowFish算法
#ifndef _BLOW_FISH_H_
#define _BLOW_FISH_H_

#include <stddef.h>                // for size_t
#include <stdint.h>


namespace utils {
//! 初始化盒
//! @param key 密钥
//! @param key_len 密钥长度
//! @param key_pbox 盒
//! @param key_sbox 盒
void BlowFish_Initialize(char *key, int key_len, uint32_t key_pbox[18], uint32_t key_sbox[4][256]);


//! 加密
//! @param key_pbox 盒
//! @param key_sbox 盒
//! @param in 输入数据
//! @param in_len 输入数据长度
//! @param out 输出缓冲区
//! @param out_buf_len 输出缓冲区长度
//! @param out_len 加密数据长度
//!
//! in 和 out 可以指向同一缓冲区
void BlowFish_Encipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], const char *in, int in_len, char *out, int out_buf_len, int &out_len);


//! 解密
//! @param key_pbox 盒
//! @param key_sbox 盒
//! @param in 输入数据
//! @param in_len 输入数据长度
//! @param out 输出缓冲区
//! @param out_buf_len 输出缓冲区长度
//! @param out_len 解密数据长度
//!
//! in 和 out 可以指向同一缓冲区
void BlowFish_Decipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], const char *in, int in_len, char *out, int out_buf_len, int &out_len);




//! BlowFish算法
class BlowFish
{
public:
    //! 初始化盒
    //! @param key 密钥
    //! @param key_len 密钥长度
    void initialize(char *key, int key_len);

    //! 加密
    //! @param in 输入数据
    //! @param in_len 输入数据长度
    //! @param out 输出缓冲区
    //! @param out_buf_len 输出缓冲区长度
    //! @param out_len 加密数据长度
    //!
    //! in 和 out 可以指向同一缓冲区
    void encipher(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

    //! 解密
    //! @param in 输入数据
    //! @param in_len 输入数据长度
    //! @param out 输出缓冲区
    //! @param out_buf_len 输出缓冲区长度
    //! @param out_len 解密数据长度
    //!
    //! in 和 out 可以指向同一缓冲区
    void decipher(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

private:
    uint32_t m_key_pbox[18];
    uint32_t m_key_sbox[4][256];
};


} // namepsace utils


#endif // _BLOW_FISH_H_
