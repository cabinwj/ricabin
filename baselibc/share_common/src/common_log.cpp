#include "common_log.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef WIN32
#include <winsock2.h>
#if _MSC_VER <= 1400
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if WIN32
#define DATE_START  0
char logc::m_level_str_[][64] = {
    "2008-11-07 09:35:00 FATAL ",
    "2008-11-07 09:35:00 ERROR ",
    "2008-11-07 09:35:00 WARN  ",
    "2008-11-07 09:35:00 INFO  ",
    "2008-11-07 09:35:00 TRACE ",
    "2008-11-07 09:35:00 DEBUG "
};

char logc::m_level_str_usec_[][64] = {
    "\033[1;31m2008-11-07 09:35:00.000000 FATAL ",
    "\033[1;33m2008-11-07 09:35:00.000000 ERROR ",
    "\033[1;35m2008-11-07 09:35:00.000000 WARN    ",
    "\033[1;32m2008-11-07 09:35:00.000000 INFO    ",
    "\033[0;00m2008-11-07 09:35:00.000000 TRACE ",
    "\033[0;00m2008-11-07 09:35:00.000000 DEBUG "
};
#else
#define DATE_START  7
char logc::m_level_str_[][64] = {
    "\033[1;31m2008-11-07 09:35:00 FATAL ",
    "\033[1;33m2008-11-07 09:35:00 ERROR ",
    "\033[1;35m2008-11-07 09:35:00 WARN  ",
    "\033[1;32m2008-11-07 09:35:00 INFO  ",
    "\033[0;00m2008-11-07 09:35:00 TRACE ",
    "\033[0;00m2008-11-07 09:35:00 DEBUG "
};

char logc::m_level_str_usec_[][64] = {
    "\033[1;31m2008-11-07 09:35:00.000000 FATAL ",
    "\033[1;33m2008-11-07 09:35:00.000000 ERROR ",
    "\033[1;35m2008-11-07 09:35:00.000000 WARN  ",
    "\033[1;32m2008-11-07 09:35:00.000000 INFO  ",
    "\033[0;00m2008-11-07 09:35:00.000000 TRACE ",
    "\033[0;00m2008-11-07 09:35:00.000000 DEBUG "
};

#endif

#define TIME_START  (DATE_START + 11)

logc::logc()
{
    memset(m_file_name_, 0, sizeof(m_file_name_));
    m_max_size_ = 500 * 1024 * 1024;  // 500M
    m_file_ = NULL;
    m_max_level_ = L_INFO;
    m_enable_usec_ = false;
}

logc::~logc()
{
    close();
}

int logc::set_file_name(const char* filename)
{
    // 已经打开，不能设置了
    if (NULL != m_file_)
    {
        return -1;
    }

    strcpy(m_file_name_, filename);
    return 0;
}

void logc::set_max_size(size_t maxsize)
{
    // 不立即生效
    m_max_size_ = maxsize;
}

void logc::set_max_level(log_level_t level)
{
    if (0 != (L_LEVEL_MAX & level))
    {
        m_max_level_ = (log_level_t)(L_LEVEL_MAX & level);
        return;
    }

    m_max_level_ = L_LEVEL_MAX;
}

void logc::set_usec(bool in_enable_usec)
{
    m_enable_usec_ = in_enable_usec;
}

int logc::open()
{
    if (NULL != m_file_)
    {
        return -1;
    }

    char name[MAX_PATH_LEN];
    size_t len = 0;

    strncpy(name, m_file_name_, MAX_PATH_LEN);
    len = strlen(name);

    time_t t;
    time(&t);
    struct tm lt = *localtime(&t);
    strftime(name + len, MAX_PATH_LEN - len, "-%Y%m%d-%H%M%S.log", &lt);

    m_file_ = fopen(name, "a+");
    if (NULL == m_file_)
    {
        return -1;
    }

    // 填写日志记录中的日期，在一天之内就不用填写了
    strftime(name, 12, "%Y-%m-%d", &lt);
    for (int i = 0; (1 << i) < L_LEVEL_MAX; i++)
    {
        memcpy(m_level_str_[i] + DATE_START, name, 10);
    }

    for (int i = 0; (1 << i) < L_LEVEL_MAX; i++)
    {
        memcpy(m_level_str_usec_[i] + DATE_START, name, 10);
    }

    lt.tm_hour = lt.tm_min = lt.tm_sec = 0;
    m_middle_night_ = mktime(&lt);

    return 0;
}

int logc::close()
{
    if (NULL == m_file_)
    {
        return -1;
    }

    fclose(m_file_);
    m_file_ = NULL;

    return 0;
}

int logc::log(log_level_t level, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(level, fmt, ap); // not safe
    va_end(ap);
    return ret;
}

int logc::log_fatal(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(L_FATAL, fmt, ap);
    va_end(ap);
    return ret;
}

int logc::log_error(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(L_ERROR, fmt, ap);
    va_end(ap);
    return ret;
}

int logc::log_warn(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(L_WARN, fmt, ap);
    va_end(ap);
    return ret;
}

int logc::log_info(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(L_INFO, fmt, ap);
    va_end(ap);
    return ret;
}

