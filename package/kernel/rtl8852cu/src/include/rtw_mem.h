/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __RTW_MEM_H__
#define __RTW_MEM_H__

#define RTKM_MGMT_SIZE PAGE_SIZE

void *rtkm_kmalloc(size_t size, gfp_t flags);
void *rtkm_kzalloc(size_t size, gfp_t flags);
void rtkm_kfree(const void *objp, size_t size);

int rtkm_prealloc_init(void);
void rtkm_prealloc_destroy(void);
void rtkm_dump_mstatus(void *sel);
void rtkm_set_trace(unsigned int mask);

#endif /* __RTW_MEM_H__ */
