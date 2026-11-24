// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitops.h>
#include <linux/gpio/driver.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>

#include "pinctrl-utils.h"

#define DVF99_GPIO_DATA		0x00
#define DVF99_GPIO_DATA_SET	0x04
#define DVF99_GPIO_DATA_CLR	0x08
#define DVF99_GPIO_DIR		0x0c
#define DVF99_GPIO_DIR_OUT	0x10
#define DVF99_GPIO_DIR_IN	0x14
#define DVF99_GPIO_EN		0x18
#define DVF99_GPIO_EN_SET	0x1c
#define DVF99_GPIO_EN_CLR	0x20
#define DVF99_GPIO_PULL_CTRL	0x24
#define DVF99_GPIO_PULL_EN	0x28
#define DVF99_GPIO_PULL_DIS	0x2c
#define DVF99_GPIO_PULL_TYPE	0x30
#define DVF99_GPIO_PULL_UP	0x34
#define DVF99_GPIO_PULL_DOWN	0x38
#define DVF99_GPIO_KEEP		0x3c
#define DVF99_GPIO_KEEP_EN	0x40
#define DVF99_GPIO_KEEP_DIS	0x44
#define DVF99_GPIO_OD		0x48
#define DVF99_GPIO_OD_SET	0x4c
#define DVF99_GPIO_OD_CLR	0x50

#define DVF99_GPIO_BANK_STRIDE	0x60
#define DVF99_GPIO_BANKS	5
#define DVF99_GPIO_PER_BANK	32
#define DVF99_GPIO_NUM		(DVF99_GPIO_BANKS * DVF99_GPIO_PER_BANK)

#define DVF99_EXTINT_CFG	0x2a0
#define DVF99_EXTINT_EDGE_CLR	0x2ac
#define DVF99_EXTINT_POL_HIGH	BIT(0)
#define DVF99_EXTINT_MODE_EDGE	BIT(1)
#define DVF99_EXTINT_CFG_MASK	GENMASK(1, 0)

#define DVF99_SYSCFG_SPCR1	0x54
#define DVF99_SYSCFG_SPCR2	0x58

#define DVF99_PAD_NONE		-1

#define PIN_A(n)		(0 * DVF99_GPIO_PER_BANK + (n))
#define PIN_B(n)		(1 * DVF99_GPIO_PER_BANK + (n))
#define PIN_C(n)		(2 * DVF99_GPIO_PER_BANK + (n))
#define PIN_D(n)		(3 * DVF99_GPIO_PER_BANK + (n))
#define PIN_E(n)		(4 * DVF99_GPIO_PER_BANK + (n))

struct dvf99_group {
	struct pingroup grp;
	int pad_bit;
	int loc_bit;
	bool loc;
};

struct dvf99_extint {
	unsigned int pin;
	unsigned int parent_hwirq;
};

struct dvf99_pinctrl {
	struct device *dev;
	void __iomem *base;
	struct regmap *syscfg;
	struct pinctrl_dev *pctldev;
	struct pinctrl_desc desc;
	struct gpio_chip gc;
	raw_spinlock_t lock;
	DECLARE_BITMAP(both_edges, DVF99_GPIO_NUM);
};

static const u32 dvf99_invalid_pins[DVF99_GPIO_BANKS] = {
	0xc3f0c000, 0x780080f0, 0x00000000, 0x38000000, 0x00000000,
};

static const struct dvf99_extint dvf99_extints[] = {
	{ PIN_A(16), 24 }, { PIN_A(17), 25 }, { PIN_A(18), 26 },
	{ PIN_A(19), 27 }, { PIN_B(26), 28 }, { PIN_C(28), 29 },
	{ PIN_D(8), 30 }, { PIN_D(9), 32 }, { PIN_D(26), 33 },
	{ PIN_D(24), 34 }, { PIN_D(25), 35 }, { PIN_B(31), 36 },
	{ PIN_E(6), 37 }, { PIN_D(30), 38 }, { PIN_D(31), 39 },
	{ PIN_B(11), 40 },
};

