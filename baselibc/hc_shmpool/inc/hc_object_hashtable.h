#ifndef __COMMON_OBJECT_HASHTABLE_H__
#define __COMMON_OBJECT_HASHTABLE_H__

#include "hc_base.h"
#include "hc_object_pool.h"

#include <string.h>

/*
 * @purpose 实现一个hash_table，底层由object_pool来实现。特别注意：SIZE不能为0。
 * base_object的key 如果是字符串类型，keylength为strlen()的返回值。
 */

// bucket of hash table
struct hash_bucket
{
    int32_t head_;     //该slot/bucket冲突链的首元素(其实是对象的索引值)
};

template<int32_t SIZE>
class hash_table
{
public:
    enum
    {
        max_hash_bucket_size = SIZE,
        max_hash_key_length = max_object_key_length
    };

public:
    hash_table() {}
    hash_table(object_pool* pool, int32_t next);
    virtual ~hash_table() {}

public:
    /*
    * @method:    key_to_index 由key得到相应对象的桶索引
    * @param: const void * key
    * @param: int32_t keylength
    */
    virtual int32_t key_to_index(const void* key, int32_t keylength);

    /*
    * @method:    get_object_by_key
    * @param: const void * key
    * @param: int32_t keylength
    * @return:   base_object*
    */
    base_object* get_object_by_key(const void* key, int32_t keylength);

    /*
    * @method:    delete_object_by_key
    * @param: const void * key
    * @param: int32_t keylength
    * @return:   int32_t 返回回收的对象的索引值/对象ID
    * - invalid_object_index : 表示回收失败
    * - 回收对象的索引值/对象ID
    */
    int32_t delete_object_by_key(const void* key, int32_t keylength);

    /*
    * @method:    create_object_by_key
    * @param: const void * key
    * @param: int32_t keylength
    * @return:   base_object*
    */
    base_object* create_object_by_key(const void* key, int32_t keylength);

    /*
    * @method:    operator[]
    * @param: int32_t index
    * @return:   hash_bucket&
    * @note 调用者自己保证形参index的正确性
    */
    hash_bucket& operator [](int32_t index);

protected:
    /*
    * @method:    get_hashcode 哈西函数
    * @param: void * key
    * @param: int32_t keylength
    * @return:   uint32_t 返回key的哈西值
    */
    uint32_t get_hashcode(const void* key, int32_t keylength);


    /*
    * @method:    equal_key 键的比较函数
    * @param: const void * srckey
    * @param: int32_t srckeylength
    * @param: const void * dstkey
    * @param: int32_t dstkeylength
    */
    bool equal_key(const void* srckey,int32_t srckeylength, const void* dstkey, int32_t dstkeylength);

private:
    int32_t hash_next_;                /** 用于形成冲突链所用的元素在index_node::additional_data_[]中的下标*/
    object_pool* object_pool_;        /** hashtable的底层对象管理器 */
    hash_bucket bucket_[max_hash_bucket_size];    /** 桶*/
};

template<int32_t SIZE>
hash_table<SIZE>::hash_table(object_pool* pool, int32_t next)
                             : object_pool_(pool), hash_next_(next)
{
    if (next <= invalid_object_index)
    {
        return;
    }

    if (object_createmode_init == base_object::create_mode_)
    {
        for (int32_t i = 0; i < max_hash_bucket_size; ++i)
        {
            bucket_[i].head_ = invalid_object_index;
        }
    }
}

template<int32_t SIZE>
hash_bucket& hash_table<SIZE>::operator [](int32_t index)
{
    return bucket_[index];
}

template<int32_t SIZE>
base_object* hash_table<SIZE>::create_object_by_key(const void* key, int32_t keylength)
{
    if (NULL == key || 0 >= keylength || NULL == object_pool_)
    {
        return NULL;
    }

    char* tmp_key[max_hash_key_length];
    int32_t tmp_keysize = 0;

    base_object* object_ptr = NULL;
    index_node* index_node_ptr = NULL;

    int32_t tmp_index = invalid_object_index;
    int32_t bucket_idx = key_to_index(key, keylength);
    if (bucket_[bucket_idx].head_ >= 0)//有冲突链
    {
        tmp_index = bucket_[bucket_idx].head_;
        while (tmp_index >= 0)
        {
            index_node_ptr = object_pool_->get_index(tmp_index);
            if (NULL == index_node_ptr)
            {
                return NULL;
            }

            object_ptr = object_pool_->get_object(tmp_index);
            if (NULL == object_ptr)
            {
                return NULL;
            }

            tmp_keysize = (int32_t)sizeof(tmp_key);
            object_ptr->get_key(&tmp_key[0], tmp_keysize);
            if (equal_key(key, keylength, &tmp_key[0], tmp_keysize))
            {
                break;
            }

            index_node_ptr->get_additional_data_by_index(hash_next_, tmp_index);
        }//while

        object_ptr = object_pool_->get_object(tmp_index);
        if (NULL != object_ptr)
        {
            return object_ptr;
        }
    }//if (bucket_[bucket_idx].head_ >= 0)

    tmp_index = object_pool_->create_object();
    if (invalid_object_index == tmp_index)
    {
        return NULL;
    }

    //放入冲突链头部
    index_node_ptr = object_pool_->get_index(tmp_index);
    index_node_ptr->set_additional_data_by_index(hash_next_, bucket_[bucket_idx].head_);
    bucket_[bucket_idx].head_ = tmp_index;
    object_ptr = object_pool_->get_object(tmp_index);
    object_ptr->set_key(key, keylength);

    return object_ptr;
}

