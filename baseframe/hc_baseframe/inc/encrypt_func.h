#ifndef _ENCRYPT_FUNC_H_
#define _ENCRYPT_FUNC_H_

#include "entity_types.h"

// 加密解密出玩家的signature
int encrypt_signature(const signature_t& signature, char* out, int16_t& out_length);
int decrypt_signature(char* in, int16_t in_length, signature_t& signature);

int encrypt_buffer(const char* key, const char* in, const int32_t in_length,
                   char* out, int32_t& out_length);
int decrypt_buffer(const char* key, const char* in, const int32_t in_length,
                   char* out, int32_t& out_length);

#endif   // _ENCRYPT_FUNC_H_