static const unsigned int dvf99_uart1_pins[] = { PIN_D(20), PIN_D(21) };
static const unsigned int dvf99_uart2_pins[] = { PIN_E(16), PIN_E(17) };
static const unsigned int dvf99_uart2_alt_pins[] = { PIN_A(28), PIN_A(29) };
static const unsigned int dvf99_spi1_pins[] = { PIN_B(8), PIN_B(9), PIN_B(12) };
static const unsigned int dvf99_spi1_cs0_pins[] = { PIN_B(10) };
static const unsigned int dvf99_spi2_pins[] = { PIN_B(13), PIN_B(14), PIN_B(16) };
static const unsigned int dvf99_i2c1_pins[] = { PIN_A(16), PIN_A(17) };
static const unsigned int dvf99_i2c2_pins[] = { PIN_E(30), PIN_E(31) };
static const unsigned int dvf99_nand_pins[] = {
	PIN_A(0), PIN_A(1), PIN_A(2), PIN_A(3), PIN_A(4), PIN_A(5), PIN_A(6),
	PIN_A(7), PIN_A(8), PIN_A(9), PIN_A(10), PIN_A(11), PIN_A(12), PIN_A(13),
};
static const unsigned int dvf99_rgmii_pins[] = {
	PIN_E(0), PIN_E(1), PIN_E(2), PIN_E(3), PIN_E(4), PIN_E(5), PIN_E(7),
	PIN_E(11), PIN_E(12), PIN_E(13), PIN_E(14), PIN_E(15),
};
static const unsigned int dvf99_mdio_pins[] = { PIN_E(8), PIN_E(9) };
static const unsigned int dvf99_lcd_pins[] = {
	PIN_D(1), PIN_D(2), PIN_D(3), PIN_D(4), PIN_D(5), PIN_D(6), PIN_D(7),
	PIN_C(0), PIN_C(1), PIN_C(2), PIN_C(3), PIN_C(4), PIN_C(5), PIN_C(6),
	PIN_C(7), PIN_C(8), PIN_C(9), PIN_C(10), PIN_C(11), PIN_C(12),
	PIN_C(13), PIN_C(14), PIN_C(15), PIN_C(16), PIN_C(17),
};
static const unsigned int dvf99_lcdgp0_pins[] = { PIN_C(19) };
static const unsigned int dvf99_lcdgp1_pins[] = { PIN_C(20) };
static const unsigned int dvf99_lcdgp2_pins[] = { PIN_C(21) };
static const unsigned int dvf99_lcdgp3_pins[] = { PIN_C(22) };
static const unsigned int dvf99_keypad_pins[] = {
	PIN_E(18), PIN_E(19), PIN_E(20), PIN_E(21), PIN_E(22), PIN_E(23),
	PIN_E(24), PIN_E(25), PIN_E(26), PIN_E(27), PIN_E(28),
};
static const unsigned int dvf99_keypad_row5_pins[] = { PIN_E(29) };
static const unsigned int dvf99_keypad_col67_pins[] = { PIN_A(26), PIN_A(27) };
static const unsigned int dvf99_usb_vbus_pins[] = { PIN_D(26) };

#define DVF99_GROUP(_name, _pins, _pad)					\
	{								\
		.grp = PINCTRL_PINGROUP(_name, _pins, ARRAY_SIZE(_pins)), \
		.pad_bit = _pad,					\
		.loc_bit = DVF99_PAD_NONE,				\
	}

#define DVF99_GROUP_LOC(_name, _pins, _pad, _loc_bit, _loc)		\
	{								\
		.grp = PINCTRL_PINGROUP(_name, _pins, ARRAY_SIZE(_pins)), \
		.pad_bit = _pad,					\
		.loc_bit = _loc_bit,					\
		.loc = _loc,						\
	}

