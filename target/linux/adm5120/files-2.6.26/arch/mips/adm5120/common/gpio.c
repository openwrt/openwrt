/*
 *  ADM5120 generic GPIO API support
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <asm/addrspace.h>
#include <linux/gpio.h>

#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_switch.h>
#include <asm/mach-adm5120/adm5120_irq.h>

#define GPIO_READ(r)		__raw_readl((r))
#define GPIO_WRITE(v, r)	__raw_writel((v), (r))
#define GPIO_REG(r)	(void __iomem *)(KSEG1ADDR(ADM5120_SWITCH_BASE)+r)

struct adm5120_gpio_line {
	u32 flags;
	const char *label;
	int irq;
};

#define GPIO_FLAG_VALID		0x01
#define GPIO_FLAG_USED		0x02

struct led_desc {
	void __iomem *reg;	/* LED register address */
	u8 iv_shift;		/* shift amount for input bit */
	u8 mode_shift;		/* shift amount for mode bits */
};

#define LED_DESC(p, l) { \
		.reg = GPIO_REG(SWITCH_REG_PORT0_LED+((p) * 4)), \
		.iv_shift = LED0_IV_SHIFT + (l), \
		.mode_shift = (l) * 4 \
	}

static struct led_desc led_table[15] = {
	LED_DESC(0, 0), LED_DESC(0, 1), LED_DESC(0, 2),
	LED_DESC(1, 0), LED_DESC(1, 1), LED_DESC(1, 2),
	LED_DESC(2, 0), LED_DESC(2, 1), LED_DESC(2, 2),
	LED_DESC(3, 0), LED_DESC(3, 1), LED_DESC(3, 2),
	LED_DESC(4, 0), LED_DESC(4, 1), LED_DESC(4, 2)
};

static struct adm5120_gpio_line adm5120_gpio_map[ADM5120_GPIO_COUNT];

static u32 gpio_conf2;

/*-------------------------------------------------------------------------*/

static inline int gpio_is_invalid(unsigned gpio)
{
	if ((gpio > ADM5120_GPIO_MAX) ||
		(adm5120_gpio_map[gpio].flags & GPIO_FLAG_VALID) == 0)
		return 1;

	return 0;
}

static inline int gpio_is_used(unsigned gpio)
{
	return ((adm5120_gpio_map[gpio].flags & GPIO_FLAG_USED) != 0);
}

/*-------------------------------------------------------------------------*/

/*
 * Helpers for GPIO lines in GPIO_CONF0 register
 */
#define PIN_IM(p)	((1 << GPIO_CONF0_IM_SHIFT) << p)
#define PIN_IV(p)	((1 << GPIO_CONF0_IV_SHIFT) << p)
#define PIN_OE(p)	((1 << GPIO_CONF0_OE_SHIFT) << p)
#define PIN_OV(p)	((1 << GPIO_CONF0_OV_SHIFT) << p)

static inline int pins_direction_input(unsigned pin)
{
	void __iomem **reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	t &= ~(PIN_OE(pin));
	t |= PIN_IM(pin);
	GPIO_WRITE(t, reg);

	return 0;
}

static inline int pins_direction_output(unsigned pin, int value)
{
	void __iomem **reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	t &= ~(PIN_IM(pin) | PIN_OV(pin));
	t |= PIN_OE(pin);

	if (value)
		t |= PIN_OV(pin);

	GPIO_WRITE(t, reg);

	return 0;
}

static inline int pins_get_value(unsigned pin)
{
	void __iomem **reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	if ((t & PIN_IM(pin)) != 0)
		t &= PIN_IV(pin);
	else
		t &= PIN_OV(pin);

	return (t) ? 1 : 0;
}

static inline void pins_set_value(unsigned pin, int value)
{
	void __iomem **reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	if (value == 0)
		t &= ~(PIN_OV(pin));
	else
		t |= PIN_OV(pin);

	GPIO_WRITE(t, reg);
}

/*
 * Helpers for GPIO lines in PORTx_LED registers
 */
static inline int leds_direction_input(unsigned led)
{
	void __iomem **reg;
	u32 t;

	reg = led_table[led].reg;
	t = GPIO_READ(reg);
	t &= ~(LED_MODE_MASK << led_table[led].mode_shift);
	GPIO_WRITE(t, reg);

	return 0;
}

static inline int leds_direction_output(unsigned led, int value)
{
	void __iomem **reg;
	u32 t, s;

	reg = led_table[led].reg;
	s = led_table[led].mode_shift;

	t = GPIO_READ(reg);
	t &= ~(LED_MODE_MASK << s);

	switch (value) {
	case ADM5120_GPIO_LOW:
		t |= (LED_MODE_OUT_LOW << s);
		break;
	case ADM5120_GPIO_FLASH:
	case ADM5120_GPIO_LINK:
	case ADM5120_GPIO_SPEED:
	case ADM5120_GPIO_DUPLEX:
	case ADM5120_GPIO_ACT:
	case ADM5120_GPIO_COLL:
	case ADM5120_GPIO_LINK_ACT:
	case ADM5120_GPIO_DUPLEX_COLL:
	case ADM5120_GPIO_10M_ACT:
	case ADM5120_GPIO_100M_ACT:
		t |= ((value & LED_MODE_MASK) << s);
		break;
	default:
		t |= (LED_MODE_OUT_HIGH << s);
		break;
	}

	GPIO_WRITE(t, reg);

	return 0;
}

