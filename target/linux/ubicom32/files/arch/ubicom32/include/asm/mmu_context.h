/*
 * arch/ubicom32/include/asm/mmu_context.h
 *   MMU context definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2004, Microtronix Datacom Ltd., All rights reserved.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#ifndef _ASM_UBICOM32_MMU_CONTEXT_H
#define _ASM_UBICOM32_MMU_CONTEXT_H

#include <asm/setup.h>
#include <asm/page.h>
#include <asm/pgalloc.h>

static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
}

extern inline int
init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
	// mm->context = virt_to_phys(mm->pgd);
	return(0);
}

#define destroy_context(mm)		do { } while(0)

static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next, struct task_struct *tsk)
{
}

#define deactivate_mm(tsk,mm)	do { } while (0)

extern inline void activate_mm(struct mm_struct *prev_mm, struct mm_struct *next_mm)
{
}

#endif /* _ASM_UBICOM32_MMU_CONTEXT_H */