static const struct dvf99_group dvf99_groups[] = {
	DVF99_GROUP("uart1", dvf99_uart1_pins, 26),
	DVF99_GROUP_LOC("uart2", dvf99_uart2_pins, 25, 24, false),
	DVF99_GROUP_LOC("uart2_alt", dvf99_uart2_alt_pins, 25, 24, true),
	DVF99_GROUP("spi1", dvf99_spi1_pins, 20),
	DVF99_GROUP("spi1_cs0", dvf99_spi1_cs0_pins, 20),
	DVF99_GROUP("spi2", dvf99_spi2_pins, 19),
	DVF99_GROUP("i2c1", dvf99_i2c1_pins, 17),
	DVF99_GROUP("i2c2", dvf99_i2c2_pins, 16),
	DVF99_GROUP("nand", dvf99_nand_pins, 27),
	DVF99_GROUP("rgmii", dvf99_rgmii_pins, 14),
	DVF99_GROUP("mdio", dvf99_mdio_pins, 14),
	DVF99_GROUP("lcd", dvf99_lcd_pins, 10),
	DVF99_GROUP("lcdgp0", dvf99_lcdgp0_pins, 0),
	DVF99_GROUP("lcdgp1", dvf99_lcdgp1_pins, 1),
	DVF99_GROUP("lcdgp2", dvf99_lcdgp2_pins, 2),
	DVF99_GROUP("lcdgp3", dvf99_lcdgp3_pins, 3),
	DVF99_GROUP("keypad", dvf99_keypad_pins, DVF99_PAD_NONE),
	DVF99_GROUP("keypad_row5", dvf99_keypad_row5_pins, DVF99_PAD_NONE),
	DVF99_GROUP("keypad_col67", dvf99_keypad_col67_pins, DVF99_PAD_NONE),
	DVF99_GROUP("usb_vbus", dvf99_usb_vbus_pins, 32 + 5),
};

static const char * const dvf99_uart1_groups[] = { "uart1" };
static const char * const dvf99_uart2_groups[] = { "uart2", "uart2_alt" };
static const char * const dvf99_spi1_groups[] = { "spi1", "spi1_cs0" };
static const char * const dvf99_spi2_groups[] = { "spi2" };
static const char * const dvf99_i2c1_groups[] = { "i2c1" };
static const char * const dvf99_i2c2_groups[] = { "i2c2" };
static const char * const dvf99_nand_groups[] = { "nand" };
static const char * const dvf99_eth_groups[] = { "rgmii", "mdio" };
static const char * const dvf99_lcd_groups[] = {
	"lcd", "lcdgp0", "lcdgp1", "lcdgp2", "lcdgp3",
};
static const char * const dvf99_keypad_groups[] = {
	"keypad", "keypad_row5", "keypad_col67",
};
static const char * const dvf99_usb_groups[] = { "usb_vbus" };
static const char * const dvf99_gpio_groups[] = { "i2c1", "i2c2" };

#define DVF99_FUNCTION(_name, _groups)					\
	PINCTRL_PINFUNCTION(_name, _groups, ARRAY_SIZE(_groups))

static const struct pinfunction dvf99_functions[] = {
	DVF99_FUNCTION("uart1", dvf99_uart1_groups),
	DVF99_FUNCTION("uart2", dvf99_uart2_groups),
	DVF99_FUNCTION("spi1", dvf99_spi1_groups),
	DVF99_FUNCTION("spi2", dvf99_spi2_groups),
	DVF99_FUNCTION("i2c1", dvf99_i2c1_groups),
	DVF99_FUNCTION("i2c2", dvf99_i2c2_groups),
	DVF99_FUNCTION("nand", dvf99_nand_groups),
	DVF99_FUNCTION("eth", dvf99_eth_groups),
	DVF99_FUNCTION("lcd", dvf99_lcd_groups),
	DVF99_FUNCTION("keypad", dvf99_keypad_groups),
	DVF99_FUNCTION("usb", dvf99_usb_groups),
	DVF99_FUNCTION("gpio", dvf99_gpio_groups),
};

static void __iomem *dvf99_pin_reg(struct dvf99_pinctrl *pctl,
				   unsigned int pin, unsigned int reg)
{
	return pctl->base + (pin / DVF99_GPIO_PER_BANK) * DVF99_GPIO_BANK_STRIDE +
	       reg;
}

static u32 dvf99_pin_bit(unsigned int pin)
{
	return BIT(pin % DVF99_GPIO_PER_BANK);
}

static bool dvf99_pin_test(struct dvf99_pinctrl *pctl, unsigned int pin,
			   unsigned int reg)
{
	return readl(dvf99_pin_reg(pctl, pin, reg)) & dvf99_pin_bit(pin);
}

