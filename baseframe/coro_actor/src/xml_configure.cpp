#include "xml_configure.h"

#include <stdlib.h>
#include <string>
#include <iostream>


xml_configure::xml_configure()
    : m_use_index_(master), m_root_elem_(NULL)
{
    bzero(m_net_xml_, sizeof(m_net_xml_));
}

xml_configure::xml_configure(const char* xml_file_name)
    : m_use_index_(master), m_root_elem_(NULL)
{
    //加载配置文件
    TiXmlDocument xml_doc(xml_file_name);
    if (!xml_doc.LoadFile())
    {
        std::cerr << "load file error!" << std::endl;
        exit(-1);
    }

    //得到配置文件的根结点
    m_root_elem_ = xml_doc.RootElement();

    bzero(m_net_xml_, sizeof(m_net_xml_));
}


void xml_configure::load_all(const char* xml_file_name)
{
    if (NULL == xml_file_name)
    {
        std::cerr << "xml file name is NULL" << std::endl;
        return;
    }

    m_root_elem_ = NULL;
    //加载配置文件
    TiXmlDocument xml_doc(xml_file_name);
    if (!xml_doc.LoadFile())
    {
        std::cerr << "load file error!" << std::endl;
        exit(-1);
    }

    //得到配置文件的根结点
    m_root_elem_ = xml_doc.RootElement();
    if (NULL == m_root_elem_)
    {
        std::cerr << "xml file(" << xml_file_name << ") read root error!" << std::endl;
        exit(-1);
    }

    int16_t index = master;
    if (master == m_use_index_)
    {
        index = slave;
    }

    // 获取common节点
    TiXmlElement* node_elem = m_root_elem_->FirstChildElement("common");
    if (NULL == node_elem)
    {
        std::cerr << "node: common is null!" << std::endl;
        exit(-1);
    }

    const char* current = NULL;
    //current = node_elem->Attribute("zone_id");
    //if (NULL == current)
    //{
    //    std::cerr << "attribute: zone_id is null!" << std::endl;
    //    exit(-1);
    //}
    // m_net_xml_[index].m_common_.m_zone_id_ = atoi(current);

    current = node_elem->Attribute("entity_id");
    if (NULL == current)
    {
        std::cerr << "attribute: entity_id is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_common_.m_entity_id_ = (uint32_t)strtol(current, NULL, 16);


    // 获取log节点
    node_elem = m_root_elem_->FirstChildElement("log");
    if (NULL == node_elem)
    {
        std::cerr << "node: log is null!" << std::endl;
        exit(-1);
    }

    current = node_elem->Attribute("log_path");
    if (NULL == current)
    {
        std::cerr << "attribute: log_path is null!" << std::endl;
        exit(-1);
    }
    strcpy(m_net_xml_[index].m_log_.m_log_path_, current);

    current = node_elem->Attribute("log_level");
    if (NULL == current)
    {
        std::cerr << "attribute: log_level is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_log_.m_log_level_ = (uint32_t)strtol(current, NULL, 16);

    current = node_elem->Attribute("file_size");
    if (NULL == current)
    {
        std::cerr << "attribute: file_size is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_log_.m_file_size_ = atol(current);

    // 获取listen节点
    node_elem = m_root_elem_->FirstChildElement("listen");
    if (NULL == node_elem)
    {
        std::cerr << "node: listen is null!" << std::endl;
        exit(-1);
    }

    current = node_elem->Attribute("ip");
    if (NULL == current)
    {
        std::cerr << "attribute: ip is null!" << std::endl;
        exit(-1);
    }
    strcpy(m_net_xml_[index].m_listen_.m_ip_port_.m_ip_, current);

    current = node_elem->Attribute("port");
    if (NULL == current)
    {
        std::cerr << "attribute: port is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_listen_.m_ip_port_.m_port_ = atoi(current);

    current = node_elem->Attribute("alive_time");
    if (NULL == current)
    {
        std::cerr << "attribute: alive_time is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_listen_.m_alive_timestamp_ = atoi(current);

    current = node_elem->Attribute("first_packet_time");
    if (NULL == current)
    {
        std::cerr << "attribute: first_packet_time is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_listen_.m_recv_first_packet_timestamp_ = atoi(current);

    // 获取entity_type节点
    node_elem = m_root_elem_->FirstChildElement("entity_file");
    if (NULL == node_elem)
    {
        std::cerr << "node: entity_type is null!" << std::endl;
        exit(-1);
    }

    current = node_elem->Attribute("count");
    if (NULL == current)
    {
        std::cerr << "attribute: count is null!" << std::endl;
        exit(-1);
    }
    m_net_xml_[index].m_ent_file_.m_ent_file_count_ = atoi(current);

    if (m_net_xml_[index].m_ent_file_.m_ent_file_count_ > max_connect_entity_type_count)
    {
        std::cerr << "attribute: count value(" << m_net_xml_[index].m_ent_file_.m_ent_file_count_
                  << ") is out of range(" << max_connect_entity_type_count << ")!" << std::endl;
        exit(-1);
    }

    // 获取item节点
    TiXmlElement* item_elem = node_elem->FirstChildElement("item");
    if (NULL == item_elem)
    {
        std::cerr << "node: item is null!" << std::endl;
        exit(-1);
    }

    int16_t file_count = m_net_xml_[index].m_ent_file_.m_ent_file_count_;
    for (int16_t i = 0; ((i < file_count) && (NULL != item_elem)); i++)
    {
        current = item_elem->Attribute("id");
        if (NULL == current)
        {
            std::cerr << "attribute: id is null!" << std::endl;
            exit(-1);
        }
        int16_t id = atoi(current);
        if (i != id)
        {
            std::cerr << "attribute: id(" << id << ")!=i(" << i << ")!" << std::endl;
            exit(-1);
        }

        current = item_elem->Attribute("name");
        if (NULL == current)
        {
            std::cerr << "attribute: name is null!" << std::endl;
            exit(-1);
        }
        strcpy(m_net_xml_[index].m_ent_file_.m_ent_file_[i], current);

        item_elem = item_elem->NextSiblingElement();
    }

    // get entity file detail
    m_net_xml_[index].m_ent_set_.m_ent_type_count_ = file_count;
    for (int16_t x = 0; x < file_count; x++)
    {
        //加载配置文件
        TiXmlDocument xml_doc(m_net_xml_[index].m_ent_file_.m_ent_file_[x]);
        if (!xml_doc.LoadFile())
        {
            std::cerr << "load file error!" << std::endl;
            exit(-1);
        }

        //得到配置文件的根结点
        TiXmlElement* root_elem = xml_doc.RootElement();
        if (NULL == root_elem)
        {
            std::cerr << "xml file(" << xml_file_name << ") read root error!" << std::endl;
            exit(-1);
        }

        // 获取entity节点
        node_elem = root_elem->FirstChildElement("entity");
        if (NULL == node_elem)
        {
            std::cerr << "node: entity is null!" << std::endl;
            exit(-1);
        }

        current = node_elem->Attribute("type");
        if (NULL == current)
        {
            std::cerr << "attribute: type is null!" << std::endl;
            exit(-1);
        }
        uint32_t entity_type = ((uint32_t)strtol(current, NULL, 16) & pf_entity_type_mark);
        m_net_xml_[index].m_ent_set_.m_ent_array_[x].m_ent_type_ = entity_type;

        current = node_elem->Attribute("count");
        if (NULL == current)
        {
            std::cerr << "attribute: count is null!" << std::endl;
            exit(-1);
        }
        int16_t entity_id_count = atoi(current);

        if ( (entity_id_count > max_connect_entity_id_count) || (0 == entity_id_count))
        {
            std::cerr << "attribute: count value(" << entity_id_count
                << ") is out of range(" << max_connect_entity_id_count << ")!" << std::endl;
            exit(-1);
        }
        m_net_xml_[index].m_ent_set_.m_ent_array_[x].m_ent_id_count_ = entity_id_count;

        // 获取item节点
        item_elem = node_elem->FirstChildElement("item");
        if (NULL == item_elem)
        {
            std::cerr << "node: item is null!" << std::endl;
            exit(-1);
        }

        for (int16_t y = 0; ((y < entity_id_count) && (NULL != item_elem)); y++)
        {
            current = item_elem->Attribute("id");
            if (NULL == current)
            {
                std::cerr << "attribute: id is null!" << std::endl;
                exit(-1);
            }
            int16_t id = atoi(current);
            if (y + 1 != id)
            {
                std::cerr << "attribute: id(" << id << ")!=y(" << y << ")!" << std::endl;
                exit(-1);
            }

            current = item_elem->Attribute("ip");
            if (NULL == current)
            {
                std::cerr << "attribute: ip is null!" << std::endl;
                exit(-1);
            }
            strcpy(m_net_xml_[index].m_ent_set_.m_ent_array_[x].m_array_[y].m_ip_, current);

            current = item_elem->Attribute("port");
            if (NULL == current)
            {
                std::cerr << "attribute: port is null!" << std::endl;
                exit(-1);
            }
            m_net_xml_[index].m_ent_set_.m_ent_array_[x].m_array_[y].m_port_ = atoi(current);

            item_elem = item_elem->NextSiblingElement();
        }
    }

    m_use_index_ = index;
}

int16_t xml_configure::get_entity_count_by_entity_type(const uint32_t pfent)
{
     for (int16_t x = 0; x < m_net_xml_[m_use_index_].m_ent_set_.m_ent_type_count_; ++x)
     {
         if (m_net_xml_[m_use_index_].m_ent_set_.m_ent_array_[x].m_ent_type_ == (pfent & pf_entity_type_mark))
         {
             return m_net_xml_[m_use_index_].m_ent_set_.m_ent_array_[x].m_ent_id_count_;
         }
     }

     return 0;
}

uint32_t xml_configure::get_entity_id_by_addr(const Address& laddr)
{
    for (int16_t x = 0; x < m_net_xml_[m_use_index_].m_ent_set_.m_ent_type_count_; ++x)
    {
        int16_t entity_id_count = m_net_xml_[m_use_index_].m_ent_set_.m_ent_array_[x].m_ent_id_count_;
        for (int16_t y = 0; y < entity_id_count; ++y)
        {
            Address raddr(m_net_xml_[m_use_index_].m_ent_set_.m_ent_array_[x].m_array_[y].m_ip_,
                          m_net_xml_[m_use_index_].m_ent_set_.m_ent_array_[x].m_array_[y].m_port_);

            if (raddr.get_net_ip() == laddr.get_net_ip())
            {
                return m_net_xml_[m_use_index_].m_ent_set_.m_ent_array_[x].m_ent_type_ + ((y+1) & pf_entity_id_mark);
            }
        }
    }

    return 0;
}

uint32_t xml_configure::get_server_entity_id(const uint32_t uin, uint32_t pfet)
{
    uint16_t count = get_entity_count_by_entity_type((pfet & pf_entity_type_mark));
    return (pfet & pf_entity_type_mark) + (((uin % ( (0 < count) ? count : 1))) & pf_entity_id_mark);
}
