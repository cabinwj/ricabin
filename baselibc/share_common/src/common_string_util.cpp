#include "common_string_util.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <WinSock2.h>
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

// class string_util
string string_util::upper(string str)
{
    string result = "";

    for (string::const_iterator c = str.begin(); c != str.end(); ++c)
    {
        result += toupper(*c);
    }

    return result;
}

void string_util::upper(char* str)
{
    int i = 0;
    while(str[i] != '\0' )
    {
        str[i] = toupper(str[i]);
        i++;
    }
}

string string_util::lower(string str)
{
    string result = "";

    for (string::const_iterator c = str.begin(); c != str.end(); ++c)
    {
        result += tolower(*c);
    }

    return result;
}

void string_util::lower(char *str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        str[i] = tolower(str[i]);
        i++;
    }
}

string string_util::ltrim(string str, string skip)
{
    string::size_type pos;
    for (pos = 0; pos < str.length(); pos++)
    {
        if (string::npos == skip.find(str[pos]))
            break;
    }
    return str.substr(pos);
}

void string_util::ltrim(char *str, const char *skip)
{
    char s[2];
    s[1] = 0;

    size_t i;
    for (i = 0; i < strlen(str); i++)
    {
        s[0] = str[i];
        if (NULL == strstr(skip, s))
        {
            break;
        }
    }

    int j = 0;
    for (size_t p = i; p < strlen(str) + 1; p++)
    {
        str[j++] = str[p];
    }
}

string string_util::rtrim(string str, string skip)
{
    string::size_type pos;
    for (pos = str.length() - 1; pos >= 0; pos--)
    {
        if (string::npos == skip.find(str[pos]))
            break;
    }
    return str.substr(0, pos + 1);
}

void string_util::rtrim(char *str, const char *skip)
{
    char s[2];
    s[1] = 0;

    for (int i = (int)strlen(str); i >= 0; i--)
    {
        s[0] = str[i];
        if (NULL == strstr(skip, s))
        {
            break;
        }
        else
        {
            str[i] = 0;
        }
    }
}

string string_util::trim(string str, string skip)
{
    return rtrim(ltrim(str, skip), skip);
}

void string_util::trim(char *str, const char *skip)
{
    rtrim(str, skip);
    ltrim(str, skip);
}

string string_util::escape(const string& str)
{
    std::string res;
    for (size_t i=0; i<str.length(); i++)
    {
        char c = str[i];
        if (c >= 0x20 && c <= 0x7e)
        {
            res += c;
        }
        else
        {
            switch (c)
            {
            case '\t':
                res += "\\t"; break;
            case '\n':
                res += "\\n"; break;
            case '\r':
                res += "\\r"; break;
            case '\\':
                res += "\\\\"; break;
            case '\0':
                res += "\\0"; break;
            default: {
                char buf[5];
                snprintf(buf, sizeof(buf), "\\x%02x", (unsigned char)c);
                res += buf;
                }
            }
        }
    }
    return res;
}

