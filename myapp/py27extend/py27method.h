#ifndef _PY27METHOD_H_
#define _PY27METHOD_H_

#include "hc_py27embed.h"

struct user_t
{
    void add_money(int money) { money_ += money; }
    int money_;
};

class py27init_module : public ipy27init_module
{
public:
	virtual ~py27init_module() { }

public:
	virtual void init_methods();
};

#endif