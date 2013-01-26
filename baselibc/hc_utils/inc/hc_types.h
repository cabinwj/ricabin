//! @file types.h
//! @brief 模块用到的各项基本数据类型定义
#ifndef _BASE_TYPES_H_
#define _BASE_TYPES_H_

#include <stddef.h>                // for size_t
#include <stdint.h>

#ifdef WIN32    // for windows
#define CHECK_FORMAT(i, j)
#else            // for linux(gcc)
#define CHECK_FORMAT(i, j) __attribute__((format(printf, i, j)))
#endif

#ifdef WIN32    // for windows

//typedef __int8                    int8_t;
//typedef __int16                    int16_t;
//typedef __int32                    int32_t;
//typedef __int64                    int64_t;
//
//typedef unsigned __int8            uint8_t;
//typedef unsigned __int16        uint16_t;
//typedef unsigned __int32        uint32_t;
//typedef unsigned __int64        uint64_t;

typedef int                        ssize_t;
typedef int                        socklen_t;
typedef uint32_t                in_addr_t;
typedef uint16_t                in_port_t;

#else            // for linux

#include <getopt.h>
#include <unistd.h>                // for ssize_t and socklen_t
#include <netinet/in.h>            // for in_addr_t and in_port_t

#endif

//#ifdef _WIN64
//  typedef long int                 intptr_t;
//  typedef unsigned long int            uintptr_t;
//#else
//  typedef int                    intptr_t;
//  typedef unsigned int            uintptr_t;
//#endif

#endif // _TYPES_H_
