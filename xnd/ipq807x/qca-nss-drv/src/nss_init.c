/*
 **************************************************************************
 * Copyright (c) 2013-2021, The Linux Foundation. All rights reserved.
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
 * nss_init.c
 *	NSS init APIs
 *
 */
#include "nss_core.h"
#if (NSS_PM_SUPPORT == 1)
#include "nss_pm.h"
#endif
#include "nss_tx_rx_common.h"
#include "nss_data_plane.h"
#include "nss_capwap.h"
#include "nss_strings.h"

#include <nss_hal.h>

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/device.h>

#if (NSS_DT_SUPPORT == 1)
#if (NSS_FABRIC_SCALING_SUPPORT == 1)
#include <linux/fab_scaling.h>
#endif
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/reset.h>
#else
#include <mach/msm_nss.h>
#endif

#include <linux/sysctl.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>

/*
 * Global declarations
 */
int nss_ctl_redirect __read_mostly = 0;
int nss_ctl_debug __read_mostly = 0;
int nss_ctl_logbuf __read_mostly = 0;
int nss_jumbo_mru  __read_mostly = 0;
int nss_paged_mode __read_mostly = 0;
#if (NSS_SKB_REUSE_SUPPORT == 1)
int nss_max_reuse __read_mostly = PAGE_SIZE;
#endif
int nss_skip_nw_process = 0x0;
module_param(nss_skip_nw_process, int, S_IRUGO);

/*
 * PM client handle
 */
#if (NSS_PM_SUPPORT == 1)
static void *pm_client;
#endif

/*
 * Handler to send NSS messages
 */
struct clk *nss_core0_clk;
struct clk *nss_core1_clk;

/*
 * Handle fabric requests - only on new kernel
 */
#if (NSS_DT_SUPPORT == 1)
struct clk *nss_fab0_clk;
struct clk *nss_fab1_clk;
#endif

/*
 * Top level nss context structure
 */
struct nss_top_instance nss_top_main;
struct nss_cmd_buffer nss_cmd_buf;
struct nss_runtime_sampling nss_runtime_samples;
struct workqueue_struct *nss_wq;

/*
 * Work Queue to handle messages to Kernel
 */
nss_work_t *nss_work;

extern struct of_device_id nss_dt_ids[];

/*
 * nss_probe()
 *	HLOS device probe callback
 */
static inline int nss_probe(struct platform_device *nss_dev)
{
	return nss_hal_probe(nss_dev);
}

/*
 * nss_remove()
 *	HLOS device remove callback
 */
static inline int nss_remove(struct platform_device *nss_dev)
{
	return nss_hal_remove(nss_dev);
}

#if (NSS_DT_SUPPORT == 1)
/*
 * Platform Device ID for NSS core.
 */
struct of_device_id nss_dt_ids[] = {
	{ .compatible = "qcom,nss" },
	{ .compatible = "qcom,nss0" },
	{ .compatible = "qcom,nss1" },
	{},
};
MODULE_DEVICE_TABLE(of, nss_dt_ids);
#endif

/*
 * nss_driver
 *	Platform driver structure for NSS
 */
struct platform_driver nss_driver = {
	.probe	= nss_probe,
	.remove	= nss_remove,
	.driver	= {
		.name	= "qca-nss",
		.owner	= THIS_MODULE,
#if (NSS_DT_SUPPORT == 1)
		.of_match_table = of_match_ptr(nss_dt_ids),
#endif
	},
};

#if (NSS_FREQ_SCALE_SUPPORT == 1)
/*
 * nss_reset_frequency_stats_samples()
 *	Reset all frequency sampling state when auto scaling is turned off.
 */
static void nss_reset_frequency_stats_samples(void)
{
	nss_runtime_samples.buffer_index = 0;
	nss_runtime_samples.sum = 0;
	nss_runtime_samples.average = 0;
	nss_runtime_samples.sample_count = 0;
	nss_runtime_samples.message_rate_limit = 0;
	nss_runtime_samples.freq_scale_rate_limit_down = 0;
}

/*
 * nss_current_freq_handler()
 *	Handle Userspace Frequency Change Requests
 */
