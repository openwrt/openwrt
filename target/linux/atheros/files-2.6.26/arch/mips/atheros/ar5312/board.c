/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

/*
 * Platform devices for Atheros SoCs
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <asm/bootinfo.h>
#include <asm/reboot.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <ar531x.h>
#include <linux/leds.h>

#define NO_PHY 0x1f

static int is_5312 = 0;
static struct platform_device *ar5312_devs[6];

static struct resource ar5312_eth0_res[] = {
	{
		.name = "eth0_membase",
		.flags = IORESOURCE_MEM,
		.start = KSEG1ADDR(AR531X_ENET0),
		.end = KSEG1ADDR(AR531X_ENET0 + 0x2000),
	},
	{
		.name = "eth0_irq",
		.flags = IORESOURCE_IRQ,
		.start = AR5312_IRQ_ENET0_INTRS,
		.end = AR5312_IRQ_ENET0_INTRS,
	},
};
static struct ar531x_eth ar5312_eth0_data = {
	.phy = NO_PHY,
	.mac = 0,
	.reset_base = AR531X_RESET,
	.reset_mac = AR531X_RESET_ENET0,
	.reset_phy = AR531X_RESET_EPHY0,
	.phy_base = KSEG1ADDR(AR531X_ENET0),
};

static struct resource ar5312_eth1_res[] = {
	{
		.name = "eth1_membase",
		.flags = IORESOURCE_MEM,
		.start = KSEG1ADDR(AR531X_ENET1),
		.end = KSEG1ADDR(AR531X_ENET1 + 0x2000),
	},
	{
		.name = "eth1_irq",
		.flags = IORESOURCE_IRQ,
		.start = AR5312_IRQ_ENET1_INTRS,
		.end = AR5312_IRQ_ENET1_INTRS,
	},
};
static struct ar531x_eth ar5312_eth1_data = {
	.phy = NO_PHY,
	.mac = 1,
	.reset_base = AR531X_RESET,
	.reset_mac = AR531X_RESET_ENET1,
	.reset_phy = AR531X_RESET_EPHY1,
	.phy_base = KSEG1ADDR(AR531X_ENET1),
};

static struct platform_device ar5312_eth[] = {
	{
		.id = 0,
		.name = "ar531x-eth",
		.dev.platform_data = &ar5312_eth0_data,
		.resource = ar5312_eth0_res,
		.num_resources = ARRAY_SIZE(ar5312_eth0_res)
	},
	{
		.id = 1,
		.name = "ar531x-eth",
		.dev.platform_data = &ar5312_eth1_data,
		.resource = ar5312_eth1_res,
		.num_resources = ARRAY_SIZE(ar5312_eth1_res)
	},
};


/*
 * AR2312/3 ethernet uses the PHY of ENET0, but the MAC
 * of ENET1. Atheros calls it 'twisted' for a reason :)
 */
static struct resource ar231x_eth0_res[] = {
	{
		.name = "eth0_membase",
		.flags = IORESOURCE_MEM,
		.start = KSEG1ADDR(AR531X_ENET1),
		.end = KSEG1ADDR(AR531X_ENET1 + 0x2000),
	},
	{
		.name = "eth0_irq",
		.flags = IORESOURCE_IRQ,
		.start = AR5312_IRQ_ENET1_INTRS,
		.end = AR5312_IRQ_ENET1_INTRS,
	},
};
static struct ar531x_eth ar231x_eth0_data = {
	.phy = 1,
	.mac = 1,
	.reset_base = AR531X_RESET,
	.reset_mac = AR531X_RESET_ENET1,
	.reset_phy = AR531X_RESET_EPHY1,
	.phy_base = KSEG1ADDR(AR531X_ENET0),
};
static struct platform_device ar231x_eth0 = {
	.id = 0,
	.name = "ar531x-eth",
	.dev.platform_data = &ar231x_eth0_data,
	.resource = ar231x_eth0_res,
	.num_resources = ARRAY_SIZE(ar231x_eth0_res)
};


