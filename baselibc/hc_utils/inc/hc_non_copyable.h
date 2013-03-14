#ifndef _HC_NON_COPYABLE_H_
#define _HC_NON_COPYABLE_H_

// 所有要求不可赋值(拷贝构造)的类都可以继承于此
class non_copyable
{
protected:
    non_copyable() {}
    ~non_copyable() {}

private:
    non_copyable(const non_copyable&);
    const non_copyable& operator=(const non_copyable&);
};

#endif // _HC_NON_COPYABLE_H_
