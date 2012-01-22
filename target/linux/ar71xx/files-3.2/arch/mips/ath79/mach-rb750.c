/*
 *  MikroTik RouterBOARD 750 support
 *
 *  Copyright (C) 2010-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/export.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/mach-rb750.h>

#include "common.h"
#include "dev-eth.h"
#include "machtypes.h"

static struct rb750_led_data rb750_leds[] = {
	{
		.name		= "rb750:green:act",
		.mask		= RB750_LED_ACT,
		.active_low	= 1,
	}, {
		.name		= "rb750:green:port1",
		.mask		= RB750_LED_PORT5,
		.active_low	= 1,
	}, {
		.name		= "rb750:green:port2",
		.mask		= RB750_LED_PORT4,
		.active_low	= 1,
	}, {
		.name		= "rb750:green:port3",
		.mask		= RB750_LED_PORT3,
		.active_low	= 1,
	}, {
		.name		= "rb750:green:port4",
		.mask		= RB750_LED_PORT2,
		.active_low	= 1,
	}, {
		.name		= "rb750:green:port5",
		.mask		= RB750_LED_PORT1,
		.active_low	= 1,
	}
};

static struct rb750_led_platform_data rb750_leds_data = {
	.num_leds	= ARRAY_SIZE(rb750_leds),
	.leds		= rb750_leds,
};

static struct platform_device rb750_leds_device = {
	.name	= "leds-rb750",
	.dev	= {
		.platform_data = &rb750_leds_data,
	}
};

static struct platform_device rb750_nand_device = {
	.name	= "rb750-nand",
	.id	= -1,
};

int rb750_latch_change(u32 mask_clr, u32 mask_set)
{
	static DEFINE_SPINLOCK(lock);
	static u32 latch_set = RB750_LED_BITS | RB750_LVC573_LE;
	static u32 latch_oe;
	static u32 latch_clr;
	unsigned long flags;
	u32 t;
	int ret = 0;

	spin_lock_irqsave(&lock, flags);

	if ((mask_clr & BIT(31)) != 0 &&
	    (latch_set & RB750_LVC573_LE) == 0) {
		goto unlock;
	}

	latch_set = (latch_set | mask_set) & ~mask_clr;
	latch_clr = (latch_clr | mask_clr) & ~mask_set;

	if (latch_oe == 0)
		latch_oe = __raw_readl(ath79_gpio_base + AR71XX_GPIO_REG_OE);

	if (likely(latch_set & RB750_LVC573_LE)) {
		void __iomem *base = ath79_gpio_base;

		t = __raw_readl(base + AR71XX_GPIO_REG_OE);
		t |= mask_clr | latch_oe | mask_set;

		__raw_writel(t, base + AR71XX_GPIO_REG_OE);
		__raw_writel(latch_clr, base + AR71XX_GPIO_REG_CLEAR);
		__raw_writel(latch_set, base + AR71XX_GPIO_REG_SET);
	} else if (mask_clr & RB750_LVC573_LE) {
		void __iomem *base = ath79_gpio_base;

		latch_oe = __raw_readl(base + AR71XX_GPIO_REG_OE);
		__raw_writel(RB750_LVC573_LE, base + AR71XX_GPIO_REG_CLEAR);
		/* flush write */
		__raw_readl(base + AR71XX_GPIO_REG_CLEAR);
	}

	ret = 1;

unlock:
	spin_unlock_irqrestore(&lock, flags);
	return ret;
}
EXPORT_SYMBOL_GPL(rb750_latch_change);

void rb750_nand_pins_enable(void)
{
	ath79_gpio_function_setup(AR724X_GPIO_FUNC_JTAG_DISABLE,
				  AR724X_GPIO_FUNC_SPI_EN);
}
EXPORT_SYMBOL_GPL(rb750_nand_pins_enable);

void rb750_nand_pins_disable(void)
{
	ath79_gpio_function_setup(AR724X_GPIO_FUNC_SPI_EN,
				  AR724X_GPIO_FUNC_JTAG_DISABLE);
}
EXPORT_SYMBOL_GPL(rb750_nand_pins_disable);

static void __init rb750_setup(void)
{
	ath79_gpio_function_disable(AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ath79_init_mac(ath79_eth0_data.mac_addr, ath79_mac_base, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, ath79_mac_base, 1);

	ath79_register_mdio(0, 0x0);

	/* LAN ports */
	ath79_register_eth(1);

	/* WAN port */
	ath79_register_eth(0);

	platform_device_register(&rb750_leds_device);
	platform_device_register(&rb750_nand_device);
}

MIPS_MACHINE(ATH79_MACH_RB_750, "750i", "MikroTik RouterBOARD 750",
	     rb750_setup);
