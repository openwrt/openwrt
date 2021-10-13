/*
 **************************************************************************
 * Copyright (c) 2014-2017,2020, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

/**
 * nss_data_plane
 *	Data plane used for communication between qca-nss-drv & data plane host
 */

#ifndef __NSS_DATA_PLANE_H
#define __NSS_DATA_PLANE_H

#include <nss_api_if.h>
#include "nss_phys_if.h"

/*
 * nss_data_plane_schedule_registration()
 *	Called from nss_init to schedule a work to do data_plane register to data plane host driver
 */
bool nss_data_plane_schedule_registration(void);

/*
 * nss_data_plane_init_delay_work()
 *	Initialize data_plane workqueue
 */
int nss_data_plane_init_delay_work(void);

/*
 * nss_data_plane_destroy_delay_work()
 *	Destroy data_plane workqueue
 */
void nss_data_plane_destroy_delay_work(void);

/*
 * nss_data_plane_ops defines the API required to support multiple data plane targets
 */
struct nss_data_plane_ops {
	void (*data_plane_register)(struct nss_ctx_instance *nss_ctx);
	void (*data_plane_unregister)(void);
	void (*data_plane_stats_sync)(struct nss_phys_if_stats *stats, uint16_t interface);
	uint16_t (*data_plane_get_mtu_sz)(uint16_t max_mtu);
};

extern struct nss_data_plane_ops nss_data_plane_gmac_ops;
extern struct nss_data_plane_ops nss_data_plane_ops;

extern int nss_skip_nw_process;
#endif
