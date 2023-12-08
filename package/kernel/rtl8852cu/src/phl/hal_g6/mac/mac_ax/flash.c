/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#include "flash.h"
#include "../mac_ax/fwcmd.h"

#if MAC_AX_FW_REG_OFLD
u32 mac_flash_erase(struct mac_ax_adapter *adapter,
		    u32 addr,
		    u32 length,
		    u32 timeout)
{
	u8 *buf;
	u32 ret = 0, pkt_len, local_timeout = 10000, no_timeout = 0;
	u32 data[2];
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	if (timeout == 0)
		no_timeout = 1;
	else
		local_timeout = timeout;

	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.erase_done = 0;
	adapter->flash_info.erasing = 1;
	adapter->flash_info.erase_addr = addr;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		return MACNPTR; // Maybe set a timeout counter
	}
	pkt_len = LEN_FLASH_H2C_HDR;
	buf = h2cb_put(h2cb, pkt_len);
	if (!buf) {
		ret = MACNOBUF;
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}
	data[0] = cpu_to_le32(addr);
	data[1] = cpu_to_le32(length);
	PLTFM_MEMCPY(buf, (u8 *)data, pkt_len);
	ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
				   FWCMD_TYPE_H2C,
				   FWCMD_H2C_CAT_MAC,
				   FWCMD_H2C_CL_FLASH,
				   FWCMD_H2C_FUNC_PLAT_FLASH_ERASE,/*platform auto test*/
				   0,
				   0);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %x\n", ret);
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}
	h2cb_free(adapter, h2cb);
	h2cb = NULL;
	while (1) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		if (adapter->flash_info.erase_done == 1) {
			adapter->flash_info.erase_done = 0;
			adapter->flash_info.erasing = 0;
			adapter->flash_info.erase_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			break;
		}
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);

		//Sleep(50);
		PLTFM_DELAY_MS(1);
		if (no_timeout == 0) {
			local_timeout--;
			if (local_timeout == 0) {
				ret = MACFLASHFAIL;
				PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
				adapter->flash_info.erasing = 0;
				adapter->flash_info.erase_addr = 0;
				PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
				goto fail;
			}
		}
	}
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.erasing = 0;
	adapter->flash_info.erase_addr = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	return MACSUCCESS;
fail:
	if (h2cb)
		h2cb_free(adapter, h2cb);
	adapter->fw_info.h2c_seq--;

	return ret;
}

