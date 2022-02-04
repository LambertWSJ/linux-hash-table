#ifndef BARRIER_H
#define BARRIER_H

#define __noreturn	__attribute__((noreturn))

#define __compiletime_error(message) __attribute__((error(message)))

#define __compiletime_assert(condition, msg, prefix, suffix)   \
    do                                                         \
    {                                                          \
        /*                                                     \
         * __noreturn is needed to give the compiler enough    \
         * information to avoid certain possibly-uninitialized \
         * warnings (regardless of the build failing).         \
         */                                                    \
        __noreturn extern void prefix##suffix(void)            \
            __compiletime_error(msg);                          \
        if (!(condition))                                      \
            prefix##suffix();                                  \
    } while (0)
#define _compiletime_assert(condition, msg, prefix, suffix) \
    __compiletime_assert(condition, msg, prefix, suffix)

#define __native_word(t)                                        \
    (sizeof(t) == sizeof(char) || sizeof(t) == sizeof(short) || \
     sizeof(t) == sizeof(int) || sizeof(t) == sizeof(long))

#define compiletime_assert(condition, msg) \
    _compiletime_assert(condition, msg, __compiletime_assert_, __COUNTER__)

#define compiletime_assert_atomic_type(t) \
    compiletime_assert(__native_word(t),  \
                       "Need native word sized stores/loads for atomicity.")

#ifndef mb
#define mb() barrier()
#endif

#ifndef __smp_mb
#define __smp_mb() mb()
#endif

#ifndef __smp_store_release
#define __smp_store_release(p, v)           \
    do                                      \
    {                                       \
        compiletime_assert_atomic_type(*p); \
        __smp_mb();                         \
        WRITE_ONCE(*p, v);                  \
    } while (0)
#endif

#ifndef smp_store_release
#define smp_store_release(p, v)    \
    do                             \
    {                              \
        kcsan_release();           \
        __smp_store_release(p, v); \
    } while (0)
#endif

#endif