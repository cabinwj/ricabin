#ifndef _COMMON_NONCOPYABLE_H_
#define _COMMON_NONCOPYABLE_H_

/* 
 * brief ����Ҫ�󲻿ɸ�ֵ(��������)���඼���Լ̳��ڴ�
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
