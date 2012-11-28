/**
@file   xmlconf.cpp
@brief  ��ȡxml�����ļ����ʵ��
*/

#include "xmlconf.h"
#include <iostream>

xml_config::xml_config()
{
	m_root_elem_ = 0;
}

int xml_config::init(const char* xml_file_name)
{
	m_xml_conf_file_ = xml_file_name;
	//���������ļ�
	m_xml_conf_file_.LoadFile();
	//�õ������ļ��ĸ����
	m_root_elem_ = m_xml_conf_file_.RootElement();
	if (m_root_elem_)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/***
**	@brief���õ��ַ�����������
**	@param[in]  const std::string& block_name
**	@param[in]  const std::string& node_name �ڵ���
**	@param[in] char* value  ���صĽ��
**/
void xml_config::get_value(const std::string& block_name, const std::string& node_name, char* value)
{
	if (!m_root_elem_)
	{
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	TiXmlElement* node_elem = m_root_elem_->FirstChildElement(block_name.c_str());
	if (node_elem->FirstChildElement(node_name.c_str()))                          //����ǽڵ�
	{
		strcpy(value, node_elem->FirstChildElement(node_name.c_str())->GetText());
	}
    else if (node_elem->Attribute(node_name.c_str()))                            //���������
	{
		strcpy(value, node_elem->Attribute(node_name.c_str()));
	}
    else
	{
		std::cerr << "node:" << node_name << " is null!" << std::endl;
		exit(-1);
	}
}


/***
**	@brief���õ���������
**  @param[in]  const std::string& block_name ������
**	@param[in]  const std::string& node_name �ڵ�����������
**	@param[out] int& value  ���صĽ��
**  @param[in]  int  defaut Ĭ�ϲ���
**/
void xml_config::get_value(const std::string& block_name, const std::string& node_name, int32_t& value, int defaut)
{
	if (!m_root_elem_)
	{
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	TiXmlElement* node_elem = m_root_elem_->FirstChildElement(block_name.c_str());
	if (node_elem->FirstChildElement(node_name.c_str()))                          //����ǽڵ�
	{
		value = atoi(node_elem->FirstChildElement(node_name.c_str())->GetText());
	}
    else if (node_elem->Attribute(node_name.c_str()))                            //���������
	{
		value = atoi(node_elem->Attribute(node_name.c_str()));
	}
    else
	{
		if (defaut == -1)
		{
			std::cerr << "node:" << node_name << " is null!" << std::endl;
			exit(-1);
		}
		value = defaut;
		std::cerr << node_name << " use default value[" << defaut << "]." << std::endl;
	}
}

/***
**	@brief���õ��ַ����������ݣ�ֻ��һ��Ľṹ
**	@param[in]  const std::string& node_name �ڵ���
**	@param[in]  char* value  ���صĽ��
**/
void xml_config::get_value(const std::string& node_name, char* value)
{
	if (!m_root_elem_)
	{
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	TiXmlElement* node_elem = m_root_elem_->FirstChildElement(node_name.c_str());
	if (node_elem)
	{
		strcpy(value, node_elem->GetText());
	}
    else
	{
		std::cerr << "node:"<<node_name << " is null!" << std::endl;
		exit(-1);
	}
}


/***
**	@brief���õ���������
**	@param[in]  const std::string& node_name �ڵ���
**	@param[out] int& value  ���صĽ��
**  @param[in]  int  defaut Ĭ�ϲ���
**/
void xml_config::get_value(const std::string& node_name, int32_t& value, int defaut)
{
	if (!m_root_elem_)
	{
		std::cerr << "read root error!" << std::endl;
		exit(-1);
	}

	TiXmlElement* node_elem = m_root_elem_->FirstChildElement(node_name.c_str());
	if (node_elem)
	{
		value = atoi(node_elem->GetText());
	}
	else
	{
		if (defaut == -1)
		{
			std::cerr << "node:" << node_name << " is null!" << std::endl;
			exit(-1);
		}
		value = defaut;
		std::cerr << node_name << " use default value[" << defaut << "]." << std::endl;
	}
}

/*
@brief �ض������ļ�
@param[in] const char* xml_file_name
*/
void xml_config::reload_config(const char* xml_file_name)
{
	m_xml_conf_file_ = xml_file_name;
	m_root_elem_ = 0;
	//���������ļ�
	m_xml_conf_file_.LoadFile();
	//�õ������ļ��ĸ����
	m_root_elem_ = m_xml_conf_file_.RootElement();
	return;
}

/*
@brief ��ʾ�����ļ�����
*/
void xml_config::list_all()
{
	dump_to_stdout(&m_xml_conf_file_);
	return;
}

void xml_config::dump_to_stdout( TiXmlNode* parent, uint32_t indent)
{
	if ( !parent ) return;

	TiXmlNode* child;
	TiXmlText* text;
	int t = parent->Type();
	printf( "%s", get_indent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		printf( "Element [%s]", parent->Value() );
		num = dump_attribs_to_stdout(parent->ToElement(), indent+1);
		switch (num)
		{
		case 0: printf( " (No attributes)"); break;
		case 1: printf( "%s1 attribute", get_indent_alt(indent)); break;
		default: printf( "%s%d attributes", get_indent_alt(indent), num); break;
		}
		break;

	case TiXmlNode::TINYXML_COMMENT:
		printf( "Comment: [%s]", parent->Value());
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TINYXML_TEXT:
		text = parent->ToText();
		printf( "Text: [%s]", text->Value() );
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}

	printf( "\n" );
	for ( child = parent->FirstChild(); child != 0; child = child->NextSibling())
	{
		dump_to_stdout( child, indent+1 );
	}
}

// ����������������������������������������������-
// STDOUT���������ʵ�ú���
// ����������������������������������������������-
const char* xml_config::get_indent( unsigned int num_indents )
{
	static const char* indent = " + ";
	static const unsigned int len = strlen( indent );
	unsigned int n = num_indents * INDENTS_COUNT_PER_SPACE;
	if ( n > len ) n = len;

	return &indent[ len-n ];
}

// ��get_indent��ͬ�������û�С�+��
const char* xml_config::get_indent_alt( unsigned int num_indents )
{
	static const char* indent = " ";
	static const unsigned int len = strlen( indent );
	unsigned int n = num_indents * INDENTS_COUNT_PER_SPACE;
	if ( n > len ) n = len;

	return &indent[ len-n ];
}

int xml_config::dump_attribs_to_stdout(TiXmlElement* elem, unsigned int indent)
{
	if ( !elem ) return 0;

	TiXmlAttribute* attrib = elem->FirstAttribute();
	int i = 0;
	int ival;
	double dval;
	const char* con_indent = get_indent(indent);
	printf("\n");
	while (attrib)
	{
		printf( "%s%s: value=[%s]", con_indent, attrib->Name(), attrib->Value());

		if (attrib->QueryIntValue(&ival) == TIXML_SUCCESS) printf( " int=%d", ival);
		if (attrib->QueryDoubleValue(&dval) == TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		attrib = attrib->Next();
	}
	return i;
}
