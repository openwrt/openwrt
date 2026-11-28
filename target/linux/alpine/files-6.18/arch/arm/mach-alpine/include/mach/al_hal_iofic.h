/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @defgroup group_interrupts Common I/O Fabric Interrupt Controller
 * This HAL provides the API for programming the Common I/O Fabric Interrupt
 * Controller (IOFIC) found in most of the units attached to the I/O Fabric of
 * Alpine platform
 *  @{
 * @file   al_hal_iofic.h
 *
 * @brief Header file for the interrupt controller that's embedded in various units
 *
 */

#ifndef __AL_HAL_IOFIC_H__
#define __AL_HAL_IOFIC_H__

#include <mach/al_hal_common.h>

#define AL_IOFIC_MAX_GROUPS	4

/*
 * Configurations
 */

/**
 * Configure the interrupt controller registers, actual interrupts are still
 * masked at this stage.
 *
 * @param regs_base regs pointer to interrupt controller registers
 * @param group the interrupt group.
 * @param flags flags of Interrupt Control Register
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_iofic_config(void __iomem *regs_base, int group,
		   uint32_t flags);

/**
 * configure the moderation timer resolution for a given group
 * Applies for both msix and legacy mode.
 *
 * @param regs_base pointer to unit registers
 * @param group the interrupt group
 * @param resolution resolution of the timer interval, the resolution determines the rate
 * of decrementing the interval timer, setting value N means that the interval
 * timer will be decremented each (N+1) * (0.68) micro seconds.
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_iofic_moder_res_config(void __iomem *regs_base, int group,
			     uint8_t resolution);

/**
 * configure the moderation timer interval for a given msix vector
 *
 * @param regs_base pointer to unit registers
 * @param group the interrupt group
 * @param vector vector index
 * @param interval interval between interrupts, 0 disable
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_iofic_msix_moder_interval_config(void __iomem *regs_base, int group,
				       uint8_t vector, uint8_t interval);

/**
 * return the offset of the unmask register for a given group.
 * this function can be used when the upper layer wants to directly
 * access the unmask regiter and bypass the al_iofic_unmask() API.
 *
 * @param regs_base regs pointer to unit registers
 * @param group the interrupt group
 * @return the offset of the unmask register.
 */
uint32_t __iomem * al_iofic_unmask_offset_get(void __iomem *regs_base, int group);

/**
 * unmask specific interrupts for a given group
 * this functions guarantees atomic operations, it is performance optimized as
 * it will not require read-modify-write. The unmask done using the interrupt
 * mask clear register, so it's safe to call it while the mask is changed by
 * the HW (auto mask) or another core.
 *
 * @param regs_base pointer to unit registers
 * @param group the interrupt group
 * @param mask bitwise of interrupts to unmask, set bits will be unmasked.
 */
void al_iofic_unmask(void __iomem *regs_base, int group, uint32_t mask);

/**
 * mask specific interrupts for a given group
 * this functions modifies interrupt mask register, the callee must make sure
 * the mask is not changed by another cpu.
 *
 * @param regs_base pointer to unit registers
 * @param group the interrupt group
 * @param mask bitwise of interrupts to mask, set bits will be masked.
 */
void al_iofic_mask(void __iomem *regs_base, int group, uint32_t mask);

/**
 * read interrupt cause register for a given group
 * this will clear the set bits if the Clear on Read mode enabled.
 * @param regs_base pointer to unit registers
 * @param group the interrupt group
 */
uint32_t al_iofic_read_cause(void __iomem *regs_base, int group);

/**
 * unmask specific interrupts from aborting the udma a given group
 *
 * @param regs_base pointer to unit registers
 * @param group the interrupt group
 * @param mask bitwise of interrupts to mask
 */
void al_iofic_abort_mask(void __iomem *regs_base, int group, uint32_t mask);

#endif
/** @} end of interrupt controller group */