static struct platform_device ar5312_wmac[] = {
	{
		.id = 0,
		.name = "ar531x-wmac",
	},
	{
		.id = 1,
		.name = "ar531x-wmac",
	},
};

static struct physmap_flash_data ar5312_flash_data = {
	.width	  = 2,
};

static struct resource ar5312_flash_resource = {
	.start	= AR531X_FLASH,
	.end	= AR531X_FLASH + 0x800000 - 1,
	.flags	= IORESOURCE_MEM,
};

static struct platform_device ar5312_physmap_flash = {
	.name	   = "physmap-flash",
	.id	 = 0,
	.dev		= {
		.platform_data  = &ar5312_flash_data,
	},
	.num_resources  = 1,
	.resource   = &ar5312_flash_resource,
};

#ifdef CONFIG_LEDS_GPIO
static struct gpio_led ar5312_leds[] = {
	{ .name = "wlan", .gpio = 0, .active_low = 1, },
};

static const struct gpio_led_platform_data ar5312_led_data = {
	.num_leds = ARRAY_SIZE(ar5312_leds),
	.leds = (void *) ar5312_leds,
};

static struct platform_device ar5312_gpio_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = (void *) &ar5312_led_data,
	}
};
#endif

/*
 * NB: This mapping size is larger than the actual flash size,
 * but this shouldn't be a problem here, because the flash
 * will simply be mapped multiple times.
 */
static char __init *ar5312_flash_limit(void)
{
	u32 ctl;
	/*
	 * Configure flash bank 0.
	 * Assume 8M window size. Flash will be aliased if it's smaller
	 */
	ctl = FLASHCTL_E |
		FLASHCTL_AC_8M |
		FLASHCTL_RBLE |
		(0x01 << FLASHCTL_IDCY_S) |
		(0x07 << FLASHCTL_WST1_S) |
		(0x07 << FLASHCTL_WST2_S) |
		(sysRegRead(AR531X_FLASHCTL0) & FLASHCTL_MW);

	sysRegWrite(AR531X_FLASHCTL0, ctl);

	/* Disable other flash banks */
	sysRegWrite(AR531X_FLASHCTL1,
		sysRegRead(AR531X_FLASHCTL1) & ~(FLASHCTL_E | FLASHCTL_AC));

	sysRegWrite(AR531X_FLASHCTL2,
		sysRegRead(AR531X_FLASHCTL2) & ~(FLASHCTL_E | FLASHCTL_AC));

	return (char *) KSEG1ADDR(AR531X_FLASH + 0x800000);
}

static struct ar531x_config __init *init_wmac(int unit)
{
	struct ar531x_config *config;

	config = (struct ar531x_config *) kzalloc(sizeof(struct ar531x_config), GFP_KERNEL);
	config->board = board_config;
	config->radio = radio_config;
	config->unit = unit;
	config->tag = (u_int16_t) ((sysRegRead(AR531X_REV) >> AR531X_REV_WMAC_MIN_S) & AR531X_REV_CHIP);

	return config;
}

