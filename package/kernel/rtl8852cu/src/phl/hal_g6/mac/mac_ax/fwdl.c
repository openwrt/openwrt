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

#include "fwdl.h"
#define SUPPORT_52BP 1
static inline void fwhdr_section_parser(struct mac_ax_adapter *adapter,
					struct fwhdr_section_t *section,
					struct fwhdr_section_info *info)
{
	u32 hdr_val;
	u32 section_len;

	hdr_val = le32_to_cpu(section->dword1);
	section_len = GET_FIELD(hdr_val, SECTION_INFO_SEC_SIZE);
	if (hdr_val & SECTION_INFO_CHECKSUM)
		section_len += FWDL_SECTION_CHKSUM_LEN;
	info->type = GET_FIELD(hdr_val, SECTION_INFO_SECTIONTYPE);
	if (info->type == FWDL_SECURITY_SECTION_TYPE)
		info->mssc = le32_to_cpu(section->dword2);
	else
		info->mssc = 0;
	info->len = section_len;
	info->redl = (hdr_val & SECTION_INFO_REDL) ? 1 : 0;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		info->dladdr = (GET_FIELD(le32_to_cpu(section->dword0),
		SECTION_INFO_SEC_DL_ADDR)) & 0x1FFFFFFF;
	} else {
		info->dladdr = (GET_FIELD(le32_to_cpu(section->dword0),	SECTION_INFO_SEC_DL_ADDR));
	}
}

static inline u32 fwhdr_hdr_parser(struct mac_ax_adapter *adapter, struct fwhdr_hdr_t *hdr,
				   struct fw_bin_info *info)
{
	u32 hdr_val;
	u32 val32;

	hdr_val = le32_to_cpu(hdr->dword6);
	info->section_num = GET_FIELD(hdr_val, FWHDR_SEC_NUM);

	hdr_val = le32_to_cpu(hdr->dword3);
	info->hdr_len = GET_FIELD(hdr_val, FWHDR_FWHDR_SZ);

	/* fill HALMAC information */
	hdr_val = le32_to_cpu(hdr->dword7);
	hdr_val = SET_CLR_WORD(hdr_val, FWDL_SECTION_PER_PKT_LEN,
			       FWHDR_FW_PART_SZ);
	info->dynamic_hdr_en = GET_FIELD(hdr_val, FWHDR_FW_DYN_HDR);
	hdr->dword7 = cpu_to_le32(hdr_val);

	if (info->dynamic_hdr_en) {
		info->dynamic_hdr_len = info->hdr_len - (FWHDR_HDR_LEN +
							 info->section_num * FWHDR_SECTION_LEN);
		val32 = le32_to_cpu(*(u32 *)(((u8 *)hdr) +
					     (FWHDR_HDR_LEN +
					      info->section_num * FWHDR_SECTION_LEN)));
		if (val32 != info->dynamic_hdr_len) {
			PLTFM_MSG_ERR("[ERR]%s Dynamic Header Len miss match\n", __func__);
			PLTFM_MSG_ERR("[ERR]Recorded Len (0x%X) != Calculated Len (0x%X)\n", val32,
				      info->dynamic_hdr_len);
			return MACCMP;
		}
		PLTFM_MSG_TRACE("[TRACE]%s:FW Use Dynamic Header:0x%X\n", __func__,
				info->dynamic_hdr_en);
		PLTFM_MSG_TRACE("[TRACE]%s:FW Dynamic Header Len:0x%X\n", __func__, val32);
	} else {
		val32 = FWHDR_HDR_LEN + info->section_num * FWHDR_SECTION_LEN;
		if (val32 != info->hdr_len) {
			PLTFM_MSG_ERR("[ERR]%s Static Header Len miss match\n", __func__);
			PLTFM_MSG_ERR("[ERR]Recorded Len (0x%X) != Calculated Len (0x%X)\n", val32,
				      info->hdr_len);
			return MACCMP;
		}
		info->dynamic_hdr_len = 0;
	}

	hdr_val = le32_to_cpu(hdr->dword2);
	info->git_idx = GET_FIELD(hdr_val, FWHDR_COMMITID);
	return MACSUCCESS;
}

static u32 get_ple_base(struct mac_ax_adapter *adapter)
{
	u32 chip_id;
	u32 ple_base = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	chip_id = GET_FIELD(MAC_REG_R32(R_AX_SYS_CHIPINFO), B_AX_HW_ID);

	switch (chip_id) {
#ifdef MAC_8852A_SUPPORT
	case RTL8852A_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8852A;
		break;
#endif
#ifdef MAC_8852B_SUPPORT
	case RTL8852B_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8852B;
		break;
#endif
#ifdef MAC_8852C_SUPPORT
	case RTL8852C_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8852C;
		break;
#endif
#ifdef MAC_8192XB_SUPPORT
	case RTL8192XB_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8192XB;
		break;
#endif
#ifdef MAC_8851B_SUPPORT
	case RTL8851B_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8851B;
		break;
#endif
#ifdef MAC_8851E_SUPPORT
	case RTL8851E_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8851E;
		break;
#endif
#ifdef MAC_8852D_SUPPORT
	case RTL8852D_ID:
		ple_base = FWDL_PLE_BASE_ADDR_8852D;
		break;
#endif
	default:
		PLTFM_MSG_ERR("[ERR]%s: invalid chip\n", __func__);
		ple_base = 0;
		break;
	}
	return ple_base;
}

static s8 __mss_index(struct mac_ax_adapter *adapter)
{
	s8 ret = 0;
	u8 externalPN = 0;
	u8 customer = 0;
	u8 serialNum = 0;
	u8 b1;
	u8 b2;
	u8 i;
	enum mac_ax_efuse_bank bank = MAC_AX_EFUSE_BANK_WIFI;
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	if (ops->read_efuse(adapter, EFUSE_EXTERNALPN_ADDR, 1, &b1, bank) != 0) {
		PLTFM_MSG_ERR("[ERR]%s: Read efuse 0x5EC failed.\n", __func__);
		return -1;
	}
	if (ops->read_efuse(adapter, EFUSE_CUSTOMER_ADDR, 1, &b2, bank) != 0) {
		PLTFM_MSG_ERR("[ERR]%s: Read efuse 0x5ED failed.\n", __func__);
		return -1;
	}
	b1 = le32_to_cpu(b1);
	b2 = le32_to_cpu(b2);
	externalPN = 0xFF - GET_FIELD(b1, EFUSE_EXTERNALPN);
	customer = 0xF - GET_FIELD(b2, EFUSE_CUSTOMER);
	serialNum = 0x7 - GET_FIELD(b2, EFUSE_SERIALNUM);
	PLTFM_MSG_ALWAYS("[DBG]%s: External PN %x\n", __func__, externalPN);
	PLTFM_MSG_ALWAYS("[DBG]%s: customer %x\n", __func__, customer);
	PLTFM_MSG_ALWAYS("[DBG]%s: Serial Num %x\n", __func__, serialNum);
	for (i = 0; i < OTP_KEY_INFO_NUM; i++) {
		if (externalPN == otp_key_info_externalPN[i] &&
		    customer == otp_key_info_customer[i] &&
		    serialNum == otp_key_info_serialNum[i]) {
			ret = (s8)i;
			break;
		}
	}
	PLTFM_MSG_ALWAYS("[DBG]%s: ret %d\n", __func__, ret);
	return ret;
}

static u32 fwhdr_parser(struct mac_ax_adapter *adapter, u8 *fw, u32 len,
			struct fw_bin_info *info)
{
	u32 i;
	u8 *fw_end = fw + len;
	u8 *bin_ptr;
	struct fwhdr_section_info *cur_section_info;
	u32 ret;
	u32 fwdl_ple_base;
	enum DLE_RSVD_INFO dle_info;
	u8 *mss_start;
	u8 *mss_selected;
	s8 mss_idx;

