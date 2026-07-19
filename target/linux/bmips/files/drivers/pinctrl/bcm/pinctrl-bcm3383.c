// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for BCM3383 pin controller
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/bitfield.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>

#include "pinctrl-bcm338x.h"

// GpioGpioRgmiiPadCtrl
#define BCM3383_PAD_CTRL_REG		0x18
// GPL bcm3383/gpio_regs.h: GpioRegs.PadCtrl.Bits.Core0Rgmii1Sel0
#define BCM3383_PAD_CTRL_CORE0_RGMII1_SEL0	BIT(9)
// GPL bcm3383/gpio_regs.h: GpioRegs.PadCtrl.Bits.Core0Rgmii0Sel0
#define BCM3383_PAD_CTRL_CORE0_RGMII0_SEL0	BIT(4)
#define BCM3383_PAD_CTRL_RGMII0_EXTERNAL_MASK	BCM3383_PAD_CTRL_CORE0_RGMII0_SEL0
#define BCM3383_PAD_CTRL_RGMII0_EXTERNAL	BCM3383_PAD_CTRL_CORE0_RGMII0_SEL0
#define BCM3383_PAD_CTRL_GMAC1_INTERNAL_MASK	BCM3383_PAD_CTRL_CORE0_RGMII1_SEL0
#define BCM3383_PAD_CTRL_GMAC1_INTERNAL		BCM3383_PAD_CTRL_CORE0_RGMII1_SEL0

// GpioPinMuxSel
#define BCM3383_PINMUX_REG		0xc0
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio2928
#define BCM3383_PINMUX_PM_SELECT_GPIO_2928	GENMASK(31, 30)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio2724
#define BCM3383_PINMUX_PM_SELECT_GPIO_2724	GENMASK(29, 27)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio2320
#define BCM3383_PINMUX_PM_SELECT_GPIO_2320	GENMASK(26, 24)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio1918
#define BCM3383_PINMUX_PM_SELECT_GPIO_1918	GENMASK(23, 21)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio1716
#define BCM3383_PINMUX_PM_SELECT_GPIO_1716	GENMASK(20, 18)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio1514
#define BCM3383_PINMUX_PM_SELECT_GPIO_1514	GENMASK(17, 15)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio1310
#define BCM3383_PINMUX_PM_SELECT_GPIO_1310	GENMASK(14, 12)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio0908
#define BCM3383_PINMUX_PM_SELECT_GPIO_0908	GENMASK(11, 9)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio0706
#define BCM3383_PINMUX_PM_SELECT_GPIO_0706	GENMASK(8, 6)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio0504
#define BCM3383_PINMUX_PM_SELECT_GPIO_0504	GENMASK(5, 3)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSel.Bits.PmSelectGpio0300
#define BCM3383_PINMUX_PM_SELECT_GPIO_0300	GENMASK(2, 0)

// GpioPinMuxSelMid
#define BCM3383_PINMUX_MID_REG		0xc4
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii1Tx0
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII1_TX0	BIT(31)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii1Rx
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RX	GENMASK(30, 28)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii1Rxd
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RXD	GENMASK(27, 25)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii0Txd
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TXD	GENMASK(24, 22)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii0Tx
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TX	GENMASK(21, 19)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii0Rx
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RX	GENMASK(18, 16)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectRgmii0Rxd
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RXD	GENMASK(15, 13)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectGpio4138
#define BCM3383_PINMUX_MID_PM_SELECT_GPIO_4138	GENMASK(12, 10)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectGpio3736
#define BCM3383_PINMUX_MID_PM_SELECT_GPIO_3736	GENMASK(9, 7)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectGpio3532
#define BCM3383_PINMUX_MID_PM_SELECT_GPIO_3532	GENMASK(6, 4)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectGpio3130
#define BCM3383_PINMUX_MID_PM_SELECT_GPIO_3130	GENMASK(3, 1)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelMid.Bits.PmSelectGpio29282
#define BCM3383_PINMUX_MID_PM_SELECT_GPIO_29282	BIT(0)
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RX_INT	FIELD_PREP_CONST(BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RX, 1)
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RXD_INT	FIELD_PREP_CONST(BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RXD, 1)
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TXD_EXT	FIELD_PREP_CONST(BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TXD, 1)
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TX_EXT	FIELD_PREP_CONST(BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TX, 1)
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RX_EXT	FIELD_PREP_CONST(BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RX, 1)
#define BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RXD_EXT	FIELD_PREP_CONST(BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RXD, 1)
#define BCM3383_PINMUX_MID_RGMII0_EXTERNAL_MASK		(BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TXD | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TX | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RX | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RXD)
#define BCM3383_PINMUX_MID_RGMII0_EXTERNAL		(BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TXD_EXT | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII0_TX_EXT | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RX_EXT | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII0_RXD_EXT)
#define BCM3383_PINMUX_MID_GMAC1_INTERNAL_MASK		(BCM3383_PINMUX_MID_PM_SELECT_RGMII1_TX0 | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RX | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RXD)
#define BCM3383_PINMUX_MID_GMAC1_INTERNAL		(BCM3383_PINMUX_MID_PM_SELECT_RGMII1_TX0 | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RX_INT | \
							 BCM3383_PINMUX_MID_PM_SELECT_RGMII1_RXD_INT)

