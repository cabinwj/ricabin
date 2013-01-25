#include "common_text_configure.h"
#include "common_string_util.h"

#include <fstream>
using namespace std;

// class text_configure
int text_configure::load(const char *file)
{
	vector<string> lines;
	vector<string> last_comments;
	vector<string> line_comments;

	// 读取配置文件, 分析出注释
	ifstream config_file;

	config_file.open(file);
	if (!config_file.is_open()) {
		return -1;
	}

	char l[10240];
	string last_comment;
	while (config_file.getline(l, 10240))
	{
		string line_comment;

		// 兼容DOS, UNIX两种文件格式
		for (size_t i = 0; i < strlen(l); i++)
		{
			if ((l[i] == '\r') || (l[i] == '\n')) {
				l[i] = 0;
			}
		}

		// 取出注释
		char *comment = strstr(l, "#");
		if (comment != NULL) {
			line_comment = comment + 1;
			*comment = 0;
		}

		// trim " \t"
		string_util::trim(l, " \t");

		// 忽略空行和无意义行
		if (strlen(l) < 1) {
			// 此行为一个纯注释行
			if (line_comment.length() >= 1) {
				last_comment = line_comment;
			}
			continue;
		}

		//
		string line = l;

		lines.push_back(line);
		last_comments.push_back(last_comment);
		line_comments.push_back(line_comment);

		last_comment = "";
	}

	config_file.close();

	// 分析配置
	string section;
	for (size_t i = 0; i < lines.size(); i++)
	{
		string line = lines[i];

		// 记录section
		if ((line.length() > 2) && (line[0] == '[') && (line[line.length() - 1] == ']'))
		{
			section = line.substr(1, line.length() - 2);
			add_section(section.c_str(), last_comments[i].c_str(), line_comments[i].c_str());
			continue;
		}

		// 取出key-value
		string::size_type pos = line.find("=");
		if (pos == string::npos) {
			continue;
		}

		string key = line.substr(0, pos);
		string value = line.substr(pos + 1);

		key = string_util::trim(key, " \t");
		value = string_util::trim(value, " \t");

		if (value == "(")
		{
			vector<string> val;
			int x = i;
			for (i = i + 1; i < lines.size(); i++)
			{
				line = lines[i];
				if (line[0] == ')') {
					break;
				}

				if ((line.length() > 2) && (line[0] == '(') && (line[line.length() - 1] == ')')) {
					string v = line.substr(1, line.length() - 2);
					val.push_back(v);
				}
				else {
					return -1;
				}
			}

			add_value_array2(section.c_str(), key.c_str(), val, last_comments[x].c_str(), line_comments[x].c_str());
		}
		else if ((value.length() > 2) && (value[0] == '(') && (value[value.length() - 1] == ')'))
		{
			string v = value.substr(1, value.length() - 2);
			add_value_array(section.c_str(), key.c_str(), v.c_str(), last_comments[i].c_str(), line_comments[i].c_str());
		}
		else
		{
			add_value_single(section.c_str(), key.c_str(), value.c_str(), last_comments[i].c_str(), line_comments[i].c_str());
		}	
	}

	return 0;
}

int text_configure::get_single(const char *section, const char *key, string &val)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_SINGLE) {
		// 值类型错误
		return -1;
	}

	val = v->val_single;
	return 0;
}

int text_configure::get_single(const char *section, const char *key, long &val)
{
	string s;
	int rc = get_single(section, key, s);
	if (0 != rc) {
		return -1;
	}

	val = atol(s.c_str());
	return 0;
}

int text_configure::get_single(const char *section, const char *key, double &val)
{
	string s;
	int rc = get_single(section, key, s);
	if (0 != rc) {
		return -1;
	}

	val = atof(s.c_str());
	return 0;
}

int text_configure::count1_array(const char *section, const char *key, int &count)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY) {
		// 值类型错误
		return -1;
	}

	count = (int)v->val_array.size();
	return 0;
}

int text_configure::get_array(const char *section, const char *key, int i, string &val)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY) {
		// 值类型错误
		return -1;
	}

	int count = (int)v->val_array.size();
	if ((i < 0) || (i >= count)) {
		// 下标范围错误
		return -1;
	}

	val = v->val_array[i];
	return 0;
}

int text_configure::get_array(const char *section, const char *key, int i, long &val)
{
	string s;
	int rc = get_array(section, key, i, s);
	if (0 != rc) {
		return -1;
	}

	val = atol(s.c_str());
	return 0;
}

