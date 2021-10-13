/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#ifndef __NSS_UDP_ST_STATS_H
#define __NSS_UDP_ST_STATS_H

#include <nss_cmn.h>

/*
 * nss_udp_st_stats.h
 *	NSS driver UDP_ST statistics header file.
 */

/*
 * udp_st statistics APIs
 */
extern void nss_udp_st_stats_reset(uint32_t if_num);
extern void nss_udp_st_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_udp_st_stats *nus);
extern void nss_udp_st_stats_dentry_create(void);

#endif /* __NSS_UDP_ST_STATS_H */
