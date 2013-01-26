#include "hc_memory.h"
#include "hc_base.h"

#include <assert.h>
#include <time.h>

#include "hc_os.h"


char* shm_memory::shm_memory_pointer_ = NULL;

shm_memory::shm_memory()
{
    available_shm_pointer_ = shm_memory_pointer_ + sizeof(*this);
}

shm_memory::shm_memory(key_t shm_key, size_t size, int32_t init_flag)
{
    assert(init_flag);

    available_shm_pointer_ = shm_memory_pointer_ + sizeof(*this);
    init(shm_key, size);
    
    return;
}

shm_memory::shm_memory(key_t shm_key, size_t size )
{
    available_shm_pointer_ = shm_memory_pointer_ + sizeof(*this);

    if (shm_key != shm_key_ || size != shm_size_)
    {
        init(shm_key, size);
        return;
    }

    //检查校验和
    uint32_t crc = (uint32_t)shm_key_
                    ^ (uint32_t)shm_size_
                    ^ (uint32_t)create_time_
                    ^ (uint32_t)last_timestamp_
                    ^ crc_;

    if (0 != crc) //校验不通过，则进行初始化
    {
        init(shm_key, size);
        return;
    }

    time_t now = time(NULL);
    if ((now - create_time_) < 0 || (now - create_time_) > 1*YEAR)
    {
        init(shm_key, size);
        return;
    }

    if ((now - last_timestamp_) > 2*HOUR)
    {
        init(shm_key, size);
        return;
    }

    set_time_stamp();
    shm_mode_ = shm_mode_resume;

    return;
}

shm_memory::~shm_memory()
{  
}

int32_t shm_memory::init(key_t shm_key, size_t size)
{
    shm_mode_ = shm_mode_init;
    shm_key_ = shm_key;
    shm_size_ = size;

    time_t now;
    time(&now);
    create_time_ = now;
    last_timestamp_ = now;

    //计算校验和
    crc_ = (uint32_t)shm_key_
            ^ (uint32_t)shm_size_
            ^ (uint32_t)create_time_
            ^ (uint32_t)last_timestamp_;

    return 0;
}

void shm_memory::set_time_stamp()
{
    time_t now;
    time(&now);

    last_timestamp_ = now;

    crc_ = (uint32_t)shm_key_
            ^ (uint32_t)shm_size_
            ^ (uint32_t)create_time_
            ^ (uint32_t)last_timestamp_;
}

void* shm_memory::operator new(size_t size) throw()
{  
    if (NULL == shm_memory_pointer_)
    {
        return NULL;
    }

    return (void*)shm_memory_pointer_;
}

void shm_memory::operator delete(void* memory_pointer)
{
}

int32_t shm_memory::get_mode()
{    
    return shm_mode_;
}

void* shm_memory::create_segment(size_t size)
{
    if (size <= 0)
    {
        return NULL;
    }

    if (size > get_free_size())
    {
        return NULL;
    }

    char* memory_pointer = available_shm_pointer_;
    available_shm_pointer_ += size;

    return (void*)memory_pointer;  
}

size_t shm_memory::get_free_size()
{
    int used_size = get_used_size();
    return (shm_size_ - used_size);
}

size_t shm_memory::get_used_size()
{
    return (size_t)(available_shm_pointer_ - (char*)this);
}

shm_memory* ushm_map::create_shm_memory(key_t shm_key, size_t size, int32_t init_flag/*=0*/)
{
    if (size <= 0)
    {
        return NULL;
    }

    size_t real_size = size + sizeof(shm_memory);

#ifndef WIN32
    //首先去创建指定大小的共享内存
    int shm_id = shmget(shm_key, real_size, IPC_CREAT|IPC_EXCL|0666);
    if (shm_id < 0)
    {
        if (error_no() != EEXIST )
        {
            return NULL;
        }
        
        //Ya! 失败，那就去Attach指定大小的内存
        shm_id = shmget(shm_key, real_size, 0666);
        if (shm_id < 0)
        {
            //Yaya!还是失败，那爱attach多少就多少吧
            shm_id = shmget(shm_key, 0, 0666);
            if (shm_id < 0)
            {
                //只有放弃
                return NULL;
            }
            else
            {
                //不符合内存大小的要求，删之
                if (shmctl(shm_id, IPC_RMID, NULL))
                {
                    return NULL;
                }

                //然后，重新创建崭新的内存
                shm_id = shmget(shm_key, real_size,  IPC_CREAT|IPC_EXCL|0666);
                if (shm_id < 0)
                {
                    return NULL;
                }
            }
        }
        else
        {
            //attach到已有内存区域，并没有真正创建新的内存
        } 
    }

    //attach到自己家先
    shm_memory::shm_memory_pointer_ = (char*)shmat(shm_id, NULL, 0);
    if (NULL == shm_memory::shm_memory_pointer_)
    {
        return NULL;
    }
    
    //在得到的内存上创建一个管理对象先
    if (init_flag)
    {
        return (new shm_memory(shm_key, real_size, init_flag));
    }
    else
    {
        return (new shm_memory(shm_key, real_size)); 
    }

    return NULL;

#else

    hShmmap_ = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)real_size, (LPCWSTR)(&shm_key));

    if (INVALID_HANDLE_VALUE == hShmmap_)
    {
        return NULL;
    }
    else if (ERROR_ALREADY_EXISTS == GetLastError())
    {
        CloseHandle(hShmmap_);
        return NULL;
    }        

    pViewmap_ = MapViewOfFile(hShmmap_, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, real_size);
    if (NULL == pViewmap_)
    {
        CloseHandle(pViewmap_);
        return NULL;
    }

    shm_memory::shm_memory_pointer_ = (char*)(pViewmap_);

    //在得到的内存上创建一个管理对象先
    if (init_flag)
    {
        return (new shm_memory(shm_key, real_size, init_flag));
    }
    else
    {
        return (new shm_memory(shm_key, real_size)); 
    }

    return NULL;

#endif
}

int32_t ushm_map::destroy_shm_memory(key_t shm_key)
{
#ifndef WIN32
    int shm_id = shmget(shm_key, 0, 0666 );
    if (shm_id < 0)
    {
        return -1;
    }
    
    if (shmctl(shm_id, IPC_RMID, NULL))
    {
        return -1;
    }

    return 0;
#else
    BOOL bret = UnmapViewOfFile(pViewmap_);
    if (bret)
    {
        bret = CloseHandle(hShmmap_);
        if (bret)
        {
            return 0;
        }
    }

    return -1;

#endif
}