static int nss_current_freq_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret, i;

	BUG_ON(!nss_wq);

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (!*lenp || (*ppos && !write)) {
		printk("Frequency Set to %d\n", nss_cmd_buf.current_freq);
		*lenp = 0;
		return ret;
	}

	/*
	 * Check if frequency exists in frequency Table
	 */
	i = 0;
	while (i < NSS_FREQ_MAX_SCALE) {
		if (nss_runtime_samples.freq_scale[i].frequency == nss_cmd_buf.current_freq) {
			break;
		}
		i++;
	}
	if (i == NSS_FREQ_MAX_SCALE) {
		printk("Frequency not found. Please check Frequency Table\n");
		nss_cmd_buf.current_freq = nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency;
		return ret;
	}

	/*
	 * Turn off Auto Scale
	*/
	nss_cmd_buf.auto_scale = 0;
	nss_runtime_samples.freq_scale_ready = 0;
	nss_runtime_samples.freq_scale_index = i;

	nss_work = (nss_work_t *)kmalloc(sizeof(nss_work_t), GFP_ATOMIC);
	if (!nss_work) {
		nss_info("NSS Freq WQ kmalloc fail");
		return ret;
	}
	INIT_WORK((struct work_struct *)nss_work, nss_hal_wq_function);
	nss_work->frequency = nss_cmd_buf.current_freq;
	nss_work->stats_enable = 0;

	/*
	 * Ensure we start with a fresh set of samples later
	 */
	nss_reset_frequency_stats_samples();

	queue_work(nss_wq, (struct work_struct *)nss_work);

	return ret;
}

/*
 * nss_auto_scale_handler()
 *	Enables or Disable Auto Scaling
 */
static int nss_auto_scale_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (!*lenp || (*ppos && !write)) {
		return ret;
	}

	if (nss_cmd_buf.auto_scale != 1) {
		/*
		 * Is auto scaling currently enabled? If so, send the command to
		 * disable stats reporting to NSS
		 */
		if (nss_runtime_samples.freq_scale_ready != 0) {
			nss_cmd_buf.current_freq = nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency;
			nss_work = (nss_work_t *)kmalloc(sizeof(nss_work_t), GFP_ATOMIC);
			if (!nss_work) {
				nss_info("NSS Freq WQ kmalloc fail");
				return ret;
			}
			INIT_WORK((struct work_struct *)nss_work, nss_hal_wq_function);
			nss_work->frequency = nss_cmd_buf.current_freq;
			nss_work->stats_enable = 0;
			queue_work(nss_wq, (struct work_struct *)nss_work);
			nss_runtime_samples.freq_scale_ready = 0;

			/*
			 * The current samples would be stale later when scaling is
			 * enabled again, hence reset them
			 */
			nss_reset_frequency_stats_samples();
		}
		return ret;
	}

	/*
	 * Setup default values - Middle of Freq Scale Band
	 */
	nss_runtime_samples.freq_scale_index = 1;
	nss_runtime_samples.sample_count = 0;
	nss_runtime_samples.initialized = 0;
	nss_cmd_buf.current_freq = nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency;

	nss_work = (nss_work_t *)kmalloc(sizeof(nss_work_t), GFP_ATOMIC);
	if (!nss_work) {
		nss_info("NSS Freq WQ kmalloc fail");
		return ret;
	}
	INIT_WORK((struct work_struct *)nss_work, nss_hal_wq_function);
	nss_work->frequency = nss_cmd_buf.current_freq;
	nss_work->stats_enable = 1;
	queue_work(nss_wq, (struct work_struct *)nss_work);

	nss_cmd_buf.auto_scale = 0;
	nss_runtime_samples.freq_scale_ready = 1;

	return ret;
}

/*
 * nss_get_freq_table_handler()
 *	Display Support Freq and Ex how to Change.
 */
