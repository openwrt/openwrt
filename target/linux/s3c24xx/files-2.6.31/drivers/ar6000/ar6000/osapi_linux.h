/*
 * $Id: //depot/sw/releases/olca2.0-GPL/host/os/linux/include/osapi_linux.h#1 $
 *
 * This file contains the definitions of the basic atheros data types.
 * It is used to map the data types in atheros files to a platform specific
 * type.
 *
 * Copyright 2003-2005 Atheros Communications, Inc.,  All Rights Reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */

#ifndef _OSAPI_LINUX_H_
#define _OSAPI_LINUX_H_

#ifdef __KERNEL__

#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/jiffies.h>
#endif
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/wait.h>
#ifdef KERNEL_2_4
#include <asm/arch/irq.h>
#include <asm/irq.h>
#endif

#ifdef __GNUC__
#define __ATTRIB_PACK           __attribute__ ((packed))
#define __ATTRIB_PRINTF         __attribute__ ((format (printf, 1, 2)))
#define __ATTRIB_NORETURN       __attribute__ ((noreturn))
#ifndef INLINE
#define INLINE                  __inline__
#endif
#else /* Not GCC */
#define __ATTRIB_PACK
#define __ATTRIB_PRINTF
#define __ATTRIB_NORETURN
#ifndef INLINE
#define INLINE                  __inline
#endif
#endif /* End __GNUC__ */

#define PREPACK
#define POSTPACK                __ATTRIB_PACK

/*
 * Endianes macros
 */
#define A_BE2CPU8(x)       ntohb(x)
#define A_BE2CPU16(x)      ntohs(x)
#define A_BE2CPU32(x)      ntohl(x)

#define A_LE2CPU8(x)       (x)
#define A_LE2CPU16(x)      (x)
#define A_LE2CPU32(x)      (x)

#define A_CPU2BE8(x)       htonb(x)
#define A_CPU2BE16(x)      htons(x)
#define A_CPU2BE32(x)      htonl(x)

#define A_MEMCPY(dst, src, len)         memcpy((A_UINT8 *)(dst), (src), (len))
#define A_MEMZERO(addr, len)            memset(addr, 0, len)
#define A_MEMCMP(addr1, addr2, len)     memcmp((addr1), (addr2), (len))
#define A_MALLOC(size)                  kmalloc((size), GFP_KERNEL)
#define A_MALLOC_NOWAIT(size)           kmalloc((size), GFP_ATOMIC)
#define A_FREE(addr)                    kfree(addr)
#define A_PRINTF(args...)               printk(args)

/* Mutual Exclusion */
typedef spinlock_t                      A_MUTEX_T;
#define A_MUTEX_INIT(mutex)             spin_lock_init(mutex)
#define A_MUTEX_LOCK(mutex)             spin_lock_bh(mutex)
#define A_MUTEX_UNLOCK(mutex)           spin_unlock_bh(mutex)
#define A_IS_MUTEX_VALID(mutex)         TRUE  /* okay to return true, since A_MUTEX_DELETE does nothing */
#define A_MUTEX_DELETE(mutex)           /* spin locks are not kernel resources so nothing to free.. */

/* Get current time in ms adding a constant offset (in ms) */
#define A_GET_MS(offset)    \
	(jiffies + ((offset) / 1000) * HZ)

/*
 * Timer Functions
 */
#define A_MDELAY(msecs)                 mdelay(msecs)
typedef struct timer_list               A_TIMER;

#define A_INIT_TIMER(pTimer, pFunction, pArg) do {              \
    init_timer(pTimer);                                         \
    (pTimer)->function = (pFunction);                           \
    (pTimer)->data   = (unsigned long)(pArg);                   \
} while (0)

/*
 * Start a Timer that elapses after 'periodMSec' milli-seconds
 * Support is provided for a one-shot timer. The 'repeatFlag' is
 * ignored.
 */
#define A_TIMEOUT_MS(pTimer, periodMSec, repeatFlag) do {                   \
    if (repeatFlag) {                                                       \
        printk("\n" __FILE__ ":%d: Timer Repeat requested\n",__LINE__);     \
        panic("Timer Repeat");                                              \
    }                                                                       \
    mod_timer((pTimer), jiffies + HZ * (periodMSec) / 1000);                \
} while (0)

