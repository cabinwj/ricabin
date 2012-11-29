#include "common_code_queue.h"
#include "common_memory.h"

#include <string.h>

shm_memory* object_code_queue::shared_memory_pointer_ = NULL;

void object_code_queue::set_sharedmemory(shm_memory* shm_memory_pointer)
{
    shared_memory_pointer_ = shm_memory_pointer;
}

size_t object_code_queue::size(int32_t buffersize)
{
    size_t itotal = sizeof(object_code_queue);
    if (buffersize > 0)
    {
        itotal += buffersize;
    }

    return itotal;
}

object_code_queue::object_code_queue(int32_t size)
{
    if (NULL == shared_memory_pointer_)
    {
        size_ = 0;
        head_ = 0;
        tail_ = 0;
        code_offset_ = invalid_offset;
        return;
    }

    //shm_mode_init mode
    if (shm_memory::shm_mode_init == shared_memory_pointer_->get_mode())
    {
        char* code_zone = (char*)shared_memory_pointer_->create_segment(size);
        if (NULL == code_zone)
        {
            return;
        }

        code_offset_ = int32_t(code_zone - (char*)this);
        size_ = size; //调用者设置的总长度
        head_ = 0;
        tail_ = 0;
    }
    else//resume
    {
        shared_memory_pointer_->create_segment(size);
    }
}

object_code_queue::~object_code_queue()
{
}

void* object_code_queue::operator new(size_t size)throw()
{
    if (NULL == shared_memory_pointer_)
    {
        return NULL;
    }
    
    //为object_code_queue对象自身分配空间
    return (void*)(shared_memory_pointer_->create_segment((int32_t)size));
}

void object_code_queue::operator delete(void *pbuffer)
{
}

int32_t object_code_queue::append(const char* code, int32_t size)
{
    if (invalid_offset == code_offset() || size_ <= 0)
    {
        return fail;
    }

    if (full())
    {
        return queue_buffer_is_not_enough;
    }

    int32_t head = 0;
    int32_t tail = 0;

    get_boundary(head, tail);
    if (head < 0 || head >= size_ || tail < 0 || tail >= size_)
    {
        //出错后，清除所有队列中的数据
        head = tail = 0;
        set_boundary(head, tail);
        return fail;
    }

    int32_t free_size = get_freesize();

    //每个消息code的最前面4Byte为消息code的长度(不包含这4个Byte)
    //pop()函数中也默认这一规则
    if ((int32_t)sizeof(int32_t) + size > free_size)
    {
        return queue_buffer_is_not_enough;
    }

    char* dest_pointer = get_codebuffer();
    char* source_pointer = (char*)&size;

    for (size_t i = 0; i < sizeof(int32_t); ++i)
    {
        dest_pointer[tail] = source_pointer[i];
        tail = (tail + 1)%size_;  //注意：保证m_iSize不能为0!!
    }

    if (head > tail)//
    {
        memcpy(&dest_pointer[tail], (const void*)code, (size_t)size);
    }
    else
    {
        if (size > (size_ - tail))
        {
            //需要分段拷贝
            memcpy(&dest_pointer[tail], (const void*)code, (size_t)(size_ - tail));
            memcpy(&dest_pointer[0], (const void*)(code + (size_-tail)), (size_t)(size - (size_ - tail)));
        }
        else
        {
            memcpy(&dest_pointer[tail], (const void*)code, (size_t)size);
        }
    }

    tail = (tail + size)%size_;

    //仅设置m_iTail
    set_boundary(-1, tail);

    return success;
}

