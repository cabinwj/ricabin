#ifndef __COMMON_MEMEORY_H__
#define __COMMON_MEMEORY_H__

#include "hc_types.h"
#include "hc_os.h"

#ifdef WIN32
#ifndef key_t
typedef int key_t;
#endif
#else
#include <sys/types.h>
#include <sys/shm.h>
#endif

/**
  *共享内存管理
  *限制：单线
  */

class shm_memory
{
public:
    enum
    {
        shm_mode_init = 0,
        shm_mode_resume = 1
    };

public:
    static char* shm_memory_pointer_;

protected:
    //共享内存键
    key_t shm_key_;
    //内存总大小
    size_t shm_size_;
    //内存头校验和
    uint32_t crc_;
    //内存创建时间
    time_t create_time_;
    //最后访问时间
    time_t last_timestamp_;
    //可用内存
    char* available_shm_pointer_;
    //内存使用模式
    int32_t shm_mode_;

public:
    shm_memory();
    shm_memory(key_t shm_key, size_t size);
    shm_memory(key_t shm_key, size_t size, int32_t init_flag);
    ~shm_memory();

public:
    //打时间戳，防止内存失效
    void set_time_stamp();

    void* operator new(size_t size) throw();
    void  operator delete(void* memory_pointer);

    int32_t get_mode();
    //分配大小为size的内存块，失败返回NULL
    void* create_segment(size_t size);
    //得到可用空间大小，单位:Byte
    size_t get_free_size();
    //得到已用空间大小，单位:Byte
    size_t get_used_size();

protected:
    int32_t init(key_t shm_key, size_t size);
};


typedef struct ushm_map
{
    //创建共享内存
    shm_memory* create_shm_memory(key_t shm_key, size_t size, int32_t init_flag = 0);

    //回收共享内存
    int32_t destroy_shm_memory(key_t shm_key);

#ifdef WIN32
    HANDLE hShmmap_;
    LPVOID pViewmap_;
#endif
}ushm_map_t;


#endif
