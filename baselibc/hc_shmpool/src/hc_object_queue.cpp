#include "hc_object_queue.h"

#include <stdio.h>

object_queue::object_queue(object_pool* pool, int32_t ad_data_index)
{
    if (NULL == pool)
    {
        return;
    }

    pool_ = pool;

    if (object_createmode_init == base_object::create_mode_)
    {
        if (ad_data_index < 0 || ad_data_index >= index_node::max_additional_data_size)
        {
            return;
        }

        pool_ = pool;
        ad_data_index_ = ad_data_index;

        head_ = invalid_object_index;
        tail_ = invalid_object_index;
    }
}

int32_t object_queue::next(int32_t object) const
{
    index_node* index = pool_->get_index(object);
    if (NULL == index)
    {
        return invalid_object_index;
    }
    int32_t value = invalid_object_index;
    if (0 != index->get_additional_data_by_index(ad_data_index_, value))
    {
        return invalid_object_index;
    }

    return value;
}

int32_t object_queue::set_next(int32_t object, int32_t next)
{
    index_node* index = pool_->get_index(object);
    if (NULL == index)
    {
        return -1;
    }    

    return index->set_additional_data_by_index(ad_data_index_, next);
}

int32_t object_queue::append(int32_t object)
{
    index_node* index = pool_->get_index(object);
    if (NULL == index)
    {
        return -1;
    }

    set_next(object, invalid_object_index);

    //若队列为空
    if (invalid_object_index == tail_)
    {
        head_ = object;
        tail_ = object;

        return 0;
    }

    set_next(tail_, object);
    tail_ = object;

    return 0;
}

int32_t object_queue::push(int32_t object)
{
    index_node* index = pool_->get_index(object);
    if (NULL == index)
    {
        return -1;
    }

    set_next(object, head_);

    //若队列为空
    if (invalid_object_index == head_)
    {
        tail_ = object;
    }

    head_ = object;

    return 0;
}

int32_t object_queue::insert(int32_t prev, int32_t object)
{
    index_node* current_index = pool_->get_index(object);
    if (NULL == current_index)
    {
        return -1;
    }

    index_node* prev_index = pool_->get_index(prev);
    if (NULL == prev_index)
    {
        return -1;
    }

    int32_t prev_next = next(prev);
    set_next(object, prev_next);
    set_next(prev, object);

    return 0;
}

int32_t object_queue::pop()
{
    int32_t old_head = head_;

    if (invalid_object_index == head_)
    {
        return invalid_object_id;
    }

    if (0 != erase(head_))
    {
        old_head = invalid_object_id;
    }

    return old_head;
}

int32_t object_queue::erase(int32_t object)
{
    index_node* index = pool_->get_index(object);
    if (NULL == index)
    {
        return -1;
    }

    if (object == head_)
    {
        head_ = next(head_);
        set_next(object, invalid_object_index);//清除该对象索引用于形成该队列所用到的additional信息
        if (invalid_object_index == head_) //若队列为空了
        {
            tail_ = head_;
        }

        return 0;
    }

    //如果删除的不是头元素，由于该队列是单向队列，必须要从队列首部遍历，直到找到待删除对象object的前驱对象
    int32_t prev = invalid_object_index;
    for (prev = head_; prev != invalid_object_index; prev = next(prev))
    {
        if (next(prev) == object)//是否为object的前驱对象
        {
            break;
        }
    }

    if (invalid_object_index == prev) //没有找到前驱对象
    {
        return -1;
    }

    //delete the object
    set_next(prev, next(object));
    set_next(object, invalid_object_index);//清除该对象索引用于形成该队列所用到的additional信息

    if (object == tail_)//若待删除的对象是队列的尾对象，则要更新尾对象指针
    {
        tail_ = prev;
    }

    return 0;
}

void object_queue::dump(const char* filename)
{
    if (NULL == filename)
    {
        return;
    }

#ifndef WIN32
    FILE* fd = fopen(filename, "a+");
    if (NULL == fd)
    {
        return;
    }

    int32_t index = head();
    int32_t loops = 0;
    while (index != invalid_object_id)
    {
        if (!(loops%16))
        {
            fprintf(fd, "\n%06d>>\t", loops);
        }

        fprintf(fd, "%06d  ", index);
        index = next(index);
        ++loops;
    }

    fclose(fd);
#endif
}
