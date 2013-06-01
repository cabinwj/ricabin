#ifndef _HC_TYPE_TRAIT_H_
#define _HC_TYPE_TRAIT_H_


template <typename T>
struct type_trait
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_NONE); }
};

template <>
struct type_trait<char>
{
    enum {
        IS_CHAR = 1,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_CHAR); }
};
template <>
struct type_trait<const char>
{
    enum {
        IS_CHAR = 1,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_CHAR); }
};

template <>
struct type_trait<signed char>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 1,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_SIGNED_CHAR); }
};
template <>
struct type_trait<const signed char>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 1,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_SIGNED_CHAR); }
};

template <>
struct type_trait<unsigned char>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 1,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_CHAR); }
};
template <>
struct type_trait<const unsigned char>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 1,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_CHAR); }
};

template <>
struct type_trait<short>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 1,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_SHORT); }
};
template <>
struct type_trait<const short>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 1,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_SHORT); }
};

template <>
struct type_trait<unsigned short>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 1,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_SHORT); }
};
template <>
struct type_trait<const unsigned short>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 1,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_SHORT); }
};

template <>
struct type_trait<int>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 1,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_INT); }
};
template <>
struct type_trait<const int>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 1,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_INT); }
};

template <>
struct type_trait<unsigned int>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 1,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_INT); }
};
template <>
struct type_trait<const unsigned int>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 1,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_INT); }
};

template <>
struct type_trait<long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 1,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_LONG); }
};
template <>
struct type_trait<const long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 1,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_LONG); }
};

template <>
struct type_trait<unsigned long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 1,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_LONG); }
};
template <>
struct type_trait<const unsigned long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 1,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_LONG); }
};

template <>
struct type_trait<long long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 1,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_LONG_LONG); }
};
template <>
struct type_trait<const long long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 1,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_LONG_LONG); }
};

template <>
struct type_trait<unsigned long long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 1,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_LONG_LONG); }
};
template <>
struct type_trait<const unsigned long long>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 1,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_UNSIGNED_LONG_LONG); }
};

template <>
struct type_trait<float>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 1,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_FLOAT); }
};
template <>
struct type_trait<const float>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 1,
        IS_DOUBLE = 0,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_FLOAT); }
};

template <>
struct type_trait<double>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 1,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_DOUBLE); }
};
template <>
struct type_trait<const double>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 1,
        IS_POINTER = 0,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_DOUBLE); }
};

template <class T>
struct type_trait<T*>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 1,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_POINTER); }
};
template <class T>
struct type_trait<const T*>
{
    enum {
        IS_CHAR = 0,
        IS_SIGNED_CHAR = 0,
        IS_UNSIGNED_CHAR = 1,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_LONG = 0,
        IS_UNSIGNED_LONG = 0,
        IS_LONG_LONG = 0,
        IS_UNSIGNED_LONG_LONG = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINTER = 1,
        IS_FUNCTION = 0
    };

    enum {
        TT_NONE = 0,
        TT_CHAR = 1,
        TT_SIGNED_CHAR,
        TT_UNSIGNED_CHAR,
        TT_SHORT,
        TT_UNSIGNED_SHORT,
        TT_INT,
        TT_UNSIGNED_INT,
        TT_LONG,
        TT_UNSIGNED_LONG,
        TT_LONG_LONG,
        TT_UNSIGNED_LONG_LONG,
        TT_FLOAT,
        TT_DOUBLE,
        TT_POINTER,
        TT_FUNCTION,
    };

    static int TT() { return (int)(TT_POINTER); }
};


#endif

/*
  #include <iostream>
  using namespace std;

  int main(int argc,char * argv[])
  {
      cout<<type_test<const signed char>::IS_CHAR<<endl;
      cout<<type_test<const  char>::IS_CHAR<<endl;
      cout<<type_test<const int>::IS_INT<<endl;
      return 0;
  }
*/
