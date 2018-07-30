/*
 * TP-LINK EAP245 v1 board support
 *
 * Copyright (c) 2016 The Linux Foundation. All rights reserved.
 * Copyright (c) 2017 Julien Dusser <julien.dusser@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Based on the Qualcomm Atheros u-boot-1.1.4 board956x support code
 *   Copyright (c) 2014, 2016 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 */

#include <ath79.h>
#include <linux/ath9k_platform.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <linux/platform_data/phy-at803x.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

/*
 * EAP245 v1 board support wifi AC1750 and gigabit LAN:
 *  - QCA9563-AL3A MIPS 74kc and 2.4GHz wifi
 *  - QCA9880-BR4A 5 GHz wifi ath10k
 *  - AR8033-AL1A 1 gigabit lan port (@4 via GPIO 8-10 MDIO bus)
 *  - 25Q128CSIG SPI NOR
 *
 *  - GPIO1  red led
 *  - GPIO2  reset button
 *  - GPIO4  reset the board when direction is changed
 *  - GPIO5  master switch for all leds
 *  - GPIO7  green led
 *  - GPIO8  MDC
 *  - GPIO9  yellow led
 *  - GPIO10 MDIO
 *  - GPIO15 test point?
 *  - GPIO16 UART RX
 *  - GPIO17 UART TX
 */
#define EAP245_V1_GPIO_LED_RED			1
#define EAP245_V1_GPIO_LED_YEL			9
#define EAP245_V1_GPIO_LED_GRN			7
#define EAP245_V1_GPIO_LED_ALL			5
#define EAP245_V1_GPIO_BTN_RESET		2
#define EAP245_V1_KEYS_POLL_INTERVAL		20 /* msecs */
#define EAP245_V1_KEYS_DEBOUNCE_INTERVAL	(3 * EAP245_V1_KEYS_POLL_INTERVAL)

#define EAP245_V1_GPIO_SMI_MDC			8
#define EAP245_V1_GPIO_SMI_MDIO			10
#define EAP245_V1_LAN_PHYADDR			4

#define EAP245_V1_QCA956X_SGMII_LINK_MAX_RESET	10

/*
 * Factory U-Boot does not initialize SGMII on normal boot.
 * Some other boards should be concerned.
 *
 * Add initialization code from QC/A SDK U-Boot 1.1.4.
 * Note that on my board eap245_v1_test_sgmii_res_cal is always 0x7.
 *
 * If you are using a good U-Boot, you can disable next define
 */
#define EAP245_V1_FACTORY_UBOOT

static struct gpio_led eap245_v1_leds_gpio[] __initdata = {
	{
		.name = "eap245-v1:red:system",
		.gpio = EAP245_V1_GPIO_LED_RED,
	}, {
		.name = "eap245-v1:yellow:system",
		.gpio = EAP245_V1_GPIO_LED_YEL,
	}, {
		.name = "eap245-v1:green:system",
		.gpio = EAP245_V1_GPIO_LED_GRN,
	},
};

static struct gpio_keys_button eap245_v1_gpio_keys[] __initdata = {
	{
		.desc = "Reset button",
		.type = EV_KEY,
		.code = KEY_RESTART,
		.debounce_interval = EAP245_V1_KEYS_DEBOUNCE_INTERVAL,
		.gpio = EAP245_V1_GPIO_BTN_RESET,
		.active_low = 1,
	},
};

/* AR8033 PHY settings */
static struct at803x_platform_data eap245_v1_ar8033_data = {
	.disable_smarteee = 0,
	.enable_rgmii_rx_delay = 1,
	.enable_rgmii_tx_delay = 0,
	.fixup_rgmii_tx_delay = 1,
};

static struct mdio_board_info eap245_v1_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = EAP245_V1_LAN_PHYADDR,
		.platform_data = &eap245_v1_ar8033_data,
	},
};

static void __init eap245_v1_setup_qca956x_eth_cfg(u32 mask, u32 rxd, u32 rdv)
{
	void __iomem *base;
	u32 t;

	base = ioremap(AR71XX_MII_BASE, AR71XX_MII_SIZE);

	t = mask;
	t |= rxd << QCA956X_ETH_CFG_RXD_DELAY_SHIFT;
	t |= rdv << QCA956X_ETH_CFG_RDV_DELAY_SHIFT;

	__raw_writel(t, base + QCA956X_GMAC_REG_ETH_CFG);

	iounmap(base);
}

