/*
 * Copyright (C) 2006 OpenWrt.org
 *
 * This is free software, licensed under the GNU General Public License v2.
 * See /LICENSE for more information.
 */

#ifndef __GPIO_H
#define __GPIO_H

#ifdef CONFIG_BCM47XX
#include <linux/gpio.h>
#else
#warning "Unsupported configuration."

#define bcm47xx_gpio_in(mask)			(-1U)
#define bcm47xx_gpio_out(mask, value)		(-1U)
#define bcm47xx_gpio_outen(mask, value)		(-1U)
#define bcm47xx_gpio_control(mask, value)	(-1U)
#define bcm47xx_gpio_intmask(mask, value)	(-1U)
#define bcm47xx_gpio_polarity(mask, value)	(-1U)

#endif

#endif /* __GPIO_H */
