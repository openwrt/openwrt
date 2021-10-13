/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _AOS_LOCK_PVT_H
#define _AOS_LOCK_PVT_H

#include <pthread.h>
#include "sal/os/aos_types.h"


typedef pthread_mutex_t aos_lock_pvt_t;


#define __aos_lock_init(lock) \
    pthread_mutex_init(lock, NULL); \
    pthread_mutexattr_setpshared(lock, PTHREAD_PROCESS_SHARED)


#define __aos_lock(lock) pthread_mutex_lock(lock)


#define __aos_unlock(lock) pthread_mutex_unlock(lock)


#define __aos_irq_save(flags)


#define __aos_irq_restore(flags)


#define __aos_default_unlock PTHREAD_MUTEX_INITIALIZER


#endif /*_AOS_LOCK_PVT_H*/

