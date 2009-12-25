/*
 *  Atheros PB42 reference board PCI initialization
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_PB42_PCI_H
#define _AR71XX_DEV_PB42_PCI_H

#if defined(CONFIG_AR71XX_DEV_PB42_PCI)
void pb42_pci_init(void) __init;
#else
static inline void pb42_pci_init(void) { }
#endif

#endif /* _AR71XX_DEV_PB42_PCI_H */
