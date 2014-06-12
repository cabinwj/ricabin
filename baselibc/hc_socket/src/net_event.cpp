#include "net_event.h"

#include "net_config.h"
#include "net_package.h"


object_holder<net_event>* net_event::m_pool_ = new \
    object_holder<net_event>( \
    new object_pool_allocator<net_event>(NET_EVENT_POOL_COUNT, \
                                         5 * NET_EVENT_POOL_COUNT, \
                                         NET_EVENT_POOL_COUNT, \
                                         new_allocator::Instance()));

net_event::net_event() : m_net_ev_t_(net_event::NE_NULL),
    m_net_id_(0), m_net_package_(NULL), m_user_data_(NULL)
{
}

net_event::~net_event()
{
    if ( NULL != m_net_package_ )
    {
        m_net_package_->Destroy();
        m_net_package_ = NULL;
    }
}