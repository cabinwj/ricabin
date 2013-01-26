#ifndef __COMMON_OBJECT_POOL_H__
#define __COMMON_OBJECT_POOL_H__

#include "hc_memory.h"

/**
  * ʵ��һ������ع����ࡣ
  * ������Ķ����Ǵ�һ��base_object�Ļ���̳����ģ�
  * �ڳ�ʼ�������ʱ�����������ڴ�������ĺ�����
  * ����һ����˵������ؾ���һ�����󹤳���
  * �ö���ؿ�����Ϊ����ADT�ĵײ�ʵ�֡�
  * ����صĺ���˼����ǰѶ������Ͷ����������ֿ���
  * �������Զ�����и�����ʽ����֯����ʵ�ָ����ӵ����ݽṹ��
  * �������������ά��һ�����ж�������һ���ѷ����������������˫�������
  */

enum 
{
    _memory_align = 4
};

enum
{
    object_createmode_init   = 0,                         /**< ʵ�ʴ������� */
    object_createmode_resume = 1,                     /**< �ָ�����*/
};

enum
{
    invalid_object_index     = -1,                            /**< ��Ч�Ķ�������ֵ */
    invalid_object_id        = invalid_object_index, /**< ��Ч�Ķ���ID��Ҳ������Ч�Ķ������� */
};

enum
{
    max_object_key_length    = 512,                    /**< ����key����󳤶�*/
};

class shm_memory;

class base_object;

typedef base_object* (*allocate_object_memory)(void*);

//! IMPLEMENT_SHARE_ALLOCATOR
//! �����������������ص�new, delete
#define DECLARE_SHARE_ALLOCATOR \
private:                                                    \
    static base_object* allot_memory_for_object(void* memory_pointer);   \
\
private:    \
    static object_pool* object_pool_; \
\
public: \
    static object_pool* allocate_pool();   \
\
public: \
    void* operator new(size_t size);    \
    void operator delete(void *p, size_t size);


//! IMPLEMENT_SHARE_ALLOCATOR
//! ����������������ص�new, delete
#define IMPLEMENT_SHARE_ALLOCATOR(T, COUNT)   \
base_object* T::allot_memory_for_object(void* memory_pointer) \
{   \
    return (base_object*)((::new((char*)memory_pointer) T));  \
}   \
\
object_pool* T::object_pool_ = NULL;  \
\
object_pool* T::allocate_pool()    \
{   \
    if (NULL == object_pool_)   \
    {   \
        object_pool_ = new object_pool(sizeof(T), COUNT, allot_memory_for_object); \
    }   \
    return object_pool_; \
}\
\
void* T::operator new(size_t size)  \
{   \
    return (void*)allocate_pool()->acquire();  \
}   \
void T::operator delete(void* p, size_t size)   \
{   \
    return allocate_pool()->release(void* p);  \
}

class index_node
{
public:
    enum
    {
        max_additional_data_size = 8,         /**< ��󸽼��ֶγ��� */
    };
    
    enum
    {
        index_node_free        = 0,               /**< δʹ�ñ�ʶ*/
        index_node_used        = 1                /**< ��ʹ�ñ�ʶ*/
    };

public:
    index_node() : prev_(invalid_object_index), next_(invalid_object_index)
    , flag_(index_node_free), object_pointer_(NULL)
    { clear_additional_data(); }

    ~index_node() { }

public:
    inline void init()
    {   prev_ = invalid_object_index;
        next_ = invalid_object_index;
        flag_ = index_node_free;
        object_pointer_ = NULL;
        clear_additional_data(); }

    inline void set_free() { flag_ = index_node_free; }
    inline void set_used() { flag_ = index_node_used; }
    inline int32_t is_used()const { return flag_; }

    inline void set_next(int32_t next) { next_ = next; }
    inline void set_prev(int32_t prev) { prev_ = prev; }
    inline int32_t get_next(void) { return next_; }
    inline int32_t get_prev(void) { return prev_; }

    inline bool is_valid_object_index(int32_t object_index)const
    {  return ((object_index >= 0) && (object_index < max_additional_data_size)); }

    inline bool get_additional_data_by_index(int32_t index, int32_t& value)
    {  if (!is_valid_object_index(index)) { return false; }
        value = addition_data_[index];  return true; }

    inline bool set_additional_data_by_index(int32_t index, int32_t value)
    {  if (!is_valid_object_index(index)) { return false; }
        addition_data_[index] = value;  return true; }

    inline void clear_additional_data(void)
    { for (size_t i = 0; i < sizeof(addition_data_)/sizeof(addition_data_[0]); ++i)
        { addition_data_[i] = invalid_object_index;  } }

public:
    inline void set_object(base_object* object) { object_pointer_ = object; }
    inline base_object* get_object(void)const { return object_pointer_; }

private:
    int32_t prev_;
    int32_t next_;
    int32_t flag_;
    int32_t addition_data_[max_additional_data_size];

