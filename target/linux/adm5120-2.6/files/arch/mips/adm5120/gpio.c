/*
 *  $Id$
 *
 *  ADM5120 GPIO support
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/gpio.h>

#include <asm/mach-adm5120/adm5120_defs.h>
#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_switch.h>

typedef void __iomem *	gpio_reg_t;
#define GPIO_READ(r)	readl((r))
#define GPIO_WRITE(v,r)	writel((v),(r))
#define GPIO_REG(r)	(gpio_reg_t)(KSEG1ADDR(ADM5120_SWITCH_BASE)+r)

struct adm5120_gpio_line {
	u32 flags;
	const char *label;
};

#define GPIO_FLAG_VALID		0x01
#define GPIO_FLAG_USED		0x02

struct led_desc {
	gpio_reg_t reg;	/* LED register address */
	u8 iv_shift;	/* shift amount for input bit */
	u8 mode_shift;	/* shift amount for mode bits */
};

#define LED_DESC(_port,_led) { \
		.reg = GPIO_REG(SWITCH_REG_PORT0_LED+_port*4), \
		.iv_shift = LED0_IV_SHIFT+_led, \
		.mode_shift = _led*4 \
	}

static struct led_desc led_table[15] = {
	LED_DESC(0, 0), LED_DESC(0, 1), LED_DESC(0, 2),
	LED_DESC(1, 0), LED_DESC(1, 1), LED_DESC(1, 2),
	LED_DESC(2, 0), LED_DESC(2, 1), LED_DESC(2, 2),
	LED_DESC(3, 0), LED_DESC(3, 1), LED_DESC(3, 2),
	LED_DESC(4, 0), LED_DESC(4, 1), LED_DESC(4, 2)
};

static struct adm5120_gpio_line adm5120_gpio_map[ADM5120_GPIO_COUNT] = {
	[ADM5120_GPIO_PIN0] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN1] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN2] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN3] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN4] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN5] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN6] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_PIN7] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P0L0] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P0L1] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P0L2] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P1L0] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P1L1] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P1L2] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P2L0] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P2L1] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P2L2] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P3L0] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P3L1] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P3L2] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P4L0] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P4L1] = {.flags = GPIO_FLAG_VALID},
	[ADM5120_GPIO_P4L2] = {.flags = GPIO_FLAG_VALID}
};

#define gpio_is_invalid(g) ( \
		(g) > ADM5120_GPIO_MAX || \
		((adm5120_gpio_map[(g)].flags & GPIO_FLAG_VALID) == 0) \
	)

#define gpio_is_used(g)	((adm5120_gpio_map[(g)].flags & GPIO_FLAG_USED) != 0)

/*
 * Helpers for GPIO lines in GPIO_CONF0 register
 */
#define PIN_IM(p)	((1 << GPIO_CONF0_IM_SHIFT) << p)
#define PIN_IV(p)	((1 << GPIO_CONF0_IV_SHIFT) << p)
#define PIN_OE(p)	((1 << GPIO_CONF0_OE_SHIFT) << p)
#define PIN_OV(p)	((1 << GPIO_CONF0_OV_SHIFT) << p)

static inline int pins_direction_input(unsigned pin)
{
	gpio_reg_t *reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	t &= ~(PIN_OE(pin));
	t |= PIN_IM(pin);
	GPIO_WRITE(t,reg);

	return 0;
}

static inline int pins_direction_output(unsigned pin, int value)
{
	gpio_reg_t *reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	t &= ~(PIN_IM(pin) | PIN_OV(pin));
	t |= PIN_OE(pin);

	if (value)
		t |= PIN_OV(pin);

	GPIO_WRITE(t,reg);

	return 0;
}

static inline int pins_get_value(unsigned pin)
{
	gpio_reg_t *reg;
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
	gpio_reg_t *reg;
	u32 t;

	reg = GPIO_REG(SWITCH_REG_GPIO_CONF0);

	t = GPIO_READ(reg);
	if (value == 0)
		t &= ~(PIN_OV(pin));
	else
		t |= PIN_OV(pin);

	GPIO_WRITE(t,reg);
}

