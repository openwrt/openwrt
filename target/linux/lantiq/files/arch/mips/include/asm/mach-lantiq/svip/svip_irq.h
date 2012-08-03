/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2010 Lantiq
 */
#ifndef __SVIP_IRQ_H
#define __SVIP_IRQ_H

#define IM_NUM				6

#define INT_NUM_IRQ0			8
#define INT_NUM_IM0_IRL0		(INT_NUM_IRQ0 + 0)
#define INT_NUM_IM1_IRL0		(INT_NUM_IM0_IRL0 + 32)
#define INT_NUM_IM2_IRL0		(INT_NUM_IM1_IRL0 + 32)
#define INT_NUM_IM3_IRL0		(INT_NUM_IM2_IRL0 + 32)
#define INT_NUM_IM4_IRL0		(INT_NUM_IM3_IRL0 + 32)
#define INT_NUM_EXTRA_START		(INT_NUM_IM4_IRL0 + 32)
#define INT_NUM_IM_OFFSET		(INT_NUM_IM1_IRL0 - INT_NUM_IM0_IRL0)

#define INT_NUM_IM5_IRL0		(INT_NUM_IRQ0 + 160)
#define MIPS_CPU_TIMER_IRQ		(INT_NUM_IM5_IRL0 + 2)

#endif