/*
 * Cancel the Timer.
 */
#define A_UNTIMEOUT(pTimer) do {                                \
    del_timer((pTimer));                                        \
} while (0)

#define A_DELETE_TIMER(pTimer) do {                             \
} while (0)

/*
 * Wait Queue related functions
 */
typedef wait_queue_head_t               A_WAITQUEUE_HEAD;
#define A_INIT_WAITQUEUE_HEAD(head)     init_waitqueue_head(head)
#ifndef wait_event_interruptible_timeout
#define __wait_event_interruptible_timeout(wq, condition, ret)          \
do {                                                                    \
        wait_queue_t __wait;                                            \
        init_waitqueue_entry(&__wait, current);                         \
                                                                        \
        add_wait_queue(&wq, &__wait);                                   \
        for (;;) {                                                      \
                set_current_state(TASK_INTERRUPTIBLE);                  \
                if (condition)                                          \
                        break;                                          \
                if (!signal_pending(current)) {                         \
                        ret = schedule_timeout(ret);                    \
                        if (!ret)                                       \
                                break;                                  \
                        continue;                                       \
                }                                                       \
                ret = -ERESTARTSYS;                                     \
                break;                                                  \
        }                                                               \
        current->state = TASK_RUNNING;                                  \
        remove_wait_queue(&wq, &__wait);                                \
} while (0)

#define wait_event_interruptible_timeout(wq, condition, timeout)        \
({                                                                      \
        long __ret = timeout;                                           \
        if (!(condition))                                               \
                __wait_event_interruptible_timeout(wq, condition, __ret); \
        __ret;                                                          \
})
#endif /* wait_event_interruptible_timeout */

#define A_WAIT_EVENT_INTERRUPTIBLE_TIMEOUT(head, condition, timeout) do { \
    wait_event_interruptible_timeout(head, condition, timeout); \
} while (0)

#define A_WAKE_UP(head)                 wake_up(head)

