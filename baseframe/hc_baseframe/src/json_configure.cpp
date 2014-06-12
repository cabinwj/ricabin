#include "json_configure.h"

#include "hc_log.h"

#include "json/json.h"

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>


json_configure::json_configure() : m_use_index_(master)
{
    memset(m_configure_, 0, sizeof(m_configure_));
}

//加载配置文件
void json_configure::load_mylog(const char* json_file_name)
{
    if (NULL == json_file_name)
    {
        std::cout << "json_file_name is NULL" << std::endl;
        exit(-1);
    }
    std::cout << "json_file_name is " << json_file_name << std::endl;

    // 此处可以改成从配置中心（catlog）拉取
    FILE* pfJsonFile = fopen(json_file_name, "rb");
    if (NULL == pfJsonFile)
    {
        std::cout << "open json_file_name " << json_file_name << " failed." << std::endl;
        exit(-1);
    }

    fseek(pfJsonFile, 0, SEEK_END);
    uint32_t unFileSize = ftell(pfJsonFile);
    fseek(pfJsonFile, 0, SEEK_SET);

    char* pTextBuffer = (char*)malloc(sizeof(char) * (unFileSize + 1));
    uint32_t unCopyLen = fread(pTextBuffer, sizeof(char), unFileSize, pfJsonFile);
    if (unCopyLen != unFileSize)  
    {  
        std::cout << "read json_file_name " << json_file_name << " failed. copylen(" 
            << unCopyLen << ") != filesize(" << unFileSize << ")" << std::endl;  
        exit(-1);  
    }  
    pTextBuffer[unCopyLen] = '\0';

    fclose(pfJsonFile);

    Json::Reader jReader;
    Json::Value jRoot;
    bool rc = jReader.parse(pTextBuffer, jRoot);
    free(pTextBuffer);
    if (!rc)
    {
        std::cout << "oops, could not parse json file. json_file_name: " << json_file_name << std::endl;
        exit(-1);
    }

    // 获取log节点
    Json::Value jLogDict = jRoot["log"];
    if (jLogDict.isNull())
    {
        std::cout << "dict: log is NULL. json_file_name: " << json_file_name << std::endl;
        exit(-1);
    }

    strcpy(m_log_.m_path_, jLogDict["path"].asCString());
    std::cout << "attribute: path is " << jLogDict["path"].asCString() << ". json_file_name: " << json_file_name << std::endl;

    uint32_t un32Value = (uint32_t)strtoul(jLogDict["level"].asCString(), NULL, 16);
    m_log_.m_level_ = un32Value;
    std::cout << "attribute: level is " << un32Value << ". json_file_name: " << json_file_name << std::endl;

    un32Value = (uint32_t)strtoul(jLogDict["file_size"].asCString(), NULL, 16);
    m_log_.m_file_size_ = un32Value;
    std::cout << "attribute: file_size is " << un32Value << ". json_file_name: " << json_file_name << std::endl;
}

