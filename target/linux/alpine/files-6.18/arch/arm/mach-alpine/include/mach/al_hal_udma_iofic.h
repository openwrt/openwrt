/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @defgroup group_udma_interrupts UDMA I/O Fabric Interrupt Controller
 * @ingroup group_udma_api
 *  UDMA IOFIC API
 *  @{
 * @file   al_hal_udma_iofic.h
 *
 * @brief C Header file for programming the interrupt controller that found
 * in UDMA based units. These APIs rely and use some the Interrupt controller
 * API under al_hal_iofic.h
 */

#ifndef __AL_HAL_UDMA_IOFIC_H__
#define __AL_HAL_UDMA_IOFIC_H__

#include <mach/al_hal_common.h>
#include <mach/al_hal_iofic.h>
#include <mach/al_hal_udma_regs.h>

/**
 * Interrupt Mode
 * This is the interrupt mode for the primary interrupt level The secondary
 * interrupt level does not have mode and it is always a level sensitive
 * interrupt that is reflected in group D of the primary.
 */
enum al_iofic_mode {
	AL_IOFIC_MODE_LEGACY, /**< level-sensitive interrupt wire */
	AL_IOFIC_MODE_MSIX_PER_Q, /**< per UDMA queue MSI-X interrupt */
	AL_IOFIC_MODE_MSIX_PER_GROUP
};

/** interrupt controller level (primary/secondary) */
enum al_udma_iofic_level {
	AL_UDMA_IOFIC_LEVEL_PRIMARY,
	AL_UDMA_IOFIC_LEVEL_SECONDARY
};

/*
 * The next four groups represents the standard 4 groups in the primary
 * interrupt controller of each bus-master unit in the I/O Fabric.
 * The first two groups can be used when accessing the secondary interrupt
 * controller as well.
 */
#define AL_INT_GROUP_A		0 /**< summary of the below events */
#define AL_INT_GROUP_B		1 /**< RX completion queues */
#define AL_INT_GROUP_C		2 /**< TX completion queues */
#define AL_INT_GROUP_D		3 /**< Misc */

/*******************************************************************************
 * Primary interrupt controller, group A bits
 ******************************************************************************/
/* Group A bits which are just summary bits of GROUP B, C and D */
#define AL_INT_GROUP_A_GROUP_B_SUM	AL_BIT(0)
#define AL_INT_GROUP_A_GROUP_C_SUM	AL_BIT(1)
#define AL_INT_GROUP_A_GROUP_D_SUM	AL_BIT(2)

/*
 * Until this point, all description above is for Groups A/B/C/D in the PRIMARY
 * Interrupt controller.
 * Following are definitions related to the secondary interrupt controller with
 * two cause registers (group A and group B) that covers UDMA M2S/S2M errors.
 * Secondary interrupt controller summary bits are not mapped to the Processor
 * GIC directly, rather they are represented in Group D of the primary interrupt
 * controller.
 */

/******************************************************************************
 * Secondary interrupt Controller, Group A, which holds the TX (M2S) error
 * interrupt bits
 ******************************************************************************/

/*******************************************************************************
 * Secondary interrupt Controller, Group B, which holds the RX (S2M) error
 * interrupt bits
 ******************************************************************************/

/*******************************************************************************
 * Configurations
 ******************************************************************************/

/**
 * Configure the UDMA interrupt controller registers, interrupts will are kept
 * masked.
 * This is a static setting that should be called while initialized the
 * interrupt controller within a given UDMA, and should not be modified during
 * runtime unless the UDMA is completely disabled. The first argument sets the
 * interrupt and MSIX modes. The m2s/s2m errors/abort are a set of bit-wise
 * masks to define the behaviour of the UDMA once an error happens: The _abort
 * will put the UDMA in abort state once an error happens The _error bitmask
 * will indicate and error in the secondary cause register but will not abort.
 * The bit-mask that the _errors_disable and _aborts_disable are described in
 * 'AL_INT_2ND_GROUP_A_*' and 'AL_INT_2ND_GROUP_B_*'
 *
 * @param regs pointer to unit registers
 * @param mode interrupt scheme mode (legacy, MSI-X..)
 * @param m2s_errors_disable
 * 	  This is a bit-wise mask, to indicate which one of the error causes in
 * 	  secondary interrupt group_A should generate an interrupt. When a bit is
 * 	  set, the error cause is ignored.
 * 	  Recommended value: 0 (enable all errors).
 * @param m2s_aborts_disable
 * 	  This is a bit-wise mask, to indicate which one of the error causes in
 * 	  secondary interrupt group_A should automatically put the UDMA in
 * 	  abort state. When a bit is set, the error cause does cause an abort.
 * 	  Recommended value: 0 (enable all aborts).
 * @param s2m_errors_disable
 * 	  This is a bit-wise mask, to indicate which one of the error causes in
 * 	  secondary interrupt group_A should generate an interrupt. When a bit is
 * 	  set, the error cause is ignored.
 * 	  Recommended value: 0xE0 (disable hint errors).
 * @param s2m_aborts_disable
 * 	  This is a bit-wise mask, to indicate which one of the error causes in
 * 	  secondary interrupt group_A should automatically put the UDMA in
 * 	  abort state. When a bit is set, the error cause does cause an abort.
 * 	  Recommended value: 0xE0 (disable hint aborts).
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_udma_iofic_config(struct unit_regs __iomem *regs,
			enum al_iofic_mode mode,
			uint32_t	m2s_errors_disable,
			uint32_t	m2s_aborts_disable,
			uint32_t	s2m_errors_disable,
			uint32_t	s2m_aborts_disable);
/**
 * return the offset of the unmask register for a given group.
 * this function can be used when the upper layer wants to directly
 * access the unmask regiter and bypass the al_udma_iofic_unmask() API.
 *
 * @param regs pointer to udma registers
 * @param level the interrupt controller level (primary / secondary)
 * @param group the interrupt group ('AL_INT_GROUP_*')
 * @return the offset of the unmask register.
 */
uint32_t __iomem * al_udma_iofic_unmask_offset_get(
	struct unit_regs __iomem	*regs,
	enum al_udma_iofic_level	level,
	int				group);

/**
 * Get the interrupt controller base address for either the primary or secondary
 * interrupt controller
 *
 * @param regs pointer to udma unit registers
 * @param level the interrupt controller level (primary / secondary)
 *
 * @returns	The interrupt controller base address
 *
 */
static INLINE void __iomem *al_udma_iofic_reg_base_get(
	struct unit_regs __iomem	*regs,
	enum al_udma_iofic_level	level)
{
	void __iomem *iofic_regs = (level == AL_UDMA_IOFIC_LEVEL_PRIMARY) ?
		(void __iomem *)&regs->gen.interrupt_regs.main_iofic :
		(void __iomem *)&regs->gen.interrupt_regs.secondary_iofic_ctrl;

	return iofic_regs;
}

/**
 * Check the interrupt controller level/group validity
 *
 * @param level the interrupt controller level (primary / secondary)
 * @param group the interrupt group ('AL_INT_GROUP_*')
 *
 * @returns	0 - invalid, 1 - valid
 *
 */
static INLINE int al_udma_iofic_level_and_group_valid(
	enum al_udma_iofic_level	level,
	int				group)
{
	if (((level == AL_UDMA_IOFIC_LEVEL_PRIMARY) && (group >= 0) && (group < 4)) ||
		((level == AL_UDMA_IOFIC_LEVEL_SECONDARY) && (group >= 0) && (group < 2)))
		return 1;

	return 0;
}
/**
 * unmask specific interrupts for a given group
 * this functions uses the interrupt mask clear register to guarantee atomicity
 * it's safe to call it while the mask is changed by the HW (auto mask) or another cpu.
 *
 * @param regs pointer to udma unit registers
 * @param level the interrupt controller level (primary / secondary)
 * @param group the interrupt group ('AL_INT_GROUP_*')
 * @param mask bitwise of interrupts to unmask, set bits will be unmasked.
 */
static INLINE void al_udma_iofic_unmask(
	struct unit_regs __iomem	*regs,
	enum al_udma_iofic_level	level,
	int				group,
	uint32_t			mask)
{
	al_assert(al_udma_iofic_level_and_group_valid(level, group));
	al_iofic_unmask(al_udma_iofic_reg_base_get(regs, level), group, mask);
}

/**
 * mask specific interrupts for a given group
 * this functions modifies interrupt mask register, the callee must make sure
 * the mask is not changed by another cpu.
 *
 * @param regs pointer to udma unit registers
 * @param level the interrupt controller level (primary / secondary)
 * @param group the interrupt group ('AL_INT_GROUP_*')
 * @param mask bitwise of interrupts to mask, set bits will be masked.
 */
static INLINE void al_udma_iofic_mask(
	struct unit_regs __iomem	*regs,
	enum al_udma_iofic_level	level,
	int				group,
	uint32_t			mask)
{
	al_assert(al_udma_iofic_level_and_group_valid(level, group));
	al_iofic_mask(al_udma_iofic_reg_base_get(regs, level), group, mask);
}

/**
 * read interrupt cause register for a given group
 * this will clear the set bits if the Clear on Read mode enabled.
 * @param regs pointer to udma unit registers
 * @param level the interrupt controller level (primary / secondary)
 * @param group the interrupt group ('AL_INT_GROUP_*')
 */
static INLINE uint32_t al_udma_iofic_read_cause(
	struct unit_regs __iomem	*regs,
	enum al_udma_iofic_level	level,
	int				group)
{
	al_assert(al_udma_iofic_level_and_group_valid(level, group));
	return al_iofic_read_cause(al_udma_iofic_reg_base_get(regs, level), group);
}

#endif
/** @} end of UDMA group */
