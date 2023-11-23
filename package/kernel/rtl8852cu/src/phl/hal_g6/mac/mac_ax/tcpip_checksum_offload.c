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

#include "tcpip_checksum_offload.h"
#if MAC_AX_FW_REG_OFLD
u32 mac_tcpip_chksum_ofd(struct mac_ax_adapter *adapter,
			 u8 en_tx_chksum_ofd, u8 en_rx_chksum_ofd)
{
	u32 ret = 0;
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct mac_ax_en_tcpipchksum *content;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_en_tcpipchksum));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	content = (struct mac_ax_en_tcpipchksum *)buf;
	content->en_tx_chksum_ofd = en_tx_chksum_ofd;
	content->en_rx_chksum_ofd = en_rx_chksum_ofd;

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_TCPIP_CHKSUM_OFFLOAD_REG,
			      0,
			      1);

	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}
#else
u32 mac_tcpip_chksum_ofd(struct mac_ax_adapter *adapter,
			 u8 en_tx_chksum_ofd, u8 en_rx_chksum_ofd)
{
	u32 val;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (en_tx_chksum_ofd) {
		val = MAC_REG_R32(R_AX_TX_TCPIP_CHECKSUM_FUNCTION);
		val |= B_AX_HDT_TCPIP_CHKSUM_EN;
		MAC_REG_W32(R_AX_TX_TCPIP_CHECKSUM_FUNCTION, val);
	} else {
		val = MAC_REG_R32(R_AX_TX_TCPIP_CHECKSUM_FUNCTION);
		val &= (~B_AX_HDT_TCPIP_CHKSUM_EN);
		MAC_REG_W32(R_AX_TX_TCPIP_CHECKSUM_FUNCTION, val);
	}

	if (en_rx_chksum_ofd) {
		val = MAC_REG_R32(R_AX_RX_TCPIP_CHECKSUM_FUNCTION);
		val |= B_AX_HDR_TCPIP_CHKSUM_EN;
		MAC_REG_W32((R_AX_RX_TCPIP_CHECKSUM_FUNCTION), val);
	} else {
		val = MAC_REG_R32((R_AX_RX_TCPIP_CHECKSUM_FUNCTION));
		val &= (~B_AX_HDR_TCPIP_CHKSUM_EN);
		MAC_REG_W32((R_AX_RX_TCPIP_CHECKSUM_FUNCTION), val);
	}

	return MACSUCCESS;
}
#endif
u32 mac_chk_rx_tcpip_chksum_ofd(struct mac_ax_adapter *adapter,
				u8 chksum_status)
{
	u8 chk_val = (chksum_status & 0xF0);

	if (!(chk_val & MAC_AX_CHKOFD_TCP_CHKSUM_VLD))
		return MAC_AX_CHKSUM_OFD_HW_NO_SUPPORT;

	if (chk_val & MAC_AX_CHKOFD_TCP_CHKSUM_ERR)
		return MAC_AX_CHKSUM_OFD_CHKSUM_ERR;

	if (!(chk_val & MAC_AX_CHKOFD_RX_IS_TCP_UDP) &&
	    !(chk_val & MAC_AX_CHKOFD_RX_IPV))
		return MAC_AX_CHKSUM_OFD_IPV4_TCP_OK;

	if (!(chk_val & MAC_AX_CHKOFD_RX_IS_TCP_UDP) &&
	    (chk_val & MAC_AX_CHKOFD_RX_IPV))
		return MAC_AX_CHKSUM_OFD_IPV6_TCP_OK;

	if ((chk_val & MAC_AX_CHKOFD_RX_IS_TCP_UDP) &&
	    !(chk_val & MAC_AX_CHKOFD_RX_IPV))
		return MAC_AX_CHKSUM_OFD_IPV4_UDP_OK;

	return MAC_AX_CHKSUM_OFD_IPV6_UDP_OK;
}

