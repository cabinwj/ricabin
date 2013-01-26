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
  *�����ڴ����
  *���ƣ�����
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
    //�����ڴ��
    key_t shm_key_;
    //�ڴ��ܴ�С
    size_t shm_size_;
    //�ڴ�ͷУ���
    uint32_t crc_;
    //�ڴ洴��ʱ��
    time_t create_time_;
    //������ʱ��
    time_t last_timestamp_;
    //�����ڴ�
    char* available_shm_pointer_;
    //�ڴ�ʹ��ģʽ
    int32_t shm_mode_;

public:
    shm_memory();
    shm_memory(key_t shm_key, size_t size);
    shm_memory(key_t shm_key, size_t size, int32_t init_flag);
    ~shm_memory();

public:
    //��ʱ�������ֹ�ڴ�ʧЧ
    void set_time_stamp();

    void* operator new(size_t size) throw();
    void  operator delete(void* memory_pointer);

    int32_t get_mode();
    //�����СΪsize���ڴ�飬ʧ�ܷ���NULL
    void* create_segment(size_t size);
    //�õ����ÿռ��С����λ:Byte
    size_t get_free_size();
    //�õ����ÿռ��С����λ:Byte
    size_t get_used_size();

protected:
    int32_t init(key_t shm_key, size_t size);
};


typedef struct ushm_map
{
    //���������ڴ�
    shm_memory* create_shm_memory(key_t shm_key, size_t size, int32_t init_flag = 0);

    //���չ����ڴ�
    int32_t destroy_shm_memory(key_t shm_key);

#ifdef WIN32
    HANDLE hShmmap_;
    LPVOID pViewmap_;
#endif
}ushm_map_t;


#endif
