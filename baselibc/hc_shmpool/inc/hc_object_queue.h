#ifndef __COMMON_OBJECT_QUEUE_H__
#define __COMMON_OBJECT_QUEUE_H__

#include "hc_object_pool.h"

/*
 * @purpose 以object_pool为底层对象管理器，实现一个单向队列。
 *                主要提供查找，删除，插入等操作。 可作为主控消息队列
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
    * @method:    返回object对象在对象中的后置对象的索引/ID
    */
    int32_t next(int32_t object)const;

    /*
    * @method:    set_next 设置object对象的后置对象为next
    */
    int32_t set_next(int32_t object, int32_t ad_data_index);

    /*
    * @method:    append 从尾部追加
    */
    int32_t append(int32_t object);

    /*
    * @method:    push 从头部插入
    */
    int32_t push(int32_t object);

    /*
    * @method:    pop 删除头部对象
    * @return:   返回头部对象的索引/ID,invalid_object_id 表示无效的对象id
    */
    int32_t pop();

    /*
    * @method:    insert 在prev后面插入object
    */
    int32_t insert(int32_t prev, int32_t object);

    /*
    * @method:   erase 删除指定对象object
    */
    int32_t erase(int32_t object);

    void dump(const char* file);

private:
    int32_t head_;                    //队列的头对象的Index(ID)
    int32_t tail_;                    //队列的最后一个对象的Index(ID)

    int32_t ad_data_index_;    //某对象的object_index::addition_data_[next_index_]用于指向其在队列中的下一个元素
    object_pool* pool_;    //实际对象的管理器
};

#endif //__COMMON_OBJECT_QUEUE_H__

