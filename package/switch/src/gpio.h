/*
 * Copyright (C) 2006 OpenWrt.org
 *
 * This is free software, licensed under the GNU General Public License v2.
 * See /LICENSE for more information.
 */

#ifndef __GPIO_H
#define __GPIO_H

#ifdef CONFIG_BCM47XX
#include "gpio-bcm947xx.h"
#else
#warning "Unsupported configuration."

#define gpio_in()			(-1U)
#define gpio_out(mask, value)		(-1U)
#define gpio_outen(mask, value)		(-1U)
#define gpio_control(mask, value)	(-1U)
#define gpio_intmask(mask, value)	(-1U)
#define gpio_intpolarity(mask, value)	(-1U)

#endif

#endif /* __GPIO_H */
