/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2009 Hauke Mehrtens <hauke@hauke-m.de>
 */


#include <linux/platform_device.h>
#include <asm/amazon/irq.h>

#define AMAZON_FLASH_START 0x13000000
#define AMAZON_FLASH_MAX 0x1000000

static struct platform_device amazon_mii = {
	.id = 0,
	.name = "amazon_mii0",
//	.dev = {
//		.platform_data = amazon_ethaddr,
//	}
};

static struct platform_device amazon_wdt = {
	.id = 0,
	.name = "amazon_wdt",
};

static struct platform_device amazon_asc = {
	.id = 0,
	.name = "amazon_asc",
};

static struct resource amazon_mtd_resource = {
	.start  = AMAZON_FLASH_START,
	.end    = AMAZON_FLASH_START + AMAZON_FLASH_MAX - 1,
	.flags  = IORESOURCE_MEM,
};

static struct platform_device amazon_mtd = {
	.id = 0,
	.name = "amazon_mtd",
	.num_resources  = 1,
	.resource   = &amazon_mtd_resource,
};


struct platform_device *amazon_devs[] = {
	&amazon_mii, &amazon_mtd, &amazon_wdt, &amazon_asc
};


int __init amazon_init_devices(void)
{
	printk(KERN_INFO "");
	return platform_add_devices(amazon_devs, 4);
}

arch_initcall(amazon_init_devices);
