#include "hc_stack_trace.h"

#include "hc_os.h"
#include "hc_log.h"
#include "hc_thread_storage.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <WinSock2.h>
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

static threadc_storage m_stack_trace_ts_(&stack_trace::stack_trace_cleanup);

void stack_trace::stack_trace_cleanup(void* p)
{
    size_t* pi = static_cast<size_t*>(p);
    delete pi;
}

stack_trace::stack_trace(const char* file, size_t line, const char* func) : m_file_(file), m_line_(line), m_func_(func)
{
    size_t* indent = static_cast<size_t*>(m_stack_trace_ts_.thread_getspecific());
    if ( NULL == indent )
    {
        indent = new size_t(0);
        m_stack_trace_ts_.thread_setspecific(indent);
    }

    dump(*indent, "->", m_file_, m_line_, m_func_);

    (*indent)++;
}

stack_trace::~stack_trace()
{
    size_t* indent = static_cast<size_t*>(m_stack_trace_ts_.thread_getspecific());
    if ( NULL == indent )
    {
        indent = new size_t(0);
        m_stack_trace_ts_.thread_setspecific(indent);
    }

    (*indent)--;

    dump(*indent, "<-", m_file_, m_line_, m_func_);
}

void stack_trace::dump(size_t indent, const char* action, const char* file, size_t line, const char* func)
{
    const int buffer_len = 1024;
    char buffer[buffer_len] = { 0 };
    char* current = buffer;
    size_t remain_len = sizeof(buffer);

    while (indent-- > 0)
    {
        int copy_len = snprintf(current, remain_len, "  ");
        if ( copy_len < 0 )
        {
            buffer[buffer_len - 1] = '\0';
            break;
        }

        current += copy_len;
        remain_len -= copy_len;
    }

#ifdef WIN32
    snprintf(current, remain_len, "[TID:0x%08x] %s %s %s:%06u", (ssize_t)GetCurrentThreadId(), action, func, file, line);
#else
    snprintf(current, remain_len, "[TID:0x%08x] %s %s %s:%06u", (ssize_t)pthread_self(), action, func, file, line);
#endif
    LOG(TRACE)("%s", buffer);
}

