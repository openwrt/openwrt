// SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#include <mach/al_hal_common.h>
#include <mach/al_hal_reg_utils.h>
#include "al_hal_nb_regs.h"
#include "al_init_sys_fabric.h"
#include "al_init_ccu_regs.h"

/* definition currently missing from nb_regs */
#define AL_NB_ACF_MISC_READ_BYPASS (1 << 30)

/* initialization of different units */
void al_nbservice_init(void __iomem *nb_regs_address,
		al_bool dev_ord_relax)
{
	struct al_nb_regs __iomem *nb_regs = nb_regs_address;

	/* allow reads to bypass writes to different addresses */
	al_reg_write32_masked(
			&(nb_regs->global.acf_misc),
			AL_NB_ACF_MISC_READ_BYPASS,
			(dev_ord_relax) ? AL_NB_ACF_MISC_READ_BYPASS : 0);
}

void al_ccu_init(void __iomem *ccu_address, al_bool iocc)
{
	/* enable snoop */
	if (iocc) {
		al_reg_write32(ccu_address + AL_CCU_SNOOP_CONTROL_IOFAB_0_OFFSET
									, 1);
		al_reg_write32(ccu_address + AL_CCU_SNOOP_CONTROL_IOFAB_1_OFFSET
									, 1);
	}
	/* disable speculative fetches from masters */
	al_reg_write32(ccu_address + AL_CCU_SPECULATION_CONTROL_OFFSET, 7);
}
