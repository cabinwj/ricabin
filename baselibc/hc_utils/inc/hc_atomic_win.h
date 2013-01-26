#ifndef __ARCH_WIN32_ATOMIC__
#define __ARCH_WIN32_ATOMIC__


#ifdef WIN32

#include "hc_os.h"

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef volatile long atomic_t;

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 * 
 * Atomically reads the value of @v.
 */ 
#define atomic_read(v)		(*(v))

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 * 
 * Atomically sets the value of @v to @i.
 */ 
#define atomic_set(v,i)		(*(v) = (i))

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type atomic_t
 * 
 * Atomically adds @i to @v.
 */
static __inline void atomic_add(long i, atomic_t *v)
{
	(void)InterlockedExchangeAdd((long*)v, i);
}

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 * 
 * Atomically subtracts @i from @v.
 */
static __inline void atomic_sub(long i, atomic_t *v)
{
	(void)atomic_add(-i, v);
}

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type atomic_t
 * 
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static __inline int atomic_sub_and_test(long i, atomic_t *v)
{
	return InterlockedExchangeAdd((long*)v, -i) == 0;
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 * 
 * Atomically increments @v by 1.
 */ 
static __inline void atomic_inc(atomic_t *v)
{
	(void)InterlockedExchangeAdd((long*)v, 1);
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type atomic_t
 * 
 * Atomically decrements @v by 1.
 */ 
static __inline void atomic_dec(atomic_t *v)
{
	(void)InterlockedExchangeAdd((long*)v, -1);
}

/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type atomic_t
 * 
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */ 
static __inline int atomic_dec_and_test(atomic_t *v)
{
	return InterlockedExchangeAdd((long*)v, -1) == 0;
}

/**
 * atomic_inc_and_test - increment and test 
 * @v: pointer of type atomic_t
 * 
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */ 
static __inline int atomic_inc_and_test(atomic_t *v)
{
	return InterlockedExchangeAdd((long*)v, 1) == 0;
}

/**
 * atomic_add_negative - add and test if negative
 * @v: pointer of type atomic_t
 * @i: integer value to add
 * 
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */ 
static __inline int atomic_add_negative(long i, atomic_t *v)
{
	return InterlockedExchangeAdd((long*)v, i) < 0;
}

/**
 * atomic_add_return - add and return
 * @v: pointer of type atomic_t
 * @i: integer value to add
 *
 * Atomically adds @i to @v and returns @i + @v
 */

static __inline int atomic_add_return(long i, atomic_t *v)
{
    return InterlockedExchangeAdd((long*)v, i);
}

static __inline int atomic_sub_return(long i, atomic_t *v)
{
    return atomic_add_return(-i, v);
}

#define atomic_inc_return(v)  (atomic_add_return(1,v))
#define atomic_dec_return(v)  (atomic_sub_return(1,v))

#endif

#endif