#ifdef EAP245_V1_FACTORY_UBOOT
/* Find SGMII VCO resistor value boundaries and return mean */
static u32 __init eap245_v1_test_sgmii_res_cal(void)
{
	void __iomem *base = ioremap_nocache(AR71XX_MII_BASE, AR71XX_MII_SIZE);
	u32 reversed_sgmii_value, i, startValue = 0, endValue = 0;

	ath79_pll_wr(QCA956X_PLL_ETH_SGMII_SERDES_REG,
			QCA956X_PLL_ETH_SGMII_SERDES_LOCK_DETECT |
			QCA956X_PLL_ETH_SGMII_SERDES_EN_PLL);

	/* set resistor calibration from 0000 -> 1111 */
	for (i = 0; i < 0x10; i++) {
		u32 vco_before, vco_after, t;

		t = __raw_readl(base + QCA956X_GMAC_REG_SGMII_SERDES);
		vco_before = t & (QCA956X_SGMII_SERDES_VCO_FAST |
							QCA956X_SGMII_SERDES_VCO_SLOW);

		t &= ~(QCA956X_SGMII_SERDES_RES_CALIBRATION_MASK
				<< QCA956X_SGMII_SERDES_RES_CALIBRATION_SHIFT);
		t |= i << QCA956X_SGMII_SERDES_RES_CALIBRATION_SHIFT;
		__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_SERDES);

		udelay(50);

		vco_after = __raw_readl(base + QCA956X_GMAC_REG_SGMII_SERDES) &
						(QCA956X_SGMII_SERDES_VCO_FAST |
								QCA956X_SGMII_SERDES_VCO_SLOW);

		if (vco_before != vco_after) {
			if (startValue == 0) {
				startValue = endValue = i;
			} else {
				endValue = i;
				break;
			}
		}
	}

	if (startValue == 0) {
		/* No boundary found, use middle value for resistor calibration */
		reversed_sgmii_value = 0x7;
	} else {
		/* get resistor calibration from the middle of boundary */
		reversed_sgmii_value = (startValue + endValue) / 2;
	}

	pr_debug("SERDES calibration: %i (min: %i, max: %i)", reversed_sgmii_value,
			startValue, endValue);
	iounmap(base);

	return reversed_sgmii_value;
}

static void __init eap245_v1_setup_qca956x_eth_serdes_cal(u32 sgmii_value)
{
	void __iomem *base;
	u32 t;

	base = ioremap_nocache(AR71XX_MII_BASE, AR71XX_MII_SIZE);

	t = __raw_readl(base + QCA956X_GMAC_REG_SGMII_SERDES);
	t &= ~(QCA956X_SGMII_SERDES_RES_CALIBRATION_MASK
			<< QCA956X_SGMII_SERDES_RES_CALIBRATION_SHIFT);
	t |= (sgmii_value & QCA956X_SGMII_SERDES_RES_CALIBRATION_MASK)
			<< QCA956X_SGMII_SERDES_RES_CALIBRATION_SHIFT;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_SERDES);

	ath79_pll_wr(QCA956X_PLL_ETH_SGMII_SERDES_REG,
			QCA956X_PLL_ETH_SGMII_SERDES_LOCK_DETECT
					| QCA956X_PLL_ETH_SGMII_SERDES_EN_PLL);

	t = __raw_readl(base + QCA956X_GMAC_REG_SGMII_SERDES);

	/* missing in QCA u-boot code, clear before setting */
	t &= ~(QCA956X_SGMII_SERDES_CDR_BW_MASK
					<< QCA956X_SGMII_SERDES_CDR_BW_SHIFT |
			QCA956X_SGMII_SERDES_TX_DR_CTRL_MASK
					<< QCA956X_SGMII_SERDES_TX_DR_CTRL_SHIFT |
			QCA956X_SGMII_SERDES_VCO_REG_MASK
					<< QCA956X_SGMII_SERDES_VCO_REG_SHIFT);

	t |= (3 << QCA956X_SGMII_SERDES_CDR_BW_SHIFT) |
			(1 << QCA956X_SGMII_SERDES_TX_DR_CTRL_SHIFT) |
			QCA956X_SGMII_SERDES_PLL_BW |
			QCA956X_SGMII_SERDES_EN_SIGNAL_DETECT |
			QCA956X_SGMII_SERDES_FIBER_SDO |
			(3 << QCA956X_SGMII_SERDES_VCO_REG_SHIFT);

	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_SERDES);

	ath79_device_reset_clear(QCA956X_RESET_SGMII_ANALOG);
	ath79_device_reset_clear(QCA956X_RESET_SGMII);

	while (!(__raw_readl(base + QCA956X_GMAC_REG_SGMII_SERDES)
			& QCA956X_SGMII_SERDES_LOCK_DETECT_STATUS))
		;

	iounmap(base);
}

