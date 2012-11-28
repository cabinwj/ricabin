/**
@file   xmlconf.h
@brief  读取xml配置文件类头文件
*/

#ifndef _XML_CONFIG_H_
#define _XML_CONFIG_H_

#include <stdint.h>
#include <string>
#include <iostream>
#include "tinystr.h"
#include "tinyxml.h"


class xml_config
{
public:
    enum
    {
        INDENTS_COUNT_PER_SPACE = 2,
    };

public:
	xml_config();
	explicit xml_config(const char* xml_file_name)
		:m_xml_conf_file_(xml_file_name), m_root_elem_(0)
	{
		//加载配置文件
		m_xml_conf_file_.LoadFile();
		//得到配置文件的根结点
		m_root_elem_ = m_xml_conf_file_.RootElement();
	}

	int init(const char* xml_file_name);

public:
	/***
	**	@brief：得到字符串配置内容
	**  @param[in]  const std::string& block_name 区块名
	**	@param[in]  const std::string& node_name  节点名
	**	@param[in] char* value  返回的结果
	**/
	void get_value(const std::string& block_name, const std::string& node_name, char* value);

	/***
	**	@brief：得到整数参数
	**  @param[in]  const std::string& block_name 区块名
	**	@param[in]  const std::string& node_name 节点名
	**	@param[out] int& value  返回的结果
	**  @param[in]  int  defaut 默认参数
	**/
	void get_value(const std::string& block_name, const std::string& node_name, int32_t& value, int vdefault = -1);

	/***
	**	@brief：得到字符串配置内容
	**	@param[in]  const std::string& node_name 节点名
	**	@param[in]  char* value  返回的结果
	**/
	void get_value(const std::string& node_name, char* value);

	/***
	**	@brief：得到整数参数
	**	@param[in]  const std::string& node_name 节点名
	**	@param[out] int& value  返回的结果
	**  @param[in]  int  defaut 默认参数
	**/
	void get_value(const std::string& node_name, int32_t& value, int defaut = -1);

	//重读配置文件
	void reload_config(const char* xml_file_name);

	//列出所有选项
	void list_all();

private:
	void dump_to_stdout(TiXmlNode* parent, uint32_t indent = 0 );
	int dump_attribs_to_stdout(TiXmlElement* element, unsigned int indent);
	const char* get_indent_alt(unsigned int num_indents );
	const char* get_indent(unsigned int num_indents );

private:
	TiXmlDocument m_xml_conf_file_;
	TiXmlElement* m_root_elem_;
};

#endif

