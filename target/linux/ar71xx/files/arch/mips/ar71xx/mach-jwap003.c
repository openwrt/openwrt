/*
 *  jjPlus JWAP003 board support
 *
 */

#include <asm/mach-ar71xx/ar71xx.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/platform_device.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-pb42-pci.h"
#include "dev-usb.h"

#define JWAP003_BUTTONS_POLL_INTERVAL	20
#define JWAP003_GPIO_WPS	11
#define JWAP003_GPIO_I2C_SCL	0
#define JWAP003_GPIO_I2C_SDA	1

static struct gpio_button jwap003_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= JWAP003_GPIO_WPS,
		.active_low	= 1,
	}
};

static struct i2c_gpio_platform_data jwap003_i2c_gpio_data = {
	.sda_pin	= JWAP003_GPIO_I2C_SDA,
	.scl_pin	= JWAP003_GPIO_I2C_SCL,
};

static struct platform_device jwap003_i2c_gpio_device = {
	.name		= "i2c-gpio",
	.id		= 0,
	.dev = {
		.platform_data  = &jwap003_i2c_gpio_data,
	}
};

#define JWAP003_WAN_PHYMASK	BIT(0)
#define JWAP003_LAN_PHYMASK	BIT(4)

static void __init jwap003_init(void)
{
	ar71xx_add_device_m25p80(NULL);

	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = JWAP003_WAN_PHYMASK;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.has_ar8216 = 1;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = JWAP003_LAN_PHYMASK;
	ar71xx_eth1_data.speed = SPEED_100;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	platform_device_register(&jwap003_i2c_gpio_device);

	ar71xx_add_device_usb();

	ar71xx_add_device_gpio_buttons(-1, JWAP003_BUTTONS_POLL_INTERVAL,
				       ARRAY_SIZE(jwap003_gpio_buttons),
				       jwap003_gpio_buttons);

	pb42_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_JWAP003, "JWAP003", "jjPlus JWAP003", jwap003_init);
