/*
 * Copyright (C) 2006 OpenWrt.org
 *
 * This is free software, licensed under the GNU General Public License v2.
 * See /LICENSE for more information.
 */

#ifndef __GPIO_H
#define __GPIO_H

#ifdef BROADCOM
#include "gpio-bcm947xx.h"
#else
#error "Unsupported configuration."
#endif

#endif /* __GPIO_H */
