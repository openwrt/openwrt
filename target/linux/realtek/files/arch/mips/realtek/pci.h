/*
 *  Realtek RLX based SoC PCI bus controller initialization
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _REALTEK_PCI_H
#define _REALTEK_PCI_H

#ifdef CONFIG_PCI
void realtek_register_pci(void);
#else
static inline void realtek_register_pci(void) {}
#endif

#endif /* _REALTEK_PCI_H */

