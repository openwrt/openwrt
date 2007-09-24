/*
 *  $Id$
 *
 *  ADM5120 board definitions
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
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

#ifndef _ADM5120_BOARD_H_
#define _ADM5120_BOARD_H_

#include <linux/init.h>
#include <linux/list.h>

#define ADM5120_BOARD_NAMELEN	64

struct adm5120_board {
	unsigned long		mach_type;
	char			name[ADM5120_BOARD_NAMELEN];

	void			(*board_setup)(void);
	void			(*board_reset)(void);

	unsigned int		eth_num_ports;
	unsigned char		*eth_vlans;
	unsigned int		num_devices;
	struct platform_device	**devices;
	unsigned int		pci_nr_irqs;
	struct adm5120_pci_irq	*pci_irq_map;

	struct list_head	list;
};

extern void adm5120_board_register(struct adm5120_board *) __init;

#endif /* _ADM5120_BOARD_H_ */