u32 mac_flash_read(struct mac_ax_adapter *adapter,
		   u32 addr,
		   u32 length,
		   u8 *buffer,
		   u32 timeout)
{
	u8 *buf;
	u32 ret = 0, pkt_len, local_timeout = 10000, no_timeout = 0;
	u32 data[2];
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	//PLTFM_MSG_TRACE("testdata = 0x%llx\n", (u64)buffer);
	if (length > FLASH_H2C_SIZE)
		return MACFLASHFAIL;

	if (timeout == 0)
		no_timeout = 1;
	else
		local_timeout = timeout;
	if ((addr % 4) || (length % 4)) {
		PLTFM_MSG_ERR("Address/length not 4 byte aligned, addr : 0x%x, length : 0x%x\n"
			, addr, length);
		return MACFLASHFAIL;
	}
	// mutex
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.read_done = 0;
	adapter->flash_info.reading = 1;
	adapter->flash_info.buf_addr = buffer;
	adapter->flash_info.read_addr = addr;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR; // Maybe set a timeout counter
	pkt_len = LEN_FLASH_H2C_HDR;
	buf = h2cb_put(h2cb, pkt_len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	data[0] = cpu_to_le32(addr);
	data[1] = cpu_to_le32(length);
	PLTFM_MEMCPY(buf, (u8 *)data, pkt_len);
	ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
				   FWCMD_TYPE_H2C,
				   FWCMD_H2C_CAT_MAC,
				   FWCMD_H2C_CL_FLASH,
				   FWCMD_H2C_FUNC_PLAT_FLASH_READ,/*platform auto test*/
				   0,
				   0);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.reading = 0;
		adapter->flash_info.buf_addr = NULL;
		adapter->flash_info.read_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.reading = 0;
		adapter->flash_info.buf_addr = NULL;
		adapter->flash_info.read_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %x\n", ret);
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.reading = 0;
		adapter->flash_info.buf_addr = NULL;
		adapter->flash_info.read_addr = 0;
		adapter->flash_info.read_done = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}
	h2cb_free(adapter, h2cb);
	h2cb = NULL;
	while (1) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		//PLTFM_MSG_TRACE("polling enter critical\n");
		if (adapter->flash_info.read_done == 1) {
			//PLTFM_MSG_TRACE("read DONE\n");
			adapter->flash_info.reading = 0;
			adapter->flash_info.buf_addr = NULL;
			adapter->flash_info.read_addr = 0;
			adapter->flash_info.read_done = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			break;
		}
		//PLTFM_MSG_TRACE("polling exit critical\n");
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		//Sleep(1);
		PLTFM_DELAY_MS(1);
		if (no_timeout == 0) {
			local_timeout--;
			if (local_timeout == 0) {
				ret = MACFLASHFAIL;
				PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
				adapter->flash_info.reading = 0;
				adapter->flash_info.buf_addr = NULL;
				adapter->flash_info.read_addr = 0;
				adapter->flash_info.read_done = 0;
				PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
				goto fail;
			}
		}
	}
	// end mutex
	//adapter->flash_info.read_done == 0;
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.reading = 0;
	adapter->flash_info.buf_addr = NULL;
	adapter->flash_info.read_addr = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	return MACSUCCESS;
fail:
	if (h2cb)
		h2cb_free(adapter, h2cb);
	adapter->fw_info.h2c_seq--;

	return ret;
}

u32 mac_flash_write(struct mac_ax_adapter *adapter,
		    u32 addr,
		    u32 length,
		    u8 *buffer,
		    u32 timeout)
{
	u8 *buf;
	u32 ret = 0, residue_len, pkt_len, local_timeout = 5000, no_timeout = 0, i;
	u32 data[2];
	u32 waddr, wlength;
	u32 *pbuf;
	u32 *psource_data;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	if (timeout == 0)
		no_timeout = 1;
	else
		local_timeout = timeout;

	residue_len = length;
	waddr = addr;
	wlength = length;
	if ((addr % 4) || (length % 4)) {
		PLTFM_MSG_ERR("Address/length not 4 byte aligned, addr : 0x%x, length : 0x%x\n"
			, addr, length);
		return MACFLASHFAIL;
	}
	while (residue_len) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.write_done = 0;
		adapter->flash_info.writing = 1;
		adapter->flash_info.write_addr = waddr;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		if (residue_len >= FLASH_H2C_SIZE)
			pkt_len = FLASH_H2C_SIZE;
		else
			pkt_len = residue_len;

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
		if (!h2cb) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			return MACNPTR; // Maybe set a timeout counter
		}

		buf = h2cb_put(h2cb, pkt_len + LEN_FLASH_H2C_HDR);
		if (!buf) {
			ret = MACNOBUF;
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
		data[0] = cpu_to_le32(waddr);
		data[1] = cpu_to_le32(pkt_len);
		PLTFM_MEMCPY(buf, (u8 *)data, LEN_FLASH_H2C_HDR);
		buf += LEN_FLASH_H2C_HDR;
		//copy data
		pbuf = (u32 *)buf;
		psource_data = (u32 *)buffer;
		for (i = 0; i < (pkt_len / sizeof(u32)); i++) {
			*pbuf = cpu_to_le32(*psource_data);
			pbuf++;
			psource_data++;
		}
		//PLTFM_MEMCPY(buf, buffer, pkt_len);
		ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
					   FWCMD_TYPE_H2C,
					   FWCMD_H2C_CAT_MAC,
					   FWCMD_H2C_CL_FLASH,
					   FWCMD_H2C_FUNC_PLAT_FLASH_WRITE,/*platform auto test*/
					   0,
					   0);
		if (ret) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
		if (ret) {
			PLTFM_MSG_ERR("[ERR]platform tx: %x\n", ret);
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
		h2cb_free(adapter, h2cb);
		h2cb = NULL;
		PLTFM_MSG_ERR("Write H2C, addr = 0x%x, length = %d\n", waddr, pkt_len);

		// delay for flash write
		//Sleep(1000);
		while (1) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			if (adapter->flash_info.write_done == 1) {
				adapter->flash_info.writing = 0;
				adapter->flash_info.write_addr = 0;
				adapter->flash_info.write_done = 0;
				PLTFM_MSG_TRACE("write DONE\n");
				PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
				break;
			}
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			//Sleep(1);
			PLTFM_DELAY_MS(1);
			if (no_timeout == 0) {
				local_timeout--;
				if (local_timeout == 0) {
					ret = MACFLASHFAIL;
					PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
					adapter->flash_info.writing = 0;
					adapter->flash_info.write_addr = 0;
					PLTFM_MSG_TRACE("write timeout\n");
					PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
					goto fail;
				}
			}
		}
		residue_len -= pkt_len;
		buffer += pkt_len;
		waddr += pkt_len;
	}
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.writing = 0;
	adapter->flash_info.write_addr = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	return MACSUCCESS;
