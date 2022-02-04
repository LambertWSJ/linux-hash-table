#ifndef RCUDATE_H
#define RCUDATE_H

#include "compiler.h"
#include <stdint.h>
#include "barrier.h"



#define rcu_check_sparse(p, space) \
    ((void)(((typeof(*p) space *)p) == p))
#define preemptible() 0

#define rcu_dereference_raw(p)                             \
    (                                                      \
        {                                                  \
            /* Dependency order vs. p above. */            \
            typeof(p) ________p1 = READ_ONCE(p);           \
            ((typeof(*p) __force __kernel *)(________p1)); \
        })

static inline int rcu_read_lock_any_held(void)
{
    return !preemptible();
}

#define RCU_LOCKDEP_WARN(c, s)                               \
    do                                                       \
    {                                                        \
        static bool __section(.data.unlikely) __warned;      \
        if (debug_lockdep_rcu_enabled() && !__warned && (c)) \
        {                                                    \
            __warned = true;                                 \
            lockdep_rcu_suspicious(__FILE__, __LINE__, s);   \
        }                                                    \
    } while (0)

#define RCU_INITIALIZER(v) (typeof(*(v)) __force __rcu *)(v)

#define rcu_assign_pointer(p, v)                                          \
    do                                                                    \
    {                                                                     \
        uintptr_t _r_a_p__v = (uintptr_t)(v);                             \
        rcu_check_sparse(p, __rcu);                                       \
                                                                          \
        if (__builtin_constant_p(v) && (_r_a_p__v) == (uintptr_t)NULL)    \
            WRITE_ONCE((p), (typeof(p))(_r_a_p__v));                      \
        else                                                              \
            smp_store_release(&p, RCU_INITIALIZER((typeof(p))_r_a_p__v)); \
    } while (0)

#endif