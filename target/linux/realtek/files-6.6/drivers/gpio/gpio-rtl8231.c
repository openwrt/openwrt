// SPDX-License-Identifier: GPL-2.0-only

#include <linux/gpio/driver.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <asm/mach-rtl838x/mach-rtl83xx.h>

/* RTL8231 registers for LED control */
#define RTL8231_LED_FUNC0			0x0000
#define RTL8231_LED_FUNC1			0x0001
#define RTL8231_READY_MASK			0x03f0
#define RTL8231_READY_VALUE			0x0370
#define RTL8231_GPIO_PIN_SEL(gpio)		((0x0002) + ((gpio) >> 4))
#define RTL8231_GPIO_DIR(gpio)			((0x0005) + ((gpio) >> 4))
#define RTL8231_GPIO_DATA(gpio)			((0x001C) + ((gpio) >> 4))

#define USEC_TIMEOUT 5000

#define RTL8231_SMI_BUS_ID_MAX			0x1F

struct rtl8231_gpios {
	struct gpio_chip gc;
	struct device *dev;
	u32 id;
	u32 smi_bus_id;
	u16 reg_shadow[0x20];
	u32 reg_cached;
	int ext_gpio_indrt_access;
};

extern struct rtl83xx_soc_info soc_info;

DEFINE_MUTEX(miim_lock);

static u32 rtl8231_read(struct rtl8231_gpios *gpios, u32 reg)
{
	u32 t = 0, n = 0;

	reg &= 0x1f;

	/* Calculate read register address */
	t = (gpios->smi_bus_id << 2) | (reg << 7);

	/* Set execution bit: cleared when operation completed */
	t |= 1;

	/* Start execution */
	sw_w32(t, gpios->ext_gpio_indrt_access);
	do {
		udelay(1);
		t = sw_r32(gpios->ext_gpio_indrt_access);
		n++;
	} while ((t & 1) && (n < USEC_TIMEOUT));

	if (n >= USEC_TIMEOUT)
		return 0x80000000;

	if (soc_info.family == RTL9310_FAMILY_ID) {
		pr_debug("%s: %x, %x, %x\n", __func__, gpios->smi_bus_id,
			reg, (t & 0xffff000) >> 12);
		return (t & 0xffff000) >> 12;
	} else {
		pr_debug("%s: %x, %x, %x\n", __func__, gpios->smi_bus_id,
			reg, (t & 0xffff0000) >> 16);

		return (t & 0xffff0000) >> 16;
	}
}

static int rtl8231_write(struct rtl8231_gpios *gpios, u32 reg, u32 data)
{
	u32 t = 0, n = 0;

	pr_debug("%s: %x, %x, %x\n", __func__, gpios->smi_bus_id, reg, data);
	reg &= 0x1f;

	if (soc_info.family == RTL9310_FAMILY_ID) {
		t = (gpios->smi_bus_id << 2) | (reg << 7) | (data << 12);
	} else {
		t = (gpios->smi_bus_id << 2) | (reg << 7) | (data << 16);
	}
	/* Set write bit */
	t |= 2;

	/* Set execution bit: cleared when operation completed */
	t |= 1;

	/* Start execution */
	sw_w32(t, gpios->ext_gpio_indrt_access);
	do {
		udelay(1);
		t = sw_r32(gpios->ext_gpio_indrt_access);
	} while ((t & 1) && (n < USEC_TIMEOUT));

	if (n >= USEC_TIMEOUT)
		return -1;

	return 0;
}

static u32 rtl8231_read_cached(struct rtl8231_gpios *gpios, u32 reg)
{
	if (reg > 0x1f)
		return 0;

	if (gpios->reg_cached & (1 << reg))
		return gpios->reg_shadow[reg];

	return rtl8231_read(gpios, reg);
}

/* Set Direction of the RTL8231 pin:
 * dir 1: input
 * dir 0: output
 */
static int rtl8231_pin_dir(struct rtl8231_gpios *gpios, u32 gpio, u32 dir)
{
	u32 v;
	int pin_sel_addr = RTL8231_GPIO_PIN_SEL(gpio);
	int pin_dir_addr = RTL8231_GPIO_DIR(gpio);
	int dpin = gpio % 16;

	if (gpio > 31) {
		pr_debug("WARNING: HIGH pin\n");
		dpin += 5;
		pin_dir_addr = pin_sel_addr;
	}

	v = rtl8231_read_cached(gpios, pin_dir_addr);
	if (v & 0x80000000) {
		pr_err("Error reading RTL8231\n");
		return -1;
	}

	v = (v & ~(1 << dpin)) | (dir << dpin);
	rtl8231_write(gpios, pin_dir_addr, v);
	gpios->reg_shadow[pin_dir_addr] = v;
	gpios->reg_cached |= 1 << pin_dir_addr;

	return 0;
}

