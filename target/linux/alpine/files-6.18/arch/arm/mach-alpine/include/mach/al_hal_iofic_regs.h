/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_IOFIC_REG_H
#define __AL_HAL_IOFIC_REG_H

#include <mach/al_hal_common.h>

/*
* Unit Registers
*/

struct al_iofic_grp_ctrl {
	uint32_t int_cause_grp;         /* Interrupt Cause RegisterSet by hardware */
	uint32_t rsrvd1;
	uint32_t int_cause_set_grp;     /* Interrupt Cause Set RegisterWriting 1 to a bit in t ... */
	uint32_t rsrvd2;
	uint32_t int_mask_grp;          /* Interrupt Mask RegisterIf Auto-mask control bit =TR ... */
	uint32_t rsrvd3;
	uint32_t int_mask_clear_grp;    /* Interrupt Mask Clear RegisterUsed when auto-mask co ... */
	uint32_t rsrvd4;
	uint32_t int_status_grp;        /* Interrupt status RegisterThis register latch the st ... */
	uint32_t rsrvd5;
	uint32_t int_control_grp;       /* Interrupt Control Register */
	uint32_t rsrvd6;
	uint32_t int_abort_msk_grp;     /* Interrupt Mask RegisterEach bit in this register ma ... */
	uint32_t rsrvd7;
	uint32_t int_log_msk_grp;       /* Interrupt Log RegisterEach bit in this register mas ... */
	uint32_t rsrvd8;
};

struct al_iofic_grp_mod {
	uint32_t grp_int_mod_reg;      /* Interrupt moderation registerDedicated moderation in ... */
	uint32_t rsrvd;
};

struct al_iofic_regs {
	struct al_iofic_grp_ctrl ctrl[0];
	uint32_t rsrvd1[0x400 >> 2];
	struct al_iofic_grp_mod grp_int_mod[0][32];
};

/*
* Registers Fields
*/

/**** int_control_grp register ****/
/* When Clear_on_Read =1, All bits of  Cause register  ... */
#define INT_CONTROL_GRP_CLEAR_ON_READ (1 << 0)
/* (must be set only when MSIX is enabled)When Auto-Ma ... */
#define INT_CONTROL_GRP_AUTO_MASK (1 << 1)
/* Auto_Clear (RW)When Auto-Clear =1, the bits in the  ... */
#define INT_CONTROL_GRP_AUTO_CLEAR (1 << 2)
/* When Set_on_Posedge =1, the bits in the interrupt c ... */
#define INT_CONTROL_GRP_SET_ON_POSEDGE (1 << 3)
/* When mask_msi_x =1, No MSI-X from this group is sen ... */
#define INT_CONTROL_GRP_MASK_MSI_X (1 << 5)
/* This value determines the Moderation_Timer_Clock sp ... */
#define INT_CONTROL_GRP_MOD_RES_MASK 0x0F000000
#define INT_CONTROL_GRP_MOD_RES_SHIFT 24

/**** grp_int_mod_reg register ****/
/* Interrupt Moderation Interval registerDedicated reg ... */
#define INT_MOD_INTV_MASK 0x000000FF
#define INT_MOD_INTV_SHIFT 0

#endif /* __AL_HAL_IOFIC_REG_H */