static void dvf99_pin_write(struct dvf99_pinctrl *pctl, unsigned int pin,
			    unsigned int reg)
{
	writel(dvf99_pin_bit(pin), dvf99_pin_reg(pctl, pin, reg));
}

static bool dvf99_pin_valid(unsigned int pin)
{
	return !(dvf99_invalid_pins[pin / DVF99_GPIO_PER_BANK] &
		 dvf99_pin_bit(pin));
}

static int dvf99_syscfg_bit_set(struct dvf99_pinctrl *pctl, int bit, bool on)
{
	unsigned int reg = bit < 32 ? DVF99_SYSCFG_SPCR1 : DVF99_SYSCFG_SPCR2;
	u32 mask = BIT(bit % 32);

	return regmap_update_bits(pctl->syscfg, reg, mask, on ? mask : 0);
}

static int dvf99_get_groups_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(dvf99_groups);
}

static const char *dvf99_get_group_name(struct pinctrl_dev *pctldev,
					unsigned int selector)
{
	return dvf99_groups[selector].grp.name;
}

static int dvf99_get_group_pins(struct pinctrl_dev *pctldev,
				unsigned int selector,
				const unsigned int **pins,
				unsigned int *npins)
{
	*pins = dvf99_groups[selector].grp.pins;
	*npins = dvf99_groups[selector].grp.npins;

	return 0;
}

static void dvf99_pin_dbg_show(struct pinctrl_dev *pctldev,
			       struct seq_file *s, unsigned int pin)
{
	struct dvf99_pinctrl *pctl = pinctrl_dev_get_drvdata(pctldev);

	seq_printf(s, "%s", dvf99_pin_test(pctl, pin, DVF99_GPIO_EN) ?
		   "gpio" : "function");
}

static const struct pinctrl_ops dvf99_pinctrl_ops = {
	.get_groups_count	= dvf99_get_groups_count,
	.get_group_name		= dvf99_get_group_name,
	.get_group_pins		= dvf99_get_group_pins,
	.pin_dbg_show		= dvf99_pin_dbg_show,
	.dt_node_to_map		= pinconf_generic_dt_node_to_map_all,
	.dt_free_map		= pinctrl_utils_free_map,
};

static int dvf99_get_functions_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(dvf99_functions);
}

static const char *dvf99_get_function_name(struct pinctrl_dev *pctldev,
					   unsigned int selector)
{
	return dvf99_functions[selector].name;
}

static int dvf99_get_function_groups(struct pinctrl_dev *pctldev,
				     unsigned int selector,
				     const char * const **groups,
				     unsigned int * const ngroups)
{
	*groups = dvf99_functions[selector].groups;
	*ngroups = dvf99_functions[selector].ngroups;

	return 0;
}

static int dvf99_set_mux(struct pinctrl_dev *pctldev, unsigned int func,
			 unsigned int group)
{
	struct dvf99_pinctrl *pctl = pinctrl_dev_get_drvdata(pctldev);
	const struct dvf99_group *g = &dvf99_groups[group];
	unsigned int i;
	int ret;

	if (dvf99_functions[func].groups == dvf99_gpio_groups) {
		for (i = 0; i < g->grp.npins; i++)
			dvf99_pin_write(pctl, g->grp.pins[i], DVF99_GPIO_EN_SET);

		return 0;
	}

	for (i = 0; i < g->grp.npins; i++)
		dvf99_pin_write(pctl, g->grp.pins[i], DVF99_GPIO_EN_CLR);

	if (g->loc_bit != DVF99_PAD_NONE) {
		ret = dvf99_syscfg_bit_set(pctl, g->loc_bit, g->loc);
		if (ret)
			return ret;
	}

	if (g->pad_bit == DVF99_PAD_NONE)
		return 0;

	return dvf99_syscfg_bit_set(pctl, g->pad_bit, true);
}

static int dvf99_gpio_request_enable(struct pinctrl_dev *pctldev,
				     struct pinctrl_gpio_range *range,
				     unsigned int pin)
{
	struct dvf99_pinctrl *pctl = pinctrl_dev_get_drvdata(pctldev);

	dvf99_pin_write(pctl, pin, DVF99_GPIO_EN_SET);

	return 0;
}

