#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string>

using namespace std;

//! �ַ���������
class string_util
{
public:
    //! ת��Ϊ��д
    static string upper(string str);

    //! ת��Ϊ��д
    static void upper(char* str);

    //! ת��ΪСд
    static string lower(string str);

    //! ת��ΪСд
    static void lower(char *str);

    //! ȥ������skip�е��ַ�
    static string ltrim(string str, string skip = " \t");

    //! ȥ������skip�е��ַ�
    static void ltrim(char *str, const char *skip = " \t");

    //! ȥ���Ҳ��skip�е��ַ�
    static string rtrim(string str, string skip = " \t");

    //! ȥ���Ҳ��skip�е��ַ�
    static void rtrim(char *str, const char *skip = " \t");

    //! ȥ�������skip�е��ַ�
    static string trim(string str, string skip = " \t");

    //! ȥ�������skip�е��ַ�
    static void trim(char *str, const char *skip = " \t");

    //! 
    static string escape(const string&);
};

#endif // _STRING_UTIL_H_
