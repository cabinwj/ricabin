/*************************************************************************
 Author: Chris Wonder
 Created Time: 2009��11��11�� ���ڶ� 12ʱ02��48��
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
	/* ��������Լ�ʵ��һ����SQL������ execute / query  ���� */
    void query();

	/* ȡ��һ�н���� */
    bool nextRow();

	/* �����ֶΣ���ȡ�ֶ�ֵ��ע����Ҫint�͵ģ�������Ӧ�ò�����תΪint */
	std::string fieldString(const std::string& sColName);
	std::string fieldString(unsigned int unColIndex);

	char* fieldValue(const std::string& sColName);
	char* fieldValue(unsigned int unColIndex);

	/* �����ֶΣ���ȡ�ֶ�ֵ���� */
	unsigned long fieldLength(const std::string& sColName);
	unsigned long fieldLength(unsigned int unColIndex);

public:
	/* �Զ��Իس���ִ�� */
    inline void autoQuery(bool isAutoQuery) { m_bAutoQuery = isAutoQuery; }
	/* �ַ��� */
    inline void queryCharset(std::string& sQueryCharset) { m_sQueryCharset = sQueryCharset; }

    /* �������һ��insert��ID*/
    inline unsigned int lastInsertId() { return (unsigned int)mysql_insert_id(&m_mysqlConnection); }
    /* ���ؽ��������*/
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

	/* ������֮ǰģ��س�ʵ��ִ��shell��SQL */
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
    // ���ݿ�����.�û�.����
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
APIʹ�ÿͻ���ȷӦ���ѯ��Ϊ����(��������Ҫ����)������֪����ѯ�Ƿ�Ϊһ��SELECT��
����ͨ����mysql_query()(��mysql_real_query())֮�����mysql_store_result()������
���������ϵ��óɹ����Ҳ�ѯ��һ��SELECT���ܶ�ȡ����С����򣬵���mysql_field_count()ȷ������Ƿ���ʵ�������ġ�
���mysql_field_count()����0����ѯû�з�������(��������һ��INSERT��UPDATE��DELETE��)�����Բ����������С�
���mysql_field_count()�Ƿ��㣬��ѯӦ���з����У�����û�С��������ѯ��һ��ʧ�ܵ�SELECT��
��mysql_field_count()��������������ӵ�����
****/

#endif //_CMYSQL_H_