int logc::log_trace(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(L_TRACE, fmt, ap);
    va_end(ap);
    return ret;
}

int logc::log_debug(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = vlog(L_DEBUG, fmt, ap);
    va_end(ap);
    return ret;
}

int logc::get_level_str_index(log_level_t level)
{
    if (0 == (level & m_max_level_))
    {
        return 0;
    }

    int lv = 0;
    for (lv = 0; (1 << lv) < L_LEVEL_MAX; lv++)
    {
        if ((1 << lv) == (m_max_level_ & level))
        {
            return lv;
        }
    }

    return 0;
}

int logc::vlog(log_level_t level, const char* fmt, va_list ap)
{
    if ((0 == (level & m_max_level_)) || (NULL == m_file_))
    {
        return -1;
    }

    threadc_mutex_guard lock(m_mutex_);
    struct tm tm_now;
    struct timeval tv;
#ifdef WIN32
    time_t now = time(NULL);
    tv.tv_sec = now;
#else
    struct timezone tz;
    gettimeofday(&tv, &tz);
    time_t now = tv.tv_sec;
#endif
    if ((now - m_middle_night_) > 24 * 60 * 60)
    {
        close();
        open();
    }
#ifdef WIN32
    localtime_s(&tm_now, &now);
#else
    localtime_r(&now, &tm_now);
#endif
    int lv = get_level_str_index(level);

    // strftime消耗大，改为直接格式化
    // write level
    if (m_enable_usec_)
    {
        sprintf(((char*)m_level_str_usec_[lv]+TIME_START), "%02d:%02d:%02d.%06ld",
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, tv.tv_usec);
        m_level_str_usec_[lv][strlen(m_level_str_usec_[lv])] = ' ';
        fputs(m_level_str_usec_[lv], m_file_);
    }
    else
    {
        sprintf(((char*)m_level_str_[lv]+TIME_START), "%02d:%02d:%02d",
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
        m_level_str_[lv][strlen(m_level_str_[lv])] = ' ';
        fputs(m_level_str_[lv], m_file_);
    }
    // write msg
    vfprintf(m_file_, fmt, ap);
    // reset color
    if (fmt[strlen(fmt) - 1] != '\n')
    {
        fputc('\n', m_file_);
    }

    // 无缓存，直接写
    fflush(m_file_);

    if ((size_t)ftell(m_file_) > m_max_size_)
    {
        close();
        open();
    }

    return 0;
}

static const char chex[] = "0123456789ABCDEF";

int logc::log_hex_prefix(unsigned char* prefix, unsigned char* data, size_t len, log_level_t level)
{
    log(level, "%s", prefix);
    return log_hex(data, len, level);
}

int logc::log_hex(unsigned char* data, size_t len, log_level_t level)
{
    size_t i, j, k, l;

    if ((0 == (level & m_max_level_)) || NULL == m_file_)
    {
        return -1;
    }

    threadc_mutex_guard lock(m_mutex_);

    char msg_str[128] = {0};

    msg_str[0] = '[';
    msg_str[3] = '0';
    msg_str[4] = ']';
    msg_str[57] = ' ';
    msg_str[58] = '|';
    msg_str[75] = '|';
    msg_str[76] = 0;
    k = 4;
    for (j = 0; j < 16; j++)
    {
        if ((j & 0x03) == 0)
        {
            msg_str[++k] = ' ';
        }
        k += 3;
        msg_str[k] = ' ';
    }

    for (i = 0; i < len / 16; i++)
    {
        msg_str[1] = chex[i >> 4];
        msg_str[2] = chex[i &0xF];
        k = 5;
        l = i * 16;
        memcpy(msg_str + 59, data + l, 16);
        for (j = 0; j < 16; j++)
        {
            if ((j & 0x03) == 0)
            {
                k++;
            }
            msg_str[k++] = chex[data[l] >> 4];
            msg_str[k++] = chex[data[l++] & 0x0F];
            k++;
            if (!isgraph(msg_str[59 + j]))
            {
                msg_str[59 + j]= '.';
            }
        }
        msg_str[127] = 0;
        //threadc_mutex_guard lock(m_mutex_);
        fprintf(m_file_, "# %s\n", msg_str);
    }
    msg_str[1] = chex[i >> 4];
    msg_str[2] = chex[i &0xF];
    k = 5;
    l = i * 16;
    memcpy(msg_str + 59, data + l, len % 16);
    for (j = 0; j < len % 16; j++)
    {
        if ((j & 0x03) == 0)
        {
            k++;
        }
        msg_str[k++] = chex[data[l] >> 4];
        msg_str[k++] = chex[data[l++] & 0x0F];
        k++;
        if (!isgraph(msg_str[59 + j]))
        {
            msg_str[59 + j]= '.';
        }
    }
    for (; j < 16; j++)
    {
        if ((j & 0x03) == 0)
        {
            k++;
        }
        msg_str[k++] = ' ';
        msg_str[k++] = ' ';
        k++;
        msg_str[59 + j]= ' ';
    }
    msg_str[127] = 0;
    //threadc_mutex_guard lock(m_mutex_);
    fprintf(m_file_, "# %s\n", msg_str);
    return 0;
}

logc logc::global_log;

