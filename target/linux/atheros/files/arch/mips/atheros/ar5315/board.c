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
#include <asm/gpio.h>

static int is_5315 = 0;

static struct resource ar5315_eth_res[] = {
	{
		.name = "eth0_membase",
		.flags = IORESOURCE_MEM,
		.start = AR5315_ENET0,
		.end = AR5315_ENET0 + 0x2000,
	},
	{
		.name = "eth0_irq",
		.flags = IORESOURCE_IRQ,
		.start = AR5315_IRQ_ENET0_INTRS,
		.end = AR5315_IRQ_ENET0_INTRS,
	},
};

static struct ar531x_eth ar5315_eth_data = {
	.phy = 1,
	.mac = 0,
	.reset_base = AR5315_RESET,
	.reset_mac = AR5315_RESET_ENET0,
	.reset_phy = AR5315_RESET_EPHY0,
	.phy_base = AR5315_ENET0
};

static struct platform_device ar5315_eth = {
	.id = 0,
	.name = "ar531x-eth",
	.dev.platform_data = &ar5315_eth_data,
	.resource = ar5315_eth_res,
	.num_resources = ARRAY_SIZE(ar5315_eth_res)
};

static struct platform_device ar5315_wmac = {
	.id = 0,
	.name = "ar531x-wmac",
	/* FIXME: add resources */
};

static struct resource ar5315_spiflash_res[] = {
	{
		.name = "flash_base",
		.flags = IORESOURCE_MEM,
		.start = KSEG1ADDR(AR5315_SPI_READ),
		.end = KSEG1ADDR(AR5315_SPI_READ) + 0x800000,
	},
	{
		.name = "flash_regs",
		.flags = IORESOURCE_MEM,
		.start = 0x11300000,
		.end = 0x11300012,
	},
};

static struct platform_device ar5315_spiflash = {
	.id = 0,
	.name = "spiflash",
	.resource = ar5315_spiflash_res,
	.num_resources = ARRAY_SIZE(ar5315_spiflash_res)
};

#ifdef CONFIG_LEDS_GPIO
static struct gpio_led ar5315_leds[8];

static struct gpio_led_platform_data ar5315_led_data = {
	.num_leds = ARRAY_SIZE(ar5315_leds),
	.leds = (void *) ar5315_leds,
};

static struct platform_device ar5315_gpio_leds = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = (void *) &ar5315_led_data,
	}
};
#endif

static struct platform_device ar5315_wdt =
{
	.id = 0,
	.name = "ar2315_wdt",
};

static __initdata struct platform_device *ar5315_devs[6];

static void *flash_regs;

static inline __u32 spiflash_regread32(int reg)
{
	volatile __u32 *data = (__u32 *)(flash_regs + reg);

	return (*data);
}

static inline void spiflash_regwrite32(int reg, __u32 data)
{
	volatile __u32 *addr = (__u32 *)(flash_regs + reg);

	*addr = data;
}

#define SPI_FLASH_CTL      0x00
#define SPI_FLASH_OPCODE   0x04
#define SPI_FLASH_DATA     0x08

static __u8 spiflash_probe(void)
{
	 __u32 reg;

	do {
		reg = spiflash_regread32(SPI_FLASH_CTL);
	} while (reg & SPI_CTL_BUSY);

	spiflash_regwrite32(SPI_FLASH_OPCODE, 0xab);

	reg = (reg & ~SPI_CTL_TX_RX_CNT_MASK) | 4 |
		(1 << 4) | SPI_CTL_START;

	spiflash_regwrite32(SPI_FLASH_CTL, reg);

	do {
		reg = spiflash_regread32(SPI_FLASH_CTL);
	} while (reg & SPI_CTL_BUSY);

	reg = (__u32) spiflash_regread32(SPI_FLASH_DATA);
	reg &= 0xff;

	return (u8) reg;
}


#define STM_8MBIT_SIGNATURE     0x13
#define STM_16MBIT_SIGNATURE    0x14
#define STM_32MBIT_SIGNATURE    0x15
#define STM_64MBIT_SIGNATURE    0x16
#define STM_128MBIT_SIGNATURE   0x17


