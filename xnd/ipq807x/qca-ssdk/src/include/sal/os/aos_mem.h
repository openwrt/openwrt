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

#ifndef _AOS_MEM_H
#define _AOS_MEM_H

#include "aos_types.h"
#include "aos_mem_pvt.h"

/**
 * @g aos_mem mem
 * @{
 *
 * @ig shim_ext
 */

/**
 * @brief Allocate a memory buffer. Note it's a non-blocking call.
 * This call can block.
 *
 * @param[in] size    buffer size
 *
 * @return Buffer pointer or NULL if there's not enough memory.
 */
static inline void *
aos_mem_alloc(aos_size_t size)
{
    return __aos_mem_alloc(size);
}

/**
 * @brief Free malloc'ed buffer
 *
 * @param[in] buf     buffer pointer allocated by aos_alloc()
 * @param[in] size    buffer size
 */
static inline void
aos_mem_free(void *buf)
{
    __aos_mem_free(buf);
}

/**
 * @brief Move a memory buffer
 *
 * @param[in] dst     destination address
 * @param[in] src     source address
 * @param[in] size    buffer size
 */
static inline void
aos_mem_copy(void *dst, void *src, aos_size_t size)
{
    __aos_mem_copy(dst, src, size);
}

/**
 * @brief Fill a memory buffer
 *
 * @param[in] buf   buffer to be filled
 * @param[in] b     byte to fill
 * @param[in] size  buffer size
 */
static inline void
aos_mem_set(void *buf, a_uint8_t b, aos_size_t size)
{
    __aos_mem_set(buf, b, size);
}

/**
 * @brief Zero a memory buffer
 *
 * @param[in] buf   buffer to be zeroed
 * @param[in] size  buffer size
 */
static inline void
aos_mem_zero(void *buf, aos_size_t size)
{
    __aos_mem_zero(buf, size);
}

/**
 * @brief Compare two memory buffers
 *
 * @param[in] buf1  first buffer
 * @param[in] buf2  second buffer
 * @param[in] size  buffer size
 *
 * @retval    0     equal
 * @retval    1     not equal
 */
static inline int
aos_mem_cmp(void *buf1, void *buf2, aos_size_t size)
{
    return __aos_mem_cmp(buf1, buf2, size);
}

/**
 * @}
 */

#endif
