/*
 *  Cisco WAP4410N board support
 *
 *  Copyright (C) 2014 Caleb James DeLisle <cjd@cjdns.fr>
 *  Copyright (C) 2015 Ryan A Young <rayoung@utexas.edu>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation, or (at your option) any later version.
 */
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/nxp_74hc153.h>

#include <asm/mach-ath79/ath79.h>

#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

/* -------------- begin flash device -------------- */

#define FLASH_BASE 0xbf000000

/* where the actual art data is within the art partition. */
#define ART_DATA_OFFSET 0x1000

/* If changed, make sure to change image/Makefile too! */
#define KERN_SIZE 0x190000

/* Flash layout: u-boot/include/upgrade.h in cisco's GPL dump */
#define FLASH_SIZE			(0x800000)
#define BOOT_SIZE			(0x40000)
#define NVRAM_SIZE			(0x10000)
#define ART_SIZE			(0x10000)
/*
 * Note that this is different from upgrade.h, in which ENV_SIZE is 0x10000.
 * This is because the sercomm header is located in the upper end of the root
 * section instead of the env section, and it must not be overwritten by rootfs.
 */
#define ENV_SIZE			(0x20000)

#define NODE_INFO_OFFSET (BOOT_SIZE - 0x90)
struct wap4410n_node_info {
	/** Serial number written on back of device. */
	char serial_no[16];

	/** Internal to Sercomm (?), France = { domain: 0x80, country: 0x00 } */
	uint8_t domain;
	uint8_t country;

	/** written on the board, eg: 13 */
	uint8_t hw_ver;

	uint8_t zero0[5];

	/** ASCII numeric digits */
	char wps_pin[8];

	uint8_t zero1[16];

	uint8_t mac_addr[6];

	uint8_t zero2[3];

	/** 31734572436f4d6d -> "1sErCoMm" does not seem to be checked. */
	char magic_1sErCoMm[8];

	/** 00010000 (offset 0x41, completely unaligned) */
	uint8_t unknown0[4];

	/** Used by upslug2 protocol */
	uint8_t hardware_id[32];

	/** 0000240800008000000000030000200400000008 */
	uint8_t unknown1[20];

	/** 734572436f4d6d -> "sErCoMm" */
	uint8_t magic_sErCoMm[7];

	uint8_t zero3[16];
};

#define UPGRADE_INFO_OFFSET	0x7dfff0
struct wap4410n_upgrade_info {
	/** Stock WAP4410: 2408 */
	uint16_t product_id;

	/** Always 8000 */
	uint16_t protocol;

	/** 2007 -> v2.0.7.x */
	uint16_t fw_version;

	/** 90f7 same value as NSLU2 */
	uint16_t unknown0;

	/** bootloader checks this and will "soft brick" if it's not correct. */
	uint8_t eRcOmM[6];

	uint8_t pad[2];
};

/*
 * An instruction in the bootloader which checks that 0x7dfff8 == "eRcOmM" and
 * bricks if it's not.
 * If this instruction is overwritten with a zero, you get 64k of extra space.
 * 2406 0006 1440 000a 8fbc 0020 <-- unpatched
 * 2406 0006 0000 0000 8fbc 0020 <-- patched
 */
#define SERCOMM_CHECK_LAST_INSN   0x24060006
#define SERCOMM_CHECK_INSN_OFFSET 0x19e08
#define SERCOMM_CHECK_NEXT_INSN   0x8fbc0020


/* Little bit of arithmatic on the flash layout. */
#define NVRAM_OFFSET BOOT_SIZE
#define KERN_OFFSET (NVRAM_OFFSET + NVRAM_SIZE)
#define ROOT_OFFSET (KERN_OFFSET + KERN_SIZE)
#define ART_OFFSET (FLASH_SIZE - ART_SIZE)
#define ENV_OFFSET (ART_OFFSET - ENV_SIZE)
/* rootfs is whatever is left. */
#define ROOT_SIZE (ENV_OFFSET - ROOT_OFFSET)

#define PART(b, s, n, f) { .name = n, .offset = b, .size = s, .mask_flags = f }
static struct mtd_partition wap4410n_flash_partitions[] = {
	PART(0x00000000,   BOOT_SIZE,  "u-boot",	 MTD_WRITEABLE),
	PART(NVRAM_OFFSET, NVRAM_SIZE, "u-boot-env", MTD_WRITEABLE),
	PART(KERN_OFFSET,  KERN_SIZE,  "kernel",	 0),
	PART(ROOT_OFFSET,  ROOT_SIZE,  "rootfs",	 0),
	PART(ENV_OFFSET,   ENV_SIZE,   "sercomm",	MTD_WRITEABLE),
	PART(ART_OFFSET,   ART_SIZE,   "art",		MTD_WRITEABLE),