	if (!info) {
		PLTFM_MSG_ERR("[ERR]%s: *info = NULL\n", __func__);
		return MACNPTR;
	} else if (!fw) {
		PLTFM_MSG_ERR("[ERR]%s: *fw = NULL\n", __func__);
		return MACNOITEM;
	} else if (!len) {
		PLTFM_MSG_ERR("[ERR]%s: len = 0\n", __func__);
		return MACBUFSZ;
	}

	fwdl_ple_base = get_ple_base(adapter);
	ret = fwhdr_hdr_parser(adapter, (struct fwhdr_hdr_t *)fw, info);
	if (ret)
		return ret;
	bin_ptr = fw + info->hdr_len;

	/* jump to section header */
	fw += FWHDR_HDR_LEN;
	cur_section_info = info->section_info;
	info->is_fw_use_ple = 0;
	for (i = 0; i < info->section_num; i++) {
		fwhdr_section_parser(adapter, (struct fwhdr_section_t *)fw,
				     cur_section_info);
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
			if (cur_section_info->type == FWDL_SECURITY_SECTION_TYPE) {
				cur_section_info->len = 2048;
				((struct fwhdr_section_t *)fw)->dword1 = 0x09000800; //0x800 = 2048
			}
		}
		cur_section_info->addr = bin_ptr;
		bin_ptr += cur_section_info->len;
		if (cur_section_info->dladdr == fwdl_ple_base)
			info->is_fw_use_ple = 1;
		if (cur_section_info->type == FWDL_SECURITY_SECTION_TYPE &&
			cur_section_info->mssc > 0) {
			fw_end -= (cur_section_info->mssc * FWDL_SECURITY_SIGLEN);
			mss_start = cur_section_info->addr + FWDL_SECURITY_SECTION_CONSTANT;
			mss_idx = __mss_index(adapter);
			if (mss_idx < 0) {
				PLTFM_MSG_ERR("[ERR]%s:", __func__);
				PLTFM_MSG_ERR("Failed to get secure info offset\n");
				return MACFWBIN;
			}
			mss_selected = bin_ptr + (mss_idx * FWDL_SECURITY_SIGLEN);
			PLTFM_MEMCPY(mss_start, mss_selected, FWDL_SECURITY_SIGLEN);
			if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
				// Workaround for 1344 workaround,
				// Secure Boot CAN NOT have 1344 workaround
				// Assuming if mss_idx>0, than we are in secure boot.
				// Sub 1088 (1344-256) from cur_section_info->len
				if (mss_idx > 0) {
					//cur_section_info->len should be 960
					cur_section_info->len = 960;
					//0x3C0=960
					((struct fwhdr_section_t *)fw)->dword1 = 0x090003C0;
				}
			}
		}
		fw += FWHDR_SECTION_LEN;
		cur_section_info++;
	}

	ret = get_dle_rsvd_info(adapter, (enum DLE_RSVD_INFO *)&dle_info);
	if (ret != MACSUCCESS)
		return ret;
	// Check DLE reserved quota for FW
	if (dle_info != DLE_RSVD_INFO_FW && info->is_fw_use_ple) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("fw try to download something to ple illegally\n");
		return MACFWBIN;
	}

	if (fw_end != bin_ptr) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("fw bin size (0x%x) != fw size in fwhdr (0x%x)\n",
			      len, (u32)(bin_ptr - fw));
		return MACFWBIN;
	}

	return MACSUCCESS;
}

static inline u32 update_fw_ver(struct mac_ax_adapter *adapter,
				struct fwhdr_hdr_t *hdr)
{
	u32 hdr_val;
	struct mac_ax_fw_info *info = &adapter->fw_info;

	hdr_val = le32_to_cpu(hdr->dword1);
	info->major_ver = GET_FIELD(hdr_val, FWHDR_MAJORVER);
	info->minor_ver = GET_FIELD(hdr_val, FWHDR_MINORVER);
	info->sub_ver = GET_FIELD(hdr_val, FWHDR_SUBVERSION);
	info->sub_idx = GET_FIELD(hdr_val, FWHDR_SUBINDEX);

	hdr_val = le32_to_cpu(hdr->dword5);
	info->build_year = GET_FIELD(hdr_val, FWHDR_YEAR);

	hdr_val = le32_to_cpu(hdr->dword4);
	info->build_mon = GET_FIELD(hdr_val, FWHDR_MONTH);
	info->build_date = GET_FIELD(hdr_val, FWHDR_DATE);
	info->build_hour = GET_FIELD(hdr_val, FWHDR_HOUR);
	info->build_min = GET_FIELD(hdr_val, FWHDR_MIN);

	info->h2c_seq = 0;
	info->rec_seq = 0;

	return MACSUCCESS;
}

u32 mac_get_dynamic_hdr_ax(struct mac_ax_adapter *adapter, u8 *fw, u32 fw_len)
{
	u32 ret = MACSUCCESS;
	struct fw_bin_info info;
	u32 dynamic_hdr_len = 0;
	u32 dynamic_hdr_count = 0;
	u16 dynamic_section_len = 0;
	u8 dynamic_section_type = 0;
	u32 val32 = 0;
	u32 total_len = 0;
	u8 *dynamic_hdr_content;
	u8 *content;

	adapter->fw_info.cap_size = 0;
	PLTFM_MEMSET(adapter->fw_info.cap_buff, 0, MAC_WLANFW_CAP_MAX_SIZE);

	if (!fw) {
		PLTFM_MSG_ERR("[ERR]%s: FW == NULL\n", __func__);
		return MACNOFW;
	}
	ret = fwhdr_hdr_parser(adapter, (struct fwhdr_hdr_t *)fw, &info);
	if (ret)
		return ret;
	if (!info.dynamic_hdr_en) {
		PLTFM_MSG_TRACE("[TRACE]%s: WCPU Dynamic Header Disabled\n", __func__);
		return MACSUCCESS;
	}
	PLTFM_MSG_TRACE("[TRACE]%s: WCPU Dynamic Header Enabled\n", __func__);
	dynamic_hdr_content = fw + (FWHDR_HDR_LEN + info.section_num * FWHDR_SECTION_LEN);
	dynamic_hdr_len = le32_to_cpu(*(u32 *)dynamic_hdr_content);
	dynamic_hdr_count = le32_to_cpu(*(u32 *)(dynamic_hdr_content + 4));
	dynamic_hdr_content += 8;
	total_len += 8;
	PLTFM_MSG_TRACE("[TRACE]%s: dynamic_hdr_len:0x%X\n", __func__, dynamic_hdr_len);
	PLTFM_MSG_TRACE("[TRACE]%s: dynamic_hdr_count:0x%X\n", __func__, dynamic_hdr_count);
	// Check Dynamic Header length match first, so we won't need to malloc and free if failed.
	for (val32 = 0; val32 < dynamic_hdr_count; val32++) {
		dynamic_section_len = le16_to_cpu(*(u16 *)(dynamic_hdr_content));
		dynamic_section_type = *(u8 *)(dynamic_hdr_content + 2);
		PLTFM_MSG_TRACE("[TRACE]%s: Dynamic Hdr Section %d Len:0x%X\n", __func__,
				val32, dynamic_section_len);
		PLTFM_MSG_TRACE("[TRACE]%s: Dynamic Hdr Section %d Type:0x%X\n", __func__,
				val32, dynamic_section_type);
		total_len += dynamic_section_len;
		dynamic_hdr_content += dynamic_section_len;
	}
	if (total_len % 16)
		total_len += (16 - (total_len % 16));
	if (total_len != dynamic_hdr_len) {
		PLTFM_MSG_ERR("[ERR]%s: Dynamic Hdr Len Compare Fail:0x%X!=0x%X\n",
			      __func__, total_len, dynamic_hdr_len);
		return MACCMP;
	}
	// Restore dynamic_hdr_content, reparsing from dynamic head again
	dynamic_hdr_content = fw + (FWHDR_HDR_LEN + info.section_num * FWHDR_SECTION_LEN);
	dynamic_hdr_content += 8;
	for (val32 = 0; val32 < dynamic_hdr_count; val32++) {
		dynamic_section_len = le16_to_cpu(*(u16 *)(dynamic_hdr_content));
		dynamic_section_type = *(u8 *)(dynamic_hdr_content + 2);
		content = dynamic_hdr_content + 4;
		//switch CPU
		switch (dynamic_section_type) {
		case FWDL_DYNAMIC_HDR_FWCAP:
			if (dynamic_section_len - 4 > MAC_WLANFW_CAP_MAX_SIZE) {
				PLTFM_MSG_ERR("[ERR]%s: Dynamic Hdr Len 0x%X Exceed Fix Len 0x%X\n",
					      __func__, dynamic_section_len - 4,
					      MAC_WLANFW_CAP_MAX_SIZE);
				return MACNOBUF;
			}
			PLTFM_MEMCPY(adapter->fw_info.cap_buff, content, dynamic_section_len - 4);
			adapter->fw_info.cap_size = dynamic_section_len - 4;
			break;
		case FWDL_DYNAMIC_HDR_NOUSE:
		case FWDL_DYNAMIC_HDR_MAX:
		default:
			PLTFM_MSG_ERR("[ERR]%s: Dynamic Hdr Type Unused or Undefind:0x%X\n",
				      __func__, dynamic_section_type);
			return MACNOITEM;
		}
		dynamic_hdr_content += dynamic_section_len;
	}

	return ret;
}

