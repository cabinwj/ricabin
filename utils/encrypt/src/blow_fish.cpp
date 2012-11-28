

#include "blow_fish.h"
#include <string.h>
#include <assert.h>


#include "blow_fish.inl"


namespace utils {


static inline uint32_t make_dword(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
	return (uint32_t(byte0) << 24) | (uint32_t(byte1) << 16) | (uint32_t(byte2) << 8) | uint32_t(byte3);
}

static inline uint8_t byte_0(uint32_t x)
{
	return uint8_t((x >> 24) & 0xff);
}

static inline uint8_t byte_1(uint32_t x)
{
	return uint8_t((x >> 16) & 0xff);
}

static inline uint8_t byte_2(uint32_t x)
{
	return uint8_t((x >> 8) & 0xff);
}

static inline uint8_t byte_3(uint32_t x)
{
	return uint8_t(x & 0xff);
}


static inline uint32_t F(uint32_t key_sbox[4][256], uint32_t x)
{
	return ((key_sbox[0][byte_0(x)] + key_sbox[1][byte_1(x)]) ^ key_sbox[2][byte_2(x)]) + key_sbox[3][byte_3(x)];
}

static inline void encipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], uint32_t &left, uint32_t &right)
{
	uint32_t l = left;
	uint32_t r = right;

	l ^= key_pbox[0];
	r ^= F(key_sbox, l) ^ key_pbox[1];	l ^= F(key_sbox, r) ^ key_pbox[2];
	r ^= F(key_sbox, l) ^ key_pbox[3];	l ^= F(key_sbox, r) ^ key_pbox[4];
	r ^= F(key_sbox, l) ^ key_pbox[5];	l ^= F(key_sbox, r) ^ key_pbox[6];
	r ^= F(key_sbox, l) ^ key_pbox[7];	l ^= F(key_sbox, r) ^ key_pbox[8];
	r ^= F(key_sbox, l) ^ key_pbox[9];	l ^= F(key_sbox, r) ^ key_pbox[10];
	r ^= F(key_sbox, l) ^ key_pbox[11];	l ^= F(key_sbox, r) ^ key_pbox[12];
	r ^= F(key_sbox, l) ^ key_pbox[13];	l ^= F(key_sbox, r) ^ key_pbox[14];
	r ^= F(key_sbox, l) ^ key_pbox[15];	l ^= F(key_sbox, r) ^ key_pbox[16];
	r ^= key_pbox[17];

	left = r;
	right = l;
}

static inline void decipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], uint32_t &left, uint32_t &right)
{
	uint32_t l = left;
	uint32_t r = right;

	l ^= key_pbox[17];
	r ^= F(key_sbox, l) ^ key_pbox[16];	l ^= F(key_sbox, r) ^ key_pbox[15];
	r ^= F(key_sbox, l) ^ key_pbox[14];	l ^= F(key_sbox, r) ^ key_pbox[13];
	r ^= F(key_sbox, l) ^ key_pbox[12];	l ^= F(key_sbox, r) ^ key_pbox[11];
	r ^= F(key_sbox, l) ^ key_pbox[10];	l ^= F(key_sbox, r) ^ key_pbox[9];
	r ^= F(key_sbox, l) ^ key_pbox[8];	l ^= F(key_sbox, r) ^ key_pbox[7];
	r ^= F(key_sbox, l) ^ key_pbox[6];	l ^= F(key_sbox, r) ^ key_pbox[5];
	r ^= F(key_sbox, l) ^ key_pbox[4];	l ^= F(key_sbox, r) ^ key_pbox[3];
	r ^= F(key_sbox, l) ^ key_pbox[2];	l ^= F(key_sbox, r) ^ key_pbox[1];
	r ^= key_pbox[0];

	left = r;
	right = l;
}


void BlowFish_Initialize(char *key, int key_len, uint32_t key_pbox[18], uint32_t key_sbox[4][256])
{
	// 1
	memcpy((char*)key_pbox, (char*)bf_pbox, sizeof(bf_pbox));
	memcpy((char*)key_sbox, (char*)bf_sbox, sizeof(bf_sbox));

	// 2
	for (int i = 0; i < 18; i++)
	{
		uint32_t k = make_dword(key[(i * 4 + 0) % key_len], key[(i * 4 + 1) % key_len], key[(i * 4 + 2) % key_len], key[(i * 4 + 3) % key_len]);
		key_pbox[i] ^= k;
	}

	// 3
	uint32_t l = 0;
	uint32_t r = 0;

	for (int i = 0; i < 18; i += 2)
	{
		encipher(key_pbox, key_sbox, l, r);
		key_pbox[i] = l;
		key_pbox[i + 1] = r;
	}

	// 4
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 256; j += 2)
		{
			encipher(key_pbox, key_sbox, l, r);
			key_sbox[i][j] = l;
			key_sbox[i][j + 1] = r;
		}
	}
}

void BlowFish_Encipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	if (in_len <= 0)
	{
		out_len = 0;
		return;
	}

	int padding = 0;
	if (0 != (in_len % 8))
	{
		padding = 8 - (in_len % 8);
	}

	if (out_buf_len < in_len + padding)
	{
		out_len = 0;
		return;
	}

	out_len = in_len + padding;

	//
	int in_index = 0;
	int out_index = 0;
	while ((in_len - in_index) >= 8)
	{
		uint32_t left = *(uint32_t*)(in + in_index);
		uint32_t right = *(uint32_t*)(in + in_index + 4);

		encipher(key_pbox, key_sbox, left, right);

		in_index += 8;

		*(uint32_t*)(out + out_index) = left;
		*(uint32_t*)(out + out_index + 4) = right;
		out_index += 8;
	}

	if ((in_len - in_index) > 0)
	{
		char temp[8];
		memset(temp, 0, sizeof(temp));
		memcpy(temp, in + in_index, in_len - in_index);

		uint32_t left = *(uint32_t*)temp;
		uint32_t right = *(uint32_t*)(temp + 4);

		encipher(key_pbox, key_sbox, left, right);

		*(uint32_t*)(out + out_index) = left;
		*(uint32_t*)(out + out_index + 4) = right;
		out_index += 8;
	}

	assert(out_index == out_len);
	return;
}

void BlowFish_Decipher(uint32_t key_pbox[18], uint32_t key_sbox[4][256], const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	if (in_len <= 0)
	{
		out_len = 0;
		return;
	}

	if (0 != (in_len % 8))
	{
		out_len = 0;
		return;
	}

	out_len = in_len;

	//
	int in_index = 0;
	int out_index = 0;
	while ((in_len - in_index) >= 8)
	{
		uint32_t left = *(uint32_t*)(in + in_index);
		uint32_t right = *(uint32_t*)(in + in_index + 4);

		decipher(key_pbox, key_sbox, left, right);

		in_index += 8;

		*(uint32_t*)(out + out_index) = left;
		*(uint32_t*)(out + out_index + 4) = right;
		out_index += 8;
	}

	assert((in_len - in_index) == 0);
	assert(out_index == out_len);
	return;
}



// class BlowFish
void BlowFish::initialize(char *key, int key_len)
{
	BlowFish_Initialize(key, key_len, m_key_pbox, m_key_sbox);
}

void BlowFish::encipher(const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	BlowFish_Encipher(m_key_pbox, m_key_sbox, in, in_len, out, out_buf_len, out_len);
}

void BlowFish::decipher(const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	BlowFish_Decipher(m_key_pbox, m_key_sbox, in, in_len, out, out_buf_len, out_len);
}


} // namepsace utils

