/*
 * arch/ubicom32/include/asm/vdc_tio.h
 *   Ubicom32 architecture VDC TIO definitions.
 *
 * (C) Copyright 2009, Ubicom, Inc.
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
#ifndef _ASM_UBICOM32_VDC_TIO_H
#define _ASM_UBICOM32_VDC_TIO_H

#include <asm/devtree.h>

#define VDCTIO_VP_VERSION			5

#define VDCTIO_SCALE_FLAG_VSUB			(1 << 9)
#define VDCTIO_SCALE_FLAG_YUV_SCAN_ORDER	(1 << 8)
#define VDCTIO_SCALE_FLAG_YUV_BLOCK_ORDER	(1 << 7)
#define VDCTIO_SCALE_FLAG_YUV			(1 << 6)
#define VDCTIO_SCALE_FLAG_VRANGE_16_255		(1 << 5)
#define VDCTIO_SCALE_FLAG_VRANGE_0_255		(1 << 4)
#define VDCTIO_SCALE_FLAG_HSUB_2_1		(1 << 3)
#define VDCTIO_SCALE_FLAG_HSUB_1_1		(1 << 2)
#define VDCTIO_SCALE_FLAG_SET_FRAME_BUFFER	(1 << 1)
#define VDCTIO_SCALE_FLAG_ENABLE		(1 << 0)

#define VDCTIO_NEXT_FRAME_FLAG_YUV_BIT		0
#define VDCTIO_NEXT_FRAME_FLAG_YUV		(1 << (VDCTIO_NEXT_FRAME_FLAG_YUV_BIT))

#define VDCTIO_CAPS_SUPPORTS_SCALING		(1 << 0)

#define VDCTIO_COMMAND_START			(1 << 3)
#define VDCTIO_COMMAND_SET_COEFF		(1 << 2)
#define VDCTIO_COMMAND_SET_LUT			(1 << 1)
#define VDCTIO_COMMAND_SET_SCALE_MODE		(1 << 0)

/*
 * Command / Data registers to access the VDC
 */
struct vdc_tio_vp_regs {
	/*
	 * Version of this TIO register map
	 */
	u32_t		version;

	volatile u32_t	command;

	/*
	 * Next frame pointer, when the command VDCTIO_COMMAND_SET_FRAME_BUFFER is set,
	 * the vdc will take the pointer here and display it.
	 */
	void		*next_frame;
	u32_t		next_frame_flags;

	/*
	 * These map directly into the PIXP registers 0x20-0x80.
	 * DO NOT change the order of these three variables.
	 */
	u32_t		red_lut[6];
	u32_t		blue_lut[6];
	u32_t		green_lut[13];

	/*
	 * These map directly into the PIXP registers 0x04, 0x08
	 */
	u32_t		coeff0;
	u32_t		coeff1;

	/*
	 * There are used to set the scaling parameters
	 */
	u32_t		x_in;
	u32_t		x_out;
	u32_t		y_in;
	u32_t		y_out;
	u32_t		scale_flags;

	/*
	 * Current frame number, monotonically increasing number
	 */
	u32_t		frame_number;

	/*
	 * These variables tell the guest OS what the underlying hardware looks like
	 */
	u32_t		caps;
	u32_t		xres;
	u32_t		yres;
	u32_t		fb_align;
	u8_t		bpp;
	u8_t		rbits;
	u8_t		gbits;
	u8_t		bbits;
	u8_t		rshift;
	u8_t		gshift;
	u8_t		bshift;
};

/*
 * Devtree node for VDC
 */
struct vdc_tio_node {
	struct devtree_node	dn;

	struct vdc_tio_vp_regs	*regs;
};

extern void vdc_tio_init(void);

#endif /* _ASM_UBICOM32_VDC_TIO_H */
