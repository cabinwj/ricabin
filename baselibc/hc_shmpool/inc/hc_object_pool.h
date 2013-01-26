#ifndef __COMMON_OBJECT_POOL_H__
#define __COMMON_OBJECT_POOL_H__

#include "hc_memory.h"

/**
  * 实现一个对象池管理类。
  * 所管理的对象都是从一个base_object的基类继承来的，
  * 在初始化对象池时可以设置用于创建对象的函数，
  * 从这一点来说，对象池就是一个对象工厂。
  * 该对象池可以作为其他ADT的底层实现。
  * 对象池的核心思想就是把对象区和对象索引区分开，
  * 由索引对对象进行各种形式的组织，以实现更复杂的数据结构。
  * 对象池利用索引维护一个空闲对象链和一个已分配对象链，并且是双向的链。
  */

enum 
{
    _memory_align = 4
};

enum
{
    object_createmode_init   = 0,                         /**< 实际创建对象 */
    object_createmode_resume = 1,                     /**< 恢复对象*/
};

enum
{
    invalid_object_index     = -1,                            /**< 无效的对象索引值 */
    invalid_object_id        = invalid_object_index, /**< 无效的对象ID，也就是无效的对象索引 */
};

enum
{
    max_object_key_length    = 512,                    /**< 对象key的最大长度*/
};

class shm_memory;

class base_object;

typedef base_object* (*allocate_object_memory)(void*);

//! IMPLEMENT_SHARE_ALLOCATOR
//! 声明池配置器和重载的new, delete
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
//! 定义池配置器和重载的new, delete
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
        max_additional_data_size = 8,         /**< 最大附加字段长度 */
    };
    
    enum
    {
        index_node_free        = 0,               /**< 未使用标识*/
        index_node_used        = 1                /**< 已使用标识*/
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

    base_object* object_pointer_;  //与该index关联的T对象
};


/*
  * 对象池。用于管理指定类型(该类型必须是base_object的子类)，指定数量的对象
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
    * @method:  create_object 从空闲对象链的头部分配一个对象，并放入已分配对象链的头部
    * @return:   int32_t 分配到的对象的索引值。invalid_object_index/invalid_object_id表示分配失败    
    */
    int32_t create_object();

    /*
    * @method:  destroy_object 回收指定的对象(并没有析构和回收内存资源)，并放入空闲对象链头部
    * @param:    int32_t object_index 回收的对象的索引/ID
    * @return:   int32_t 返回回收的对象的索引。invalid_object_index表示回收失败
    */
    int32_t destroy_object(int32_t object_index);

public:
    //在已分配对象中获取。注意：该函数判断了对应的index_node是否是已分配了
    index_node* get_index(int32_t object_index);
    //在已分配对象中获取。注意：该函数判断了对应的index_node是否是已分配了
    base_object* get_object(int32_t object_index);
    
    int32_t get_object_id(base_object* object);
    int32_t get_next_object_id(int32_t object_id);
    int32_t get_next_object_id(base_object* object);

    //格式化对象索引区
    int32_t format_index();
    //格式化对象区
    int32_t format_object();
    //初始化对象池
    void init(size_t object_size, int32_t object_count,
                allocate_object_memory allot_method);

public:
    void* acquire(size_t size);
    void release(void* p, size_t size);

public:
    void* operator new(size_t size) throw();
    void operator delete(void* object);

public:
    //得到对象池的最大对象数目
    inline int32_t get_object_count(void)const    { return object_count_; }     
    inline int32_t get_used_count(void)const { return used_count_; }      
    inline int32_t get_free_count(void)const { return (object_count_ - used_count_); }  
    inline int32_t get_free_head(void)const { return free_head_; }                       
    inline int32_t get_used_head(void)const { return used_head_; }    

    //判断对象池是否为空(没有分配任何对象)
    inline bool empty(void)const { return (0 == used_count_); }   
    //判断对象池是已满（分配完了所有对象）
    inline bool full(void)const { return(used_count_ == object_count_); }     

protected:
    /*
    * @method: allocate_one_object 操作对象索引，把分配的对象的索引从空闲队列移入已分配队列的头部
    * @return: int32_t 分配到的对象的索引值。invalid_object_index表示分配失败
    */
    int32_t allocate_one_object(void);

protected:
    inline bool is_valid_object_index(int32_t object_index) const
            { return ((object_index >= 0) && (object_index < object_count_)); }

public:
    static shm_memory* shm_memory_pointer_; 

public:
    /*
    * @method: buffer_size 计算对象池所占用的内存大小
    * @return: int32_t 返回对象池所占用的内存大小，单位:Byte
    */
    static size_t buffer_size(size_t object_size, int32_t object_count);
    /*
    * @method: set_memory(必须在构造内存池前调用) 给定一个shm_memory*的buffer用来构造
    */
    static void set_memory(shm_memory* memory_pointer);

private:
    size_t object_size_;           //对象大小
    int32_t object_count_;       //对象区最大对象数目
    int32_t free_head_;          //未分配对象链的起始对象的索引值
    int32_t used_head_;          //已分配对象链的起始对象的索引值
    int32_t used_count_;         //已分配对象的个数

private:
    index_node* index_link_;      //对象索引区 
    base_object* object_link_;     //对象区
    allocate_object_memory allot_method_; //对象创建方法
};


/*
  * 所有实体对象的虚基类
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
    * @param: int32_t & keylength  传入void* key的最大长度，传出void* key的实际长度。单位:Byte
    */
    virtual int32_t get_key(void* key, int32_t& keylength) = 0;  
    virtual int32_t set_key(const void* key, int32_t keylength) = 0;

public:
    virtual int32_t get_object_id(void) { return object_id_; }

public:
    static int32_t create_mode_;

private:
    int32_t object_id_;
    //由于object_pool::format_object()需要直接操作object_id_
    friend int32_t object_pool::format_object();
};

#endif