static const struct pinmux_ops dvf99_pinmux_ops = {
	.get_functions_count	= dvf99_get_functions_count,
	.get_function_name	= dvf99_get_function_name,
	.get_function_groups	= dvf99_get_function_groups,
	.set_mux		= dvf99_set_mux,
	.gpio_request_enable	= dvf99_gpio_request_enable,
};

static int dvf99_pinconf_get(struct pinctrl_dev *pctldev, unsigned int pin,
			     unsigned long *config)
{
	struct dvf99_pinctrl *pctl = pinctrl_dev_get_drvdata(pctldev);
	enum pin_config_param param = pinconf_to_config_param(*config);
	bool pull = dvf99_pin_test(pctl, pin, DVF99_GPIO_PULL_CTRL);
	bool up = dvf99_pin_test(pctl, pin, DVF99_GPIO_PULL_TYPE);
	bool active;

	switch (param) {
	case PIN_CONFIG_BIAS_DISABLE:
		active = !pull;
		break;
	case PIN_CONFIG_BIAS_PULL_UP:
		active = pull && up;
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		active = pull && !up;
		break;
	case PIN_CONFIG_BIAS_BUS_HOLD:
		active = dvf99_pin_test(pctl, pin, DVF99_GPIO_KEEP);
		break;
	case PIN_CONFIG_DRIVE_OPEN_DRAIN:
		active = dvf99_pin_test(pctl, pin, DVF99_GPIO_OD);
		break;
	case PIN_CONFIG_DRIVE_PUSH_PULL:
		active = !dvf99_pin_test(pctl, pin, DVF99_GPIO_OD);
		break;
	default:
		return -ENOTSUPP;
	}

	if (!active)
		return -EINVAL;

	*config = pinconf_to_config_packed(param, 1);

	return 0;
}

static int dvf99_pinconf_set_one(struct dvf99_pinctrl *pctl, unsigned int pin,
				 enum pin_config_param param)
{
	switch (param) {
	case PIN_CONFIG_BIAS_DISABLE:
		dvf99_pin_write(pctl, pin, DVF99_GPIO_PULL_DIS);
		dvf99_pin_write(pctl, pin, DVF99_GPIO_KEEP_DIS);
		break;
	case PIN_CONFIG_BIAS_PULL_UP:
		dvf99_pin_write(pctl, pin, DVF99_GPIO_PULL_UP);
		dvf99_pin_write(pctl, pin, DVF99_GPIO_PULL_EN);
		break;
	case PIN_CONFIG_BIAS_PULL_DOWN:
		dvf99_pin_write(pctl, pin, DVF99_GPIO_PULL_DOWN);
		dvf99_pin_write(pctl, pin, DVF99_GPIO_PULL_EN);
		break;
	case PIN_CONFIG_BIAS_BUS_HOLD:
		dvf99_pin_write(pctl, pin, DVF99_GPIO_KEEP_EN);
		break;
	case PIN_CONFIG_DRIVE_OPEN_DRAIN:
		dvf99_pin_write(pctl, pin, DVF99_GPIO_OD_SET);
		break;
	case PIN_CONFIG_DRIVE_PUSH_PULL:
		dvf99_pin_write(pctl, pin, DVF99_GPIO_OD_CLR);
		break;
	default:
		return -ENOTSUPP;
	}

	return 0;
}

static int dvf99_pinconf_set(struct pinctrl_dev *pctldev, unsigned int pin,
			     unsigned long *configs, unsigned int num_configs)
{
	struct dvf99_pinctrl *pctl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int i;
	int ret;

	for (i = 0; i < num_configs; i++) {
		ret = dvf99_pinconf_set_one(pctl, pin,
					    pinconf_to_config_param(configs[i]));
		if (ret)
			return ret;
	}

	return 0;
}

static int dvf99_pinconf_group_get(struct pinctrl_dev *pctldev,
				   unsigned int group, unsigned long *config)
{
	return dvf99_pinconf_get(pctldev, dvf99_groups[group].grp.pins[0],
				 config);
}

