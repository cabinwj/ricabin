#ifndef _HC_OBJECT_GUARD_H_
#define _HC_OBJECT_GUARD_H_

#include "hc_object_allocator.h"
#include "hc_destroyable.h"


template <class T>
class object_guard : public destroy_callback
{
public:
    object_guard(object_allocator<T>* object_alloc)
        : m_object_allocator_(object_alloc)
    {
        if ( NULL == m_object_allocator_ )
        {
            m_object_allocator_ = new object_single_allocator<T>;
        }
    }

public:
    T* Create()
    {
        T* object = m_object_allocator_->Create();
        if ( NULL != object)
        {
            object->set_destroy_callback(this);
        }

        return object;
    }

    virtual void Call(destroyable* object)
    {
        return m_object_allocator_->Destroy(static_cast<T*>(object));
    }

private:
    object_allocator<T>* m_object_allocator_;
};

#endif // _HC_OBJECT_GUARD_H_
