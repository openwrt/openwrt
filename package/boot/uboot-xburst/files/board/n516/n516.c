/*
 * (C) Copyright 2006
 * Ingenic Semiconductor, <jlwei@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/jz4740.h>
#include <asm/addrspace.h>
#include <asm/cacheops.h>

void _machine_restart(void)
{
	__wdt_select_extalclk();
	__wdt_select_clk_div64();
	__wdt_set_data(100);
	__wdt_set_count(0);
	__tcu_start_wdt_clock();
	__wdt_start();
	while(1);

}

static void gpio_init(void)
{

	REG_GPIO_PXPES(0) = 0xffffffff;
	REG_GPIO_PXPES(1) = 0xffffffff;
	REG_GPIO_PXPES(2) = 0xffffffff;
	REG_GPIO_PXPES(3) = 0xffffffff;

	/*
	 * Initialize NAND Flash Pins
	 */
	__gpio_as_nand();

	/*
	 * Initialize SDRAM pins
	 */
	__gpio_as_sdram_32bit();

	/*
	 * Initialize UART0 pins
	 */
	__gpio_as_uart0();

	/*
	 * Initialize MSC pins
	 */
	__gpio_as_msc();

	/*
	 * Initialize LCD pins
	 */
	__gpio_as_lcd_16bit();

	/*
	 * Initialize Other pins
	 */
	__gpio_as_output(GPIO_SD_VCC_EN_N);
	__gpio_clear_pin(GPIO_SD_VCC_EN_N);

	__gpio_as_input(GPIO_SD_CD_N);
	__gpio_disable_pull(GPIO_SD_CD_N);

	__gpio_as_output(GPIO_DISP_OFF_N);

	__gpio_as_output(GPIO_LED_EN);
	__gpio_set_pin(GPIO_LED_EN);

	__gpio_as_input(127);
}

static void cpm_init(void)
{
	__cpm_stop_ipu();
	__cpm_stop_cim();
	__cpm_stop_i2c();
	__cpm_stop_ssi();
	__cpm_stop_uart1();
	__cpm_stop_sadc();
	__cpm_stop_uhc();
	__cpm_stop_udc();
	__cpm_stop_aic1();
	__cpm_stop_aic2();
	__cpm_suspend_udcphy();
	__cpm_suspend_usbphy();
}

//----------------------------------------------------------------------
// board early init routine

void board_early_init(void)
{
	gpio_init();
	cpm_init();
}

//----------------------------------------------------------------------
// U-Boot common routines

int checkboard (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Hanvon n516 e-book (CPU Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
