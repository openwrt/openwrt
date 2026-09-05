// SPDX-License-Identifier: GPL-2.0-only
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/stringify.h>

#define RTL9607C_GPIO_PINS_NUM	96

#define RTL9607C_IO_GPIO_EN	0x38
/*
 * Bits 0-17 - LEDs 0-17
 */
#define RTL9607C_IO_LED_EN	0x23010
#define   IO_LED_SERIAL_EN	BIT(23)
/*
 * Bit set - peripheral connected to SoC pins
 */
#define RTL9607C_IO_MODE_EN	0x23014
#define   IO_MODE_UART3		BIT(0)
/*
 * Perhaps number of UART which is
 * using flow control (RTS/CTS)
 */
#define   IO_MODE_UART_FC	GENMASK(2, 1)
#define   IO_MODE_UART1		BIT(3)
#define   IO_MODE_UART0		BIT(4)
#define   IO_MODE_UART2_NC	BIT(5)
#define   IO_MODE_SPI		BIT(6)
#define   IO_MODE_MDIO_SLAVE	BIT(7)
/*
 * Effect of these bits is unclear
 * It might make sence if MDIO_SLAVE set
 * But MDIO slave functionality is quite
 * questionable for router SoC
 */
#define   IO_MODE_MDIO0_MASTER	BIT(8)
#define   IO_MODE_MDIO1_MASTER	BIT(9)
#define   IO_MODE_MDIO0		BIT(10)
#define   IO_MODE_MDIO1		BIT(11)
#define   IO_MODE_I2C_SLAVE	BIT(12)
#define   IO_MODE_I2C0		BIT(13)
#define   IO_MODE_I2C1		BIT(14)
/*
 * Following bits are listed only for future references
 */
#define   IO_MODE_SLIC_ISI	BIT(15)
#define   IO_MODE_SLIC_ZSI	BIT(16)
#define   IO_MODE_SLIC_PCM	BIT(17)
#define   IO_MODE_DYING_GASP0	BIT(18)
#define   IO_MODE_OEM		BIT(19)
#define   IO_MODE_PPS		BIT(20)
#define   IO_MODE_SC_CD		BIT(21)
#define   IO_MODE_PPS_SEL	BIT(22)
#define   IO_MODE_DYING_GASP1	BIT(23)
#define   IO_MODE_DYING_GASP2	BIT(24)

struct rtl9607c_pinctrl {
	struct device *dev;
	struct pinctrl_dev *pctrldev;
	struct regmap *regmap;
	/*
	 * Keep track of set groups to unset it later
	 */
	const struct rtl9607c_function *pin_states[RTL9607C_GPIO_PINS_NUM];
};

/*
 * PINs numbers are taken from GPIO numbers
 */

/*
 * It isn't clear if RTS/CTS hardcoded to only one UART,
 * or it could be used with any UART. Or could be that
 * HS UART used without RTS/CTS. Or what UART it actually
 * is (4 in software, 3 routed to SoC pins).
 * Leave it like this for future
 */
static const unsigned int hs_uart_pins[] __maybe_unused = { 15, 16, 32, 62 };
static const unsigned int uart1_pins[] = { 20, 21 };
static const unsigned int uart0_pins[] = { 41, 42 };
/*
 * SPI couldn't be muxed with anything.
 * However there is SPI enable bit in IO_MODE register
 */
static const unsigned int spi_pins[] = { };
static const unsigned int mdio0_pins[] = { 6, 7 };
static const unsigned int mdio1_pins[] = { 10, 12 };
static const unsigned int i2c0_pins[] = { 0, 1 };
static const unsigned int i2c1_pins[] = { 8, 9 };

static const unsigned int spi_qio_pins[] = { 70, 71 };
static const unsigned int spi_reset_pins[] = { 57 };
static const unsigned int jtag_pins[] = { 36, 37, 38, 39, 40 };
static const unsigned int dis_tx_pins[] = { 2 };
static const unsigned int tx_sd_pins[] = { 3 };
static const unsigned int ptp_pps_pins[] = { 5 };
static const unsigned int ptp_tod_pins[] = { 6 };

static const unsigned int serial_led_pins[] = { 33, 35 };
static const unsigned int pled0_pins[] = { 6 };
static const unsigned int pled1_pins[] = { 7 };
static const unsigned int pled2_pins[] = { 8 };
static const unsigned int pled3_pins[] = { 9 };
static const unsigned int pled4_pins[] = { 10 };
static const unsigned int pled5_pins[] = { 12 };
static const unsigned int pled8_pins[] = { 15 };
static const unsigned int pled9_pins[] = { 16 };
static const unsigned int pled13_pins[] = { 20 };
static const unsigned int pled14_pins[] = { 21 };
static const unsigned int pled15_pins[] = { 32 };
static const unsigned int pled16_pins[] = { 33 };
static const unsigned int pled17_pins[] = { 35 };

