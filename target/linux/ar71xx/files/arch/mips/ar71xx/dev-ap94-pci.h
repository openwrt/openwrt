/*
 *  Atheros AP94 reference board PCI initialization
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_AP94_PCI_H
#define _AR71XX_DEV_AP94_PCI_H

#include <linux/spi/flash.h>

#if defined(CONFIG_AR71XX_DEV_AP94_PCI)
void ap94_pci_init(u8 *cal_data0, u8 *mac_addr0,
		   u8 *cal_data1, u8 *mac_addr1) __init;
#else
static inline void ap94_pci_init(u8 *cal_data0, u8 *mac_addr0,
				 u8 *cal_data1, u8 *mac_addr1)
{
}
#endif

#endif /* _AR71XX_DEV_AP94_PCI_H */

