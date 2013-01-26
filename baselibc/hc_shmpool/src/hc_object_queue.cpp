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

    //������Ϊ��
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

    //������Ϊ��
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
        set_next(object, invalid_object_index);//����ö������������γɸö������õ���additional��Ϣ
        if (invalid_object_index == head_) //������Ϊ����
        {
            tail_ = head_;
        }

        return 0;
    }

    //���ɾ���Ĳ���ͷԪ�أ����ڸö����ǵ�����У�����Ҫ�Ӷ����ײ�������ֱ���ҵ���ɾ������object��ǰ������
    int32_t prev = invalid_object_index;
    for (prev = head_; prev != invalid_object_index; prev = next(prev))
    {
        if (next(prev) == object)//�Ƿ�Ϊobject��ǰ������
        {
            break;
        }
    }

    if (invalid_object_index == prev) //û���ҵ�ǰ������
    {
        return -1;
    }

    //delete the object
    set_next(prev, next(object));
    set_next(object, invalid_object_index);//����ö������������γɸö������õ���additional��Ϣ

    if (object == tail_)//����ɾ���Ķ����Ƕ��е�β������Ҫ����β����ָ��
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
