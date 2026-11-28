/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @defgroup group_common HAL Common Layer
 *  @{
 * @file   al_hal_reg_utils.h
 *
 * @brief  Register utilities used by HALs and platform layer
 *
 *
 */

#ifndef __AL_HAL_REG_UTILS_H__
#define __AL_HAL_REG_UTILS_H__

#include <mach/al_hal_plat_types.h>
#include <mach/al_hal_plat_services.h>

#define AL_BIT(b)	(1UL << (b))

#define AL_ADDR_LOW(x)	((uint32_t)((al_phys_addr_t)(x)))
#define AL_ADDR_HIGH(x)	((uint32_t)((((al_phys_addr_t)(x)) >> 16) >> 16))

/** get field out of 32 bit register */
#define AL_REG_FIELD_GET(reg, mask, shift)  (((reg) & (mask)) >> (shift))

/** set field of 32 bit register */
#define AL_REG_FIELD_SET(reg, mask, shift, val)			\
	(reg) =							\
		(((reg) & (~(mask))) |				\
		((((unsigned)(val)) << (shift)) & (mask)))

/** get single bit out of 32 bit register */
#define AL_REG_BIT_GET(reg, shift)				\
	AL_REG_FIELD_GET(reg, AL_BIT(shift), shift)

#define AL_REG_BITS_FIELD(shift, val)				\
		(((unsigned)(val)) << (shift))

/** set single bit field of 32 bit register to a given value */
#define AL_REG_BIT_VAL_SET(reg, shift, val)				\
	AL_REG_FIELD_SET(reg, AL_BIT(shift), shift, val)

/** set single bit of 32 bit register to 1 */
#define AL_REG_BIT_SET(reg, shift)				\
	AL_REG_BIT_VAL_SET(reg, shift, 1)

#define AL_BIT_MASK(n)						\
	(AL_BIT(n) - 1)

#define AL_FIELD_MASK(msb, lsb)					\
	(AL_BIT(msb) + AL_BIT_MASK(msb) - AL_BIT_MASK(lsb))

/** clear bits specified by clear_mask */
#define AL_REG_MASK_CLEAR(reg, clear_mask)			\
	((reg) = (((reg) & (~(clear_mask)))))

/** set bits specified by clear_mask */
#define AL_REG_MASK_SET(reg, clear_mask)			\
	((reg) = (((reg) | (clear_mask))))

#define AL_ALIGN_UP(val, size)					\
	((size) * (((val) + (size) - 1) / (size)))

/** take bits selected by mask from one data, the rest from background */
#define AL_MASK_VAL(mask, data, background)		\
	(((mask) & (data)) | ((~mask) & (background)))

/**
 * 32 bits register masked write
 *
 * @param	reg
 *	register address
 * @param	mask
 *	bits not selected (1) by mask will be left unchanged
 * @param	data
 *	data to write. bits not selected by mask ignored.
 */
static inline void al_reg_write32_masked(uint32_t __iomem *reg, uint32_t mask
								, uint32_t data)
{
	uint32_t temp;
	temp = al_reg_read32(reg);
	al_reg_write32(reg, AL_MASK_VAL(mask, data, temp));
}

/** @} end of Common group */
#endif
