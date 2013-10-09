#include "net_package.h"
#include "net_config.h"

#include "hc_log.h"
#include "hc_stack_trace.h"
#include "hc_allocator.h"


object_holder<net_package>* net_package::m_pool_ = new \
    object_holder<net_package>( \
    new object_pool_allocator<net_package>(NET_PACKET_POOL_COUNT, \
    5 * NET_PACKET_POOL_COUNT, \
    NET_PACKET_POOL_COUNT, \
    new_allocator::Instance()));


net_package::net_package()
    : m_allocator_(NULL), m_data_block_(NULL), m_handler_id_(0)
{
    STACK_TRACE_LOG();
}

net_package::~net_package()
{
    STACK_TRACE_LOG();

    if (NULL != m_data_block_)
    {
        m_data_block_->~data_block();
        m_allocator_->Deallocate(m_data_block_);
        m_data_block_ = NULL;
    }
}

int net_package::allocate_data_block(allocatorc* alloc, uint32_t buffer_length)
{
    STACK_TRACE_LOG();

    if (NULL == alloc)
    {
        LOG(ERROR)("assert: data_package::allocator_data_block() m_allocator_ is NULL.");
        return -1;
    }

    m_allocator_ = alloc;

    if (0 == buffer_length)
    {
        LOG(WARN)("data_package::allocator_data_block() data_block_length is 0.");
        return -1;
    }

    buffer_length = (buffer_length + (net_package::MEMORY_ALIGN - 1)) & ~(net_package::MEMORY_ALIGN - 1);

    if (NULL != m_data_block_)
    {
        LOG(WARN)("data_package::allocator_data_block() m_data_block_ is not NULL, already exist.");

        m_data_block_->clear();
        m_data_block_->resize(buffer_length);

        return 0;
    }

    void* p = m_allocator_->Allocate(sizeof(data_block));
    if (NULL == p)
    {
        LOG(ERROR)("assert: data_package::allocator_data_block() Allocate data_block is NULL.");
        return -1;
    }

    m_data_block_ = new (p) data_block(buffer_length, m_allocator_);

    return 0;
}

void net_package::data_o(char* data, uint32_t data_len)
{
    STACK_TRACE_LOG();

    // data_block剩下的数据空间
    uint32_t available_len = m_data_block_->available();
    if ((data_len > available_len) || (data_len <= 0))
    {
        LOG(ERROR)("data_package::set_data() data_len:%u < available_len:%u.", data_len, available_len);
        return;
    }

    // 拷贝数据
    m_data_block_->append(data, data_len);

    return;
}

