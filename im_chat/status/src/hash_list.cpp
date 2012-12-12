#include "hash_list.h"


static inline uint32_t hashfn(uint32_t key, uint32_t hashsize)
{
	key ^= (key >> 16);
	key ^= (key >> 8);
	return (key % hashsize);
}

hash_node::hash_node(uint32_t key) : m_key_(key)
{
    INIT_LIST_HEAD(&m_hash_item_);
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_link_item_);
}

hash_node::~hash_node()
{
}

void hash_list::init(list_head* bucket, uint32_t hashsize)
{
	for (uint32_t i = 0; i < hashsize; i++)
	{
		 INIT_LIST_HEAD( bucket + i );
	}
}

hash_node* hash_list::get(uint32_t key, list_head* bucket, uint32_t hashsize)
{
	list_head* pos;
	list_head* head = &bucket[hashfn(key, hashsize)];

	list_for_each(pos, head) 
	{
		hash_node* p = list_entry(pos, hash_node, m_hash_item_);
		if (p->m_key_ == key)
        {
            return p;
        }
	}
    return 0;
}

void hash_list::put(hash_node* p, list_head* bucket, uint32_t hashsize)
{
	uint32_t index = hashfn(p->m_key_, hashsize);
 	list_add_head(&(p->m_hash_item_), &(bucket[index]) );
}
