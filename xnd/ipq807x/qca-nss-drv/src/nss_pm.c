/*
 **************************************************************************
 * Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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

/*
 * nss_pm.c
 *    NSS Power Management APIs
 *
 */
#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <nss_api_if.h>
#include <nss_core.h>

#if (NSS_DT_SUPPORT != 1)
#include <nss_clocks.h>
#endif

#if (NSS_PM_SUPPORT == 1)
#include "nss_pm.h"

/*
 * Global NSS PM structure
 */
struct nss_pm_global_ctx ctx;

/*
 * Bus vector table for GMAC driver
 */
static struct msm_bus_paths nss_gmac_bw_level_tbl[NSS_PM_PERF_MAX_LEVELS] = {
		[NSS_PM_PERF_LEVEL_SUSPEND] =  GMAC_BW_MBPS(0, 0),
				/* 0 MHz to DDR, 0 MHz to TCM */
		[NSS_PM_PERF_LEVEL_IDLE] =  GMAC_BW_MBPS(133, 5),
				/* 133 MHz to DDR, 5 MHz to TCM */
		[NSS_PM_PERF_LEVEL_NOMINAL] =  GMAC_BW_MBPS(200, 400),
				/* 200 MHz to DDR, 10 MHz to TCM */
		[NSS_PM_PERF_LEVEL_TURBO] = GMAC_BW_MBPS(266, 533),
				/* 266 MHz to DDR, 20 MHz to TCM  */
};

/*
 * Bus vector table for Crypto driver
 */
static struct msm_bus_paths nss_crypto_bw_level_tbl[NSS_PM_PERF_MAX_LEVELS] = {
		[NSS_PM_PERF_LEVEL_SUSPEND] =  CRYPTO_BW_MBPS(0, 0),
				/* 0 MHz to DDR, 0 MHz to TCM */
		[NSS_PM_PERF_LEVEL_IDLE] =  CRYPTO_BW_MBPS(133, 5),
				/* 133 MHz to DDR, 5 MHz to TCM */
		[NSS_PM_PERF_LEVEL_NOMINAL] =  CRYPTO_BW_MBPS(200, 400),
				/* 200 MHz to DDR, 10 MHz to TCM */
		[NSS_PM_PERF_LEVEL_TURBO] = CRYPTO_BW_MBPS(266, 533),
				/* 266 MHz to DDR, 20 MHz to TCM  */
};

#ifdef  NSS_PM_NETAP_GMAC_SCALING

/*
 * Bus vector table for NSS HLOS driver
 * This requests bw for both NSS Fab0 and Fab1 on behalf of GMAC and NSS Drivers
 */
static struct msm_bus_paths nss_netap_bw_level_tbl[NSS_PM_PERF_MAX_LEVELS] = {
		[NSS_PM_PERF_LEVEL_SUSPEND] =  GMAC_BW_MBPS(0, 0),
			/* 0 MHz to DDR, 0 MHz to TCM */
		[NSS_PM_PERF_LEVEL_IDLE] =  GMAC_BW_MBPS(122, 122),
			/* 133 MHz to DDR and TCM */
		[NSS_PM_PERF_LEVEL_NOMINAL] =  GMAC_BW_MBPS(200, 200),
			/* 400 MHz to DDR and TCM */
		[NSS_PM_PERF_LEVEL_TURBO] = GMAC_BW_MBPS(400, 400),
			/* 533 MHz to DDR and TCM  */
};

#else

/*
 * Bus vector table for NSS HLOS driver
 */
static struct msm_bus_paths nss_netap_bw_level_tbl[NSS_PM_PERF_MAX_LEVELS] = {
		[NSS_PM_PERF_LEVEL_SUSPEND] =  NETAP_BW_MBPS(0, 0),
			/* 0 MHz to DDR, 0 MHz to TCM */
		[NSS_PM_PERF_LEVEL_IDLE] =  NETAP_BW_MBPS(133, 133),
			/* 133 MHz to DDR and TCM */
		[NSS_PM_PERF_LEVEL_NOMINAL] =  NETAP_BW_MBPS(400, 400),
			/* 400 MHz to DDR and TCM */
		[NSS_PM_PERF_LEVEL_TURBO] = NETAP_BW_MBPS(533, 533),
			/* 533 MHz to DDR and TCM  */
};