static int rtl8231_pin_dir_get(struct rtl8231_gpios *gpios, u32 gpio, u32 *dir)
{
	/* dir 1: input
	 * dir 0: output
	 */

	u32  v;
	int pin_dir_addr = RTL8231_GPIO_DIR(gpio);
	int pin = gpio % 16;

	if (gpio > 31) {
		pin_dir_addr = RTL8231_GPIO_PIN_SEL(gpio);
		pin += 5;
	}

	v = rtl8231_read(gpios, pin_dir_addr);
	if (v & (1 << pin))
		*dir = 1;
	else
		*dir = 0;

	return 0;
}

static int rtl8231_pin_set(struct rtl8231_gpios *gpios, u32 gpio, u32 data)
{
	u32 v = rtl8231_read(gpios, RTL8231_GPIO_DATA(gpio));

	pr_debug("%s: %d to %d\n", __func__, gpio, data);
	if (v & 0x80000000) {
		pr_err("Error reading RTL8231\n");
		return -1;
	}
	v = (v & ~(1 << (gpio % 16))) | (data << (gpio % 16));
	rtl8231_write(gpios, RTL8231_GPIO_DATA(gpio), v);
	gpios->reg_shadow[RTL8231_GPIO_DATA(gpio)] = v;
	gpios->reg_cached |= 1 << RTL8231_GPIO_DATA(gpio);

	return 0;
}

static int rtl8231_pin_get(struct rtl8231_gpios *gpios, u32 gpio, u16 *state)
{
	u32 v = rtl8231_read(gpios, RTL8231_GPIO_DATA(gpio));

	if (v & 0x80000000) {
		pr_err("Error reading RTL8231\n");
		return -1;
	}

	*state = v & 0xffff;

	return 0;
}

static int rtl8231_direction_input(struct gpio_chip *gc, unsigned int offset)
{
	int err;
	struct rtl8231_gpios *gpios = gpiochip_get_data(gc);

	pr_debug("%s: %d\n", __func__, offset);
	mutex_lock(&miim_lock);
	err = rtl8231_pin_dir(gpios, offset, 1);
	mutex_unlock(&miim_lock);

	return err;
}

static int rtl8231_direction_output(struct gpio_chip *gc, unsigned int offset, int value)
{
	int err;
	struct rtl8231_gpios *gpios = gpiochip_get_data(gc);

	pr_debug("%s: %d\n", __func__, offset);
	mutex_lock(&miim_lock);
	err = rtl8231_pin_dir(gpios, offset, 0);
	mutex_unlock(&miim_lock);

	if (!err)
		err = rtl8231_pin_set(gpios, offset, value);

	return err;
}

static int rtl8231_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	u32 v = 0;
	struct rtl8231_gpios *gpios = gpiochip_get_data(gc);

	pr_debug("%s: %d\n", __func__, offset);
	mutex_lock(&miim_lock);
	rtl8231_pin_dir_get(gpios, offset, &v);
	mutex_unlock(&miim_lock);

	return v;
}

static int rtl8231_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	u16 state = 0;
	struct rtl8231_gpios *gpios = gpiochip_get_data(gc);

	mutex_lock(&miim_lock);
	rtl8231_pin_get(gpios, offset, &state);
	mutex_unlock(&miim_lock);

	if (state & (1 << (offset % 16)))
		return 1;

	return 0;
}

void rtl8231_gpio_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct rtl8231_gpios *gpios = gpiochip_get_data(gc);

	rtl8231_pin_set(gpios, offset, value);
}

