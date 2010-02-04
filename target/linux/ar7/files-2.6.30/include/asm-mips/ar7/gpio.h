/*
 * Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __AR7_GPIO_H__
#define __AR7_GPIO_H__
#include <asm/ar7/ar7.h>
#ifndef __AR7_TITAN_H__
#include <asm/ar7/titan.h>
#endif

#define AR7_GPIO_MAX 32
#define TITAN_GPIO_MAX 51

extern int gpio_request(unsigned gpio, const char *label);
extern void gpio_free(unsigned gpio);

/* Common GPIO layer */
static inline int gpio_get_value_ar7(unsigned gpio)
{
	void __iomem *gpio_in =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + AR7_GPIO_INPUT);

	return readl(gpio_in) & (1 << gpio);
}

static inline int gpio_get_value_titan(unsigned gpio)
{
	void __iomem *gpio_in0 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_INPUT_0);
	void __iomem *gpio_in1 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_INPUT_1);

	return readl(gpio >> 5 ? gpio_in1 : gpio_in0) & (1 << (gpio & 0x1f));
}

static inline int gpio_get_value(unsigned gpio)
{
	return ar7_is_titan() ? gpio_get_value_titan(gpio) :
		gpio_get_value_ar7(gpio);
}

static inline void gpio_set_value_ar7(unsigned gpio, int value)
{
	void __iomem *gpio_out =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + AR7_GPIO_OUTPUT);
	unsigned tmp;

	tmp = readl(gpio_out) & ~(1 << gpio);
	if (value)
		tmp |= 1 << gpio;
	writel(tmp, gpio_out);
}

static inline void gpio_set_value_titan(unsigned gpio, int value)
{
	void __iomem *gpio_out0 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_OUTPUT_0);
	void __iomem *gpio_out1 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_OUTPUT_1);
	unsigned tmp;

	tmp = readl(gpio >> 5 ? gpio_out1 : gpio_out0) & ~(1 << (gpio & 0x1f));
	if (value)
		tmp |= 1 << (gpio & 0x1f);
	writel(tmp, gpio >> 5 ? gpio_out1 : gpio_out0);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if (ar7_is_titan())
		gpio_set_value_titan(gpio, value);
	else
		gpio_set_value_ar7(gpio, value);
}

static inline int gpio_direction_input_ar7(unsigned gpio)
{
	void __iomem *gpio_dir =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + AR7_GPIO_DIR);

	if (gpio >= AR7_GPIO_MAX)
		return -EINVAL;

	writel(readl(gpio_dir) | (1 << gpio), gpio_dir);

	return 0;
}

static inline int gpio_direction_input_titan(unsigned gpio)
{
	void __iomem *gpio_dir0 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_DIR_0);
	void __iomem *gpio_dir1 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_DIR_1);

	if (gpio >= TITAN_GPIO_MAX)
		return -EINVAL;

	writel(readl(gpio >> 5 ? gpio_dir1 : gpio_dir0) | (1 << (gpio & 0x1f)),
		gpio >> 5 ? gpio_dir1 : gpio_dir0);

	return 0;
}

static inline int gpio_direction_input(unsigned gpio)
{
	return ar7_is_titan() ?  gpio_direction_input_titan(gpio) :
		gpio_direction_input_ar7(gpio);
}

static inline int gpio_direction_output_ar7(unsigned gpio, int value)
{
	void __iomem *gpio_dir =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + AR7_GPIO_DIR);

	if (gpio >= AR7_GPIO_MAX)
		return -EINVAL;

	gpio_set_value(gpio, value);
	writel(readl(gpio_dir) & ~(1 << gpio), gpio_dir);

	return 0;
}

static inline int gpio_direction_output_titan(unsigned gpio, int value)
{
	void __iomem *gpio_dir0 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_DIR_0);
	void __iomem *gpio_dir1 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_DIR_1);

	if (gpio >= TITAN_GPIO_MAX)
		return -EINVAL;

	gpio_set_value_titan(gpio, value);
	writel(readl(gpio >> 5 ? gpio_dir1 : gpio_dir0) & ~(1 <<
		(gpio & 0x1f)), gpio >> 5 ? gpio_dir1 : gpio_dir0);

	return 0;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	return ar7_is_titan() ?  gpio_direction_output_titan(gpio, value) :
		gpio_direction_output_ar7(gpio, value);
}

static inline int gpio_to_irq(unsigned gpio)
{
	return -EINVAL;
}

static inline int irq_to_gpio(unsigned irq)
{
	return -EINVAL;
}

/* Board specific GPIO functions */
static inline int ar7_gpio_enable_ar7(unsigned gpio)
{
	void __iomem *gpio_en =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + AR7_GPIO_ENABLE);

	writel(readl(gpio_en) | (1 << gpio), gpio_en);

	return 0;
}

static inline int ar7_gpio_enable_titan(unsigned gpio)
{
	void __iomem *gpio_en0 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_ENBL_0);
	void __iomem *gpio_en1 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_ENBL_1);

	writel(readl(gpio >> 5 ? gpio_en1 : gpio_en0) | (1 << (gpio & 0x1f)),
		gpio >> 5 ? gpio_en1 : gpio_en0);

	return 0;
}

static inline int ar7_gpio_enable(unsigned gpio)
{
	return ar7_is_titan() ? ar7_gpio_enable_titan(gpio) :
		ar7_gpio_enable_ar7(gpio);
}

static inline int ar7_gpio_disable_ar7(unsigned gpio)
{
	void __iomem *gpio_en =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + AR7_GPIO_ENABLE);

	writel(readl(gpio_en) & ~(1 << gpio), gpio_en);

	return 0;
}

static inline int ar7_gpio_disable_titan(unsigned gpio)
{
	void __iomem *gpio_en0 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_ENBL_0);
	void __iomem *gpio_en1 =
		(void __iomem *)KSEG1ADDR(AR7_REGS_GPIO + TITAN_GPIO_ENBL_1);

	writel(readl(gpio >> 5 ? gpio_en1 : gpio_en0) & ~(1 << (gpio & 0x1f)),
		gpio >> 5 ? gpio_en1 : gpio_en0);

	return 0;
}

static inline int ar7_gpio_disable(unsigned gpio)
{
	return ar7_is_titan() ? ar7_gpio_disable_titan(gpio) :
		ar7_gpio_disable_ar7(gpio);
}

static inline int ar7_init_titan_variant( void )
{
	/*UINT32 new_val;*/
	unsigned new_val;

	/* set GPIO 44 - 47 as input */
	/*PAL_sysGpioCtrl(const int, GPIO_PIN, GPIO_INPUT_PIN); */
	/*define titan_gpio_ctrl in titan.h*/
	titan_gpio_ctrl(44, GPIO_PIN, GPIO_INPUT_PIN);
	titan_gpio_ctrl(45, GPIO_PIN, GPIO_INPUT_PIN);
	titan_gpio_ctrl(46, GPIO_PIN, GPIO_INPUT_PIN);
	titan_gpio_ctrl(47, GPIO_PIN, GPIO_INPUT_PIN);
    
	/* read GPIO to get Titan variant type */
	/*fix this*/
	titan_sysGpioInValue( &new_val, 1 );

	new_val >>= 12;
	new_val &= 0x0f;

	switch ( new_val )
	{
	case TITAN_CHIP_1050:
	case TITAN_CHIP_1055:
	case TITAN_CHIP_1056:
	case TITAN_CHIP_1060:
		return new_val;
 
	default:
		break;
	}
	/* In case we get an invalid value, return the default Titan chip */
	return TITAN_CHIP_1050;
}

#include <asm-generic/gpio.h>

#endif
