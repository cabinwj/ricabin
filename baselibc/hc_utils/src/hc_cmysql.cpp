/*************************************************************************
 Author: Chris Wonder
 Created Time: 2009年11月11日 星期二 12时02分48秒
 File Name: cmysql.cpp
 Description: 
 ************************************************************************/

#include "hc_cmysql.h"


CMysql::CMysql():m_nPort(3306), m_bAutoQuery(true), m_bFieldIndexInitialized(false),
        m_bConnected(false), m_unResultFields(0), m_unResultRows(0), m_unAffectedRows(0),
        m_mysqlResult(NULL)
{
}

CMysql::~CMysql()
{
	close();
}

void CMysql::init(std::string sHost, std::string sUser, std::string sPass, int nPort, std::string sQueryCharset)
{
	m_sHost = sHost;
	m_sUser = sUser;
	m_sPass = sPass;
	m_nPort = nPort;
    queryCharset(sQueryCharset);
	connect();
}

void CMysql::reInit()
{
	m_bConnected = false;
    freeResult();
	connect();
}

void CMysql::close()
{
	if (m_bConnected)
    {
		freeResult();
		mysql_close(&m_mysqlConnection);
		m_bConnected = false;
	}
}

void CMysql::connect()
{
	if (!m_bConnected)
    {
		mysql_init(&m_mysqlConnection);
		//unsigned int timeout = 5;
        unsigned int timeOut = 0;
        my_bool reConnect = true;
		mysql_options(&m_mysqlConnection, MYSQL_OPT_CONNECT_TIMEOUT, (char*)(&timeOut));
		mysql_options(&m_mysqlConnection, MYSQL_OPT_READ_TIMEOUT, (char*)(&timeOut));
		mysql_options(&m_mysqlConnection, MYSQL_OPT_WRITE_TIMEOUT, (char*)(&timeOut));
        mysql_options(&m_mysqlConnection, MYSQL_OPT_RECONNECT, &reConnect);
        mysql_options(&m_mysqlConnection, MYSQL_SET_CHARSET_NAME, m_sQueryCharset.c_str());
		if (NULL == mysql_real_connect(&m_mysqlConnection, m_sHost.c_str(),
                                       m_sUser.c_str(), m_sPass.c_str(), NULL, m_nPort, NULL, 0))
        {
            std::ostringstream ossErrMsg;
            ossErrMsg << "CMysql::Connect() connect[-h" << m_sHost 
					  << " -u" << m_sUser << " -p" << m_sPass << "] fail.\nError "
					  << mysql_error(&m_mysqlConnection) << "\n";
            throw CMysqlException(ossErrMsg.str().c_str());
		}
        /**
        // 这里为了设置字符集，多了一次查询。
		std::string sql = "SET NAMES 'gbk'";
        if( 0 != mysql_real_query(&m_connection, sql.c_str(), sql.length()))
        {
			throw CMysqlException("SET NAMES 'gbk' error.");
		}*/
		m_bConnected = true;
	}
}

std::string CMysql::escapeString(const std::string& in)
{
	if (0 == in.size())
    {
        return in;
    }

	char* szBuff = new char[in.size() * 2 + 1];
	mysql_escape_string(szBuff, in.c_str(), in.size());
	std::string sEscapeString(szBuff);
	delete[] szBuff;
	return sEscapeString;
}

std::string CMysql::escapeString(const char* in, unsigned int inlen)
{
	if (0 == inlen)
    {
        return std::string(in, inlen);
    }

	char* szBuff = new char[inlen * 2 + 1];
	mysql_escape_string(szBuff, in, inlen);
	std::string sEscapeString(szBuff);
	delete[] szBuff;
	return sEscapeString;
}