static u32 __fwhdr_download(struct mac_ax_adapter *adapter,
			    u8 *fw, u32 hdr_len, u8 redl)
{
	u8 *buf;
	u32 ret = 0;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: h2cb_alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, hdr_len);
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: h2cb_put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, fw, hdr_len);

	if (redl) {
		ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
					   FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
					   FWCMD_H2C_CL_FWDL,
					   FWCMD_H2C_FUNC_FWHDR_REDL, 0, 0);
	} else {
		ret = h2c_pkt_set_hdr_fwdl(adapter, h2cb,
					   FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
					   FWCMD_H2C_CL_FWDL,
					   FWCMD_H2C_FUNC_FWHDR_DL, 0, 0);
	}

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: set h2c hdr fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: build h2c txd fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: PLTFM_TX fail\n", __func__);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	PLTFM_MSG_ERR("[ERR]%s ret: %d\n", __func__, ret);

	return ret;
}

#if MAC_AX_PHL_H2C
static u32 __sections_build_txd(struct mac_ax_adapter *adapter,
				struct rtw_h2c_pkt *h2cb)
{
	u8 *buf;
	u32 ret;
	u32 txd_len;
	struct rtw_t_meta_data info = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	info.type = RTW_PHL_PKT_TYPE_FWDL;
	info.pktlen = (u16)h2cb->data_len;
	txd_len = ops->txdesc_len(adapter, &info);

	buf = h2cb_push(h2cb, txd_len);
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: h2cb_push fail\n", __func__);
		return MACNPTR;
	}

	ret = ops->build_txdesc(adapter, &info, buf, txd_len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("build_txdesc fail\n");
		return ret;
	}

	return MACSUCCESS;
}

static u32 __sections_push(struct rtw_h2c_pkt *h2cb)
{
#define section_push_len 8
	h2cb->vir_data -= section_push_len;
	h2cb->vir_tail -= section_push_len;

	return MACSUCCESS;
}

#else
static u32 __sections_build_txd(struct mac_ax_adapter *adapter,
				struct h2c_buf *h2cb)
{
	u8 *buf;
	u32 ret;
	u32 txd_len;
	struct rtw_t_meta_data info;
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	info.type = RTW_PHL_PKT_TYPE_FWDL;
	info.pktlen = (u16)h2cb->len;
	txd_len = ops->txdesc_len(adapter, &info);

	buf = h2cb_push(h2cb, txd_len);
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: h2cb_push fail\n", __func__);
		return MACNPTR;
	}

	ret = ops->build_txdesc(adapter, &info, buf, txd_len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("mac_build_txdesc fail\n");
		return ret;
	}

	return MACSUCCESS;
}
#endif
static u32 __sections_download(struct mac_ax_adapter *adapter,
			       struct fwhdr_section_info *info)
{
	u8 *section = info->addr;
	u32 residue_len = info->len;
	u32 pkt_len;
	u8 *buf;
	u32 ret = 0;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	while (residue_len) {
		if (residue_len >= FWDL_SECTION_PER_PKT_LEN)
			pkt_len = FWDL_SECTION_PER_PKT_LEN;
		else
			pkt_len = residue_len;

		h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
		if (!h2cb) {
			PLTFM_MSG_ERR("[ERR]%s: ", __func__);
			PLTFM_MSG_ERR("h2cb_alloc fail\n");
			return MACNPTR;
		}
		#if MAC_AX_PHL_H2C
		__sections_push(h2cb);
		#endif
		buf = h2cb_put(h2cb, pkt_len);
		if (!buf) {
			PLTFM_MSG_ERR("[ERR]%s: ", __func__);
			PLTFM_MSG_ERR("h2cb_put fail\n");
			ret = MACNOBUF;
			goto fail;
		}

		PLTFM_MEMCPY(buf, section, pkt_len);

		ret = __sections_build_txd(adapter, h2cb);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: ", __func__);
			PLTFM_MSG_ERR("__sections_build_txd fail\n");
			goto fail;
		}
		#if MAC_AX_PHL_H2C
		ret = PLTFM_TX(h2cb);
		#else
		ret = PLTFM_TX(h2cb->data, h2cb->len);
		#endif
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: PLTFM_TX fail\n", __func__);
			goto fail;
		}

		h2cb_free(adapter, h2cb);

		section += pkt_len;
		residue_len -= pkt_len;
	}

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	PLTFM_MSG_ERR("[ERR]%s ret: %d\n", __func__, ret);

	return ret;
}

static u32 __write_memory(struct mac_ax_adapter *adapter,
			  u8 *buffer, u32 addr, u32 len)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 *content = NULL;
	u32 dl_size;
	u32 target_addr, write_addr;
	u32 seg_size, seg_bytes;
	u32 val32;
	u32 index = 0;
	u32 ret = MACSUCCESS;

	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]security mode ind accees\n");
		PLTFM_MSG_ERR("[ERR]Abort %s.\n", __func__);
		return MACIOERRINSEC;
	}

	PLTFM_MSG_WARN("%s ind access start\n", __func__);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;

	MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, addr);
	MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY, 0xAAAAAAAA);
	MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY + 4, 0xBBBBBBBB);

	val32 = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY);
	if (val32 != 0xAAAAAAAA) {
		ret = MACMEMRO;
		goto ind_aces_end;
	}

	val32 = MAC_REG_R32(R_AX_INDIR_ACCESS_ENTRY + 4);
	if (val32 != 0xBBBBBBBB) {
		ret = MACMEMRO;
		goto ind_aces_end;
	}

ind_aces_end:
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	PLTFM_MSG_WARN("%s ind access end\n", __func__);
	if (ret != MACSUCCESS)
		return ret;

	content = (u8 *)PLTFM_MALLOC(len);
	if (!content) {
		PLTFM_MSG_ERR("[ERR]%s: malloc fail\n", __func__);
		return MACNOBUF;
	}

	PLTFM_MEMCPY(content, buffer, len);

	dl_size = len;
	target_addr = addr;

	PLTFM_MSG_WARN("%s ind access trg 0x%X start\n", __func__, target_addr);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;
	while (dl_size != 0) {
		MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, target_addr);
		write_addr = R_AX_INDIR_ACCESS_ENTRY;

		if (dl_size >= ROMDL_SEG_LEN)
			seg_size = ROMDL_SEG_LEN;
		else
			seg_size = dl_size;

		seg_bytes = seg_size;

		while (seg_bytes != 0) {
			val32 = *((u32 *)(content + index));
			MAC_REG_W32(write_addr,
				    cpu_to_le32(val32));

			seg_bytes -= 4;
			write_addr += 4;
			index += 4;
		}

		target_addr += seg_size;
		dl_size -= seg_size;
	}
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	PLTFM_MSG_WARN("%s ind access trg 0x%X end\n", __func__, target_addr);

	PLTFM_FREE(content, len);

	return MACSUCCESS;
}