#define RTL9607C_PIN_BLOCK(base)	\
	PINCTRL_PIN_ANON(base + 0),	\
	PINCTRL_PIN_ANON(base + 1),	\
	PINCTRL_PIN_ANON(base + 2),	\
	PINCTRL_PIN_ANON(base + 3),	\
	PINCTRL_PIN_ANON(base + 4),	\
	PINCTRL_PIN_ANON(base + 5),	\
	PINCTRL_PIN_ANON(base + 6),	\
	PINCTRL_PIN_ANON(base + 7)
#define RTL9607C_PIN_BANK(base)		\
	RTL9607C_PIN_BLOCK(base + 0),	\
	RTL9607C_PIN_BLOCK(base + 8),	\
	RTL9607C_PIN_BLOCK(base + 16),	\
	RTL9607C_PIN_BLOCK(base + 24)
static const struct pinctrl_pin_desc rtl9607c_pins[] = {
	RTL9607C_PIN_BANK(0),
	RTL9607C_PIN_BANK(32),
	RTL9607C_PIN_BANK(64),
};

static_assert(RTL9607C_GPIO_PINS_NUM == ARRAY_SIZE(rtl9607c_pins));

struct rtl9607c_function {
	const char *const name;
	const unsigned int *pins;
	const unsigned int npins;

	const u32 reg;
	const u32 mask;
};

#define GROUP_AND_FUNCTION(_name, _reg, _mask) {	\
	.name = #_name,			\
	.pins = _name ## _pins,		\
	.npins = ARRAY_SIZE(_name ## _pins),	\
	.reg = _reg,			\
	.mask = _mask,			\
}

static const struct rtl9607c_function groups_and_functions[] = {
	GROUP_AND_FUNCTION(uart1, RTL9607C_IO_MODE_EN, IO_MODE_UART1),
	GROUP_AND_FUNCTION(uart0, RTL9607C_IO_MODE_EN, IO_MODE_UART0),
	GROUP_AND_FUNCTION(spi, RTL9607C_IO_MODE_EN, IO_MODE_SPI),
	GROUP_AND_FUNCTION(mdio0, RTL9607C_IO_MODE_EN, IO_MODE_MDIO0_MASTER | IO_MODE_MDIO0),
	GROUP_AND_FUNCTION(mdio1, RTL9607C_IO_MODE_EN, IO_MODE_MDIO1_MASTER | IO_MODE_MDIO1),
	GROUP_AND_FUNCTION(i2c0, RTL9607C_IO_MODE_EN, IO_MODE_I2C0),
	GROUP_AND_FUNCTION(i2c1, RTL9607C_IO_MODE_EN, IO_MODE_I2C1),

	/*
	 * No suitable enable bit for these groups found
	 * or could be tested
	 */
	GROUP_AND_FUNCTION(spi_qio, 0, 0),
	GROUP_AND_FUNCTION(spi_reset, 0, 0),
	GROUP_AND_FUNCTION(jtag, 0, 0),
	GROUP_AND_FUNCTION(dis_tx, 0, 0),
	GROUP_AND_FUNCTION(tx_sd, 0, 0),
	GROUP_AND_FUNCTION(ptp_pps, 0, 0),
	GROUP_AND_FUNCTION(ptp_tod, 0, 0),

	GROUP_AND_FUNCTION(serial_led, RTL9607C_IO_LED_EN, IO_LED_SERIAL_EN),
	GROUP_AND_FUNCTION(pled0, RTL9607C_IO_LED_EN, BIT(0)),
	GROUP_AND_FUNCTION(pled1, RTL9607C_IO_LED_EN, BIT(1)),
	GROUP_AND_FUNCTION(pled2, RTL9607C_IO_LED_EN, BIT(2)),
	GROUP_AND_FUNCTION(pled3, RTL9607C_IO_LED_EN, BIT(3)),
	GROUP_AND_FUNCTION(pled4, RTL9607C_IO_LED_EN, BIT(4)),
	GROUP_AND_FUNCTION(pled5, RTL9607C_IO_LED_EN, BIT(5)),
	GROUP_AND_FUNCTION(pled8, RTL9607C_IO_LED_EN, BIT(8)),
	GROUP_AND_FUNCTION(pled9, RTL9607C_IO_LED_EN, BIT(9)),
	GROUP_AND_FUNCTION(pled13, RTL9607C_IO_LED_EN, BIT(13)),
	GROUP_AND_FUNCTION(pled14, RTL9607C_IO_LED_EN, BIT(14)),
	GROUP_AND_FUNCTION(pled15, RTL9607C_IO_LED_EN, BIT(15)),
	GROUP_AND_FUNCTION(pled16, RTL9607C_IO_LED_EN, BIT(16)),
	GROUP_AND_FUNCTION(pled17, RTL9607C_IO_LED_EN, BIT(17)),
};

static int rtl9607c_get_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(groups_and_functions);
}

static const char *rtl9607c_get_name(struct pinctrl_dev *pctldev,
					   unsigned int index)
{
	return groups_and_functions[index].name;
}

static int rtl9607c_get_group_pins(struct pinctrl_dev *pctldev,
				   unsigned int group,
				   const unsigned int **pins,
				   unsigned int *npins)
{
	*pins = groups_and_functions[group].pins;
	*npins = groups_and_functions[group].npins;
	return 0;
}

