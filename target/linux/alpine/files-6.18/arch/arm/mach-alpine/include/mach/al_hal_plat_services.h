/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2015 Annapurna Labs Ltd. */

/**
 * @defgroup group_services Platform Services API
 *  Platform Services API
 *  @{
 * @file   al_hal_plat_services.h
 *
 * @brief  API for Platform services provided for to HAL drivers
 *
 *
 */

#ifndef __PLAT_SERVICES_H__
#define __PLAT_SERVICES_H__

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/printk.h>
#include <linux/delay.h>

#include <linux/errno.h>

#define al_reg_read16(l)	readw(l)
#define al_reg_read32(l)	readl(l)

#define al_reg_write16(l,v)	writew(v,l)
#define al_reg_write32(l,v)	writel(v,l)

#define al_reg_write32_relaxed(l,v)	writel_relaxed(v,l)

/**
 * print error message
 *
 * @param format
 */
#define al_err(...)			pr_err(__VA_ARGS__)

/**
 * print warning message
 *
 * @param format
 */
#define al_warn(...)			pr_info(__VA_ARGS__)

/**
 * print info message
 *
 * @param format
 */
#define al_info(...)			pr_info(__VA_ARGS__)

/**
 * print debug message
 *
 * @param format
 */
#define al_dbg(...)			pr_debug(__VA_ARGS__)

/**
 * Assertion
 *
 * @param condition
 */
#define al_assert(COND)		BUG_ON(!(COND))

/**
  * Make sure data will be visible by DMA masters. usually this is achieved by
  * the ARM DMB instruction.
  */

static inline void al_local_data_memory_barrier(void)
{
	mb();
}

/**
 * al_udelay - micro sec delay
 * @param micro seconds to delay
 */
#define al_udelay(u)    udelay(u)

#define al_msleep(m)		msleep(m)

#define swap32_to_le(x)		cpu_to_le32(x)
#define swap64_to_le(x)		cpu_to_le64(x)
#define swap32_from_le(x)	le32_to_cpu(x)

/**
 * Memory set
 *
 * @param memory pointer
 * @param value for setting
 * @param number of bytes to set
 */
#define al_memset(p, val, cnt)  memset(p, val, cnt)

/**
 * memory compare
 *
 * @param  p1 memory pointer
 * @param  p2 memory pointer
 * @param  cnt number of bytes to compare
 *
 * @return 0 if equal, else otherwise
 */
#define al_memcmp(p1, p2, cnt)  memcmp(p1, p2, cnt)

/**
 * memory copy
 *
 * @param  dest memory pointer to destination
 * @param  src memory pointer to source
 * @param  cnt number of bytes to copy
 */
#define al_memcpy(dest, src, cnt)	memcpy(dest, src, cnt)

/** @} end of Platform Services API group */
#endif				/* __PLAT_SERVICES_H__ */
