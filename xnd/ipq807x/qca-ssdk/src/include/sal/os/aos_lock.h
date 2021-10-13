/*
 * Copyright (c) 2012,2018, The Linux Foundation. All rights reserved.
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

#ifndef _AOS_LOCK_H
#define _AOS_LOCK_H


#include "aos_lock_pvt.h"


typedef aos_lock_pvt_t aos_lock_t;


#define aos_lock_init(lock)  __aos_lock_init(lock)


#define aos_lock(lock) __aos_lock(lock)


#define aos_unlock(lock) __aos_unlock(lock)

#define aos_lock_bh(lock) __aos_lock_bh(lock)

#define aos_unlock_bh(lock) __aos_unlock_bh(lock)


#define aos_irq_save(flags) __aos_irq_save(flags)


#define aos_irq_restore(flags) __aos_irq_restore(flags)


#define aos_default_unlock  __aos_default_unlock


#endif
