#include "packet_splitter.h"
#include "hc_os.h"
#include "hc_log.h"
#include "config.h"

// class string_splitter
int string_splitter::split(const char* buf, int len, int& real_pkglen)
{
    if (len < 6) 
    {
        return 0;
    }

    int pack_len = 0;
    //sscanf(buf, "%6x", &pack_len);
    
    //if ((pack_len + 6)> MAX_PACKET_LENGTH)
    //{
    //    LOG_DEBUG("Exceed max packet len, max=%d pack_len=%d", MAX_PACKET_LENGTH, pack_len);

    //    LOG_HEX_PREFIX("RECV msg:", buf, len, utils::L_ERROR);
    //    return -1;
    //}

    if (len >= (pack_len+6))
    {
        real_pkglen = pack_len+6;
        return 1;
    }

    real_pkglen = pack_len+6;

    LOG_DEBUG("packet not complete, len=%d pack_len=%d", len, pack_len);
    return 0;

#if 0
    int pack_len = 0;
    packet_begin = 0;
    for (int i = 0; i < 4; i++)
    {
        if (buf[i] >= '0' && buf[i] <= '9')
        {
            pack_len = pack_len*0x10+(buf[i]-'0');
        }
        else if (buf[i] >= 'A' && buf[i] <= 'F')
        {
            pack_len = pack_len*0x10+(buf[i]-'A')+10;
        }
        else if (buf[i] >= 'a' && buf[i] <= 'f') 
        {
            pack_len = pack_len*0x10+(buf[i]-'a')+10;
        }
        else
        {
            packet_begin++;
        }
    }
    if (pack_len < 2) {
        return -1;
    }
    if (pack_len+4 > MAX_PACKET_LENGTH) {
        return -1;
    }

    if (len >= pack_len+4) {
        packet_len = pack_len+4;
        if (buf[packet_len] == '\r') packet_len++;
        if (buf[packet_len] == '\n') packet_len++;
        return 1;
    }

    return 0;
#endif
}

string_splitter* string_splitter::Instance()
{
    static string_splitter __split;
    return &__split;
}


// class binary2_splitter
int binary2_splitter::split(const char* buf, int len, int& real_pkglen)
{
    uint8_t min_len = sizeof(uint16_t);
    if (len < min_len)
    {
        real_pkglen = 0;
        LOG(TRACE)("binary_splitter::split, buffer_len:%d, min_len:%d", len, min_len);
        return 0;
    }

    int pkg_len = ntohs(*(uint16_t*)buf);
    LOG(TRACE)("binary_splitter::split, buffer_len:%d, min_len:%d, pkg_len:%d", len, min_len, pkg_len);
    if (pkg_len < min_len)
    {
        return -1;
    }

    if (len >= pkg_len)
    {
        real_pkglen = pkg_len;
        return 1;
    }

    if (len < pkg_len)    //缓冲区中的数据不足一个包
    {
        real_pkglen = pkg_len;
    }

    return 0;
}

binary2_splitter* binary2_splitter::Instance()
{
    static binary2_splitter __split;
    return &__split;
}

// class binary4_splitter
int binary4_splitter::split(const char* buf, int len, int& real_pkglen)
{
    uint8_t min_len = sizeof(uint32_t);
    if (len < min_len)
    {
        real_pkglen = 0;
        LOG(TRACE)("binary_splitter::split, buffer_len:%d, min_len:%d", len, min_len);
        return 0;
    }

    int pkg_len = ntohl(*(uint32_t*)buf);
    LOG(TRACE)("binary_splitter::split, buffer_len:%d, min_len:%d, pkg_len:%d", len, min_len, pkg_len);
    if (pkg_len < min_len)
    {
        return -1;
    }

    if (len >= pkg_len)
    {
        real_pkglen = pkg_len;
        return 1;
    }

    if (len < pkg_len)    //缓冲区中的数据不足一个包
    {
        real_pkglen = pkg_len;
    }

    return 0;
}

binary4_splitter* binary4_splitter::Instance()
{
    static binary4_splitter __split;
    return &__split;
}


// class line_splitter
int line_splitter::split(const char* buf, int len, int& real_pkglen)
{
    int p1 = -1;
    int p2 = -1;

    // 找到本行的起始字符(第一个非/r/n的字符)
    for (int i = 0; i < len; i++)
    {
        if ((buf[i] != '\r') && (buf[i] != '\n'))
        {
            p1 = i;
            break;
        }
    }
    // 没找到第一个非/r/n的字符
    if (-1 == p1)
    {
        if (len > MAX_PACKET_LENGTH)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    // 找到本行的结束字符(/r/n)
    for (int i = p1; i < len; i++)
    {
        if ((buf[i] == '\r') || (buf[i] == '\n'))
        {
            p2 = i - 1;
            break;
        }
    }
    // 没找到本行的结束字符(/r/n)
    if (-1 == p2)
    {
        if (len > MAX_PACKET_LENGTH)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    real_pkglen = p2 - p1 + 1;
    if (real_pkglen > MAX_PACKET_LENGTH)
    {
        return -1;
    }

    return 1;
}

line_splitter* line_splitter::Instance()
{
    static line_splitter __split;
    return &__split;
}

packet_splitter* packet_splitter::Instance()
{
    return binary4_splitter::Instance();
}
