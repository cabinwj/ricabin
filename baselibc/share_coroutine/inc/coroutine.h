#ifndef _CORO_COROUTINE_H_
#define _CORO_COROUTINE_H_

#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#endif

extern "C" {
#include "coro.h"
}

#include "common_os.h"
#include "common_log.h"
#include "common_list.h"

#include <list>
#include <map>

typedef struct coro_context coro_context;

class coro_pool
{
public:
    std::map<size_t, std::list<coro_context*> > m_coro_pool_;

public:
    coro_context* allocate(size_t size);
    void deallocate(coro_context*, size_t size);
    static coro_pool* instance();
};

class coro_scheduler;

class coroutine
{
public:
    enum
    {
        DEFAULT_STACK_SIZE = 8 * 1024
    };

public:
    // ����������Զ����б�
    list_head m_tasks_item_;

    // ����ͨ��netid�����б�
    list_head m_net_item_;

    // ����sequence�����б�
    list_head m_sequence_item_;

private:
    // �������ü�����ȷ��Э�̰�ȫ����
    int32_t m_reference_count_;

    // Э���Ƿ�����Э�̺������н����󲻻��������٣�ֻ��������ǣ�������Э�̽�������
    bool m_is_zombie_;

    // Э���Ƿ�����������У�����������У�����Э��
    bool m_is_scheduling_;

    // Э��ջ�ռ��С
    size_t m_stack_size_;

    // Э�̵�sequence
    uint32_t m_sequence_id_;

    //// Э�̵�ͨ��netid
    //uint32_t m_net_id_;

    // Э�̵��첽ͨѶ����
    void* m_io_result_;

protected:
    // Э�̡��ֳ���
#ifdef WIN32
    LPVOID m_fiber_;
#else
    coro_context* m_coro_context_;
#endif

protected:
    friend class coro_scheduler;

public:
    coroutine(size_t stack_size = DEFAULT_STACK_SIZE);
    virtual ~coroutine();

public:
    // Call() ��װ�� �û��Զ����Ӧ�ú�����
    virtual void Call() = 0;

public:
    // ���ü���+1
    inline void add_reference() { ++m_reference_count_; }

    // ���ü���-1
    inline void release() { --m_reference_count_; if (0 == m_reference_count_){ delete this; } }

public:
    // ����Э�̵��첽ͨѶ���� 
    inline void set_io_result(void* rc) { m_io_result_ = rc; }
    // ȡЭ�̵��첽ͨѶ����
    inline void* get_io_result() { return m_io_result_; }

    // ����Э�̵��Ƿ���
    inline void set_zombie(bool zombie) { m_is_zombie_ = zombie; }
    // ȡЭ�̵Ľ���״̬
    inline bool is_zombie() { return m_is_zombie_; }
    // ����Э���Ƿ������������
    inline void set_scheduling(bool scheduling) { m_is_scheduling_ = scheduling; }
    // ȡЭ�����������״̬
    inline bool is_scheduling() { return m_is_scheduling_; }
    // ����Э�̵�sequence
    inline void set_sequence_id(uint32_t sequence_id) { m_sequence_id_ = sequence_id; }
    // ȡЭ�̵�sequence
    inline uint32_t get_sequence_id() { return m_sequence_id_; }
    //// ����Э�̵�net_id
    //inline void set_net_id(uint32_t net_id) { m_net_id_ = net_id; }
    //// ȡЭ�̵�net_id
    //inline uint32_t get_net_id() { return m_net_id_; }
 
protected:
    // proc()��Э��ִ�к��������뺯����
    // �����ó����ִ����ɺ� ���ý������л���Э��
#ifdef WIN32
    static VOID WINAPI proc(PVOID pv_param);
#else
    static void proc(void* ctx);
#endif
};


template <class T>
class CoroutineT
{
};

template <>
class CoroutineT<void()> : public coroutine
{
    void (*func)();
public:
    CoroutineT(void(*func_)()) : func(func_) {}
    CoroutineT(size_t n, void(*func_)()) : coroutine(n), func(func_) {}
    virtual void Call() {
        func();
    }
};

template <>
class CoroutineT<void(*)()> : public coroutine
{
    void (*func)();
public:
    CoroutineT(void(*func_)()) : func(func_) {}
    CoroutineT(size_t n, void(*func_)()) : coroutine(n), func(func_) {}
    virtual void Call() {
        func();
    } 
};

template <class T>
class CoroutineT<void(T::*)()> : public coroutine
{
    void (T::*func)();
    T& obj;
public:
    CoroutineT(void(T::*func_)(), T& obj_) : func(func_), obj(obj_) {}
    CoroutineT(size_t n, void(T::*func_)(), T& obj_) : coroutine(n), func(func_), obj(obj_) {}
    virtual void Call() {
        (obj.*func)();
    }
};