fail:
	if (h2cb)
		h2cb_free(adapter, h2cb);
	adapter->fw_info.h2c_seq--;

	return ret;
}

#else
u32 mac_flash_erase(struct mac_ax_adapter *adapter,
		    u32 addr,
		    u32 length,
		    u32 timeout)
{
	u8 *buf;
	u32 ret = 0, pkt_len, local_timeout = 10000, no_timeout = 0;
	u32 data[2];
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	if (timeout == 0)
		no_timeout = 1;
	else
		local_timeout = timeout;

	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.erase_done = 0;
	adapter->flash_info.erasing = 1;
	adapter->flash_info.erase_addr = addr;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		return MACNPTR; // Maybe set a timeout counter
	}
	pkt_len = LEN_FLASH_H2C_HDR;
	buf = h2cb_put(h2cb, pkt_len);
	if (!buf) {
		ret = MACNOBUF;
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}
	data[0] = cpu_to_le32(addr);
	data[1] = cpu_to_le32(length);
	PLTFM_MEMCPY(buf, (u8 *)data, pkt_len);
	ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
				   FWCMD_TYPE_H2C,
				   FWCMD_H2C_CAT_MAC,
				   FWCMD_H2C_CL_FLASH,
				   FWCMD_H2C_FUNC_PLAT_FLASH_ERASE,/*platform auto test*/
				   0,
				   0);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %x\n", ret);
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.erasing = 0;
		adapter->flash_info.erase_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}
	h2cb_free(adapter, h2cb);
	h2cb = NULL;
	while (1) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		if (adapter->flash_info.erase_done == 1) {
			adapter->flash_info.erase_done = 0;
			adapter->flash_info.erasing = 0;
			adapter->flash_info.erase_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			break;
		}
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);

		//Sleep(50);
		PLTFM_DELAY_MS(1);
		if (no_timeout == 0) {
			local_timeout--;
			if (local_timeout == 0) {
				ret = MACFLASHFAIL;
				PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
				adapter->flash_info.erasing = 0;
				adapter->flash_info.erase_addr = 0;
				PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
				goto fail;
			}
		}
	}
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.erasing = 0;
	adapter->flash_info.erase_addr = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	return MACSUCCESS;
fail:
	if (h2cb)
		h2cb_free(adapter, h2cb);
	adapter->fw_info.h2c_seq--;

	return ret;
}

