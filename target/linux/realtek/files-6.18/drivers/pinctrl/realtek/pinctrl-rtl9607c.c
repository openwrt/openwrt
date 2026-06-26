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

#define RTL9607C_IO_GPIO_EN	0x38
#define RTL9607C_IO_LED_EN	0x23010
/*
 * Bit set - peripheral connected to SoC pins
 */
#define RTL9607C_IO_MODE_EN	0x23014
#define   IO_MODE_UART2		BIT(0)
#define   IO_MODE_UART_FC	GENMASK(2, 1)
#define   IO_MODE_UART1		BIT(3)
/*
 * Bootloader only does set BIT(4)
 * Meaning of BIT(5) is unclear
 */
#define   IO_MODE_UART0		GENMASK(5, 4)
#define   IO_MODE_SPI		BIT(6)
#define   IO_MODE_MDIO_SLAVE	BIT(7)
#define   IO_MODE_MDIO0_MASTER	BIT(8)
#define   IO_MODE_MDIO1_MASTER	BIT(9)
#define   IO_MODE_MDIO0		BIT(10)
#define   IO_MODE_MDIO1		BIT(11)
#define   IO_MODE_I2C_SLAVE	BIT(12)
#define   IO_MODE_I2C0		BIT(13)
#define   IO_MODE_I2C1		BIT(14)
/*
 * Next bits are listed only for future references
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
};

/*
 * PINs numbers are taken from GPIO numbers
 */
static const unsigned int i2c0_pins[] = { 0, 1 };
static const unsigned int i2c1_pins[] = { 8, 9 };
static const unsigned int mdio0_pins[] = { 6, 7 };
static const unsigned int mdio1_pins[] = { 10, 12 };
static const unsigned int uart0_pins[] = { 41, 42 };
static const unsigned int uart1_pins[] = { 20, 21 };
static const unsigned int serial_led_pins[] = { 33, 35 };
static const unsigned int hs_uart_pins[] = { 15, 16, 32, 62 };
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

#define RTL9607C_PINGROUP(name)					\
	PINCTRL_PINGROUP(__stringify(name), name##_pins,	\
			 ARRAY_SIZE(name##_pins))
static const struct pingroup rtl9607c_groups[] = {
	RTL9607C_PINGROUP(i2c0),
	RTL9607C_PINGROUP(i2c1),
	RTL9607C_PINGROUP(mdio0),
	RTL9607C_PINGROUP(mdio1),
	RTL9607C_PINGROUP(uart0),
	RTL9607C_PINGROUP(uart1),
	RTL9607C_PINGROUP(serial_led),
	RTL9607C_PINGROUP(hs_uart),
	RTL9607C_PINGROUP(pled0),
	RTL9607C_PINGROUP(pled1),
	RTL9607C_PINGROUP(pled2),
	RTL9607C_PINGROUP(pled3),
	RTL9607C_PINGROUP(pled4),
	RTL9607C_PINGROUP(pled5),
	RTL9607C_PINGROUP(pled8),
	RTL9607C_PINGROUP(pled9),
	RTL9607C_PINGROUP(pled13),
	RTL9607C_PINGROUP(pled14),
	RTL9607C_PINGROUP(pled15),
	RTL9607C_PINGROUP(pled16),
	RTL9607C_PINGROUP(pled17),
};

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

static int rtl9607c_get_groups_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(rtl9607c_groups);
}

static const char *rtl9607c_get_group_name(struct pinctrl_dev *pctldev,
					   unsigned int group)
{
	return rtl9607c_groups[group].name;
}

static int rtl9607c_get_group_pins(struct pinctrl_dev *pctldev,
				   unsigned int group,
				   const unsigned int **pins,
				   unsigned int *npins)
{
	*pins = rtl9607c_groups[group].pins;
	*npins = rtl9607c_groups[group].npins;
	return 0;
}

static struct pinctrl_ops rtl9607c_pctrl_ops = {
	.get_groups_count = rtl9607c_get_groups_count,
	.get_group_name = rtl9607c_get_group_name,
	.get_group_pins = rtl9607c_get_group_pins,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_group,
};

static const char *const i2c_groups[] = { "i2c0", "i2c1" };
static const char *const mdio_groups[] = { "mdio0", "mdio1" };
static const char *const uart_groups[] = { "uart0", "uart1" };
static const char *const serial_led_groups[] = { "serial_led" };
static const char *const hs_uart_groups[] = { "hs_uart" };
static const char *const pled_groups[] = {
	"pled0", "pled1",  "pled2",  "pled3",  "pled4",	 "pled5",  "pled8",
	"pled9", "pled13", "pled14", "pled15", "pled16", "pled17",
};

#define RTL9607C_PINFUNCTION(name)				\
	PINCTRL_PINFUNCTION(__stringify(name), name##_groups,	\
			    ARRAY_SIZE(name##_groups))
static const struct pinfunction rtl9607c_functions[] = {
	RTL9607C_PINFUNCTION(i2c),
	RTL9607C_PINFUNCTION(mdio),
	RTL9607C_PINFUNCTION(uart),
	RTL9607C_PINFUNCTION(serial_led),
	RTL9607C_PINFUNCTION(hs_uart),
	RTL9607C_PINFUNCTION(pled),
};

static int rtl9607c_get_functions_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(rtl9607c_functions);
}

static const char *rtl9607c_get_fname(struct pinctrl_dev *pctldev,
				      unsigned int func)
{
	return rtl9607c_functions[func].name;
}

static int rtl9607c_get_groups(struct pinctrl_dev *pctldev, unsigned int func,
			       const char *const **groups,
			       unsigned int *const ngroups)
{
	*groups = rtl9607c_functions[func].groups;
	*ngroups = rtl9607c_functions[func].ngroups;
	return 0;
}

static void set_led(struct pinctrl_dev *pctldev, const char *group_name,
		    bool enable)
{
	struct rtl9607c_pinctrl *pctrl = pinctrl_dev_get_drvdata(pctldev);
	u32 led;
	int ret;

	ret = sscanf(group_name, "pled%d", &led);

	if (ret != 1 || led > 17)
		return;

	regmap_assign_bits(pctrl->regmap, RTL9607C_IO_LED_EN, BIT(led), enable);
}

static int rtl9607c_set_mux(struct pinctrl_dev *pctldev, unsigned int func,
			    unsigned int group)
{
	bool is_led = strcmp(rtl9607c_get_fname(pctldev, func), "pled") == 0;

	set_led(pctldev, rtl9607c_get_group_name(pctldev, group), is_led);

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
	.get_functions_count = rtl9607c_get_functions_count,
	.get_function_name = rtl9607c_get_fname,
	.get_function_groups = rtl9607c_get_groups,
	.gpio_request_enable = rtl9607c_gpio_request_enable,
	.gpio_disable_free = rtl9607c_gpio_disable_free,
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
		dev_err(dev, "could not get syscon regmap\n");
		return PTR_ERR(pctrl->regmap);
	}

	pctrl->pctrldev =
		devm_pinctrl_register(dev, &rtl9607c_pinctrl_desc, pctrl);
	if (IS_ERR(pctrl->pctrldev)) {
		dev_err(dev, "could not register RTL9607C pinmux driver\n");
		return PTR_ERR(pctrl->pctrldev);
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
