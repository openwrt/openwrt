/*
 *  $Id$
 *
 *  Generic prom definitions
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#ifndef _ADM5120_PROM_H_
#define _ADM5120_PROM_H_

/*
 * Helper routines
 */
static inline u16 prom_read_le16(void *buf)
{
	u8 *p = buf;

	return ((u16)p[0] + ((u16)p[1] << 8));
}

static inline u32 prom_read_le32(void *buf)
{
	u8 *p = buf;

	return ((u32)p[0] + ((u32)p[1] << 8) + ((u32)p[2] << 16) +
		((u32)p[3] << 24));
}

static inline u16 prom_read_be16(void *buf)
{
	u8 *p = buf;

	return (((u16)p[0] << 8) + (u16)p[1]);
}

static inline u32 prom_read_be32(void *buf)
{
	u8 *p = buf;

	return (((u32)p[0] << 24) + ((u32)p[1] << 16) + ((u32)p[2] << 8) +
		((u32)p[3]));
}

#endif /* _ADM5120_PROM_H_ */


