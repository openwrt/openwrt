/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "ssdk_init.h"
#include <linux/kconfig.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <generated/autoconf.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include "ssdk_plat.h"
#include <linux/string.h>

sw_error_t hppe_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t *val)
{
	qca_switch_reg_read(dev_id, reg_addr, (a_uint8_t *)val, 4);
	return SW_OK;
}

sw_error_t hppe_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t val) 
{
	qca_switch_reg_write(dev_id, reg_addr, (a_uint8_t *)&val, 4);
	return SW_OK; 
}

sw_error_t hppe_reg_tbl_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t *val, a_uint32_t num)   
{
	a_uint32_t i = 0;
	for(i = 0; i < num; i++) {
		hppe_reg_get(dev_id, (reg_addr + i *4), &val[i]);
	}
	return SW_OK;
}

sw_error_t hppe_reg_tbl_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t *val, a_uint32_t num)   
{
	a_uint32_t i = 0;
	for(i = 0; i < num; i++) {
		hppe_reg_set(dev_id, (reg_addr + i *4), val[i]);
	}
	return SW_OK;   
} 

sw_error_t hppe_uniphy_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t index, a_uint32_t *val)
{
	qca_uniphy_reg_read(dev_id, index, reg_addr, (a_uint8_t *)val, 4);
	return SW_OK;
}

sw_error_t hppe_uniphy_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t index, a_uint32_t val)
{
	qca_uniphy_reg_write(dev_id, index, reg_addr, (a_uint8_t *)&val, 4);
	return SW_OK;
}