static u32 fwdl_phase0(struct mac_ax_adapter *adapter)
{
	u32 cnt = FWDL_WAIT_CNT;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_TRACE("[TRACE]%s:Start.\n", __func__);
	if (adapter->sm.fwdl != MAC_AX_FWDL_CPU_ON) {
		PLTFM_MSG_ERR("[ERR]%s: state != CPU_ON\n", __func__);
		return MACPROCERR;
	}

	while (--cnt) {
		if (MAC_REG_R8(R_AX_WCPU_FW_CTRL) & B_AX_H2C_PATH_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]%s: poll 0x1E0[1] = 1 fail\n", __func__);
		return MACPOLLTO;
	}

	adapter->sm.fwdl = MAC_AX_FWDL_H2C_PATH_RDY;
	PLTFM_MSG_TRACE("[TRACE]%s:End.\n", __func__);

	return MACSUCCESS;
}

static u32 fwdl_phase1(struct mac_ax_adapter *adapter,
		       u8 *fw, u32 hdr_len, u8 redl)
{
	u32 ret;
	u32 cnt = FWDL_WAIT_CNT;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	PLTFM_MSG_TRACE("[TRACE]%s:Start.\n", __func__);
	if (adapter->sm.fwdl != MAC_AX_FWDL_H2C_PATH_RDY) {
		PLTFM_MSG_ERR("[ERR]%s: state != H2C_PATH_RDY\n", __func__);
		return MACPROCERR;
	}

	ret = __fwhdr_download(adapter, fw, hdr_len, redl);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: __fwhdr_download fail\n", __func__);
		return ret;
	}

	while (--cnt) {
		if (MAC_REG_R8(R_AX_WCPU_FW_CTRL) & B_AX_FWDL_PATH_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]%s: poll 0x1E0[2] = 1 fail\n", __func__);
		return MACPOLLTO;
	}

	MAC_REG_W32(R_AX_HALT_H2C_CTRL, 0);
	MAC_REG_W32(R_AX_HALT_C2H_CTRL, 0);

	adapter->sm.fwdl = MAC_AX_FWDL_PATH_RDY;
	PLTFM_MSG_TRACE("[TRACE]%s:End.\n", __func__);

	return MACSUCCESS;
}

static u32 check_fw_rdy(struct mac_ax_adapter *adapter)
{
	u32 pre_val8;
	u32 val8 = FWDL_INITIAL_STATE;
	u32 cnt = FWDL_WAIT_CNT;
	u32 pre_bootstep = 0, cur_bootstep = 0, pre_secure_step = 0, cur_secure_step = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

#if defined(MAC_8852A_SUPPORT) || defined(MAC_8852B_SUPPORT) || defined(MAC_8851B_SUPPORT)
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		pre_bootstep = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG), B_AX_BOOT_STATUS);
		pre_secure_step = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG), B_AX_SECUREBOOT_STATUS);
	}
#endif
#if defined(MAC_8852C_SUPPORT) || defined(MAC_8192XB_SUPPORT) || \
defined(MAC_8851E_SUPPORT) || defined(MAC_8852D_SUPPORT)
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		pre_bootstep = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG_V1), B_AX_BOOT_STATUS);
		pre_secure_step = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG_V1), B_AX_SECUREBOOT_STATUS);
	}
#endif
	pre_val8 = GET_FIELD(MAC_REG_R8(R_AX_WCPU_FW_CTRL),
			     B_AX_WCPU_FWDL_STS);

	while (--cnt) {
#if defined(MAC_8852A_SUPPORT) || defined(MAC_8852B_SUPPORT) || defined(MAC_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			cur_bootstep = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG), B_AX_BOOT_STATUS);
			cur_secure_step = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG),
						    B_AX_SECUREBOOT_STATUS);
		}
#endif
#if defined(MAC_8852C_SUPPORT) || defined(MAC_8192XB_SUPPORT) || \
defined(MAC_8851E_SUPPORT) || defined(MAC_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			cur_bootstep = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG_V1), B_AX_BOOT_STATUS);
			cur_secure_step = GET_FIELD(MAC_REG_R32(R_AX_BOOT_DBG_V1),
						    B_AX_SECUREBOOT_STATUS);
		}
#endif
		val8 = GET_FIELD(MAC_REG_R8(R_AX_WCPU_FW_CTRL),
				 B_AX_WCPU_FWDL_STS);
		if (cur_bootstep != pre_bootstep) {
			PLTFM_MSG_TRACE("[TRACE]%s: BOOTSTEP 0x%x -> 0x%x\n",
					__func__, pre_bootstep, cur_bootstep);
			pre_bootstep = cur_bootstep;
		}
		if (cur_secure_step != pre_secure_step) {
			PLTFM_MSG_TRACE("[TRACE]%s: SECURE_STEP 0x%x -> 0x%x\n",
					__func__, pre_secure_step, cur_secure_step);
			pre_secure_step = cur_secure_step;
		}
		if (val8 != pre_val8) {
			PLTFM_MSG_TRACE("[TRACE]%s: 0x1E0[7:5] 0x%x -> 0x%x\n",
					__func__, pre_val8, val8);
			pre_val8 = val8;
		}
		if (val8 == FWDL_WCPU_FW_INIT_RDY) {
			break;
		} else if (val8 == FWDL_CHECKSUM_FAIL) {
			PLTFM_MSG_ERR("[ERR]%s: FWDL_DISPATCHER_CHECKSUM_FAIL\n", __func__);
			PLTFM_MSG_ERR("[ERR]%s: 0x1E0[7:5] last value = %d\n", __func__, val8);
			return MACFWCHKSUM;
		} else if (val8 == FWDL_SECURITY_FAIL) {
			PLTFM_MSG_ERR("[ERR]%s: FWDL_SECURITY_FAIL\n", __func__);
			PLTFM_MSG_ERR("[ERR]%s: 0x1E0[7:5] last value = %d\n", __func__, val8);
			return MACFWSECBOOT;
		} else if (val8 == FWDL_CUT_NOT_MATCH) {
			PLTFM_MSG_ERR("[ERR]%s: FWDL_CUT_NOT_MATCH\n", __func__);
			PLTFM_MSG_ERR("[ERR]%s: 0x1E0[7:5] last value = %d\n", __func__, val8);
			return MACFWCUT;
		}
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]%s: Polling 0x1E0[7:5] == 7 Timeout\n", __func__);
		PLTFM_MSG_ERR("[ERR]%s: 0x1E0[7:5] last value = %d\n", __func__, val8);
		return MACPOLLTO;
	}

	adapter->sm.fwdl = MAC_AX_FWDL_INIT_RDY;

	return MACSUCCESS;
}

static u32 fwdl_phase2(struct mac_ax_adapter *adapter, u8 *fw,
		       struct fw_bin_info *info, u8 redl)
{
	u32 ret;
	u32 section_num = info->section_num;
	struct fwhdr_section_info *section_info = info->section_info;

	PLTFM_MSG_TRACE("[TRACE]%s:Start.\n", __func__);
	if (adapter->sm.fwdl != MAC_AX_FWDL_PATH_RDY) {
		PLTFM_MSG_ERR("[ERR]%s: state != FWDL_PATH_RDY\n", __func__);
		return MACPROCERR;
	}
	PLTFM_MSG_TRACE("[TRACE]%s:Section Download Start.\n", __func__);
	while (section_num > 0) {
		PLTFM_MSG_TRACE("[TRACE]%s:Remain Section Count:%d\n", __func__, section_num);
		if (!redl) {
			ret = __sections_download(adapter, section_info);
			if (ret) {
				PLTFM_MSG_ERR("[ERR]%s: ", __func__);
				PLTFM_MSG_ERR("__sections_download fail\n");
				return ret;
			}
		} else {
			if (section_info->redl) {
				ret = __sections_download(adapter,
							  section_info);
				if (ret) {
					PLTFM_MSG_ERR("[ERR]%s: ", __func__);
					PLTFM_MSG_ERR("__sections_download ");
					PLTFM_MSG_ERR("fail\n");
					return ret;
				}
			}
		}
		section_info++;
		section_num--;
	}
	PLTFM_MSG_TRACE("[TRACE]%s:Section Send End.\n", __func__);
	PLTFM_DELAY_MS(5);
	PLTFM_MSG_TRACE("[TRACE]%s:Polling 0x1E0[7:5] = 7 Start\n", __func__);
	ret = check_fw_rdy(adapter);
	if (ret) {
		PLTFM_MSG_ERR("%s: check_fw_rdy fail\n", __func__);
		return ret;
	}
	PLTFM_MSG_TRACE("[TRACE]%s:Polling 0x1E0[7:5] = 7 End\n", __func__);
	PLTFM_MSG_TRACE("[TRACE]%s:End.\n", __func__);
	return MACSUCCESS;
}

