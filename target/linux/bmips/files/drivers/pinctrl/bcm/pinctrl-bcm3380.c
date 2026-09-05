// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for BCM3380 GPIO unit (pinctrl + GPIO)
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 * Copyright (C) 2021 Álvaro Fernández Rojas <noltari@gmail.com>
 * Copyright (C) 2016 Jonas Gorski <jonas.gorski@gmail.com>
 */

#include <linux/bits.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include "pinctrl-bcm338x.h"

#define BCM3380_PINMUX_LO_REG		0x80
#define BCM3380_PINMUX_HI_REG		0x84
#define BCM3380_SPIMASTER_CTRL_REG	0x88

#define BCM3380_PINMUX_FIELD_MASK	GENMASK(1, 0)

// BCM3380 seems to have two SPIs, but the bootloader and the stock eCos only use the LSSPI.
// These two bits are used to select LSSPI and disable HSSPI.
// GpioSpimasterControl.SpimModeOverride (bit 19) = 1
#define BCM3380_SPIMASTER_MODE_OVERRIDE	BIT(19)
// GpioSpimasterControl.HsSpimEn (bit 16) = 0
#define BCM3380_SPIMASTER_HS_SPIM_EN	BIT(16)

/*
 * The GPL GpioPinMuxSel bitfield declarations are laid out for the MIPS
 * big-endian Reg32 view. Describe the hardware as raw Reg32 shifts so the
 * driver does not depend on C bitfield ordering.
 */
enum bcm3380_pinmux_lo_shift {
	BCM3380_PINMUX_LO_GPIO0500_SHIFT = 0,
	BCM3380_PINMUX_LO_GPIO1106_SHIFT = 2,
	BCM3380_PINMUX_LO_GPIO1312_SHIFT = 4,
	BCM3380_PINMUX_LO_GPIO1514_SHIFT = 6,
	BCM3380_PINMUX_LO_GPIO1716_SHIFT = 8,
	BCM3380_PINMUX_LO_GPIO1918_SHIFT = 10,
	BCM3380_PINMUX_LO_GPIO2320_SHIFT = 12,
	BCM3380_PINMUX_LO_GPIO2924_SHIFT = 14,
	BCM3380_PINMUX_LO_GPIO3130_SHIFT = 16,
	BCM3380_PINMUX_LO_GPIO3532_SHIFT = 18,
	BCM3380_PINMUX_LO_GMII_RXD_SHIFT = 22,
	BCM3380_PINMUX_LO_GMII_RX_SHIFT = 24,
	BCM3380_PINMUX_LO_GMII_TX_SHIFT = 26,
	BCM3380_PINMUX_LO_GMII_TXD_SHIFT = 28,
	BCM3380_PINMUX_LO_MDIO_SHIFT = 30,
};

enum bcm3380_pinmux_hi_shift {
	BCM3380_PINMUX_HI_TP_MISC_SHIFT = 0,
	BCM3380_PINMUX_HI_BMU_SHIFT = 2,
	BCM3380_PINMUX_HI_HVG_SHIFT = 4,
	BCM3380_PINMUX_HI_PASS_SHIFT = 6,
	BCM3380_PINMUX_HI_AGCI0300_SHIFT = 8,
	BCM3380_PINMUX_HI_AGCI0704_SHIFT = 10,
	BCM3380_PINMUX_HI_SPIS_UART1_SHIFT = 12,
	BCM3380_PINMUX_HI_LED0700_SHIFT = 14,
	BCM3380_PINMUX_HI_GMII_CLK_SHIFT = 16,
	BCM3380_PINMUX_HI_USB0_SHIFT = 18,
	BCM3380_PINMUX_HI_USB1_SHIFT = 20,
};

#define BCM3380_MUX_FIELD(_reg, _shift, _value)		\
	{						\
		.reg = (_reg),				\
		.mask = BCM3380_PINMUX_FIELD_MASK << (_shift), \
		.value = (_value) << (_shift),		\
	}

