#include "xtea.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


namespace utils {

static void encipher_block(int rounds, uint32_t *key, uint32_t *in, uint32_t *out)
{
	uint32_t in0 = in[0];
	uint32_t in1 = in[1];

	uint32_t delta = 0x9e3779b9;
	uint32_t sum = 0;

	for (int i = 0; i < rounds; i++)
	{
		in0 += (((in1 << 4) ^ (in1 >> 5)) + in1) ^ (sum + key[sum & 3]);
		sum += delta;
		in1 += (((in0 << 4) ^ (in0 >> 5)) + in0) ^ (sum + key[(sum>>11) & 3]);
	}

	out[0] = in0;
	out[1] = in1;
}

static void decipher_block(int rounds, uint32_t *key, uint32_t *in, uint32_t *out)
{
	uint32_t in0 = in[0];
	uint32_t in1 = in[1];

	uint32_t delta = 0x9e3779b9;
	uint32_t sum = delta * rounds;

	for (int i = 0; i < rounds; i++)
	{
		in1 -= (((in0 << 4) ^ (in0 >> 5)) + in0) ^ (sum + key[(sum>>11) & 3]);
		sum -= delta;
		in0 -= (((in1 << 4) ^ (in1 >> 5)) + in1) ^ (sum + key[sum & 3]);
	}

	out[0] = in0;
	out[1] = in1;
}


// class XTEA
void XTEA::encipher(const char key[16], const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	if (in_len <= 0)
	{
		out_len = 0;
		return;
	}

	// 计算padding
	int padding = 0;
	if (0 != (in_len % 8))
	{
		padding = 8 - (in_len % 8);
	}

	if (out_buf_len < (1 + padding + in_len + 7))
	{
		out_len = 0;
		return;
	}

	out_len = 1 + padding + in_len + 7;

	// 开始加密
	int in_index = 0;
	int out_index = 0;

	char plain_temp[2][8];
	char cipher_temp[2][8];
	char *last_plain, *this_plain;
	char *last_cipher, *this_cipher;

	// 1
	last_plain = NULL;
	this_plain = plain_temp[0];
	last_cipher = NULL;
	this_cipher = cipher_temp[0];

	this_plain[0] = ((char)rand() & 0xf8) | (char)padding;
	for (int i = 1; i <= padding; i++)
	{
		this_plain[i] = (char)rand();
	}
	for (int i = 1 + padding; i < 8; i++)
	{
		this_plain[i] = in[in_index++];
	}

	encipher_block(32, (uint32_t*)key, (uint32_t*)this_plain, (uint32_t*)this_cipher);

	last_plain = this_plain;
	last_cipher = this_cipher;

	this_plain = plain_temp[1];
	this_cipher = cipher_temp[1];

	// 2
	while ((in_len - in_index) >= 8)
	{
		memcpy(this_plain, in + in_index, 8);

		char prepare_plain[8];
		for (int i = 0; i < 8; i++) {
			prepare_plain[i] = this_plain[i] ^ last_cipher[i];
		}

		encipher_block(32, (uint32_t*)key, (uint32_t*)prepare_plain, (uint32_t*)this_cipher);

		for (int i = 0; i < 8; i++) {
			this_cipher[i] ^= last_plain[i];
		}

		memcpy(out + out_index, last_cipher, 8);
		out_index += 8;

		char *tp = last_plain;
		char *tc = last_cipher;

		last_plain = this_plain;
		last_cipher = this_cipher;

		this_plain = tp;
		this_cipher = tc;

		in_index += 8;
	}

	// 3
	assert((in_len - in_index) == 1);
	this_plain[0] = in[in_index];
	for (int i = 1; i < 8; i++)
	{
		this_plain[i] = 0;
	}

	char prepare_plain[8];
	for (int i = 0; i < 8; i++) {
		prepare_plain[i] = this_plain[i] ^ last_cipher[i];
	}

	encipher_block(32, (uint32_t*)key, (uint32_t*)prepare_plain, (uint32_t*)this_cipher);

	for (int i = 0; i < 8; i++) {
		this_cipher[i] ^= last_plain[i];
	}

	memcpy(out + out_index, last_cipher, 8);
	out_index += 8;

	memcpy(out + out_index, this_cipher, 8);
	out_index += 8;

	assert(out_index == out_len);
	return;
}

void XTEA::decipher(const char key[16], const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	if ((in_len < 16) || (0 != (in_len % 8)))
	{
		out_len = 0;
		return;
	}

	// 开始解密
	int in_index = 0;
	int out_index = 0;

	char plain_temp[2][8];
	char cipher_temp[2][8];
	char *last_plain, *this_plain;
	char *last_cipher, *this_cipher;

	// 1
	last_cipher = NULL;
	this_cipher = cipher_temp[0];
	last_plain = NULL;
	this_plain = plain_temp[0];
	memcpy(this_cipher, in + in_index, 8);
	in_index += 8;

	decipher_block(32, (uint32_t*)key, (uint32_t*)this_cipher, (uint32_t*)this_plain);

	int padding = this_plain[0] & 0x07;

	if (out_buf_len < (in_len - 1 - padding - 7))
	{
		out_len = 0;
		return;
	}
	out_len = in_len - 1 - padding - 7;

	memcpy(out + out_index, this_plain + 1 + padding, 8 - 1 - padding);
	out_index += (8 - 1 - padding);

	last_plain = this_plain;
	last_cipher = this_cipher;

	this_plain = plain_temp[1];
	this_cipher = cipher_temp[1];

	// 2
	while ((in_len - in_index) > 8)
	{
		memcpy(this_cipher, in + in_index, 8);

		char prepare_cipher[8];
		for (int i = 0; i < 8; i++) {
			prepare_cipher[i] = this_cipher[i] ^ last_plain[i];
		}

		decipher_block(32, (uint32_t*)key, (uint32_t*)prepare_cipher, (uint32_t*)this_plain);

		for (int i = 0; i < 8; i++) {
			this_plain[i] ^= last_cipher[i];
		}

		memcpy(out + out_index, this_plain, 8);
		out_index += 8;

		char *tp = last_plain;
		char *tc = last_cipher;

		last_plain = this_plain;
		last_cipher = this_cipher;

		this_plain = tp;
		this_cipher = tc;

		in_index += 8;
	}

	// 3
	assert((in_len - in_index) == 8);
	memcpy(this_cipher, in + in_index, 8);

	char prepare_cipher[8];
	for (int i = 0; i < 8; i++) {
		prepare_cipher[i] = this_cipher[i] ^ last_plain[i];
	}

	decipher_block(32, (uint32_t*)key, (uint32_t*)prepare_cipher, (uint32_t*)this_plain);

	for (int i = 0; i < 8; i++) {
		this_plain[i] ^= last_cipher[i];
	}

	memcpy(out + out_index, this_plain, (8 - 7));
	out_index += (8 - 7);

	for (int i = 1; i < 8; i++)
	{
		if (0 != this_plain[i]) {
			out_len = 0;
			return;
		}
	}

	assert(out_index == out_len);
	return;
}


} // namepsace utils

