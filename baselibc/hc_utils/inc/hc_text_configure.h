//! @brief ������
#ifndef _HC_TEXT_CONFIGURE_H_
#define _HC_TEXT_CONFIGURE_H_


#include <string>
#include <vector>

//! @class configure
//! @brief ������
//!
//! <pre>
//! ����ʵ��һ�� section - key ��ʽ��������
//! �����ļ�����һ�� [section:key] ��Ӧһ������ֵ(value)
//! ����ֵ(value)��������ʽ single, array, array2
//!     single: �����ַ���
//!     array: ����ַ�������һά����
//!     array2: ����ַ������ɶ�ά����
//! �����ļ����磺
//!
//! # �����������
//! [admin_server]
//! admin_addr = (127.0.0.1 8012)    # array
//! reconnect_interval = 10            # single
//!
//! # ��Ⱥ��������
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
    //! ��������
    //! @param file �����ļ���
    //! @return 0:�ɹ�, -1ʧ��
    int load(const char *file);

    //! ȡ��ֵ����
    //! @param section ������
    //! @param key ������
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_single(const char *section, const char *key, std::string &val);

    //! ȡ��ֵ����
    //! @param section ������
    //! @param key ������
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_single(const char *section, const char *key, long &val);

    //! ȡ��ֵ����
    //! @param section ������
    //! @param key ������
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_single(const char *section, const char *key, double &val);

    //! ȡһά��������Ԫ�ظ���
    //! @param section ������
    //! @param key ������
    //! @param count [out]Ԫ�ظ���
    //! @return 0:�ɹ�, -1ʧ��
    int count1_array(const char *section, const char *key, int &count);

    //! ȡһά��������
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ���±�
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_array(const char *section, const char *key, int i, std::string &val);

    //! ȡһά��������
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ���±�
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_array(const char *section, const char *key, int i, long &val);

    //! ȡһά��������
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ���±�
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_array(const char *section, const char *key, int i, double &val);

    //! ȡ��ά��������Ԫ�ص�һά����
    //! @param section ������
    //! @param key ������
    //! @param count [out]��һά����
    //! @return 0:�ɹ�, -1ʧ��
    int count1_array2(const char *section, const char *key, int &count);

    //! ȡ��ά��������Ԫ�صڶ�ά����
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ�ص�һά�±�
    //! @param count [out]�ڶ�ά����
    //! @return 0:�ɹ�, -1ʧ��
    int count2_array2(const char *section, const char *key, int i, int &count);

    //! ȡ��ά��������
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ�ص�һά�±�
    //! @param j Ԫ�صڶ�ά�±�
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_array2(const char *section, const char *key, int i, int j, std::string &val);

    //! ȡ��ά��������
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ�ص�һά�±�
    //! @param j Ԫ�صڶ�ά�±�
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_array2(const char *section, const char *key, int i, int j, long &val);

    //! ȡ��ά��������
    //! @param section ������
    //! @param key ������
    //! @param i Ԫ�ص�һά�±�
    //! @param j Ԫ�صڶ�ά�±�
    //! @param val [out]����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int get_array2(const char *section, const char *key, int i, int j, double &val);

public:
    //! ��������
    //! @param file �����ļ���
    //! @return 0:�ɹ�, -1ʧ��
    int save(const char *file);

    //! �޸ĵ�ֵ����
    //! @param section ������
    //! @param key ������
    //! @param val ����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int set_single(const char *section, const char *key, const std::string &val);

    //! �޸�һά��������
    //! @param section ������
    //! @param key ������
    //! @param val ����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int set_array(const char *section, const char *key, const std::vector<std::string> &val);

    //! �޸Ķ�ά��������
    //! @param section ������
    //! @param key ������
    //! @param val ����ֵ
    //! @return 0:�ɹ�, -1ʧ��
    int set_array2(const char *section, const char *key, const std::vector<std::vector<std::string> > &val);

private:
    //! @enum VALUE_TYPE
    //! @brief ����ֵ����
    enum VALUE_TYPE
    {
        VT_SINGLE,        //!< ��ֵ
        VT_ARRAY,        //!< һά����
        VT_ARRAY2        //!< ��ά����
    };

    //! @struct Value
    //! ������
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
    //! ���ö�
    struct Section
    {
        std::string section;
        std::string comment1;
        std::string comment2;
        std::vector<Value> values;
    };

private:
    //! �������ö�
    //! @param section ������
    //! @param sec [out]Sectionָ��
    //! @return 0:�ɹ�, -1ʧ��
    int get_section(const char *section, Section *&sec);

    //! ����������
    //! @param section ������
    //! @param key ������
    //! @param value [out]Valueָ��
    //! @return 0:�ɹ�, -1ʧ��
    int get_value(const char *section, const char *key, Value *&value);

    //! �������ö�
    //! @param section ������
    //! @param comment ��ע��
    //! @return 0:�ɹ�, -1ʧ��
    int add_section(const char *section, const char *comment1, const char *comment2);

    //! ����������(��ֵ)
    //! @param section ������
    //! @param key ������
    //! @param value ������ֵ
    //! @param comment ��ע��
    //! @return 0:�ɹ�, -1ʧ��
    int add_value_single(const char *section, const char *key, const char *value, const char *comment1, const char *comment2);

    //! ����������(һά����)
    //! @param section ������
    //! @param key ������
    //! @param line ������
    //! @param comment ��ע��
    //! @return 0:�ɹ�, -1ʧ��
    int add_value_array(const char *section, const char *key, const char *line, const char *comment1, const char *comment2);

    //! ����������(��ά����)
    //! @param section ������
    //! @param key ������
    //! @param lines ���������
    //! @param comment ��ע��
    //! @return 0:�ɹ�, -1ʧ��
    int add_value_array2(const char *section, const char *key, const std::vector<std::string> &lines, const char *comment1, const char *comment2);

private:
    //! ��������
    std::vector<Section> m_configure;
};

#endif // _HC_TEXT_CONFIGURE_H_
