//! @file packet_splitter.h 
#ifndef _PACKET_SPLITTER_H_
#define _PACKET_SPLITTER_H_

#include "hc_types.h"


//! @class packet_splitter
//! @brief tcp拆包接口
class ipacket_splitter
{
public:
    virtual ~ipacket_splitter() { }

    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char* buf, int len, int& pkg_len) = 0;

public:
    static ipacket_splitter* Instance();
};

//! @class string_splitter
//! @brief 文本行协议拆包器
//!
//! 每行文本为一个包, CR LF 字符已经去掉
class string_splitter : public ipacket_splitter
{
public:
    virtual ~string_splitter() { }

    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static string_splitter* Instance();
};


//! @class binary_splitter
//! @brief 二进制协议tcp拆包器
//!
//! 包前两字节为网络字节序的包长
class binary2_splitter : public ipacket_splitter
{
public:
    virtual ~binary2_splitter() { }

public:
    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static binary2_splitter* Instance();
};

class binary4_splitter : public ipacket_splitter
{
public:
    virtual ~binary4_splitter() { }

public:
    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static binary4_splitter* Instance();
};


//! @class line_splitter
//! @brief 文本行协议拆包器
//!
//! 每行文本为一个包, CR LF 字符已经去掉
class line_splitter : public ipacket_splitter
{
public:
    virtual ~line_splitter() { }

    //! 拆包
    //! @param buf 数据缓存
    //! @param len 数据长度
    //! @param packet_begin 返回参数 包起始位置
    //! @param packet_len 返回参数 包长度
    //! @return -1:数据格式错误, 0:缓存中没有完整包, 1:得到一个包
    virtual int split(const char *buf, int len, int& packet_len);

public:
    static line_splitter* Instance();
};


#endif // _PACKET_SPLITTER_H_
