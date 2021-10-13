/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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


#include <linux/interrupt.h>
#include <linux/spinlock.h>


typedef spinlock_t aos_lock_pvt_t;


#define __aos_lock_init(lock)  spin_lock_init(lock)


#define __aos_lock(lock) spin_lock(lock)


#define __aos_unlock(lock) spin_unlock(lock)

#define __aos_lock_bh(lock) spin_lock_bh(lock)

#define __aos_unlock_bh(lock) spin_unlock_bh(lock)

#define __aos_irq_save(flags) local_irq_save(flags)

#define __aos_irq_restore(flags) local_irq_restore(flags)

#ifndef KVER32
#define __aos_default_unlock SPIN_LOCK_UNLOCKED
#endif

#endif /*_AOS_LOCK_PVT_H*/

