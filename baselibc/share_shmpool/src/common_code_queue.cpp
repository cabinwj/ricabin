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
        size_ = size; //���������õ��ܳ���
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
    
    //Ϊobject_code_queue�����������ռ�
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
        //�����������ж����е�����
        head = tail = 0;
        set_boundary(head, tail);
        return fail;
    }

    int32_t free_size = get_freesize();

    //ÿ����Ϣcode����ǰ��4ByteΪ��Ϣcode�ĳ���(��������4��Byte)
    //pop()������ҲĬ����һ����
    if ((int32_t)sizeof(int32_t) + size > free_size)
    {
        return queue_buffer_is_not_enough;
    }

    char* dest_pointer = get_codebuffer();
    char* source_pointer = (char*)&size;

    for (size_t i = 0; i < sizeof(int32_t); ++i)
    {
        dest_pointer[tail] = source_pointer[i];
        tail = (tail + 1)%size_;  //ע�⣺��֤m_iSize����Ϊ0!!
    }

    if (head > tail)//
    {
        memcpy(&dest_pointer[tail], (const void*)code, (size_t)size);
    }
    else
    {
        if (size > (size_ - tail))
        {
            //��Ҫ�ֶο���
            memcpy(&dest_pointer[tail], (const void*)code, (size_t)(size_ - tail));
            memcpy(&dest_pointer[0], (const void*)(code + (size_-tail)), (size_t)(size - (size_ - tail)));
        }
        else
        {
            memcpy(&dest_pointer[tail], (const void*)code, (size_t)size);
        }
    }

    tail = (tail + size)%size_;

    //������m_iTail
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
    
    //���code����4��Byte,��ζ�Ŷ�����
    if (code_size < (int32_t)sizeof(int32_t))
    {
        //�Ѷ������
        outlength = 0;
        head = tail = 0;
        set_boundary(head, tail);
        return fail;
    }

    //Ԥ�ȱ������buffer�Ĵ�С
    int32_t buffer_size = outlength;

    //ʹ��real_outlength����ʵ��code�ĳ���
    int32_t real_outlength = 0;
    char* dest_pointer = (char*)&real_outlength;

    char* code_zone = get_codebuffer();
    if (NULL == code_zone)
    {
        return fail;
    }

    //����ȡcode�ĳ���(Ҳ����ǰ4Byte)
    for (size_t i = 0; i < sizeof(int32_t); ++i)
    {
        dest_pointer[i] = code_zone[head];
        head = (head+1)%size_;
    }

    //����Ӷ����ж�ȡ��code���Ȳ�����Ҫ��
    if (real_outlength <= 0 || real_outlength > (code_size - (int32_t)sizeof(int32_t)))
    {
        outlength = 0;
        head = tail = 0;
        set_boundary(head, tail);
        return fail;
    }

    //���buffer����
    if (buffer_size < real_outlength)
    {
        //added by aprilliu, 2008-09-16
        //�����Ƿ���.���������Ƿ��׵�? real_outlength�ĸ��ֽڵĳ�����connectorsvrd����
        //Ӧ���ǿ��ŵ�
        head = (head+real_outlength)%size_;
        set_boundary(head, -1);//������headָ��
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
        //�����ǰcode���ֶַ�
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

    //�ƶ�ͷָ��
    head = (head+outlength)%size_;
    set_boundary(head, -1);//������headָ��

    return success;
}

bool object_code_queue::full()
{
    return!(get_freesize() > 0);
}

//��Ʊ�֤m_iHead��m_iTailֻ���ڶ���Ϊ�յ�ʱ��������
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

/**���ڶ�4Byte��int32_t���еĶ�д��������ԭ���Եģ����Լ�ʹ���ڶ��̻߳�����Ҳ����Ҫ��/��������*/
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

    if (head == tail)//�ն���
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

    //ȥ��Ԥ���Ŀռ�
    free_size -= reserved_length;
    if (free_size < 0)
    {
        free_size = 0;
    }

    return free_size;
}

int32_t object_code_queue::get_codesize(void) const
{
    //����get_freesize�ķ���ֵ����>=0(������ָ�ֵ)�����Ըú��������Ĵ�������ȷ��
    return (size_ - get_freesize() - reserved_length);
}

char* object_code_queue::get_codebuffer() const
{
    return((code_offset_ != invalid_offset)? (char*)((char*)this + code_offset_) : NULL);
}
