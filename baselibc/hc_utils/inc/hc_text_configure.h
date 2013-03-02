//! @brief 配置器
#ifndef _HC_TEXT_CONFIGURE_H_
#define _HC_TEXT_CONFIGURE_H_


#include <string>
#include <vector>

//! @class configure
//! @brief 配置器
//!
//! <pre>
//! 本类实现一个 section - key 格式的配置器
//! 配置文件根据一个 [section:key] 对应一个配置值(value)
//! 配置值(value)有三种形式 single, array, array2
//!     single: 单个字符串
//!     array: 多个字符串构成一维数组
//!     array2: 多个字符串构成二维数组
//! 配置文件形如：
//!
//! # 管理服务配置
//! [admin_server]
//! admin_addr = (127.0.0.1 8012)    # array
//! reconnect_interval = 10            # single
//!
//! # 集群服务配置
//! [cluster_server]
//! cluster_list = (                # array2
//!     (0 192.168.52.99 7201)
//!     (1 192.168.52.99 7301)
//!     (2 192.168.52.99 7401)
//!     (3 192.168.52.99 7501)
//! )
//! connect_timeout = 5                # single
//! reconnect_interval = 5
//! keepalive_interval = 10
//! </pre>
//!
class text_configure
{
public:
    //! 加载配置
    //! @param file 配置文件名
    //! @return 0:成功, -1失败
    int load(const char *file);

    //! 取单值配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_single(const char *section, const char *key, std::string &val);

    //! 取单值配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_single(const char *section, const char *key, long &val);

    //! 取单值配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_single(const char *section, const char *key, double &val);

    //! 取一维数组配置元素个数
    //! @param section 段名称
    //! @param key 项名称
    //! @param count [out]元素个数
    //! @return 0:成功, -1失败
    int count1_array(const char *section, const char *key, int &count);

    //! 取一维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素下标
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_array(const char *section, const char *key, int i, std::string &val);

    //! 取一维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素下标
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_array(const char *section, const char *key, int i, long &val);

    //! 取一维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素下标
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_array(const char *section, const char *key, int i, double &val);

    //! 取二维数组配置元素第一维个数
    //! @param section 段名称
    //! @param key 项名称
    //! @param count [out]第一维个数
    //! @return 0:成功, -1失败
    int count1_array2(const char *section, const char *key, int &count);

    //! 取二维数组配置元素第二维个数
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素第一维下标
    //! @param count [out]第二维个数
    //! @return 0:成功, -1失败
    int count2_array2(const char *section, const char *key, int i, int &count);

    //! 取二维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素第一维下标
    //! @param j 元素第二维下标
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_array2(const char *section, const char *key, int i, int j, std::string &val);

    //! 取二维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素第一维下标
    //! @param j 元素第二维下标
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_array2(const char *section, const char *key, int i, int j, long &val);

    //! 取二维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param i 元素第一维下标
    //! @param j 元素第二维下标
    //! @param val [out]配置值
    //! @return 0:成功, -1失败
    int get_array2(const char *section, const char *key, int i, int j, double &val);

public:
    //! 保存配置
    //! @param file 配置文件名
    //! @return 0:成功, -1失败
    int save(const char *file);

    //! 修改单值配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param val 配置值
    //! @return 0:成功, -1失败
    int set_single(const char *section, const char *key, const std::string &val);

    //! 修改一维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param val 配置值
    //! @return 0:成功, -1失败
    int set_array(const char *section, const char *key, const std::vector<std::string> &val);

    //! 修改二维数组配置
    //! @param section 段名称
    //! @param key 项名称
    //! @param val 配置值
    //! @return 0:成功, -1失败
    int set_array2(const char *section, const char *key, const std::vector<std::vector<std::string> > &val);

private:
    //! @enum VALUE_TYPE
    //! @brief 配置值类型
    enum VALUE_TYPE
    {
        VT_SINGLE,        //!< 单值
        VT_ARRAY,        //!< 一维数组
        VT_ARRAY2        //!< 二维数组
    };

    //! @struct Value
    //! 配置项
    struct Value
    {
        std::string key;
        VALUE_TYPE type;
        std::string val_single;
        std::vector<std::string> val_array;
        std::vector<std::vector<std::string> > val_array2;
        std::string comment1;
        std::string comment2;
    };

    //! @struct Section
    //! 配置段
    struct Section
    {
        std::string section;
        std::string comment1;
        std::string comment2;
        std::vector<Value> values;
    };

private:
    //! 查找配置段
    //! @param section 段名称
    //! @param sec [out]Section指针
    //! @return 0:成功, -1失败
    int get_section(const char *section, Section *&sec);

    //! 查找配置项
    //! @param section 段名称
    //! @param key 项名称
    //! @param value [out]Value指针
    //! @return 0:成功, -1失败
    int get_value(const char *section, const char *key, Value *&value);

    //! 新增配置段
    //! @param section 段名称
    //! @param comment 段注释
    //! @return 0:成功, -1失败
    int add_section(const char *section, const char *comment1, const char *comment2);

    //! 新增配置项(单值)
    //! @param section 段名称
    //! @param key 项名称
    //! @param value 配置项值
    //! @param comment 项注释
    //! @return 0:成功, -1失败
    int add_value_single(const char *section, const char *key, const char *value, const char *comment1, const char *comment2);

    //! 新增配置项(一维数组)
    //! @param section 段名称
    //! @param key 项名称
    //! @param line 数据行
    //! @param comment 项注释
    //! @return 0:成功, -1失败
    int add_value_array(const char *section, const char *key, const char *line, const char *comment1, const char *comment2);

    //! 新增配置项(二维数组)
    //! @param section 段名称
    //! @param key 项名称
    //! @param lines 多个数据行
    //! @param comment 项注释
    //! @return 0:成功, -1失败
    int add_value_array2(const char *section, const char *key, const std::vector<std::string> &lines, const char *comment1, const char *comment2);

private:
    //! 配置数据
    std::vector<Section> m_configure;
};

#endif // _HC_TEXT_CONFIGURE_H_