u32 mac_flash_read(struct mac_ax_adapter *adapter,
		   u32 addr,
		   u32 length,
		   u8 *buffer,
		   u32 timeout)
{
	u8 *buf;
	u32 ret = 0, pkt_len, local_timeout = 10000, no_timeout = 0;
	u32 data[2];
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	//PLTFM_MSG_TRACE("testdata = 0x%llx\n", (u64)buffer);
	if (length > FLASH_H2C_SIZE)
		return MACFLASHFAIL;

	if (timeout == 0)
		no_timeout = 1;
	else
		local_timeout = timeout;
	if ((addr % 4) || (length % 4)) {
		PLTFM_MSG_ERR("Address/length not 4 byte aligned, addr : 0x%x, length : 0x%x\n"
			, addr, length);
		return MACFLASHFAIL;
	}
	// mutex
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.read_done = 0;
	adapter->flash_info.reading = 1;
	adapter->flash_info.buf_addr = buffer;
	adapter->flash_info.read_addr = addr;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR; // Maybe set a timeout counter
	pkt_len = LEN_FLASH_H2C_HDR;
	buf = h2cb_put(h2cb, pkt_len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	data[0] = cpu_to_le32(addr);
	data[1] = cpu_to_le32(length);
	PLTFM_MEMCPY(buf, (u8 *)data, pkt_len);
	ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
				   FWCMD_TYPE_H2C,
				   FWCMD_H2C_CAT_MAC,
				   FWCMD_H2C_CL_FLASH,
				   FWCMD_H2C_FUNC_PLAT_FLASH_READ,/*platform auto test*/
				   0,
				   0);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.reading = 0;
		adapter->flash_info.buf_addr = NULL;
		adapter->flash_info.read_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.reading = 0;
		adapter->flash_info.buf_addr = NULL;
		adapter->flash_info.read_addr = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %x\n", ret);
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.reading = 0;
		adapter->flash_info.buf_addr = NULL;
		adapter->flash_info.read_addr = 0;
		adapter->flash_info.read_done = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		goto fail;
	}
	h2cb_free(adapter, h2cb);
	h2cb = NULL;
	while (1) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		//PLTFM_MSG_TRACE("polling enter critical\n");
		if (adapter->flash_info.read_done == 1) {
			//PLTFM_MSG_TRACE("read DONE\n");
			adapter->flash_info.reading = 0;
			adapter->flash_info.buf_addr = NULL;
			adapter->flash_info.read_addr = 0;
			adapter->flash_info.read_done = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			break;
		}
		//PLTFM_MSG_TRACE("polling exit critical\n");
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		//Sleep(1);
		PLTFM_DELAY_MS(1);
		if (no_timeout == 0) {
			local_timeout--;
			if (local_timeout == 0) {
				ret = MACFLASHFAIL;
				PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
				adapter->flash_info.reading = 0;
				adapter->flash_info.buf_addr = NULL;
				adapter->flash_info.read_addr = 0;
				adapter->flash_info.read_done = 0;
				PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
				goto fail;
			}
		}
	}
	// end mutex
	//adapter->flash_info.read_done == 0;
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.reading = 0;
	adapter->flash_info.buf_addr = NULL;
	adapter->flash_info.read_addr = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	return MACSUCCESS;
fail:
	if (h2cb)
		h2cb_free(adapter, h2cb);
	adapter->fw_info.h2c_seq--;

	return ret;
}

