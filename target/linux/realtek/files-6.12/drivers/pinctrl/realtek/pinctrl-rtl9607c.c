#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of_platform.h>

struct rtl9607c_pinctrl {
        struct device *dev;
        struct pinctrl_dev *pctrldev;
};

// PINs numbers are taken from GPIO numbers
static const unsigned int i2c0_pins[] = {0, 1};
static const unsigned int i2c1_pins[] = {8, 9};
static const unsigned int mdio0_pins[] = {6, 7};
static const unsigned int mdio1_pins[] = {10, 12};
static const unsigned int uart0_pins[] = {41, 42};
static const unsigned int uart1_pins[] = {20, 21};
static const unsigned int serial_led_pins[] = {33, 35};
static const unsigned int hs_uart_pins[] = {15, 16, 32, 62};
// TODO parallel LED

#define RTL9607C_PINGROUP(name)	\
	PINCTRL_PINGROUP("name", name##_pins, ARRAY_SIZE(name##_pins))
static const struct pingroup rtl9607c_groups[] = {
	RTL9607C_PINGROUP(i2c0),
	RTL9607C_PINGROUP(i2c1),
	RTL9607C_PINGROUP(mdio0),
	RTL9607C_PINGROUP(mdio1),
	RTL9607C_PINGROUP(uart0),
	RTL9607C_PINGROUP(uart1),
	RTL9607C_PINGROUP(serial_led),
	RTL9607C_PINGROUP(hs_uart),
};

// Attempt to save my sanity
#define RTL9607C_PIN_BLOCk(base)	\
PINCTRL_PIN_ANON(base+0),	\
PINCTRL_PIN_ANON(base+1),	\
PINCTRL_PIN_ANON(base+2),	\
PINCTRL_PIN_ANON(base+3),	\
PINCTRL_PIN_ANON(base+4),	\
PINCTRL_PIN_ANON(base+5),	\
PINCTRL_PIN_ANON(base+6),	\
PINCTRL_PIN_ANON(base+7)
#define RTL9607C_PIN_BANK(base)	\
RTL9607C_PIN_BLOCk(base+0),	\
RTL9607C_PIN_BLOCk(base+8),	\
RTL9607C_PIN_BLOCk(base+16),	\
RTL9607C_PIN_BLOCk(base+24)
static const struct pinctrl_pin_desc rtl9607c_pins[] = {
RTL9607C_PIN_BANK(0),
RTL9607C_PIN_BANK(32),
RTL9607C_PIN_BANK(64),
};

static int rtl9607c_get_groups_count(struct pinctrl_dev *pctldev)
{
	return ARRAY_SIZE(rtl9607c_groups);
}

static const char *rtl9607c_get_group_name(struct pinctrl_dev *pctldev, unsigned int selector)
{
	return rtl9607c_groups[selector].name;
        }

        static int rtl9607c_get_group_pins(struct pinctrl_dev *pctldev, unsigned int selector,
                                      const unsigned int **pins,
                                      unsigned int *npins)
        {
                *pins = rtl9607c_groups[selector].pins;
                *npins = rtl9607c_groups[selector].npins;
                return 0;
        }

        static struct pinctrl_ops rtl9607c_pctrl_ops = {
                .get_groups_count = rtl9607c_get_groups_count,
                .get_group_name = rtl9607c_get_group_name,
                .get_group_pins = rtl9607c_get_group_pins,
        };

#define RTL9607C_DEFINE_GROUP(name)	\
static const char * const name##_groups[] = {"name"}
//	        static const char * const spi0_groups[] = { "spi0_0_grp", "spi0_1_grp" };
RTL9607C_DEFINE_GROUP(i2c0);
	RTL9607C_DEFINE_GROUP(i2c1);
	RTL9607C_DEFINE_GROUP(mdio0);
	RTL9607C_DEFINE_GROUP(mdio1);
	RTL9607C_DEFINE_GROUP(uart0);
	RTL9607C_DEFINE_GROUP(uart1);
	RTL9607C_DEFINE_GROUP(serial_led);
	RTL9607C_DEFINE_GROUP(hs_uart);

#define RTL9607C_PINFUNCTION(name)	\
PINCTRL_PINFUNCTION("name", name##_groups, ARRAY_SIZE(name##_groups))
        static const struct pinfunction rtl9607c_functions[] = {
RTL9607C_PINFUNCTION(i2c0),
	RTL9607C_PINFUNCTION(i2c1),
	RTL9607C_PINFUNCTION(mdio0),
	RTL9607C_PINFUNCTION(mdio1),
	RTL9607C_PINFUNCTION(uart0),
	RTL9607C_PINFUNCTION(uart1),
	RTL9607C_PINFUNCTION(serial_led),
	RTL9607C_PINFUNCTION(hs_uart),
        };

        static int rtl9607c_get_functions_count(struct pinctrl_dev *pctldev)
        {
                return ARRAY_SIZE(rtl9607c_functions);
        }

        static const char *rtl9607c_get_fname(struct pinctrl_dev *pctldev, unsigned int selector)
        {
                return rtl9607c_functions[selector].name;
        }

	        static int rtl9607c_get_groups(struct pinctrl_dev *pctldev, unsigned int selector,
                                  const char * const **groups,
                                  unsigned int * const ngroups)
        {
                *groups = rtl9607c_functions[selector].groups;
                *ngroups = rtl9607c_functions[selector].ngroups;
                return 0;
        }

	        static int rtl9607c_set_mux(struct pinctrl_dev *pctldev, unsigned int selector,
                               unsigned int group)
        {
//                u8 regbit = BIT(group);
printk("%s:%d\n", __func__, __LINE__);

  //              writeb((readb(MUX) | regbit), MUX);
                return 0;
        }

	        static int rtl9607c_gpio_request_enable (struct pinctrl_dev *pctldev,
                                    struct pinctrl_gpio_range *range,
                                    unsigned int offset){
printk("%s:%d\n", __func__, __LINE__);
				    return 0;
				    }
        static void rtl9607c_gpio_disable_free (struct pinctrl_dev *pctldev,
                                   struct pinctrl_gpio_range *range,
                                   unsigned int offset){
printk("%s:%d\n", __func__, __LINE__);
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
//        int ret, i;
printk("%s:%d\n", __func__, __LINE__);

        pctrl = devm_kzalloc(&pdev->dev, sizeof(*pctrl), GFP_KERNEL);
        if (!pctrl)
                return -ENOMEM;

		        pctrl->dev = &pdev->dev;

      pctrl->pctrldev = devm_pinctrl_register(&pdev->dev,
                                        &rtl9607c_pinctrl_desc, pctrl);
        if (IS_ERR(pctrl->pctrldev)) {
                dev_err(&pdev->dev, "could not register RTL9607C pinmux driver\n");
                return PTR_ERR(pctrl->pctrldev);
        }

        platform_set_drvdata(pdev, pctrl);

        return 0;
}

static const struct of_device_id rtl9607c_pinctrl_of_match[] = {
        { .compatible = "realtek,rtl9607c-pinctrl", },
        { },
};
	static struct platform_driver rtl9607c_pinctrl_driver = {
        .probe = rtl9607c_pinctrl_probe,
        .driver = {
                .name = "rtl9607c-pinctrl",
                .of_match_table = rtl9607c_pinctrl_of_match,
        },
};

static int rtl9607c_pinctrl_init(void)
{
        return platform_driver_register(&rtl9607c_pinctrl_driver);
}

arch_initcall(rtl9607c_pinctrl_init);