int __init ar5312_init_devices(void)
{
	struct ar531x_boarddata *bcfg;
	char *radio, *c;
	int dev = 0;
	uint32_t fctl = 0;

	if (!is_5312)
		return 0;

	/* Locate board/radio config data */
	ar531x_find_config(ar5312_flash_limit());
	bcfg = (struct ar531x_boarddata *) board_config;


	/*
	 * Chip IDs and hardware detection for some Atheros
	 * models are really broken!
	 *
	 * Atheros uses a disabled WMAC0 and Silicon ID of AR5312
	 * as indication for AR2312, which is otherwise
	 * indistinguishable from the real AR5312.
	 */
	if (radio_config) {
		radio = radio_config + AR531X_RADIO_MASK_OFF;
		if ((*((u32 *) radio) & AR531X_RADIO0_MASK) == 0)
			bcfg->config |= BD_ISCASPER;
	} else
		radio = NULL;

	/* AR2313 has CPU minor rev. 10 */
	if ((current_cpu_data.processor_id & 0xff) == 0x0a)
		mips_machtype = MACH_ATHEROS_AR2313;

	/* AR2312 shares the same Silicon ID as AR5312 */
	else if (bcfg->config & BD_ISCASPER)
		mips_machtype = MACH_ATHEROS_AR2312;

	/* Everything else is probably AR5312 or compatible */
	else
		mips_machtype = MACH_ATHEROS_AR5312;

	ar5312_eth0_data.board_config = board_config;
	ar5312_eth1_data.board_config = board_config;

	/* fixup flash width */
	fctl = sysRegRead(AR531X_FLASHCTL) & FLASHCTL_MW;
	switch (fctl) {
		case FLASHCTL_MWx16:
			ar5312_flash_data.width = 2;
			break;
		case FLASHCTL_MWx8:
		default:
			ar5312_flash_data.width = 1;
			break;
	}

	ar5312_devs[dev++] = &ar5312_physmap_flash;

#ifdef CONFIG_LEDS_GPIO
	ar5312_leds[0].gpio = bcfg->sysLedGpio;
	ar5312_devs[dev++] = &ar5312_gpio_leds;
#endif

	if (!memcmp(bcfg->enet0Mac, "\xff\xff\xff\xff\xff\xff", 6))
		memcpy(bcfg->enet0Mac, bcfg->enet1Mac, 6);

	if (memcmp(bcfg->enet0Mac, bcfg->enet1Mac, 6) == 0) {
		/* ENET0 and ENET1 have the same mac.
		 * Increment the one from ENET1 */
		c = bcfg->enet1Mac + 5;
		while ((c >= (char *) bcfg->enet1Mac) && !(++(*c)))
			c--;
	}

	switch(mips_machtype) {
		case MACH_ATHEROS_AR5312:
			ar5312_eth0_data.macaddr = bcfg->enet0Mac;
			ar5312_eth1_data.macaddr = bcfg->enet1Mac;
			ar5312_devs[dev++] = &ar5312_eth[0];
			ar5312_devs[dev++] = &ar5312_eth[1];
			break;
		case MACH_ATHEROS_AR2312:
		case MACH_ATHEROS_AR2313:
			ar231x_eth0_data.macaddr = bcfg->enet0Mac;
			ar5312_devs[dev++] = &ar231x_eth0;
			ar5312_flash_data.width = 1;
			break;
	}

	if (radio) {
		if (mips_machtype == MACH_ATHEROS_AR5312) {
			if (*((u32 *) radio) & AR531X_RADIO0_MASK) {
				ar5312_wmac[0].dev.platform_data = init_wmac(0);
				ar5312_devs[dev++] = &ar5312_wmac[0];
			}
		}
		if (*((u32 *) radio) & AR531X_RADIO1_MASK) {
			ar5312_wmac[1].dev.platform_data = init_wmac(1);
			ar5312_devs[dev++] = &ar5312_wmac[1];
		}
	}

	return platform_add_devices(ar5312_devs, dev);
}


static void ar5312_halt(void)
{
	 while (1);
}

static void ar5312_power_off(void)
{
	 ar5312_halt();
}


static void ar5312_restart(char *command)
{
	/* reset the system */
	for(;;) sysRegWrite(AR531X_RESET, AR531X_RESET_SYSTEM);
}


/*
 * This table is indexed by bits 5..4 of the CLOCKCTL1 register
 * to determine the predevisor value.
 */
static int __initdata CLOCKCTL1_PREDIVIDE_TABLE[4] = {
	1,
	2,
	4,
	5
};