template <class T1>
class CoroutineT<void(T1)> : public coroutine
{
    void (*func)(T1);
    T1 v1;
public:
    CoroutineT(void(*func_)(T1), T1 v1_) : func(func_), v1(v1_) {}
    CoroutineT(size_t n, void(*func_)(T1), T1 v1_) : coroutine(n), func(func_), v1(v1_) {}
    virtual void Call() {
        func(v1);
    }   
};

template <class T1>
class CoroutineT<void(*)(T1)> : public coroutine
{
    void (*func)(T1);
    T1 v1;
public:
    CoroutineT(void(*func_)(T1), T1 v1_) : func(func_), v1(v1_) {}
    CoroutineT(size_t n, void(*func_)(T1), T1 v1_) : coroutine(n), func(func_), v1(v1_) {}
    virtual void Call() {
        func(v1);
    }  
};

template <class T, class T1>
class CoroutineT<void(T::*)(T1)> : public coroutine
{
    void (T::*func)(T1);
    T& obj;
    T1 v1;
public:
    CoroutineT(void(T::*func_)(T1), T& obj_, T1 v1_) : func(func_), obj(obj_), v1(v1_) {}
    CoroutineT(size_t n, void(T::*func_)(T1), T& obj_, T1 v1_) : coroutine(n), func(func_), obj(obj_), v1(v1_) {}
    virtual void Call() {
        (obj.*func)(v1);
    }
};

template <class T1, class T2>
class CoroutineT<void(T1, T2)> : public coroutine
{
    void (*func)(T1, T2);
    T1 v1;
    T2 v2;
public:
    CoroutineT(void(*func_)(T1, T2), T1 v1_, T2 v2_) : func(func_), v1(v1_), v2(v2_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2), T1 v1_, T2 v2_) : coroutine(n), func(func_), v1(v1_), v2(v2_) {}

    virtual void Call() {
        func(v1, v2);
    }  
};

template <class T1, class T2>
class CoroutineT<void(*)(T1, T2)> : public coroutine
{
    void (*func)(T1, T2);
    T1 v1;
    T2 v2;
public:
    CoroutineT(void(*func_)(T1, T2), T1 v1_, T2 v2_) : func(func_), v1(v1_), v2(v2_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2), T1 v1_, T2 v2_) : coroutine(n), func(func_), v1(v1_), v2(v2_) {}

    virtual void Call() {
        func(v1, v2);
    }
};

template <class T, class T1, class T2>
class CoroutineT<void(T::*)(T1, T2)> : public coroutine
{
    void (T::*func)(T1, T2);
    T& obj;
    T1 v1;
    T2 v2;
public:
    CoroutineT(void(T::*func_)(T1, T2), T& obj_, T1 v1_, T2 v2_) : func(func_), obj(obj_), v1(v1_), v2(v2_) {}
    CoroutineT(size_t n, void(T::*func_)(T1, T2), T& obj_, T1 v1_, T2 v2_) : coroutine(n), func(func_), obj(obj_), v1(v1_), v2(v2_) {}

    virtual void Call() {
        (obj.*func)(v1, v2);
    }
};

template <class T1, class T2, class T3>
class CoroutineT<void(T1, T2, T3)> : public coroutine
{
    void (*func)(T1, T2, T3);
    T1 v1;
    T2 v2;
    T3 v3;
public:
    CoroutineT(void(*func_)(T1, T2, T3), T1 v1_, T2 v2_, T3 v3_)
        : func(func_), v1(v1_), v2(v2_), v3(v3_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2, T3), T1 v1_, T2 v2_, T3 v3_)
        : coroutine(n), func(func_), v1(v1_), v2(v2_), v3(v3_) {}

    virtual void Call() {
        func(v1, v2, v3);
    }
};

template <class T1, class T2, class T3>
class CoroutineT<void(*)(T1, T2, T3)> : public coroutine
{
    void (*func)(T1, T2, T3);
    T1 v1;
    T2 v2;
    T3 v3;
public:
    CoroutineT(void(*func_)(T1, T2, T3), T1 v1_, T2 v2_, T3 v3_)
        : func(func_), v1(v1_), v2(v2_), v3(v3_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2, T3), T1 v1_, T2 v2_, T3 v3_)
        : coroutine(n), func(func_), v1(v1_), v2(v2_), v3(v3_) {}

    virtual void Call() {
        func(v1, v2, v3);
    }
};

template <class T, class T1, class T2, class T3>
class CoroutineT<void(T::*)(T1, T2, T3)> : public coroutine
{
    void (T::*func)(T1, T2, T3);
    T& obj;
    T1 v1;
    T2 v2;
    T3 v3;
public:
    CoroutineT(void(T::*func_)(T1, T2, T3), T& obj_, T1 v1_, T2 v2_, T3 v3_)
        : func(func_), obj(obj_), v1(v1_), v2(v2_), v3(v3_) {}
    CoroutineT(size_t n, void(T::*func_)(T1, T2, T3), T& obj_, T1 v1_, T2 v2_, T3 v3_)
        : coroutine(n), func(func_), obj(obj_), v1(v1_), v2(v2_), v3(v3_) {}

