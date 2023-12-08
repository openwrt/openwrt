/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
#define _RTW_SDIO_C_

/*#include "drv_types_sdio.h"*/	/* RTW_SDIO_ADDR_CMD52_GEN */
#include "drv_types.h"		/* drv_types_sdio.h, struct dvobj_priv and etc. */
#include "sdio_ops.h"		/* rtw_sdio_raw_read(), rtw_sdio_raw_write() */

/*
 * Description:
 *	Use SDIO cmd52 or cmd53 to read/write data
 *
 * Parameters:
 *	d	pointer of device object(struct dvobj_priv)
 *	addr	SDIO address, 17 bits
 *	buf	buffer for I/O
 *	len	length
 *	write	0:read, 1:write
 *	cmd52	0:cmd52, 1:cmd53
 *
 * Return:
 *	_SUCCESS	I/O ok.
 *	_FAIL		I/O fail.
 */
static u8 sdio_io(struct dvobj_priv *d, u32 addr, void *buf, size_t len, u8 write, u8 cmd52)
{
#ifdef DBG_SDIO
#if (DBG_SDIO >= 3)
	struct sdio_data *sdio = dvobj_to_sdio(d);
#endif /* DBG_SDIO >= 3 */
#endif /* DBG_SDIO */
	u32 addr_drv;	/* address with driver defined bit */
	int err;
	u8 retry = 0;
	u8 stop_retry = _FALSE;	/* flag for stopping retry or not */

	if (dev_is_surprise_removed(d)) {
		RTW_ERR("%s: bSurpriseRemoved, skip %s 0x%05x, %zu bytes\n",
			__FUNCTION__, write?"write":"read", addr, len);
		return _FAIL;
	}

	addr_drv = addr;
	if (cmd52)
		addr_drv = RTW_SDIO_ADDR_CMD52_GEN(addr_drv);

	do {
		if (write)
			err = rtw_sdio_raw_write(d, addr_drv, buf, len, 0);
		else
			err = rtw_sdio_raw_read(d, addr_drv, buf, len, 0);
		if (!err) {
			if (retry) {
				RTW_INFO("%s: Retry %s OK! addr=0x%05x %zu bytes, retry=%u,%u\n",
					 __FUNCTION__, write?"write":"read",
					 addr, len, retry, ATOMIC_READ(&d->continual_io_error));
				RTW_INFO_DUMP("Data: ", buf, len);
			}
			rtw_reset_continual_io_error(d);
			break;
		}
		RTW_ERR("%s: %s FAIL! error(%d) addr=0x%05x %zu bytes, retry=%u,%u\n",
			__FUNCTION__, write?"write":"read", err, addr, len,
			retry, ATOMIC_READ(&d->continual_io_error));

#ifdef DBG_SDIO
#if (DBG_SDIO >= 3)
		if (sdio->dbg_enable) {
			if (sdio->err_test && sdio->err_test_triggered)
				sdio->err_test = 0;

			if (sdio->err_stop) {
				RTW_ERR("%s: I/O error! Set surprise remove flag ON!\n",
					__FUNCTION__);
				dev_set_surprise_removed(d);
				return _FAIL;
			}
		}
#endif /* DBG_SDIO >= 3 */
#endif /* DBG_SDIO */

		retry++;
		stop_retry = rtw_inc_and_chk_continual_io_error(d);
		if ((err == -1) || (stop_retry == _TRUE) || (retry > SD_IO_TRY_CNT)) {
			/* critical error, unrecoverable */
			RTW_ERR("%s: Fatal error! Set surprise remove flag ON! (retry=%u,%u)\n",
				__FUNCTION__, retry, ATOMIC_READ(&d->continual_io_error));
			dev_set_surprise_removed(d);
			return _FAIL;
		}

		/* WLAN IOREG or SDIO Local */
		if ((addr & 0x10000) || !(addr & 0xE000)) {
			RTW_WARN("%s: Retry %s addr=0x%05x %zu bytes, retry=%u,%u\n",
				 __FUNCTION__, write?"write":"read", addr, len,
				 retry, ATOMIC_READ(&d->continual_io_error));
			continue;
		}
		return _FAIL;
	} while (1);

	return _SUCCESS;
}

u8 rtw_sdio_read_cmd52(struct dvobj_priv *d, u32 addr, void *buf, size_t len)
{
	return sdio_io(d, addr, buf, len, 0, 1);
}

u8 rtw_sdio_read_cmd53(struct dvobj_priv *d, u32 addr, void *buf, size_t len)
{
	return sdio_io(d, addr, buf, len, 0, 0);
}

u8 rtw_sdio_write_cmd52(struct dvobj_priv *d, u32 addr, void *buf, size_t len)
{
	return sdio_io(d, addr, buf, len, 1, 1);
}

u8 rtw_sdio_write_cmd53(struct dvobj_priv *d, u32 addr, void *buf, size_t len)
{
	return sdio_io(d, addr, buf, len, 1, 0);
}

u8 rtw_sdio_f0_read(struct dvobj_priv *d, u32 addr, void *buf, size_t len)
{
	int err;
	u8 ret;


	ret = _SUCCESS;
	addr = RTW_SDIO_ADDR_F0_GEN(addr);

	err = rtw_sdio_raw_read(d, addr, buf, len, 0);
	if (err)
		ret = _FAIL;

	return ret;
}

/**
 * rtw_sdio_cmd53_align_size() - Align size to one CMD53 could complete
 * @d		struct dvobj_priv*
 * @len		length to align
 *
 * Adjust len to align block size, and the new size could be transfered by one
 * CMD53.
 * If len < block size, it would keep original value, otherwise the value
 * would be rounded up by block size.
 *
 * Return adjusted length.
 */
size_t rtw_sdio_cmd53_align_size(struct dvobj_priv *d, size_t len)
{
	u32 blk_sz;


	blk_sz = rtw_sdio_get_block_size(d);
	if (len <= blk_sz)
		return len;

	return _RND(len, blk_sz);
}

static s32 sdio_init_xmit_priv(struct _ADAPTER *a)
{
#ifdef CONFIG_TX_AMSDU_SW_MODE
	rtw_tasklet_init(&a->xmitpriv.xmit_tasklet,
			(void(*)(unsigned long))core_tx_amsdu_tasklet,
			(unsigned long)a);
#endif
	return _SUCCESS;
}

static void sdio_free_xmit_priv(_adapter *adapter)
{
}

static s32 sdio_init_recv_priv(struct dvobj_priv *dvobj)
{
	return _SUCCESS;
}

static void sdio_free_recv_priv(struct dvobj_priv *dvobj)
{
}

struct rtw_intf_ops sdio_ops = {
	/****************** xmit *********************/
	.init_xmit_priv = sdio_init_xmit_priv,
	.free_xmit_priv = sdio_free_xmit_priv,

	/******************  recv *********************/
	.init_recv_priv = sdio_init_recv_priv,
	.free_recv_priv = sdio_free_recv_priv,
};
