#ifndef _NET_HANDLER_H_
#define _NET_HANDLER_H_

#include "hc_types.h"
//#include "hc_rwlock.h"
//#include "hc_thread_mutex.h"

#if defined(_MSC_VER)
#include <hash_map>
#include <hash_set>
using stdext::hash_map;
using stdext::hash_set;
#elif defined(__GNUC__)
#include <ext/hash_map>
#include <ext/hash_set>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;
#endif


class ihandler;

class net_handler
{
public:
    typedef hash_map<int32_t, ihandler*> net2handler_hashmap;

public:
    static net2handler_hashmap m_net2hdr_hashmap_;
    ////! 线程锁, 保护处理器 handler_list
    //static rw_lockc m_rwlock_;
    //static threadc_mutex m_mutex_;

public:
    static ihandler* select_handler(int32_t net_id);
    static void insert_handler(ihandler* ish);
    static void remove_handler(ihandler* ish);
    static void remove_handler(int32_t net_id);
    static void clear_all_handler();
    static int32_t current_handler_count();
};

#endif // _NET_HANDLER_H_
