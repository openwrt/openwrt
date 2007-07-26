/*
	Copyright (C) 2004 - 2007 rt2x00 SourceForge Project
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2x00debug
	Abstract: Data structures for the rt2x00debug.
	Supported chipsets: RT2460, RT2560, RT2570,
	rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

#ifndef RT2X00DEBUG_H
#define RT2X00DEBUG_H

#include <net/wireless.h>

typedef void (debug_access_t)(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data);

struct rt2x00debug_reg {
	debug_access_t *read;
	debug_access_t *write;

	unsigned int word_size;
	unsigned int word_count;
};

struct rt2x00debug {
	/*
	 * Reference to the modules structure.
	 */
	struct module *owner;

	/*
	 * Register access information.
	 */
	struct rt2x00debug_reg reg_csr;
	struct rt2x00debug_reg reg_eeprom;
	struct rt2x00debug_reg reg_bbp;
};

#ifdef CONFIG_RT2X00_LIB_DEBUGFS
void rt2x00debug_register(struct rt2x00_dev *rt2x00dev);
void rt2x00debug_deregister(struct rt2x00_dev *rt2x00dev);
#else /* CONFIG_RT2X00_LIB_DEBUGFS */
static inline void rt2x00debug_register(struct rt2x00_dev *rt2x00dev){}
static inline void rt2x00debug_deregister(struct rt2x00_dev *rt2x00dev){}
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */

#endif /* RT2X00DEBUG_H */