static const struct pinctrl_pin_desc bcm3380_pins[] = {
	PINCTRL_PIN(0, "gpio0"),
	PINCTRL_PIN(1, "gpio1"),
	PINCTRL_PIN(2, "gpio2"),
	PINCTRL_PIN(3, "gpio3"),
	PINCTRL_PIN(4, "gpio4"),
	PINCTRL_PIN(5, "gpio5"),
	PINCTRL_PIN(6, "gpio6"),
	PINCTRL_PIN(7, "gpio7"),
	PINCTRL_PIN(8, "gpio8"),
	PINCTRL_PIN(9, "gpio9"),
	PINCTRL_PIN(10, "gpio10"),
	PINCTRL_PIN(11, "gpio11"),
	PINCTRL_PIN(12, "gpio12"),
	PINCTRL_PIN(13, "gpio13"),
	PINCTRL_PIN(14, "gpio14"),
	PINCTRL_PIN(15, "gpio15"),
	PINCTRL_PIN(16, "gpio16"),
	PINCTRL_PIN(17, "gpio17"),
	PINCTRL_PIN(18, "gpio18"),
	PINCTRL_PIN(19, "gpio19"),
	PINCTRL_PIN(20, "gpio20"),
	PINCTRL_PIN(21, "gpio21"),
	PINCTRL_PIN(22, "gpio22"),
	PINCTRL_PIN(23, "gpio23"),
	PINCTRL_PIN(24, "gpio24"),
	PINCTRL_PIN(25, "gpio25"),
	PINCTRL_PIN(26, "gpio26"),
	PINCTRL_PIN(27, "gpio27"),
	PINCTRL_PIN(28, "gpio28"),
	PINCTRL_PIN(29, "gpio29"),
	PINCTRL_PIN(30, "gpio30"),
	PINCTRL_PIN(31, "gpio31"),
	PINCTRL_PIN(32, "gpio32"),
	PINCTRL_PIN(33, "gpio33"),
	PINCTRL_PIN(34, "gpio34"),
	PINCTRL_PIN(35, "gpio35"),
	PINCTRL_PIN(36, "gpio36"),
	PINCTRL_PIN(37, "gpio37"),
	PINCTRL_PIN(38, "gpio38"),
	PINCTRL_PIN(39, "gpio39"),
	PINCTRL_PIN(40, "gpio40"),
	PINCTRL_PIN(41, "gpio41"),
	PINCTRL_PIN(42, "gpio42"),
	PINCTRL_PIN(43, "gpio43"),
	PINCTRL_PIN(44, "gpio44"),
	PINCTRL_PIN(45, "gpio45"),
	PINCTRL_PIN(46, "gpio46"),
	PINCTRL_PIN(47, "gpio47"),
	PINCTRL_PIN(48, "gpio48"),
	PINCTRL_PIN(49, "gpio49"),
	PINCTRL_PIN(50, "gpio50"),
	PINCTRL_PIN(51, "gpio51"),
};

