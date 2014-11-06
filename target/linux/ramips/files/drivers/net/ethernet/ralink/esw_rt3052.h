/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2009-2013 John Crispin <blogic@openwrt.org>
 */

#ifndef _RALINK_ESW_RT3052_H__
#define _RALINK_ESW_RT3052_H__

#ifdef CONFIG_NET_RALINK_ESW_RT3052

int __init rtesw_init(void);
void rtesw_exit(void);

#else

static inline int __init rtesw_init(void) { return 0; }
static inline void rtesw_exit(void) { }

#endif
#endif
