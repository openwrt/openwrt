/*
 *  D-Link DAP-1350 board support
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define DAP_1350_GPIO_LED_POWER_BLUE	8
#define DAP_1350_GPIO_LED_POWER_RED	9
#define DAP_1350_GPIO_LED_WPS		14

#define DAP_1350_GPIO_BUTTON_WPS	0	/* active low */
#define DAP_1350_GPIO_BUTTON_RESET	10	/* active low */
#define DAP_1350_GPIO_SWITCH_MODE_AP	7	/* active low */
#define DAP_1350_GPIO_SWITCH_MODE_RT	11	/* active low */

#define DAP_1350_KEYS_POLL_INTERVAL	20
#define DAP_1350_KEYS_DEBOUNCE_INTERVAL	(3 * DAP_1350_KEYS_POLL_INTERVAL)

static struct gpio_led dap_1350_leds_gpio[] __initdata = {
	{
		.name		= "d-link:blue:power",
		.gpio		= DAP_1350_GPIO_LED_POWER_BLUE,
		.active_low	= 1,
	}, {
		.name		= "d-link:red:power",
		.gpio		= DAP_1350_GPIO_LED_POWER_RED,
		.active_low	= 1,
	}, {
		.name		= "d-link:blue:wps",
		.gpio		= DAP_1350_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button dap_1350_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = DAP_1350_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DAP_1350_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DAP_1350_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DAP_1350_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}, {
		.desc		= "rt",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = DAP_1350_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DAP_1350_GPIO_SWITCH_MODE_RT,
		.active_low	= 1,
	}, {
		.desc		= "ap",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = DAP_1350_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DAP_1350_GPIO_SWITCH_MODE_AP,
		.active_low	= 1,
	}
};

static void __init dap_1350_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(dap_1350_leds_gpio),
				  dap_1350_leds_gpio);
	ramips_register_gpio_buttons(-1, DAP_1350_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(dap_1350_gpio_buttons),
				     dap_1350_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_DAP_1350, "DAP-1350", "D-Link DAP-1350",
	     dap_1350_init);