static void fwdl_fail_dump(struct mac_ax_adapter *adapter,
			   struct fw_bin_info *info, u32 ret)
{
	u32 val32;
	u16 val16, index;
	u8 chip_id;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
#if MAC_AX_FEATURE_DBGPKG
	struct mac_ax_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_ax_dbgpkg dbg_val = {0};
	struct mac_ax_dbgpkg_en dbg_en = {0};
#endif

	chip_id = adapter->hw_info->chip_id;
	PLTFM_MSG_ERR("[ERR]fwdl ret = %d\n", ret);
	val32 = MAC_REG_R32(R_AX_WCPU_FW_CTRL);
	PLTFM_MSG_ERR("[ERR]fwdl 0x1E0 = 0x%x\n", val32);

#if defined(MAC_8852A_SUPPORT) || defined(MAC_8852B_SUPPORT) || defined(MAC_8851B_SUPPORT)
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		val16 = MAC_REG_R16(R_AX_BOOT_DBG + 2);
		PLTFM_MSG_ERR("[ERR]fwdl 0x83F2 = 0x%x\n", val16);
	}

#elif defined(MAC_8852C_SUPPORT) || defined(MAC_8192XB_SUPPORT) || \
defined(MAC_8851E_SUPPORT) || defined(MAC_8852D_SUPPORT)
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		val16 = MAC_REG_R16(R_AX_BOOT_DBG_V1 + 2);
		PLTFM_MSG_ERR("[ERR]fwdl 0x78F2 = 0x%x\n", val16);
	}
#else
#error Chip not define
#endif
	val32 = MAC_REG_R32(R_AX_UDM3);
	PLTFM_MSG_ERR("[ERR]fwdl 0x1FC = 0x%x\n", val32);

	val32 = info->git_idx;
	PLTFM_MSG_ERR("[ERR]fw git idx = 0x%x\n", val32);

	PLTFM_MUTEX_LOCK(&adapter->hw_info->dbg_port_lock);
	adapter->hw_info->dbg_port_cnt++;
	if (adapter->hw_info->dbg_port_cnt != 1) {
		PLTFM_MSG_ERR("[ERR]fwdl fail dump lock cnt %d\n",
			      adapter->hw_info->dbg_port_cnt);
		goto end;
	}

	MAC_REG_W32(R_AX_DBG_CTRL, 0xf200f2);
	val32 = MAC_REG_R32(R_AX_SYS_STATUS1);
	val32 = SET_CLR_WORD(val32, 0x1, B_AX_SEL_0XC0);
	MAC_REG_W32(R_AX_SYS_STATUS1, val32);

	for (index = 0; index < 15; index++) {
		switch (chip_id) {
#if defined(MAC_8852A_SUPPORT) || defined(MAC_8852B_SUPPORT) || defined(MAC_8851B_SUPPORT)
		case RTL8852A_ID:
		case RTL8852B_ID:
		case RTL8851B_ID:
			val32 = MAC_REG_R32(R_AX_DBG_PORT_SEL);
			break;
#endif
#if defined(MAC_8852C_SUPPORT) || defined(MAC_8192XB_SUPPORT) || \
defined(MAC_8851E_SUPPORT) || defined(MAC_8852D_SUPPORT)
		case RTL8852C_ID:
		case RTL8192XB_ID:
		case RTL8851E_ID:
		case RTL8852D_ID:
			val32 = MAC_REG_R32(R_AX_WLCPU_PORT_PC);
			break;
#endif
		default:
			val32 = 0xEAEAEAEA;
			PLTFM_MSG_ERR("Unknown Chip ID.\n");
			break;
		}
		PLTFM_MSG_ERR("[ERR]fw PC = 0x%x\n", val32);
		PLTFM_DELAY_US(10);
	}

	//unknown purpose dump, disable
	//mac_dump_ple_dbg_page(adapter, 0);

	pltfm_dbg_dump(adapter);
end:
#if MAC_AX_FEATURE_DBGPKG
	dbg_en.ss_dbg = 0;
	dbg_en.dle_dbg = 0;
	dbg_en.dmac_dbg = 0;
	dbg_en.cmac_dbg = 0;
	dbg_en.mac_dbg_port = 0;
	dbg_en.plersvd_dbg = 0;
	mac_ops->dbg_status_dump(adapter, &dbg_val, &dbg_en);
#endif
	adapter->hw_info->dbg_port_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->dbg_port_lock);
}

u32 mac_fwredl(struct mac_ax_adapter *adapter, u8 *fw, u32 len)
{
	u32 val32;
	u32 ret;
	struct fw_bin_info info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	//FWREDL funtion is available only on 8852B
	if (!(is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))) {
		PLTFM_MSG_ERR("%s: FWREDL is available only on 8852B/8851B\n",
			      __func__);
		return MACSUCCESS;
	}

	ret = fwhdr_parser(adapter, fw, len, &info);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: fwhdr_parser fail\n", __func__);
		goto fwdl_err;
	}

	if (!info.is_fw_use_ple) {
		PLTFM_MSG_WARN("[WARN]%s: no need to redownload\n", __func__);
		return MACSUCCESS;
	}
	ret = update_fw_ver(adapter, (struct fwhdr_hdr_t *)fw);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: update_fw_ver fail\n", __func__);
		goto fwdl_err;
	}

	val32 = MAC_REG_R32(R_AX_WCPU_FW_CTRL);
	val32 &= ~(B_AX_WCPU_FWDL_EN | B_AX_H2C_PATH_RDY | B_AX_FWDL_PATH_RDY);
	val32 = SET_CLR_WORD(val32, FWDL_INITIAL_STATE,
			     B_AX_WCPU_FWDL_STS);
	MAC_REG_W32(R_AX_WCPU_FW_CTRL, val32);

	adapter->sm.fwdl = MAC_AX_FWDL_H2C_PATH_RDY;

	ret = fwdl_phase1(adapter, fw, info.hdr_len - info.dynamic_hdr_len, 1);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: fwdl_phase1 fail\n", __func__);
		goto fwdl_err;
	}

	ret = fwdl_phase2(adapter, fw, &info, 1);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: fwdl_phase2 fail\n", __func__);
		goto fwdl_err;
	}

	mac_scanofld_reset_state(adapter);
	return MACSUCCESS;

fwdl_err:
	fwdl_fail_dump(adapter, &info, ret);

	return ret;
}

u32 mac_fwdl(struct mac_ax_adapter *adapter, u8 *fw, u32 len)
{
	u8 retry_cnt;
	u32 ret;
	struct fw_bin_info info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = 0;
	retry_cnt = 0;
	MAC_REG_W32(R_AX_UDM1, 0);
	if (!fw) {
		PLTFM_MSG_ERR("[ERR]%s: no fw\n", __func__);
		ret = MACNOFW;
		PLTFM_MEMSET(&info, 0, sizeof(struct fw_bin_info));
		return ret;
	}
	/* Move this fuinction outside the retry loop may be buggy.
	 * Since we've reed efuse in this function.
	 */
	ret = fwhdr_parser(adapter, fw, len, &info);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: fwhdr_parser fail\n", __func__);
		return ret;
	}

	ret = update_fw_ver(adapter, (struct fwhdr_hdr_t *)fw);
	if (ret)
		return ret;

	// FWDL retry, for 025 temp workaround
	while (retry_cnt < FWDL_TRY_CNT) {
		ret = fwdl_phase0(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: fwdl_phase0 fail\n", __func__);
			goto fwdl_err;
		}

		ret = fwdl_phase1(adapter, fw, info.hdr_len - info.dynamic_hdr_len, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: fwdl_phase1 fail\n", __func__);
			goto fwdl_err;
		}

		ret = fwdl_phase2(adapter, fw, &info, 0);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: fwdl_phase2 fail\n", __func__);
			goto fwdl_err;
		}

		ret = mac_get_dynamic_hdr_ax(adapter, fw, len);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: mac_get_dynamic_hdr_ax fail\n", __func__);
			goto fwdl_err;
		}

		return MACSUCCESS;

