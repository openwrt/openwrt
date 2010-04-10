#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/physmap.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <mach/common.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>
#include <mach/imx-uart.h>
#include <mach/imxfb.h>
#include <mach/iomux-mx21.h>
#include <mach/board-vp6500.h>

#include "devices.h"

#include <linux/input.h>
#include <linux/input/matrix_keypad.h>

static unsigned int vp6500_pins[] = {

	/* UART1 */
	PE12_PF_UART1_TXD,
	PE13_PF_UART1_RXD,

#if 0
	/* LCDC */
	PA5_PF_LSCLK,
	PA6_PF_LD0,
	PA7_PF_LD1,
	PA8_PF_LD2,
	PA9_PF_LD3,
	PA10_PF_LD4,
	PA11_PF_LD5,
	PA12_PF_LD6,
	PA13_PF_LD7,
	PA14_PF_LD8,
	PA15_PF_LD9,
	PA16_PF_LD10,
	PA17_PF_LD11,
	PA18_PF_LD12,
	PA19_PF_LD13,
	PA20_PF_LD14,
	PA21_PF_LD15,
	PA22_PF_LD16,
	PA28_PF_HSYNC,
	PA29_PF_VSYNC,
	PA30_PF_CONTRAST,
	PA31_PF_OE_ACD,
#endif
};

static struct physmap_flash_data vp6500_flash_data = {
	.width = 2,
};

static struct resource vp6500_flash_resource = {
	.start = MX21_CS0_BASE_ADDR,
	.end = MX21_CS0_BASE_ADDR + SZ_64M - 1,
	.flags = IORESOURCE_MEM,
};

static struct platform_device vp6500_nor_mtd_device = {
	.name = "physmap-flash",
	.id = -1,
	.dev = {
		.platform_data = &vp6500_flash_data,
	},
	.num_resources = 1,
	.resource = &vp6500_flash_resource,
};

static struct gpio_led vp6500_leds[] = {
	{
		.name = "vp6500:orange:keypad",
		.gpio = VP6500_GPIO_KEYPAD_LEDS,
	},
};

static struct gpio_led_platform_data vp6500_leds_data = {
	.leds		= vp6500_leds,
	.num_leds	= ARRAY_SIZE(vp6500_leds),
};

static struct platform_device vp6500_leds_device = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &vp6500_leds_data,
	},
};

static struct platform_device *platform_devices[] __initdata = {
	&vp6500_nor_mtd_device,
	&vp6500_leds_device,
};

static void __init vp6500_board_init(void)
{
	mxc_gpio_setup_multiple_pins(vp6500_pins, ARRAY_SIZE(vp6500_pins),
			"vp6500");

	mxc_register_device(&mxc_uart_device0, NULL);

	platform_add_devices(platform_devices, ARRAY_SIZE(platform_devices));
}

static void __init vp6500_timer_init(void)
{
	mx21_clocks_init(32768, 26000000);
}

static struct sys_timer vp6500_timer = {
	.init	= vp6500_timer_init,
};

MACHINE_START(VP6500, "Phillips VP6500")
	.phys_io        = MX21_AIPI_BASE_ADDR,
	.io_pg_offst    = ((MX21_AIPI_BASE_ADDR_VIRT) >> 18) & 0xfffc,
	.boot_params    = MX21_PHYS_OFFSET + 0x100,
	.map_io         = mx21_map_io,
	.init_irq       = mx21_init_irq,
	.init_machine   = vp6500_board_init,
	.timer          = &vp6500_timer,
MACHINE_END
