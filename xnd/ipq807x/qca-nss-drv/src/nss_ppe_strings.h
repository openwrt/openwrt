/*
 ***************************************************************************
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
 ***************************************************************************
 */

#ifndef __NSS_PPE_STRINGS_H
#define __NSS_PPE_STRINGS_H

#include "nss_ppe_stats.h"

extern struct nss_stats_info nss_ppe_stats_str_conn[NSS_PPE_STATS_CONN_MAX];
extern struct nss_stats_info nss_ppe_stats_str_sc[NSS_PPE_STATS_SERVICE_CODE_MAX];
extern struct nss_stats_info nss_ppe_stats_str_l3[NSS_PPE_STATS_L3_MAX];
extern struct nss_stats_info nss_ppe_stats_str_code[NSS_PPE_STATS_CODE_MAX];
extern struct nss_stats_info nss_ppe_stats_str_dc[NSS_PPE_STATS_DROP_CODE_MAX];
extern struct nss_stats_info nss_ppe_stats_str_cc[NSS_PPE_STATS_CPU_CODE_MAX];
extern void nss_ppe_strings_dentry_create(void);

#endif /* __NSS_PPE_STRINGS_H */
