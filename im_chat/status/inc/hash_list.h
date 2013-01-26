#ifndef _STATUS_HASH_LIST_H_
#define _STATUS_HASH_LIST_H_

#include "hc_base.h"
#include "hc_list.h"

class hash_node
{
public:
    hash_node(uint32_t key);
    virtual ~hash_node() = 0;

public:
    uint32_t m_key_;
    list_head m_hash_item_;
    list_head m_list_item_;
    list_head m_link_item_;
};

class hash_list 
{
protected:
	static void init(list_head* bucket, uint32_t hashsize);
	static hash_node* get(uint32_t key, list_head* bucket, uint32_t hashsize);
	static void put(hash_node* p, list_head* bucket, uint32_t hashsize);
};

#endif