static char __init *ar5315_flash_limit(void)
{
	u8 sig;
	u32 flash_size = 0;

	/* probe the flash chip size */
	flash_regs = ioremap_nocache(ar5315_spiflash_res[1].start, ar5315_spiflash_res[1].end - ar5315_spiflash_res[1].start);
	sig = spiflash_probe();
	iounmap(flash_regs);

	switch(sig) {
		case STM_8MBIT_SIGNATURE:
			flash_size = 0x00100000;
			break;
		case STM_16MBIT_SIGNATURE:
			flash_size = 0x00200000;
			break;
		case STM_32MBIT_SIGNATURE:
			flash_size = 0x00400000;
			break;
		case STM_64MBIT_SIGNATURE:
			flash_size = 0x00800000;
			break;
		case STM_128MBIT_SIGNATURE:
			flash_size = 0x01000000;
			break;
	}

	ar5315_spiflash_res[0].end = ar5315_spiflash_res[0].start + flash_size;
	return (char *) ar5315_spiflash_res[0].end;
}

int __init ar5315_init_devices(void)
{
	struct ar531x_config *config;
	struct ar531x_boarddata *bcfg;
	int dev = 0;
#ifdef CONFIG_LEDS_GPIO
	int i;
	char *tmp;
#endif

	if (!is_5315)
		return 0;

	/* Find board configuration */
	ar531x_find_config(ar5315_flash_limit());
	bcfg = (struct ar531x_boarddata *) board_config;

	config = (struct ar531x_config *) kzalloc(sizeof(struct ar531x_config), GFP_KERNEL);
	config->board = board_config;
	config->radio = radio_config;
	config->unit = 0;
	config->tag = (u_int16_t) (sysRegRead(AR5315_SREV) & AR5315_REV_CHIP);

	ar5315_eth_data.board_config = board_config;
	ar5315_eth_data.macaddr = bcfg->enet0Mac;
	ar5315_wmac.dev.platform_data = config;

	ar5315_devs[dev++] = &ar5315_eth;
	ar5315_devs[dev++] = &ar5315_wmac;
	ar5315_devs[dev++] = &ar5315_spiflash;
	ar5315_devs[dev++] = &ar5315_wdt;

#ifdef CONFIG_LEDS_GPIO
	ar5315_led_data.num_leds = 0;
	for(i = 1; i < 8; i++)
	{
		if((i != AR5315_RESET_GPIO) && (i != bcfg->resetConfigGpio))
		{
			if(i == bcfg->sysLedGpio)
			{
				tmp = kstrdup("wlan", GFP_KERNEL);
			} else {
				tmp = kmalloc(6, GFP_KERNEL);
				if(tmp)
					sprintf((char*)tmp, "gpio%d", i);
			}
			if(tmp)
			{
				ar5315_leds[ar5315_led_data.num_leds].name = tmp;
				ar5315_leds[ar5315_led_data.num_leds].gpio = i;
				ar5315_leds[ar5315_led_data.num_leds].active_low = 0;
				ar5315_led_data.num_leds++;
			} else {
				printk("failed to alloc led string\n");
				continue;
			}
		}
	}
	ar5315_devs[dev++] = &ar5315_gpio_leds;
#endif

	return platform_add_devices(ar5315_devs, dev);
}

static void ar5315_halt(void)
{
	 while (1);
}

static void ar5315_power_off(void)
{
	 ar5315_halt();
}


