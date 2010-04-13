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
#include <linux/gpio_keys.h>
#include <linux/pwm_backlight.h>

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

	/* LCD Backlight */
	PE5_PF_PWMO,
	VP6500_GPIO_BACKLIGHT_EN | GPIO_GPIO | GPIO_OUT,
};

/* Flash */

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

/* LEDs */

static struct gpio_led vp6500_leds[] = {
	{
		.name = "vp6500:orange:keypad",
		.gpio = VP6500_GPIO_LED_KEYPAD,
	},
	{
		.name = "vp6500:green:",
		.gpio = VP6500_GPIO_LED_GREEN,
		.active_low = 1,
		.default_state = LEDS_GPIO_DEFSTATE_ON,
	},
	{
		.name = "vp6500:red:",
		.gpio = VP6500_GPIO_LED_RED,
	},
	{
		.name = "vp6500:red:camera",
		.gpio = VP6500_GPIO_LED_CAMERA,
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

/* Keypad */

static const uint32_t vp6500_keypad_keys[] = {
	KEY(0, 3, KEY_F2),
	KEY(0, 4, KEY_RIGHT),
	KEY(1, 0, KEY_ZOOM),
	KEY(1, 1, KEY_NUMERIC_POUND),
	KEY(1, 2, KEY_0),
	KEY(1, 3, KEY_ENTER),
	KEY(1, 4, KEY_8),
	KEY(2, 0, KEY_5),
	KEY(2, 1, KEY_2),
	KEY(2, 2, KEY_DOWN),
	KEY(2, 3, KEY_OK),
	KEY(2, 4, KEY_UP),
	KEY(3, 0, KEY_CAMERA),
	KEY(3, 1, KEY_NUMERIC_STAR),
	KEY(3, 2, KEY_9),
	KEY(3, 3, KEY_LEFT),
	KEY(3, 4, KEY_6),
	KEY(4, 0, KEY_7),
	KEY(4, 1, KEY_4),
	KEY(4, 2, KEY_1),
	KEY(4, 3, KEY_3),
	KEY(4, 4, KEY_F1),
};

static struct matrix_keymap_data vp6500_keypad_data = {
	.keymap = vp6500_keypad_keys,
	.keymap_size = ARRAY_SIZE(vp6500_keypad_keys),
};

static struct resource vp6500_keypad_resources[] = {
	{
		.start = MX21_KPP_BASE_ADDR,
		.end = MX21_KPP_BASE_ADDR + 0x10 - 1,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = MX21_INT_KPP,
		.flags = IORESOURCE_IRQ,
	}
};

static struct platform_device vp6500_keypad_device = {
	.name = "imx-keypad",
	.id = 0,
	.dev = {
		.platform_data = &vp6500_keypad_data,
	},
	.resource = vp6500_keypad_resources,
	.num_resources = ARRAY_SIZE(vp6500_keypad_resources),
};

static struct gpio_keys_button vp6500_keys = {
	.gpio = VP6500_GPIO_POWER_KEY,
	.code = KEY_POWER,
	.desc = "Power button",
	.active_low = 1,
};

static struct gpio_keys_platform_data vp6500_key_data = {
	.buttons = &vp6500_keys,
	.nbuttons = 1,
};

static struct platform_device vp6500_key_device = {
	.name = "gpio-keys",
	.id = -1,
	.dev = {
		.platform_data = &vp6500_key_data,
	},
};

/* LCD backlight */

static int vp6500_backlight_notify(struct device *dev, int brightness)
{
	gpio_set_value(VP6500_GPIO_BACKLIGHT_EN, !!brightness);

	return brightness;
}

static struct platform_pwm_backlight_data vp6500_backlight_data = {
	.max_brightness = 255,
	.dft_brightness = 100,
	.pwm_period_ns = 15000000,
	.notify = vp6500_backlight_notify,
};

static struct platform_device vp6500_backlight_device = {
	.name = "pwm-backlight",
	.id = -1,
	.dev = {
		.parent = &mxc_pwm_device.dev,
		.platform_data = &vp6500_backlight_data,
	},
};


static struct platform_device *platform_devices[] __initdata = {
	&vp6500_nor_mtd_device,
	&vp6500_leds_device,
	&vp6500_keypad_device,
	&vp6500_key_device,
	&vp6500_backlight_device,
};

static void __init vp6500_board_init(void)
{
	mxc_gpio_setup_multiple_pins(vp6500_pins, ARRAY_SIZE(vp6500_pins),
			"vp6500");

	mxc_register_device(&mxc_uart_device0, NULL);
	mxc_register_device(&mxc_pwm_device, NULL);

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
