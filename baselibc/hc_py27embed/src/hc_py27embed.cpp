#include "hc_py27embed.h"


int py27embed::init(ipy27init_module* init_module)
{
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        // todo log
        return -1;
    }

	if (NULL == init_module)
	{
		// todo log
		return -2;
	}

	// 导出的模块名，前面结构数组的指针
	init_module->init_methods();

    int rc = PyRun_SimpleString("import sys");
    rc = PyRun_SimpleString("sys.path.append('C:/Python27')");
    rc = PyRun_SimpleString("sys.path.append('C:/Python27/DLLS')");
    rc = PyRun_SimpleString("sys.path.append('C:/Python27/lib')");
    rc = PyRun_SimpleString("sys.path.append('C:/Python27/lib/site-packages')");

    return rc;
}

void py27embed::finalize()
{
    std::map<std::string, PyObject*>::iterator iter = m_pymodule_map_.begin();
    for ( ; m_pymodule_map_.end() != iter; iter++)
    {
        release_result(iter->second);
        iter->second = NULL;
    }
    
    Py_Finalize();
}

std::string py27embed::traceback()
{
    std::string traceback_str;
    PyObject* pyobj_type = NULL;
    PyObject* pyobj_value = NULL;
    PyObject* pyobj_traceback = NULL;

    if (NULL == PyErr_Occurred())
    {
        return traceback_str;
    }

    PyErr_Fetch(&pyobj_type, &pyobj_value, &pyobj_traceback);
    PyErr_Clear();
    if ( (NULL != pyobj_type) && (PyString_Check(pyobj_type)))  // PyObject_Str();
    {
        traceback_str += PyString_AsString(pyobj_type);
    }
    else
    {
        traceback_str += "<unknown exception type>";
    }

    if ( (NULL != pyobj_value) && (PyString_Check(pyobj_value)))
    {
        traceback_str += " : ";
        traceback_str += PyString_AsString(pyobj_value);
    }
    else
    {
        traceback_str += " : <unknown exception value> ";
    }

    if (NULL != pyobj_traceback)
    {
        PyObject* pyscript_module = load_script("traceback");
        if (NULL == pyscript_module)
        {
            // todo log
            release_result(pyobj_type);
            release_result(pyobj_value);
            release_result(pyobj_traceback);
            return traceback_str;
        }

        PyObject* pymodule_dict = module_dict("traceback");
        if (NULL == pymodule_dict)
        {
            // todo log
            release_result(pyobj_type);
            release_result(pyobj_value);
            release_result(pyobj_traceback);
            return traceback_str;
        }

        PyObject* pydict_func = PyDict_GetItemString(pymodule_dict, "format_exception");
        if (NULL == pydict_func)
        {
            // todo log
            release_result(pyobj_type);
            release_result(pyobj_value);
            release_result(pyobj_traceback);
            return traceback_str;
        }

        PyObject* pyerr_list = PyObject_CallFunctionObjArgs(pydict_func, pyobj_type, pyobj_value, pyobj_traceback, NULL);
        if (NULL != pyerr_list)
        {
            int pylist_size = PyList_Size(pyerr_list);
            for (int i = 0; i < pylist_size; i++)
            {
                traceback_str += PyString_AsString(PyList_GetItem(pyerr_list, i));
            }

            release_result(pyerr_list);
        }

        release_result(pyobj_type);
        release_result(pyobj_value);
        release_result(pyobj_traceback);
    }

    return traceback_str;
}

void py27embed::acquire_result(PyObject* res_object)
{
    Py_XINCREF(res_object);
}

void py27embed::release_result(PyObject* res_object)
{
    Py_XDECREF(res_object);
}

