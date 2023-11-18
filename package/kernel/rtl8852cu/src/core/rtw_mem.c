/******************************************************************************
 *
 * Copyright(c) 2016 - 2022 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include <linux/kernel.h>
#include <drv_types.h>
#include <rtw_mem.h>

extern int rtkm_prealloc_init(void);
extern void rtkm_prealloc_destroy(void);

static int __init rtw_mem_init(void)
{
	rtkm_prealloc_init();
	return 0;
}

static void __exit rtw_mem_exit(void)
{
	rtkm_prealloc_destroy();
}

module_init(rtw_mem_init);
module_exit(rtw_mem_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek Wireless Lan Driver");
MODULE_AUTHOR("Realtek Semiconductor Corp.");
MODULE_VERSION("DRIVERVERSION");