fwdl_err:
		retry_cnt++;
		PLTFM_MSG_ERR("[ERR]%s: Retry FWDL count %d\n", __func__, retry_cnt);
		// At most retry 2 times
		if (retry_cnt < FWDL_TRY_CNT) {
			ret = mac_disable_cpu(adapter);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]%s: mac_disable_cpu fail\n", __func__);
				fwdl_fail_dump(adapter, &info, ret);
				return ret;
			}

			ret = mac_enable_cpu(adapter, AX_BOOT_REASON_PWR_ON, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
				fwdl_fail_dump(adapter, &info, ret);
				return ret;
			}
			MAC_REG_W32(R_AX_UDM1, retry_cnt);
		} else {
			break;
		}
	}

	fwdl_fail_dump(adapter, &info, ret);

	return ret;
}

u32 mac_enable_cpu(struct mac_ax_adapter *adapter, u8 boot_reason, u8 dlfw)
{
	u32 val32, ret;
	u16 val16;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (MAC_REG_R32(R_AX_PLATFORM_ENABLE) & B_AX_WCPU_EN)
		return MACCPUSTATE;

	if (adapter->sm.fwdl != MAC_AX_FWDL_IDLE) {
		PLTFM_MSG_ERR("[ERR]%s: state != FWDL_IDLE\n", __func__);
		return MACPROCERR;
	}

	// Monitor PLE queue
	{
		u32 ple_que_ch0_empty, ple_que_ch1_empty;

		MAC_REG_W32(R_AX_PLE_DBG_FUN_INTF_CTL, 0x80070000);
		val32 = MAC_REG_R32(R_AX_PLE_DBG_FUN_INTF_DATA);
		PLTFM_MSG_ALWAYS("[TRACE]Set R_AX_PLE_DBG_FUN_INTF_CTL to  0x80070000");
		PLTFM_MSG_ALWAYS("[TRACE]R_AX_PLE_DBG_FUN_INTF_DATA = 0x%x\n", val32);

		ple_que_ch0_empty = (val32 & (1 << 8));
		ple_que_ch1_empty = (val32 & (1 << 9));
		if (ple_que_ch0_empty == 0) {
			PLTFM_MSG_ERR("PLE queue for PAXIDMA ch0 is not empty\n");
			//return MACWQBUSY;
		}
		if (ple_que_ch1_empty == 0) {
			PLTFM_MSG_ERR("PLE queue for PAXIDMA ch1 is not empty\n");
			//return MACWQBUSY;
		}
	}

	//FW cannot support too much log. Reset R_AX_LDM for FW debug config
	MAC_REG_W32(R_AX_LDM, 0);

	//Default open the debug mode
	val32 = MAC_REG_R32(R_AX_UDM0);
	//val32 |= BIT(0);
	//MAC_REG_W32(R_AX_UDM0, val32);

	//Clear SER status
	MAC_REG_W32(R_AX_HALT_H2C_CTRL, 0);
	MAC_REG_W32(R_AX_HALT_C2H_CTRL, 0);
	MAC_REG_W32(R_AX_HALT_H2C, 0);
	MAC_REG_W32(R_AX_HALT_C2H, 0);
	//Clear SER status end

	// write 1 clear for R_AX_HISR0(HALT_C2H ISR)
	val32 = MAC_REG_R32(R_AX_HISR0);
	MAC_REG_W32(R_AX_HISR0, val32);

	MAC_REG_W32(R_AX_SYS_CLK_CTRL,
		    MAC_REG_R32(R_AX_SYS_CLK_CTRL) | B_AX_CPU_CLK_EN);

	val32 = MAC_REG_R32(R_AX_WCPU_FW_CTRL);
	val32 &= ~(B_AX_WCPU_FWDL_EN | B_AX_H2C_PATH_RDY | B_AX_FWDL_PATH_RDY);
	val32 = SET_CLR_WORD(val32, FWDL_INITIAL_STATE,
			     B_AX_WCPU_FWDL_STS);

	if (dlfw)
		val32 |= B_AX_WCPU_FWDL_EN;

	MAC_REG_W32(R_AX_WCPU_FW_CTRL, val32);

	val16 = MAC_REG_R16(R_AX_BOOT_REASON);
	val16 = SET_CLR_WORD(val16, boot_reason, B_AX_BOOT_REASON);
	MAC_REG_W16(R_AX_BOOT_REASON, val16);

	//Set IDMEM share mode to default value because NIC/NICCE use different mode
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		val32 = MAC_REG_R32(R_AX_SEC_CTRL);
		val32 = SET_CLR_WORD(val32, 0x2, B_AX_SEC_IDMEM_SIZE_CONFIG);
		MAC_REG_W32(R_AX_SEC_CTRL, val32);
	}

	val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE);
	MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32 | B_AX_WCPU_EN);

	adapter->sm.fwdl = MAC_AX_FWDL_CPU_ON;

	if (!dlfw) {
		PLTFM_DELAY_MS(5);

		ret = check_fw_rdy(adapter);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]%s: ", __func__);
			PLTFM_MSG_ERR("check_fw_rdy fail\n");
			return ret;
		}
	}

    // Prevent sequence number in HALMAC and FW mismatching
	reset_lps_seq_num(adapter);

	return MACSUCCESS;
}

u32 mac_disable_cpu(struct mac_ax_adapter *adapter)
{
	u32 val32;

	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	adapter->sm.fwdl = MAC_AX_FWDL_IDLE;
	//todo: need to check cpu in safe state before reset CPU

	val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE);
	MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32 & ~B_AX_WCPU_EN);

	val32 = MAC_REG_R32(R_AX_WCPU_FW_CTRL);
	val32 &= ~(B_AX_WCPU_FWDL_EN | B_AX_H2C_PATH_RDY | B_AX_FWDL_PATH_RDY);
	MAC_REG_W32(R_AX_WCPU_FW_CTRL, val32);

	val32 = MAC_REG_R32(R_AX_SYS_CLK_CTRL);
	MAC_REG_W32(R_AX_SYS_CLK_CTRL, val32 & ~B_AX_CPU_CLK_EN);

	/* Disable WDT by Reset CPU CR
	 * Reset B_AX_APB_WRAP_EN will reset all CPU Local CR.
	 * Although reset B_AX_PLATFORM_EN will have same effect.
	 * This redundant flow represent as an explicit point,
	 * where we've reseted CPU Local CR, including WDT.
	 *
	 * After 52C, Disable WCPU Will Also Disable WDT
	 * So only 52A, 52B and 51B need to reset B_AX_APB_WRAP_EN
	 */
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE);
		MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32 & ~B_AX_APB_WRAP_EN);

		val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE);
		MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32 | B_AX_APB_WRAP_EN);
	}

	adapter->sm.plat = MAC_AX_PLAT_OFF;

	if (chk_patch_pcie_hang(adapter)) {
		val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE);
		MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32 & ~B_AX_PLATFORM_EN);

		val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE);
		MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32 | B_AX_PLATFORM_EN);
	}

	adapter->sm.plat = MAC_AX_PLAT_ON;

	return MACSUCCESS;
}