static struct pinctrl_ops rtl9607c_pctrl_ops = {
	.get_groups_count = rtl9607c_get_count,
	.get_group_name = rtl9607c_get_name,
	.get_group_pins = rtl9607c_get_group_pins,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_group,
};

static int rtl9607c_get_groups(struct pinctrl_dev *pctldev, unsigned int func,
			       const char *const **groups,
			       unsigned int *const ngroups)
{
	*groups = &groups_and_functions[func].name;
	*ngroups = 1;
	return 0;
}

static int rtl9607c_set_mux(struct pinctrl_dev *pctldev, unsigned int func,
			    unsigned int group)
{
	struct rtl9607c_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);

	const struct rtl9607c_function *pgroup = &groups_and_functions[func];
	int i;

	WARN(group != func, "Group %d and func %d doesn't match!\n", group, func);

	if (pgroup->reg) {
		regmap_set_bits(pctrl->regmap, pgroup->reg, pgroup->mask);
	}

	for (i = 0; i < pgroup->npins; i++) {
		pctrl->pin_states[pgroup->pins[i]] = pgroup;
	}

	return 0;
}

static int rtl9607c_free_pin(struct pinctrl_dev *pctldev,
					unsigned int pin)
{
	struct rtl9607c_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
	const struct rtl9607c_function *pgroup = pctrl->pin_states[pin];

	pctrl->pin_states[pin] = NULL;

	if (!pgroup)
		return 0;

	if (pgroup->reg) {
		/*
		 * Disable pin routing
		 * Mostly useful for PLED pins,
		 * because these might be muxed with another groups
		 * Double-clear for each group pin is okay
		 */
		regmap_clear_bits(pctrl->regmap, pgroup->reg, pgroup->mask);
	}

	return 0;
}

static int rtl9607c_gpio_request_enable(struct pinctrl_dev *pctldev,
					struct pinctrl_gpio_range *range,
					unsigned int offset)
{
	struct rtl9607c_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int reg_offset = offset / 32 * 4;
	unsigned int bit = BIT(offset % 32);

	regmap_set_bits(pctrl->regmap, RTL9607C_IO_GPIO_EN + reg_offset, bit);
	return 0;
}

static void rtl9607c_gpio_disable_free(struct pinctrl_dev *pctldev,
				       struct pinctrl_gpio_range *range,
				       unsigned int offset)
{
	struct rtl9607c_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
	unsigned int reg_offset = offset / 32 * 4;
	unsigned int bit = BIT(offset % 32);

	regmap_clear_bits(pctrl->regmap, RTL9607C_IO_GPIO_EN + reg_offset, bit);
}

static struct pinmux_ops rtl9607c_pmxops = {
	.get_functions_count = rtl9607c_get_count,
	.get_function_name = rtl9607c_get_name,
	.get_function_groups = rtl9607c_get_groups,
	.gpio_request_enable = rtl9607c_gpio_request_enable,
	.gpio_disable_free = rtl9607c_gpio_disable_free,
	.free = rtl9607c_free_pin,
	.set_mux = rtl9607c_set_mux,
	.strict = true,
};

static struct pinctrl_desc rtl9607c_pinctrl_desc = {
	.pctlops = &rtl9607c_pctrl_ops,
	.pmxops = &rtl9607c_pmxops,
	.pins = rtl9607c_pins,
	.npins = ARRAY_SIZE(rtl9607c_pins),
	.name = "rtl9607c-pinctrl",
};

static int rtl9607c_pinctrl_probe(struct platform_device *pdev)
{
	struct rtl9607c_pinctrl *pctrl;
	struct device *dev = &pdev->dev;

	pctrl = devm_kzalloc(dev, sizeof(*pctrl), GFP_KERNEL);
	if (!pctrl)
		return -ENOMEM;

	pctrl->dev = dev;

	pctrl->regmap = syscon_node_to_regmap(dev->parent->of_node);
	if (IS_ERR(pctrl->regmap)) {
		return dev_err_probe(dev, PTR_ERR(pctrl->regmap), "could not get syscon regmap\n");
	}

	pctrl->pctrldev =
		devm_pinctrl_register(dev, &rtl9607c_pinctrl_desc, pctrl);
	if (IS_ERR(pctrl->pctrldev)) {
		return dev_err_probe(dev, PTR_ERR(pctrl->pctrldev), "could not register RTL9607C pinmux driver\n");
	}

	platform_set_drvdata(pdev, pctrl);

	return 0;
}

static const struct of_device_id rtl9607c_pinctrl_of_match[] = {
	{
		.compatible = "realtek,rtl9607c-pinctrl",
	},
	{},
};

static struct platform_driver rtl9607c_pinctrl_driver = {
	.probe = rtl9607c_pinctrl_probe,
	.driver =
	{
		.name = "rtl9607c-pinctrl",
		.of_match_table = rtl9607c_pinctrl_of_match,
	},
};

static int rtl9607c_pinctrl_init(void)
{
	return platform_driver_register(&rtl9607c_pinctrl_driver);
}

arch_initcall(rtl9607c_pinctrl_init);
