/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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


/**
 * @defgroup
 * @{
 */
#ifndef _FAL_FLOWCOOKIE_H_
#define _FAL_FLOWCOOKIE_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

int ssdk_flow_cookie_set(
		u32 protocol, __be32 src_ip,
		__be16 src_port, __be32 dst_ip,
		__be16 dst_port, u16 flowcookie);



#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_FLOWCOOKIE_H_ */

/**
 * @}
 */


