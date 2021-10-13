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


#ifndef _AOS_TIMER_H
#define _AOS_TIMER_H

#include "sal/os/aos_types.h"
#ifdef KERNEL_MODULE
#include "sal/os/linux/aos_timer_pvt.h"
#else
#include "sal/os/linux_user/aos_timer_pvt.h"
#endif


typedef __aos_timer_t           aos_timer_t;


/*
 * Delay in microseconds
 */
static inline void
aos_udelay(int usecs)
{
    return __aos_udelay(usecs);
}

/*
 * Delay in milliseconds.
 */
static inline void
aos_mdelay(int msecs)
{
    return __aos_mdelay(msecs);
}


#endif

