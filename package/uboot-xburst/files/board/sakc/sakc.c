/*
 * Authors: Xiangfu Liu <xiangfu.z@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 3 of the License, or (at your option) any later version.
 */

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/jz4740.h>

DECLARE_GLOBAL_DATA_PTR;

static void gpio_init(void)
{
	/*
	 * Initialize NAND Flash Pins
	 */
	__gpio_as_nand();

	/*
	 * Initialize SDRAM pins
	 */	
	__gpio_as_sdram_16bit_4725();

	/*
	 * Initialize UART0 pins
	 */
	__gpio_as_uart0();

	/*
	 * Initialize LCD pins
	 */
	__gpio_as_lcd_18bit();

	/*
	 * Initialize MSC pins
	 */
	__gpio_as_msc();

	/*
	 * Initialize SSI pins
	 */
	__gpio_as_ssi();

	/*
	 * Initialize I2C pins
	 */
	__gpio_as_i2c();

	/*
	 * Initialize MSC pins
	 */
	__gpio_as_msc();

	/*
	 * Initialize Other pins
	 */
	__gpio_as_input(GPIO_SD_DETECT);
	__gpio_disable_pull(GPIO_SD_DETECT);
}
/* TODO SAKC
static void cpm_init(void)
{
	__cpm_stop_ipu();
	__cpm_stop_cim();
	__cpm_stop_i2c();
	__cpm_stop_ssi();
	__cpm_stop_uart1();
	__cpm_stop_sadc();
	__cpm_stop_uhc();
	__cpm_stop_aic1();
	__cpm_stop_aic2();
}*/

void board_early_init(void)
{
	gpio_init();
	//cpm_init(); //TODO SAKC
}

/* U-Boot common routines */

int checkboard (void)
{

	printf("Board: SAKC (Ingenic XBurst Jz4725 SoC, Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
