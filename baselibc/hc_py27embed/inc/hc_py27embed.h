#ifndef _HC_PY27EMBED_H_
#define _HC_PY27EMBED_H_

#include <Python.h>
#include <string>
#include <vector>
#include <map>

// python ģ���� �����࣬��������Ѿ�����ķ�����������Ӧ��ģ����
class ipy27init_module
{
public:
	virtual ~ipy27init_module() { }

public:
	// ������ģ������ǰ��ṹ�����ָ�룺Py_InitModule("bdh", bdh_methods); 
	virtual void init_methods() = 0;
};

class py27embed
{
public:
    int init(ipy27init_module* init_module);
    void finalize();
    void acquire_result(PyObject* res_object);
    void release_result(PyObject* res_object);

public:
    PyObject* load_script(const std::string& file_name, bool is_reload = false);
    PyObject* module_dict(const std::string& file_name);

public:
    int call_extend(const std::string& file_name, const std::string& func_name,
                    const std::string& va_fmt = "", void* res_object = NULL, ...);

private:
    std::string traceback();

private:
    // �������е� python �ű��� Module �Ա��� ModuleDict 
    std::map<std::string, PyObject*> m_pymodule_map_;
};

class py27object_hoder
{
public:
    py27object_hoder(std::vector<PyObject*>& pyobj_holder) : m_py27obj_holder_(pyobj_holder)
    {
    }

    ~py27object_hoder()
    {
        std::vector<PyObject*>::iterator iter = m_py27obj_holder_.begin();
        for ( ; m_py27obj_holder_.end() != iter; iter++ )
        {
            if ( NULL != *iter )
            {
                Py_XDECREF(*iter);
                *iter = NULL;
            }
        }
    }

private:
    std::vector<PyObject*>& m_py27obj_holder_;
};

#endif