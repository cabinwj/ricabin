#ifndef _NET_PACKAGE_H_
#define _NET_PACKAGE_H_

#include "common_non_copyable.h"
#include "common_destroyable.h"
#include "common_allocator.h"
#include "common_data_block.h"
#include "common_object_guard.h"
#include "common_sync_queue.h"


class net_package : public non_copyable, public destroyable
{
public:
    enum {
        MEMORY_ALIGN = 4
    };

public:
    net_package();
    virtual ~net_package();

public:
    int allocator_data_block(allocatorc* alloc, uint32_t buffer_length);

public:
    void set_data(char* data, uint32_t len);
    inline char* get_data() { return m_data_block_->begin(); }

    inline uint32_t length() const { return m_data_block_->size(); }
    inline uint32_t capacity() const { return m_data_block_->capacity(); }
    inline void offset_cursor(size_t len) { m_data_block_->offset_cursor(len); }

    inline uint32_t handler_id() const { return m_handler_id_; }
    inline void handler_id(uint32_t handlerid) { m_handler_id_ = handlerid; }

    inline uint32_t size() const { return sizeof(m_handler_id_) + sizeof(uint32_t) + length(); }

private:
    allocatorc* m_allocator_;
    data_block* m_data_block_;
    uint32_t m_handler_id_;
};

typedef object_guard<net_package> net_package_pool;

typedef sync_queue<net_package*> net_package_queue;

#endif
