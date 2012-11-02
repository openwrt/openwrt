/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011 Andrej Vlašić
 *  Copyright (C) 2011 Luka Perkov
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/ath5k_platform.h>
#include <linux/ath9k_platform.h>
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/io.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/module.h>

#include <irq.h>
#include <lantiq_soc.h>
#include <lantiq_platform.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>

#include "../machtypes.h"
//#include "dev-wifi-ath9k.h"
#include "devices.h"
#include "dev-dwc_otg.h"

#undef USE_BTHH_GPIO_INIT

// this reads certain data from u-boot if it's there
#define USE_UBOOT_ENV_DATA

#define UBOOT_ENV_OFFSET	0x040000
#define UBOOT_ENV_SIZE		0x010000

#ifdef NAND_ORGLAYOUT
// this is only here for reference
// definition of NAND flash area
static struct mtd_partition bthomehubv2b_nand_partitions[] =
{
	{
		.name	= "ART",
		.offset	= 0x0000000,
		.size	= 0x0004000,
	},
	{
		.name	= "image1",
		.offset	= 0x0004000,
		.size	= 0x0E00000,
	},
	{
		.name	= "unknown1",
		.offset	= 0x0E04000,
		.size	= 0x00FC000,
	},
	{
		.name	= "image2",
		.offset	= 0x0F00000,
		.size	= 0x0E00000,
	},
	{
		.name	= "unknown2",
		.offset	= 0x1D00000,
		.size	= 0x0300000,
	},

};
#endif

#ifdef NAND_KEEPOPENRG
// this allows both firmwares to co-exist

static struct mtd_partition bthomehubv2b_nand_partitions[] =
{
	{
		.name	= "art",
		.offset	= 0x0000000,
		.size	= 0x0004000,
	},
	{
		.name	= "Image1",
		.offset	= 0x0004000,
		.size	= 0x0E00000,
	},
	{
		.name	= "linux",
		.offset	= 0x0E04000,
		.size	= 0x11fC000,
	},
	{
		.name	= "wholeflash",
		.offset	= 0x0000000,
		.size	= 0x2000000,
	},

};
#endif

// this gives more jffs2 by overwriting openrg

static struct mtd_partition bthomehubv2b_nand_partitions[] =
{
	{
		.name	= "art",
		.offset	= 0x0000000,
		.size	= 0x0004000,
	},
	{
		.name	= "linux",
		.offset	= 0x0004000,
		.size	= 0x1ffC000,
	},
	{
		.name	= "wholeflash",
		.offset	= 0x0000000,
		.size	= 0x2000000,
	},

};

extern void __init xway_register_nand(struct mtd_partition *parts, int count);

// end BTHH_USE_NAND

static struct gpio_led
bthomehubv2b_gpio_leds[] __initdata = {

	{ .name = "soc:orange:upgrading", 	.gpio = 213, },
	{ .name = "soc:orange:phone", 		.gpio = 214, },
	{ .name = "soc:blue:phone", 		.gpio = 215, },
	{ .name = "soc:orange:wireless", 	.gpio = 216, },
	{ .name = "soc:blue:wireless", 		.gpio = 217, },
	{ .name = "soc:red:broadband", 		.gpio = 218, },
	{ .name = "soc:orange:broadband", 	.gpio = 219, },
	{ .name = "soc:blue:broadband", 	.gpio = 220, },
	{ .name = "soc:red:power", 		.gpio = 221, },
	{ .name = "soc:orange:power", 		.gpio = 222, },
	{ .name = "soc:blue:power", 		.gpio = 223, },
};

static struct gpio_keys_button
bthomehubv2b_gpio_keys[] __initdata = {
	{
		.desc		= "restart",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 2,
		.active_low	= 1,
	},
	{
		.desc		= "findhandset",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 15,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 22,
		.active_low	= 1,
	},
};

// definition of NOR flash area - as per original.
static struct mtd_partition bthomehubv2b_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x000000,
		.size	= 0x040000,
	},
	{
		.name	= "uboot_env",
		.offset	= UBOOT_ENV_OFFSET,
		.size	= UBOOT_ENV_SIZE,
	},
	{
		.name	= "rg_conf_1",
		.offset	= 0x050000,
		.size	= 0x010000,
	},
	{
		.name	= "rg_conf_2",
		.offset	= 0x060000,
		.size	= 0x010000,
	},
	{
		.name	= "rg_conf_factory",
		.offset	= 0x070000,
		.size	= 0x010000,
	},
};


/* nor flash */
/* bt homehubv2b has a very small nor flash */
/* so make it look for a small one, else we get a lot of alias chips identified - */
/* not a bug problem, but fills the logs. */
static struct resource bthhv2b_nor_resource =
	MEM_RES("nor", LTQ_FLASH_START, 0x80000);

