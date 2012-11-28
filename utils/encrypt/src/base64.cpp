

#include "base64.h"
#include <assert.h>


namespace utils {


static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char rstr[] = {
	-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
	-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  62,  -1,  -1,  -1,  63,
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  -1,  -1,  -1, '=',  -1,  -1,
	-1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  -1,  -1,  -1,  -1,  -1,
	-1,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
	41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  -1,  -1,  -1,  -1,  -1};


// class Base64
void Base64::encode(const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	int padding = 0;
	if (0 != (in_len % 3)) {
		padding = 3 - (in_len % 3);
	}

	int need_len = (in_len + padding) / 3 * 4;
	if (out_buf_len < need_len) {
		out_len = 0;
		return;
	}

	//
	int in_index = 0;
	int out_index = 0;

	in_index = in_len + padding - 3;
	out_index = need_len - 4;

	if (0 == padding)
	{
		char c0 = cb64[(in[in_index] >> 2) & 0x3f];
		char c1 = cb64[((in[in_index] << 4) & 0x30) + ((in[in_index + 1] >> 4) & 0x0f)];
		char c2 = cb64[((in[in_index + 1] << 2) & 0x3c) + ((in[in_index + 2] >> 6) & 0x03)];
		char c3 = cb64[in[in_index + 2] & 0x3f];

		out[out_index] = c0;
		out[out_index + 1] = c1;
		out[out_index + 2] = c2;
		out[out_index + 3] = c3;
	}
	else if (1 == padding)
	{
		char c0 = cb64[(in[in_index] >> 2) & 0x3f];
		char c1 = cb64[((in[in_index] << 4) & 0x30) + ((in[in_index+1] >> 4) & 0x0f)];
		char c2 = cb64[(in[in_index+1] << 2) & 0x3c];
		char c3 = '=';

		out[out_index] = c0;
		out[out_index + 1] = c1;
		out[out_index + 2] = c2;
		out[out_index + 3] = c3;
	}
	else
	{
		char c0 = cb64[(in[in_index] >> 2) & 0x3f];
		char c1 = cb64[(in[in_index] << 4) & 0x30];
		char c2 = '=';
		char c3 = '=';

		out[out_index] = c0;
		out[out_index + 1] = c1;
		out[out_index + 2] = c2;
		out[out_index + 3] = c3;
	}

	if (in_index == 0) {
		out_len = 4;
		return;
	}

	in_index -= 3;
	out_index -= 4;

	while (true)
	{
		char c0 = cb64[(in[in_index] >> 2) & 0x3f];
		char c1 = cb64[((in[in_index] << 4) & 0x30) + ((in[in_index + 1] >> 4) & 0x0f)];
		char c2 = cb64[((in[in_index + 1] << 2) & 0x3c) + ((in[in_index + 2] >> 6) & 0x03)];
		char c3 = cb64[in[in_index + 2] & 0x3f];

		out[out_index] = c0;
		out[out_index + 1] = c1;
		out[out_index + 2] = c2;
		out[out_index + 3] = c3;

		if ((in_index <= 0) || (out_index <= 0))
			break;

		in_index -= 3;
		out_index -= 4;
	}

	assert(in_index == 0);
	assert(out_index == 0);
	out_len = need_len;
	return;
}

void Base64::decode(const char *in, int in_len, char *out, int out_buf_len, int &out_len)
{
	if (0 != (in_len % 4)) {
		out_len = 0;
		return;
	}
	if (in_len < 4) {
		out_len = 0;
		return;
	}

	int need_len = 0;
	if ('=' != in[in_len - 1]) {
		need_len = in_len / 4 * 3;
	}
	else if ('=' != in[in_len - 2]) {
		need_len = in_len / 4 * 3 - 1;
	}
	else {
		need_len = in_len / 4 * 3 - 2;
	}

	if (out_buf_len < need_len) {
		out_len = 0;
		return;
	}

	//
	int in_index = 0;
	int out_index = 0;

	while ((in_len - in_index) > 4)
	{
		unsigned char i0 = in[in_index];
		unsigned char i1 = in[in_index + 1];
		unsigned char i2 = in[in_index + 2];
		unsigned char i3 = in[in_index + 3];

		if ((i0 > 127) || (i1 > 127) || (i2 > 127) || (i3 > 127)) {
			out_len = 0;
			return;
		}
		if ((i0 == '=') || (i1 == '=') || (i2 == '=') || (i3 == '=')) {
			out_len = 0;
			return;
		}

		char d0 = rstr[i0];
		char d1 = rstr[i1];
		char d2 = rstr[i2];
		char d3 = rstr[i3];

		if ((d0 == -1) || (d1 == -1) || (d2 == -1) || (d3 == -1)) {
			out_len = 0;
			return;
		}

		//
		out[out_index] = (char)((d0 << 2 & 0xfc) + (d1 >> 4 & 0x03));
		out[out_index + 1] = (char)((d1 << 4 & 0xf0) + (d2 >> 2 & 0x0f));
		out[out_index + 2] = (char)((d2 << 6 & 0xc0) + d3);

		in_index += 4;
		out_index += 3;
	}

	assert((in_len - in_index) == 4);

	unsigned char i0 = in[in_index];
	unsigned char i1 = in[in_index + 1];
	unsigned char i2 = in[in_index + 2];
	unsigned char i3 = in[in_index + 3];

	if ((i0 > 127) || (i1 > 127) || (i2 > 127) || (i3 > 127)) {
		out_len = 0;
		return;
	}
	if ((i0 == '=') || (i1 == '=')) {
		out_len = 0;
		return;
	}
	if ((i2 == '=') && (i3 != '=')) {
		out_len = 0;
		return;
	}

	char d0 = rstr[i0];
	char d1 = rstr[i1];
	char d2 = rstr[i2];
	char d3 = rstr[i3];

	if ((d0 == -1) || (d1 == -1) || (d2 == -1) || (d3 == -1)) {
		out_len = 0;
		return;
	}

	//
	if ('=' != i3)
	{
		out[out_index] = (char)((d0 << 2 & 0xfc) + (d1 >> 4 & 0x03));
		out[out_index + 1] = (char)((d1 << 4 & 0xf0) + (d2 >> 2 & 0x0f));
		out[out_index + 2] = (char)((d2 << 6 & 0xc0) + d3);
		out_index += 3;
	}
	else if ('=' != i2)	// end with "="
	{
		out[out_index] = (char)((d0 << 2 & 0xfc) + (d1 >> 4 & 0x03));
		out[out_index + 1] = (char)((d1 << 4 & 0xf0) + (d2 >> 2 & 0x0f));
		out_index += 2;
	}
	else	// end with "=="
	{
		out[out_index] = (char)((d0 << 2 & 0xfc) + (d1 >> 4 & 0x03));
		out_index += 1;
	}

	assert(out_index == need_len);
	out_len = out_index;
}


} // namepsace utils

