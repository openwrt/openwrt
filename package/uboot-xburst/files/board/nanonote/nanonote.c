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
	__gpio_as_sdram_32bit();

	/*
	 * Initialize LCD pins
	 */
	__gpio_as_lcd_8bit();

	/*
	 * Initialize MSC pins
	 */
	__gpio_as_msc();

	/*
	 * Initialize Other pins
	 */
	unsigned int i;
	for (i = 0; i < 7; i++){
		__gpio_as_input(GPIO_KEYIN_BASE + i);
		__gpio_enable_pull(GPIO_KEYIN_BASE + i);
	}

	for (i = 0; i < 8; i++) {
		__gpio_as_output(GPIO_KEYOUT_BASE + i);
		__gpio_clear_pin(GPIO_KEYOUT_BASE + i);
	}

	/*
	 * Initialize UART0 pins, in Ben NanoNote uart0 and keyin8 use the
	 * same gpio, init the gpio as uart0 cause a keyboard bug. so for
	 * end user we disable the uart0
	 */
	if (__gpio_get_pin(GPIO_KEYIN_BASE + 2) == 0){
		/* if pressed [S] */
		printf("[S] pressed, enable UART0\n");
		gd->boot_option = 5;
		__gpio_as_uart0();
	} else {
		printf("[S] not pressed, disable UART0\n");
		__gpio_as_input(GPIO_KEYIN_8);
		__gpio_enable_pull(GPIO_KEYIN_8);
	}

	__gpio_as_output(GPIO_AUDIO_POP);
	__gpio_set_pin(GPIO_AUDIO_POP);

	__gpio_as_output(GPIO_LCD_CS);
	__gpio_clear_pin(GPIO_LCD_CS);

	__gpio_as_output(GPIO_AMP_EN);
	__gpio_clear_pin(GPIO_AMP_EN);

	__gpio_as_output(GPIO_SDPW_EN);
	__gpio_disable_pull(GPIO_SDPW_EN);
	__gpio_clear_pin(GPIO_SDPW_EN);

	__gpio_as_input(GPIO_SD_DETECT);
	__gpio_disable_pull(GPIO_SD_DETECT);

	__gpio_as_input(GPIO_USB_DETECT);
	__gpio_enable_pull(GPIO_USB_DETECT);

	if (__gpio_get_pin(GPIO_KEYIN_BASE + 3) == 0) {
		printf("[M] pressed, boot from sd card\n");
		gd->boot_option = 1;
	}
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
/*	__cpm_stop_aic2();*/
}

void board_early_init(void)
{
	gpio_init();
	cpm_init();
}

/* U-Boot common routines */

int checkboard (void)
{

	printf("Board: Qi LB60 (Ingenic XBurst Jz4740 SoC, Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
