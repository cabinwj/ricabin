#ifndef __COMMON_CODEQUEUE_H__
#define __COMMON_CODEQUEUE_H__

/*
 * @defgroup common_codequeue
 * @purpose 字节流形式的队列，每个消息code保存到队列中时，队列会预留4个Byte保存消息code的长度（不包括这4个Byte本身）
 */

#include "hc_base.h"

class shm_memory;

class object_code_queue
{
public:
    enum
    {
        invalid_offset = -1, /**< 无效的偏移量 */
        reserved_length = 8, /**< 预留的空间长度*/
    };

public:
    /** 队列所用的共享内存区 */
    static shm_memory* shared_memory_pointer_;
    /** 设置队列所在的共享内存区 */
    static void set_sharedmemory(shm_memory* shm_memory_pointer);
    /** 得到队列所占用的内存大小*/
    static size_t size(int32_t buffersize);

protected:
    //数据区总长度，单位: Byte。
    int32_t size_;
    //可用数据起始位置
    int32_t head_;
    //可用数据结束位置
    int32_t tail_;
    //数据区的偏移位置
    int32_t code_offset_;

public:
    object_code_queue(int32_t size);
    ~object_code_queue();

public:
    void* operator new( size_t size ) throw();
    void operator delete( void *buffer);

public:
    /*
    * @method:    append 从尾部追加消息buffer。该函数只是改变tail_，即使是在多线程环境下也不需要加/解锁操作，
    *                因为，对于四字节的int32_t的读写操作是原子操作
    */
    int32_t append(const char* code, int32_t size);

    /*
    * @method:    pop 从队列头部取一条消息
    * @param[in,out]: short & outlength。传入dst的长度，传出实际消息code的长度
    * - 0: 成功. outlength返回实际code的长度
    * - >0 : 失败， dst,outlength的值无意义
    */
    int32_t pop(char* dst, int32_t& outlength);

    /*
    * @method:    full 判断队列是否满
    */
    bool full(void);

    /*
    * @method:    empty 判读队列是否为空
    */
    bool empty(void);

protected:
    /** 返回数据区偏移 */
    int32_t code_offset(void)const;
    int32_t set_boundary(int32_t head, int32_t tail);
    int32_t get_boundary(int32_t& head, int32_t& tail)const;
    char* get_codebuffer(void)const;

public:
    /*
    * - 返回可用空间的大小。如果没有可用空间，返回值为0。也就是说该函数的
    *    返回值总是>=0的。
    * @note    保证队列buffer的长度不能为0,否则结果为定义.
    */
    int32_t get_freesize(void)const;
    int32_t get_codesize(void)const;

private:
    object_code_queue(const object_code_queue& init);
    object_code_queue& operator =(const object_code_queue&);
};

#endif /*__COMMON_CODEQUEUE_H__*/
