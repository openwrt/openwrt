/*
 **************************************************************************
 * Copyright (c) 2013-2017, 2019-2021 The Linux Foundation. All rights reserved.
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
 * nss_rps.c
 *	NSS RPS based APIs
 */

#include "nss_tx_rx_common.h"

#define NSS_RPS_MAX_CORE_HASH_BITMAP  ((1 << (NSS_HOST_CORES)) - 1)
			/**< Maximum value that when all cores are available. */
#define NSS_RPS_PRI_MAP_PARAM_FIELD_COUNT 2

int nss_rps_config __read_mostly;
int nss_rps_hash_bitmap = NSS_RPS_MAX_CORE_HASH_BITMAP;
int nss_rps_pri_map[NSS_MAX_NUM_PRI];

/*
 * It is used to parse priority and core from the input.
 */
struct nss_rps_pri_map_parse_data {
	uint8_t pri;	/**< Priority Index. */
	int8_t core;	/**< Host core-id. */
};

/*
 * Private data structure.
 */
struct nss_rps_pvt {
	struct semaphore sem;		/* Semaphore structure. */
	struct completion complete;	/* Completion structure. */
	int response;			/* Response from FW. */
	void *cb;			/* Original cb for sync msgs. */
	void *app_data;			/* Original app_data for sync msgs. */
};

static struct nss_rps_pvt nss_rps_cfg_pvt;

/*
 * nss_rps_pri_map_usage()
 *      Help function shows the usage of the command.
 */
static inline void nss_rps_pri_map_usage(void)
{
	nss_info_always("\nUsage:\n");
	nss_info_always("echo <priority> <core> > /proc/sys/dev/nss/rps/pri_map\n\n");
	nss_info_always("priority[0 to %u] core[-1 to %u]:\n\n",
			NSS_MAX_NUM_PRI - 1,
			NSS_HOST_CORES - 1);
}

/*
 * nss_rps_pri_map_print()
 *	Sysctl handler for printing rps/pri mapping.
 */
static int nss_rps_pri_map_print(struct ctl_table *ctl, void __user *buffer,
				size_t *lenp, loff_t *ppos, int *pri_map)
{
	char *r_buf;
	int i, len;
	size_t cp_bytes = 0;

	/*
	 * (2 * 4) + 12 bytes for the buffer size is sufficient to write
	 * the table including the spaces and new line characters.
	 */
	r_buf = kzalloc(((4 * NSS_MAX_NUM_PRI) + 12) * sizeof(char),
			GFP_KERNEL);
	if (!r_buf) {
		nss_warning("Failed to alloc buffer to print pri map\n");
		return -EFAULT;
	}

	/*
	 * Write the core values that corresponds to each priorities.
	 */
	len = scnprintf(r_buf + cp_bytes, 8, "Cores: ");
	cp_bytes += len;
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		len = scnprintf(r_buf + cp_bytes, 4, "%d ", pri_map[i]);
		if (!len) {
			nss_warning("failed to read from buffer %d\n", pri_map[i]);
			kfree(r_buf);
			return -EFAULT;
		}
		cp_bytes += len;
	}

	/*
	 * Add new line character at the end.
	 */
	len = scnprintf(r_buf + cp_bytes, 4, "\n");
	cp_bytes += len;

	cp_bytes = simple_read_from_buffer(buffer, *lenp, ppos, r_buf, cp_bytes);
	*lenp = cp_bytes;
	kfree(r_buf);
	return 0;
}

/*
 * nss_rps_pri_map_parse()
 *	Sysctl handler for rps/pri mappings.
 */
static int nss_rps_pri_map_parse(struct ctl_table *ctl, void __user *buffer,
	size_t *lenp, loff_t *ppos, struct nss_rps_pri_map_parse_data *out)
{
	size_t cp_bytes = 0;
	char w_buf[5];
	loff_t w_offset = 0;
	char *str;
	unsigned int pri;
	int core, res;

	/*
	 * Buffer length cannot be different than 4 or 5.
	 */
	if (*lenp < 4 || *lenp > 5) {
		nss_warning("Buffer is not correct. Invalid lenght: %d\n", (int)*lenp);
		return -EINVAL;
	}

	/*
	 * It's a write operation
	 */
	cp_bytes = simple_write_to_buffer(w_buf, *lenp, &w_offset, buffer, 5);
	if (cp_bytes != *lenp) {
		nss_warning("failed to write to buffer\n");
		return -EFAULT;
	}

	str = w_buf;
	res = sscanf(str, "%u %d", &pri, &core);
	if (res != NSS_RPS_PRI_MAP_PARAM_FIELD_COUNT) {
		nss_warning("failed to read the buffer\n");
		return -EFAULT;
	}
	/*
	 * pri value cannot be higher than NSS_MAX_NUM_PRI.
	 */
	if (pri >= NSS_MAX_NUM_PRI) {
		nss_warning("invalid pri value: %d\n", pri);
		return -EINVAL;
	}

	/*
	 * Host core must be less than NSS_HOST_CORE.
	 */
	if (core >= NSS_HOST_CORES || core < NSS_N2H_RPS_PRI_DEFAULT) {
		nss_warning("invalid priority value: %d\n", core);
		return -EINVAL;
	}

	nss_info("priority: %d core: %d\n", pri, core);

	out->pri = pri;
	out->core = core;
	return 0;
}