int rtl8231_init(struct rtl8231_gpios *gpios)
{
	u32 ret;

	pr_info("%s called, MDIO bus ID: %d\n", __func__, gpios->smi_bus_id);

	gpios->reg_cached = 0;

	if (soc_info.family == RTL8390_FAMILY_ID) {
		/* RTL8390: Enable external gpio in global led control register */
		sw_w32_mask(0x7 << 18, 0x4 << 18, RTL839X_LED_GLB_CTRL);
	} else if (soc_info.family == RTL8380_FAMILY_ID) {
		/* RTL8380: Enable RTL8231 indirect access mode */
		sw_w32_mask(0, 1, RTL838X_EXTRA_GPIO_CTRL);
		sw_w32_mask(3, 1, RTL838X_DMY_REG5);
	} else if (soc_info.family == RTL9300_FAMILY_ID) {
		sw_w32_mask(1 << 8, 1 << 8, RTL930X_EXT_GPIO_GLB_CTRL);
	} else if (soc_info.family == RTL9310_FAMILY_ID) {
		/* RTL9310 Enable external gpio in EXT_GPIO_GLB_CTRL register */
		sw_w32_mask(1 << 8, 1 << 8, RTL931X_EXT_GPIO_GLB_CTRL);
	}

	if (soc_info.family == RTL8390_FAMILY_ID || soc_info.family == RTL8380_FAMILY_ID) {
		ret = rtl8231_read(gpios, RTL8231_LED_FUNC1);
		if ((ret & 0x80000000) || ((ret & RTL8231_READY_MASK) != RTL8231_READY_VALUE)) {
			return -ENXIO;
		}
	}

	/* Select GPIO functionality and force input direction for pins 0-36 */
	rtl8231_write(gpios, RTL8231_GPIO_PIN_SEL(0), 0xffff);
	rtl8231_write(gpios, RTL8231_GPIO_DIR(0), 0xffff);
	rtl8231_write(gpios, RTL8231_GPIO_PIN_SEL(16), 0xffff);
	rtl8231_write(gpios, RTL8231_GPIO_DIR(16), 0xffff);
	rtl8231_write(gpios, RTL8231_GPIO_PIN_SEL(32), 0x03ff);

	/* Set LED_Start to enable drivers for output mode */
	rtl8231_write(gpios, RTL8231_LED_FUNC0, 1 << 1);

	return 0;
}

static const struct of_device_id rtl8231_gpio_of_match[] = {
	{ .compatible = "realtek,rtl8231-gpio" },
	{},
};

MODULE_DEVICE_TABLE(of, rtl8231_gpio_of_match);

static int rtl8231_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct rtl8231_gpios *gpios;
	int err;

	pr_info("Probing RTL8231 GPIOs\n");

	if (!np) {
		dev_err(&pdev->dev, "No DT found\n");
		return -EINVAL;
	}

	gpios = devm_kzalloc(dev, sizeof(*gpios), GFP_KERNEL);
	if (!gpios)
		return -ENOMEM;

	gpios->id = soc_info.id;
	if (soc_info.family == RTL8380_FAMILY_ID) {
		gpios->ext_gpio_indrt_access = RTL838X_EXT_GPIO_INDRT_ACCESS;
	}

	if (soc_info.family == RTL8390_FAMILY_ID) {
		gpios->ext_gpio_indrt_access = RTL839X_EXT_GPIO_INDRT_ACCESS;
	}

	if (soc_info.family == RTL9300_FAMILY_ID) {
		gpios->ext_gpio_indrt_access = RTL930X_EXT_GPIO_INDRT_ACCESS;
	}

	if (soc_info.family == RTL9310_FAMILY_ID) {
		gpios->ext_gpio_indrt_access = RTL931X_EXT_GPIO_INDRT_ACCESS;
	}

	err = of_property_read_u32(np, "indirect-access-bus-id", &gpios->smi_bus_id);
	if (!err && gpios->smi_bus_id > RTL8231_SMI_BUS_ID_MAX)
		err = -EINVAL;

	if (err) {
		dev_err(dev, "invalid or missing indirect-access-bus-id\n");
		return err;
	}

	err = rtl8231_init(gpios);
	if (err) {
		dev_err(dev, "no device found at bus address %d\n", gpios->smi_bus_id);
		return err;
	}

	gpios->dev = dev;
	gpios->gc.base = -1;
	gpios->gc.ngpio = 37;
	gpios->gc.label = "rtl8231";
	gpios->gc.parent = dev;
	gpios->gc.owner = THIS_MODULE;
	gpios->gc.can_sleep = true;

	gpios->gc.direction_input = rtl8231_direction_input;
	gpios->gc.direction_output = rtl8231_direction_output;
	gpios->gc.set = rtl8231_gpio_set;
	gpios->gc.get = rtl8231_gpio_get;
	gpios->gc.get_direction = rtl8231_get_direction;

	return devm_gpiochip_add_data(dev, &gpios->gc, gpios);
}

static struct platform_driver rtl8231_gpio_driver = {
	.driver = {
		.name = "rtl8231-gpio",
		.of_match_table	= rtl8231_gpio_of_match,
	},
	.probe = rtl8231_gpio_probe,
};

module_platform_driver(rtl8231_gpio_driver);

MODULE_DESCRIPTION("Realtek RTL8231 GPIO expansion chip support");
MODULE_LICENSE("GPL v2");
