/**
@file   xmlconf.h
@brief  ��ȡxml�����ļ���ͷ�ļ�
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
		//���������ļ�
		m_xml_conf_file_.LoadFile();
		//�õ������ļ��ĸ����
		m_root_elem_ = m_xml_conf_file_.RootElement();
	}

	int init(const char* xml_file_name);

public:
	/***
	**	@brief���õ��ַ�����������
	**  @param[in]  const std::string& block_name ������
	**	@param[in]  const std::string& node_name  �ڵ���
	**	@param[in] char* value  ���صĽ��
	**/
	void get_value(const std::string& block_name, const std::string& node_name, char* value);

	/***
	**	@brief���õ���������
	**  @param[in]  const std::string& block_name ������
	**	@param[in]  const std::string& node_name �ڵ���
	**	@param[out] int& value  ���صĽ��
	**  @param[in]  int  defaut Ĭ�ϲ���
	**/
	void get_value(const std::string& block_name, const std::string& node_name, int32_t& value, int vdefault = -1);

	/***
	**	@brief���õ��ַ�����������
	**	@param[in]  const std::string& node_name �ڵ���
	**	@param[in]  char* value  ���صĽ��
	**/
	void get_value(const std::string& node_name, char* value);

	/***
	**	@brief���õ���������
	**	@param[in]  const std::string& node_name �ڵ���
	**	@param[out] int& value  ���صĽ��
	**  @param[in]  int  defaut Ĭ�ϲ���
	**/
	void get_value(const std::string& node_name, int32_t& value, int defaut = -1);

	//�ض������ļ�
	void reload_config(const char* xml_file_name);

	//�г�����ѡ��
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