/*
 * nss_rps_cfg_callback()
 *	Callback function for rps configuration.
 */
static void nss_rps_cfg_callback(void *app_data, struct nss_n2h_msg *nnm)
{
	struct nss_ctx_instance *nss_ctx =  (struct nss_ctx_instance *)app_data;
	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {

		/*
		 * Error, hence we are not updating the nss_rps
		 * Send a FAILURE to restore the current value
		 * to its previous state.
		 */
		nss_rps_cfg_pvt.response = NSS_FAILURE;
		complete(&nss_rps_cfg_pvt.complete);
		nss_warning("%px: RPS configuration failed : %d\n", nss_ctx,
								   nnm->cm.error);
		return;
	}

	nss_info("%px: RPS configuration succeeded: %d\n", nss_ctx,
							   nnm->cm.error);
	nss_ctx->rps_en = nnm->msg.rps_cfg.enable;
	nss_rps_cfg_pvt.response = NSS_SUCCESS;
	complete(&nss_rps_cfg_pvt.complete);
}

/*
 * nss_rps_pri_map_cfg_callback()
 *	Callback function for rps pri map configuration.
 */
static void nss_rps_pri_map_cfg_callback(void *app_data, struct nss_n2h_msg *nnm)
{
	if (nnm->cm.response != NSS_CMN_RESPONSE_ACK) {

		/*
		 * Error, hence we are not updating the nss_pri_map
		 * Send a failure to restore the current value
		 * to its previous state.
		 */
		nss_rps_cfg_pvt.response = NSS_FAILURE;
		complete(&nss_rps_cfg_pvt.complete);
		nss_warning("%px: RPS pri_map configuration failed : %d\n",
				app_data, nnm->cm.error);
		return;
	}

	nss_info("%px: RPS pri_map configuration succeeded: %d\n",
			app_data, nnm->cm.error);

	nss_rps_cfg_pvt.response = NSS_SUCCESS;
	complete(&nss_rps_cfg_pvt.complete);
}

/*
 * nss_rps_cfg()
 *	Send Message to NSS to enable RPS.
 */
static nss_tx_status_t nss_rps_cfg(struct nss_ctx_instance *nss_ctx, int enable_rps)
{
	struct nss_n2h_msg nnm;
	nss_tx_status_t nss_tx_status;
	int ret;

	down(&nss_rps_cfg_pvt.sem);
	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE, NSS_TX_METADATA_TYPE_N2H_RPS_CFG,
			sizeof(struct nss_n2h_rps),
			nss_rps_cfg_callback,
			(void *)nss_ctx);

	nnm.msg.rps_cfg.enable = enable_rps;

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting rps\n", nss_ctx);

		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_rps_cfg_pvt.complete, msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	/*
	 * ACK/NACK received from NSS FW
	 * If NACK: Handler function will restore nss_rps_config
	 * to previous state.
	 */
	if (NSS_FAILURE == nss_rps_cfg_pvt.response) {
		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	up(&nss_rps_cfg_pvt.sem);
	return NSS_SUCCESS;
}

/*
 * nss_rps_ipv4_hash_bitmap_cfg()
 *	Send Message to NSS to configure hash_bitmap.
 */
static nss_tx_status_t nss_rps_ipv4_hash_bitmap_cfg(struct nss_ctx_instance *nss_ctx, int hash_bitmap)
{
	struct nss_ipv4_msg nim;
	nss_tx_status_t nss_tx_status;

	down(&nss_rps_cfg_pvt.sem);
	nss_ipv4_msg_init(&nim, NSS_IPV4_RX_INTERFACE, NSS_IPV4_TX_RPS_HASH_BITMAP_CFG_MSG,
			sizeof(struct nss_ipv4_rps_hash_bitmap_cfg_msg),
			  NULL, NULL);

	nim.msg.rps_hash_bitmap.hash_bitmap = hash_bitmap;

	nss_tx_status = nss_ipv4_tx_sync(nss_ctx, &nim);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting rps\n", nss_ctx);

		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	up(&nss_rps_cfg_pvt.sem);
	return NSS_SUCCESS;
}