static void __init eap245_v1_setup_qca956x_sgmii(void)
{
	void __iomem *base;
	u32 count = 0, t;

	base = ioremap_nocache(AR71XX_MII_BASE, AR71XX_MII_SIZE);

	t = 2 << QCA956X_SGMII_CONFIG_MODE_CTRL_SHIFT;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_CONFIG);

	t = QCA956X_MR_AN_CONTROL_AN_ENABLE | QCA956X_MR_AN_CONTROL_PHY_RESET;
	__raw_writel(t, base + QCA956X_GMAC_REG_MR_AN_CONTROL);

	/* SGMII reset sequence suggested by systems team */
	t = QCA956X_SGMII_RESET_RX_CLK_N_RESET;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_RESET);

	t = QCA956X_SGMII_RESET_HW_RX_125M_N;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_RESET);

	t |= QCA956X_SGMII_RESET_RX_125M_N;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_RESET);

	t |= QCA956X_SGMII_RESET_TX_125M_N;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_RESET);

	t |= QCA956X_SGMII_RESET_RX_CLK_N;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_RESET);

	t |= QCA956X_SGMII_RESET_TX_CLK_N;
	__raw_writel(t, base + QCA956X_GMAC_REG_SGMII_RESET);

	t = __raw_readl(base + QCA956X_GMAC_REG_MR_AN_CONTROL)
			& ~QCA956X_MR_AN_CONTROL_PHY_RESET;
	__raw_writel(t, base + QCA956X_GMAC_REG_MR_AN_CONTROL);

	/*
	 * Across resets SGMII link status goes to weird state.
	 * if SGMII_DEBUG register reads other then 0x1f or 0x10
	 * for sure we are in bad state.
	 * Issue a PHY reset in QCA956X_GMAC_REG_MR_AN_CONTROL to keep going. */
	while (count++ < EAP245_V1_QCA956X_SGMII_LINK_MAX_RESET) {
		u8 status = (u8) __raw_readl(base + QCA956X_GMAC_REG_SGMII_DEBUG);
		if (status == 0x1f || status == 0x10)
			break;

		__raw_writel(t | QCA956X_MR_AN_CONTROL_PHY_RESET,
				base + QCA956X_GMAC_REG_MR_AN_CONTROL);
		udelay(100);
		__raw_writel(t, base + QCA956X_GMAC_REG_MR_AN_CONTROL);
	}

	if (count == EAP245_V1_QCA956X_SGMII_LINK_MAX_RESET)
		pr_warn("Max resets limit reached exiting...\n");

	iounmap(base);
}
#endif /* EAP245_V1_FACTORY_UBOOT */

static void __init eap245_v1_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f030008);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 wmac_addr[ETH_ALEN];
#ifdef EAP245_V1_FACTORY_UBOOT
	u32 reversed_sgmii_value;

	/* Find SERDES calibration */
	reversed_sgmii_value = eap245_v1_test_sgmii_res_cal();
#endif
	ath79_register_leds_gpio(-1, ARRAY_SIZE(eap245_v1_leds_gpio),
			eap245_v1_leds_gpio);

	ath79_register_gpio_keys_polled(-1, EAP245_V1_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(eap245_v1_gpio_keys), eap245_v1_gpio_keys);

	ath79_register_m25p80(NULL);
	eap245_v1_setup_qca956x_eth_cfg(
			QCA956X_ETH_CFG_RGMII_EN | QCA956X_ETH_CFG_GE0_SGMII, 3, 3);

	/* Configure MDIO bus */
	ath79_gpio_output_select(EAP245_V1_GPIO_SMI_MDC,
			QCA956X_GPIO_OUT_MUX_GE0_MDC);
	ath79_gpio_output_select(EAP245_V1_GPIO_SMI_MDIO,
			QCA956X_GPIO_OUT_MUX_GE0_MDO);
	ath79_register_mdio(0, 0x0);

	/* Store PHY settings */
	mdiobus_register_board_info(eap245_v1_mdio0_info,
			ARRAY_SIZE(eap245_v1_mdio0_info));

#ifdef EAP245_V1_FACTORY_UBOOT
	/* Calibrate SERDES */
	eap245_v1_setup_qca956x_eth_serdes_cal(reversed_sgmii_value);
	eap245_v1_setup_qca956x_sgmii();
#endif
	/* Set lan port to eepromm mac address */
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(EAP245_V1_LAN_PHYADDR);
	ath79_register_eth(0);

	/* Set 2.4 GHz to mac + 1 */
	ath79_init_mac(wmac_addr, mac, 1);
	ath79_register_wmac(ee, wmac_addr);

	/* Initializer pci bus for ath10k (5GHz) */
	ath79_register_pci();
}

MIPS_MACHINE(ATH79_MACH_EAP245_V1, "EAP245-V1", "TP-LINK EAP245 v1",
		eap245_v1_setup);
