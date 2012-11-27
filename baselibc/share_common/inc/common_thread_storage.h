#ifndef _THREAD_STORAGE_H_
#define _THREAD_STORAGE_H_

#include "common_os.h"
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#ifdef WIN32
typedef DWORD pthread_key_t;
#endif
typedef void(*destroy_func_t)(void*);

//! @class threadc_storage
//! @brief œﬂ≥Ã¿‡
class threadc_storage
{
public:
    threadc_storage() { }
    virtual ~threadc_storage() { }

public:
    int thread_key_create(void (*destroy_callback)(void*))
    {
#ifdef WIN32
        // Semantics are: we create a new key, and then promise to call
        // destroy_callback with TlsGetValue(key) when the thread is destroyed
        // (as long as TlsGetValue(key) is not NULL).
        pthread_key_t tkey = TlsAlloc();
        if (tkey != TLS_OUT_OF_INDEXES)
        {
            m_key_ = tkey;
            return 0;
        }
        else
        {
            return GetLastError();
        }
#else
        return pthread_key_create(&m_key_, destroy_callback);
#endif
    }

    int thread_key_create(pthread_key_t* key, void (*destroy_callback)(void*))
    {
#ifdef WIN32
        // Semantics are: we create a new key, and then promise to call
        // destr_fn with TlsGetValue(key) when the thread is destroyed
        // (as long as TlsGetValue(key) is not NULL).
        pthread_key_t tkey = TlsAlloc();
        if (tkey != TLS_OUT_OF_INDEXES)
        {
            *(key) = tkey;
            return 0;
        }
        else
        {
            return GetLastError();
        }
#else
        return pthread_key_create(key, destroy_callback);
#endif
    }

    void* thread_getspecific()
    {
#ifdef WIN32
        DWORD err = GetLastError();
        void* rv = TlsGetValue(m_key_);
        if (err)
        {
            SetLastError(err);
        }
        return rv;
#else
        return pthread_getspecific(m_key_);
#endif
    }

    void* thread_getspecific(pthread_key_t key)
    {
#ifdef WIN32
        DWORD err = GetLastError();
        void* rv = TlsGetValue(key);
        if (err)
        {
            SetLastError(err);
        }
        return rv;
#else
        return pthread_getspecific(key);
#endif
    }

    int thread_setspecific(void* value)
    {
#ifdef WIN32
        if (TlsSetValue(m_key_, (LPVOID)value))
        {
            return 0;
        }
        else
        {
            return GetLastError();
        }
#else
        return pthread_setspecific(m_key_, value);
#endif
    }

    int thread_setspecific(pthread_key_t key, void* value)
    {
#ifdef WIN32
        if (TlsSetValue(key, (LPVOID)value))
        {
            return 0;
        }
        else
        {
            return GetLastError();
        }
#else
        return pthread_setspecific(key, value);
#endif
    }

private:
    pthread_key_t m_key_;
};


#endif // _THREAD_STORAGE_H_
