/*
 * Hardware I/O implementation for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>

#include "xradio.h"
#include "hwio.h"
#include "sdio.h"

#define CHECK_ADDR_LEN  1

 /* Sdio addr is 4*spi_addr */
#define SPI_REG_ADDR_TO_SDIO(spi_reg_addr) ((spi_reg_addr) << 2)
#define SDIO_ADDR17BIT(buf_id, mpf, rfu, reg_id_ofs) \
				((((buf_id)    & 0x1F) << 7) \
				| (((mpf)        & 1) << 6) \
				| (((rfu)        & 1) << 5) \
				| (((reg_id_ofs) & 0x1F) << 0))
#define MAX_RETRY		3


static int __xradio_read(struct xradio_common *hw_priv, u16 addr,
                         void *buf, size_t buf_len, int buf_id)
{
	u16 addr_sdio;
	u32 sdio_reg_addr_17bit ;

#if (CHECK_ADDR_LEN)
	/* Check if buffer is aligned to 4 byte boundary */
	if (WARN_ON(((unsigned long)buf & 3) && (buf_len > 4))) {
		dev_dbg(hw_priv->pdev, "buffer is not aligned.\n");
		return -EINVAL;
	}
#endif

	/* Convert to SDIO Register Address */
	addr_sdio = SPI_REG_ADDR_TO_SDIO(addr);
	sdio_reg_addr_17bit = SDIO_ADDR17BIT(buf_id, 0, 0, addr_sdio);
	return sdio_data_read(hw_priv,
	                                         sdio_reg_addr_17bit,
	                                         buf, buf_len);
}

static int __xradio_write(struct xradio_common *hw_priv, u16 addr,
                              const void *buf, size_t buf_len, int buf_id)
{
	u16 addr_sdio;
	u32 sdio_reg_addr_17bit ;

#if (CHECK_ADDR_LEN)
	/* Check if buffer is aligned to 4 byte boundary */
	if (WARN_ON(((unsigned long)buf & 3) && (buf_len > 4))) {
		dev_err(hw_priv->pdev, "buffer is not aligned.\n");
		return -EINVAL;
	}
#endif

	/* Convert to SDIO Register Address */
	addr_sdio = SPI_REG_ADDR_TO_SDIO(addr);
	sdio_reg_addr_17bit = SDIO_ADDR17BIT(buf_id, 0, 0, addr_sdio);

	return sdio_data_write(hw_priv,
	                                          sdio_reg_addr_17bit,
	                                          buf, buf_len);
}

static inline int __xradio_read_reg32(struct xradio_common *hw_priv,
                                       u16 addr, u32 *val)
{
	return __xradio_read(hw_priv, addr, val, sizeof(val), 0);
}

static inline int __xradio_write_reg32(struct xradio_common *hw_priv,
                                        u16 addr, u32 val)
{
	return __xradio_write(hw_priv, addr, &val, sizeof(val), 0);
}

int xradio_reg_read(struct xradio_common *hw_priv, u16 addr, 
                    void *buf, size_t buf_len)
{
	int ret;
	sdio_lock(hw_priv);
	ret = __xradio_read(hw_priv, addr, buf, buf_len, 0);
	sdio_unlock(hw_priv);
	return ret;
}

int xradio_reg_write(struct xradio_common *hw_priv, u16 addr, 
                     const void *buf, size_t buf_len)
{
	int ret;
	sdio_lock(hw_priv);
	ret = __xradio_write(hw_priv, addr, buf, buf_len, 0);
	sdio_unlock(hw_priv);
	return ret;
}

int xradio_data_read(struct xradio_common *hw_priv, void *buf, size_t buf_len)
{
	int ret, retry = 1;
	sdio_lock(hw_priv);
	{
		int buf_id_rx = hw_priv->buf_id_rx;
		while (retry <= MAX_RETRY) {
			ret = __xradio_read(hw_priv, HIF_IN_OUT_QUEUE_REG_ID, buf,
			                    buf_len, buf_id_rx + 1);
			if (!ret) {
				buf_id_rx = (buf_id_rx + 1) & 3;
				hw_priv->buf_id_rx = buf_id_rx;
				break;
			} else {
				//~dgp this retrying stuff is silly as it can crash the fw if there is nothing to read
				dev_err(hw_priv->pdev, "data read error :%d - attempt %d of %d\n", ret, retry, MAX_RETRY);
				retry++;
				mdelay(1);
			}
		}
	}
	sdio_unlock(hw_priv);
	return ret;
}

