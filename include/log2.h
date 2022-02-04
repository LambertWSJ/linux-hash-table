#ifndef LOG2_H
#define LOG2_H

#include "types.h"
static __always_inline int fls64(u64 x);


static inline int fls(unsigned int x)
{
	return fls64(x);
}

static __always_inline int fls64(u64 x)
{
	u32 h = x >> 32;
	if (h)
		return fls(h) + 32;
	return fls(x);
}


int __ilog2_u32(u32 n)
{
	return fls(n) - 1;
}


static inline __attribute__((const))
int __ilog2_u64(u64 n)
{
	return fls64(n) - 1;
}


#define ilog2(n) \
( \
	__builtin_constant_p(n) ?	\
	((n) < 2 ? 0 :			\
	 63 - __builtin_clzll(n)) :	\
	(sizeof(n) <= 4) ?		\
	__ilog2_u32(n) :		\
	__ilog2_u64(n)			\
 )
#endif