// GpioPinMuxSelHi1
#define BCM3383_PINMUX_HI1_REG		0xc8
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectUsOe
#define BCM3383_PINMUX_HI1_PM_SELECT_US_OE		GENMASK(31, 29)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectTpSc
#define BCM3383_PINMUX_HI1_PM_SELECT_TP_SC		GENMASK(28, 26)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectTpSpim
#define BCM3383_PINMUX_HI1_PM_SELECT_TP_SPIM		GENMASK(25, 23)
#define BCM3383_PINMUX_HI1_PM_SELECT_TP_SPIM_SWITCH	FIELD_PREP_CONST(BCM3383_PINMUX_HI1_PM_SELECT_TP_SPIM, 1)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectTpSpi
#define BCM3383_PINMUX_HI1_PM_SELECT_TP_SPI		GENMASK(22, 20)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectSpisUart1
#define BCM3383_PINMUX_HI1_PM_SELECT_SPIS_UART1		GENMASK(19, 17)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectPass
#define BCM3383_PINMUX_HI1_PM_SELECT_PASS		GENMASK(16, 14)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectHvg
#define BCM3383_PINMUX_HI1_PM_SELECT_HVG		GENMASK(13, 11)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectBmu
#define BCM3383_PINMUX_HI1_PM_SELECT_BMU		GENMASK(10, 8)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectMdio
#define BCM3383_PINMUX_HI1_PM_SELECT_MDIO		GENMASK(7, 5)
#define BCM3383_PINMUX_HI1_PM_SELECT_MDIO_GMAC		FIELD_PREP_CONST(BCM3383_PINMUX_HI1_PM_SELECT_MDIO, 1)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectRgmii1Txd
#define BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TXD		GENMASK(4, 2)
#define BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TXD_INT	FIELD_PREP_CONST(BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TXD, 1)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi1.Bits.PmSelectRgmii1Tx21
#define BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TX21	GENMASK(1, 0)
#define BCM3383_PINMUX_HI1_RGMII0_EXTERNAL_MASK		BCM3383_PINMUX_HI1_PM_SELECT_MDIO
#define BCM3383_PINMUX_HI1_RGMII0_EXTERNAL		BCM3383_PINMUX_HI1_PM_SELECT_MDIO_GMAC
#define BCM3383_PINMUX_HI1_HSSPI_CS2			BCM3383_PINMUX_HI1_PM_SELECT_TP_SPIM_SWITCH
#define BCM3383_PINMUX_HI1_GMAC1_INTERNAL_MASK		(BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TXD | \
							 BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TX21)
#define BCM3383_PINMUX_HI1_GMAC1_INTERNAL		BCM3383_PINMUX_HI1_PM_SELECT_RGMII1_TXD_INT

// GpioPinMuxSelHi2
#define BCM3383_PINMUX_HI2_REG		0x20
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi2.Bits.PmSelectTpMisc
#define BCM3383_PINMUX_HI2_PM_SELECT_TP_MISC	GENMASK(11, 9)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi2.Bits.PmSelectLed0700
#define BCM3383_PINMUX_HI2_PM_SELECT_LED_0700	GENMASK(8, 6)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi2.Bits.PmSelectTpUsb
#define BCM3383_PINMUX_HI2_PM_SELECT_TP_USB	GENMASK(5, 3)
// GPL bcm3383/gpio_regs.h: GpioRegs.PinMuxSelHi2.Bits.PmSelectTpPcie
#define BCM3383_PINMUX_HI2_PM_SELECT_TP_PCIE	GENMASK(2, 0)

#define BCM3383_MUX_FIELD(_reg, _value)		\
	{					\
		.reg = (_reg),			\
		.mask = (_value),		\
		.value = (_value),		\
	}

#define BCM3383_MUX_MASKED_FIELD(_reg, _mask, _value)	\
	{						\
		.reg = (_reg),				\
		.mask = (_mask),			\
		.value = (_value),			\
	}

static const struct pinctrl_pin_desc bcm3383_pins[] = {
	PINCTRL_PIN(0, "rgmii0"),
	PINCTRL_PIN(1, "gmac1_internal"),
};

static unsigned int rgmii0_pins[] = { 0 };
static unsigned int gmac1_internal_pins[] = { 1 };

static const struct pingroup bcm3383_groups[] = {
	BCM338X_PIN_GROUP(rgmii0),
	BCM338X_PIN_GROUP(gmac1_internal),
	PINCTRL_PINGROUP("hsspi_cs2", NULL, 0),
};

