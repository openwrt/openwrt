/*
 * Copyright (c) 2014, 2017, The Linux Foundation. All rights reserved.
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



#ifndef _SW_CONFIG_H
#define _SW_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SW_MAX_NR_DEV      3
#define SW_MAX_NR_PORT     16

#ifdef HSL_STANDALONG
#define HSL_LOCAL
#else
#define HSL_LOCAL static
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
