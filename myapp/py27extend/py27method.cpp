#include "py27method.h"

#include <vector>

PyObject* add_money_ex(PyObject* self, PyObject* args)
{
    //std::vector<PyObject*> pyobj_holder;
    //py27object_hoder myholder(pyobj_holder);

    long user_addr = 0;
    int money = 0;

    if (!PyArg_ParseTuple(args, "li", &user_addr, &money))
    {
        Py_RETURN_FALSE;
    }

    user_t* xuser = (user_t*)user_addr;
    if ((NULL == xuser) || (NULL == money))
    {
        Py_RETURN_FALSE;
    }

    xuser->add_money(money);

    Py_RETURN_TRUE;
}

PyObject* attr_list_ex(PyObject* self, PyObject* args)
{
    //std::vector<PyObject*> pyobj_holder;
    //py27object_hoder myholder(pyobj_holder);

    PyObject* pylist = PyList_New(2);
    //pyobj_holder.push_back(pylist);
    PyList_SetItem(pylist, 0, Py_BuildValue("s", "wangxianming"));
    PyList_SetItem(pylist, 1, Py_BuildValue("s", "wangxianming01"));

    return pylist;
}

PyMethodDef bdh_methods[] = 
{
    //导出到Python中的名字、函数指针，参数类型，函数描述
    {"add_money", add_money_ex, METH_VARARGS, "descript call user_t::add_money_ex" },
    //结束
    {NULL, NULL, 0, NULL}
};

PyMethodDef wxm_methods[] = 
{
	//导出到Python中的名字、函数指针，参数类型，函数描述
	{"attr_list", attr_list_ex, METH_VARARGS, "descript call return a list" },
	//结束
	{NULL, NULL, 0, NULL}
};

// 可以针对已经归类的方法，导出模块名
void py27init_module::init_methods()
{
	// 导出的模块名，前面结构数组的指针
	Py_InitModule("bdh", bdh_methods);
	Py_InitModule("wxm", wxm_methods);
}