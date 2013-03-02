#ifndef _HC_TYPE_TRAIT_H_
#define _HC_TYPE_TRAIT_H_


template <typename T>
struct type_test
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};

template <>
struct type_test<char>
{
    enum {
        IS_CHAR = 1,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<unsigned char>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 1,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const char>
{
    enum {
        IS_CHAR = 1,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const unsigned char>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 1,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};

template <>
struct type_test<short>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 1,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<unsigned short>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 1,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const short>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 1,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const unsigned short>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 1,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};

template <>
struct type_test<int>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 1,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<unsigned int>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 1,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const int>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 1,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const unsigned int>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 1,
        IS_FLOAT = 0,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};

template <>
struct type_test<float>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 1,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const float>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 1,
        IS_DOUBLE = 0,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<double>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 1,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
};
template <>
struct type_test<const double>
{
    enum {
        IS_CHAR = 0,
        IS_UNSIGNED_CHAR = 0,
        IS_SHORT = 0,
        IS_UNSIGNED_SHORT = 0,
        IS_INT = 0,
        IS_UNSIGNED_INT = 0,
        IS_FLOAT = 0,
        IS_DOUBLE = 1,
        IS_POINT = 0,
        IS_FUNCTION = 0
    };
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