#ifdef DEBUG
#define A_ASSERT(expr)  \
    if (!(expr)) {   \
        printk(KERN_ALERT "\n" __FILE__ ":%d: Assertion " #expr " failed!\n",__LINE__); \
        panic(#expr); \
    }

#else
#define A_ASSERT(expr)
#endif /* DEBUG */

/*
 * Initialization of the network buffer subsystem
 */
#define A_NETBUF_INIT()

/*
 * Network buffer queue support
 */
typedef struct sk_buff_head A_NETBUF_QUEUE_T;

#define A_NETBUF_QUEUE_INIT(q)  \
    a_netbuf_queue_init(q)

#define A_NETBUF_ENQUEUE(q, pkt) \
    a_netbuf_enqueue((q), (pkt))
#define A_NETBUF_PREQUEUE(q, pkt) \
    a_netbuf_prequeue((q), (pkt))
#define A_NETBUF_DEQUEUE(q) \
    (a_netbuf_dequeue(q))
#define A_NETBUF_QUEUE_SIZE(q)  \
    a_netbuf_queue_size(q)
#define A_NETBUF_QUEUE_EMPTY(q) \
    a_netbuf_queue_empty(q)

/*
 * Network buffer support
 */
#define A_NETBUF_ALLOC(size) \
    a_netbuf_alloc(size)
#define A_NETBUF_ALLOC_RAW(size) \
    a_netbuf_alloc_raw(size)
#define A_NETBUF_FREE(bufPtr) \
    a_netbuf_free(bufPtr)
#define A_NETBUF_DATA(bufPtr) \
    a_netbuf_to_data(bufPtr)
#define A_NETBUF_LEN(bufPtr) \
    a_netbuf_to_len(bufPtr)
#define A_NETBUF_PUSH(bufPtr, len) \
    a_netbuf_push(bufPtr, len)
#define A_NETBUF_PUT(bufPtr, len) \
    a_netbuf_put(bufPtr, len)
#define A_NETBUF_TRIM(bufPtr,len) \
    a_netbuf_trim(bufPtr, len)
#define A_NETBUF_PULL(bufPtr, len) \
    a_netbuf_pull(bufPtr, len)
#define A_NETBUF_HEADROOM(bufPtr)\
    a_netbuf_headroom(bufPtr)
#define A_NETBUF_SETLEN(bufPtr,len) \
    a_netbuf_setlen(bufPtr, len)

/* Add data to end of a buffer  */
#define A_NETBUF_PUT_DATA(bufPtr, srcPtr,  len) \
    a_netbuf_put_data(bufPtr, srcPtr, len)

/* Add data to start of the  buffer */
#define A_NETBUF_PUSH_DATA(bufPtr, srcPtr,  len) \
    a_netbuf_push_data(bufPtr, srcPtr, len)

/* Remove data at start of the buffer */
#define A_NETBUF_PULL_DATA(bufPtr, dstPtr, len) \
    a_netbuf_pull_data(bufPtr, dstPtr, len)

/* Remove data from the end of the buffer */
#define A_NETBUF_TRIM_DATA(bufPtr, dstPtr, len) \
    a_netbuf_trim_data(bufPtr, dstPtr, len)

/* View data as "size" contiguous bytes of type "t" */
#define A_NETBUF_VIEW_DATA(bufPtr, t, size) \
    (t )( ((struct skbuf *)(bufPtr))->data)

/* return the beginning of the headroom for the buffer */
#define A_NETBUF_HEAD(bufPtr) \
        ((((struct sk_buff *)(bufPtr))->head))

/*
 * OS specific network buffer access routines
 */
void *a_netbuf_alloc(int size);
void *a_netbuf_alloc_raw(int size);
void a_netbuf_free(void *bufPtr);
void *a_netbuf_to_data(void *bufPtr);
A_UINT32 a_netbuf_to_len(void *bufPtr);
A_STATUS a_netbuf_push(void *bufPtr, A_INT32 len);
A_STATUS a_netbuf_push_data(void *bufPtr, char *srcPtr, A_INT32 len);
A_STATUS a_netbuf_put(void *bufPtr, A_INT32 len);
A_STATUS a_netbuf_put_data(void *bufPtr, char *srcPtr, A_INT32 len);
A_STATUS a_netbuf_pull(void *bufPtr, A_INT32 len);
A_STATUS a_netbuf_pull_data(void *bufPtr, char *dstPtr, A_INT32 len);
A_STATUS a_netbuf_trim(void *bufPtr, A_INT32 len);
A_STATUS a_netbuf_trim_data(void *bufPtr, char *dstPtr, A_INT32 len);
A_STATUS a_netbuf_setlen(void *bufPtr, A_INT32 len);
A_INT32 a_netbuf_headroom(void *bufPtr);
void a_netbuf_enqueue(A_NETBUF_QUEUE_T *q, void *pkt);
void a_netbuf_prequeue(A_NETBUF_QUEUE_T *q, void *pkt);
void *a_netbuf_dequeue(A_NETBUF_QUEUE_T *q);
int a_netbuf_queue_size(A_NETBUF_QUEUE_T *q);
int a_netbuf_queue_empty(A_NETBUF_QUEUE_T *q);
int a_netbuf_queue_empty(A_NETBUF_QUEUE_T *q);
void a_netbuf_queue_init(A_NETBUF_QUEUE_T *q);

/*
 * Kernel v.s User space functions
 */
A_UINT32 a_copy_to_user(void *to, const void *from, A_UINT32 n);
A_UINT32 a_copy_from_user(void *to, const void *from, A_UINT32 n);

#else /* __KERNEL__ */

#ifdef __GNUC__
#define __ATTRIB_PACK           __attribute__ ((packed))
#define __ATTRIB_PRINTF         __attribute__ ((format (printf, 1, 2)))
#define __ATTRIB_NORETURN       __attribute__ ((noreturn))
#ifndef INLINE
#define INLINE                  __inline__
#endif
#else /* Not GCC */
#define __ATTRIB_PACK
#define __ATTRIB_PRINTF
#define __ATTRIB_NORETURN
#ifndef INLINE
#define INLINE                  __inline
#endif
#endif /* End __GNUC__ */

#define PREPACK
#define POSTPACK                __ATTRIB_PACK

#endif /* __KERNEL__ */

#endif /* _OSAPI_LINUX_H_ */