PyObject* py27embed::load_script(const std::string& file_name, bool is_reload)
{
    PyObject* pyscript_module = NULL;
    std::map<std::string, PyObject*>::iterator iter = m_pymodule_map_.find(file_name);
    if (m_pymodule_map_.end() == iter)
    {
        pyscript_module = PyImport_ImportModule(file_name.c_str());
        if (NULL == pyscript_module)
        {
            // todo log
            traceback();
            return NULL;
        }

        m_pymodule_map_[file_name] = pyscript_module;
        //std::string pyimport_cmd = "import " + file_name;
        //int rc = PyRun_SimpleString(pyimport_cmd.c_str());

        return pyscript_module;
    }

    if (NULL == iter->second)
    {
        pyscript_module = PyImport_ImportModule(file_name.c_str());
        if (NULL == pyscript_module)
        {
            // todo log
            traceback();
            m_pymodule_map_.erase(iter);
            return NULL;
        }

        m_pymodule_map_[file_name] = pyscript_module;
        //std::string pyimport_cmd = "import " + file_name;
        //int rc = PyRun_SimpleString(pyimport_cmd.c_str());

        return pyscript_module;
    }

    if (is_reload)
    {
        pyscript_module = PyImport_ReloadModule(iter->second);
        if (NULL == pyscript_module)
        {
            // todo log
            traceback();
            release_result(iter->second);
            iter->second = NULL;
            m_pymodule_map_.erase(iter);
            return NULL;
        }

        release_result(iter->second);
        iter->second = NULL;

        m_pymodule_map_[file_name] = pyscript_module;
        //std::string pyimport_cmd = "import " + file_name;
        //int rc = PyRun_SimpleString(pyimport_cmd.c_str());

        return pyscript_module;
    }

    return iter->second;
}

PyObject* py27embed::module_dict(const std::string& file_name)
{
    std::map<std::string, PyObject*>::iterator iter = m_pymodule_map_.find(file_name);
    if (m_pymodule_map_.end() == iter)
    {
        // todo log
        return NULL;
    }

    if (NULL == iter->second)
    {
        // todo log
        m_pymodule_map_.erase(iter);
        return NULL;
    }

    return PyModule_GetDict(iter->second);
}

int py27embed::call_extend(const std::string& file_name, const std::string& func_name,
                           const std::string& va_fmt, void* res_object, ...)
{
    PyObject* pymodule_dict = module_dict(file_name);
    if (NULL == pymodule_dict)
    {
        // todo log
        PyObject* pyscript_module = load_script(file_name);
        if (NULL == pyscript_module)
        {
            // todo log
            return -1;
        }
    }

    pymodule_dict = module_dict(file_name);
    if (NULL == pymodule_dict)
    {
        // todo log
        return -1;
    }

    PyObject* pydict_func = PyDict_GetItemString(pymodule_dict, func_name.c_str());
    if ((NULL == pydict_func) || (0 == PyCallable_Check(pydict_func)))
    {
        // todo log
        traceback();
        return -1;
    }

    PyObject* pyresult = NULL;
    PyObject* pyobj_args = NULL;
    if ( 0 == va_fmt.size() )
    {
        pyresult = PyObject_CallObject(pydict_func, pyobj_args);
        if (NULL == pyresult)
        {
            // todo log
            traceback();
            return -1;
        }

        return 0;
    }

    std::string va_fmt_res = va_fmt.substr(0, 1);
    if ( va_fmt.size() > 1 )
    {
        std::string va_fmt_new = va_fmt.substr(1, va_fmt.size() - 1);
        va_list va_params_list;
        va_start(va_params_list, res_object);
        pyobj_args = Py_VaBuildValue(va_fmt_new.c_str(), va_params_list);
        va_end(va_params_list);
    }

    pyresult = PyObject_CallObject(pydict_func, pyobj_args);
    release_result(pyobj_args);
    if (NULL == pyresult)
    {
        // todo log
        traceback();
        return -1;
    }
    
    //if (!PyArg_ParseTuple(pyresult, va_fmt_res.c_str(), res_object))
    if (!PyArg_Parse(pyresult, va_fmt_res.c_str(), res_object))
    {
        // todo log
        traceback();
        return -1;
    }

    return 0;
}

