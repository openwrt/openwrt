#include <linux/init.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/etherdevice.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/irq.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include <linux/platform_data/phy-at803x.h>
#include <linux/ar8216_platform.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"


#define UBNT_WA_PHY_ID 4

#define UBNT_WA_KEYS_POLL_INTERVAL 20
#define UBNT_WA_KEYS_DEBOUNCE_INTERVAL (3 * UBNT_WA_KEYS_POLL_INTERVAL)

#define UBNT_WA_GPIO_BTN_RESET 12

#define UBNT_WA_ETH0_MAC_OFFSET         0x0000
#define UBNT_WA_WMAC_CALDATA_OFFSET     0x1000
#define UBNT_WA_PCI_CALDATA_OFFSET      0x5000


static struct flash_platform_data ubnt_wa_flash_data = {
	/* mx25l12805d and mx25l12835f have the same JEDEC ID */
	.type = "mx25l12805d",
};

static struct gpio_keys_button ubnt_wa_gpio_keys[] __initdata = {
	{
		.desc			= "reset",
		.type			= EV_KEY,
		.code			= KEY_RESTART,
		.debounce_interval	= UBNT_WA_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= UBNT_WA_GPIO_BTN_RESET,
		.active_low		= 1,
	}
};

static void __init ubnt_wa_setup(void)
{
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(&ubnt_wa_flash_data);

	// Register mdio interface
	ath79_register_mdio(0, ~BIT(UBNT_WA_PHY_ID));

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0
		| AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_data.phy_mask = BIT(UBNT_WA_PHY_ID);
	ath79_eth0_pll_data.pll_1000 = 0x06000000;
	ath79_eth0_pll_data.pll_10 = 0x00001313;

	ath79_setup_ar934x_eth_rx_delay(3, 3);


	ath79_init_mac(ath79_eth0_data.mac_addr, eeprom
		+ UBNT_WA_ETH0_MAC_OFFSET, 0);
	ath79_register_eth(0);


	ath79_register_wmac(eeprom
		+ UBNT_WA_WMAC_CALDATA_OFFSET, NULL);


	ap91_pci_init(eeprom + UBNT_WA_PCI_CALDATA_OFFSET, NULL);



	ath79_register_gpio_keys_polled(-1, UBNT_WA_KEYS_POLL_INTERVAL,
	                                ARRAY_SIZE(ubnt_wa_gpio_keys),
	                                ubnt_wa_gpio_keys);

}

MIPS_MACHINE(ATH79_MACH_UBNT_WA, "UBNT-WA",
	     "Ubiquiti Networks WA board", ubnt_wa_setup);

MIPS_MACHINE(ATH79_MACH_UBNT_LITEBEAMACAP, "UBNT-LITEBEAMACAP",
	     "Ubiquiti LiteBeam ac AP", ubnt_wa_setup);

MIPS_MACHINE(ATH79_MACH_UBNT_NANOSTATIONACL, "UBNT-NANOSTATION-ACL",
	     "Ubiquiti Nanostation AC loco", ubnt_wa_setup);
