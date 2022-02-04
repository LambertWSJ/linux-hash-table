#ifndef RCULIST_H
#define RCULIST_H

#include "types.h"
#include "barrier.h"
#include "rcupdate.h"
#include "list.h"

#define hlist_first_rcu(head)	(*((struct hlist_node __rcu **)(&(head)->first)))

static inline void hlist_add_head_rcu(struct hlist_node *n,
                                      struct hlist_head *h)
{
    struct hlist_node *first = h->first;

    n->next = first;
    WRITE_ONCE(n->pprev, &h->first);
    rcu_assign_pointer(hlist_first_rcu(h), n);
    if (first)
        WRITE_ONCE(first->pprev, &n->next);
}

static inline void hlist_del_init_rcu(struct hlist_node *n)
{
    if (!hlist_unhashed(n))
    {
        __hlist_del(n);
        WRITE_ONCE(n->pprev, NULL);
    }
}

#define __list_check_rcu(dummy, cond, extra...)                            \
    (                                                                      \
        {                                                                  \
            check_arg_count_one(extra);                                    \
            RCU_LOCKDEP_WARN(!(cond) && !rcu_read_lock_any_held(),         \
                             "RCU-list traversed in non-reader section!"); \
        })

#define hlist_for_each_entry_rcu(pos, head, member, cond...)                \
    for (__list_check_rcu(dummy, ##cond, 0),                                \
         pos = hlist_entry_safe(rcu_dereference_raw(hlist_first_rcu(head)), \
                                typeof(*(pos)), member);                    \
         pos;                                                               \
         pos = hlist_entry_safe(rcu_dereference_raw(hlist_next_rcu(         \
                                    &(pos)->member)),                       \
                                typeof(*(pos)), member))

#define hlist_for_each_entry_rcu_notrace(pos, head, member)                       \
    for (pos = hlist_entry_safe(rcu_dereference_raw_check(hlist_first_rcu(head)), \
                                typeof(*(pos)), member);                          \
         pos;                                                                     \
         pos = hlist_entry_safe(rcu_dereference_raw_check(hlist_next_rcu(         \
                                    &(pos)->member)),                             \
                                typeof(*(pos)), member))

#endif