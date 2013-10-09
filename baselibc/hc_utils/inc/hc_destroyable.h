#ifndef _HC_DESTROYABLE_H_
#define _HC_DESTROYABLE_H_

#include "hc_types.h"

class destroyable;
class idestroy_callback
{
public:
    virtual ~idestroy_callback() {}
    virtual void Call(destroyable*) = 0;
};

class destroyable
{
public:
    destroyable() : m_destroy_cb_(NULL) {}
    virtual ~destroyable() {}

public:
    inline void Destroy() { if ( NULL != m_destroy_cb_ ) m_destroy_cb_->Call(this); }
    inline void destroy_callback_o(idestroy_callback* cb) { m_destroy_cb_ = cb; }

private:
    idestroy_callback* m_destroy_cb_;
};

#endif // _HC_DESTROYABLE_H_