    virtual void Call() {
        (obj.*func)(v1, v2, v3);
    }
};

template <class T1, class T2, class T3, class T4>
class CoroutineT<void(T1, T2, T3, T4)> : public coroutine
{
    void (*func)(T1, T2, T3, T4);
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
public:
    CoroutineT(void(*func_)(T1, T2, T3, T4), T1 v1_, T2 v2_, T3 v3_, T4 v4_)
        : func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2, T3, T4), T1 v1_, T2 v2_, T3 v3_, T4 v4_)
        : coroutine(n), func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_) {}

    virtual void Call() {
        func(v1, v2, v3, v4);
    }
};

template <class T1, class T2, class T3, class T4>
class CoroutineT<void(*)(T1, T2, T3, T4)> : public coroutine
{
    void (*func)(T1, T2, T3, T4);
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
public:
    CoroutineT(void(*func_)(T1, T2, T3, T4), T1 v1_, T2 v2_, T3 v3_, T4 v4_)
        : func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2, T3, T4), T1 v1_, T2 v2_, T3 v3_, T4 v4_)
        : coroutine(n), func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_) {}

    virtual void Call() {
        func(v1, v2, v3, v4);
    }
};

template <class T, class T1, class T2, class T3, class T4>
class CoroutineT<void(T::*)(T1, T2, T3, T4)> : public coroutine
{
    void (T::*func)(T1, T2, T3, T4);
    T& obj;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
public:
    CoroutineT(void(T::*func_)(T1, T2, T3, T4), T& obj_, T1 v1_, T2 v2_, T3 v3_, T4 v4_)
        : func(func_), obj(obj_), v1(v1_), v2(v2_), v3(v3_), v4(v4_) {}
    CoroutineT(size_t n, void(T::*func_)(T1, T2, T3, T4), T& obj_, T1 v1_, T2 v2_, T3 v3_, T4 v4_)
        : coroutine(n), func(func_), obj(obj_), v1(v1_), v2(v2_), v3(v3_), v4(v4_) {}

    virtual void Call() {
        (obj.*func)(v1, v2, v3, v4);
    }
};

template <class T1, class T2, class T3, class T4, class T5>
class CoroutineT<void(T1, T2, T3, T4, T5)> : public coroutine
{
    void (*func)(T1, T2, T3, T4, T5);
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;
public:
    CoroutineT(void(*func_)(T1, T2, T3, T4, T5), T1 v1_, T2 v2_, T3 v3_, T4 v4_, T5 v5_)
        : func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_), v5(v5_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2, T3, T4, T5), T1 v1_, T2 v2_, T3 v3_, T4 v4_, T5 v5_)
        : coroutine(n), func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_), v5(v5_) {}

    virtual void Call() {
        func(v1, v2, v3, v4, v5);
    }
};

template <class T1, class T2, class T3, class T4, class T5>
class CoroutineT<void(*)(T1, T2, T3, T4, T5)> : public coroutine
{
    void (*func)(T1, T2, T3, T4, T5);
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;
public:
    CoroutineT(void(*func_)(T1, T2, T3, T4, T5), T1 v1_, T2 v2_, T3 v3_, T4 v4_, T5 v5_)
        : func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_), v5(v5_) {}
    CoroutineT(size_t n, void(*func_)(T1, T2, T3, T4, T5), T1 v1_, T2 v2_, T3 v3_, T4 v4_, T5 v5_)
        : coroutine(n), func(func_), v1(v1_), v2(v2_), v3(v3_), v4(v4_), v5(v5_) {}

    virtual void Call() {
        func(v1, v2, v3, v4, v5);
    }
};

template <class T, class T1, class T2, class T3, class T4, class T5>
class CoroutineT<void(T::*)(T1, T2, T3, T4, T5)> : public coroutine
{
    void (T::*func)(T1, T2, T3, T4, T5);
    T& obj;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;
public:
    CoroutineT(void(T::*func_)(T1, T2, T3, T4, T5), T& obj_, T1 v1_, T2 v2_, T3 v3_, T4 v4_, T5 v5_)
        : func(func_), obj(obj_), v1(v1_), v2(v2_), v3(v3_), v4(v4_), v5(v5_) {}
    CoroutineT(size_t n, void(T::*func_)(T1, T2, T3, T4, T5), T& obj_, T1 v1_, T2 v2_, T3 v3_, T4 v4_, T5 v5_)
        : coroutine(n), func(func_), obj(obj_), v1(v1_), v2(v2_), v3(v3_), v4(v4_), v5(v5_) {}

    virtual void Call() {
        (obj.*func)(v1, v2, v3, v4, v5);
    }
};

#endif // _CORO_COROUTINE_H_