int text_configure::get_array(const char *section, const char *key, int i, double &val)
{
	string s;
	int rc = get_array(section, key, i, s);
	if (0 != rc) {
		return -1;
	}

	val = atof(s.c_str());
	return 0;
}

int text_configure::count1_array2(const char *section, const char *key, int &count)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY2) {
		// 值类型错误
		return -1;
	}

	count = (int)v->val_array2.size();
	return 0;
}

int text_configure::count2_array2(const char *section, const char *key, int i, int &count)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY2) {
		// 值类型错误
		return -1;
	}

	int c1 = (int)v->val_array2.size();
	if ((i < 0) || (i >= c1)) {
		// 下标范围错误
		return -1;
	}

	count = (int)v->val_array2[i].size();
	return 0;
}

int text_configure::get_array2(const char *section, const char *key, int i, int j, string &val)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY2) {
		// 值类型错误
		return -1;
	}

	int c1 = (int)v->val_array2.size();
	if ((i < 0) || (i >= c1)) {
		// 下标范围错误
		return -1;
	}

	int c2 = (int)v->val_array2[i].size();
	if ((j < 0) || (j >= c2)) {
		// 下标范围错误
		return -1;
	}

	val = v->val_array2[i][j];
	return 0;
}

int text_configure::get_array2(const char *section, const char *key, int i, int j, long &val)
{
	string s;
	int rc = get_array2(section, key, i, j, s);
	if (0 != rc) {
		return -1;
	}

	val = atol(s.c_str());
	return 0;
}

int text_configure::get_array2(const char *section, const char *key, int i, int j, double &val)
{
	string s;
	int rc = get_array2(section, key, i, j, s);
	if (0 != rc) {
		return -1;
	}

	val = atoi(s.c_str());
	return 0;
}

int text_configure::save(const char *file)
{
	// 读取配置文件, 分析出注释
	ofstream config_file;

	config_file.open(file);
	if (!config_file.is_open()) {
		return -1;
	}

	for (size_t i = 0; i < m_configure.size(); i++)
	{
		Section &sec = m_configure[i];

		if (sec.comment1.length() >= 1) {
			config_file << "#" << sec.comment1 << endl;
		}

		config_file << "[" << sec.section << "]";
		if (sec.comment2.length() > 1) {
			config_file << "\t#" << sec.comment2;
		}
		config_file << endl;

		for (size_t j = 0; j < sec.values.size(); j++)
		{
			Value &val = sec.values[j];

			if (val.comment1.length() >= 1) {
				config_file << "#" << val.comment1 << endl;
			}

			if (val.type == VT_SINGLE)
			{
				config_file << val.key << " = " << val.val_single;
				if (val.comment2.length() > 1) {
					config_file << "\t#" << val.comment2;
				}
				config_file << endl;
			}
			else if (val.type == VT_ARRAY)
			{
				config_file << val.key << " = (";
				for (size_t m = 0; m < val.val_array.size(); m++)
				{
					if (m != (val.val_array.size() - 1)) {
						config_file << val.val_array[m] << " ";
					}
					else {
						config_file << val.val_array[m];
					}
				}
				config_file << ")";
				if (val.comment2.length() > 1) {
					config_file << "\t#" << val.comment2;
				}
				config_file << endl;
			}
			else if (val.type == VT_ARRAY2)
			{
				config_file << val.key << " = (";
				if (val.comment2.length() > 1) {
					config_file << "\t#" << val.comment2;
				}
				config_file << endl;

				for (size_t m = 0; m < val.val_array2.size(); m++)
				{
					config_file << "\t(";

					for (size_t n = 0; n < val.val_array2[m].size(); n++)
					{
						if (n != (val.val_array2[m].size() - 1)) {
							config_file << val.val_array2[m][n] << " ";
						}
						else {
							config_file << val.val_array2[m][n];
						}
					}

					config_file << ")" << endl;
				}

				config_file << ")" << endl;
			}
		}

		config_file << endl << endl;
	}

	config_file.close();

	return 0;
}

int text_configure::set_single(const char *section, const char *key, const string &val)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_SINGLE) {
		// 值类型错误
		return -1;
	}

	v->val_single = val;
	return 0;
}

int text_configure::set_array(const char *section, const char *key, const vector<string> &val)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY) {
		// 值类型错误
		return -1;
	}

	v->val_array = val;
	return 0;
}

