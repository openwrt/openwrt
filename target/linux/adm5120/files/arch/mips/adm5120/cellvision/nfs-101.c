/*
 *  Cellvision/SparkLAN NFS-101U/WU support
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include "cellvision.h"

static u8 nfs101_vlans[6] __initdata = { /* TODO: not tested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

static void __init nfs101_setup(void)
{
	cas6xx_flash_setup();
	cellvision_mac_setup();

	adm5120_add_device_uart(0);
	adm5120_add_device_uart(1);
	adm5120_add_device_switch(5, nfs101_vlans);
}

ADM5120_BOARD(MACH_ADM5120_NFS101U, "Cellvision NFS-101U/101WU", nfs101_setup);