static inline int leds_get_value(unsigned led)
{
	void __iomem **reg;
	u32 t, m;

	reg = led_table[led].reg;

	t = GPIO_READ(reg);
	m = (t >> led_table[led].mode_shift) & LED_MODE_MASK;
	if (m == LED_MODE_INPUT)
		return (t >> led_table[led].iv_shift) & 1;

	if (m == LED_MODE_OUT_LOW)
		return 0;

	return 1;
}

/*-------------------------------------------------------------------------*/

/*
 * Main GPIO support routines
 */
int adm5120_gpio_direction_input(unsigned gpio)
{
	if (gpio_is_invalid(gpio))
		return -EINVAL;

	if (gpio < ADM5120_GPIO_P0L0)
		return pins_direction_input(gpio);

	gpio -= ADM5120_GPIO_P0L0;
	return leds_direction_input(gpio);
}
EXPORT_SYMBOL(adm5120_gpio_direction_input);

int adm5120_gpio_direction_output(unsigned gpio, int value)
{
	if (gpio_is_invalid(gpio))
		return -EINVAL;

	if (gpio < ADM5120_GPIO_P0L0)
		return pins_direction_output(gpio, value);

	gpio -= ADM5120_GPIO_P0L0;
	return leds_direction_output(gpio, value);
}
EXPORT_SYMBOL(adm5120_gpio_direction_output);

int adm5120_gpio_get_value(unsigned gpio)
{
	if (gpio < ADM5120_GPIO_P0L0)
		return pins_get_value(gpio);

	gpio -= ADM5120_GPIO_P0L0;
	return leds_get_value(gpio);
}
EXPORT_SYMBOL(adm5120_gpio_get_value);

void adm5120_gpio_set_value(unsigned gpio, int value)
{
	if (gpio < ADM5120_GPIO_P0L0) {
		pins_set_value(gpio, value);
		return;
	}

	gpio -= ADM5120_GPIO_P0L0;
	leds_direction_output(gpio, value);
}
EXPORT_SYMBOL(adm5120_gpio_set_value);

int adm5120_gpio_request(unsigned gpio, const char *label)
{
	if (gpio_is_invalid(gpio))
		return -EINVAL;

	if (gpio_is_used(gpio))
		return -EBUSY;

	adm5120_gpio_map[gpio].flags |= GPIO_FLAG_USED;
	adm5120_gpio_map[gpio].label = label;

	return 0;
}
EXPORT_SYMBOL(adm5120_gpio_request);

void adm5120_gpio_free(unsigned gpio)
{
	if (gpio_is_invalid(gpio))
		return;

	adm5120_gpio_map[gpio].flags &= ~GPIO_FLAG_USED;
	adm5120_gpio_map[gpio].label = NULL;
}
EXPORT_SYMBOL(adm5120_gpio_free);

int adm5120_gpio_to_irq(unsigned gpio)
{
	if (gpio > ADM5120_GPIO_MAX)
		return -EINVAL;

	return adm5120_gpio_map[gpio].irq;
}
EXPORT_SYMBOL(adm5120_gpio_to_irq);

int adm5120_irq_to_gpio(unsigned irq)
{
	int i;

	for (i = 0; i < ADM5120_GPIO_COUNT; i++)
		if (adm5120_gpio_map[i].irq == irq)
			return i;

	return -EINVAL;
}
EXPORT_SYMBOL(adm5120_irq_to_gpio);

/*-------------------------------------------------------------------------*/

void __init adm5120_gpio_csx0_enable(void)
{
	gpio_conf2 |= GPIO_CONF2_CSX0;
	SW_WRITE_REG(SWITCH_REG_GPIO_CONF2, gpio_conf2);

	adm5120_gpio_map[ADM5120_GPIO_PIN1].flags &= ~GPIO_FLAG_VALID;
	adm5120_gpio_map[ADM5120_GPIO_PIN2].irq = ADM5120_IRQ_GPIO2;
}

void __init adm5120_gpio_csx1_enable(void)
{
	gpio_conf2 |= GPIO_CONF2_CSX1;
	SW_WRITE_REG(SWITCH_REG_GPIO_CONF2, gpio_conf2);

	adm5120_gpio_map[ADM5120_GPIO_PIN3].flags &= ~GPIO_FLAG_VALID;
	if (adm5120_package_bga())
		adm5120_gpio_map[ADM5120_GPIO_PIN4].irq = ADM5120_IRQ_GPIO4;
}

void __init adm5120_gpio_ew_enable(void)
{
	gpio_conf2 |= GPIO_CONF2_EW;
	SW_WRITE_REG(SWITCH_REG_GPIO_CONF2, gpio_conf2);

	adm5120_gpio_map[ADM5120_GPIO_PIN0].flags &= ~GPIO_FLAG_VALID;
}

void __init adm5120_gpio_init(void)
{
	int i;

	gpio_conf2 = 0;
	SW_WRITE_REG(SWITCH_REG_GPIO_CONF2, gpio_conf2);

	for (i = 0; i < ADM5120_GPIO_COUNT; i++)
		adm5120_gpio_map[i].flags = GPIO_FLAG_VALID;

	if (adm5120_package_pqfp()) {
		adm5120_gpio_map[ADM5120_GPIO_PIN4].flags &= ~GPIO_FLAG_VALID;
		adm5120_gpio_map[ADM5120_GPIO_PIN5].flags &= ~GPIO_FLAG_VALID;
		adm5120_gpio_map[ADM5120_GPIO_PIN6].flags &= ~GPIO_FLAG_VALID;
		adm5120_gpio_map[ADM5120_GPIO_PIN7].flags &= ~GPIO_FLAG_VALID;
	}
}
