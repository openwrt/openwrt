/*
 *  Copyright (C) 2009, Jiejing Zhang <kzjeef@gmail.com>
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __JZ4740_BATTERY_H
#define __JZ4740_BATTERY_H

struct jz_batt_info {
	int dc_dect_gpio;	/* GPIO port of DC charger detection */
	int usb_dect_gpio;	/* GPIO port of USB charger detection */
	int charg_stat_gpio;	/* GPIO port of Charger state */

	int min_voltag;		/* Mininal battery voltage in uV */
	int max_voltag;		/* Maximum battery voltage in uV */
	int batt_tech;		/* Battery technology */
};

#endif