int xradio_data_write(struct xradio_common *hw_priv, const void *buf,
                      size_t buf_len)
{
	int ret, retry = 1;
	sdio_lock(hw_priv);
	{
		int buf_id_tx = hw_priv->buf_id_tx;
		while (retry <= MAX_RETRY) {
			ret = __xradio_write(hw_priv, HIF_IN_OUT_QUEUE_REG_ID, buf,
			                     buf_len, buf_id_tx);
			if (!ret) {
				buf_id_tx = (buf_id_tx + 1) & 31;
				hw_priv->buf_id_tx = buf_id_tx;
				break;
			} else {
				dev_err(hw_priv->pdev, "data write error :%d - attempt %d - %d\n", ret, retry, MAX_RETRY);
				retry++;
				mdelay(1);
			}
		}
	}
	sdio_unlock(hw_priv);
	return ret;
}

int xradio_indirect_read(struct xradio_common *hw_priv, u32 addr, void *buf,
                         size_t buf_len, u32 prefetch, u16 port_addr)
{
	u32 val32 = 0;
	int i, ret;

	if ((buf_len / 2) >= 0x1000) {
		dev_err(hw_priv->pdev, "Can't read more than 0xfff words.\n");
		return -EINVAL;
		goto out;
	}

	sdio_lock(hw_priv);
	/* Write address */
	ret = __xradio_write_reg32(hw_priv, HIF_SRAM_BASE_ADDR_REG_ID, addr);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't write address register.\n");
		goto out;
	}

	/* Read CONFIG Register Value - We will read 32 bits */
	ret = __xradio_read_reg32(hw_priv, HIF_CONFIG_REG_ID, &val32);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't read config register.\n");
		goto out;
	}

	/* Set PREFETCH bit */
	ret = __xradio_write_reg32(hw_priv, HIF_CONFIG_REG_ID, val32 | prefetch);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't write prefetch bit.\n");
		goto out;
	}

	/* Check for PRE-FETCH bit to be cleared */
	for (i = 0; i < 20; i++) {
		ret = __xradio_read_reg32(hw_priv, HIF_CONFIG_REG_ID, &val32);
		if (ret < 0) {
			dev_err(hw_priv->pdev, "Can't check prefetch bit.\n");
			goto out;
		}
		if (!(val32 & prefetch))
			break;
		mdelay(i);
	}

	if (val32 & prefetch) {
		dev_err(hw_priv->pdev, "Prefetch bit is not cleared.\n");
		goto out;
	}

	/* Read data port */
	ret = __xradio_read(hw_priv, port_addr, buf, buf_len, 0);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't read data port.\n");
		goto out;
	}

out:
	sdio_unlock(hw_priv);
	return ret;
}

int xradio_apb_write(struct xradio_common *hw_priv, u32 addr, const void *buf,
                     size_t buf_len)
{
	int ret;

	if ((buf_len / 2) >= 0x1000) {
		dev_err(hw_priv->pdev, "Can't wrire more than 0xfff words.\n");
		return -EINVAL;
	}

	sdio_lock(hw_priv);

	/* Write address */
	ret = __xradio_write_reg32(hw_priv, HIF_SRAM_BASE_ADDR_REG_ID, addr);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't write address register.\n");
		goto out;
	}

	/* Write data port */
	ret = __xradio_write(hw_priv, HIF_SRAM_DPORT_REG_ID, buf, buf_len, 0);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't write data port.\n");
		goto out;
	}

out:
	sdio_unlock(hw_priv);
	return ret;
}

int xradio_ahb_write(struct xradio_common *hw_priv, u32 addr, const void *buf,
                     size_t buf_len)
{
	int ret;

	if ((buf_len / 2) >= 0x1000) {
		dev_err(hw_priv->pdev, "Can't wrire more than 0xfff words.\n");
		return -EINVAL;
	}

	sdio_lock(hw_priv);
	
	/* Write address */
	ret = __xradio_write_reg32(hw_priv, HIF_SRAM_BASE_ADDR_REG_ID, addr);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't write address register.\n");
		goto out;
	}

	/* Write data port */
	ret = __xradio_write(hw_priv, HIF_AHB_DPORT_REG_ID, buf, buf_len, 0);
	if (ret < 0) {
		dev_err(hw_priv->pdev, "Can't write data port.\n");
		goto out;
	}

out:
	sdio_unlock(hw_priv);
	return ret;
}