static int nss_get_freq_table_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret, i;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write) {
		return ret;
	}

	printk("Frequency Supported - ");

	i = 0;
	while (i < NSS_FREQ_MAX_SCALE) {
		if (nss_runtime_samples.freq_scale[i].frequency != NSS_FREQ_SCALE_NA) {
			printk("%d Hz ", nss_runtime_samples.freq_scale[i].frequency);
		}
		i++;
	}
	printk("\n");

	*lenp = 0;
	return ret;
}

/*
 * nss_get_average_inst_handler()
 *	Display AVG Inst Per Ms.
 */
static int nss_get_average_inst_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write) {
		return ret;
	}

	printk("Current Inst Per Ms %x\n", nss_runtime_samples.average);

	*lenp = 0;
	return ret;
}
#endif

#if (NSS_FW_DBG_SUPPORT == 1)
/*
 * nss_debug_handler()
 *	Enable NSS debug output
 */
static int nss_debug_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (!ret) {
		if ((write) && (nss_ctl_debug != 0)) {
			printk("Enabling NSS SPI Debug\n");
			nss_hal_debug_enable();
		}
	}

	return ret;
}
#endif

/*
 * nss_coredump_handler()
 *	Send Signal To Coredump NSS Cores
 */
static int nss_coredump_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[NSS_CORE_0];
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (!ret) {
		/*
		 * if nss_cmd_buf.coredump is not 0 or 1, panic will be disabled
		 * when NSS FW crashes, so OEM/ODM have a chance to use mdump
		 * to dump crash dump (coredump) and send dump to us for analysis.
		 */
		if ((write) && (nss_ctl_debug != 0) && nss_cmd_buf.coredump == 1) {
			printk("Coredumping to DDR\n");
			nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_TRIGGER_COREDUMP);
		}
	}

	return ret;
}

/*
 * nss_jumbo_mru_handler()
 *	Sysctl to modify nss_jumbo_mru
 */
static int nss_jumbo_mru_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	if (write) {
		nss_core_set_jumbo_mru(nss_jumbo_mru);
		nss_info("jumbo_mru set to %d\n", nss_jumbo_mru);
	}

	return ret;
}

/* nss_paged_mode_handler()
 *	Sysctl to modify nss_paged_mode.
 */

static int nss_paged_mode_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	if (write) {
		nss_core_set_paged_mode(nss_paged_mode);
		nss_info("paged_mode set to %d\n", nss_paged_mode);
	}

	return ret;
}

#if (NSS_SKB_REUSE_SUPPORT == 1)
/*
 * nss_get_min_reuse_handler()
 *	Sysctl to get min reuse sizes
 */
static int nss_get_min_reuse_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;
	struct nss_ctx_instance *nss_ctx = NULL;
	uint32_t core_id;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	printk("Min SKB reuse sizes - ");

	for (core_id = 0; core_id < NSS_CORE_MAX; core_id++) {
		nss_ctx = (struct nss_ctx_instance *)&nss_top_main.nss[core_id];
		printk("core %d: %d ", core_id, nss_core_get_min_reuse(nss_ctx));
	}

	printk("\n");
	*lenp = 0;
	return ret;
}

/*
 * nss_max_reuse_handler()
 *	Sysctl to modify nss_max_reuse
 */
static int nss_max_reuse_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	nss_max_reuse = nss_core_get_max_reuse();
	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret) {
		return ret;
	}

	if (write) {
		nss_core_set_max_reuse(nss_max_reuse);
		nss_info("max_reuse set to %d\n", nss_max_reuse);
	}

	return ret;
}

/*
 * sysctl-tuning for NSS driver SKB reuse
 */
static struct ctl_table nss_skb_reuse_table[] = {
	{
		.procname		= "min_sizes",
		.data			= NULL,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler	= &nss_get_min_reuse_handler,
	},
	{
		.procname		= "max_size",
		.data			= &nss_max_reuse,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler	= &nss_max_reuse_handler,
	},
	{ }
};
#endif

#if (NSS_FREQ_SCALE_SUPPORT == 1)
/*
 * sysctl-tuning infrastructure.
 */
