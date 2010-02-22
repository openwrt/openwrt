/*
 * arch/ubicom32/include/asm/ubicom32ring.h
 * Userspace I/O platform driver for Ubicom32 ring buffers
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

#ifndef _ASM_UBICOM32_UBICOM32RING_H
#define _ASM_UBICOM32_UBICOM32RING_H

#define UIO_UBICOM32RING_REG_VERSION	2

struct uio_ubicom32ring_desc {
	volatile unsigned int		head;
	volatile unsigned int		tail;
	unsigned int			entries;
	volatile unsigned int		ring[0];
};

struct uio_ubicom32ring_regs {
	unsigned int			version;

	/*
	 * Magic type used to identify the ring set.  Each driver will
	 * have a different magic value.
	 */
	unsigned int			magic;

	/*
	 * Registers defined by the driver
	 */
	unsigned int			regs_size;
	void				*regs;

	/*
	 * The locations of the rings
	 *
	 * DO NOT ADD ANYTHING BELOW THIS LINE
	 */
	unsigned int			num_rings;
	struct uio_ubicom32ring_desc	*rings[0];
};

/*
 * ringtio_ring_flush
 */
static inline void ringtio_ring_flush(struct uio_ubicom32ring_desc *rd)
{
	rd->head = rd->tail = 0;
}

/*
 * ringtio_ring_get
 */
static inline int ringtio_ring_get(struct uio_ubicom32ring_desc *rd, void **val)
{
	if (rd->head == rd->tail) {
		return 0;
	}

	*val = (void *)rd->ring[rd->head++];
	if (rd->head == rd->entries) {
		rd->head = 0;
	}
	return 1;
}

/*
 * ringtio_ring_put
 */
static inline int ringtio_ring_put(struct uio_ubicom32ring_desc *rd, void *val)
{
	unsigned int newtail = rd->tail + 1;
	if (newtail == rd->entries) {
		newtail = 0;
	}

	if (newtail == rd->head) {
		return 0;
	}

	rd->ring[rd->tail] = (unsigned int)val;
	rd->tail = newtail;
	return 1;
}

#endif /* _ASM_UBICOM32_UBICOM32RING_H */