#endif

/*
 * Bus Driver Platform data for GMAC, Crypto and Netap clients
 */
static struct msm_bus_scale_pdata nss_bus_scale[] = {
	[NSS_PM_CLIENT_GMAC] = {
		.usecase = nss_gmac_bw_level_tbl,
		.num_usecases = ARRAY_SIZE(nss_gmac_bw_level_tbl),
		.active_only = 1,
		.name = "qca-nss-gmac",
	},

	[NSS_PM_CLIENT_CRYPTO] = {
		.usecase = nss_crypto_bw_level_tbl,
		.num_usecases = ARRAY_SIZE(nss_crypto_bw_level_tbl),
		.active_only = 1,
		.name = "qca-nss-crypto",
	},

	[NSS_PM_CLIENT_NETAP] = {
		.usecase = nss_netap_bw_level_tbl,
		.num_usecases = ARRAY_SIZE(nss_netap_bw_level_tbl),
		.active_only = 1,
		.name = "qca-nss-drv",
	},
};

/*
 * nss_pm_dbg_perf_level_get
 *    debugfs hook to get the current performance level
 */
static int nss_pm_dbg_perf_level_get(void *data, u64 *val)
{
	nss_pm_client_data_t *pm_client;

	pm_client = (nss_pm_client_data_t *)data;
	*val = pm_client->current_perf_lvl;

	return NSS_PM_API_SUCCESS;
}

/*
 * nss_pm_dbg_autoscale_get
 *    debugfs hook to get the current autoscale setting
 */
static int nss_pm_dbg_autoscale_get(void *data, u64 *val)
{
	nss_pm_client_data_t *pm_client;

	pm_client = (nss_pm_client_data_t *)data;
	*val = pm_client->auto_scale;

	return NSS_PM_API_SUCCESS;
}

/*
 * nss_pm_dbg_perf_level_set
 *    debugfs hook to set perf level for a client
 */
static int nss_pm_dbg_perf_level_set(void *data, u64 val)
{
	uint32_t perf_level;

	perf_level = (uint32_t) val;

	if (perf_level >= NSS_PM_PERF_MAX_LEVELS ||
			perf_level < NSS_PM_PERF_LEVEL_IDLE) {
		nss_pm_warning("unsupported performance level %d \n", perf_level);
		return NSS_PM_API_FAILED;
	}

	nss_pm_set_perf_level(data, perf_level);
	return NSS_PM_API_SUCCESS;
}

/*
 * nss_pm_dbg_autoscale_set
 *    debugfs hook to enable auto scaling for a client
 */
static int nss_pm_dbg_autoscale_set(void *data, u64 val)
{
	nss_pm_client_data_t *pm_client;

	if (val > 1) {
		nss_pm_warning(" Invalid set value, valid values are 0/1 \n");
		return NSS_PM_API_FAILED;
	}

	pm_client->auto_scale = (uint32_t)val;
	return NSS_PM_API_SUCCESS;
}

DEFINE_SIMPLE_ATTRIBUTE(perf_level_fops, nss_pm_dbg_perf_level_get, nss_pm_dbg_perf_level_set, "%llu\n");

DEFINE_SIMPLE_ATTRIBUTE(autoscale_fops, nss_pm_dbg_autoscale_get, nss_pm_dbg_autoscale_set, "%llu\n");
#endif /** (NSS_PM_SUPPORT == 1) */

/*
 * nss_pm_client_register
 *    Initialize GMAC specific PM parameters
 *
 * Creates debugfs hooks for user-space control of NSS Client PM
 * Initializes Bus BW to Idle Perf level
 * Returns PM handle to the caller.
 *
 */