#ifdef NSS_DRV_IPV6_ENABLE
/*
 * nss_rps_ipv6_hash_bitmap_cfg()
 *	Send Message to NSS to configure hash_bitmap.
 */
static nss_tx_status_t nss_rps_ipv6_hash_bitmap_cfg(struct nss_ctx_instance *nss_ctx, int hash_bitmap)
{
	struct nss_ipv6_msg nim;
	nss_tx_status_t nss_tx_status;

	down(&nss_rps_cfg_pvt.sem);
	nss_ipv6_msg_init(&nim, NSS_IPV6_RX_INTERFACE, NSS_IPV6_TX_RPS_HASH_BITMAP_CFG_MSG,
			sizeof(struct nss_ipv4_rps_hash_bitmap_cfg_msg),
			  NULL, NULL);

	nim.msg.rps_hash_bitmap.hash_bitmap = hash_bitmap;

	nss_tx_status = nss_ipv6_tx_sync(nss_ctx, &nim);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting rps\n", nss_ctx);

		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	up(&nss_rps_cfg_pvt.sem);
	return NSS_SUCCESS;
}
#endif

/*
 * nss_rps_pri_map_cfg()
 *	Send Message to NSS to configure pri_map.
 */
static nss_tx_status_t nss_rps_pri_map_cfg(struct nss_ctx_instance *nss_ctx, int *pri_map)
{
	struct nss_n2h_msg nnm;
	struct nss_n2h_rps_pri_map *rps_pri_map;
	nss_tx_status_t nss_tx_status;
	int ret, i;

	down(&nss_rps_cfg_pvt.sem);
	nss_n2h_msg_init(&nnm, NSS_N2H_INTERFACE, NSS_TX_METADATA_TYPE_N2H_RPS_PRI_MAP_CFG,
			sizeof(struct nss_n2h_rps_pri_map),
			nss_rps_pri_map_cfg_callback,
			(void *)nss_ctx);

	rps_pri_map = &nnm.msg.rps_pri_map;

	/*
	 * Fill entries at pri_map.
	 */
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		rps_pri_map->pri_map[i] = pri_map[i];
	}

	nss_tx_status = nss_n2h_tx_msg(nss_ctx, &nnm);

	if (nss_tx_status != NSS_TX_SUCCESS) {
		nss_warning("%px: nss_tx error setting rps\n", nss_ctx);

		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	/*
	 * Blocking call, wait till we get ACK for this msg.
	 */
	ret = wait_for_completion_timeout(&nss_rps_cfg_pvt.complete, msecs_to_jiffies(NSS_CONN_CFG_TIMEOUT));
	if (ret == 0) {
		nss_warning("%px: Waiting for ack timed out\n", nss_ctx);
		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	/*
	 * ACK/NACK received from NSS FW
	 * If NACK: Handler function will restore nss_rps_config
	 * to previous state.
	 */
	if (NSS_FAILURE == nss_rps_cfg_pvt.response) {
		up(&nss_rps_cfg_pvt.sem);
		return NSS_FAILURE;
	}

	up(&nss_rps_cfg_pvt.sem);
	return NSS_SUCCESS;
}

/*
 * nss_rps_cfg_handler()
 *	Enable NSS RPS.
 */
static int nss_rps_cfg_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx;
	int ret, ret_rps, current_state, i;
	current_state = nss_rps_config;
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (ret != NSS_SUCCESS) {
		return ret;
	}

	if (!write) {
		return ret;
	}

	if (nss_rps_config == 0) {
		nss_info_always("Runtime disabling of NSS RPS not supported\n");
		return ret;
	}

	if (nss_rps_config != 1) {
		nss_info_always("Invalid input value. Valid values are 0 and 1\n");
		return ret;
	}

	for (i = 0; i < nss_top_main.num_nss; i++) {
		nss_ctx = &nss_top->nss[i];
		nss_info("Enabling NSS RPS\n");
		ret_rps = nss_rps_cfg(nss_ctx, 1);

		/*
		 * In here, we also need to revert the state of the previously enabled cores.
		 * However, runtime disabling is currently not supported since queues are not
		 * flushed in NSS FW.
		 * TODO: Flush queues in NSS FW.
		 */
		if (ret_rps != NSS_SUCCESS) {
			nss_warning("%px: rps enabling failed\n", nss_ctx);
			nss_rps_config = current_state;
			return ret_rps;
		}
	}
	return NSS_SUCCESS;
}

/*
 * nss_rps_hash_bitmap_cfg_handler()
 *	Configure NSS rps_hash_bitmap
 */