static int dvf99_pinconf_group_set(struct pinctrl_dev *pctldev,
				   unsigned int group, unsigned long *configs,
				   unsigned int num_configs)
{
	const struct dvf99_group *g = &dvf99_groups[group];
	unsigned int i;
	int ret;

	for (i = 0; i < g->grp.npins; i++) {
		ret = dvf99_pinconf_set(pctldev, g->grp.pins[i], configs,
					num_configs);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct pinconf_ops dvf99_pinconf_ops = {
	.is_generic		= true,
	.pin_config_get		= dvf99_pinconf_get,
	.pin_config_set		= dvf99_pinconf_set,
	.pin_config_group_get	= dvf99_pinconf_group_get,
	.pin_config_group_set	= dvf99_pinconf_group_set,
};

static int dvf99_gpio_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);

	if (dvf99_pin_test(pctl, offset, DVF99_GPIO_DIR))
		return GPIO_LINE_DIRECTION_IN;

	return GPIO_LINE_DIRECTION_OUT;
}

static int dvf99_gpio_direction_input(struct gpio_chip *gc,
				      unsigned int offset)
{
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);

	dvf99_pin_write(pctl, offset, DVF99_GPIO_DIR_IN);

	return 0;
}

static int dvf99_gpio_set(struct gpio_chip *gc, unsigned int offset,
			  int value)
{
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);

	dvf99_pin_write(pctl, offset,
			value ? DVF99_GPIO_DATA_SET : DVF99_GPIO_DATA_CLR);

	return 0;
}

static int dvf99_gpio_direction_output(struct gpio_chip *gc,
				       unsigned int offset, int value)
{
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);

	dvf99_gpio_set(gc, offset, value);
	dvf99_pin_write(pctl, offset, DVF99_GPIO_DIR_OUT);

	return 0;
}

static int dvf99_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);

	return dvf99_pin_test(pctl, offset, DVF99_GPIO_DATA);
}

static int dvf99_gpio_init_valid_mask(struct gpio_chip *gc,
				      unsigned long *valid_mask,
				      unsigned int ngpios)
{
	unsigned int pin;

	for (pin = 0; pin < ngpios; pin++)
		if (!dvf99_pin_valid(pin))
			clear_bit(pin, valid_mask);

	return 0;
}

static int dvf99_extint_find(unsigned int pin)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(dvf99_extints); i++)
		if (dvf99_extints[i].pin == pin)
			return i;

	return -EINVAL;
}

static void dvf99_extint_cfg_write(struct dvf99_pinctrl *pctl,
				   unsigned int extint, u32 cfg)
{
	unsigned int shift = extint * 2;
	u32 val;

	val = readl(pctl->base + DVF99_EXTINT_CFG);
	val &= ~(DVF99_EXTINT_CFG_MASK << shift);
	val |= cfg << shift;
	writel(val, pctl->base + DVF99_EXTINT_CFG);
}

static void dvf99_extint_track_level(struct dvf99_pinctrl *pctl,
				     unsigned int pin, unsigned int extint)
{
	u32 cfg = dvf99_pin_test(pctl, pin, DVF99_GPIO_DATA) ?
		  0 : DVF99_EXTINT_POL_HIGH;
	unsigned long flags;

	raw_spin_lock_irqsave(&pctl->lock, flags);
	dvf99_extint_cfg_write(pctl, extint, cfg);
	raw_spin_unlock_irqrestore(&pctl->lock, flags);
}

static void dvf99_irq_ack(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);
	int extint = dvf99_extint_find(irqd_to_hwirq(d));

	writel(BIT(extint), pctl->base + DVF99_EXTINT_EDGE_CLR);
	irq_chip_ack_parent(d);
}

static void dvf99_irq_mask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	irq_chip_mask_parent(d);
	gpiochip_disable_irq(gc, irqd_to_hwirq(d));
}

static void dvf99_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);
	irq_hw_number_t pin = irqd_to_hwirq(d);

	gpiochip_enable_irq(gc, pin);
	if (test_bit(pin, pctl->both_edges))
		dvf99_extint_track_level(pctl, pin, dvf99_extint_find(pin));
	irq_chip_unmask_parent(d);
}

