#ifndef _COMMON_NONCOPYABLE_H_
#define _COMMON_NONCOPYABLE_H_

/* 
 * brief 所有要求不可赋值(拷贝构造)的类都可以继承于此
 */
class non_copyable
{
protected:
    non_copyable() {}
    ~non_copyable() {}

private:
    non_copyable(const non_copyable &);
    const non_copyable & operator=(const non_copyable &);
};

#endif // _COMMON_NONCOPYABLE_H_