/*
 * Helpers for GPIO lines in PORTx_LED registers
 */
static inline int leds_direction_input(unsigned led)
{
	gpio_reg_t *reg;
	u32 t;

	reg = led_table[led].reg;
	t = GPIO_READ(reg);
	t &= ~(LED_MODE_MASK << led_table[led].mode_shift);
	GPIO_WRITE(t,reg);

	return 0;
}

static inline int leds_direction_output(unsigned led, int value)
{
	gpio_reg_t *reg;
	u32 t, s;

	reg = led_table[led].reg;
	s = led_table[led].mode_shift;

	t = GPIO_READ(reg);
	t &= ~(LED_MODE_MASK << s);
	if (value)
		t |= (LED_MODE_OUT_HIGH << s);
	else
		t |= (LED_MODE_OUT_LOW << s);

	GPIO_WRITE(t,reg);

	return 0;
}

static inline int leds_get_value(unsigned led)
{
	gpio_reg_t *reg;
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

static inline void leds_set_value(unsigned led, int value)
{
	gpio_reg_t *reg;
	u32 s,t;

	reg = led_table[led].reg;
	s = led_table[led].mode_shift;

	t = GPIO_READ(reg);
	t &= ~(LED_MODE_MASK << s);
	if (value)
		t |= (LED_MODE_OUT_HIGH << s);
	else
		t |= (LED_MODE_OUT_LOW << s);

	GPIO_WRITE(t,reg);
}

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

int adm5120_gpio_direction_output(unsigned gpio, int value)
{
	if (gpio_is_invalid(gpio))
		return -EINVAL;

	if (gpio < ADM5120_GPIO_P0L0)
		return pins_direction_output(gpio, value);

	gpio -= ADM5120_GPIO_P0L0;
	return leds_direction_output(gpio, value);
}

int adm5120_gpio_get_value(unsigned gpio)
{
	if (gpio < ADM5120_GPIO_P0L0)
		return pins_get_value(gpio);

	gpio -= ADM5120_GPIO_P0L0;
	return leds_get_value(gpio);
}

void adm5120_gpio_set_value(unsigned gpio, int value)
{
	if (gpio < ADM5120_GPIO_P0L0) {
		pins_set_value(gpio, value);
		return;
	}

	gpio -= ADM5120_GPIO_P0L0;
	leds_set_value(gpio, value);
}

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

void adm5120_gpio_free(unsigned gpio)
{
	if (gpio_is_invalid(gpio))
		return;

	adm5120_gpio_map[gpio].flags &= ~GPIO_FLAG_USED;
	adm5120_gpio_map[gpio].label = NULL;
}

int adm5120_gpio_to_irq(unsigned gpio)
{
	/* FIXME: not yet implemented */
	return -EINVAL;
}

int adm5120_irq_to_gpio(unsigned irq)
{
	/* FIXME: not yet implemented */
	return -EINVAL;
}

static int __init adm5120_gpio_init(void)
{
	int i;

	if (adm5120_package_pqfp()) {
		/* GPIO pins 4-7 are unavailable in ADM5120P */
		for (i=ADM5120_GPIO_PIN4; i<=ADM5120_GPIO_PIN7; i++)
			adm5120_gpio_map[i].flags &= ~GPIO_FLAG_VALID;
	}

	return 0;
}

pure_initcall(adm5120_gpio_init);

EXPORT_SYMBOL(adm5120_gpio_direction_output);
EXPORT_SYMBOL(adm5120_gpio_direction_input);
EXPORT_SYMBOL(adm5120_gpio_get_value);
EXPORT_SYMBOL(adm5120_gpio_set_value);
EXPORT_SYMBOL(adm5120_gpio_request);
EXPORT_SYMBOL(adm5120_gpio_free);
EXPORT_SYMBOL(adm5120_gpio_to_irq);
EXPORT_SYMBOL(adm5120_irq_to_gpio);
