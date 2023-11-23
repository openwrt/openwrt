/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
#ifndef __SDIO_OPS_H__
#define __SDIO_OPS_H__

#include <drv_types.h>		/* struct dvobj_priv, struct sdio_data and etc. */

int __must_check rtw_sdio_raw_read(struct dvobj_priv *d, unsigned int addr,
				   void *buf, size_t len, bool fixed);
int __must_check rtw_sdio_raw_write(struct dvobj_priv *d, unsigned int addr,
				    void *buf, size_t len, bool fixed);

void dump_sdio_card_info(void *sel, struct dvobj_priv *dvobj);

u32 rtw_sdio_init(struct dvobj_priv *dvobj);
void rtw_sdio_deinit(struct dvobj_priv *dvobj);
int rtw_sdio_alloc_irq(struct dvobj_priv *dvobj);
void rtw_sdio_free_irq(struct dvobj_priv *dvobj);
u8 rtw_sdio_get_num_of_func(struct dvobj_priv *dvobj);

#endif /* !__SDIO_OPS_H__ */