//加载配置文件
void json_configure::load_myconf(const char* json_file_name)
{
    if (NULL == json_file_name)
    {
        LOG(ERROR)("json_file_name is NULL.\n");
        exit(-1);
    }

    // 此处可以改成从配置中心（catlog）拉取
    FILE* pfJsonFile = fopen(json_file_name, "rb");
    if (NULL == pfJsonFile)
    {
        LOG(ERROR)("open json_file_name %s failed.\n", json_file_name);
        exit(-1);
    }

    fseek(pfJsonFile, 0, SEEK_END);
    uint32_t unFileSize = ftell(pfJsonFile);
    fseek(pfJsonFile, 0, SEEK_SET);

    char* pTextBuffer = (char*)malloc(sizeof(char) * (unFileSize + 1));
    uint32_t unCopyLen = fread(pTextBuffer, sizeof(char), unFileSize, pfJsonFile);
    if (unCopyLen != unFileSize)  
    {  
        LOG(ERROR)("read json_file_name %s failed. copylen(%d) != filesize(%d)", json_file_name, unCopyLen, unFileSize);  
        exit(-1);  
    }  
    pTextBuffer[unCopyLen] = '\0';

    fclose(pfJsonFile);

    Json::Reader jReader;
    Json::Value jRoot;
    bool rc = jReader.parse(pTextBuffer, jRoot);
    free(pTextBuffer);
    if (!rc)
    {
        LOG(ERROR)("oops, could not parse json file. json_file_name: %s", json_file_name);
        exit(-1);
    }

    uint16_t index = master;
    if (master == m_use_index_)
    {
        index = slave;
    }

    // 获取common节点
    Json::Value jCommonDict = jRoot["common"];
    if (jCommonDict.isNull())
    {
        LOG(ERROR)("dict: common is NULL. json_file_name: %s", json_file_name);
        exit(-1);
    }

    uint16_t un16Value = (uint16_t)jCommonDict["zone_id"].asUInt();
    m_configure_[index].m_common_.m_zone_id_ = un16Value;
    LOG(DEBUG)("attribute: zone_id is %d. json_file_name: %s", un16Value, json_file_name);

    uint32_t un32Value = (uint32_t)strtoul(jCommonDict["module_id"].asCString(), NULL, 16);
    m_configure_[index].m_common_.m_module_id_ = un32Value;
    LOG(DEBUG)("attribute: module_id is %08X. json_file_name: %s", un32Value, json_file_name);

    un16Value = (uint16_t)jCommonDict["socket_send_buffer_size"].asUInt();
    m_configure_[index].m_common_.m_socket_send_buffer_size_ = un16Value;
    LOG(DEBUG)("attribute: socket_send_buffer_size is %d. json_file_name: %s", un16Value, json_file_name);

    un16Value = (uint16_t)jCommonDict["socket_recv_buffer_size"].asUInt();
    m_configure_[index].m_common_.m_socket_recv_buffer_size_ = un16Value;
    LOG(DEBUG)("attribute: socket_recv_buffer_size is %d. json_file_name: %s", un16Value, json_file_name);

    static char* module_label_prefix = "module_id_";
    static char module_label[max_module_label_length];
    strncpy(module_label, module_label_prefix, strlen(module_label_prefix));

    // 获取accept节点
    Json::Value jAcceptList = jRoot["accept"];
    if (jAcceptList.isNull())
    {
        LOG(ERROR)("dict: accept is NULL. json_file_name: %s", json_file_name);
        exit(-1);
    }

    un16Value = (uint16_t)jAcceptList.size();
    if (un16Value > max_module_type_count)
    {
        LOG(ERROR)("attribute: count(%d) is out of range(%d)!. json_file_name: %s", un16Value, max_module_type_count, json_file_name);
        exit(-1);
    }
    m_configure_[index].m_accept_set_.m_count_ = un16Value;
    for (uint16_t idx_x = 0; idx_x < (uint16_t)jAcceptList.size(); idx_x++)
    {
        Json::Value jCurrentAcceptDict = jAcceptList[idx_x];
        if (jCurrentAcceptDict.isNull())
        {
            LOG(ERROR)("dict: accept is NULL. json_file_name: %s", json_file_name);
            exit(-1);
        }

        strncpy(module_label + strlen(module_label_prefix),
            jCurrentAcceptDict["module_id"].asCString(),
            strlen(jCurrentAcceptDict["module_id"].asCString()));

        uint32_t un32Value = (uint32_t)strtoul(jCurrentAcceptDict["module_id"].asCString(), NULL, 16);
        m_configure_[index].m_accept_set_.m_array_[idx_x].m_module_id_ = un32Value;
        LOG(DEBUG)("attribute: idx %d module_id is %08X. json_file_name: %s", idx_x, un32Value, json_file_name);

        uint16_t un16Value = (uint16_t)jCurrentAcceptDict["keepalive_timeout"].asUInt();
        m_configure_[index].m_accept_set_.m_array_[idx_x].m_keepalive_timeout_ = un16Value;
        LOG(DEBUG)("attribute: idx %d keepalive_timeout is %d. json_file_name: %s", idx_x, un16Value, json_file_name);

        Json::Value jModuleLabelDict = jRoot[module_label];
        un32Value = (uint32_t)strtoul(jModuleLabelDict["module_id"].asCString(), NULL, 16);
        if (un32Value != m_configure_[index].m_accept_set_.m_array_[idx_x].m_module_id_)
        {
            LOG(DEBUG)("attribute: idx %d module_id is %08X but accept_module_id %08X. json_file_name: %s", idx_x, un32Value, m_configure_[index].m_accept_set_.m_array_[idx_x].m_module_id_, json_file_name);
            exit(-1);
        }

        strcpy(m_configure_[index].m_accept_set_.m_array_[idx_x].m_ip_, jModuleLabelDict["ip"].asCString());
        LOG(DEBUG)("attribute: idx %d ip is %s. json_file_name: %s", idx_x, jModuleLabelDict["ip"].asCString(), json_file_name);

        un16Value = (uint16_t)jModuleLabelDict["port"].asUInt();
        m_configure_[index].m_accept_set_.m_array_[idx_x].m_port_ = un16Value;
        LOG(DEBUG)("attribute: idx %d port is %d. json_file_name: %s", idx_x, un16Value, json_file_name);
    }

    // 获取connect节点
    Json::Value jConnectList = jRoot["connect"];
    if (jConnectList.isNull())
    {
        LOG(ERROR)("dict: connect is NULL. json_file_name: %s", json_file_name);
        exit(-1);
    }

    un16Value = (uint16_t)jConnectList.size();
    if (un16Value > max_module_type_count)
    {
        LOG(ERROR)("attribute: count(%d) is out of range(%d)!. json_file_name: %s", un16Value, max_module_type_count, json_file_name);
        exit(-1);
    }
    m_configure_[index].m_connect_set_.m_count_ = un16Value;
    for (uint16_t idx_x = 0; idx_x < (uint16_t)jConnectList.size(); idx_x++)
    {
        Json::Value jCurrentConnectDict = jConnectList[idx_x];
        if (jCurrentConnectDict.isNull())
        {
            LOG(ERROR)("dict: accept is NULL. json_file_name: %s", json_file_name);
            exit(-1);
        }

        strncpy(module_label + strlen(module_label_prefix),
            jCurrentConnectDict["module_id"].asCString(),
            strlen(jCurrentConnectDict["module_id"].asCString()));

        un32Value = (uint32_t)strtoul(jCurrentConnectDict["module_id"].asCString(), NULL, 16);
        m_configure_[index].m_connect_set_.m_array_[idx_x].m_module_id_ = un32Value;
        LOG(DEBUG)("attribute: idx %d module_id is %08X. json_file_name: %s", idx_x, un32Value, json_file_name);

        un16Value = (uint16_t)jCurrentConnectDict["connect_out_timeout"].asUInt();
        m_configure_[index].m_connect_set_.m_array_[idx_x].m_connect_out_timeout_ = un16Value;
        LOG(DEBUG)("attribute: idx %d connect_out_timeout is %d. json_file_name: %s", idx_x, un16Value, json_file_name);

        un16Value = (uint16_t)jCurrentConnectDict["keepalive_interval"].asUInt();
        m_configure_[index].m_connect_set_.m_array_[idx_x].m_keepalive_interval_ = un16Value;
        LOG(DEBUG)("attribute: idx %d keepalive_interval is %d. json_file_name: %s", idx_x, un16Value, json_file_name);

        Json::Value jModuleLabelDict = jRoot[module_label];
        un32Value = (uint32_t)strtoul(jModuleLabelDict["module_id"].asCString(), NULL, 16);
        if (un32Value != m_configure_[index].m_connect_set_.m_array_[idx_x].m_module_id_)
        {
            LOG(DEBUG)("attribute: idx %d module_id is %08X but connect_module_id %08X. json_file_name: %s", idx_x, un32Value, m_configure_[index].m_connect_set_.m_array_[idx_x].m_module_id_, json_file_name);
            exit(-1);
        }

        strcpy(m_configure_[index].m_connect_set_.m_array_[idx_x].m_ip_, jModuleLabelDict["ip"].asCString());
        LOG(DEBUG)("attribute: idx %d ip is %s. json_file_name: %s", idx_x, jModuleLabelDict["ip"].asCString(), json_file_name);

        un16Value = (uint16_t)jModuleLabelDict["port"].asUInt();
        m_configure_[index].m_connect_set_.m_array_[idx_x].m_port_ = un16Value;
        LOG(DEBUG)("attribute: idx %d port is %d. json_file_name: %s", idx_x, un16Value, json_file_name);
    }

    m_use_index_ = index;
}

