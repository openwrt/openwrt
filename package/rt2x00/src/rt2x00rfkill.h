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
	Module: rt2x00lib
	Abstract: Data structures for the rfkill.
	Supported chipsets: RT2460, RT2560, rt2561, rt2561s, rt2661.
 */

#ifndef RT2X00RFKILL_H
#define RT2X00RFKILL_H

#ifdef CONFIG_RT2X00_LIB_RFKILL
int rt2x00lib_register_rfkill(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_unregister_rfkill(struct rt2x00_dev *rt2x00dev);
int rt2x00lib_allocate_rfkill(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_free_rfkill(struct rt2x00_dev *rt2x00dev);
#else /* CONFIG_RT2X00_LIB_RFKILL */
static inline int rt2x00lib_register_rfkill(struct rt2x00_dev *rt2x00dev)
{
	return 0;
}

static inline void rt2x00lib_unregister_rfkill(struct rt2x00_dev *rt2x00dev){}

static inline int rt2x00lib_allocate_rfkill(struct rt2x00_dev *rt2x00dev)
{
	return 0;
}

static inline void rt2x00lib_free_rfkill(struct rt2x00_dev *rt2x00dev){}
#endif /* CONFIG_RT2X00_LIB_RFKILL */

#endif /* RT2X00RFKILL_H */
