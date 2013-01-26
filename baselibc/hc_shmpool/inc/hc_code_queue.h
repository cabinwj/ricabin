#ifndef __COMMON_CODEQUEUE_H__
#define __COMMON_CODEQUEUE_H__

/*
 * @defgroup common_codequeue
 * @purpose �ֽ�����ʽ�Ķ��У�ÿ����Ϣcode���浽������ʱ�����л�Ԥ��4��Byte������Ϣcode�ĳ��ȣ���������4��Byte����
 */

#include "hc_base.h"

class shm_memory;

class object_code_queue
{
public:
    enum
    {
        invalid_offset = -1, /**< ��Ч��ƫ���� */
        reserved_length = 8, /**< Ԥ���Ŀռ䳤��*/
    };

public:
    /** �������õĹ����ڴ��� */
    static shm_memory* shared_memory_pointer_;
    /** ���ö������ڵĹ����ڴ��� */
    static void set_sharedmemory(shm_memory* shm_memory_pointer);
    /** �õ�������ռ�õ��ڴ��С*/
    static size_t size(int32_t buffersize);

protected:
    //�������ܳ��ȣ���λ: Byte��
    int32_t size_;
    //����������ʼλ��
    int32_t head_;
    //�������ݽ���λ��
    int32_t tail_;
    //��������ƫ��λ��
    int32_t code_offset_;

public:
    object_code_queue(int32_t size);
    ~object_code_queue();

public:
    void* operator new( size_t size ) throw();
    void operator delete( void *buffer);

public:
    /*
    * @method:    append ��β��׷����Ϣbuffer���ú���ֻ�Ǹı�tail_����ʹ���ڶ��̻߳�����Ҳ����Ҫ��/����������
    *                ��Ϊ���������ֽڵ�int32_t�Ķ�д������ԭ�Ӳ���
    */
    int32_t append(const char* code, int32_t size);

    /*
    * @method:    pop �Ӷ���ͷ��ȡһ����Ϣ
    * @param[in,out]: short & outlength������dst�ĳ��ȣ�����ʵ����Ϣcode�ĳ���
    * - 0: �ɹ�. outlength����ʵ��code�ĳ���
    * - >0 : ʧ�ܣ� dst,outlength��ֵ������
    */
    int32_t pop(char* dst, int32_t& outlength);

    /*
    * @method:    full �ж϶����Ƿ���
    */
    bool full(void);

    /*
    * @method:    empty �ж������Ƿ�Ϊ��
    */
    bool empty(void);

protected:
    /** ����������ƫ�� */
    int32_t code_offset(void)const;
    int32_t set_boundary(int32_t head, int32_t tail);
    int32_t get_boundary(int32_t& head, int32_t& tail)const;
    char* get_codebuffer(void)const;

public:
    /*
    * - ���ؿ��ÿռ�Ĵ�С�����û�п��ÿռ䣬����ֵΪ0��Ҳ����˵�ú�����
    *    ����ֵ����>=0�ġ�
    * @note    ��֤����buffer�ĳ��Ȳ���Ϊ0,������Ϊ����.
    */
    int32_t get_freesize(void)const;
    int32_t get_codesize(void)const;

private:
    object_code_queue(const object_code_queue& init);
    object_code_queue& operator =(const object_code_queue&);
};

#endif /*__COMMON_CODEQUEUE_H__*/
