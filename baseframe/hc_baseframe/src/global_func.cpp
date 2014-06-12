#include "global_func.h"

#include "entity_types.h"


//int16_t entity_count_by_entity_type_o(const entity_set_t& entity_set, const uint32_t entity_type)
//{
//    for (int16_t x = 0; x < entity_set.m_count_; ++x)
//    {
//        if ( (entity_set.m_array_[x].m_type_ & pf_entity_type_mark) == (entity_type & pf_entity_type_mark))
//        {
//            return entity_set.m_array_[x].m_count_;
//        }
//    }
//
//    return 0;
//}

//uint32_t entity_id_by_addr_o(const entity_set_t& entity_set, const Address& laddr)
//{
//    for (int16_t x = 0; x < entity_set.m_count_; ++x)
//    {
//        int16_t entity_id_count = entity_set.m_array_[x].m_count_;
//        for (int16_t y = 0; y < entity_id_count; ++y)
//        {
//            Address raddr(entity_set.m_array_[x].m_array_[y].m_ip_,
//                entity_set.m_array_[x].m_array_[y].m_port_);
//
//            if (raddr.net_ip() == laddr.net_ip())
//            {
//                return (entity_set.m_array_[x].m_type_ & pf_entity_type_mark) + ((y+1) & pf_entity_id_mark);
//            }
//        }
//    }
//
//    return 0;
//}