static const char * const rgmii0_groups[] = {
	"rgmii0",
};

static const char * const gmac1_internal_groups[] = {
	"gmac1_internal",
};

static const char * const hsspi_cs2_groups[] = {
	"hsspi_cs2",
};

/*
 * eCos configures GMAC0 external/RGMII0 by selecting MDIO plus the RGMII0
 * pinmux fields.  The stock bootloader also ORs raw RGMII1 field values in
 * its external path, but those fields are owned by the GMAC1/internal group
 * below and must not be rewritten from the RGMII0 pinctrl state.
 */
static const struct bcm338x_mux_field rgmii0_fields[] = {
	BCM3383_MUX_MASKED_FIELD(BCM3383_PAD_CTRL_REG,
				 BCM3383_PAD_CTRL_RGMII0_EXTERNAL_MASK,
				 BCM3383_PAD_CTRL_RGMII0_EXTERNAL),
	BCM3383_MUX_MASKED_FIELD(BCM3383_PINMUX_MID_REG,
				 BCM3383_PINMUX_MID_RGMII0_EXTERNAL_MASK,
				 BCM3383_PINMUX_MID_RGMII0_EXTERNAL),
	BCM3383_MUX_MASKED_FIELD(BCM3383_PINMUX_HI1_REG,
				 BCM3383_PINMUX_HI1_RGMII0_EXTERNAL_MASK,
				 BCM3383_PINMUX_HI1_RGMII0_EXTERNAL),
};

/*
 * Flash cold boot through second-stage U-Boot reaches Linux without the stock
 * HSSPI CS2/TP SPIM mux state needed by the BCM53125 SPI path.  UniMAC0 owns
 * the RGMII0 mux separately, so this state only sets PmSelectTpSpim.
 */
static const struct bcm338x_mux_field hsspi_cs2_fields[] = {
	BCM3383_MUX_FIELD(BCM3383_PINMUX_HI1_REG,
			  BCM3383_PINMUX_HI1_HSSPI_CS2),
};

/*
 * eCos uses this masked GMAC1 mux when it selects the second UniMAC data path.
 * The bootloader's explicit internal-phy path also performs extra PHY setup in
 * addition to RGMII pad/mux writes.
 */
static const struct bcm338x_mux_field gmac1_internal_fields[] = {
	BCM3383_MUX_MASKED_FIELD(BCM3383_PAD_CTRL_REG,
				 BCM3383_PAD_CTRL_GMAC1_INTERNAL_MASK,
				 BCM3383_PAD_CTRL_GMAC1_INTERNAL),
	BCM3383_MUX_MASKED_FIELD(BCM3383_PINMUX_MID_REG,
				 BCM3383_PINMUX_MID_GMAC1_INTERNAL_MASK,
				 BCM3383_PINMUX_MID_GMAC1_INTERNAL),
	BCM3383_MUX_MASKED_FIELD(BCM3383_PINMUX_HI1_REG,
				 BCM3383_PINMUX_HI1_GMAC1_INTERNAL_MASK,
				 BCM3383_PINMUX_HI1_GMAC1_INTERNAL),
};

#define BCM3383_PINMUX_FUN(n)				\
	{						\
		.name = #n,				\
		.groups = n##_groups,			\
		.num_groups = ARRAY_SIZE(n##_groups),	\
		.fields = n##_fields,			\
		.num_fields = ARRAY_SIZE(n##_fields),	\
	}

static const struct bcm338x_function bcm3383_funcs[] = {
	BCM3383_PINMUX_FUN(rgmii0),
	BCM3383_PINMUX_FUN(gmac1_internal),
	BCM3383_PINMUX_FUN(hsspi_cs2),
};

static const struct bcm338x_pinctrl_variant bcm3383_variant = {
	.pins = bcm3383_pins,
	.npins = ARRAY_SIZE(bcm3383_pins),
	.groups = bcm3383_groups,
	.ngroups = ARRAY_SIZE(bcm3383_groups),
	.functions = bcm3383_funcs,
	.nfunctions = ARRAY_SIZE(bcm3383_funcs),
	.gpio_compatible = "brcm,bcm3383-gpio",
};

static int bcm3383_pinctrl_probe(struct platform_device *pdev)
{
	return bcm338x_pinctrl_probe(pdev, &bcm3383_variant, NULL);
}

static const struct of_device_id bcm3383_pinctrl_match[] = {
	{ .compatible = "brcm,bcm3383-pinctrl", },
	{ /* sentinel */ }
};

static struct platform_driver bcm3383_pinctrl_driver = {
	.probe = bcm3383_pinctrl_probe,
	.driver = {
		.name = "bcm3383-pinctrl",
		.of_match_table = bcm3383_pinctrl_match,
	},
};

builtin_platform_driver(bcm3383_pinctrl_driver);
