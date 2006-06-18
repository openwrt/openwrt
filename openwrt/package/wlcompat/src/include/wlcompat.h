/*
 * wlcompat.h
 *
 * Copyright (C) 2005 Felix Fietkau <nbd@vd-s.ath.cx>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id$
 */
#include <linux/wireless.h>

#ifndef WLCOMPAT_H
#define WLCOMPAT_H

#define WLCOMPAT_SET_MONITOR		SIOCIWFIRSTPRIV + 0
#define WLCOMPAT_GET_MONITOR		SIOCIWFIRSTPRIV + 1
#define WLCOMPAT_SET_TXPWR_LIMIT	SIOCIWFIRSTPRIV + 2
#define WLCOMPAT_GET_TXPWR_LIMIT	SIOCIWFIRSTPRIV + 3
#define WLCOMPAT_SET_ANTDIV		SIOCIWFIRSTPRIV + 4
#define WLCOMPAT_GET_ANTDIV		SIOCIWFIRSTPRIV + 5
#define WLCOMPAT_SET_TXANT		SIOCIWFIRSTPRIV + 6
#define WLCOMPAT_GET_TXANT		SIOCIWFIRSTPRIV + 7

#endif
