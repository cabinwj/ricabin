/// @file common_log.h
/// @brief ��־ģ��
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

/// ��־����
typedef enum log_level_t
{
    L_NONE        = 0,
    L_FATAL       = 1 << 0,        ///< �������󣬳��򲻵ò�ֹͣ
    L_ERROR       = 1 << 1,        ///< ���ϣ����������Ӵ���
    L_WARN        = 1 << 2,        ///< ���棬��Ӱ��ҵ��Ĺ���
    L_INFO        = 1 << 3,        ///< ҵ����Ϣ��¼
    L_TRACE       = 1 << 4,        ///< �������̸���
    L_DEBUG       = 1 << 5,        ///< ������Ϣ
    L_LEVEL_MAX   = (L_FATAL | L_FATAL | L_ERROR | L_WARN | L_INFO | L_TRACE | L_DEBUG),
}log_level_t;

/**
    ��־��

    ��־�ļ�����ʽ��
        - xxx-20081105-090000.log

    ��־�ļ��ָ
        - �µ�һ�쿪ʼ�������ļ���С�����趨ֵ�������ļ�д��

    �÷���
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
        ����Log����
    */
    logc();

    /**
        ����Log����

        ����ǰ���ر���־�ļ�
        @see close()
    */
    ~logc();

public:
    /**
        ������־�ļ���

        ����Ѿ����˾Ͳ�����������
        @param filename �µ���־�ļ���
        @return 0 �ɹ� -1 ���ִ���
    */
    int set_file_name(const char* filename);

    /**
        ������־�ļ���ģ����

        ��������Ч�����´������־ʱ�ż��
        @param maxsize �µĹ�ģ����
    */
    void set_max_size(size_t maxsize);

    /**
        ���������־�ȼ�

        ֻ�Ժ����������־��Ч
        @param level �µļ�������
    */
    void set_max_level(log_level_t level);

    /**
        �Ƿ������ usec����
    **/
    void set_usec(bool in_enable_usec);

    /**
        ��ȡ��־�ļ���ģ����

        @return ��־�ļ���ģ����
    */
    size_t get_max_size() {return m_max_size_;}

    /**
        ��ȡ�����־�ȼ�

        @return �����־�ȼ�
    */
    log_level_t get_max_level() {return m_max_level_;}

    /**
        ����־�ļ�

        ��ǰ���������ļ���
        @return 0 �ɹ� -1 ���ִ���
        @see set_file_name()
    */
    int open();

    /**
        �ر���־�ļ�

        �رպ���������־�ļ�
        @return 0 �ɹ� -1 ���ִ���
        @see set_file_name()
    */
    int close();

#ifdef WIN32    // for windows
#define CHECK_FORMAT(i, j)
#else            // for linux(gcc)
#define CHECK_FORMAT(i, j) __attribute__((format(printf, i, j)))
#endif

    /**
        ���һ����־��¼

        @param level ��־�ȼ�
        @param fmt ��ʽ���ַ���
        @return 0 �ɹ� -1 ���ִ���
    */
    int log(log_level_t level, const char* fmt, ...) CHECK_FORMAT(3, 4);

    /// ���һ��FATAL��־��¼
    int log_fatal(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// ���һ��ERROR��־��¼
    int log_error(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// ���һ��WARN��־��¼
    int log_warn(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// ���һ��INFO��־��¼
    int log_info(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// ���һ��TRACE��־��¼
    int log_trace(const char* fmt, ...) CHECK_FORMAT(2, 3);

    /// ���һ��DEBUG��־��¼
    int log_debug(const char* fmt, ...) CHECK_FORMAT(2, 3);

#undef CHECK_FORMAT

    /**
        ��ʮ������dumpһ������

        @param data �����׵�ַ
        @param len ���ݳ���
        @param level ��־�ȼ�
        @return 0 �ɹ� -1 ���ִ���
    */
    int log_hex(unsigned char* data, size_t len, log_level_t level);
    int log_hex_prefix(unsigned char* prefix, unsigned char* data, size_t len, log_level_t level);

public:
    /// ȫ����־����
    static logc global_log;

private:
    /**
        ���һ����־��¼

        @param level ��־�ȼ�
        @param fmt ��ʽ���ַ���
        @param ap ��ʽ������
        @return 0 �ɹ� -1 ���ִ���
        @see log()
    */
    int vlog(log_level_t level, const char* fmt, va_list ap);

    /**
        �õ���־�����ڹؼ��������е�����

        @param level ��־�ȼ�
        @return ��־�����ڹؼ��������е�����
    */
    int level_str_index_o(log_level_t level);

private:
    /// ��ͬ��־�������ɫ�Լ��ؼ�������
    static char m_level_str_[L_LEVEL_MAX][64];
    static char m_level_str_usec_[L_LEVEL_MAX][64];

private:
    /// ��־�ļ���
    char m_file_name_[MAX_PATH_LEN];

    /// ������־�ļ�����ļ���С
    size_t m_max_size_;

    /// ��־����
    log_level_t m_max_level_;

    /// ��־�ļ��ļ�������
    FILE* m_file_;

    /// �߳���
    threadc_mutex m_mutex_;

    /// ���쿪ʼʱ��
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
