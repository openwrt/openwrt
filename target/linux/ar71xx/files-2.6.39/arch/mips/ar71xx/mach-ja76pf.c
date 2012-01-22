/*
 *  jjPlus JA76PF board support
 */

#include <asm/mach-ar71xx/ar71xx.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-pb42-pci.h"
#include "dev-usb.h"
#include "dev-leds-gpio.h"

#define JA76PF_KEYS_POLL_INTERVAL	20	/* msecs */
#define JA76PF_KEYS_DEBOUNCE_INTERVAL	(3 * JA76PF_KEYS_POLL_INTERVAL)

#define JA76PF_GPIO_I2C_SCL		0
#define JA76PF_GPIO_I2C_SDA		1
#define JA76PF_GPIO_LED_1		5
#define JA76PF_GPIO_LED_2		4
#define JA76PF_GPIO_LED_3		3
#define JA76PF_GPIO_BTN_RESET		11

static struct gpio_led ja76pf_leds_gpio[] __initdata = {
	{
		.name		= "ja76pf:green:led1",
		.gpio		= JA76PF_GPIO_LED_1,
		.active_low	= 1,
	}, {
		.name		= "ja76pf:green:led2",
		.gpio		= JA76PF_GPIO_LED_2,
		.active_low	= 1,
	}, {
		.name		= "ja76pf:green:led3",
		.gpio		= JA76PF_GPIO_LED_3,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ja76pf_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = JA76PF_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= JA76PF_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct i2c_gpio_platform_data ja76pf_i2c_gpio_data = {
	.sda_pin	= JA76PF_GPIO_I2C_SDA,
	.scl_pin	= JA76PF_GPIO_I2C_SCL,
};

static struct platform_device ja76pf_i2c_gpio_device = {
	.name		= "i2c-gpio",
	.id		= 0,
	.dev = {
		.platform_data  = &ja76pf_i2c_gpio_data,
	}
};

static const char *ja76pf_part_probes[] = {
	"RedBoot",
	NULL,
};

static struct flash_platform_data ja76pf_flash_data = {
	.part_probes	= ja76pf_part_probes,
};

#define JA76PF_WAN_PHYMASK	(1 << 4)
#define JA76PF_LAN_PHYMASK	((1 << 0) | (1 << 1) | (1 << 2) | (1 < 3))
#define JA76PF_MDIO_PHYMASK	(JA76PF_LAN_PHYMASK | JA76PF_WAN_PHYMASK)

static void __init ja76pf_init(void)
{
	ar71xx_add_device_m25p80(&ja76pf_flash_data);

	ar71xx_add_device_mdio(0, ~JA76PF_MDIO_PHYMASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = JA76PF_LAN_PHYMASK;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, ar71xx_mac_base, 1);
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = JA76PF_WAN_PHYMASK;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	platform_device_register(&ja76pf_i2c_gpio_device);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ja76pf_leds_gpio),
					ja76pf_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, JA76PF_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ja76pf_gpio_keys),
					 ja76pf_gpio_keys);

	ar71xx_add_device_usb();
	pb42_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_JA76PF, "JA76PF", "jjPlus JA76PF", ja76pf_init);