static unsigned gpio0_pins[] = { 0 };
static unsigned gpio1_pins[] = { 1 };
static unsigned gpio2_pins[] = { 2 };
static unsigned gpio3_pins[] = { 3 };
static unsigned gpio4_pins[] = { 4 };
static unsigned gpio5_pins[] = { 5 };
static unsigned gpio6_pins[] = { 6 };
static unsigned gpio7_pins[] = { 7 };
static unsigned gpio8_pins[] = { 8 };
static unsigned gpio9_pins[] = { 9 };
static unsigned gpio10_pins[] = { 10 };
static unsigned gpio11_pins[] = { 11 };
static unsigned gpio12_pins[] = { 12 };
static unsigned gpio13_pins[] = { 13 };
static unsigned gpio14_pins[] = { 14 };
static unsigned gpio15_pins[] = { 15 };
static unsigned gpio16_pins[] = { 16 };
static unsigned gpio17_pins[] = { 17 };
static unsigned gpio18_pins[] = { 18 };
static unsigned gpio19_pins[] = { 19 };
static unsigned gpio20_pins[] = { 20 };
static unsigned gpio21_pins[] = { 21 };
static unsigned gpio22_pins[] = { 22 };
static unsigned gpio23_pins[] = { 23 };
static unsigned gpio24_pins[] = { 24 };
static unsigned gpio25_pins[] = { 25 };
static unsigned gpio26_pins[] = { 26 };
static unsigned gpio27_pins[] = { 27 };
static unsigned gpio28_pins[] = { 28 };
static unsigned gpio29_pins[] = { 29 };
static unsigned gpio30_pins[] = { 30 };
static unsigned gpio31_pins[] = { 31 };
static unsigned gpio32_pins[] = { 32 };
static unsigned gpio33_pins[] = { 33 };
static unsigned gpio34_pins[] = { 34 };
static unsigned gpio35_pins[] = { 35 };
static unsigned gpio36_pins[] = { 36 };
static unsigned gpio37_pins[] = { 37 };
static unsigned gpio38_pins[] = { 38 };
static unsigned gpio39_pins[] = { 39 };
static unsigned gpio40_pins[] = { 40 };
static unsigned gpio41_pins[] = { 41 };
static unsigned gpio42_pins[] = { 42 };
static unsigned gpio43_pins[] = { 43 };
static unsigned gpio44_pins[] = { 44 };
static unsigned gpio45_pins[] = { 45 };
static unsigned gpio46_pins[] = { 46 };
static unsigned gpio47_pins[] = { 47 };
static unsigned gpio48_pins[] = { 48 };
static unsigned gpio49_pins[] = { 49 };
static unsigned gpio50_pins[] = { 50 };
static unsigned gpio51_pins[] = { 51 };

static const struct pingroup bcm3380_groups[] = {
	BCM338X_PIN_GROUP(gpio0),
	BCM338X_PIN_GROUP(gpio1),
	BCM338X_PIN_GROUP(gpio2),
	BCM338X_PIN_GROUP(gpio3),
	BCM338X_PIN_GROUP(gpio4),
	BCM338X_PIN_GROUP(gpio5),
	BCM338X_PIN_GROUP(gpio6),
	BCM338X_PIN_GROUP(gpio7),
	BCM338X_PIN_GROUP(gpio8),
	BCM338X_PIN_GROUP(gpio9),
	BCM338X_PIN_GROUP(gpio10),
	BCM338X_PIN_GROUP(gpio11),
	BCM338X_PIN_GROUP(gpio12),
	BCM338X_PIN_GROUP(gpio13),
	BCM338X_PIN_GROUP(gpio14),
	BCM338X_PIN_GROUP(gpio15),
	BCM338X_PIN_GROUP(gpio16),
	BCM338X_PIN_GROUP(gpio17),
	BCM338X_PIN_GROUP(gpio18),
	BCM338X_PIN_GROUP(gpio19),
	BCM338X_PIN_GROUP(gpio20),
	BCM338X_PIN_GROUP(gpio21),
	BCM338X_PIN_GROUP(gpio22),
	BCM338X_PIN_GROUP(gpio23),
	BCM338X_PIN_GROUP(gpio24),
	BCM338X_PIN_GROUP(gpio25),
	BCM338X_PIN_GROUP(gpio26),
	BCM338X_PIN_GROUP(gpio27),
	BCM338X_PIN_GROUP(gpio28),
	BCM338X_PIN_GROUP(gpio29),
	BCM338X_PIN_GROUP(gpio30),
	BCM338X_PIN_GROUP(gpio31),
	BCM338X_PIN_GROUP(gpio32),
	BCM338X_PIN_GROUP(gpio33),
	BCM338X_PIN_GROUP(gpio34),
	BCM338X_PIN_GROUP(gpio35),
	BCM338X_PIN_GROUP(gpio36),
	BCM338X_PIN_GROUP(gpio37),
	BCM338X_PIN_GROUP(gpio38),
	BCM338X_PIN_GROUP(gpio39),
	BCM338X_PIN_GROUP(gpio40),
	BCM338X_PIN_GROUP(gpio41),
	BCM338X_PIN_GROUP(gpio42),
	BCM338X_PIN_GROUP(gpio43),
	BCM338X_PIN_GROUP(gpio44),
	BCM338X_PIN_GROUP(gpio45),
	BCM338X_PIN_GROUP(gpio46),
	BCM338X_PIN_GROUP(gpio47),
	BCM338X_PIN_GROUP(gpio48),
	BCM338X_PIN_GROUP(gpio49),
	BCM338X_PIN_GROUP(gpio50),
	BCM338X_PIN_GROUP(gpio51),
};

