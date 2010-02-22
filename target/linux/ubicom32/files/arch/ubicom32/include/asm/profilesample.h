/*
 * arch/ubicom32/mach-common/profile.h
 *   Private data for the profile module
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
 */


#ifndef _PROFILESAMPLE_H_
#define _PROFILESAMPLE_H_

/*
 * a sample taken by the ipProfile package for sending to the profilertool
 */
struct profile_sample {
	unsigned int pc;			/* PC value */
	unsigned int a5;			/* a5 contents for parent of leaf function */
	unsigned int parent;			/* return address from stack, to find the caller */
	unsigned int latency;			/* CPU clocks since the last message dispatch in this thread (thread 0 ony for now) */
	unsigned short active;			/* which threads are active - for accurate counting */
	unsigned short d_blocked;		/* which threads are blocked due to D cache misses */
	unsigned short i_blocked;		/* which threads are blocked due to I cache misses */
	unsigned char cond_codes;		/* for branch prediction */
	unsigned char thread;			/* I-blocked, D-blocked, 4-bit thread number */
};

#endif