template<int32_t SIZE>
base_object* hash_table<SIZE>::get_object_by_key(const void* key, int32_t keylength)
{
    if (NULL == key || keylength <= 0 || NULL == object_pool_)
    {
        return NULL;
    }

    int32_t index = key_to_index(key, keylength);
    if (bucket_[index].head_ == invalid_object_index)
    {
        return NULL;
    }

    //在冲突链中查找
    char tmp_key[max_hash_key_length];
    int32_t tmp_keysize = 0;

    int32_t tmp_index = bucket_[index].head_;
    while (tmp_index >= 0)
    {
        index_node* tmp_objectindex = object_pool_->get_index(tmp_index);
        if (NULL == tmp_objectindex)
        {
            return NULL;
        }

        base_object* tmp_object = object_pool_->get_object(tmp_index);
        if (NULL == tmp_object)
        {
            return NULL;
        }

        tmp_keysize = (int32_t)sizeof(tmp_key);
        tmp_object->get_key(&tmp_key[0], tmp_keysize);
        if (equal_key(key, keylength, &tmp_key[0], tmp_keysize))//got it!
        {
            break;
        }

        tmp_objectindex->get_additional_data_by_index(hash_next_, tmp_index);
    }//while

    return object_pool_->get_object(tmp_index);
}

template<int32_t SIZE>
int32_t hash_table<SIZE>::delete_object_by_key(const void* key, int32_t keylength)
{
    if (NULL == key || 0 >= keylength || NULL == object_pool_)
    {
        return invalid_object_index;
    }

    int32_t bucket_idx = key_to_index(key, keylength);
    if (bucket_[bucket_idx].head_ == invalid_object_index)//对应的slot/bucket中没有任何对象
    {
        return invalid_object_index;
    }

    char tmp_key[max_hash_key_length];
    int32_t key_length = 0;

    index_node* index_node_ptr = NULL;
    base_object* object_ptr = NULL;

    //在冲突链中进行查找
    int32_t tmp_prev = invalid_object_index;
    int32_t tmp_index = bucket_[bucket_idx].head_;
    while (tmp_index >= 0)
    {
        index_node_ptr = object_pool_->get_index(tmp_index);
        if (NULL == index_node_ptr)
        {
            return invalid_object_index;
        }

        object_ptr = object_pool_->get_object(tmp_index);
        if (NULL == object_ptr)
        {
            return invalid_object_index;
        }

        key_length = (int32_t)sizeof(tmp_key);
        object_ptr->get_key(&tmp_key[0], key_length);
        if (equal_key(key, keylength, &tmp_key[0], key_length))//got it!
        {
            break;
        }

        //otherwise
        tmp_prev = tmp_index;//记录冲突链中某结点的前驱结点
        index_node_ptr->get_additional_data_by_index(hash_next_, tmp_index);
    }

    if (invalid_object_index == tmp_index)//do not get it
    {
        return invalid_object_index;
    }

    //删除找到的对象
    int32_t tmp_next_idx = invalid_object_index;
    index_node* tmp_prev_index = NULL;

    index_node_ptr->get_additional_data_by_index(hash_next_, tmp_next_idx);
    tmp_prev_index = object_pool_->get_index(tmp_prev);
    if (NULL != tmp_prev_index)//待删除对象在冲突链中有前驱对象
    {
        tmp_prev_index->set_additional_data_by_index(hash_next_, tmp_next_idx);
    }

    //清除待删除对象的索引中记录的用于形成冲突链的信息
    index_node_ptr->set_additional_data_by_index(hash_next_, invalid_object_index);

    if (tmp_index == bucket_[bucket_idx].head_)
    {
        bucket_[bucket_idx].head_ = tmp_next_idx;
    }

    return object_pool_->destroy_object(tmp_index);
}

template<int32_t SIZE>
int32_t hash_table<SIZE>::key_to_index(const void* key, int32_t keylength)
{
    uint32_t hash_code = get_hashcode(key, keylength);
    return (int32_t)(hash_code%max_hash_bucket_size);
}

template<int32_t SIZE>
uint32_t hash_table<SIZE>::get_hashcode(const void* key, int32_t keylength)
{
    uint32_t hash_code = 0;
    uint16_t* tmp_key = (uint16_t*)key;

    for (int32_t i = 0; i < (int32_t)(keylength/sizeof(uint16_t)); ++i)
    {
        hash_code += tmp_key[i];
    }

    char* tmp_key_pos = NULL;
    uint16_t tmp_key_length = 0;
    if (keylength%sizeof(uint16_t) > 0)
    {
        tmp_key_pos = ((char*)key + (keylength - (keylength%sizeof(uint16_t))));
        memcpy((void*)&tmp_key_length, (const void*)tmp_key_pos, size_t(keylength%sizeof(uint16_t)));
    }

    hash_code += tmp_key_length;

    hash_code = (hash_code & (uint16_t)0x7fff);

    return hash_code;
}

template<int32_t SIZE>
bool hash_table<SIZE>::equal_key(const void* srckey, int32_t srckeylength,
                                                     const void* dstkey, int32_t dstkeylength)
{
    return(srckeylength == dstkeylength && !memcmp(srckey, dstkey, srckeylength));
}

#endif /* __COMMON_OBJECT_HASHTABLE_H__ */
