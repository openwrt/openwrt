// SPDX-License-Identifier: GPL-2.0-or-later
/* Device Tree support for Alpine platforms. */

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

#include <linux/amba/bus.h>
#include <linux/clocksource.h>
#include <linux/clk-provider.h>
#include <linux/export.h>
#include <linux/irqchip.h>
#include <linux/irqchip/arm-gic.h>

#include <asm/mach/arch.h>
#include <asm/mach/time.h>

#include <mach/al_hal_serdes.h>
#include <mach/al_hal_iomap.h>
#include <mach/al_fabric.h>
#include <mach/alpine_machine.h>

#include "core.h"

#define WDTLOAD			0x000
	#define LOAD_MIN	0x00000001
#define WDTCONTROL		0x008
	/* control register masks */
	#define	INT_ENABLE	(1 << 0)
	#define	RESET_ENABLE	(1 << 1)
#define WDTLOCK			0xC00
	#define	UNLOCK		0x1ACCE551

#define SERDES_NUM_GROUPS	4
#define SERDES_GROUP_SIZE	0x400

static void __iomem *wd0_base;
static void __iomem *serdes_base;

static void __init al_timer_init(void)
{
	struct device_node *np;

	/* Find the first watchdog and make sure it is not disabled */
	np = of_find_compatible_node(
			NULL, NULL, "arm,sp805");

	if (np && of_device_is_available(np)) {
		wd0_base = of_iomap(np, 0);
		BUG_ON(!wd0_base);
	} else {
		wd0_base = NULL;
	}

	/* Timer initialization */
	of_clk_init(NULL);
	timer_probe();
}

static void al_watchdog_reset(void)
{
	if (!wd0_base) {
		pr_err("%s: Not supported!\n", __func__);
	} else {
		writel(UNLOCK, wd0_base + WDTLOCK);
		writel(LOAD_MIN, wd0_base + WDTLOAD);
		writel(INT_ENABLE | RESET_ENABLE, wd0_base + WDTCONTROL);
	}

	while (1)
		;
}

static void al_restart(enum reboot_mode mode, const char *cmd)
{
	al_watchdog_reset();
}

static void __init al_serdes_resource_init(void)
{
	struct device_node *np;

	/* Find the serdes node and make sure it is not disabled */
	np = of_find_compatible_node(NULL, NULL, "annapurna-labs,al-serdes");

	if (np && of_device_is_available(np)) {
		serdes_base = of_iomap(np, 0);
		BUG_ON(!serdes_base);
	} else {
		pr_err("%s: init serdes regs base failed!\n", __func__);
		serdes_base = NULL;
	}
}

void __iomem *alpine_serdes_resource_get(u32 group)
{
	void __iomem *base = NULL;

	if (group >= SERDES_NUM_GROUPS)
		return NULL;

	if (serdes_base)
		base = serdes_base + group * SERDES_GROUP_SIZE;

	return base;
}
EXPORT_SYMBOL(alpine_serdes_resource_get);

static struct alpine_serdes_eth_group_mode {
	struct mutex			lock;
	enum alpine_serdes_eth_mode	mode;
	bool				mode_set;
} alpine_serdes_eth_group_mode[SERDES_NUM_GROUPS] = {
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[0].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[1].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[2].lock),
		.mode_set = false,
	},
	{
		.lock = __MUTEX_INITIALIZER(alpine_serdes_eth_group_mode[3].lock),
		.mode_set = false,
	}};

int alpine_serdes_eth_mode_set(
	u32				group,
	enum alpine_serdes_eth_mode	mode)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	if (!serdes_base)
		return -EINVAL;

	if (group >= SERDES_NUM_GROUPS)
		return -EINVAL;

	mutex_lock(&group_mode->lock);

	if (!group_mode->mode_set || (group_mode->mode != mode)) {
		struct al_serdes_obj obj;
		struct al_serdes_adv_tx_params tx_params[AL_SRDS_NUM_LANES];
		struct al_serdes_adv_rx_params rx_params[AL_SRDS_NUM_LANES];
		int i;

		al_serdes_handle_init(serdes_base, &obj);

		/* save group params */
		for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
			al_serdes_tx_advanced_params_get(
					&obj,
					group,
					i,
					&tx_params[i]);
			al_serdes_rx_advanced_params_get(
					&obj,
					group,
					i,
					&rx_params[i]);
		}

		if (mode == ALPINE_SERDES_ETH_MODE_SGMII)
			al_serdes_mode_set_sgmii(&obj, group);
		else
			al_serdes_mode_set_kr(&obj, group);

		/* restore group params */
		for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
			al_serdes_tx_advanced_params_set(
					&obj,
					group,
					i,
					&tx_params[i]);
			al_serdes_rx_advanced_params_set(
					&obj,
					group,
					i,
					&rx_params[i]);
		}

		group_mode->mode = mode;
		group_mode->mode_set = true;
	}

	mutex_unlock(&group_mode->lock);

	return 0;
}
EXPORT_SYMBOL(alpine_serdes_eth_mode_set);

void alpine_serdes_eth_group_lock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	mutex_lock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_lock);

void alpine_serdes_eth_group_unlock(u32 group)
{
	struct alpine_serdes_eth_group_mode *group_mode =
		&alpine_serdes_eth_group_mode[group];

	mutex_unlock(&group_mode->lock);
}
EXPORT_SYMBOL(alpine_serdes_eth_group_unlock);

static void __init al_init(void)
{
	/*
	 * Power Management Services Initialization
	 * When running in SMP this should be done earlier
	 */
#ifndef CONFIG_SMP
	alpine_cpu_pm_init();
#endif

	/* fabric uses a notifier for device registration,
	 * Hence it must be initialized before registering
	 * any devices
	 **/
	al_fabric_init();

	al_serdes_resource_init();

	of_platform_default_populate(NULL, NULL, NULL);
}

static const char *al_match[] __initdata = {
	"annapurna-labs,alpine",
	NULL,
};

DT_MACHINE_START(AL_DT, "AnnapurnaLabs Alpine (Device Tree)")
	.smp		= smp_ops(al_smp_ops),
	.init_time	= al_timer_init,
	.init_machine	= al_init,
	.dt_compat	= al_match,
	.restart	= al_restart,
MACHINE_END
