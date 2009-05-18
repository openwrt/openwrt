/*
 * Common utility code for GTA02
 *
 * Copyright (C) 2008 by Openmoko, Inc.
 * Author: Holger Hans Peter Freyther <freyther@openmoko.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <linux/module.h>
#include <linux/io.h>
#include <linux/irq.h>

#include <asm/gpio.h>
#include <mach/regs-gpio.h>
#include <linux/gta02-shadow.h>

/**
 * Shadow GPIO bank B handling. For the LEDs we need to keep track of the state
 * in software. The s3c2410_gpio_setpin must not be used for GPIOs on bank B
 */
static unsigned long gpb_mask;
static unsigned long gpb_state;

void gta02_gpb_add_shadow_gpio(unsigned int gpio)
{
	unsigned long offset = S3C2410_GPIO_OFFSET(gpio);
	unsigned long flags;

	local_irq_save(flags);
	gpb_mask |= 1L << offset;
	local_irq_restore(flags);
}
EXPORT_SYMBOL(gta02_gpb_add_shadow_gpio);

static void set_shadow_gpio(unsigned long offset, unsigned int value)
{
	unsigned long state = value != 0;

	gpb_state &= ~(1L << offset);
	gpb_state |= state << offset;
}

void gta02_gpb_setpin(unsigned int pin, unsigned to)
{
	void __iomem *base = S3C24XX_GPIO_BASE(S3C2410_GPB0);
	unsigned long offset = S3C2410_GPIO_OFFSET(pin);
	unsigned long flags;
	unsigned long dat;

	BUG_ON(base != S3C24XX_GPIO_BASE(pin));

	local_irq_save(flags);
	dat = __raw_readl(base + 0x04);

	/* Add the shadow values */
	dat &= ~gpb_mask;
	dat |= gpb_state;

	/* Do the operation like s3c2410_gpio_setpin */
	dat &= ~(1L << offset);
	dat |= to << offset;

	/* Update the shadow state */
	if ((1L << offset) & gpb_mask)
		set_shadow_gpio(offset, to);

	__raw_writel(dat, base + 0x04);
	local_irq_restore(flags);
}
EXPORT_SYMBOL(gta02_gpb_setpin);