void CMysql::query()
{
	( 0 != mysql_ping(&m_mysqlConnection) ) ? reInit() : freeResult();

	if (0 != mysql_real_query(&m_mysqlConnection, m_ssSQL.str().c_str(), m_ssSQL.str().length()))
    {
		std::ostringstream ossErrMsg;
        ossErrMsg << "CMysql::Query(const std::string& szSql) mysql_real_query ["
                        << m_sHost << "].\nError=[" << mysql_error(&m_mysqlConnection)
                        << "]\nSQL=" << m_ssSQL.str();
		m_ssSQL.str("");
		throw CMysqlException(ossErrMsg.str().c_str());
	}

	m_mysqlResult = mysql_store_result(&m_mysqlConnection);
	if (NULL == m_mysqlResult)
    {
        if(0 != mysql_field_count(&m_mysqlConnection))
        {
            // query read error.
            std::ostringstream ossErrMsg;
            ossErrMsg << "CMysql::Query(const std::string& szSql) mysql_real_query ["
                            << m_sHost << "].\nError=[" << mysql_error(&m_mysqlConnection)
                            << "]\nSQL=" << m_ssSQL.str();
            m_ssSQL.str("");
            throw CMysqlException(ossErrMsg.str().c_str());
        }

        // query write success.
        m_unAffectedRows = static_cast<unsigned int>( mysql_affected_rows(&m_mysqlConnection) );
        m_ssSQL.str("");
        return;
    }

    // query read success.
	m_unResultFields = mysql_num_fields(m_mysqlResult);
	m_unResultRows = static_cast<unsigned int>( mysql_num_rows(m_mysqlResult) );
    m_ssSQL.str("");

	if ( !m_bFieldIndexInitialized )
    {
		MYSQL_FIELD* mysqlFields = mysql_fetch_fields(m_mysqlResult);
		for (unsigned int i = 0; i < m_unResultFields; i++)
        {
            m_mapFieldIndex[mysqlFields[i].name] = i;
		}
        m_bFieldIndexInitialized = true;
	}
    return;
}

void CMysql::freeResult()
{
	if (NULL != m_mysqlResult)
	{
        mysql_free_result(m_mysqlResult);
        m_mysqlResult = NULL;
	}

    m_unResultFields = 0;
    m_unResultRows = 0;
    m_unAffectedRows = 0;

	m_mapFieldIndex.clear();
	m_bFieldIndexInitialized = false;
}

bool CMysql::nextRow()
{
	if (NULL == m_mysqlResult || NULL == (m_mysqlRow = mysql_fetch_row(m_mysqlResult)))
	{
        return false;
	}
    return true;
}

std::string CMysql::fieldString(const std::string& sColName)
{
	if (m_mapFieldIndex.find(sColName) == m_mapFieldIndex.end())
	{
		return "";
	}
	char* szValue = m_mysqlRow[m_mapFieldIndex[sColName]];
	if (NULL == szValue)
    {
        return "";
	}
    return std::string(szValue, fieldLength(sColName));
}

std::string CMysql::fieldString(unsigned int unColIndex)
{
	if (unColIndex > m_unResultFields)
    {
        return "";
	}
	char* szValue = m_mysqlRow[unColIndex];
	if (NULL == szValue)
	{
		return "";
	}
    return std::string(szValue, fieldLength(unColIndex));
}

char* CMysql::fieldValue(const std::string& sColName)
{
    if (m_mapFieldIndex.find(sColName) == m_mapFieldIndex.end())
    {
        return NULL;
    }
    return m_mysqlRow[m_mapFieldIndex[sColName]];
}

char* CMysql::fieldValue(unsigned int unColIndex)
{
    if (unColIndex > m_unResultFields)
    {
        return NULL;
    }
	return m_mysqlRow[unColIndex];
}

unsigned long CMysql::fieldLength(const std::string& sColName)
{
    if (m_mapFieldIndex.find(sColName) == m_mapFieldIndex.end())
    {
        return 0;
    }
	unsigned int unColIndex = m_mapFieldIndex[sColName];
    if (unColIndex > m_unResultFields)
    {
        return 0;
    }
    return mysql_fetch_lengths(m_mysqlResult)[unColIndex];
}

unsigned long CMysql::fieldLength(unsigned int unColIndex)
{
    if (unColIndex > m_unResultFields)
    {
        return 0;
    }
    return mysql_fetch_lengths(m_mysqlResult)[unColIndex];
}

const char* CMysql::fieldName(int nField)
{
	if (NULL == m_mysqlResult)
    {
		return NULL;
	}

	MYSQL_FIELD* mysqlFields = mysql_fetch_fields(m_mysqlResult);
	if ((unsigned int)nField > m_unResultFields)
    {
		return NULL;
	}
	return mysqlFields[nField].name;
}
