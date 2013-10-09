//! @file packet_splitter.h 
#ifndef _PACKET_SPLITTER_H_
#define _PACKET_SPLITTER_H_

#include "hc_types.h"


//! @class packet_splitter
//! @brief tcp����ӿ�
class ipacket_splitter
{
public:
    virtual ~ipacket_splitter() { }

    //! ���
    //! @param buf ���ݻ���
    //! @param len ���ݳ���
    //! @param packet_len ���ز��� ������
    //! @return -1:���ݸ�ʽ����, 0:������û��������, 1:�õ�һ����
    virtual int split(const char* buf, int len, int& pkg_len) = 0;

public:
    static ipacket_splitter* Instance();
};

//! @class string_splitter
//! @brief �ı���Э������
//!
//! ÿ���ı�Ϊһ����, CR LF �ַ��Ѿ�ȥ��
class string_splitter : public ipacket_splitter
{
public:
    virtual ~string_splitter() { }

    //! ���
    //! @param buf ���ݻ���
    //! @param len ���ݳ���
    //! @param packet_len ���ز��� ������
    //! @return -1:���ݸ�ʽ����, 0:������û��������, 1:�õ�һ����
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static string_splitter* Instance();
};


//! @class binary_splitter
//! @brief ������Э��tcp�����
//!
//! ��ǰ���ֽ�Ϊ�����ֽ���İ���
class binary2_splitter : public ipacket_splitter
{
public:
    virtual ~binary2_splitter() { }

public:
    //! ���
    //! @param buf ���ݻ���
    //! @param len ���ݳ���
    //! @param packet_len ���ز��� ������
    //! @return -1:���ݸ�ʽ����, 0:������û��������, 1:�õ�һ����
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static binary2_splitter* Instance();
};

class binary4_splitter : public ipacket_splitter
{
public:
    virtual ~binary4_splitter() { }

public:
    //! ���
    //! @param buf ���ݻ���
    //! @param len ���ݳ���
    //! @param packet_len ���ز��� ������
    //! @return -1:���ݸ�ʽ����, 0:������û��������, 1:�õ�һ����
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static binary4_splitter* Instance();
};


//! @class line_splitter
//! @brief �ı���Э������
//!
//! ÿ���ı�Ϊһ����, CR LF �ַ��Ѿ�ȥ��
class line_splitter : public ipacket_splitter
{
public:
    virtual ~line_splitter() { }

    //! ���
    //! @param buf ���ݻ���
    //! @param len ���ݳ���
    //! @param packet_begin ���ز��� ����ʼλ��
    //! @param packet_len ���ز��� ������
    //! @return -1:���ݸ�ʽ����, 0:������û��������, 1:�õ�һ����
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static line_splitter* Instance();
};


#endif // _PACKET_SPLITTER_H_
