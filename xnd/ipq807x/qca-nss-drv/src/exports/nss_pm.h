/*
 * Copyright (c) 2013-2019, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * @file nss_pm.h
 *	NSS power management definitions.
 */

#ifndef __NSS_PM_H
#define __NSS_PM_H

/**
 * @addtogroup nss_pm_subsystem
 * @{
 */

/**
 * nss_pm_client
 *	Power management (PM) clients.
 *
 * These clients can query for bus or clock performance levels.
 */
typedef enum nss_pm_client {
	NSS_PM_CLIENT_GMAC,
	NSS_PM_CLIENT_CRYPTO,
	NSS_PM_CLIENT_NETAP,
	NSS_PM_MAX_CLIENTS,
} nss_pm_client_t;

/**
 * nss_pm_perf_level
 *	Performance levels.
 *
 * This enumeration is passed as a parameter to NSS PM performance-level
 * requests.
 */
typedef enum nss_pm_perf_level {
	NSS_PM_PERF_LEVEL_SUSPEND = 0,
	NSS_PM_PERF_LEVEL_IDLE,
	NSS_PM_PERF_LEVEL_NOMINAL,
	NSS_PM_PERF_LEVEL_TURBO,
	NSS_PM_PERF_MAX_LEVELS,
} nss_pm_perf_level_t;

/**
 * nss_pm_interface_status_t
 *	Status of the PM client interface.
 */
typedef enum {
	NSS_PM_API_SUCCESS = 0,
	NSS_PM_API_FAILED,
} nss_pm_interface_status_t;

/**
 * nss_pm_client_register
 *	Registers a power management driver client.
 *
 * @datatypes
 * nss_pm_client_t
 *
 * @param[in] client_id  ID of the client driver.
 *
 * @return
 * None.
 */
extern void *nss_pm_client_register(nss_pm_client_t client_id);

/**
 * nss_pm_client_unregister
 *	Deregisters a power management driver client.
 *
 * @datatypes
 * nss_pm_client_t
 *
 * @param[in] client_id  ID of the client driver.
 *
 * @return
 * None.
 */
int nss_pm_client_unregister(nss_pm_client_t client_id);

/**
 * nss_pm_set_perf_level
 *	Updates the bus bandwidth level for a client.
 *
 * @datatypes
 * nss_pm_perf_level_t
 *
 * @param[in,out] handle  Handle of the client.
 * @param[in,out] lvl     Performance level.
 *
 * @return
 * None.
 */
extern nss_pm_interface_status_t nss_pm_set_perf_level(void *handle, nss_pm_perf_level_t lvl);

/**
 * @}
 */

#endif /* __NSS_PM_H */