    base_object* object_pointer_;  //���index������T����
};


/*
  * ����ء����ڹ���ָ������(�����ͱ�����base_object������)��ָ�������Ķ���
  */
class object_pool
{
public:
    object_pool() { }
    object_pool(size_t object_size, int32_t object_count,
                      allocate_object_memory allot_method);
    ~object_pool() { }

public:
    /*
    * @method:  create_object �ӿ��ж�������ͷ������һ�����󣬲������ѷ����������ͷ��
    * @return:   int32_t ���䵽�Ķ��������ֵ��invalid_object_index/invalid_object_id��ʾ����ʧ��    
    */
    int32_t create_object();

    /*
    * @method:  destroy_object ����ָ���Ķ���(��û�������ͻ����ڴ���Դ)����������ж�����ͷ��
    * @param:    int32_t object_index ���յĶ��������/ID
    * @return:   int32_t ���ػ��յĶ����������invalid_object_index��ʾ����ʧ��
    */
    int32_t destroy_object(int32_t object_index);

public:
    //���ѷ�������л�ȡ��ע�⣺�ú����ж��˶�Ӧ��index_node�Ƿ����ѷ�����
    index_node* get_index(int32_t object_index);
    //���ѷ�������л�ȡ��ע�⣺�ú����ж��˶�Ӧ��index_node�Ƿ����ѷ�����
    base_object* get_object(int32_t object_index);
    
    int32_t get_object_id(base_object* object);
    int32_t get_next_object_id(int32_t object_id);
    int32_t get_next_object_id(base_object* object);

    //��ʽ������������
    int32_t format_index();
    //��ʽ��������
    int32_t format_object();
    //��ʼ�������
    void init(size_t object_size, int32_t object_count,
                allocate_object_memory allot_method);

public:
    void* acquire(size_t size);
    void release(void* p, size_t size);

public:
    void* operator new(size_t size) throw();
    void operator delete(void* object);

public:
    //�õ�����ص���������Ŀ
    inline int32_t get_object_count(void)const    { return object_count_; }     
    inline int32_t get_used_count(void)const { return used_count_; }      
    inline int32_t get_free_count(void)const { return (object_count_ - used_count_); }  
    inline int32_t get_free_head(void)const { return free_head_; }                       
    inline int32_t get_used_head(void)const { return used_head_; }    

    //�ж϶�����Ƿ�Ϊ��(û�з����κζ���)
    inline bool empty(void)const { return (0 == used_count_); }   
    //�ж϶�����������������������ж���
    inline bool full(void)const { return(used_count_ == object_count_); }     

protected:
    /*
    * @method: allocate_one_object ���������������ѷ���Ķ���������ӿ��ж��������ѷ�����е�ͷ��
    * @return: int32_t ���䵽�Ķ��������ֵ��invalid_object_index��ʾ����ʧ��
    */
    int32_t allocate_one_object(void);

protected:
    inline bool is_valid_object_index(int32_t object_index) const
            { return ((object_index >= 0) && (object_index < object_count_)); }

public:
    static shm_memory* shm_memory_pointer_; 

public:
    /*
    * @method: buffer_size ����������ռ�õ��ڴ��С
    * @return: int32_t ���ض������ռ�õ��ڴ��С����λ:Byte
    */
    static size_t buffer_size(size_t object_size, int32_t object_count);
    /*
    * @method: set_memory(�����ڹ����ڴ��ǰ����) ����һ��shm_memory*��buffer��������
    */
    static void set_memory(shm_memory* memory_pointer);

private:
    size_t object_size_;           //�����С
    int32_t object_count_;       //��������������Ŀ
    int32_t free_head_;          //δ�������������ʼ���������ֵ
    int32_t used_head_;          //�ѷ������������ʼ���������ֵ
    int32_t used_count_;         //�ѷ������ĸ���

private:
    index_node* index_link_;      //���������� 
    base_object* object_link_;     //������
    allocate_object_memory allot_method_; //���󴴽�����
};


/*
  * ����ʵ�����������
  */
class base_object
{
public:
    base_object() { object_id_ = invalid_object_id; }
    virtual ~base_object() { }

public:
    virtual int32_t init(void) = 0;
    virtual int32_t load(void) = 0;
    virtual int32_t dump(int32_t handle) = 0;

public:
    /*
    * @method:    get_key
    * @param: void * key
    * @param: int32_t & keylength  ����void* key����󳤶ȣ�����void* key��ʵ�ʳ��ȡ���λ:Byte
    */
    virtual int32_t get_key(void* key, int32_t& keylength) = 0;  
    virtual int32_t set_key(const void* key, int32_t keylength) = 0;

public:
    virtual int32_t get_object_id(void) { return object_id_; }

public:
    static int32_t create_mode_;

private:
    int32_t object_id_;
    //����object_pool::format_object()��Ҫֱ�Ӳ���object_id_
    friend int32_t object_pool::format_object();
};

#endif
