/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_UDMA_IOFIC_REG_H
#define __AL_HAL_UDMA_IOFIC_REG_H

#include <mach/al_hal_iofic_regs.h>

/** This structure covers all interrupt registers of a given UDMA, which is
 * built of an al_iofic_regs, which is the common I/O Fabric Interrupt
 * controller (IOFIC), and additional two interrupts groups dedicated for the
 * application-specific engine attached to the UDMA, the interrupt summary
 * of those two groups routed to gourp D of the main controller.
 */
struct udma_iofic_regs {
	struct al_iofic_regs	main_iofic;
	uint32_t rsrvd1[(0x1c00) >> 2];
	struct al_iofic_grp_ctrl secondary_iofic_ctrl[2];
};

#endif /* __AL_HAL_UDMA_IOFIC_REG_H */
