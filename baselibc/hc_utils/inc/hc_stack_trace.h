#ifndef _HC_STACK_TRACE_H_
#define _HC_STACK_TRACE_H_


class stack_trace
{
public:
    stack_trace(const char* file, size_t line, const char* func);
    ~stack_trace();

public:
    static void stack_trace_cleanup(void* p);

private:
    void dump(size_t indent, const char* action, const char* file, size_t line, const char* func);

private:
    const char* m_file_;
    size_t      m_line_;
    const char* m_func_;
};

#ifdef WIN32
#define STACK_TRACE_LOG() stack_trace __##trace_##__LINE__(__FILE__, __LINE__, __FUNCTION__)
#else
#define STACK_TRACE_LOG() stack_trace __##trace_##__LINE__(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

#endif // _HC_STACK_TRACE_H_

