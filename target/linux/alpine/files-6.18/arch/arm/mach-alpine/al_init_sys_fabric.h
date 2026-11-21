/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * The fabric consists of CCU, SMMU(*2), GIC(*2), and NB registers.
 *
 * The fabric hal provides simple api for fabric initialization, and not a
 * complete coverage of the unit's functionality.
 * main objective: to keep consistent fabric initialization between
 * different environments. Only functions used by all environments added.
 *
 * GIC and SMMU are only handled when initializing the secure context.
 * Both internal and external gic are initialized in that aspect.
 */

#ifndef __AL_HAL_SYS_FABRIC_H__
#define __AL_HAL_SYS_FABRIC_H__

#include <mach/al_hal_common.h>

/**
 * Initialize NB service regs
 *
 * @param nb_regs_address
 *	address of nb service registers
 * @param dev_ord_relax
 *	optimization: relax ordering between device-access reads and
 *	writes to different addresses.
 */
void al_nbservice_init(void __iomem *nb_regs_address,
		al_bool dev_ord_relax);

/**
 * Initialize CCU
 *
 * @param ccu_address
 *	address of ccu registers
 * @param iocc
 *	enable I/O cache coherency
 */
void al_ccu_init(void __iomem *ccu_address, al_bool iocc);

#endif /* AL_HAL_SYS_FABRIC_H_ */
