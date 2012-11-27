#ifndef _COMMON_DESTROYABLE_H_
#define _COMMON_DESTROYABLE_H_

#include "common_os.h"

class destroyable;
class destroy_callback
{
public:
    virtual ~destroy_callback() {}
    virtual void Call(destroyable*) = 0;
};

class destroyable
{
public:
    virtual ~destroyable() {}

public:
    void Destroy() { if ( NULL != m_destroy_callback_ ) m_destroy_callback_->Call(this); }

public:
    inline void set_destroy_callback(destroy_callback* cb) { m_destroy_callback_ = cb; }

private:
    destroy_callback* m_destroy_callback_;
};

#endif // _COMMON_DESTROYABLE_H_