static const char * const lsspi_cs3_groups[] = {
	"gpio15",
};

static const struct bcm338x_mux_field lsspi_cs3_fields[] = {
	BCM3380_MUX_FIELD(BCM3380_PINMUX_LO_REG,
			  BCM3380_PINMUX_LO_GPIO1514_SHIFT, 2),
};

static const char * const gpio1514_alt2_groups[] = {
	"gpio14",
	"gpio15",
};

static const struct bcm338x_mux_field gpio1514_alt2_fields[] = {
	BCM3380_MUX_FIELD(BCM3380_PINMUX_LO_REG,
			  BCM3380_PINMUX_LO_GPIO1514_SHIFT, 2),
};

static const char * const gpio2320_alt1_groups[] = {
	"gpio20",
	"gpio21",
	"gpio22",
	"gpio23",
};

static const struct bcm338x_mux_field gpio2320_alt1_fields[] = {
	BCM3380_MUX_FIELD(BCM3380_PINMUX_LO_REG,
			  BCM3380_PINMUX_LO_GPIO2320_SHIFT, 1),
};

static const char * const gpio2924_alt1_groups[] = {
	"gpio24",
	"gpio25",
	"gpio26",
	"gpio27",
	"gpio28",
	"gpio29",
};

static const struct bcm338x_mux_field gpio2924_alt1_fields[] = {
	BCM3380_MUX_FIELD(BCM3380_PINMUX_LO_REG,
			  BCM3380_PINMUX_LO_GPIO2924_SHIFT, 1),
};

static const char * const agci0300_alt1_groups[] = {
	"gpio0",
	"gpio1",
	"gpio2",
	"gpio3",
};

static const struct bcm338x_mux_field agci0300_alt1_fields[] = {
	BCM3380_MUX_FIELD(BCM3380_PINMUX_HI_REG,
			  BCM3380_PINMUX_HI_AGCI0300_SHIFT, 1),
};

static const char * const agci0704_alt1_groups[] = {
	"gpio4",
	"gpio5",
	"gpio6",
	"gpio7",
};

static const struct bcm338x_mux_field agci0704_alt1_fields[] = {
	BCM3380_MUX_FIELD(BCM3380_PINMUX_HI_REG,
			  BCM3380_PINMUX_HI_AGCI0704_SHIFT, 1),
};

