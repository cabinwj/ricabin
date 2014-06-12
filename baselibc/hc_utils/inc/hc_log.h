/// @file common_log.h
/// @brief 日志模块
#ifndef _HC_LOG_H_
#define _HC_LOG_H_

#include <stdio.h>
#include <time.h>

#include "hc_thread_mutex.h"

#ifdef WIN32
#define MAX_PATH_LEN    260
#else
#define MAX_PATH_LEN    1024
#endif

/// 日志级别
typedef enum log_level_t
{
    L_NONE        = 0,
    L_FATAL       = 1 << 0,        ///< 致命错误，程序不得不停止
    L_ERROR       = 1 << 1,        ///< 故障，如网络连接错误
    L_WARN        = 1 << 2,        ///< 警告，不影响业务的故障
    L_INFO        = 1 << 3,        ///< 业务信息记录
    L_TRACE       = 1 << 4,        ///< 程序流程跟踪
    L_DEBUG       = 1 << 5,        ///< 调试信息
    L_LEVEL_MAX   = (L_FATAL | L_FATAL | L_ERROR | L_WARN | L_INFO | L_TRACE | L_DEBUG),
}log_level_t;

/**
    日志类

    日志文件名格式：
        - xxx-20081105-090000.log

    日志文件分割：
        - 新的一天开始，或者文件大小超过设定值，打开新文件写入

    用法：
        <CODE>
        #include <log.h>

        LOG_INIT("xxx", 400000, L_TRACE);

        LOG_OPEN();

        LOG(3)("warn info %d", 3);

        LOG_CLOSE();

        </CODE>
    */
class logc
{
public:
    /**
        构造Log对象
    */
    logc();

    /**
        析构Log对象

        析构前将关闭日志文件
        @see close()
    */
    ~logc();

public:
    /**
        设置日志文件名

        如果已经打开了就不能再设置了
        @param filename 新的日志文件名
        @return 0 成功 -1 出现错误
    */
    int set_file_name(const char* filename);

    /**
        设置日志文件规模上限

        不立即生效，在下次输出日志时才检查
        @param maxsize 新的规模上限
    */
    void set_max_size(size_t maxsize);

    /**
        设置最大日志等级

        只对后续输出的日志有效
        @param level 新的级别上限
    */
    void set_max_level(log_level_t level);

    /**
        是否输出到 usec级别
    **/
    void set_usec(bool in_enable_usec);

    /**
        获取日志文件规模上限

        @return 日志文件规模上限
    */
    size_t get_max_size() {return m_max_size_;}

    /**
        获取最大日志等级

        @return 最大日志等级
    */
    log_level_t get_max_level() {return m_max_level_;}

    /**
        打开日志文件

        打开前需先设置文件名
        @return 0 成功 -1 出现错误
        @see set_file_name()
    */
    int open();

    /**
        关闭日志文件

        关闭后将重命名日志文件
        @return 0 成功 -1 出现错误
        @see set_file_name()
    */
    int close();

#ifdef WIN32    // for windows
#define CHECK_FORMAT(i, j)
#else            // for linux(gcc)
#define CHECK_FORMAT(i, j) __attribute__((format(printf, i, j)))
#endif

    /**
        输出一条日志记录

        @param level 日志等级
        @param fmt 格式化字符串
        @return 0 成功 -1 出现错误
    */
    int log(log_level_t level, const char* fmt, ...) CHECK_FORMAT(3, 4);

    /// 输出一条FATAL日志记录
    int log_fatal(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// 输出一条ERROR日志记录
    int log_error(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// 输出一条WARN日志记录
    int log_warn(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// 输出一条INFO日志记录
    int log_info(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// 输出一条TRACE日志记录
    int log_trace(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// 输出一条DEBUG日志记录
    int log_debug(const char* fmt, ...) CHECK_FORMAT(2, 3);

#undef CHECK_FORMAT

    /**
        用十六进制dump一段数据

        @param data 数据首地址
        @param len 数据长度
        @param level 日志等级
        @return 0 成功 -1 出现错误
    */
    int log_hex(unsigned char* data, size_t len, log_level_t level);
    int log_hex_prefix(unsigned char* prefix, unsigned char* data, size_t len, log_level_t level);

public:
    /// 全局日志对象
    static logc global_log;

private:
    /**
        输出一条日志记录

        @param level 日志等级
        @param fmt 格式化字符串
        @param ap 格式化参数
        @return 0 成功 -1 出现错误
        @see log()
    */
    int vlog(log_level_t level, const char* fmt, va_list ap);

    /**
        得到日志级别在关键字描述中的索引

        @param level 日志等级
        @return 日志级别在关键字描述中的索引
    */
    int level_str_index_o(log_level_t level);

private:
    /// 不同日志级别的颜色以及关键字描述
    static char m_level_str_[L_LEVEL_MAX][64];
    static char m_level_str_usec_[L_LEVEL_MAX][64];

private:
    /// 日志文件名
    char m_file_name_[MAX_PATH_LEN];

    /// 单个日志文件最大文件大小
    size_t m_max_size_;

    /// 日志级别
    log_level_t m_max_level_;

    /// 日志文件文件描述符
    FILE* m_file_;

    /// 线程锁
    threadc_mutex m_mutex_;

    /// 今天开始时刻
    time_t m_middle_night_;

    bool m_enable_usec_;
};

#define LOG(level) LOG_##level

#define LOG_FATAL logc::global_log.log_fatal
#define LOG_ERROR logc::global_log.log_error
#define LOG_WARN logc::global_log.log_warn
#define LOG_INFO logc::global_log.log_info
#define LOG_TRACE logc::global_log.log_trace
#define LOG_DEBUG logc::global_log.log_debug

#define LOG_HEX(data, len, level) logc::global_log.log_hex((unsigned char*)(data), (len), (level))
#define LOG_HEX_PREFIX(prefix, data, len, level) logc::global_log.log_hex_prefix((unsigned char*)(prefix), (unsigned char*)(data), (len), (level))


#define LOG_INIT(name, size, level) \
    logc::global_log.set_file_name(name); \
    logc::global_log.set_max_size(size); \
    logc::global_log.set_max_level((log_level_t)(level))

#define LOG_OPEN() logc::global_log.open()

#define LOG_CLOSE() logc::global_log.close()

#define LOG_SET_LEVEL(level) logc::global_log.set_max_level((log_level_t)(level))

#define LOG_GET_LEVEL() logc::global_log.get_max_level()

#define LOG_SET_USEC(enable_usec) logc::global_log.set_usec(enable_usec)


#endif // _HC_LOG_H_
