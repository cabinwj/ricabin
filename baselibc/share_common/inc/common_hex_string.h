//! @file hex_string.h
//! @brief ʮ�������ַ���
#ifndef _COMMON_HEX_STRING_H_
#define _COMMON_HEX_STRING_H_

//! ʮ�������ַ���������
class hex_string
{
public:
    //! ����������תʮ�������ַ���
    //! @param in ��������
    //! @param in_len �������ݳ���
    //! @param out ���������
    //! @param out_buf_len �������������
    //! @param out_len ������ݳ���
    //!
    //! in �� out ����ָ��ͬһ������
    static void bin2asc(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

    //! ʮ�������ַ���ת����������
    //! @param in ��������
    //! @param in_len �������ݳ���
    //! @param out ���������
    //! @param out_buf_len �������������
    //! @param out_len ������ݳ���
    //!
    //! in �� out ����ָ��ͬһ������
    static void asc2bin(const char *in, int in_len, char *out, int out_buf_len, int &out_len);

private:
    static const char* asc_map[256];
    static unsigned char bin_map[256];
};

#endif // _COMMON_HEX_STRING_H_
