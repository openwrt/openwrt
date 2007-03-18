/*
 * $Id$
 * 
 * Copyright (C) 2007 OpenWrt.org
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _ASM_MACH_MMZONE_H
#define _ASM_MACH_MMZONE_H

extern pg_data_t __node_data[];
#define NODE_DATA(nid)          (&__node_data[nid])
#define NODE_MEM_MAP(nid)       (NODE_DATA(nid)->node_mem_map)
#define pa_to_nid(addr) (((addr) >= ARCH_PFN_OFFSET << PAGE_SHIFT) ? 0 : -1)

#endif /* _ASM_MACH_MMZONE_H */