static int nss_rps_hash_bitmap_cfg_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[0];
	int ret, ret_ipv4, current_state;

	current_state = nss_rps_hash_bitmap;
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (ret != NSS_SUCCESS) {
		nss_rps_hash_bitmap = current_state;
		return ret;
	}

	if (!write) {
		return ret;
	}

	if (nss_rps_hash_bitmap <= (NSS_RPS_MAX_CORE_HASH_BITMAP)) {
		nss_info("Configuring NSS RPS hash_bitmap\n");
		ret_ipv4 = nss_rps_ipv4_hash_bitmap_cfg(nss_ctx, nss_rps_hash_bitmap);

		if (ret_ipv4 != NSS_SUCCESS) {
			nss_warning("%px: ipv4 hash_bitmap config message failed\n", nss_ctx);
			nss_rps_hash_bitmap = current_state;
			return ret_ipv4;
		}

#ifdef NSS_DRV_IPV6_ENABLE
		{
			int ret_ipv6;
			ret_ipv6 = nss_rps_ipv6_hash_bitmap_cfg(nss_ctx, nss_rps_hash_bitmap);

			if (ret_ipv6 != NSS_SUCCESS) {
				nss_warning("%px: ipv6 hash_bitmap config message failed\n", nss_ctx);
				nss_rps_hash_bitmap = current_state;
				if (nss_rps_ipv4_hash_bitmap_cfg(nss_ctx, nss_rps_hash_bitmap != NSS_SUCCESS)) {
					nss_warning("%px: ipv4 and ipv6 have different hash_bitmaps.\n", nss_ctx);
				}
				return ret_ipv6;
			}
		}
#endif
		return 0;
	}

	nss_info_always("Invalid input value. Valid values are less than %d\n", (NSS_RPS_MAX_CORE_HASH_BITMAP));
	return ret;
}

/* nss_rps_pri_map_cfg_handler()
 *	Configure NSS rps_pri_map
 */
static int nss_rps_pri_map_cfg_handler(struct ctl_table *ctl, int write,
				void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[0];

	int ret, ret_pri_map;
	struct nss_rps_pri_map_parse_data out, current_state;
	if (!write) {
		return nss_rps_pri_map_print(ctl, buffer, lenp, ppos, nss_rps_pri_map);
	}

	ret = nss_rps_pri_map_parse(ctl, buffer, lenp, ppos, &out);

	if (ret != NSS_SUCCESS) {
		nss_rps_pri_map_usage();
		return ret;
	}

	nss_info("Configuring NSS RPS Priority Map\n");
	current_state.pri = out.pri;
	current_state.core = nss_rps_pri_map[out.pri];
	nss_rps_pri_map[out.pri] = out.core;
	ret_pri_map = nss_rps_pri_map_cfg(nss_ctx, nss_rps_pri_map);
	if (ret_pri_map != NSS_SUCCESS) {
		nss_rps_pri_map[current_state.pri] = current_state.core;
		nss_warning("%px: pri_map config message failed\n", nss_ctx);
	}

	return ret_pri_map;
}

static struct ctl_table nss_rps_table[] = {
	{
		.procname	= "enable",
		.data		= &nss_rps_config,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_rps_cfg_handler,
	},
	{
		.procname	= "hash_bitmap",
		.data		= &nss_rps_hash_bitmap,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_rps_hash_bitmap_cfg_handler,
	},
	{
		.procname	= "pri_map",
		.data		= &nss_rps_pri_map[NSS_MAX_NUM_PRI],
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &nss_rps_pri_map_cfg_handler,
	},
	{ }
};

static struct ctl_table nss_rps_dir[] = {
	{
		.procname		= "rps",
		.mode			= 0555,
		.child			= nss_rps_table,
	},
	{ }
};

static struct ctl_table nss_rps_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_rps_dir,
	},
	{ }
};

static struct ctl_table nss_rps_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_rps_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_rps_header;

/*
 * nss_rps_pri_map_init_handler()
 *	Initialize pri_map for priority based rps selection.
 */
void nss_rps_pri_map_init_handler(void)
{
	int i;

	/*
	 Initialize the mapping table with the default values.
	*/
	for (i = 0; i < NSS_MAX_NUM_PRI; i++) {
		nss_rps_pri_map[i] = NSS_N2H_RPS_PRI_DEFAULT;
	}

}

/*
 * nss_rps_register_sysctl()
 */
void nss_rps_register_sysctl(void)
{

	/*
	 * rps sema init.
	 */
	sema_init(&nss_rps_cfg_pvt.sem, 1);
	init_completion(&nss_rps_cfg_pvt.complete);

	nss_rps_pri_map_init_handler();

	/*
	 * Register sysctl table.
	 */
	nss_rps_header = register_sysctl_table(nss_rps_root);
}

/*
 * nss_rps_unregister_sysctl()
 *      Unregister sysctl specific to rps
 */
void nss_rps_unregister_sysctl(void)
{
	/*
	 * Unregister sysctl table.
	 */
	if (nss_rps_header) {
		unregister_sysctl_table(nss_rps_header);
	}
}
