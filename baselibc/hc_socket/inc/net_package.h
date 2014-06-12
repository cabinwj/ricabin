#ifndef _NET_PACKAGE_H_
#define _NET_PACKAGE_H_

#include "hc_non_copyable.h"
#include "hc_destroyable.h"
#include "hc_data_block.h"
#include "hc_object_holder.h"
#include "hc_sync_queue.h"


class allocatorc;
class data_block;

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
    int allocate_data_block(allocatorc* alloc, uint32_t buffer_length);

public:
    void data_o(char* data, uint32_t len);
    inline char* data_o() { return m_data_block_->begin(); }

    inline uint32_t size() const { return m_data_block_->size(); }
    inline uint32_t length() const { return size(); }
    inline uint32_t capacity() const { return m_data_block_->capacity(); }
    inline void offset_cursor(size_t len) { m_data_block_->offset_cursor(len); }

    inline int32_t handler_id() const { return m_handler_id_; }
    inline void handler_id(int32_t handlerid) { m_handler_id_ = handlerid; }

    inline uint16_t protocol_type() const { return m_proto_type_; }
    inline void protocol_type(uint16_t app_proto) { m_proto_type_ = app_proto; }

private:
    allocatorc* m_allocator_;
    data_block* m_data_block_;
    int32_t m_handler_id_;
    uint16_t m_proto_type_;      // Э���ʾ Ӧ��Э�����ͣ�0 �����ƣ�1 4�ֽ��ı���2 6�ֽ��ı���

public:
    //! net_package ����������㱻���䲢��ʼ��, ��Ӧ�ò㱻ʹ�ò�����, Ϊ���Ч��ʹ��Ԥ��������
    static object_holder<net_package>* m_pool_;
};

typedef sync_queue<net_package*> net_package_queue;

#endif
