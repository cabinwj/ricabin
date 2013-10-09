#include "py27method.h"

#include "hc_py27embed.h"
#include <Python.h>

// test c++ call python sample
int main(int argc, char* argv[])
{
	py27init_module init_module;
    py27embed embed;
    embed.init(&init_module);
    PyObject* pyobj_tuple = NULL;
    // c++ call python
    if ( 0 != embed.call_extend("bdh_encome", "hunt_everyhour_encome", "O", &pyobj_tuple))
    {
        // todo log
        return -1;
    }

    if (NULL == pyobj_tuple)
    {
        // todo log
        return -1;
    }

    int pytuple_size = PyTuple_Size(pyobj_tuple);
    for (int idx = 0; idx < pytuple_size; ++idx)
    {
        int value = static_cast<int>( PyInt_AsLong( PyTuple_GetItem(pyobj_tuple, idx) ) );
        // todo log int
    }

    embed.release_result(pyobj_tuple);

    bool res = 0;
    user_t* user = new user_t;
    if (NULL == user)
    {
        // todo log
        return -1;
    }

    user->money_ = 0;
    int count = 99999;
    // c++ call python 
    if ( 0 != embed.call_extend("bdh_encome", "pyadd_money_cb", "bli", &res, user, count))
    {
        // todo log
        return -1;
    }

    // log user->money_ == 99999
    // log res return by python pyadd_money_cb function:  res == true || res == false

    // py27embed::release_result(res); local POD

    // c++ call python 
    if ( 0 != embed.call_extend("bdh_encome", "pyattr_list_cb"))
    {
        // todo log
        return -1;
    }

    return 0;
}

// python file