static struct ctl_table nss_freq_table[] = {
	{
		.procname		= "current_freq",
		.data			= &nss_cmd_buf.current_freq,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler	= &nss_current_freq_handler,
	},
	{
		.procname		= "freq_table",
		.data			= &nss_cmd_buf.max_freq,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler	= &nss_get_freq_table_handler,
	},
	{
		.procname		= "auto_scale",
		.data			= &nss_cmd_buf.auto_scale,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler	= &nss_auto_scale_handler,
	},
	{
		.procname		= "inst_per_sec",
		.data			= &nss_cmd_buf.average_inst,
		.maxlen			= sizeof(int),
		.mode			= 0644,
		.proc_handler	= &nss_get_average_inst_handler,
	},
	{ }
};
#endif

static struct ctl_table nss_general_table[] = {
	{
		.procname               = "redirect",
		.data                   = &nss_ctl_redirect,
		.maxlen                 = sizeof(int),
		.mode                   = 0644,
		.proc_handler		= proc_dointvec,
	},
#if (NSS_FW_DBG_SUPPORT == 1)
	{
		.procname               = "debug",
		.data                   = &nss_ctl_debug,
		.maxlen                 = sizeof(int),
		.mode                   = 0644,
		.proc_handler		= &nss_debug_handler,
	},
#endif
	{
		.procname               = "coredump",
		.data                   = &nss_cmd_buf.coredump,
		.maxlen                 = sizeof(int),
		.mode                   = 0644,
		.proc_handler		= &nss_coredump_handler,
	},
	{
		.procname               = "logbuf",
		.data                   = &nss_ctl_logbuf,
		.maxlen                 = sizeof(int),
		.mode                   = 0644,
		.proc_handler		= &nss_logbuffer_handler,
	},
	{
		.procname               = "jumbo_mru",
		.data                   = &nss_jumbo_mru,
		.maxlen                 = sizeof(int),
		.mode                   = 0644,
		.proc_handler           = &nss_jumbo_mru_handler,
	},
	{
		.procname               = "paged_mode",
		.data                   = &nss_paged_mode,
		.maxlen                 = sizeof(int),
		.mode                   = 0644,
		.proc_handler           = &nss_paged_mode_handler,
	},
	{ }
};

static struct ctl_table nss_init_dir[] = {
#if (NSS_FREQ_SCALE_SUPPORT == 1)
	{
		.procname               = "clock",
		.mode                   = 0555,
		.child                  = nss_freq_table,
	},
#endif
	{
		.procname               = "general",
		.mode                   = 0555,
		.child                  = nss_general_table,
	},
#if (NSS_SKB_REUSE_SUPPORT == 1)
	{
		.procname               = "skb_reuse",
		.mode                   = 0555,
		.child                  = nss_skb_reuse_table,
	},
#endif
	{ }
};

static struct ctl_table nss_root_dir[] = {
	{
		.procname		= "nss",
		.mode			= 0555,
		.child			= nss_init_dir,
	},
	{ }
};

static struct ctl_table nss_root[] = {
	{
		.procname		= "dev",
		.mode			= 0555,
		.child			= nss_root_dir,
	},
	{ }
};

static struct ctl_table_header *nss_dev_header;

/*
 * nss_init()
 *	Registers nss driver
 */
