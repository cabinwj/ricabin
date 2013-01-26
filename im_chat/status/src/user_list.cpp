#include "user_list.h"

#include "hc_log.h"


uint32_t user_node::m_user_count_ = 0;
list_head user_node::m_user_list_head_;
list_head* user_node::m_user_list_iter_ = NULL;

user_node::user_node(uint32_t uid) : hash_node(uid)
{
    INIT_LIST_HEAD(&m_hash_item_);
    INIT_LIST_HEAD(&m_list_item_);
    INIT_LIST_HEAD(&m_link_item_);

    m_user_count_++;
    LOG(INFO)("online user(key:%u) count:%u", uid, m_user_count_);
}

user_node::~user_node()
{
    list_del(&m_hash_item_);
    if ( m_user_list_iter_ == &m_list_item_ )
    {
        m_user_list_iter_ = m_user_list_iter_->next;
    }

    list_del(&m_list_item_);
    list_del(&m_link_item_);

    m_user_count_--;
    LOG(INFO)("online user(key:%u) count:%u", m_key_, m_user_count_);
}


list_head user_list::m_user_hash_bucket_[USER_HASH_BUCKET_COUNT];

void user_list::init()
{
    hash_list::init(m_user_hash_bucket_, USER_HASH_BUCKET_COUNT);

    INIT_LIST_HEAD(&user_node::m_user_list_head_);

    user_node::m_user_list_iter_ = &user_node::m_user_list_head_;
}

user_node* user_list::get(uint32_t key)
{
    return static_cast<user_node*>(hash_list::get(key, m_user_hash_bucket_, USER_HASH_BUCKET_COUNT));
}

void user_list::put(user_node *p)
{
    if ( NULL != user_list::get(p->m_key_) )
    {
        return;
    }

    hash_list::put(p, m_user_hash_bucket_, USER_HASH_BUCKET_COUNT);

    list_add_head( &(p->m_list_item_), &user_node::m_user_list_head_);
}

user_node* user_list::iter_user()
{
    if ( user_node::m_user_list_iter_ == &user_node::m_user_list_head_ )
    {
        if ( user_node::m_user_list_iter_->next == user_node::m_user_list_iter_ )
        {
            return NULL;
        }
        else
        {
            user_node::m_user_list_iter_ = user_node::m_user_list_iter_->next;
        }
    }

    user_node* user = static_cast<user_node*>(list_entry(user_node::m_user_list_iter_, hash_node, m_list_item_));
    user_node::m_user_list_iter_ = user_node::m_user_list_iter_->next;

    return user;                     
}

/* test function */
void user_list::print_hash()
{
    for (uint32_t i = 0;i < USER_HASH_BUCKET_COUNT;i ++)
    {
        if (list_empty(&(m_user_hash_bucket_[i])))
        {
            continue;
        }

        list_head* pos;
        list_head* head = &(m_user_hash_bucket_[i]);
        list_for_each(pos, head)
        {
            user_node* p = list_entry(pos, user_node, m_hash_item_);
        }
    }

    list_head* pos;
    list_head* head = &user_node::m_user_list_head_;
    list_for_each(pos, head)
    {
        user_node* p = list_entry(pos, user_node, m_list_item_);
    }
}

