/*
 *  Copyright (C) 2005, Broadcom Corporation
 *  Copyright (C) 2006, Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2014 Hauke Mehrtens <hauke@hauke-m.de>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#ifndef __BCM47XX_NVRAM_H
#define __BCM47XX_NVRAM_H

#include <linux/types.h>
#include <linux/kernel.h>

#ifdef CONFIG_BCM47XX
int bcm47xx_nvram_getenv(const char *name, char *val, size_t val_len);

int bcm47xx_nvram_gpio_pin(const char *name);
#else
static inline int bcm47xx_nvram_getenv(const char *name, char *val,
				       size_t val_len)
{
	return -ENXIO;
}

static inline int bcm47xx_nvram_gpio_pin(const char *name)
{
	return -ENXIO;
}
#endif
#endif /* __BCM47XX_NVRAM_H */