u32 mac_romdl(struct mac_ax_adapter *adapter, u8 *ROM, u32 ROM_addr, u32 len)
{
	u8 *content = NULL;
	u32 val32, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = mac_disable_cpu(adapter);
	if (ret)
		return ret;

	if (!ROM)
		return MACNOITEM;

	val32 = MAC_REG_R32(R_AX_SEC_CTRL);

	if (val32 & BIT(0)) {
		ret = __write_memory(adapter, ROM, ROM_addr, len);
		if (ret)
			return ret;
	} else {
		PLTFM_MSG_ERR("[ERR]%s: __write_memory fail\n", __func__);
		return MACSECUREON;
	}

	PLTFM_FREE(content, len);

	return MACSUCCESS;
}

u32 mac_ram_boot(struct mac_ax_adapter *adapter, u8 *fw, u32 len)
{
	u32 addr;
	u32 ret = 0, section_num = 1;
	struct fw_bin_info info = {0};
	struct fwhdr_section_info *section_info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (adapter->hw_info->is_sec_ic) {
		PLTFM_MSG_ERR("[ERR]security mode ind accees\n");
		PLTFM_MSG_ERR("[ERR]Abort %s.\n", __func__);
		return MACIOERRINSEC;
	}

	ret = mac_disable_cpu(adapter);
	if (ret)
		goto fwdl_err;

	ret = fwhdr_parser(adapter, fw, len, &info);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: fwhdr_parser fail\n", __func__);
		goto fwdl_err;
	}

	ret = update_fw_ver(adapter, (struct fwhdr_hdr_t *)fw);
	if (ret)
		goto fwdl_err;

	section_num = info.section_num;
	section_info = info.section_info;

	while (section_num > 0) {
		ret = __write_memory(adapter, section_info->addr,
				     section_info->dladdr, section_info->len);
		if (ret)
			goto fwdl_err;

		section_info++;
		section_num--;
	}

	addr = (0xb8003000 + R_AX_CPU_BOOT_ADDR) & 0x1FFFFFFF;
	PLTFM_MSG_WARN("%s ind access 0x%X start\n", __func__, addr);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;
	MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, addr);
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY, 0xB8970000);
	} else {
		MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY, 0x20100000);
	}
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	PLTFM_MSG_WARN("%s ind access 0x%X end\n", __func__, addr);

	ret = mac_enable_cpu(adapter, AX_BOOT_REASON_PWR_ON, 0);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
		goto fwdl_err;
	}

	PLTFM_DELAY_MS(10);

	ret = check_fw_rdy(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: check_fw_rdy fail\n", __func__);
		goto fwdl_err;
	}
	return MACSUCCESS;

fwdl_err:
	fwdl_fail_dump(adapter, &info, ret);

	return ret;
}

u32 mac_enable_fw(struct mac_ax_adapter *adapter, enum rtw_fw_type cat)
{
	u32 ret = MACSUCCESS;
#if defined(PHL_FEATURE_AP) || defined(PHL_FEATURE_NIC)
	u32 chip_id, cv;
	u32 fw_len = 0;
	u8 *fw = NULL;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum DLE_RSVD_INFO dle_info;

	chip_id = GET_FIELD(MAC_REG_R32(R_AX_SYS_CHIPINFO), B_AX_HW_ID);
	cv = GET_FIELD(MAC_REG_R32(R_AX_SYS_CFG1), B_AX_CHIP_VER);

	ret = get_dle_rsvd_info(adapter, (enum DLE_RSVD_INFO *)&dle_info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		return ret;
	}

	ret = mac_query_fw_buff(adapter, cat, &fw, &fw_len);

	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: fw selection fail\n", __func__);
		return ret;
	}

	ret = mac_disable_cpu(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: mac_disable_cpu fail\n", __func__);
		return ret;
	}

	/* _patch_otp_power_issue checks whether OTP arbiter switchs to WLAN. */
	/* If return err, print log but not return ret. */
	/* Re-DLFW executed by following mac_fwdl_ax can handle this issue. */
	if (chk_patch_otp_power_issue(adapter)) {
		ret = _patch_otp_power_issue(adapter);
		if (ret != MACSUCCESS)
			PLTFM_MSG_WARN("[WARN]%s: _patch_otp_power_issue fail\n", __func__);
	}

	ret = mac_enable_cpu(adapter, AX_BOOT_REASON_PWR_ON, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
		return ret;
	}

	ret = mac_fwdl(adapter, fw, fw_len);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
		return ret;
	}
	mac_scanofld_reset_state(adapter);

#endif /* #if defined(PHL_FEATURE_AP) || defined(PHL_FEATURE_NIC) */
	return ret;
}

u32 mac_query_fw_buff(struct mac_ax_adapter *adapter, enum rtw_fw_type cat, u8 **fw, u32 *fw_len)
{
	u32 ret = MACSUCCESS;
#if defined(PHL_FEATURE_AP) || defined(PHL_FEATURE_NIC)
	u32 chip_id, cv;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	enum DLE_RSVD_INFO dle_info;

	chip_id = GET_FIELD(MAC_REG_R32(R_AX_SYS_CHIPINFO), B_AX_HW_ID);
	cv = GET_FIELD(MAC_REG_R32(R_AX_SYS_CFG1), B_AX_CHIP_VER);

	ret = get_dle_rsvd_info(adapter, (enum DLE_RSVD_INFO *)&dle_info);
	if (ret != MACSUCCESS)
		return ret;

	switch (chip_id) {
#ifdef MAC_8852A_SUPPORT
	case RTL8852A_ID:
		switch (cv) {
#ifdef MAC_FW_8852A_U2
		case FWDL_CBV:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8852a_u2_ap;
				*fw = array_8852a_u2_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8852a_u2_nic;
				*fw = array_8852a_u2_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8852a_u2_wowlan;
				*fw = array_8852a_u2_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8852A_U2*/
#ifdef MAC_FW_8852A_U3
		case FWDL_CCV:
			// fall through
		default:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8852a_u3_ap;
				*fw = array_8852a_u3_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8852a_u3_nic;
				*fw = array_8852a_u3_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8852a_u3_wowlan;
				*fw = array_8852a_u3_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8852A_U3*/
		}
		break;
#endif /*MAC_8852A_SUPPORT*/
#ifdef MAC_8852B_SUPPORT
	case RTL8852B_ID:
		switch (cv) {
#ifdef MAC_FW_8852B_U2
		case FWDL_CBV:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				PLTFM_MSG_ERR("[ERR]%s: 8852b does not have ap image\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NICCE
			case RTW_FW_NIC_CE:
				*fw_len = array_length_8852b_u2_nicce;
				*fw = array_8852b_u2_nicce;
				break;
#endif /*MAC_FW_CATEGORY_NICCE*/

#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852B) {
					if (dle_info == DLE_RSVD_INFO_FW) {
						PLTFM_MSG_WARN("PLE FW is not in use\n");
					    // disable PLE FW temporarily
					    // *fw_len = array_length_8852b_u2_nic_ple;
					    // *fw = array_8852b_u2_nic_ple;
						*fw_len = array_length_8852b_u2_nic;
						*fw = array_8852b_u2_nic;
					} else {
						*fw_len = array_length_8852b_u2_nic;
						*fw = array_8852b_u2_nic;
					}
				} else if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852BP) {
#if (SUPPORT_52BP && defined(MAC_FW_CATEGORY_NIC_BPLUS))
					*fw_len = array_length_8852b_u2_nic_bplus;
					*fw = array_8852b_u2_nic_bplus;
#else
					PLTFM_MSG_ERR("Not support 52BP\n");
					*fw_len = 0;
					*fw = 0;
					ret = MACNOFW;
#endif /* (SUPPORT_52BP && defined(MAC_FW_CATEGORY_NIC_BPLUS))*/
				}
				break;
#endif /*MAC_FW_CATEGORY_NIC*/

#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852B) {
					*fw_len = array_length_8852b_u2_wowlan;
					*fw = array_8852b_u2_wowlan;
				} else if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852BP) {
#if (SUPPORT_52BP && defined(MAC_FW_CATEGORY_WOWLAN_BPLUS))
					*fw_len = array_length_8852b_u2_wowlan_bplus;
					*fw = array_8852b_u2_wowlan_bplus;
#else
					PLTFM_MSG_ERR("Not support 52BP\n");
					*fw_len = 0;
					*fw = 0;
					ret = MACNOFW;
#endif /*(SUPPORT_52BP && defined(MAC_FW_CATEGORY_WOWLAN_BPLUS)*/
				}
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8852B_U2*/
#ifdef MAC_FW_8852B_U3
		case FWDL_CCV:
			// fall through
		default:
			PLTFM_MSG_WARN("[WARN]%s: 8852B Fall Back to C Cut FW\n", __func__);
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				PLTFM_MSG_ERR("[ERR]%s: 8852b does not have ap image\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NICCE
			case RTW_FW_NIC_CE:
				*fw_len = array_length_8852b_u3_nicce;
				*fw = array_8852b_u3_nicce;
				break;
#endif /*MAC_FW_CATEGORY_NICCE*/

#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852B) {
					*fw_len = array_length_8852b_u3_nic;
					*fw = array_8852b_u3_nic;
				} else if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852BP) {
#if (SUPPORT_52BP && defined(MAC_FW_CATEGORY_NIC_BPLUS))
					*fw_len = array_length_8852b_u3_nic_bplus;
					*fw = array_8852b_u3_nic_bplus;
#else
					PLTFM_MSG_ERR("Not support 52BP\n");
					*fw_len = 0;
					*fw = 0;
					ret = MACNOFW;
#endif /*(SUPPORT_52BP && defined(MAC_FW_CATEGORY_NIC_BPLUS))*/
				}
				break;
