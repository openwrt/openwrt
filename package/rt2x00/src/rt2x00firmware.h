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
	Abstract: Data structures for the firmware loader.
	Supported chipsets: rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

#ifndef RT2X00FIRMWARE_H
#define RT2X00FIRMWARE_H

#ifdef CONFIG_RT2X00_LIB_FIRMWARE
int rt2x00lib_load_firmware(struct rt2x00_dev *rt2x00dev);
int rt2x00lib_load_firmware_wait(struct rt2x00_dev *rt2x00dev);
#else /* CONFIG_RT2X00_LIB_FIRMWARE */
static inline int rt2x00lib_load_firmware(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * This shouldn't happen.
	 */
	BUG();
	return 0;
}

static inline int rt2x00lib_load_firmware_wait(struct rt2x00_dev *rt2x00dev)
{
	return 0;
}
#endif /* CONFIG_RT2X00_LIB_FIRMWARE */

#endif /* RT2X00FIRMWARE_H */
