/*
 * arch/ubicom32/include/asm/ubicom32fb.h
 *   Ubicom32 architecture video frame buffer definitions.
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
 */
#ifndef _ASM_UBICOM32_UBICOM32FB_H
#define _ASM_UBICOM32_UBICOM32FB_H

#include <linux/ioctl.h>

/*
 * Set next frame
 */
#define UBICOM32FB_IOCTL_SET_NEXT_FRAME		_IOW('r',  1, void *)
#define UBICOM32FB_IOCTL_SET_NEXT_FRAME_SYNC	_IOW('r',  2, void *)

/*
 * Set Mode
 */
#define UBICOM32FB_IOCTL_SET_MODE		_IOW('r',  3, void *)
struct ubicom32fb_mode {
	unsigned long	width;
	unsigned long	height;
	unsigned long	flags;
	void		*next_frame;
};
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_SCAN_ORDER	(1 << 8)

#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_BLOCK_ORDER	(1 << 7)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV		(1 << 6)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_VSUB		(1 << 5)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_VRANGE_16_255	(1 << 4)

#define UBICOM32FB_IOCTL_SET_MODE_FLAG_VRANGE_0_255	(1 << 3)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_HSUB_2_1		(1 << 2)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_HSUB_1_1		(1 << 1)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_SCALE_ENABLE	(1 << 0)

#endif /* _ASM_UBICOM32_UBICOM32FB_H */