void *nss_pm_client_register(nss_pm_client_t client_id)
{
#if (NSS_PM_SUPPORT == 1)
	int ret;
	struct dentry *pm_dentry;
	nss_pm_client_data_t *pm_client;

	if (unlikely(client_id >= NSS_PM_MAX_CLIENTS))  {
		nss_pm_warning("nss_pm_client_register invalid client id %d \n", client_id);
		goto error;
	}

	pm_client  =  &ctx.nss_pm_client[client_id];

	pm_client->bus_perf_client = msm_bus_scale_register_client(&nss_bus_scale[client_id]);
	if (!pm_client->bus_perf_client) {
		nss_pm_warning("unable to register bus client \n");
		goto error;
	}

	ret = msm_bus_scale_client_update_request(pm_client->bus_perf_client, NSS_PM_PERF_LEVEL_IDLE);
	if (ret) {
		nss_pm_warning("initial bandwidth req failed (%d)\n", ret);
		msm_bus_scale_unregister_client((uint32_t) pm_client->bus_perf_client);
		goto error;
	}

	pm_client->current_perf_lvl = NSS_PM_PERF_LEVEL_IDLE;

	switch (client_id) {
		case NSS_PM_CLIENT_GMAC:
			pm_dentry = debugfs_create_dir("gmac" , ctx.pm_dentry);
			break;

		case NSS_PM_CLIENT_CRYPTO:
			pm_dentry = debugfs_create_dir("crypto" , ctx.pm_dentry);
			break;

		case NSS_PM_CLIENT_NETAP:
			pm_dentry = debugfs_create_dir("netap" , ctx.pm_dentry);
			break;

		default:
			nss_pm_warning("debugfs create failed invalid client id %d \n", client_id);
			msm_bus_scale_unregister_client((uint32_t) pm_client->bus_perf_client);
			goto error;

	}

	if (unlikely(pm_dentry == NULL)) {
		nss_pm_info("debugfs not created for %d client pm \n", client_id);
		goto out;
	}

	pm_client->dentry = pm_dentry;
	pm_client->client_id = client_id;

	if (!debugfs_create_file("perf_level", S_IRUGO | S_IWUSR, pm_dentry, pm_client, &perf_level_fops)) {
		nss_pm_info("debugfs perf_level file not created for %d client pm \n", client_id);
	}

	if (!debugfs_create_file("auto-scale", S_IRUGO | S_IWUSR, pm_dentry, pm_client, &autoscale_fops)) {
		nss_pm_info("debugfs auto-scale file not created for %d client pm \n", client_id);
	}

out:
	return (void *)pm_client;
error:
#endif
	return NULL;
}
EXPORT_SYMBOL(nss_pm_client_register);

/*
 * nss_pm_client_unregister
 *    Unregister the client for any PM operations
 */
int nss_pm_client_unregister(nss_pm_client_t client_id)
{
#if (NSS_PM_SUPPORT == 1)
	nss_pm_client_data_t *pm_client;

	if (unlikely(client_id >= NSS_PM_MAX_CLIENTS))  {
		nss_pm_warning("nss_pm_client_unregister invalid client id %d \n", client_id);
		goto error;
	}

	pm_client  =  &ctx.nss_pm_client[client_id];

	if (unlikely(pm_client == NULL))  {
		nss_pm_warning("nss_pm_client_unregister client not registered %d \n", client_id);
		goto error;
	}

	if (pm_client->bus_perf_client) {
		msm_bus_scale_unregister_client((uint32_t) pm_client->bus_perf_client);
	} else {
		nss_pm_info("nss_pm_client_unregister: client not registered \n");
	}

	if (likely(pm_client->dentry != NULL)) {
		debugfs_remove_recursive(pm_client->dentry);
	}

	return NSS_PM_API_SUCCESS;

error:
#endif
	return NSS_PM_API_FAILED;
}

/*
 * nss_pm_set_perf_level()
 *    Sets the performance level of client specific Fabrics and Clocks to requested level
 */
