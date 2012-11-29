#include "common_object_pool.h"

int32_t base_object::create_mode_ = object_createmode_init;

shm_memory* object_pool::shm_memory_pointer_ = NULL;    

size_t object_pool::buffer_size(size_t object_size, int32_t object_count)
{
    size_t buffer_size = sizeof(object_pool);
    size_t real_count = ((object_count != 0) ? object_count : 512);
    buffer_size += real_count * sizeof(index_node);
    buffer_size += real_count * ((object_size + (_memory_align - 1)) & ~(_memory_align - 1));

    return buffer_size;
}

void object_pool::set_memory(shm_memory* memory_pointer)
{
    shm_memory_pointer_ = memory_pointer;
}

object_pool::object_pool(size_t object_size, int32_t object_count, allocate_object_memory allot_method)
{
    init(object_size, object_count, allot_method);
}

void object_pool::init(size_t object_size, int32_t object_count, allocate_object_memory allot_method)
{
    if (NULL == shm_memory_pointer_)
    {
        return;
    }
    
    if (NULL == allot_method)
    {
        return;
    }

    if (shm_memory::shm_mode_init == shm_memory_pointer_->get_mode())
    {    
        object_size_ =  ((object_size + (_memory_align - 1)) & ~(_memory_align - 1));
        object_count_ = ((object_count != 0) ? object_count : 512);
    }

    index_link_ = (index_node*)(shm_memory_pointer_->create_segment( sizeof(index_node) * object_count_));
    if (NULL == index_link_)
    {
        return;
    }

    object_link_ = (base_object*)(shm_memory_pointer_->create_segment((int32_t)object_size_ * object_count_));
    if (NULL == object_link_)
    {
        return;
    }
    
    allot_method_ = allot_method;

    if (shm_memory::shm_mode_init == shm_memory_pointer_->get_mode())
    {
        format_index();
        base_object::create_mode_ = object_createmode_init;
    }
    else
    {
        base_object::create_mode_ = object_createmode_resume;
    }

    //格式化对象池
    format_object();

    return;
}

int32_t object_pool::format_index()
{
    used_count_ = 0;
    free_head_ = 0;
    used_head_ = invalid_object_index;

    //单独处理第0个索引
    index_link_[free_head_].init();
    index_link_[free_head_].set_prev(invalid_object_index);

    for (int32_t i = 1; i < object_count_; ++i)
    {
        index_link_[i].init();
        index_link_[i-1].set_next(i);
        index_link_[i].set_prev(i-1);
    }

    //最后一个索引的下一个是object_index[free_head_]
    index_link_[object_count_-1].set_next(invalid_object_index);

    return 0;
}

int32_t object_pool::format_object()
{
    if (NULL == allot_method_)
    {
        return -1;
    }

    base_object* object_pointer = NULL;
    for (int32_t idx = 0; idx < object_count_; ++idx)
    {
        object_pointer = (*allot_method_)((char*)object_link_ + object_size_*idx);  //replace new 
        object_pointer->object_id_ = idx;
        index_link_[idx].set_object(object_pointer);
    }

    return 0;
}

int32_t object_pool::create_object()
{
    int32_t allocated_index = allocate_one_object();
    if (invalid_object_index == allocated_index)
    {
        return allocated_index;
    }

    base_object* allocated_object_pointer = get_object(allocated_index);
    if (NULL == allocated_object_pointer)
    {
        return invalid_object_index;
    }

    allocated_object_pointer->init();

    return allocated_index;
}

int32_t object_pool::destroy_object(int32_t object_index)
{
    if (!is_valid_object_index(object_index))
    {
        return invalid_object_index;
    }

    if (!index_link_[object_index].is_used())
    {
        return invalid_object_index;
    }

    //如果待回收的对象是已分配对象链的首对象
    if (object_index == used_head_)
    {
        used_head_ = index_link_[used_head_].get_next();
    }

    //待回收对象有后续对象
    if (index_link_[object_index].get_next() >= 0)
    {
        index_link_[index_link_[object_index].get_next()].set_prev(index_link_[object_index].get_prev());
    }

    //待回收对象有前置对象
    if (index_link_[object_index].get_prev() >= 0)
    {
        index_link_[index_link_[object_index].get_prev()].set_next(index_link_[object_index].get_next());
    }

    //释放并插入空闲对象链首部
    index_link_[object_index].set_free();
    index_link_[object_index].set_prev(invalid_object_index);
    index_link_[object_index].set_next(free_head_);
    if (free_head_ >= 0)
    {
        index_link_[free_head_].set_prev(object_index);
    }

    free_head_ = object_index;
    used_count_--;

    return object_index;
}

//注意：该函数判断了对应的index_node是否是已分配了
index_node* object_pool::get_index(int32_t object_index)
{
    if (!is_valid_object_index(object_index))
    {
        return NULL;
    }

    if (!index_link_[object_index].is_used())
    {
        return NULL;
    }

    return &index_link_[object_index];
}

//注意：该函数判断了对应的index_node是否是已分配了
base_object* object_pool::get_object(int32_t object_index)
{
    return (base_object*)(get_index(object_index)->get_object());
}

void* object_pool::operator new(size_t size)throw()
{
    if (NULL == shm_memory_pointer_)
    {
        return NULL;
    }

    return shm_memory_pointer_->create_segment((int32_t)size);
}

void object_pool::operator delete(void* object)
{
}

int32_t object_pool::get_object_id(base_object* object)
{
    if (NULL == object)
    {
        return invalid_object_id;
    }

    if ((char*)object < (char*)object_link_)
    {
        return invalid_object_id;
    }

    if ((char*)object >= (char*)((char*)object_link_ + object_size_ * object_count_))
    {
        return invalid_object_id;
    }

    return(int32_t)(((char*)object -(char*)object_link_)/object_size_);
}

int32_t object_pool::get_next_object_id(int32_t object_id)
{
    if (0 > object_id)
    {
        return invalid_object_id;
    }

    return index_link_[object_id].get_next(); 
}

int32_t object_pool::get_next_object_id(base_object* object)
{
    if (NULL == object)
    {
        return invalid_object_id;
    }

    int32_t object_id = get_object_id(object);
    if (invalid_object_id == object_id)
    {
        return invalid_object_id;
    }

    return get_next_object_id(object_id);
}

void* object_pool::acquire(size_t size)
{
    int32_t index = create_object();
    if (invalid_object_index == index)
    {
        return NULL;
    }

    return (void*)get_object(index);
}

void object_pool::release(void* object, size_t size)
{
       int32_t index = get_object_id((base_object*)object);
   if (invalid_object_index == index)
    {
        return;
    }

    destroy_object(index);    
}

int32_t object_pool::allocate_one_object( void )
{
    int32_t allocated_index = invalid_object_index;
    if (full())
    {
        return allocated_index;
    }

    allocated_index = free_head_;
    free_head_ = index_link_[free_head_].get_next();
    //设置空闲链的起始对象的前置对象为无效
    if (free_head_ >= 0)
    {
        index_link_[free_head_].set_prev(invalid_object_index);
    }

    //分配对象，并放入已分配对象队列的头部
    index_link_[allocated_index].set_used();
    index_link_[allocated_index].set_next(used_head_);
    index_link_[allocated_index].set_prev(invalid_object_index);

    if (used_head_ >= 0)
    {
        index_link_[used_head_].set_prev(allocated_index);
    }

    used_head_ = allocated_index;
    index_link_[allocated_index].clear_additional_data();
    used_count_++;

    return allocated_index;
}
