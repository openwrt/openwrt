/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 *  @{
 * @file   al_hal_udma_regs.h
 *
 * @brief  udma registers definition
 *
 *
 */
#ifndef __AL_HAL_UDMA_REG_H
#define __AL_HAL_UDMA_REG_H

#include <mach/al_hal_udma_regs_m2s.h>
#include <mach/al_hal_udma_regs_s2m.h>
#include <mach/al_hal_udma_regs_gen.h>

/** UDMA registers, either m2s or s2m */
union udma_regs {
	struct udma_m2s_regs m2s;
	struct udma_s2m_regs s2m;
};

struct unit_regs {
	struct udma_m2s_regs m2s;
	uint32_t rsrvd0[(0x10000 - sizeof(struct udma_m2s_regs)) >> 2];
	struct udma_s2m_regs s2m;
	uint32_t rsrvd1[((0x1C000 - 0x10000) - sizeof(struct udma_s2m_regs)) >> 2];
	struct udma_gen_regs gen;
};

/** UDMA submission and completion registers, M2S and S2M UDMAs have same stucture */
struct udma_rings_regs {
	uint32_t rsrvd0[8];
	uint32_t cfg;		/* Descriptor ring configuration */
	uint32_t status;	/* Descriptor ring status and information */
	uint32_t drbp_low;	/* Descriptor Ring Base Pointer [31:4] */
	uint32_t drbp_high;	/* Descriptor Ring Base Pointer [63:32] */
	uint32_t drl;		/* Descriptor Ring Length[23:2] */
	uint32_t drhp;		/* Descriptor Ring Head Pointer */
	uint32_t drtp_inc;	/* Descriptor Tail Pointer increment */
	uint32_t drtp;		/* Descriptor Tail Pointer */
	uint32_t dcp;		/* Descriptor Current Pointer */
	uint32_t crbp_low;	/* Completion Ring Base Pointer [31:4] */
	uint32_t crbp_high;	/* Completion Ring Base Pointer [63:32] */
	uint32_t crhp;		/* Completion Ring Head Pointer */
	uint32_t crhp_internal;	/* Completion Ring Head Pointer internal, before AX ... */
};

/** M2S and S2M generic structure of Q registers */
union udma_q_regs {
	struct udma_rings_regs	rings;
	struct udma_m2s_q	m2s_q;
	struct udma_s2m_q	s2m_q;
};

#endif /* __AL_HAL_UDMA_REG_H */
/** @} end of UDMA group */