int32_t object_code_queue::pop(char *dst, int32_t &outlength)
{
    if (NULL == dst || 0 >= outlength)
    {
        return fail;
    }
    
    if (invalid_offset == code_offset() || size_ <= 0)
    {
        return fail;
    }

    if (empty())
    {
        return queue_is_empty;
    }

    int32_t code_size = get_codesize();

    int32_t head = 0;
    int32_t tail = 0;

    get_boundary(head, tail);
    
    //如果code不足4个Byte,意味着队列损坏
    if (code_size < (int32_t)sizeof(int32_t))
    {
        //把队列清空
        outlength = 0;
        head = tail = 0;
        set_boundary(head, tail);
        return fail;
    }

    //预先保存输出buffer的大小
    int32_t buffer_size = outlength;

    //使用real_outlength保存实际code的长度
    int32_t real_outlength = 0;
    char* dest_pointer = (char*)&real_outlength;

    char* code_zone = get_codebuffer();
    if (NULL == code_zone)
    {
        return fail;
    }

    //首先取code的长度(也就是前4Byte)
    for (size_t i = 0; i < sizeof(int32_t); ++i)
    {
        dest_pointer[i] = code_zone[head];
        head = (head+1)%size_;
    }

    //如果从队列中读取的code长度不符合要求
    if (real_outlength <= 0 || real_outlength > (code_size - (int32_t)sizeof(int32_t)))
    {
        outlength = 0;
        head = tail = 0;
        set_boundary(head, tail);
        return fail;
    }

    //输出buffer不够
    if (buffer_size < real_outlength)
    {
        //added by aprilliu, 2008-09-16
        //跳过非法包.这样处理是否妥当? real_outlength四个字节的长度是connectorsvrd增加
        //应该是可信的
        head = (head+real_outlength)%size_;
        set_boundary(head, -1);//仅设置head指针
        return fail;
    }

    //copy code buffer
    outlength = real_outlength;
    dest_pointer = dst;

    if (tail > head)
    {
        memcpy(dest_pointer, (const void*)&code_zone[head], (size_t)outlength);
    }
    else
    {
        //如果当前code出现分段
        if (outlength > (size_ - head))
        {
            memcpy(dst, (const void*)&code_zone[head], (size_t)(size_ - head));
            memcpy((void*)&dst[size_-head], (const void*)&code_zone[0], size_t(outlength-(size_-head)));
        } 
        else
        {
            memcpy(dst, (const void*)&code_zone[head], size_t(outlength));
        }

    }//else

    //移动头指针
    head = (head+outlength)%size_;
    set_boundary(head, -1);//仅设置head指针

    return success;
}

bool object_code_queue::full()
{
    return!(get_freesize() > 0);
}

//设计保证m_iHead和m_iTail只有在队列为空的时候才能相等
bool object_code_queue::empty()
{
    return(head_ == tail_);
}

int32_t object_code_queue::code_offset() const
{
    return code_offset_;
}

int32_t object_code_queue::set_boundary(int32_t head, int32_t tail)
{
    if (head >= 0 && head < size_)
    {
        head_ = head;
    }
    
    if (tail >= 0 && tail < size_)
    {
        tail_ = tail;
    }
    
    return 0;
}

/**由于对4Byte的int32_t进行的读写操作都是原子性的，所以即使是在多线程环境下也不需要加/解锁操作*/
int32_t object_code_queue::get_boundary(int32_t &head, int32_t &tail)const
{
    head = head_;
    tail = tail_;
    return 0;
}

int32_t object_code_queue::get_freesize(void)const
{
    int32_t head = 0;
    int32_t tail = 0;

    get_boundary(head, tail);

    int32_t free_size = 0;

    if (head == tail)//空队列
    {
        free_size = size_;
    }
    else if (head > tail)
    {
        free_size = head - tail;
    } 
    else
    {
        free_size = head + (size_ - tail);
    }

    //去掉预留的空间
    free_size -= reserved_length;
    if (free_size < 0)
    {
        free_size = 0;
    }

    return free_size;
}

int32_t object_code_queue::get_codesize(void) const
{
    //由于get_freesize的返回值总是>=0(不会出现负值)，所以该函数这样的处理是正确的
    return (size_ - get_freesize() - reserved_length);
}

char* object_code_queue::get_codebuffer() const
{
    return((code_offset_ != invalid_offset)? (char*)((char*)this + code_offset_) : NULL);
}
