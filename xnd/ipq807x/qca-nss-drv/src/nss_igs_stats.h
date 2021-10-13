/*
 ******************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
 * ****************************************************************************
 */

#ifndef __NSS_IGS_STATS_H
#define __NSS_IGS_STATS_H

/*
 * nss_igs_stats_sync
 *	API to sync statistics for IGS
 */
extern void nss_igs_stats_sync(struct nss_ctx_instance *nss_ctx,
		 struct nss_cmn_msg *ncm, uint16_t if_num);

/*
 * nss_igs_stats_reset()
 *	API to reset the IGS stats.
 */
extern void nss_igs_stats_reset(uint32_t if_num);

/*
 * nss_igs_stats_init()
 *	API to initialize IGS debug instance statistics.
 */
extern void nss_igs_stats_init(uint32_t if_num, struct net_device *netdev);


/*
 * IGS statistics APIs
 */
extern void nss_igs_stats_dentry_create(void);

#endif /* __NSS_IGS_STATS_H */