static void ar5315_restart(char *command)
{
	for(;;) {
		/* reset the system */
		sysRegWrite(AR5315_COLD_RESET,AR5317_RESET_SYSTEM);

		/*
		 * Cold reset does not work on the AR2315/6, use the GPIO reset bits a workaround.
		 */
		gpio_direction_output(AR5315_RESET_GPIO, 0);
	}
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

static int __initdata PLLC_DIVIDE_TABLE[5] = {
    2,
    3,
    4,
    6,
    3
};

static unsigned int __init
ar5315_sys_clk(unsigned int clockCtl)
{
    unsigned int pllcCtrl,cpuDiv;
    unsigned int pllcOut,refdiv,fdiv,divby2;
	unsigned int clkDiv;

    pllcCtrl = sysRegRead(AR5315_PLLC_CTL);
    refdiv = (pllcCtrl & PLLC_REF_DIV_M) >> PLLC_REF_DIV_S;
    refdiv = CLOCKCTL1_PREDIVIDE_TABLE[refdiv];
    fdiv = (pllcCtrl & PLLC_FDBACK_DIV_M) >> PLLC_FDBACK_DIV_S;
    divby2 = (pllcCtrl & PLLC_ADD_FDBACK_DIV_M) >> PLLC_ADD_FDBACK_DIV_S;
    divby2 += 1;
    pllcOut = (40000000/refdiv)*(2*divby2)*fdiv;


    /* clkm input selected */
	switch(clockCtl & CPUCLK_CLK_SEL_M) {
		case 0:
		case 1:
			clkDiv = PLLC_DIVIDE_TABLE[(pllcCtrl & PLLC_CLKM_DIV_M) >> PLLC_CLKM_DIV_S];
			break;
		case 2:
			clkDiv = PLLC_DIVIDE_TABLE[(pllcCtrl & PLLC_CLKC_DIV_M) >> PLLC_CLKC_DIV_S];
			break;
		default:
			pllcOut = 40000000;
			clkDiv = 1;
			break;
	}
	cpuDiv = (clockCtl & CPUCLK_CLK_DIV_M) >> CPUCLK_CLK_DIV_S;
	cpuDiv = cpuDiv * 2 ?: 1;
	return (pllcOut/(clkDiv * cpuDiv));
}

static inline unsigned int ar5315_cpu_frequency(void)
{
    return ar5315_sys_clk(sysRegRead(AR5315_CPUCLK));
}

static inline unsigned int ar5315_apb_frequency(void)
{
    return ar5315_sys_clk(sysRegRead(AR5315_AMBACLK));
}

static void __init ar5315_time_init(void)
{
	mips_hpt_frequency = ar5315_cpu_frequency() / 2;
}

void __init ar5315_prom_init(void)
{
	u32 memsize, memcfg, devid;

	is_5315 = 1;
	memcfg = sysRegRead(AR5315_MEM_CFG);
	memsize   = 1 + ((memcfg & SDRAM_DATA_WIDTH_M) >> SDRAM_DATA_WIDTH_S);
	memsize <<= 1 + ((memcfg & SDRAM_COL_WIDTH_M) >> SDRAM_COL_WIDTH_S);
	memsize <<= 1 + ((memcfg & SDRAM_ROW_WIDTH_M) >> SDRAM_ROW_WIDTH_S);
	memsize <<= 3;
	add_memory_region(0, memsize, BOOT_MEM_RAM);

	/* Detect the hardware based on the device ID */
	devid = sysRegRead(AR5315_SREV) & AR5315_REV_CHIP;
	switch(devid) {
		case 0x90:
		case 0x91:
			mips_machtype = MACH_ATHEROS_AR2317;
			break;
		default:
			mips_machtype = MACH_ATHEROS_AR2315;
			break;
	}
}

void __init ar5315_plat_setup(void)
{
	unsigned int config = read_c0_config();

	/* Clear any lingering AHB errors */
	write_c0_config(config & ~0x3);
	sysRegWrite(AR5315_AHB_ERR0,AHB_ERROR_DET);
	sysRegRead(AR5315_AHB_ERR1);
	sysRegWrite(AR5315_WDC, WDC_IGNORE_EXPIRATION);

	board_time_init = ar5315_time_init;

	_machine_restart = ar5315_restart;
	_machine_halt = ar5315_halt;
	pm_power_off = ar5315_power_off;

	serial_setup(KSEG1ADDR(AR5315_UART0), ar5315_apb_frequency());
}

arch_initcall(ar5315_init_devices);