static int dvf99_irq_set_type(struct irq_data *d, unsigned int type)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);
	irq_hw_number_t pin = irqd_to_hwirq(d);
	int extint = dvf99_extint_find(pin);
	unsigned long flags;
	u32 cfg;

	if (extint < 0)
		return extint;

	clear_bit(pin, pctl->both_edges);

	switch (type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_BOTH:
		set_bit(pin, pctl->both_edges);
		irq_set_handler_locked(d, handle_level_irq);
		dvf99_extint_track_level(pctl, pin, extint);
		return irq_chip_set_type_parent(d, IRQ_TYPE_LEVEL_HIGH);
	case IRQ_TYPE_EDGE_RISING:
		cfg = DVF99_EXTINT_MODE_EDGE | DVF99_EXTINT_POL_HIGH;
		irq_set_handler_locked(d, handle_edge_irq);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		cfg = DVF99_EXTINT_MODE_EDGE;
		irq_set_handler_locked(d, handle_edge_irq);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		cfg = DVF99_EXTINT_POL_HIGH;
		irq_set_handler_locked(d, handle_level_irq);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		cfg = 0;
		irq_set_handler_locked(d, handle_level_irq);
		break;
	default:
		return -EINVAL;
	}

	raw_spin_lock_irqsave(&pctl->lock, flags);
	dvf99_extint_cfg_write(pctl, extint, cfg);
	writel(BIT(extint), pctl->base + DVF99_EXTINT_EDGE_CLR);
	raw_spin_unlock_irqrestore(&pctl->lock, flags);

	return irq_chip_set_type_parent(d, IRQ_TYPE_LEVEL_HIGH);
}

static int dvf99_irq_reqres(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct dvf99_pinctrl *pctl = gpiochip_get_data(gc);
	irq_hw_number_t pin = irqd_to_hwirq(d);

	dvf99_pin_write(pctl, pin, DVF99_GPIO_DIR_IN);
	dvf99_pin_write(pctl, pin, DVF99_GPIO_EN_SET);

	return gpiochip_reqres_irq(gc, pin);
}

static void dvf99_irq_relres(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);

	gpiochip_relres_irq(gc, irqd_to_hwirq(d));
}

static const struct irq_chip dvf99_irq_chip = {
	.name			= "dvf99-gpio",
	.irq_ack		= dvf99_irq_ack,
	.irq_mask		= dvf99_irq_mask,
	.irq_unmask		= dvf99_irq_unmask,
	.irq_set_type		= dvf99_irq_set_type,
	.irq_request_resources	= dvf99_irq_reqres,
	.irq_release_resources	= dvf99_irq_relres,
	.flags			= IRQCHIP_IMMUTABLE | IRQCHIP_SKIP_SET_WAKE,
};

static int dvf99_child_to_parent_hwirq(struct gpio_chip *gc,
				       unsigned int child,
				       unsigned int child_type,
				       unsigned int *parent,
				       unsigned int *parent_type)
{
	int extint = dvf99_extint_find(child);

	if (extint < 0)
		return extint;

	*parent = dvf99_extints[extint].parent_hwirq;
	*parent_type = IRQ_TYPE_LEVEL_HIGH;

	return 0;
}

static void dvf99_irq_init_valid_mask(struct gpio_chip *gc,
				      unsigned long *valid_mask,
				      unsigned int ngpios)
{
	unsigned int i;

	bitmap_zero(valid_mask, ngpios);
	for (i = 0; i < ARRAY_SIZE(dvf99_extints); i++)
		set_bit(dvf99_extints[i].pin, valid_mask);
}

static int dvf99_pins_init(struct dvf99_pinctrl *pctl)
{
	struct pinctrl_pin_desc *pins;
	unsigned int pin, npins = 0;

	pins = devm_kcalloc(pctl->dev, DVF99_GPIO_NUM, sizeof(*pins),
			    GFP_KERNEL);
	if (!pins)
		return -ENOMEM;

	for (pin = 0; pin < DVF99_GPIO_NUM; pin++) {
		if (!dvf99_pin_valid(pin))
			continue;

		pins[npins].number = pin;
		pins[npins].name = devm_kasprintf(pctl->dev, GFP_KERNEL,
						  "%cGPIO%u",
						  'A' + pin / DVF99_GPIO_PER_BANK,
						  pin % DVF99_GPIO_PER_BANK);
		if (!pins[npins].name)
			return -ENOMEM;
		npins++;
	}

	pctl->desc.pins = pins;
	pctl->desc.npins = npins;

	return 0;
}