static unsigned int __init ar5312_cpu_frequency(void)
{
	unsigned int result;
	unsigned int predivide_mask, predivide_shift;
	unsigned int multiplier_mask, multiplier_shift;
	unsigned int clockCtl1, preDivideSelect, preDivisor, multiplier;
	unsigned int doubler_mask;
	unsigned int wisoc_revision;

	/* Trust the bootrom's idea of cpu frequency. */
	if ((result = sysRegRead(AR5312_SCRATCH)))
		return result;

	wisoc_revision = (sysRegRead(AR531X_REV) & AR531X_REV_MAJ) >> AR531X_REV_MAJ_S;
	if (wisoc_revision == AR531X_REV_MAJ_AR2313) {
		predivide_mask = AR2313_CLOCKCTL1_PREDIVIDE_MASK;
		predivide_shift = AR2313_CLOCKCTL1_PREDIVIDE_SHIFT;
		multiplier_mask = AR2313_CLOCKCTL1_MULTIPLIER_MASK;
		multiplier_shift = AR2313_CLOCKCTL1_MULTIPLIER_SHIFT;
		doubler_mask = AR2313_CLOCKCTL1_DOUBLER_MASK;
	} else { /* AR5312 and AR2312 */
		predivide_mask = AR5312_CLOCKCTL1_PREDIVIDE_MASK;
		predivide_shift = AR5312_CLOCKCTL1_PREDIVIDE_SHIFT;
		multiplier_mask = AR5312_CLOCKCTL1_MULTIPLIER_MASK;
		multiplier_shift = AR5312_CLOCKCTL1_MULTIPLIER_SHIFT;
		doubler_mask = AR5312_CLOCKCTL1_DOUBLER_MASK;
	}

	/*
	 * Clocking is derived from a fixed 40MHz input clock.
	 *
	 *  cpuFreq = InputClock * MULT (where MULT is PLL multiplier)
	 *  sysFreq = cpuFreq / 4	   (used for APB clock, serial,
	 *							   flash, Timer, Watchdog Timer)
	 *
	 *  cntFreq = cpuFreq / 2	   (use for CPU count/compare)
	 *
	 * So, for example, with a PLL multiplier of 5, we have
	 *
	 *  cpuFreq = 200MHz
	 *  sysFreq = 50MHz
	 *  cntFreq = 100MHz
	 *
	 * We compute the CPU frequency, based on PLL settings.
	 */

	clockCtl1 = sysRegRead(AR5312_CLOCKCTL1);
	preDivideSelect = (clockCtl1 & predivide_mask) >> predivide_shift;
	preDivisor = CLOCKCTL1_PREDIVIDE_TABLE[preDivideSelect];
	multiplier = (clockCtl1 & multiplier_mask) >> multiplier_shift;

	if (clockCtl1 & doubler_mask) {
		multiplier = multiplier << 1;
	}
	return (40000000 / preDivisor) * multiplier;
}

static inline int ar5312_sys_frequency(void)
{
	return ar5312_cpu_frequency() / 4;
}

static void __init ar5312_time_init(void)
{
	mips_hpt_frequency = ar5312_cpu_frequency() / 2;
}


void __init ar5312_prom_init(void)
{
	u32 memsize, memcfg, bank0AC, bank1AC;

	is_5312 = 1;

	/* Detect memory size */
	memcfg = sysRegRead(AR531X_MEM_CFG1);
	bank0AC = (memcfg & MEM_CFG1_AC0) >> MEM_CFG1_AC0_S;
	bank1AC = (memcfg & MEM_CFG1_AC1) >> MEM_CFG1_AC1_S;
	memsize = (bank0AC ? (1 << (bank0AC+1)) : 0)
	        + (bank1AC ? (1 << (bank1AC+1)) : 0);
	memsize <<= 20;
	add_memory_region(0, memsize, BOOT_MEM_RAM);

	/* Initialize it to AR5312 for now. Real detection will be done
	 * in ar5312_init_devices() */
	mips_machtype = MACH_ATHEROS_AR5312;
}

void __init ar5312_plat_setup(void)
{
	/* Clear any lingering AHB errors */
	sysRegRead(AR531X_PROCADDR);
	sysRegRead(AR531X_DMAADDR);
	sysRegWrite(AR531X_WD_CTRL, AR531X_WD_CTRL_IGNORE_EXPIRATION);

	board_time_init = ar5312_time_init;

	_machine_restart = ar5312_restart;
	_machine_halt = ar5312_halt;
	pm_power_off = ar5312_power_off;

	serial_setup(KSEG1ADDR(AR531X_UART0), ar5312_sys_frequency());
}

arch_initcall(ar5312_init_devices);
