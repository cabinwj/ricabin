#ifndef __COMMON_OBJECT_QUEUE_H__
#define __COMMON_OBJECT_QUEUE_H__

#include "hc_object_pool.h"

/*
 * @purpose ��object_poolΪ�ײ�����������ʵ��һ��������С�
 *                ��Ҫ�ṩ���ң�ɾ��������Ȳ����� ����Ϊ������Ϣ����
 */

class object_pool;
class index_node;

class object_queue
{
public:
    object_queue() { }
    object_queue(object_pool* pool, int32_t ad_data_index);
    ~object_queue() { }

public:
    inline int32_t head()
    {
        return head_;
    }

    inline const int32_t head()const
    {
        return head_;
    }

    inline int32_t tail()
    {
        return tail_;
    }

    inline const int32_t tail()const
    {
        return tail_;
    }

    /*
    * @method:    ����object�����ڶ����еĺ��ö��������/ID
    */
    int32_t next(int32_t object)const;

    /*
    * @method:    set_next ����object����ĺ��ö���Ϊnext
    */
    int32_t set_next(int32_t object, int32_t ad_data_index);

    /*
    * @method:    append ��β��׷��
    */
    int32_t append(int32_t object);

    /*
    * @method:    push ��ͷ������
    */
    int32_t push(int32_t object);

    /*
    * @method:    pop ɾ��ͷ������
    * @return:   ����ͷ�����������/ID,invalid_object_id ��ʾ��Ч�Ķ���id
    */
    int32_t pop();

    /*
    * @method:    insert ��prev�������object
    */
    int32_t insert(int32_t prev, int32_t object);

    /*
    * @method:   erase ɾ��ָ������object
    */
    int32_t erase(int32_t object);

    void dump(const char* file);

private:
    int32_t head_;                    //���е�ͷ�����Index(ID)
    int32_t tail_;                    //���е����һ�������Index(ID)

    int32_t ad_data_index_;    //ĳ�����object_index::addition_data_[next_index_]����ָ�����ڶ����е���һ��Ԫ��
    object_pool* pool_;    //ʵ�ʶ���Ĺ�����
};

#endif //__COMMON_OBJECT_QUEUE_H__

