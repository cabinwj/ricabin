#include "common_allocator.h"

#include <stdlib.h>
#include <new>


void* malloc_allocator::Allocate(size_t size, size_t* real_size)
{
    if ( NULL != real_size )
    {
        *real_size = size;
    }

    return malloc(size);
}

void malloc_allocator::Deallocate(void* data)
{
    return free(data);
}

malloc_allocator* malloc_allocator::Instance()
{
    static malloc_allocator __alloc;
    return &__alloc;
}

void* new_allocator::Allocate(size_t size, size_t* real_size)
{
    if ( NULL != real_size )
    {
        *real_size = size;
    }

    return ::operator new(size, ::std::nothrow_t());
}

void new_allocator::Deallocate(void* data)
{
    return ::operator delete(data, ::std::nothrow_t());
}

new_allocator* new_allocator::Instance()
{
    static new_allocator __alloc;
    return &__alloc;
}

allocatorc* allocatorc::Instance()
{
    return new_allocator::Instance();
}
