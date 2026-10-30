// SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 *  @{
 * @file   al_hal_udma_iofic.c
 *
 * @brief  unit interrupts configurations
 *
 */

#include <mach/al_hal_udma_iofic.h>
#include <mach/al_hal_udma_regs.h>

/*
 * configure the interrupt registers, interrupts will are kept masked
 */
static int al_udma_main_iofic_config(struct al_iofic_regs __iomem *base,
				    enum al_iofic_mode mode)
{
	switch (mode) {
	case AL_IOFIC_MODE_LEGACY:
		al_iofic_config(base, AL_INT_GROUP_A,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_MASK_MSI_X |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		al_iofic_config(base, AL_INT_GROUP_B,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_C,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_D,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_MASK_MSI_X |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		break;
	case AL_IOFIC_MODE_MSIX_PER_Q:
		al_iofic_config(base, AL_INT_GROUP_A,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_AUTO_MASK |
				INT_CONTROL_GRP_AUTO_CLEAR);
		al_iofic_config(base, AL_INT_GROUP_B,
				INT_CONTROL_GRP_AUTO_CLEAR |
				INT_CONTROL_GRP_AUTO_MASK |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		al_iofic_config(base, AL_INT_GROUP_C,
				INT_CONTROL_GRP_AUTO_CLEAR |
				INT_CONTROL_GRP_AUTO_MASK |
				INT_CONTROL_GRP_CLEAR_ON_READ);
		al_iofic_config(base, AL_INT_GROUP_D,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		break;
	case AL_IOFIC_MODE_MSIX_PER_GROUP:
		al_iofic_config(base, AL_INT_GROUP_A,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_AUTO_CLEAR |
				INT_CONTROL_GRP_AUTO_MASK);
		al_iofic_config(base, AL_INT_GROUP_B,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_C,
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		al_iofic_config(base, AL_INT_GROUP_D,
				INT_CONTROL_GRP_SET_ON_POSEDGE |
				INT_CONTROL_GRP_CLEAR_ON_READ |
				INT_CONTROL_GRP_MASK_MSI_X);
		break;
	default:
		al_err("%s: invalid mode (%d)\n", __func__, mode);
		return -EINVAL;
	}

	al_dbg("%s: base.%p mode %d\n", __func__, base, mode);
	return 0;
}

/*
 * configure the UDMA interrupt registers, interrupts are kept masked
 */
int al_udma_iofic_config(struct unit_regs __iomem *regs, enum al_iofic_mode mode,
			uint32_t	m2s_errors_disable,
			uint32_t	m2s_aborts_disable,
			uint32_t	s2m_errors_disable,
			uint32_t	s2m_aborts_disable)
{
	int rc;

	rc = al_udma_main_iofic_config(&regs->gen.interrupt_regs.main_iofic, mode);
	if (rc != 0)
		return rc;

	al_iofic_unmask(&regs->gen.interrupt_regs.secondary_iofic_ctrl, AL_INT_GROUP_A, ~m2s_errors_disable);
	al_iofic_abort_mask(&regs->gen.interrupt_regs.secondary_iofic_ctrl, AL_INT_GROUP_A, m2s_aborts_disable);

	al_iofic_unmask(&regs->gen.interrupt_regs.secondary_iofic_ctrl, AL_INT_GROUP_B, ~s2m_errors_disable);
	al_iofic_abort_mask(&regs->gen.interrupt_regs.secondary_iofic_ctrl, AL_INT_GROUP_B, s2m_aborts_disable);

	al_dbg("%s base.%p mode %d\n", __func__, regs, mode);
	return 0;
}

/*
 * return the offset of the unmask register for a given group
 */
uint32_t __iomem * al_udma_iofic_unmask_offset_get(
	struct unit_regs __iomem	*regs,
	enum al_udma_iofic_level	level,
	int				group)
{
	al_assert(al_udma_iofic_level_and_group_valid(level, group));
	return al_iofic_unmask_offset_get(al_udma_iofic_reg_base_get(regs, level), group);
}

/** @} end of UDMA group */
