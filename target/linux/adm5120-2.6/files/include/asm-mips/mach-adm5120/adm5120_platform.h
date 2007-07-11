/*
 *  $Id$
 *
 *  ADM5120 specific platform definitions
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
 */

#ifndef _ADM5120_PLATFORM_H_
#define _ADM5120_PLATFORM_H_

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

struct adm5120_flash_platform_data {
	void			(*set_vpp)(struct map_info *, int);
	void			(*switch_bank)(unsigned);
	unsigned int		nr_parts;
	struct mtd_partition	*parts;
};

struct adm5120_switch_platform_data {
	/* TODO: not yet implemented */
};

struct adm5120_pci_irq {
	u8	slot;
	u8	func;
	u8	pin;
	unsigned irq;
};

struct adm5120_pci_platform_data {
	unsigned int	nr_irqs;
	struct adm5120_pci_irq	*irqs;
};

extern struct adm5120_flash_platform_data adm5120_flash0_data;
extern struct adm5120_flash_platform_data adm5120_flash1_data;
extern struct adm5120_pci_platform_data adm5120_pci_data;
extern struct adm5120_switch_platform_data adm5120_switch_data;

extern struct platform_device adm5120_flash0_device;
extern struct platform_device adm5120_flash1_device;
extern struct platform_device adm5120_usbc_device;
extern struct platform_device adm5120_pci_device;
extern struct platform_device adm5120_switch_device;

#endif /* _ADM5120_PLATFORM_H_ */