#define BCM3380_PINMUX_FUN(n)				\
	{						\
		.name = #n,				\
		.groups = n##_groups,			\
		.num_groups = ARRAY_SIZE(n##_groups),	\
		.fields = n##_fields,			\
		.num_fields = ARRAY_SIZE(n##_fields),	\
	}

static const struct bcm338x_function bcm3380_funcs[] = {
	BCM3380_PINMUX_FUN(lsspi_cs3),
	BCM3380_PINMUX_FUN(gpio1514_alt2),
	BCM3380_PINMUX_FUN(gpio2320_alt1),
	BCM3380_PINMUX_FUN(gpio2924_alt1),
	BCM3380_PINMUX_FUN(agci0300_alt1),
	BCM3380_PINMUX_FUN(agci0704_alt1),
};

static int bcm3380_set_gpio(struct bcm338x_pinctrl *pc, unsigned int pin)
{
	unsigned int mux_reg = 0;
	unsigned int mux_shift = 0;
	int ret;

	switch (pin) {
	case 0 ... 5:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO0500_SHIFT;
		break;
	case 6 ... 11:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO1106_SHIFT;
		break;
	case 12 ... 13:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO1312_SHIFT;
		break;
	case 14 ... 15:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO1514_SHIFT;
		break;
	case 16 ... 17:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO1716_SHIFT;
		break;
	case 18 ... 19:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO1918_SHIFT;
		break;
	case 20 ... 23:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO2320_SHIFT;
		break;
	case 24 ... 29:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO2924_SHIFT;
		break;
	case 30 ... 31:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO3130_SHIFT;
		break;
	case 32 ... 35:
		mux_reg = BCM3380_PINMUX_LO_REG;
		mux_shift = BCM3380_PINMUX_LO_GPIO3532_SHIFT;
		break;
	}

	if (mux_reg) {
		ret = regmap_update_bits(pc->regs, mux_reg,
					 BCM3380_PINMUX_FIELD_MASK << mux_shift,
					 0);
		if (ret)
			return ret;
	}

	if (pin <= 3) {
		mux_shift = BCM3380_PINMUX_HI_AGCI0300_SHIFT;
		ret = regmap_update_bits(pc->regs, BCM3380_PINMUX_HI_REG,
					 BCM3380_PINMUX_FIELD_MASK << mux_shift,
					 0);
		if (ret)
			return ret;
	}

	if (pin >= 4 && pin <= 7) {
		mux_shift = BCM3380_PINMUX_HI_AGCI0704_SHIFT;
		ret = regmap_update_bits(pc->regs, BCM3380_PINMUX_HI_REG,
					 BCM3380_PINMUX_FIELD_MASK << mux_shift,
					 0);
		if (ret)
			return ret;
	}

	if (pin <= 7) {
		mux_shift = BCM3380_PINMUX_HI_LED0700_SHIFT;
		return regmap_update_bits(pc->regs, BCM3380_PINMUX_HI_REG,
					  BCM3380_PINMUX_FIELD_MASK << mux_shift,
					  0);
	}

	return 0;
}

static const struct bcm338x_pinctrl_variant bcm3380_variant = {
	.pins = bcm3380_pins,
	.npins = ARRAY_SIZE(bcm3380_pins),
	.groups = bcm3380_groups,
	.ngroups = ARRAY_SIZE(bcm3380_groups),
	.functions = bcm3380_funcs,
	.nfunctions = ARRAY_SIZE(bcm3380_funcs),
	.gpio_compatible = "brcm,bcm3380-gpio",
	.set_gpio = bcm3380_set_gpio,
};

static int bcm3380_pinctrl_probe(struct platform_device *pdev)
{
	u32 use_lsspi;
	int ret;

	ret = bcm338x_pinctrl_probe(pdev, &bcm3380_variant, NULL);
	if (ret)
		return ret;

	ret = of_property_read_u32(pdev->dev.of_node, "brcm,use-lsspi",
				   &use_lsspi);
	if (ret)
		return ret == -EINVAL ? 0 : ret;

	if (!use_lsspi)
		return 0;

	struct bcm338x_pinctrl *pc = platform_get_drvdata(pdev);

	return regmap_update_bits(pc->regs, BCM3380_SPIMASTER_CTRL_REG,
				  BCM3380_SPIMASTER_MODE_OVERRIDE |
				  BCM3380_SPIMASTER_HS_SPIM_EN,
				  BCM3380_SPIMASTER_MODE_OVERRIDE);
}

static const struct of_device_id bcm3380_pinctrl_match[] = {
	{ .compatible = "brcm,bcm3380-pinctrl", },
	{ /* sentinel */ }
};

static struct platform_driver bcm3380_pinctrl_driver = {
	.probe = bcm3380_pinctrl_probe,
	.driver = {
		.name = "bcm3380-pinctrl",
		.of_match_table = bcm3380_pinctrl_match,
	},
};

builtin_platform_driver(bcm3380_pinctrl_driver);
