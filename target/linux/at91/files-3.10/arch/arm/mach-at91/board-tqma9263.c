/*
 *  Copyright (C) 2008 I2SE GmbH
 *  Copyright (C) 2010 IEQualize GmbH
 *  Copyright (C) 2010-2011 Michael Heimpold
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fb.h>

#include <mach/hardware.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/at91sam9_smc.h>

#include <video/atmel_lcdc.h>

#include "sam9_smc.h"
#include "generic.h"

#include <linux/atmel_serial.h>


static void __init tqma9263_map_io(void)
{
	unsigned pins;

	/* Initialize processor: 18.432 MHz crystal */
	at91_initialize(18432000);

	/* DGBU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* USART0 on ttyS1 */
	pins = ATMEL_UART_CTS | ATMEL_UART_RTS |
	       ATMEL_UART_DSR | ATMEL_UART_DTR |
	       ATMEL_UART_DCD | ATMEL_UART_RI;
	at91_register_uart(AT91SAM9263_ID_US0, 1, pins);

	/* USART1 on ttyS2 */
	pins = ATMEL_UART_RTS;
	at91_register_uart(AT91SAM9263_ID_US1, 2, pins);

	/* USART2 on ttyS3 */
	pins = ATMEL_UART_RTS;
	at91_register_uart(AT91SAM9263_ID_US2, 3, pins);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}


/*
 * USB Host port
 */
static struct at91_usbh_data __initdata tqma9263_usbh_data = {
	.ports			= 2,
	.vbus_pin		= { AT91_PIN_PA24, AT91_PIN_PA21 },
	.vbus_pin_active_low	= 0,
};


/*
 * USB Device port
 */
static struct at91_udc_data __initdata tqma9263_udc_data = {
	.vbus_pin	= AT91_PIN_PA25,
	.pullup_pin	= 0,			/* pull-up driven by UDC */
};


/*
 * MCI (SD/MMC)
 */
static struct at91_mmc_data __initdata tqma9263_mmc_data = {
	.wire4		= 1,
	.det_pin	= AT91_PIN_PE18,
	.wp_pin		= AT91_PIN_PB28,
};


/*
 * MACB Ethernet device
 */
static struct macb_platform_data __initdata tqma9263_macb_data = {
	.phy_irq_pin	= AT91_PIN_PE31,
	.is_rmii	= 1,
};


/*
 * I2C devices
 */
static struct i2c_board_info __initdata tqma9263_i2c_devices[] = {
	{
		I2C_BOARD_INFO("pcf8563", 0x51),
	},
	{
		I2C_BOARD_INFO("24c04", 0x52),
	},
};


/*
 * NAND flash
 */
static struct mtd_partition __initdata tqma9263_nand_partition[] = {
	{
		.name	= "uboot",
		.offset	= 0,
		.size	= 3 * SZ_128K,
	},
	{
		.name	= "uboot-env",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= SZ_128K,
	},
	{
		.name	= "linux",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= SZ_2M,
	},
	{
		.name	= "rootfs",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
};

static struct mtd_partition * __init nand_partitions(int size, int *num_partitions)
{
	*num_partitions = ARRAY_SIZE(tqma9263_nand_partition);
	return tqma9263_nand_partition;
}

static struct atmel_nand_data __initdata tqma9263_nand_data = {
	.ale			= 21,
	.cle			= 22,
	.rdy_pin		= AT91_PIN_PD14,
	.enable_pin		= AT91_PIN_PD15,
};

static struct sam9_smc_config __initdata tqma9263_nand_smc_config = {
	.ncs_read_setup		= 0,
	.nrd_setup		= 1,
	.ncs_write_setup	= 0,
	.nwe_setup		= 1,

	.ncs_read_pulse		= 3,
	.nrd_pulse		= 3,
	.ncs_write_pulse	= 3,
	.nwe_pulse		= 3,

	.read_cycle		= 5,
	.write_cycle		= 5,

	.mode			= AT91_SMC_READMODE | AT91_SMC_WRITEMODE | AT91_SMC_EXNWMODE_DISABLE | AT91_SMC_DBW_8,
	.tdf_cycles		= 2,
};

static void __init tqma9263_add_device_nand(void)
{
	/* configure chip-select 3 (NAND) */
	sam9_smc_configure(0, 3, &tqma9263_nand_smc_config);

	at91_add_device_nand(&tqma9263_nand_data);
}


static void __init tqma9263_board_init(void)
{
	/* Serial */
	at91_add_device_serial();
	/* USB Host */
	at91_add_device_usbh(&tqma9263_usbh_data);
	/* USB Device */
	at91_add_device_udc(&tqma9263_udc_data);
	/* MMC */
	at91_add_device_mmc(1, &tqma9263_mmc_data);
	/* I2C */
	at91_add_device_i2c(tqma9263_i2c_devices, ARRAY_SIZE(tqma9263_i2c_devices));
	/* Ethernet */
	at91_add_device_eth(&tqma9263_macb_data);
	/* NAND flash */
	tqma9263_add_device_nand();
}

MACHINE_START(TQMA9263, "TQ Components TQMa9263")
	/* Maintainer: Michael Heimpold */
	.timer		= &at91sam926x_timer,
	.map_io		= tqma9263_map_io,
	.init_irq	= at91_init_irq_default,
	.init_machine	= tqma9263_board_init,
MACHINE_END
