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
	Abstract: Data structures for the rt2x00lib module.
	Supported chipsets: RT2460, RT2560, RT2570,
	rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

#ifndef RT2X00DEV_H
#define RT2X00DEV_H

#include "rt2x00debug.h"
#include "rt2x00firmware.h"
#include "rt2x00rfkill.h"

/*
 * Radio control.
 */
int rt2x00lib_enable_radio(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_disable_radio(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_toggle_rx(struct rt2x00_dev *rt2x00dev, int enable);

/*
 * Initialization/uninitialization handlers.
 */
int rt2x00lib_initialize(struct rt2x00_dev *rt2x00dev);
void rt2x00lib_uninitialize(struct rt2x00_dev *rt2x00dev);

/*
 * Config handlers.
 */
void rt2x00lib_config_type(struct rt2x00_dev *rt2x00dev, const int type);
void rt2x00lib_config_phymode(struct rt2x00_dev *rt2x00dev, const int phymode);
void rt2x00lib_config_channel(struct rt2x00_dev *rt2x00dev, const int value,
	const int channel, const int freq, const int txpower);
void rt2x00lib_config_promisc(struct rt2x00_dev *rt2x00dev, const int promisc);
void rt2x00lib_config_txpower(struct rt2x00_dev *rt2x00dev, const int txpower);
void rt2x00lib_config_antenna(struct rt2x00_dev *rt2x00dev,
	const int antenna_tx, const int antenna_rx);

#endif /* RT2X00DEV_H */
