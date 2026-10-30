// SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 *  @{
 * @file   al_hal_iofic.c
 *
 * @brief  interrupt controller hal
 *
 */

#include <mach/al_hal_iofic.h>
#include <mach/al_hal_iofic_regs.h>

/*
 * configure the interrupt registers, interrupts will are kept masked
 */
int al_iofic_config(void __iomem *regs_base, int group, uint32_t flags)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	al_reg_write32(&regs->ctrl[group].int_control_grp, flags);

	return 0;
}

/*
 * configure the moderation timer resolution for a given group
 */
int al_iofic_moder_res_config(void __iomem *regs_base, int group,
			     uint8_t resolution)

{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);
	uint32_t reg;

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	reg = al_reg_read32(&regs->ctrl[group].int_control_grp);
	AL_REG_FIELD_SET(reg,
			 INT_CONTROL_GRP_MOD_RES_MASK,
			 INT_CONTROL_GRP_MOD_RES_SHIFT,
			 resolution);
	al_reg_write32(&regs->ctrl[group].int_control_grp, reg);

	return 0;
}

/*
 * configure the moderation timer interval for a given msix vector.
 */
int al_iofic_msix_moder_interval_config(void __iomem *regs_base, int group,
				       uint8_t vector, uint8_t interval)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);
	uint32_t reg;

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	reg = al_reg_read32(&regs->grp_int_mod[group][vector].grp_int_mod_reg);
	AL_REG_FIELD_SET(reg,
			 INT_MOD_INTV_MASK,
			 INT_MOD_INTV_SHIFT,
			 interval);
	al_reg_write32(&regs->grp_int_mod[group][vector].grp_int_mod_reg, reg);

	return 0;
}

/*
 * return the offset of the unmask register for a given group
 */
uint32_t __iomem * al_iofic_unmask_offset_get(void __iomem *regs_base, int group)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	return &regs->ctrl[group].int_mask_clear_grp;
}

/*
 * unmask specific interrupts for a given group
 */
void al_iofic_unmask(void __iomem *regs_base, int group, uint32_t mask)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	/*
	 * use the mask clear register, no need to read the mask register
	 * itself. write 0 to unmask, 1 has no effect
	 */
	al_reg_write32_relaxed(&regs->ctrl[group].int_mask_clear_grp, ~mask);
}

/*
 * mask specific interrupts for a given group
 */
void al_iofic_mask(void __iomem *regs_base, int group, uint32_t mask)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);
	uint32_t reg;

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	reg = al_reg_read32(&regs->ctrl[group].int_mask_grp);

	al_reg_write32(&regs->ctrl[group].int_mask_grp, reg | mask);
}

/*
 * read interrupt cause register for a given group
 */
uint32_t al_iofic_read_cause(void __iomem *regs_base, int group)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	return al_reg_read32(&regs->ctrl[group].int_cause_grp);
}

/*
 * unmask specific interrupts from aborting the udma a given group
 */
void al_iofic_abort_mask(void __iomem *regs_base, int group, uint32_t mask)
{
	struct al_iofic_regs __iomem *regs = (struct al_iofic_regs __iomem *)(regs_base);

	al_assert(regs_base);
	al_assert(group < AL_IOFIC_MAX_GROUPS);

	al_reg_write32(&regs->ctrl[group].int_abort_msk_grp, mask);

}

/** @} end of interrupt controller group */
