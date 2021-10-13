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

#ifndef _AOS_MEM_PVT_H
#define _AOS_MEM_PVT_H

#include <stdlib.h>
#include <string.h>

static inline void *__aos_mem_alloc(aos_size_t size)
{
    return (malloc(size));
}

static inline void __aos_mem_free(void *buf)
{
    free(buf);
}

/* move a memory buffer */
static inline void
__aos_mem_copy(void *dst, void *src, aos_size_t size)
{
    memcpy(dst, src, size);
}

/* set a memory buffer */
static inline void
__aos_mem_set(void *buf, a_uint8_t b, aos_size_t size)
{
    memset(buf, b, size);
}

/* zero a memory buffer */
static inline void
__aos_mem_zero(void *buf, aos_size_t size)
{
    memset(buf, 0, size);
}

/* compare two memory buffers */
static inline int
__aos_mem_cmp(void *buf1, void *buf2, aos_size_t size)
{
    return (memcmp(buf1, buf2, size) == 0) ? 0 : 1;
}



#endif /*_AOS_MEM_PVT_H*/