nss_pm_interface_status_t nss_pm_set_perf_level(void *handle, nss_pm_perf_level_t lvl)
{
#if ((NSS_DT_SUPPORT == 1) && (NSS_FREQ_SCALE_SUPPORT == 1))
	nss_freq_scales_t index;

	switch (lvl) {
	case NSS_PM_PERF_LEVEL_TURBO:
		index = NSS_FREQ_HIGH_SCALE;
		break;

	case NSS_PM_PERF_LEVEL_NOMINAL:
		index = NSS_FREQ_MID_SCALE;
		break;

	default:
		index = NSS_PM_PERF_LEVEL_IDLE;
	}

#if !defined(NSS_HAL_IPQ807x_SUPPORT)
	nss_freq_sched_change(index, false);
#endif

#elif (NSS_PM_SUPPORT == 1)

	int ret = 0;
	nss_pm_client_data_t *pm_client;

	pm_client = (nss_pm_client_data_t *) handle;
	if (pm_client->current_perf_lvl == lvl) {
		nss_pm_trace("Already at perf level %d , ignoring request \n", lvl);
		return NSS_PM_API_SUCCESS;
	}

	if (!pm_client->bus_perf_client) {
		nss_pm_warning("Bus driver client not registered.request failed \n");
		return NSS_PM_API_FAILED;
	}

	/*
	 * Do client specific operations here
	 */
	if (pm_client->client_id == NSS_PM_CLIENT_NETAP) {
		if ((lvl == NSS_PM_PERF_LEVEL_TURBO) && (ctx.turbo_support == true)) {
			/*
			 * For turbo perf level, switch TCM source to
			 * SRC1 to set TCM clock = 400 MHz
			 * SRC0 and SRC1 are set to 266 and 400 MHz resp.
			 * in nss_hal/ipq806x/nss_hal_pvt.c
			 */
			writel(0x3, NSSTCM_CLK_SRC_CTL);
		} else {
			/*
			 * For Nominal and Idle perf level, switch to SRC0 to
			 * set TCM clock = 266 MHz
			 */
			writel(0x2, NSSTCM_CLK_SRC_CTL);

			if (lvl == NSS_PM_PERF_LEVEL_TURBO) {
				lvl = NSS_PM_PERF_LEVEL_NOMINAL;
			}
		}
	}

	if (pm_client->client_id == NSS_PM_CLIENT_CRYPTO) {
		if ((lvl == NSS_PM_PERF_LEVEL_TURBO) && (ctx.turbo_support == true))  {
			/*
			 * For Turbo mode, set Crypto core and
			 * Fabric  port clocks to 213 MHz
			 */
			writel(0x23, CE5_ACLK_SRC0_NS);
			writel(0x23, CE5_HCLK_SRC0_NS);
			writel(0x23, CE5_CORE_CLK_SRC0_NS);

			writel(0x2, CE5_ACLK_SRC_CTL);
			writel(0x2, CE5_HCLK_SRC_CTL);
			writel(0x2, CE5_CORE_CLK_SRC_CTL);
		} else {
			lvl = NSS_PM_PERF_LEVEL_NOMINAL;
		}
	}

	/* Update bandwidth if request has changed. This may sleep. */
	ret = msm_bus_scale_client_update_request(pm_client->bus_perf_client, lvl);
	if (ret) {
		nss_pm_warning("bandwidth request failed (%d)\n", ret);
		return NSS_PM_API_FAILED;
	}

	nss_pm_info("perf level request, current: %d new: %d \n", pm_client->current_perf_lvl, lvl);
	pm_client->current_perf_lvl = lvl;
#endif

	return NSS_PM_API_SUCCESS;
}
EXPORT_SYMBOL(nss_pm_set_perf_level);

#if (NSS_PM_SUPPORT == 1)
/*
 * nss_pm_set_turbo()
 *   Sets the turbo support flag globally for all clients
 */
void nss_pm_set_turbo() {

	nss_pm_info("NSS Bus PM - Platform supports Turbo Mode \n");
	ctx.turbo_support = true;
}

/*
 * nss_pm_init()
 *    Initialize NSS PM top level structures
 */
void nss_pm_init(void) {

	nss_pm_info("NSS Bus PM (platform - IPQ806x, build - %s:%s)\n", __DATE__, __TIME__);

	ctx.pm_dentry = debugfs_create_dir("qca-nss-pm", NULL);

	/* Default turbo support is set to off */
	ctx.turbo_support = false;

	if (unlikely(ctx.pm_dentry == NULL)) {
		nss_pm_warning("Failed to create qca-nss-drv directory in debugfs");
	}
}
#endif
