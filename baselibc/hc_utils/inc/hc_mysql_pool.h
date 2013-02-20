#ifndef _HC_TMYSQL_POOL_H_
#define _HC_TMYSQL_POOL_H_

#include "hc_os.h"
#include "hc_thread_mutex.h"

#include <mysql.h>
#include <mysqld_error.h>
#include <errmsg.h>

#include <string>
#include <deque>
#include <memory>
#include <map>

using namespace std;


class mysql_pool
{
public:
    enum {
        MAX_MYSQL_INIT_STRING_LEN = 63,
    };

public:
    mysql_pool();
    ~mysql_pool();

public:
    int init(int size, string host, int port, string user, string pass, string dbname);
    void uninit();

    MYSQL* acquire_connection(int& isfull);
    MYSQL* acquire_connection_with_retry(int maxcount);
    void release_connection(MYSQL* conn);

    MYSQL* create_connection();
    void destroy_connection(MYSQL* conn);

    int reconnect(MYSQL* conn);

public:
    deque<MYSQL*> m_mysql_deque_;
    threadc_mutex m_mutex_;

    char m_host_[MAX_MYSQL_INIT_STRING_LEN + 1];
    int m_port_;
    char m_user_[MAX_MYSQL_INIT_STRING_LEN + 1];
    char m_password_[MAX_MYSQL_INIT_STRING_LEN + 1];
    char m_dbname_[MAX_MYSQL_INIT_STRING_LEN + 1];
    int m_max_size_;
    int m_real_size_;
};

class mysql_connect_guard
{
public:
    mysql_connect_guard(mysql_pool* pool, MYSQL* cnn)
    {
        m_mysql_pool_ = pool;
        m_mysql_connect_ = cnn;
    }

    ~mysql_connect_guard()
    {
        m_mysql_pool_->release_connection(m_mysql_connect_);
    }

private:
    mysql_pool* m_mysql_pool_;
    MYSQL* m_mysql_connect_;
};

class mysql_read_res_guard
{
public:
    mysql_read_res_guard(): m_mysql_res_(NULL)
                        , m_row_count_(0)
                        , m_field_count_(0)
                        , m_is_field_index_(false)
    {
        m_mysql_field_index_map_.clear();
    }

    mysql_read_res_guard(MYSQL_RES* res): m_mysql_res_(res)
                                        , m_row_count_(0)
                                        , m_field_count_(0)
                                        , m_is_field_index_(false)
    {
        m_mysql_field_index_map_.clear();
    }

    ~mysql_read_res_guard() { mysql_free_result(m_mysql_res_); }

    string field_string(const string& sFieldName);
    string field_string(unsigned long ulFieldIndex);

    char* field_buffer(const string& sFieldName);
    char* field_buffer(unsigned long ulFieldIndex);

    unsigned long field_length(const string& sFieldName);
    unsigned long field_length(unsigned long ulFieldIndex);

    unsigned long row_count() { return m_row_count_;}
    unsigned long field_count() { return m_field_count_; }

    bool row_next();

public:
    MYSQL_RES* m_mysql_res_;
    MYSQL_ROW m_mysql_row_;

    unsigned long m_row_count_;
    unsigned long m_field_count_;

    bool m_is_field_index_;
    map<string, unsigned long> m_mysql_field_index_map_;
};

class mysql_write_res_guard
{
public:
    mysql_write_res_guard(): m_affected_rows_(0), m_last_insert_id_(0) {}
    ~mysql_write_res_guard() {}

    unsigned long affected_rows() { return m_affected_rows_; }
    unsigned long last_insert_id() { return m_last_insert_id_; }

public:
    unsigned long m_affected_rows_;
    unsigned long m_last_insert_id_;
};

#endif // _HC_TMYSQL_POOL_H_