#endif /*MAC_FW_CATEGORY_NIC*/

#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852B) {
					*fw_len = array_length_8852b_u3_wowlan;
					*fw = array_8852b_u3_wowlan;
				} else if (PLTFM_GET_CHIP_ID(void) == CHIP_WIFI6_8852BP) {
#if (SUPPORT_52BP && defined(MAC_FW_CATEGORY_WOWLAN_BPLUS))
					*fw_len = array_length_8852b_u3_wowlan_bplus;
					*fw = array_8852b_u3_wowlan_bplus;
#else
					PLTFM_MSG_ERR("Not support 52BP\n");
					*fw_len = 0;
					*fw = 0;
					ret = MACNOFW;
#endif
				}
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8852B_U3*/
		}
		break;
#endif /*MAC_8852B_SUPPORT*/
#ifdef MAC_8852C_SUPPORT
	case RTL8852C_ID:
		switch (cv) {
#ifdef MAC_FW_8852C_U1
		case FWDL_CAV:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8852c_u1_ap;
				*fw = array_8852c_u1_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8852c_u1_nic;
				*fw = array_8852c_u1_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8852c_u1_wowlan;
				*fw = array_8852c_u1_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
				}
				break;
#endif /*MAC_FW_8852C_U1*/
#ifdef MAC_FW_8852C_U2
		case FWDL_CBV:
			// fall through
		default:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8852c_u2_ap;
				*fw = array_8852c_u2_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8852c_u2_nic;
				*fw = array_8852c_u2_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8852c_u2_wowlan;
				*fw = array_8852c_u2_wowlan;
				break;
#endif /*#ifdef MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8852C_U2*/
		}
		break;
#endif /*MAC_8852C_SUPPORT*/
#ifdef MAC_8192XB_SUPPORT
	case RTL8192XB_ID:
		switch (cv) {
#ifdef MAC_FW_8192XB_U1
		case FWDL_CAV:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8192xb_u1_ap;
				*fw = array_8192xb_u1_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8192xb_u1_nic;
				*fw = array_8192xb_u1_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/

#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8192xb_u1_wowlan;
				*fw = array_8192xb_u1_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8192XB_U1*/
#ifdef MAC_FW_8192XB_U2
		case FWDL_CBV:
			// fall through
		default:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8192xb_u2_ap;
				*fw = array_8192xb_u2_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8192xb_u2_nic;
				*fw = array_8192xb_u2_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/

#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8192xb_u2_wowlan;
				*fw = array_8192xb_u2_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8192XB_U2*/
		}
		break;
#endif /*MAC_8192XB_SUPPORT*/
#ifdef MAC_8851B_SUPPORT
	case RTL8851B_ID:
		switch (cv) {
#ifdef MAC_FW_8851B_U1
		case FWDL_CAV:
			// fall through
		default:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				PLTFM_MSG_ERR("[ERR]%s: 8851b does not have ap image\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				if (dle_info == DLE_RSVD_INFO_FW) {
#ifdef MAC_FW_CATEGORY_NIC_PLE
					PLTFM_MSG_WARN("PLE FW is not in use\n");
					// disable PLE FW temporarily
					//#ifdef MAC_FW_CATEGORY_NIC_PLE
					// *fw_len = array_length_8851b_u1_nic_ple;
					// *fw = array_8851b_u1_nic_ple;
					//#endif //MAC_FW_CATEGORY_NIC_PLE
					*fw_len = array_length_8851b_u1_nic;
					*fw = array_8851b_u1_nic;
#else
					*fw_len = array_length_8851b_u1_nic;
					*fw = array_8851b_u1_nic;
#endif
				} else {
					*fw_len = array_length_8851b_u1_nic;
					*fw = array_8851b_u1_nic;
				}
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8851b_u1_wowlan;
				*fw = array_8851b_u1_wowlan;
				break;
#endif /*#ifdef MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat\n", __func__);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8851B_U1*/
		}
		break;
#endif /*MAC_8851B_SUPPORT*/

#ifdef MAC_8851E_SUPPORT
	case RTL8851E_ID:
		switch (cv) {
#ifdef MAC_FW_8851E_U1
		case FWDL_CAV:
			// fall through
		default:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8851e_u1_ap;
				*fw = array_8851e_u1_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8851e_u1_nic;
				*fw = array_8851e_u1_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8851e_u1_wowlan;
				*fw = array_8851e_u1_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat:%d\n", __func__, cat);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8851E_U1*/
		}
		break;
#endif /*MAC_8851E_SUPPORT*/

#ifdef MAC_8852D_SUPPORT
	case RTL8852D_ID:
		switch (cv) {
#ifdef MAC_FW_8852D_U1
		case FWDL_CAV:
			// fall through
		default:
			switch (cat) {
#ifdef PHL_FEATURE_AP
#ifdef MAC_FW_CATEGORY_AP
			case RTW_FW_AP:
				*fw_len = array_length_8852d_u1_ap;
				*fw = array_8852d_u1_ap;
				break;
#endif /*MAC_FW_CATEGORY_AP*/
#endif /*PHL_FEATURE_AP*/
#ifdef PHL_FEATURE_NIC
#ifdef MAC_FW_CATEGORY_NIC
			case RTW_FW_NIC:
				*fw_len = array_length_8852d_u1_nic;
				*fw = array_8852d_u1_nic;
				break;
#endif /*MAC_FW_CATEGORY_NIC*/
#ifdef CONFIG_WOWLAN
#ifdef MAC_FW_CATEGORY_WOWLAN
			case RTW_FW_WOWLAN:
				*fw_len = array_length_8852d_u1_wowlan;
				*fw = array_8852d_u1_wowlan;
				break;
#endif /*MAC_FW_CATEGORY_WOWLAN*/
#endif /*CONFIG_WOWLAN*/
#endif /*PHL_FEATURE_NIC*/
			default:
				PLTFM_MSG_ERR("[ERR]%s: no cat:%d\n", __func__, cat);
				*fw_len = 0;
				fw = 0;
				ret = MACNOFW;
				break;
			}
			break;
#endif /*MAC_FW_8852D_U1*/
		}
		break;
#endif /*MAC_8852D_SUPPORT*/

	default:
		PLTFM_MSG_ERR("[ERR]%s: invalid chip\n", __func__);
		*fw_len = 0;
		fw = 0;
		ret = MACNOFW;
		break;
	}

#endif /* #if defined(PHL_FEATURE_AP) || defined(PHL_FEATURE_NIC) */
	return ret;
}