	/* Pseudo-partition over whole upgradable space, used by sysupgrade. */
	PART(KERN_OFFSET, KERN_SIZE + ROOT_SIZE, "firmware", 0)
};
#undef PART

static struct physmap_flash_data wap4410n_flash_data = {
	.width		= 2,
	.parts		= wap4410n_flash_partitions,
	.nr_parts	= ARRAY_SIZE(wap4410n_flash_partitions),
};

static struct resource wap4410n_flash_resources[] = {
	[0] = {
		.start	= FLASH_BASE,
		.end	= FLASH_BASE + FLASH_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device wap4410n_flash_device = {
	.name		   = "physmap-flash",
	.id			 = -1,
	.resource		= wap4410n_flash_resources,
	.num_resources	= ARRAY_SIZE(wap4410n_flash_resources),
	.dev			= { .platform_data = &wap4410n_flash_data }
};


static void __init wap4410n_flash_reg(void)
{
	uint32_t *insn = (uint32_t *) (FLASH_BASE + SERCOMM_CHECK_INSN_OFFSET);
	int i;
	if (SERCOMM_CHECK_LAST_INSN != insn[-1] ||
		SERCOMM_CHECK_NEXT_INSN != insn[1]) {
		printk(KERN_INFO "Unrecognized bootloader, costs 64k storage");
	} else if (insn[0]) {
		printk(KERN_INFO "eRcOmM check at %p in uboot, costs 64k storage",
				(void *)insn);
	} else {
		printk(KERN_INFO "eRcOmM check at %p patched, gain 64k storage",
				(void *)insn);
		wap4410n_flash_partitions[3].size +=
			wap4410n_flash_partitions[4].size;
		wap4410n_flash_data.nr_parts--;
		for (i = 4; i < wap4410n_flash_data.nr_parts; i++) {
			memcpy(&wap4410n_flash_partitions[i],
				   &wap4410n_flash_partitions[i + 1],
				   sizeof(struct mtd_partition));
		}
	}
	platform_device_register(&wap4410n_flash_device);
}

/* -------------- end flash device -------------- */


/* -------------------- GPIO -------------------- */

#define LED_WIRELESS			  0
#define LED_POWER				 1
#define KEYS_POLL_INTERVAL		20	/* msecs */
#define KEYS_DEBOUNE_INTERVAL	(3 * KEYS_POLL_INTERVAL)

/* 2 lights are gpio, other 2 are hardwired. */
static struct gpio_led wap4410n_leds_gpio[] __initdata = {
	{
		.name		= "wrt4410n:green:power",
		.gpio		= LED_POWER,
		.active_low  = 1,
	},
	{
		.name		= "wrt4410n:green:wireless",
		.gpio		= LED_WIRELESS,
		.active_low  = 1,
	},
};

static struct gpio_keys_button wap4410n_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = KEYS_DEBOUNE_INTERVAL,
		.gpio		= 21,
		.active_low  = 1,
	}
};

static void __init wap4410n_gpio_reg(void)
{
	ath79_register_gpio_keys_polled(
		-1,
		KEYS_POLL_INTERVAL,
		ARRAY_SIZE(wap4410n_gpio_keys),
		wap4410n_gpio_keys
	);
	ath79_register_leds_gpio(
		-1,
		 ARRAY_SIZE(wap4410n_leds_gpio),
		 wap4410n_leds_gpio
	);
}

/* -------------------- end GPIO -------------------- */

/** Never called, just for build time verification. */
static void wap4410n_build_verify(void)
{
	BUILD_BUG_ON((KERN_SIZE / 0x10000 * 0x10000) != KERN_SIZE);
	BUILD_BUG_ON(sizeof(struct wap4410n_upgrade_info) != 16);
	BUILD_BUG_ON(sizeof(struct wap4410n_node_info) != 0x90);
}

static void __init wap4410n_setup(void)
{
	struct wap4410n_node_info *ni = (struct wap4410n_node_info *)
		(FLASH_BASE + NODE_INFO_OFFSET);
	uint8_t *art = (uint8_t *)
		(FLASH_BASE + FLASH_SIZE - ART_SIZE + ART_DATA_OFFSET);

	ath79_init_mac(ath79_eth0_data.mac_addr, ni->mac_addr, 0);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	/* TODO: SPEED_1000 causes a silent failure, testing needed. */
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_register_eth(0);

	ath79_register_usb();

	wap4410n_flash_reg();

	ath79_register_wmac(art, ni->mac_addr);

	wap4410n_gpio_reg();

	/* silence compiler warning */
	if (0)
		wap4410n_build_verify();
}

MIPS_MACHINE(
	ATH79_MACH_WAP4410N,
	"WAP4410N",
	"Linksys WAP4410N",
	wap4410n_setup
);