static int __init nss_init(void)
{
#if (NSS_DT_SUPPORT == 1)
	struct device_node *cmn = NULL;
#endif
	nss_info("Init NSS driver");

#if (NSS_DT_SUPPORT == 1)
	/*
	 * Get reference to NSS common device node
	 */
	cmn = of_find_node_by_name(NULL, "nss-common");
	if (!cmn) {
		nss_info_always("qca-nss-drv.ko is loaded for symbol link\n");
		return 0;
	}
	of_node_put(cmn);

	/*
	 * Pick up HAL by target information
	 */
#if defined(NSS_HAL_IPQ806X_SUPPORT)
	if (of_machine_is_compatible("qcom,ipq8064") || of_machine_is_compatible("qcom,ipq8062")) {
		nss_top_main.hal_ops = &nss_hal_ipq806x_ops;
		nss_top_main.data_plane_ops = &nss_data_plane_gmac_ops;
		nss_top_main.num_nss = 2;
	}
#endif
#if defined(NSS_HAL_IPQ807x_SUPPORT)
	if (of_machine_is_compatible("qcom,ipq807x") || of_machine_is_compatible("qcom,ipq8074")) {
		nss_top_main.hal_ops = &nss_hal_ipq807x_ops;
		nss_top_main.data_plane_ops = &nss_data_plane_ops;
#if defined(NSS_MEM_PROFILE_LOW)
		nss_top_main.num_nss = 1;
#else
		nss_top_main.num_nss = 2;
#endif
	}
#endif
#if defined(NSS_HAL_IPQ60XX_SUPPORT)
	if (of_machine_is_compatible("qcom,ipq6018")) {
		nss_top_main.hal_ops = &nss_hal_ipq60xx_ops;
		nss_top_main.data_plane_ops = &nss_data_plane_ops;
		nss_top_main.num_nss = 1;
	}
#endif
#if defined(NSS_HAL_IPQ50XX_SUPPORT)
	if (of_machine_is_compatible("qcom,ipq5018")) {
		nss_top_main.hal_ops = &nss_hal_ipq50xx_ops;
		nss_top_main.data_plane_ops = &nss_data_plane_ops;
		nss_top_main.num_nss = 1;
	}
#endif
#if defined(NSS_HAL_FSM9010_SUPPORT)
	if (of_machine_is_compatible("qcom,fsm9010")) {
		nss_top_main.hal_ops = &nss_hal_fsm9010_ops;
		nss_top_main.data_plane_ops = &nss_data_plane_gmac_ops;
		nss_top_main.num_nss = 1;
	}
#endif
	if (!nss_top_main.hal_ops) {
		nss_info_always("No supported HAL compiled on this platform\n");
		return -EFAULT;
	}
#else
	/*
	 * For banana, only ipq806x is supported
	 */
	nss_top_main.hal_ops = &nss_hal_ipq806x_ops;
	nss_top_main.data_plane_ops = &nss_data_plane_gmac_ops;
	nss_top_main.num_nss = 2;

#endif /* NSS_DT_SUPPORT */
	nss_top_main.nss_hal_common_init_done = false;

	/*
	 * Initialize data_plane workqueue
	 */
	if (nss_data_plane_init_delay_work()) {
		nss_warning("Error initializing nss_data_plane_workqueue\n");
		return -EFAULT;
	}

	/*
	 * Enable spin locks
	 */
	spin_lock_init(&(nss_top_main.lock));
	spin_lock_init(&(nss_top_main.stats_lock));
	mutex_init(&(nss_top_main.wq_lock));

	/*
	 * Enable NSS statistics
	 */
	nss_stats_init();

	/*
	 * Enable NSS statistics names.
	 */
	nss_strings_init();

	/*
	 * Register sysctl table.
	 */
	nss_dev_header = register_sysctl_table(nss_root);

	/*
	 * Registering sysctl for ipv4/6 specific config.
	 */
	nss_ipv4_register_sysctl();
#ifdef NSS_DRV_IPV6_ENABLE
	nss_ipv6_register_sysctl();
#endif

	/*
	 * Registering sysctl for n2h specific config.
	 */
	if (nss_top_main.num_nss == 1) {
		nss_n2h_single_core_register_sysctl();
	} else {
		nss_n2h_multi_core_register_sysctl();
	}

	/*
	 * Registering sysctl for rps specific config.
	 */
	nss_rps_register_sysctl();

#ifdef NSS_DRV_C2C_ENABLE
	/*
	 * Registering sysctl for c2c_tx specific config.
	 */
	nss_c2c_tx_register_sysctl();
#endif

	/*
	 * Registering sysctl for for printing non zero stats.
	 */
	nss_stats_register_sysctl();

	/*
	 * Register sysctl for project config
	 */
	nss_project_register_sysctl();

	/*
	 * Registering sysctl for pppoe specific config.
	 */
	nss_pppoe_register_sysctl();

	/*
	 * Setup Runtime Sample values
	 */
	nss_runtime_samples.freq_scale_index = 1;
	nss_runtime_samples.freq_scale_ready = 0;
	nss_runtime_samples.freq_scale_rate_limit_down = 0;
	nss_runtime_samples.buffer_index = 0;
	nss_runtime_samples.sum = 0;
	nss_runtime_samples.sample_count = 0;
	nss_runtime_samples.average = 0;
	nss_runtime_samples.message_rate_limit = 0;
	nss_runtime_samples.initialized = 0;

	nss_cmd_buf.current_freq = nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency;

	/*
	 * Initial Workqueue
	 */
	nss_wq = create_workqueue("nss_freq_queue");

#if (NSS_PM_SUPPORT == 1)
	/*
	 * Initialize NSS Bus PM module
	 */
	nss_pm_init();

	/*
	 * Register with Bus driver
	 */
	pm_client = nss_pm_client_register(NSS_PM_CLIENT_NETAP);
	if (!pm_client) {
		nss_warning("Error registering with PM driver");
	}
#endif

	/*
	 * Initialize mtu size needed as start
	 */
	nss_top_main.prev_mtu_sz = ETH_DATA_LEN;

	/*
	 * register panic handler and timeout control
	 */
	nss_coredump_notify_register();
	nss_coredump_init_delay_work();

#ifdef NSS_DRV_CAPWAP_ENABLE
	/*
	 * Init capwap
	 */
	nss_capwap_init();
#endif

#ifdef NSS_DRV_QRFS_ENABLE
	/*
	 * Init QRFS
	 */
	nss_qrfs_init();
#endif

#ifdef NSS_DRV_C2C_ENABLE
	/*
	 * Init c2c_tx
	 */
	nss_c2c_tx_init();
#endif

#ifdef NSS_DRV_PVXLAN_ENABLE
	/*
	 * Init pvxlan
	 */
	nss_pvxlan_init();
#endif

#ifdef NSS_DRV_CLMAP_ENABLE
	/*
	 * Init clmap
	 */
	nss_clmap_init();
#endif

	/*
	 * INIT ppe on supported platform
	 */
#ifdef NSS_DRV_PPE_ENABLE
	nss_ppe_init();
#endif

#ifdef NSS_DRV_DMA_ENABLE
	nss_dma_init();
#endif

	/*
	 * Init Wi-Fi mesh
	 */
#ifdef NSS_DRV_WIFI_MESH_ENABLE
	nss_wifi_mesh_init();
#endif

	/*
	 * Register platform_driver
	 */
	return platform_driver_register(&nss_driver);
}