u32 mac_flash_write(struct mac_ax_adapter *adapter,
		    u32 addr,
		    u32 length,
		    u8 *buffer,
		    u32 timeout)
{
	u8 *buf;
	u32 ret = 0, residue_len, pkt_len, local_timeout = 5000, no_timeout = 0, i;
	u32 data[2];
	u32 waddr, wlength;
	u32 *pbuf;
	u32 *psource_data;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	if (timeout == 0)
		no_timeout = 1;
	else
		local_timeout = timeout;

	residue_len = length;
	waddr = addr;
	wlength = length;
	if ((addr % 4) || (length % 4)) {
		PLTFM_MSG_ERR("Address/length not 4 byte aligned, addr : 0x%x, length : 0x%x\n"
			, addr, length);
		return MACFLASHFAIL;
	}
	while (residue_len) {
		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		adapter->flash_info.write_done = 0;
		adapter->flash_info.writing = 1;
		adapter->flash_info.write_addr = waddr;
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		if (residue_len >= FLASH_H2C_SIZE)
			pkt_len = FLASH_H2C_SIZE;
		else
			pkt_len = residue_len;

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
		if (!h2cb) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			return MACNPTR; // Maybe set a timeout counter
		}

		buf = h2cb_put(h2cb, pkt_len + LEN_FLASH_H2C_HDR);
		if (!buf) {
			ret = MACNOBUF;
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
		data[0] = cpu_to_le32(waddr);
		data[1] = cpu_to_le32(pkt_len);
		PLTFM_MEMCPY(buf, (u8 *)data, LEN_FLASH_H2C_HDR);
		buf += LEN_FLASH_H2C_HDR;
		//copy data
		pbuf = (u32 *)buf;
		psource_data = (u32 *)buffer;
		for (i = 0; i < (pkt_len / sizeof(u32)); i++) {
			*pbuf = cpu_to_le32(*psource_data);
			pbuf++;
			psource_data++;
		}
		//PLTFM_MEMCPY(buf, buffer, pkt_len);
		ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
					   FWCMD_TYPE_H2C,
					   FWCMD_H2C_CAT_MAC,
					   FWCMD_H2C_CL_FLASH,
					   FWCMD_H2C_FUNC_PLAT_FLASH_WRITE,/*platform auto test*/
					   0,
					   0);
		if (ret) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
		ret = h2c_pkt_build_txd(adapter, h2cb);
		if (ret) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
		if (ret) {
			PLTFM_MSG_ERR("[ERR]platform tx: %x\n", ret);
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_addr = 0;
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			goto fail;
		}
		h2cb_free(adapter, h2cb);
		h2cb = NULL;
		PLTFM_MSG_ERR("Write H2C, addr = 0x%x, length = %d\n", waddr, pkt_len);

		// delay for flash write
		//Sleep(1000);
		while (1) {
			PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
			if (adapter->flash_info.write_done == 1) {
				adapter->flash_info.writing = 0;
				adapter->flash_info.write_addr = 0;
				adapter->flash_info.write_done = 0;
				PLTFM_MSG_TRACE("write DONE\n");
				PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
				break;
			}
			PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
			//Sleep(1);
			PLTFM_DELAY_MS(1);
			if (no_timeout == 0) {
				local_timeout--;
				if (local_timeout == 0) {
					ret = MACFLASHFAIL;
					PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
					adapter->flash_info.writing = 0;
					adapter->flash_info.write_addr = 0;
					PLTFM_MSG_TRACE("write timeout\n");
					PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
					goto fail;
				}
			}
		}
		residue_len -= pkt_len;
		buffer += pkt_len;
		waddr += pkt_len;
	}
	PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
	adapter->flash_info.writing = 0;
	adapter->flash_info.write_addr = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	return MACSUCCESS;
fail:
	if (h2cb)
		h2cb_free(adapter, h2cb);
	adapter->fw_info.h2c_seq--;

	return ret;
}
#endif

