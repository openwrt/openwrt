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

#ifndef _AOS_PVTTYPES_H
#define _AOS_PVTTYPES_H

#include <generated/autoconf.h>
#include <asm/types.h>
#include <linux/compiler.h>
/*
 * Private definitions of general data types
 */

/* generic data types */
typedef struct device *   __aos_device_t;
typedef int               __aos_size_t;

#ifdef KVER26
#ifdef LNX26_22
typedef __u8 * __aos_iomem_t;
#else
typedef u8 __iomem * __aos_iomem_t;
#endif
#else /*Linux Kernel 2.4 */
typedef   u8         * __aos_iomem_t;
#endif

#ifdef KVER32
typedef u8 __iomem * __aos_iomem_t;
#endif

#ifdef LNX26_22 /* > Linux 2.6.22 */
typedef   __u8              __a_uint8_t;
typedef   __s8              __a_int8_t;
typedef   __u16             __a_uint16_t;
typedef   __s16             __a_int16_t;
typedef   __u32             __a_uint32_t;
typedef   __s32             __a_int32_t;
typedef   __u64             __a_uint64_t;
typedef   __s64             __a_int64_t;
#else
typedef   u8              __a_uint8_t;
typedef   s8              __a_int8_t;
typedef   u16             __a_uint16_t;
typedef   s16             __a_int16_t;
typedef   u32             __a_uint32_t;
typedef   s32             __a_int32_t;
typedef   u64             __a_uint64_t;
typedef   s64             __a_int64_t;
#endif

#define aos_printk        printk

#define AUTO_UPDATE_PPPOE_INFO 1
#if 0
#undef AUTO_UPDATE_PPPOE_INFO
#endif

#endif