/*
 * nss_cleanup()
 *	Unregisters nss driver
 */
static void __exit nss_cleanup(void)
{
	nss_info("Exit NSS driver");

	if (nss_dev_header)
		unregister_sysctl_table(nss_dev_header);

	/*
	 * Unregister n2h specific sysctl
	 */
	nss_n2h_unregister_sysctl();

	/*
	 * Unregister rps specific sysctl
	 */
	nss_rps_unregister_sysctl();

#ifdef NSS_DRV_C2C_ENABLE
	/*
	 * Unregister c2c_tx specific sysctl
	 */
	nss_c2c_tx_unregister_sysctl();
#endif

	/*
	 * Unregister pppoe specific sysctl
	 */
	nss_pppoe_unregister_sysctl();

	/*
	 * Unregister ipv4/6 specific sysctl and free allocated to connection tables
	 */
	nss_ipv4_unregister_sysctl();
	nss_ipv4_free_conn_tables();

#ifdef NSS_DRV_IPV6_ENABLE
	nss_ipv6_unregister_sysctl();
	nss_ipv6_free_conn_tables();
#endif

	nss_project_unregister_sysctl();
	nss_data_plane_destroy_delay_work();

	/*
	 * cleanup ppe on supported platform
	 */
#ifdef NSS_DRV_PPE_ENABLE
	nss_ppe_free();
#endif

	platform_driver_unregister(&nss_driver);
}

module_init(nss_init);
module_exit(nss_cleanup);

MODULE_DESCRIPTION("QCA NSS Driver");
MODULE_AUTHOR("Qualcomm Atheros Inc");
MODULE_LICENSE("Dual BSD/GPL");
