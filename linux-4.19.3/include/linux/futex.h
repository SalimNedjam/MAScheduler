/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_FUTEX_H
#define _LINUX_FUTEX_H

#include <linux/ktime.h>
#include <uapi/linux/futex.h>
#include <linux/rtmutex.h>

#include <linux/kref.h>

struct inode;
struct mm_struct;
struct task_struct;

extern int
handle_futex_death(u32 __user *uaddr, struct task_struct *curr, int pi);

/*
 * MAS code:
 */
extern void free_futex_state(struct kref *kref);
extern int get_futex_state_sumload(struct task_struct *task);
extern void futex_state_prio(struct task_struct *task);
extern int futex_state_inherit(struct task_struct *task, 
																struct futex_state *state,
																int op);

#define FUTEX_STATE_LOAD 		1
#define FUTEX_STATE_UNLOAD	-1

/**
 * struct futex_state - The state struct to monitor futex owner
 * @list: 	the list of the states                                                                                               :		priority-sorted list of tasks waiting on this futex
 * @mutex: 	the lock of the state
 * @owner: 	the the task_struct if the owner of the futex
 * @refcount:	the kref counter
 * @load: 	the futex load, represent the number of waiters on the futex		
 * @key: 	the key the futex is hashed on
 */
struct futex_state {
	struct list_head list_global;
	struct list_head list_local;
	struct task_struct *owner;
	struct rt_mutex mutex;
	struct kref refcount;
	int load;
	union futex_key *key;
} __randomize_layout;


/*
 * Futexes are matched on equal values of this key.
 * The key type depends on whether it's a shared or private mapping.
 * Don't rearrange members without looking at hash_futex().
 *
 * offset is aligned to a multiple of sizeof(u32) (== 4) by definition.
 * We use the two low order bits of offset to tell what is the kind of key :
 *  00 : Private process futex (PTHREAD_PROCESS_PRIVATE)
 *       (no reference on an inode or mm)
 *  01 : Shared futex (PTHREAD_PROCESS_SHARED)
 *	mapped on a file (reference on the underlying inode)
 *  10 : Shared futex (PTHREAD_PROCESS_SHARED)
 *       (but private mapping on an mm, and reference taken on it)
*/

#define FUT_OFF_INODE    1 /* We set bit 0 if key has a reference on inode */
#define FUT_OFF_MMSHARED 2 /* We set bit 1 if key has a reference on mm */

union futex_key {
	struct {
		unsigned long pgoff;
		struct inode *inode;
		int offset;
	} shared;
	struct {
		unsigned long address;
		struct mm_struct *mm;
		int offset;
	} private;
	struct {
		unsigned long word;
		void *ptr;
		int offset;
	} both;
};

#define FUTEX_KEY_INIT (union futex_key) { .both = { .ptr = NULL } }

#ifdef CONFIG_FUTEX
extern void exit_robust_list(struct task_struct *curr);

long do_futex(u32 __user *uaddr, int op, u32 val, ktime_t *timeout,
	      u32 __user *uaddr2, u32 val2, u32 val3);
#ifdef CONFIG_HAVE_FUTEX_CMPXCHG
#define futex_cmpxchg_enabled 1
#else
extern int futex_cmpxchg_enabled;
#endif
#else
static inline void exit_robust_list(struct task_struct *curr)
{
}

static inline long do_futex(u32 __user *uaddr, int op, u32 val,
			    ktime_t *timeout, u32 __user *uaddr2,
			    u32 val2, u32 val3)
{
	return -EINVAL;
}
#endif

#ifdef CONFIG_FUTEX_PI
extern void exit_pi_state_list(struct task_struct *curr);
#else
static inline void exit_pi_state_list(struct task_struct *curr)
{
}
#endif

#endif