static struct platform_device ltq_nor = {
	.name		= "ltq_nor",
	.resource	= &bthhv2b_nor_resource,
	.num_resources	= 1,
};

static void __init bthhv2b_register_nor(struct physmap_flash_data *data)
{
	ltq_nor.dev.platform_data = data;
	platform_device_register(&ltq_nor);
}

static struct physmap_flash_data bthomehubv2b_flash_data = {
	.nr_parts	= ARRAY_SIZE(bthomehubv2b_partitions),
	.parts		= bthomehubv2b_partitions,
};




static struct ltq_pci_data ltq_pci_data = {
	.clock	= PCI_CLOCK_INT,
	.gpio	= PCI_GNT1 | PCI_REQ1,
	.irq	= { [14] = INT_NUM_IM0_IRL0 + 22, },
};




static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_MII,
};




static char __init *get_uboot_env_var(char *haystack, int haystack_len, char *needle, int needle_len) {
	int i;
	for (i = 0; i <= haystack_len - needle_len; i++) {
		if (memcmp(haystack + i, needle, needle_len) == 0) {
			return haystack + i + needle_len;
		}
	}
	return NULL;
}

/*
 * bthomehubv2b_parse_hex_* are not uniq. in arm/orion there are also duplicates:
 * dns323_parse_hex_*
 * TODO: one day write a patch for this :)
 */
static int __init bthomehubv2b_parse_hex_nibble(char n) {
	if (n >= '0' && n <= '9')
		return n - '0';

	if (n >= 'A' && n <= 'F')
		return n - 'A' + 10;

	if (n >= 'a' && n <= 'f')
		return n - 'a' + 10;

	return -1;
}

static int __init bthomehubv2b_parse_hex_byte(const char *b) {
	int hi;
	int lo;

	hi = bthomehubv2b_parse_hex_nibble(b[0]);
	lo = bthomehubv2b_parse_hex_nibble(b[1]);

	if (hi < 0 || lo < 0)
		return -1;

	return (hi << 4) | lo;
}

static int __init bthomehubv2b_register_ethernet(void) {
	u_int8_t addr[6];
	int i;
	char *mac = "01:02:03:04:05:06";
	int gotmac = 0;
	char *boardid = "BTHHV2B";
	int gotboardid = 0;

	char *uboot_env_page;
	uboot_env_page = ioremap(LTQ_FLASH_START + UBOOT_ENV_OFFSET, UBOOT_ENV_SIZE);
	if (uboot_env_page)
	{
		char *Data = NULL;
		Data = get_uboot_env_var(uboot_env_page, UBOOT_ENV_SIZE, "\0ethaddr=", 9);
		if (Data)
		{
			mac = Data;
		}

		Data = get_uboot_env_var(uboot_env_page, UBOOT_ENV_SIZE, "\0boardid=", 9);

		if (Data)
			boardid = Data;
	}
	else
	{
		printk("bthomehubv2b: Failed to get uboot_env_page");
	}

	if (!mac) {
	goto error_fail;
	}

	if (!boardid) {
	goto error_fail;
	}

	/* Sanity check the string we're looking at */
	for (i = 0; i < 5; i++) {
	if (*(mac + (i * 3) + 2) != ':') {
		goto error_fail;
		}
	}

	for (i = 0; i < 6; i++) {
		int byte;
		byte = bthomehubv2b_parse_hex_byte(mac + (i * 3));
		if (byte < 0) {
			goto error_fail;
		}
		addr[i] = byte;
	}

	if (gotmac)
		printk("bthomehubv2b: Found ethernet MAC address: ");
	else
		printk("bthomehubv2b: using default MAC (pls set ethaddr in u-boot): ");

	for (i = 0; i < 6; i++)
		printk("%.2x%s", addr[i], (i < 5) ? ":" : ".\n");

	memcpy(&ltq_eth_data.mac.sa_data, addr, 6);
	ltq_register_etop(&ltq_eth_data);

	//memcpy(&bthomehubv2b_ath5k_eeprom_mac, addr, 6);
	//bthomehubv2b_ath5k_eeprom_mac[5]++;

	if (gotboardid)
		printk("bthomehubv2b: Board id is %s.", boardid);
	else
		printk("bthomehubv2b: Default Board id is %s.", boardid);

	if (strncmp(boardid, "BTHHV2B", 7) == 0) {
		// read in dev-wifi-ath9k
		//memcpy(&bthomehubv2b_ath5k_eeprom_data, sx763_eeprom_data, ATH5K_PLAT_EEP_MAX_WORDS);
	}
	else {
		printk("bthomehubv2b: Board id is unknown, fix uboot_env data.");
	}


	// should not unmap while we are using the ram?
	if (uboot_env_page)
		iounmap(uboot_env_page);

	return 0;

error_fail:
	if (uboot_env_page)
		iounmap(uboot_env_page);
	return -EINVAL;
}


