#ifndef _HC_NON_COPYABLE_H_
#define _HC_NON_COPYABLE_H_

// ����Ҫ�󲻿ɸ�ֵ(��������)���඼���Լ̳��ڴ�
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
