#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#ifdef WIN32
#include "hc_atomic_win.h"
#else
#if __GNUC__ < 4
#include "hc_atomic_asm.h"
#else
#if __WORDSIZE==64
#include "hc_atomic_gcc8.h"
#else
#include "hc_atomic_gcc.h"
#endif
#endif
#endif

#endif // _ATOMIC_H_