//加载配置文件
void json_configure::load_module(const char* json_file_name)
{
    if (NULL == json_file_name)
    {
        LOG(ERROR)("json_file_name is NULL");
        exit(-1);
    }

    // 此处可以改成从配置中心（catlog）拉取
    FILE* pfJsonFile = fopen(json_file_name, "rb");
    if (NULL == pfJsonFile)
    {
        std::cout << "open json_file_name " << json_file_name << " failed." << std::endl;
        exit(-1);
    }

    fseek(pfJsonFile, 0, SEEK_END);
    uint32_t unFileSize = ftell(pfJsonFile);
    fseek(pfJsonFile, 0, SEEK_SET);

    char* pTextBuffer = (char*)malloc(sizeof(char) * (unFileSize + 1));
    uint32_t unCopyLen = fread(pTextBuffer, sizeof(char), unFileSize, pfJsonFile);
    if (unCopyLen != unFileSize)  
    {  
        std::cout << "read json_file_name " << json_file_name << " failed. copylen(" << unCopyLen << ") != filesize(" << unFileSize << ")" << std::endl;  
        exit(-1);  
    }  
    pTextBuffer[unCopyLen] = '\0';

    fclose(pfJsonFile);

    Json::Reader jReader;
    Json::Value jRoot;
    bool rc = jReader.parse(pTextBuffer, jRoot);
    free(pTextBuffer);
    if (!rc)
    {
        LOG(ERROR)("oops, could not parse json file. json_file_name: %s", json_file_name);
        exit(-1);
    }

    // 获取module节点
    Json::Value jModuleList = jRoot["module"];
    if (jModuleList.isNull())
    {
        LOG(ERROR)("dict: module is NULL. json_file_name: %s", json_file_name);
        exit(-1);
    }

    uint16_t un16Value = (uint16_t)jModuleList.size();
    if (un16Value > max_module_count)
    {
        LOG(ERROR)("attribute: count(%d) is out of range(%d)!. json_file_name: %s", un16Value, max_module_count, json_file_name);
        exit(-1);
    }
    m_module_set_.m_count_ = un16Value;
    LOG(DEBUG)("attribute: count is %d. json_file_name: %s", un16Value, json_file_name);
    for (uint16_t idx_x = 0; idx_x < (uint16_t)jModuleList.size(); idx_x++)
    {
        Json::Value jCurrentModuleDict = jModuleList[idx_x];

        uint32_t un32Value = (uint32_t)strtoul(jCurrentModuleDict["module_id"].asCString(), NULL, 16);
        m_module_set_.m_array_[idx_x].m_module_id_ = un32Value;
        LOG(DEBUG)("attribute: idx %d module_id is %08X. json_file_name: %s", idx_x, un32Value, json_file_name);

        strcpy(m_module_set_.m_array_[idx_x].m_ip_, jCurrentModuleDict["ip"].asCString());
        LOG(DEBUG)("attribute: idx %d ip is %s. json_file_name: %s", idx_x, jCurrentModuleDict["ip"].asCString(), json_file_name);

        uint16_t un16Value = (uint16_t)jCurrentModuleDict["port"].asUInt();
        m_module_set_.m_array_[idx_x].m_port_ = un16Value;
        LOG(DEBUG)("attribute: idx %d port is %d. json_file_name: %s", idx_x, un16Value, json_file_name);
    }
}