#define PORTA2_HW_PASS1 0
#define PORTA2_HW_PASS1_SC14480 1
#define PORTA2_HW_PASS2 2

int porta2_hw_revision = -1;
EXPORT_SYMBOL(porta2_hw_revision);

#define LTQ_GPIO_OUT		0x00
#define LTQ_GPIO_IN		0x04
#define LTQ_GPIO_DIR		0x08
#define LTQ_GPIO_ALTSEL0	0x0C
#define LTQ_GPIO_ALTSEL1	0x10
#define LTQ_GPIO_OD		0x14
#define LTQ_GPIO_PUDSEL		0x1C
#define LTQ_GPIO_PUDEN		0x20

#ifdef USE_BTHH_GPIO_INIT
static void bthomehubv2b_board_prom_init(void)
{
	int revision = 0;
	unsigned int gpio = 0;
	void __iomem *mem = ioremap(LTQ_GPIO0_BASE_ADDR, LTQ_GPIO_SIZE*2);

#define DANUBE_GPIO_P0_OUT (unsigned short *)(mem + LTQ_GPIO_OUT)
#define DANUBE_GPIO_P0_IN (unsigned short *)(mem + LTQ_GPIO_IN)
#define DANUBE_GPIO_P0_DIR (unsigned short *)(mem + LTQ_GPIO_DIR)
#define DANUBE_GPIO_P0_ALTSEL0 (unsigned short *)(mem + LTQ_GPIO_ALTSEL0)
#define DANUBE_GPIO_P0_ALTSEL1 (unsigned short *)(mem + LTQ_GPIO_ALTSEL1)

#define DANUBE_GPIO_P1_OUT (unsigned short *)(mem + LTQ_GPIO_SIZE + LTQ_GPIO_OUT)
#define DANUBE_GPIO_P1_IN (unsigned short *)(mem + LTQ_GPIO_SIZE + LTQ_GPIO_IN)
#define DANUBE_GPIO_P1_DIR (unsigned short *)(mem + LTQ_GPIO_SIZE + LTQ_GPIO_DIR)
#define DANUBE_GPIO_P1_ALTSEL0 (unsigned short *)(mem + LTQ_GPIO_SIZE + LTQ_GPIO_ALTSEL0)
#define DANUBE_GPIO_P1_ALTSEL1 (unsigned short *)(mem + LTQ_GPIO_SIZE + LTQ_GPIO_ALTSEL1)
#define DANUBE_GPIO_P1_OD (unsigned short *)(mem + LTQ_GPIO_SIZE + LTQ_GPIO_OD)

	printk("About to sense board using GPIOs at %8.8X\n", (unsigned int)mem);


	/* First detect HW revision of the board. For that we need to set the GPIO
	 * lines according to table 7.2.1/7.2.2 in HSI */
	*DANUBE_GPIO_P0_OUT = 0x0200;
	*DANUBE_GPIO_P0_DIR = 0x2610;
	*DANUBE_GPIO_P0_ALTSEL0 = 0x7812;
	*DANUBE_GPIO_P0_ALTSEL1 = 0x0000;

	*DANUBE_GPIO_P1_OUT = 0x7008;
	*DANUBE_GPIO_P1_DIR = 0xF3AE;
	*DANUBE_GPIO_P1_ALTSEL0 = 0x83A7;
	*DANUBE_GPIO_P1_ALTSEL1 = 0x0400;

	gpio = (*DANUBE_GPIO_P0_IN & 0xFFFF) | 
		((*DANUBE_GPIO_P1_IN & 0xFFFF) << 16);

	revision |= (gpio & (1 << 27)) ? (1 << 0) : 0;
	revision |= (gpio & (1 << 20)) ? (1 << 1) : 0;
	revision |= (gpio & (1 << 8)) ? (1 << 2) : 0;
	revision |= (gpio & (1 << 6)) ? (1 << 3) : 0;
	revision |= (gpio & (1 << 5)) ? (1 << 4) : 0;
	revision |= (gpio & (1 << 0)) ? (1 << 5) : 0;

	porta2_hw_revision = revision;
	printk("PORTA2: detected HW revision %d\n", revision);

	/* Set GPIO lines according to HW revision. */
	/* !!! Note that we are setting SPI_CS5 (GPIO 9) to be GPIO out with value
	 * of HIGH since the FXO does not use the SPI CS mechanism, it does it
	 * manually by controlling the GPIO line. We need the CS line to be disabled
	 * (HIGH) until needed since it will intefere with other devices on the SPI
	 * bus. */
	*DANUBE_GPIO_P0_OUT = 0x0200;
	/*
	 * During the manufacturing process a different machine takes over uart0
	 * so set it as input (so it wouldn't drive the line)
	 */
#define cCONFIG_SHC_BT_MFG_TEST 0
	*DANUBE_GPIO_P0_DIR = 0x2671 | (cCONFIG_SHC_BT_MFG_TEST ? 0 : (1 << 12));

	if (revision == PORTA2_HW_PASS1_SC14480 || revision == PORTA2_HW_PASS2)
		*DANUBE_GPIO_P0_ALTSEL0 = 0x7873;
	else
		*DANUBE_GPIO_P0_ALTSEL0 = 0x3873;

	*DANUBE_GPIO_P0_ALTSEL1 = 0x0001;


	//###################################################################################    
	// Register values before patch
	// P1_ALTSEL0 = 0x83A7
	// P1_ALTSEL1 = 0x0400
	// P1_OU	T     = 0x7008
	// P1_DIR     = 0xF3AE
	// P1_OD      = 0xE3Fc
	printk("\nApplying Patch for CPU1 IRQ Issue\n");
	*DANUBE_GPIO_P1_ALTSEL0 &= ~(1<<12);  // switch P1.12 (GPIO28) to GPIO functionality
	*DANUBE_GPIO_P1_ALTSEL1 &= ~(1<<12);  // switch P1.12 (GPIO28) to GPIO functionality
	*DANUBE_GPIO_P1_OUT     &= ~(1<<12);  // set P1.12 (GPIO28) to 0
	*DANUBE_GPIO_P1_DIR     |= (1<<12);   // configure P1.12 (GPIO28) as output 
	*DANUBE_GPIO_P1_OD      |= (1<<12);   // activate Push/Pull mode 
	udelay(100);			      // wait a little bit (100us)
	*DANUBE_GPIO_P1_OD      &= ~(1<<12);  // switch back from Push/Pull to Open Drain
	// important: before! setting output to 1 (3,3V) the mode must be switched 
	// back to Open Drain because the reset pin of the SC14488 is internally 
	// pulled to 1,8V
	*DANUBE_GPIO_P1_OUT     |= (1<<12);   // set output P1.12 (GPIO28) to 1
	// Register values after patch, should be the same as before
	// P1_ALTSEL0 = 0x83A7
	// P1_ALTSEL1 = 0x0400
	// P1_OUT     = 0x7008
	// P1_DIR     = 0xF3AE
	// P1_OD      = 0xE3Fc
	//###################################################################################


	*DANUBE_GPIO_P1_OUT = 0x7008;
	*DANUBE_GPIO_P1_DIR = 0xEBAE | (revision == PORTA2_HW_PASS2 ? 0x1000 : 0);
	*DANUBE_GPIO_P1_ALTSEL0 = 0x8BA7;
	*DANUBE_GPIO_P1_ALTSEL1 = 0x0400;

	iounmap(mem);
}
#endif
static void __init bthomehubv2b_init(void) {
#define bthomehubv2b_USB		13

	// read the board version
#ifdef USE_BTHH_GPIO_INIT
	bthomehubv2b_board_prom_init();
#endif

	// register extra GPPOs used by LEDs as GPO 0x200+
	ltq_register_gpio_stp();
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(bthomehubv2b_gpio_leds), bthomehubv2b_gpio_leds);
	bthhv2b_register_nor(&bthomehubv2b_flash_data);
	xway_register_nand(bthomehubv2b_nand_partitions, ARRAY_SIZE(bthomehubv2b_nand_partitions));
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL, ARRAY_SIZE(bthomehubv2b_gpio_keys), bthomehubv2b_gpio_keys);
//	ltq_register_ath9k();
	xway_register_dwc(bthomehubv2b_USB);
	bthomehubv2b_register_ethernet();

}

MIPS_MACHINE(LANTIQ_MACH_BTHOMEHUBV2BOPENRG,
	"BTHOMEHUBV2BOPENRG",
	"BTHOMEHUBV2B - BT Homehub V2.0 Type B with OpenRG image retained",
	bthomehubv2b_init);

MIPS_MACHINE(LANTIQ_MACH_BTHOMEHUBV2B,
	"BTHOMEHUBV2B",
	"BTHOMEHUBV2B - BT Homehub V2.0 Type B",
	bthomehubv2b_init);