u32 c2h_sys_flash_pkt(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		      struct rtw_c2h_info *info)
{
	u32 hdr0, i;
	u32 func, length, addr;
	u32 *pbuf;
	u32 *psource_data;

	PLTFM_MSG_TRACE("%s\n", __func__);
	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	//set info
	info->c2h_cat = GET_FIELD(hdr0, C2H_HDR_CAT);
	info->c2h_class = GET_FIELD(hdr0, C2H_HDR_CLASS);
	info->c2h_func = GET_FIELD(hdr0, C2H_HDR_FUNC);
	//info->done_ack = 0;
	//info->rec_ack = 0;
	info->content = buf  + FWCMD_HDR_LEN;
	//info->h2c_return = info->c2h_data[1];

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	if (func == FWCMD_H2C_FUNC_PLAT_FLASH_READ) {
		addr = *((u32 *)info->content);
		addr = le32_to_cpu(addr);
		length = *((u32 *)(info->content + LEN_FLASH_C2H_HDR_ADDR));
		length = le32_to_cpu(length);
		PLTFM_MSG_TRACE("addr = 0x%x, length = 0x%x\n", addr, length);
		PLTFM_MSG_TRACE("adapter->flash_info.read_addr = 0x%x\n",
				adapter->flash_info.read_addr);
		//PLTFM_MSG_TRACE("adapter->flash_info.buf_addr = 0x%llx\n",
				//(u64)adapter->flash_info.buf_addr);

		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		if (addr == adapter->flash_info.read_addr && adapter->flash_info.reading == 1) {
			// lost data if timeout happened
			//PLTFM_MSG_TRACE("ready to read\n");
			if (adapter->flash_info.buf_addr && length <= MAX_READ_SIZE) {
				PLTFM_MSG_TRACE("memcpy to buf\n");
				//PLTFM_MEMCPY(adapter->flash_info.buf_addr,
				//	     info->content + LEN_FLASH_C2H_HDR, length);
				pbuf = (u32 *)adapter->flash_info.buf_addr;
				psource_data = (u32 *)(info->content + LEN_FLASH_C2H_HDR);
				for (i = 0; i < length / sizeof(u32); i++) {
					*pbuf = le32_to_cpu(*psource_data);
					pbuf++;
					psource_data++;
				}
				//PLTFM_MSG_TRACE("memcpy to buf end\n");
			}
		}
		//PLTFM_MSG_TRACE("set reading = 0\n");
		adapter->flash_info.reading = 0;
		adapter->flash_info.read_done = 1;
		//PLTFM_MSG_TRACE("set reading = 0 end\n");
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
		//PLTFM_MSG_TRACE("exit critical\n");
	} else if (func == FWCMD_H2C_FUNC_PLAT_FLASH_WRITE) {
		addr = *((u32 *)info->content);
		addr = le32_to_cpu(addr);
		length = *((u32 *)(info->content + LEN_FLASH_C2H_HDR_ADDR));
		length = le32_to_cpu(length);
		PLTFM_MSG_TRACE("write addr = 0x%x\n", *((u32 *)info->content));
		PLTFM_MSG_TRACE("adapter->flash_info.write_addr = 0x%x\n",
				adapter->flash_info.write_addr);
		PLTFM_MSG_TRACE("write length = 0x%x\n", length);

		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		if (addr == adapter->flash_info.write_addr && adapter->flash_info.writing == 1) {
			adapter->flash_info.writing = 0;
			adapter->flash_info.write_done = 1;
		}
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	} else if (func == FWCMD_H2C_FUNC_PLAT_FLASH_ERASE) {
		addr = *((u32 *)info->content);
		addr = le32_to_cpu(addr);
		length = *((u32 *)(info->content + LEN_FLASH_C2H_HDR_ADDR));
		length = le32_to_cpu(length);
		PLTFM_MSG_TRACE("erase addr = 0x%x\n", *((u32 *)info->content));
		PLTFM_MSG_TRACE("adapter->flash_info.erase_addr = 0x%x\n",
				adapter->flash_info.erase_addr);
		PLTFM_MSG_TRACE("erase length = 0x%x\n", length);

		PLTFM_MUTEX_LOCK(&adapter->flash_info.lock);
		if (addr == adapter->flash_info.erase_addr && adapter->flash_info.erasing == 1) {
			adapter->flash_info.erasing = 0;
			adapter->flash_info.erase_done = 1;
		}
		PLTFM_MUTEX_UNLOCK(&adapter->flash_info.lock);
	}

	return MACSUCCESS;
}

