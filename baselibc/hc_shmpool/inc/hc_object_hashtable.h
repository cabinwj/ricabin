#ifndef __COMMON_OBJECT_HASHTABLE_H__
#define __COMMON_OBJECT_HASHTABLE_H__

#include "hc_base.h"
#include "hc_object_pool.h"

#include <string.h>

/*
 * @purpose ʵ��һ��hash_table���ײ���object_pool��ʵ�֡��ر�ע�⣺SIZE����Ϊ0��
 * base_object��key ������ַ������ͣ�keylengthΪstrlen()�ķ���ֵ��
 */

// bucket of hash table
struct hash_bucket
{
    int32_t head_;     //��slot/bucket��ͻ������Ԫ��(��ʵ�Ƕ��������ֵ)
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
    * @method:    key_to_index ��key�õ���Ӧ�����Ͱ����
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
    * @return:   int32_t ���ػ��յĶ��������ֵ/����ID
    * - invalid_object_index : ��ʾ����ʧ��
    * - ���ն��������ֵ/����ID
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
    * @note �������Լ���֤�β�index����ȷ��
    */
    hash_bucket& operator [](int32_t index);

protected:
    /*
    * @method:    get_hashcode ��������
    * @param: void * key
    * @param: int32_t keylength
    * @return:   uint32_t ����key�Ĺ���ֵ
    */
    uint32_t get_hashcode(const void* key, int32_t keylength);


    /*
    * @method:    equal_key ���ıȽϺ���
    * @param: const void * srckey
    * @param: int32_t srckeylength
    * @param: const void * dstkey
    * @param: int32_t dstkeylength
    */
    bool equal_key(const void* srckey,int32_t srckeylength, const void* dstkey, int32_t dstkeylength);

private:
    int32_t hash_next_;                /** �����γɳ�ͻ�����õ�Ԫ����index_node::additional_data_[]�е��±�*/
    object_pool* object_pool_;        /** hashtable�ĵײ��������� */
    hash_bucket bucket_[max_hash_bucket_size];    /** Ͱ*/
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
    if (bucket_[bucket_idx].head_ >= 0)//�г�ͻ��
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

    //�����ͻ��ͷ��
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

    //�ڳ�ͻ���в���
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
    if (bucket_[bucket_idx].head_ == invalid_object_index)//��Ӧ��slot/bucket��û���κζ���
    {
        return invalid_object_index;
    }

    char tmp_key[max_hash_key_length];
    int32_t key_length = 0;

    index_node* index_node_ptr = NULL;
    base_object* object_ptr = NULL;

    //�ڳ�ͻ���н��в���
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
        tmp_prev = tmp_index;//��¼��ͻ����ĳ����ǰ�����
        index_node_ptr->get_additional_data_by_index(hash_next_, tmp_index);
    }

    if (invalid_object_index == tmp_index)//do not get it
    {
        return invalid_object_index;
    }

    //ɾ���ҵ��Ķ���
    int32_t tmp_next_idx = invalid_object_index;
    index_node* tmp_prev_index = NULL;

    index_node_ptr->get_additional_data_by_index(hash_next_, tmp_next_idx);
    tmp_prev_index = object_pool_->get_index(tmp_prev);
    if (NULL != tmp_prev_index)//��ɾ�������ڳ�ͻ������ǰ������
    {
        tmp_prev_index->set_additional_data_by_index(hash_next_, tmp_next_idx);
    }

    //�����ɾ������������м�¼�������γɳ�ͻ������Ϣ
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