int text_configure::set_array2(const char *section, const char *key, const vector<vector<string> > &val)
{
	Value *v = NULL;
	int rc = get_value(section, key, v);
	if (0 != rc) {
		// key 不存在
		return -1;
	}

	if (v->type != VT_ARRAY2) {
		// 值类型错误
		return -1;
	}

	v->val_array2 = val;
	return 0;
}

int text_configure::get_section(const char *section, Section *&sec)
{
	for (size_t i = 0; i < m_configure.size(); i++)
	{
		Section& s = m_configure[i];
		if (s.section == section) {
			sec = &s;
			return 0;
		}
	}

	return -1;
}

int text_configure::get_value(const char *section, const char *key, Value *&value)
{
	Section *sec = NULL;
	int rc = get_section(section, sec);
	if (0 != rc) {
		return -1;
	}

	for (size_t i = 0; i < sec->values.size(); i++)
	{
		Value& v = sec->values[i];
		if (v.key == key) {
			value = &v;
			return 0;
		}
	}

	return -1;
}

int text_configure::add_section(const char *section, const char *comment1, const char *comment2)
{
	Section *s = NULL;
	int rc = get_section(section, s);
	if (0 == rc) {
		// section 已经存在
		return -1;
	}

	Section sec;
	sec.section = section;
	sec.comment1 = comment1;
	sec.comment2 = comment2;
	m_configure.push_back(sec);
	return 0;
}

int text_configure::add_value_single(const char *section, const char *key, const char *value, const char *comment1, const char *comment2)
{
	Section *sec = NULL;
	int rc = get_section(section, sec);
	if (0 != rc) {
		// section 不存在
		return -1;
	}

	Value *v = NULL;
	rc = get_value(section, key, v);
	if (0 == rc) {
		// key 已存在
		return -1;
	}

	Value val;
	val.key = key;
	val.type = VT_SINGLE;
	val.val_single = value;
	val.comment1 = comment1;
	val.comment2 = comment2;

	sec->values.push_back(val);
	return 0;
}

int text_configure::add_value_array(const char *section, const char *key, const char *line, const char *comment1, const char *comment2)
{
	Section *sec = NULL;
	int rc = get_section(section, sec);
	if (0 != rc) {
		// section 不存在
		return -1;
	}

	Value *v = NULL;
	rc = get_value(section, key, v);
	if (0 == rc) {
		// key 已存在
		return -1;
	}

	Value val;
	val.key = key;
	val.type = VT_ARRAY;
	val.comment1 = comment1;
	val.comment2 = comment2;

	// parse line
	string l = line;
	for (size_t i = 0; i < l.length(); i++)
	{
		if (l[i] == '\t') {
			l[i] = ' ';
		}
	}

	while (true)
	{
		l = string_util::trim(l, " \t");

		string::size_type pos = l.find(" ");
		if (pos != string::npos)
		{
			string item = l.substr(0, pos);
			l = l.substr(pos + 1);
			val.val_array.push_back(item);
		}
		else
		{
			l = string_util::trim(l, " \t");
			if (l.length() > 0) {
				val.val_array.push_back(l);
			}
			break;
		}
	}

	sec->values.push_back(val);
	return 0;
}

int text_configure::add_value_array2(const char *section, const char *key, const vector<string> &lines, const char *comment1, const char *comment2)
{
	Section *sec = NULL;
	int rc = get_section(section, sec);
	if (0 != rc) {
		// section 不存在
		return -1;
	}

	Value *v = NULL;
	rc = get_value(section, key, v);
	if (0 == rc) {
		// key 已存在
		return -1;
	}

	Value val;
	val.key = key;
	val.type = VT_ARRAY2;
	val.comment1 = comment1;
	val.comment2 = comment2;

	// parse lines
	for (size_t i = 0; i < lines.size(); i++)
	{
		vector<string> val_array;

		string l = lines[i];
		for (size_t i = 0; i < l.length(); i++)
		{
			if (l[i] == '\t') {
				l[i] = ' ';
			}
		}

		while (true)
		{
			l = string_util::trim(l, " \t");

			string::size_type pos = l.find(" ");
			if (pos != string::npos)
			{
				string item = l.substr(0, pos);
				l = l.substr(pos + 1);
				val_array.push_back(item);
			}
			else
			{
				l = string_util::trim(l, " \t");
				if (l.length() > 0) {
					val_array.push_back(l);
				}
				break;
			}
		}

		val.val_array2.push_back(val_array);
	}

	sec->values.push_back(val);
	return 0;
}

