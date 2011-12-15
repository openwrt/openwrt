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

#define JWAP003_KEYS_POLL_INTERVAL	20	/* msecs */
#define JWAP003_KEYS_DEBOUNCE_INTERVAL	(3 * JWAP003_KEYS_POLL_INTERVAL)

#define JWAP003_GPIO_WPS	11
#define JWAP003_GPIO_I2C_SCL	0
#define JWAP003_GPIO_I2C_SDA	1

static struct gpio_keys_button jwap003_gpio_keys[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = JWAP003_KEYS_DEBOUNCE_INTERVAL,
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

static const char *jwap003_part_probes[] = {
	"RedBoot",
	NULL,
};

static struct flash_platform_data jwap003_flash_data = {
	.part_probes	= jwap003_part_probes,
};

#define JWAP003_WAN_PHYMASK	BIT(0)
#define JWAP003_LAN_PHYMASK	BIT(4)

static void __init jwap003_init(void)
{
	ar71xx_add_device_m25p80(&jwap003_flash_data);

	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = JWAP003_WAN_PHYMASK;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.has_ar8216 = 1;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, ar71xx_mac_base, 1);
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = JWAP003_LAN_PHYMASK;
	ar71xx_eth1_data.speed = SPEED_100;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	platform_device_register(&jwap003_i2c_gpio_device);

	ar71xx_add_device_usb();

	ar71xx_register_gpio_keys_polled(-1, JWAP003_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(jwap003_gpio_keys),
					 jwap003_gpio_keys);

	pb42_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_JWAP003, "JWAP003", "jjPlus JWAP003", jwap003_init);
