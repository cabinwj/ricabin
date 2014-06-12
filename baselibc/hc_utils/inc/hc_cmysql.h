/*************************************************************************
 Author: Chris Wonder
 Created Time: 2009年11月11日 星期二 12时02分48秒
 File Name: cmysql.h
 Description: 
 ************************************************************************/

#ifndef _HC_CMYSQL_H_
#define _HC_CMYSQL_H_

#include "hc_types.h"

#include <sstream>
#include <string>
#include <map>

#include <mysql.h>
#include <mysqld_error.h>
#include <errmsg.h>


class CException
{
public:
    CException(std::string sMsg) : m_sShowMessage(sMsg) { }
    CException(std::string sMsg, std::string sUrl) : m_sShowMessage(sMsg), m_sShowURL(sUrl) { }

    const char* showMessage() { return m_sShowMessage.c_str(); }
    const char* showURL() { return m_sShowURL.c_str(); }

public:
    std::string m_sShowMessage;
    std::string m_sShowURL;
};

class CMysqlException: public CException
{
public:
    CMysqlException(const char* sMsg) : CException(sMsg) {}
    CMysqlException(const char* sMsg, const char* sUrl) : CException(sMsg, sUrl) {}
};


class CMysql
{
public:
    CMysql();
    ~CMysql();

public:
    void init(std::string sHost, std::string sUser, std::string sPass, int nPort, std::string sQueryCharset = "utf8");
    void reInit();

public:
    static std::string escapeString(const std::string& in);
    static std::string escapeString(const char* in, unsigned int inlen);

public:
    const char* fieldName(int nField);

public:
	/* 这里可以自己实现一个加SQL参数的 execute / query  函数 */
    void query();

	/* 取下一行结果集 */
    bool nextRow();

	/* 根据字段，获取字段值。注：需要int型的，可以在应用层自行转为int */
	std::string fieldString(const std::string& sColName);
	std::string fieldString(unsigned int unColIndex);

	char* fieldValue(const std::string& sColName);
	char* fieldValue(unsigned int unColIndex);

	/* 根据字段，获取字段值长度 */
	unsigned long fieldLength(const std::string& sColName);
	unsigned long fieldLength(unsigned int unColIndex);

public:
	/* 自动以回车键执行 */
    inline void autoQuery(bool isAutoQuery) { m_bAutoQuery = isAutoQuery; }
	/* 字符集 */
    inline void queryCharset(std::string& sQueryCharset) { m_sQueryCharset = sQueryCharset; }

    /* 返回最近一次insert的ID*/
    inline unsigned int lastInsertId() { return (unsigned int)mysql_insert_id(&m_mysqlConnection); }
    /* 返回结果的行数*/
    inline unsigned int affectedRows() { return m_unAffectedRows; }
    inline unsigned int resultFields() { return m_unResultFields; }
    inline unsigned int resultRows() { return m_unResultRows; }

public:
    template<class T>
    friend CMysql& operator << (CMysql& out, T i)
    {
        out.m_ssSQL << i;
        return out;
    }

	/* 这里是之前模拟回车实现执行shell的SQL */
    friend CMysql& operator << (CMysql& out, std::ostream& (*fendl)(std::ostream&))
    {
        if ( out.m_bAutoQuery )
        {
            out.query();
        }
        return out;
    }

private:
    void close();
    void connect();
    void freeResult();

private:
    // 数据库主机.用户.密码
    std::string m_sHost;
    std::string m_sUser;
    std::string m_sPass;

    int m_nPort;

    std::string m_sQueryCharset;

    bool m_bAutoQuery;
    bool m_bFieldIndexInitialized;
    bool m_bConnected;

    unsigned int m_unResultFields;
    unsigned int m_unResultRows;
    unsigned int m_unAffectedRows;

    std::map<std::string, int> m_mapFieldIndex;
    std::stringstream m_ssSQL;

    MYSQL m_mysqlConnection;
    MYSQL_RES* m_mysqlResult;
    MYSQL_ROW m_mysqlRow;
};

/****
API使得客户正确应答查询成为可能(仅检索必要的行)，不用知道查询是否为一个SELECT。
可以通过在mysql_query()(或mysql_real_query())之后调用mysql_store_result()做到。
如果结果集合调用成功并且查询是一个SELECT，能读取结果行。否则，调用mysql_field_count()确定结果是否是实际期望的。
如果mysql_field_count()返回0，查询没有返回数据(表明它是一个INSERT、UPDATE、DELETE等)，所以不期望返回行。
如果mysql_field_count()是非零，查询应该有返回行，但是没有。这表明查询是一个失败的SELECT。
见mysql_field_count()如何能做到的例子的描述
****/

#endif //_CMYSQL_H_
