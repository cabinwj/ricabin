#ifndef _HC_ALLOCATOR_H_
#define _HC_ALLOCATOR_H_

#include "hc_types.h"


class allocatorc
{
public:
    virtual ~allocatorc() {}
    virtual void* Allocate(size_t size, size_t* real_size=NULL) = 0;
    virtual void Deallocate(void* data) = 0;

public:
    static allocatorc* Instance();
};

class malloc_allocator : public allocatorc
{
public:
    virtual void* Allocate(size_t size, size_t* real_size=NULL);
    virtual void Deallocate(void* data);

public:
    static malloc_allocator* Instance();
};

class new_allocator : public allocatorc
{
public:
    virtual void* Allocate(size_t size, size_t* real_size=NULL);
    virtual void Deallocate(void* data);

public:
    static new_allocator* Instance();
};

#endif // _HC_ALLOCATOR_H_
