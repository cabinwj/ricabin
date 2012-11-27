#include "common_mysql_pool.h"
#include "common_log.h"

#include <string.h>

mysql_pool::mysql_pool()
{
    memset(m_host_, 0, sizeof(m_host_));
    memset(m_user_, 0, sizeof(m_user_));
    memset(m_password_, 0, sizeof(m_password_));
    memset(m_dbname_, 0, sizeof(m_dbname_));
    m_max_size_ = 0;
    m_real_size_ = 0;
}

mysql_pool::~mysql_pool()
{
}

int mysql_pool::init(int size, string host, int port, string user, string pass, string dbname)
{
    strncpy(m_host_, host.c_str(), MAX_MYSQL_INIT_STRING_LEN);
    m_port_ = port;
    strncpy(m_user_, user.c_str(), MAX_MYSQL_INIT_STRING_LEN);
    strncpy(m_password_, pass.c_str(), MAX_MYSQL_INIT_STRING_LEN);
    strncpy(m_dbname_, dbname.c_str(), MAX_MYSQL_INIT_STRING_LEN);
    m_max_size_ = size;

    m_mysql_deque_.clear();
    m_real_size_ = 0;
    for (int i = 0; i < size; i++)
    {
        m_real_size_++;
        MYSQL* conn = create_connection();
        if (!conn)
        {
            return -1;
        }
        m_mysql_deque_.push_back(conn);
    }
    return 0;
}

void mysql_pool::uninit()
{
    threadc_mutex_guard lock(m_mutex_);
    deque<MYSQL*>::const_iterator it;
    for (it = m_mysql_deque_.begin(); it != m_mysql_deque_.end(); it++)
    {
        mysql_close(*it);
    }
    m_mysql_deque_.clear();
}

MYSQL* mysql_pool::acquire_connection(int& isfull)
{
    //threadc_mutex_guard lock(m_mutex_);
    m_mutex_.acquire();
    isfull = 0;
    if (m_mysql_deque_.size() > 0)
    {
        MYSQL* cnn = m_mysql_deque_.front();
        m_mysql_deque_.pop_front();
        m_mutex_.release();
        return cnn;
    }
    else if (m_real_size_ < m_max_size_)
    {
        m_real_size_++;
        m_mutex_.release();
        return create_connection();
    }
    isfull = 1;
    m_mutex_.release();
    return NULL;
}

MYSQL* mysql_pool::acquire_connection_with_retry(int max_count)
{
    int retry_count = 0;
    int isfull = 0;
    MYSQL* conn = NULL;
    while (NULL == (conn = acquire_connection(isfull)))
    {
        sleep_ms(1);
        if (retry_count++ > max_count)
        {
            return NULL;
        }
    }
    return conn;
}

void mysql_pool::release_connection(MYSQL* conn)
{
    threadc_mutex_guard lock(m_mutex_);
    m_mysql_deque_.push_back(conn);
}

void mysql_pool::destroy_connection(MYSQL* conn)
{
    mysql_close(conn);
    threadc_mutex_guard lock(m_mutex_);
    m_real_size_--;
}

MYSQL* mysql_pool::create_connection()
{
    MYSQL* cnn = mysql_init(NULL);
    if (!cnn)
    {
        threadc_mutex_guard lock(m_mutex_);
        m_real_size_--;
        return NULL;
    }

    //unsigned int timeout = 5;
    unsigned int timeout = 0;
    my_bool reconnect = true;
    mysql_options(cnn, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&timeout);
    mysql_options(cnn, MYSQL_OPT_RECONNECT, &reconnect);
    mysql_options(cnn, MYSQL_SET_CHARSET_NAME, "utf8");

    if (!mysql_real_connect(cnn, m_host_, m_user_, m_password_, m_dbname_, m_port_, NULL, 0))
    {
        LOG(DEBUG)("mysql_pool::create_connection() ERROR DESCRIPTION:%s, ERRORNO:%d", mysql_error(cnn), mysql_errno(cnn));
        mysql_close(cnn);
        threadc_mutex_guard lock(m_mutex_);
        m_real_size_--;
        return NULL;
    }
    return cnn;
}

int mysql_pool::reconnect(MYSQL* conn)
{
    if (!conn)
    {
        return -1;
    }
    if (!mysql_real_connect(conn, m_host_, m_user_, m_password_, m_dbname_, m_port_, NULL, 0))
    {
        LOG(DEBUG)("mysql_pool::reconnect() ERROR DESCRIPTION:%s, ERRORNO:%d", mysql_error(conn), mysql_errno(conn));
        mysql_close(conn);
        threadc_mutex_guard lock(m_mutex_);
        m_real_size_--;
        return -1;
    }
    return 0;
}

string mysql_read_res_guard::field_string(const string& field_name)
{
    if (m_mysql_field_index_map_.find(field_name) == m_mysql_field_index_map_.end())
    {
        return "";
    }

    char* p = m_mysql_row_[m_mysql_field_index_map_[field_name]];
    if (NULL == p)
    {
        return "";
    }

    return string(p, field_length(field_name));
}

string mysql_read_res_guard::field_string(unsigned long field_index)
{
    if (field_index > m_field_count_)
    {
        return "";
    }

    char* p = m_mysql_row_[field_index];
    if (NULL == p)
    {
        return "";
    }

    return string(p, field_length(field_index));
}

char* mysql_read_res_guard::field_buffer(const string& field_name)
{
    if (m_mysql_field_index_map_.find(field_name) == m_mysql_field_index_map_.end())
    {
        return NULL;
    }

    return m_mysql_row_[m_mysql_field_index_map_[field_name]];
}

char* mysql_read_res_guard::field_buffer(unsigned long field_index)
{
    if (field_index > m_field_count_)
    {
        return NULL;
    }

    return m_mysql_row_[field_index];
}

unsigned long mysql_read_res_guard::field_length(const string& field_name)
{
    if (m_mysql_field_index_map_.find(field_name) == m_mysql_field_index_map_.end())
    {
        return 0;
    }

    unsigned long field_index = m_mysql_field_index_map_[field_name];
    if (field_index > m_field_count_)
    {
        return 0;
    }

    return mysql_fetch_lengths(m_mysql_res_)[field_index];
}

unsigned long mysql_read_res_guard::field_length(unsigned long field_index)
{
    if (field_index > m_field_count_)
    {
        return 0;
    }

    return mysql_fetch_lengths(m_mysql_res_)[field_index];
}

bool mysql_read_res_guard::row_next()
{
    if ((NULL == m_mysql_res_) ||
        (NULL == (m_mysql_row_ = mysql_fetch_row(m_mysql_res_))))
    {
        return false;
    }

    return true;
}
