/*
 * include/linux/gta02-shadow.h
 *
 * Common utility code for GTA02
 *
 * Copyright (C) 2008 by Openmoko, Inc.
 * Author: Holger Hans Peter Freyther <freyther@openmoko.org>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#ifndef GTA02_SHADOW_H
#define GTA02_SHADOW_H

void gta02_gpb_add_shadow_gpio(unsigned int gpio);
void gta02_gpb_setpin(unsigned int pin, unsigned to);

#endif