static int dvf99_gpio_init(struct dvf99_pinctrl *pctl)
{
	struct device_node *parent_np;
	struct gpio_irq_chip *girq;
	struct irq_domain *parent;
	struct gpio_chip *gc = &pctl->gc;

	parent_np = of_irq_find_parent(pctl->dev->of_node);
	if (!parent_np)
		return dev_err_probe(pctl->dev, -ENODEV,
				     "no interrupt parent\n");

	parent = irq_find_host(parent_np);
	of_node_put(parent_np);
	if (!parent)
		return dev_err_probe(pctl->dev, -EPROBE_DEFER,
				     "interrupt parent not ready\n");

	gc->label = dev_name(pctl->dev);
	gc->parent = pctl->dev;
	gc->owner = THIS_MODULE;
	gc->base = -1;
	gc->ngpio = DVF99_GPIO_NUM;
	gc->request = gpiochip_generic_request;
	gc->free = gpiochip_generic_free;
	gc->set_config = gpiochip_generic_config;
	gc->get_direction = dvf99_gpio_get_direction;
	gc->direction_input = dvf99_gpio_direction_input;
	gc->direction_output = dvf99_gpio_direction_output;
	gc->get = dvf99_gpio_get;
	gc->set = dvf99_gpio_set;
	gc->init_valid_mask = dvf99_gpio_init_valid_mask;

	girq = &gc->irq;
	gpio_irq_chip_set_chip(girq, &dvf99_irq_chip);
	girq->fwnode = dev_fwnode(pctl->dev);
	girq->parent_domain = parent;
	girq->child_to_parent_hwirq = dvf99_child_to_parent_hwirq;
	girq->init_valid_mask = dvf99_irq_init_valid_mask;
	girq->handler = handle_level_irq;
	girq->default_type = IRQ_TYPE_NONE;

	return devm_gpiochip_add_data(pctl->dev, gc, pctl);
}

static int dvf99_pinctrl_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dvf99_pinctrl *pctl;
	int ret;

	pctl = devm_kzalloc(dev, sizeof(*pctl), GFP_KERNEL);
	if (!pctl)
		return -ENOMEM;

	pctl->dev = dev;
	raw_spin_lock_init(&pctl->lock);

	pctl->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(pctl->base))
		return PTR_ERR(pctl->base);

	pctl->syscfg = syscon_regmap_lookup_by_phandle(dev->of_node,
						       "dspg,syscfg");
	if (IS_ERR(pctl->syscfg))
		return dev_err_probe(dev, PTR_ERR(pctl->syscfg),
				     "no syscfg regmap\n");

	ret = dvf99_pins_init(pctl);
	if (ret)
		return ret;

	pctl->desc.name = dev_name(dev);
	pctl->desc.owner = THIS_MODULE;
	pctl->desc.pctlops = &dvf99_pinctrl_ops;
	pctl->desc.pmxops = &dvf99_pinmux_ops;
	pctl->desc.confops = &dvf99_pinconf_ops;

	ret = devm_pinctrl_register_and_init(dev, &pctl->desc, pctl,
					     &pctl->pctldev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to register pinctrl\n");

	ret = pinctrl_enable(pctl->pctldev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to enable pinctrl\n");

	return dvf99_gpio_init(pctl);
}

static const struct of_device_id dvf99_pinctrl_of_match[] = {
	{ .compatible = "dspg,dvf99-pinctrl" },
	{ }
};

static struct platform_driver dvf99_pinctrl_driver = {
	.probe = dvf99_pinctrl_probe,
	.driver = {
		.name = "dvf99-pinctrl",
		.of_match_table = dvf99_pinctrl_of_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(dvf99_pinctrl_driver);
