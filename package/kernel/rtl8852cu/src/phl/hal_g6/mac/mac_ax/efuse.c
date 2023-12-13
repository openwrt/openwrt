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
#include "efuse.h"
#include "mac_priv.h"

static struct mac_efuse_tbl efuse_tbl;
struct mac_bank_efuse_info bank_efuse_info;
u16 efuse_ctrl = R_AX_EFUSE_CTRL;
u16 read_efuse_cnt = EFUSE_WAIT_CNT;
bool OTP_test;
enum rtw_dv_sel dv_sel = DDV;

static struct efuse_info_item offset_pcie = {
	0x400, /* mac_addr */
	0, /* pid */
	0x408, /* did */
	0x406, /* vid */
	0x40A, /* svid */
	0x40C, /* smid */
};

static struct efuse_info_item offset_usb_8852a = {
	0x438, /* mac_addr */
	0x432, /* pid */
	0, /* did */
	0x430, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item offset_usb_8852b = {
	0x488, /* mac_addr */
	0x482, /* pid */
	0, /* did */
	0x480, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item offset_usb_8851b = {
	0x488, /* mac_addr */
	0x482, /* pid */
	0, /* did */
	0x480, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item offset_sdio = {
	0x41A, /* mac_addr */
	0, /* pid */
	0, /* did */
	0, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item def_val_pcie = {
	0x0, /* mac_addr */
	0, /* pid */
	0x52, /* did */
	0xEC, /* vid */
	0xEC, /* svid */
	0x52, /* smid */
};

static struct efuse_info_item def_val_usb_8852a = {
	0x0, /* mac_addr */
	0x5A, /* pid */
	0, /* did */
	0xDA, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item def_val_usb_8852b = {
	0x0, /* mac_addr */
	0x5B, /* pid */
	0, /* did */
	0xDA, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item def_val_usb_8851b = {
	0x0, /* mac_addr */
	0x5B, /* pid */
	0, /* did */
	0xDA, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item def_val_sdio = {
	0x0, /* mac_addr */
	0, /* pid */
	0, /* did */
	0, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item len_pcie = {
	6, /* mac_addr */
	0, /* pid */
	2, /* did */
	2, /* vid */
	2, /* svid */
	2, /* smid */
};

static struct efuse_info_item len_usb = {
	6, /* mac_addr */
	2, /* pid */
	0, /* did */
	2, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info_item len_sdio = {
	6, /* mac_addr */
	0, /* pid */
	0, /* did */
	0, /* vid */
	0, /* svid */
	0, /* smid */
};

static struct efuse_info efuse_info_pcie = {
	&offset_pcie, /* offset */
	&def_val_pcie, /* def_val */
	&len_pcie, /* len */
};

static struct efuse_info efuse_info_usb_8852a = {
	&offset_usb_8852a, /* offset */
	&def_val_usb_8852a, /* def_val */
	&len_usb, /* len */
};

static struct efuse_info efuse_info_usb_8852b = {
	&offset_usb_8852b, /* offset */
	&def_val_usb_8852b, /* def_val */
	&len_usb, /* len */
};

static struct efuse_info efuse_info_usb_8851b = {
	&offset_usb_8851b, /* offset */
	&def_val_usb_8851b, /* def_val */
	&len_usb, /* len */
};

static struct efuse_info efuse_info_sdio = {
	&offset_sdio, /* offset */
	&def_val_sdio, /* def_val */
	&len_sdio, /* len */
};

static u32 efuse_map_init(struct mac_ax_adapter *adapter,
			  enum efuse_map_sel map_sel);
static u32 efuse_fwcmd_ck(struct mac_ax_adapter *adapter);
static u32 efuse_proc_ck(struct mac_ax_adapter *adapter);
static u32 cnv_efuse_state(struct mac_ax_adapter *adapter, u8 dest_state);
static u32 proc_dump_efuse(struct mac_ax_adapter *adapter,
			   enum mac_ax_efuse_read_cfg cfg);
static u32 read_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
			 u8 *map);
static u32 write_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u8 value);
static u32 cmp_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u16 val);
static u32 eeprom_parser(struct mac_ax_adapter *adapter, u8 *phy_map,
			 u8 *log_map, enum mac_ax_efuse_parser_cfg cfg);
static u32 read_log_efuse_map(struct mac_ax_adapter *adapter, u8 *map,
			      u32 log_efuse_size);
static u32 proc_pg_efuse_by_map(struct mac_ax_adapter *adapter,
				struct mac_ax_pg_efuse_info *info,
				enum mac_ax_efuse_read_cfg cfg);
static u32 dump_efuse_drv(struct mac_ax_adapter *adapter);
static u32 dump_efuse_fw(struct mac_ax_adapter *adapter);
static u32 proc_write_log_efuse(struct mac_ax_adapter *adapter, u32 offset,
				u8 value);
static u32 read_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
		      u8 *map);
static u32 update_eeprom_mask(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask, bool pg_sim);
static u32 check_efuse_enough(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask);
static u32 proc_pg_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 word_en,
			 u8 pre_word_en, u32 eeprom_offset, bool pg_sim);
static u32 program_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 *updated_mask,
			 bool pg_sim);
static void mask_eeprom(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info);
static u32 query_status_map(struct mac_ax_adapter *adapter,
			    enum mac_ax_efuse_feature_id feature_id,
			    u8 *map, bool is_limit);
static u32 adjust_mask(struct mac_ax_adapter *adapter,
		       struct mac_ax_pg_efuse_info *info);
static u32 compare_info_length(struct efuse_info *info,
			       enum rtw_efuse_info id, u32 length);
static u32 set_check_sum_val(struct mac_ax_adapter *adapter,
			     u8 *map, u16 value);
static void cal_check_sum(struct mac_ax_adapter *adapter,
			  u16 *chksum);
static u32 compare_version(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info, u32 ver_len);
static u32 read_hw_efuse_dav(struct mac_ax_adapter *adapter, u32 offset, u32 size,
			     u8 *map);
static u32 write_hw_efuse_dav(struct mac_ax_adapter *adapter, u32 offset,
			      u8 value);
static void switch_dv(struct mac_ax_adapter *adapter, enum rtw_dv_sel);
static void free_map_mask(struct mac_ax_adapter *adapter, u32 ver_len,
			  struct mac_ax_pg_efuse_info *info_DAV,
			  struct mac_ax_pg_efuse_info *info_DDV);
static u32 proc_dump_hidden(struct mac_ax_adapter *adapter);

u32 mac_dump_efuse_map_wl_plus(struct mac_ax_adapter *adapter,
			       enum mac_ax_efuse_read_cfg cfg, u8 *efuse_map)
{
	u32 ret = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 chip_id = hw_info->chip_id;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		ret = mac_dump_efuse_map_wl(adapter, cfg, efuse_map);
	} else if ((chip_id == MAC_AX_CHIP_ID_8852B) ||
		   (chip_id == MAC_AX_CHIP_ID_8852C) ||
		   (chip_id == MAC_AX_CHIP_ID_8851E) ||
		   (chip_id == MAC_AX_CHIP_ID_8852D)) {
		ret = mac_dump_efuse_map_wl(adapter, cfg, efuse_map);
		if (ret)
			return ret;
		if (dav_exist) {
			switch_dv(adapter, DAV);
			ret = mac_dump_efuse_map_wl(adapter, cfg, efuse_map + hw_info->efuse_size);
			switch_dv(adapter, DDV);
		}
	} else {
		ret = mac_dump_efuse_map_wl(adapter, cfg, efuse_map);
		if (ret)
			return ret;
		if (dav_exist) {
			switch_dv(adapter, DAV);
			ret = mac_dump_efuse_map_wl(adapter, MAC_AX_EFUSE_R_DRV,
						    efuse_map + hw_info->efuse_size);
			switch_dv(adapter, DDV);
		}
	}

	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_dump_efuse_map_wl(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_read_cfg cfg, u8 *efuse_map)
{
	u32 ret, stat;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

#if 0
	if (cfg == MAC_AX_EFUSE_R_FW ||
	    (cfg == MAC_AX_EFUSE_R_AUTO &&
	     adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY))
		return MACNOITEM;
#else
	if (cfg == MAC_AX_EFUSE_R_FW &&
	    adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) // no fw file
		return MACNOFW;
#endif

	PLTFM_MSG_TRACE("[TRACE]cfg = %d\n", cfg);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	if (dv_sel == DAV)
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_DAV);
	else
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
	if (ret != 0)
		return ret;

	ret = proc_dump_efuse(adapter, cfg);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]dump efuse!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	query_status_map(adapter, MAC_AX_DUMP_PHYSICAL_EFUSE, efuse_map, 0);

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_dump_efuse_map_bt(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_read_cfg cfg, u8 *efuse_map)
{
	u32 ret, stat;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

#if 0
	if (cfg == MAC_AX_EFUSE_R_FW ||
	    (cfg == MAC_AX_EFUSE_R_AUTO &&
	     adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY))
		return MACNOITEM;
#else
	if (cfg == MAC_AX_EFUSE_R_FW &&
	    adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) // no fw file
		return MACNOFW;
#endif

	PLTFM_MSG_TRACE("[TRACE]cfg = %d\n", cfg);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_BT);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_BT);
	if (ret != 0)
		return ret;

	ret = proc_dump_efuse(adapter, cfg);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]dump efuse!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	query_status_map(adapter, MAC_AX_DUMP_PHYSICAL_EFUSE, efuse_map, 0);

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_write_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u8 val,
			 enum mac_ax_efuse_bank bank)
{
	u32 ret = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 chip_id = hw_info->chip_id;
	u32 efuse_size = hw_info->wl_efuse_size;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	switch (bank) {
	case MAC_AX_EFUSE_BANK_WIFI:
		if (chip_id == MAC_AX_CHIP_ID_8852A) {
			ret = mac_write_efuse(adapter, addr, val, bank);
		} else {
			if (addr < efuse_size) {
				ret = mac_write_efuse(adapter, addr, val, bank);
			} else {
				if (dav_exist) {
					switch_dv(adapter, DAV);
					ret = mac_write_efuse(adapter, addr -
							      efuse_size, val, bank);
					switch_dv(adapter, DDV);
				}
			}
		}
		break;
	case MAC_AX_EFUSE_BANK_BT:
		ret = mac_write_efuse(adapter, addr, val, bank);
		break;
	default:
		break;
	}
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_write_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val,
		    enum mac_ax_efuse_bank bank)
{
	u32 ret, stat, efuse_size = 0;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (bank == MAC_AX_EFUSE_BANK_WIFI) {
		if (OTP_test)
			efuse_size = OTP_PHY_SIZE;
		else
			efuse_size = adapter->hw_info->wl_efuse_size;
	} else if (bank == MAC_AX_EFUSE_BANK_BT) {
		efuse_size = adapter->hw_info->bt_efuse_size;
	} else {
		return MACEFUSEBANK;
	}

	if (addr >= efuse_size) {
		PLTFM_MSG_ERR("[ERR]Offset is too large\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, bank);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	p_ops->enable_efuse_sw_pwr_cut(adapter, 1);
	ret = write_hw_efuse(adapter, addr, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write physical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}
	p_ops->disable_efuse_sw_pwr_cut(adapter, 1);

	if (bank == MAC_AX_EFUSE_BANK_BT) {
		ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return ret;
}

u32 mac_read_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			u8 *val, enum mac_ax_efuse_bank bank)
{
	u32 ret = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 chip_id = hw_info->chip_id;
	u32 efuse_size = hw_info->wl_efuse_size;
	u32 size1, size2;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	switch (bank) {
	case MAC_AX_EFUSE_BANK_WIFI:
		if (chip_id == MAC_AX_CHIP_ID_8852A) {
			ret =  mac_read_efuse(adapter, addr, size, val, bank);
		} else {
			if (addr < efuse_size && addr + size <= efuse_size) {
				ret = mac_read_efuse(adapter, addr, size,
						     val, bank);
			} else if (addr >= efuse_size) {
				if (dav_exist) {
					switch_dv(adapter, DAV);
					ret = mac_read_efuse(adapter, addr - efuse_size,
							     size, val, bank);
					switch_dv(adapter, DDV);
				}
			} else if (addr < efuse_size &&
				   addr + size > efuse_size) {
				size1 = efuse_size - addr;
				size2 = addr + size - efuse_size;

				ret = mac_read_efuse(adapter, addr, size1,
						     val, bank);
				if (ret)
					return ret;
				if (dav_exist) {
					switch_dv(adapter, DAV);
					ret = mac_read_efuse(adapter, 0, size2,
							     val + size1, bank);
					switch_dv(adapter, DDV);
				}
			}
		}
		break;
	case MAC_AX_EFUSE_BANK_BT:
		ret =  mac_read_efuse(adapter, addr, size, val, bank);
		break;
	default:
		break;
	}
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_read_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size, u8 *val,
		   enum mac_ax_efuse_bank bank)
{
	u32 ret, stat, efuse_size = 0;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (bank == MAC_AX_EFUSE_BANK_WIFI) {
		if (OTP_test)
			efuse_size = OTP_PHY_SIZE;
		else
			efuse_size = adapter->hw_info->wl_efuse_size;
	} else if (bank == MAC_AX_EFUSE_BANK_BT) {
		efuse_size = adapter->hw_info->bt_efuse_size;
	} else {
		return MACEFUSEBANK;
	}

	if (addr >= efuse_size || addr + size > efuse_size) {
		PLTFM_MSG_ERR("[ERR] Wrong efuse index\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, bank);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = read_hw_efuse(adapter, addr, size, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]read hw efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	if (bank == MAC_AX_EFUSE_BANK_BT) {
		ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_read_hidden_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			  u8 *val, enum mac_ax_efuse_hidden_cfg hidden_cfg)
{
	u32 ret, stat;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 log_sec_size = hw_info->efuse_size + hw_info->sec_data_efuse_size;

	switch (hidden_cfg) {
	case MAC_AX_EFUSE_HIDDEN_RF:
		if (addr < log_sec_size || addr + size >
		    log_sec_size + hw_info->hidden_efuse_rf_size) {
			PLTFM_MSG_ERR("[ERR] Wrong hidden rf index\n");
			return MACEFUSESIZE;
		}
		break;
	default:
		PLTFM_MSG_ERR("[ERR] Hidden config invalid\n");
		return MACNOITEM;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_HIDDEN_RF);
	if (ret != 0)
		return ret;

	ret = proc_dump_hidden(adapter);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]dump hidden!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	PLTFM_MEMCPY(val, adapter->efuse_param.hidden_rf_map +
		     (addr - log_sec_size), size);

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

static u32 proc_dump_hidden(struct mac_ax_adapter *adapter)
{
	u8 *map = NULL;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 efuse_size = hw_info->efuse_size;
	u32 sec_data_efuse_size = hw_info->sec_data_efuse_size;
	u32 hidden_efuse_rf_size = hw_info->hidden_efuse_rf_size;
	u8 hidden_rf_map_valid = adapter->efuse_param.hidden_rf_map_valid;
	u32 ret;

	if (hidden_rf_map_valid == 0) {
		map = (u8 *)PLTFM_MALLOC(hidden_efuse_rf_size);
		if (!map) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}

		ret = read_hw_efuse(adapter, efuse_size + sec_data_efuse_size,
				    hidden_efuse_rf_size, map);
		if (ret != 0) {
			PLTFM_FREE(map, hidden_efuse_rf_size);
			return ret;
		}

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(adapter->efuse_param.hidden_rf_map, map, hidden_efuse_rf_size);
		adapter->efuse_param.hidden_rf_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
		PLTFM_FREE(map, hidden_efuse_rf_size);
	}

	return MACSUCCESS;
}

u32 mac_get_efuse_avl_size(struct mac_ax_adapter *adapter, u32 *size)
{
	u32 ret;
	u8 *map;
	u32 efuse_size = adapter->hw_info->log_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	map = (u8 *)PLTFM_MALLOC(efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}

	ret = mac_dump_log_efuse(adapter, MAC_AX_EFUSE_PARSER_MAP,
				 MAC_AX_EFUSE_R_AUTO, map, 0);

	PLTFM_FREE(map, efuse_size);

	if (ret != 0)
		return ret;

	*size = adapter->hw_info->efuse_size - efuse_param->efuse_end;

	return MACSUCCESS;
}

u32 mac_get_efuse_avl_size_bt(struct mac_ax_adapter *adapter, u32 *size)
{
	u32 ret;
	u8 *map;
	u32 efuse_size = adapter->hw_info->bt_log_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	map = (u8 *)PLTFM_MALLOC(efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}

	ret = mac_dump_log_efuse_bt(adapter, MAC_AX_EFUSE_PARSER_MAP,
				    MAC_AX_EFUSE_R_DRV, map);

	PLTFM_FREE(map, efuse_size);

	if (ret != 0)
		return ret;

	*size = adapter->hw_info->bt_efuse_size - efuse_param->bt_efuse_end;

	return MACSUCCESS;
}

u32 mac_dump_log_efuse_plus(struct mac_ax_adapter *adapter,
			    enum mac_ax_efuse_parser_cfg parser_cfg,
			    enum mac_ax_efuse_read_cfg cfg,
			    u8 *efuse_map, bool is_limit)
{
	u32 ret = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 chip_id = hw_info->chip_id;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	u32 map_size = hw_info->log_efuse_size;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		ret = mac_dump_log_efuse(adapter, parser_cfg, cfg,
					 efuse_map, is_limit);
	} else {
		ret = mac_dump_log_efuse(adapter, parser_cfg, cfg,
					 efuse_map, is_limit);
		if (ret)
			return ret;

		if (dav_exist) {
			switch_dv(adapter, DAV);
			if (is_limit) {
				switch (intf) {
				case MAC_AX_INTF_PCIE:
					map_size = hw_info->limit_efuse_size_pcie;
					break;
				case MAC_AX_INTF_USB:
					map_size = hw_info->limit_efuse_size_usb;
					break;
				case MAC_AX_INTF_SDIO:
					map_size = hw_info->limit_efuse_size_sdio;
					break;
				default:
					break;
				}
			}
			if ((is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) ||
			    (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C)) ||
			    (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E)) ||
			    (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))) {
				ret = mac_dump_log_efuse(adapter, parser_cfg, cfg,
							 efuse_map + map_size, 0);
			} else {
				ret = mac_dump_log_efuse(adapter, parser_cfg,
							 MAC_AX_EFUSE_R_DRV,
							 efuse_map + map_size, 0);
			}
			switch_dv(adapter, DDV);
		}
	}

	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_dump_log_efuse(struct mac_ax_adapter *adapter,
		       enum mac_ax_efuse_parser_cfg parser_cfg,
		       enum mac_ax_efuse_read_cfg cfg,
		       u8 *efuse_map, bool is_limit)
{
	u8 *map = NULL;
	u32 ret, stat;
	u32 efuse_size;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

#if 0
	if (cfg == MAC_AX_EFUSE_R_FW ||
	    (cfg == MAC_AX_EFUSE_R_AUTO &&
	     adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY))
		return MACNOITEM;
#else
	if (cfg == MAC_AX_EFUSE_R_FW &&
	    adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;
#endif
	PLTFM_MSG_TRACE("[TRACE]cfg = %d\n", cfg);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	efuse_size = *bank_efuse_info.log_map_size;

	if (dv_sel == DAV) {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_DAV);
		if (ret != 0)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_DAV);
	} else {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
		if (ret != 0)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
	}
	if (ret)
		return ret;

	if (*bank_efuse_info.log_map_valid == 0) {
		ret = proc_dump_efuse(adapter, cfg);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]dump efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}

		if (*bank_efuse_info.phy_map_valid == 1) {
			map = (u8 *)PLTFM_MALLOC(efuse_size);
			if (!map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				stat = cnv_efuse_state(adapter,
						       MAC_AX_EFUSE_IDLE);
				if (stat != 0)
					return stat;
				return MACBUFALLOC;
			}

			ret = eeprom_parser(adapter, *bank_efuse_info.phy_map,
					    map, parser_cfg);
			if (ret != 0) {
				PLTFM_FREE(map, efuse_size);
				stat = cnv_efuse_state(adapter,
						       MAC_AX_EFUSE_IDLE);
				if (stat != 0)
					return stat;
				return ret;
			}
			PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
			PLTFM_MEMCPY(*bank_efuse_info.log_map, map,
				     efuse_size);
			*bank_efuse_info.log_map_valid = 1;
			PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
			PLTFM_FREE(map, efuse_size);
		}
	}

	query_status_map(adapter, MAC_AX_DUMP_LOGICAL_EFUSE,
			 efuse_map, is_limit);

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_read_log_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			    u8 *val)
{
	u32 ret = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 chip_id = hw_info->chip_id;
	u32 efuse_size = hw_info->log_efuse_size;
	u32 size1, size2;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		ret = mac_read_log_efuse(adapter, addr, size, val);
	} else {
		if (addr < efuse_size && addr + size <= efuse_size) {
			ret = mac_read_log_efuse(adapter, addr, size, val);
		} else if (addr >= efuse_size) {
			if (dav_exist) {
				switch_dv(adapter, DAV);
				ret = mac_read_log_efuse(adapter, addr - efuse_size,
							 size, val);
				switch_dv(adapter, DDV);
			}
		} else if (addr < efuse_size && addr + size > efuse_size) {
			size1 = efuse_size - addr;
			size2 = addr + size - efuse_size;

			ret = mac_read_log_efuse(adapter, addr, size1, val);
			if (ret)
				return ret;

			if (dav_exist) {
				switch_dv(adapter, DAV);
				ret = mac_read_log_efuse(adapter, 0, size2,
							 val + size1);
				switch_dv(adapter, DDV);
			}
		}
	}

	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_read_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size,
		       u8 *val)
{
	u8 *map = NULL;
	u32 ret = 0, stat;
	u32 efuse_size;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	efuse_size = *bank_efuse_info.log_map_size;

	if (addr >= efuse_size || addr + size > efuse_size) {
		PLTFM_MSG_ERR("[ERR] Wrong efuse index\n");
		return MACEFUSESIZE;
	}

	if (dv_sel == DAV) {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_DAV);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_DAV);
	} else {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
	}
	if (ret)
		return ret;

	map = (u8 *)PLTFM_MALLOC(efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return MACBUFALLOC;
	}

	ret = read_log_efuse_map(adapter, map, efuse_size);
	if (ret != 0) {
		PLTFM_FREE(map, efuse_size);
		PLTFM_MSG_ERR("[ERR]read logical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	PLTFM_MEMCPY(val, map + addr, size);
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	PLTFM_FREE(map, efuse_size);

	stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (stat != 0)
		return stat;

	return MACSUCCESS;
}

u32 mac_write_log_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	u32 ret = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 chip_id = hw_info->chip_id;
	u32 efuse_size = hw_info->log_efuse_size;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		ret = mac_write_log_efuse(adapter, addr, val);
	} else {
		if (addr < efuse_size) {
			ret = mac_write_log_efuse(adapter, addr, val);
		} else {
			if (dav_exist) {
				switch_dv(adapter, DAV);
				ret = mac_write_log_efuse(adapter,
							  addr - efuse_size, val);
				switch_dv(adapter, DDV);
			}
		}
	}

	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_write_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	u32 ret, stat;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	if (addr >= *bank_efuse_info.log_map_size) {
		PLTFM_MSG_ERR("[ERR]addr is too large\n");
		return MACEFUSESIZE;
	}

	if (dv_sel == DAV) {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_DAV);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_DAV);
	} else {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
	}
	if (ret)
		return ret;

	ret = proc_write_log_efuse(adapter, addr, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write logical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_dump_log_efuse_bt(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_parser_cfg parser_cfg,
			  enum mac_ax_efuse_read_cfg cfg,
			  u8 *efuse_map)
{
	u8 *map = NULL;
	u32 ret, stat;
	u32 efuse_size = adapter->hw_info->bt_log_efuse_size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

#if 0
	if (cfg == MAC_AX_EFUSE_R_FW ||
	    (cfg == MAC_AX_EFUSE_R_AUTO &&
	     adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY))
		return MACNOITEM;
#else
	if (cfg == MAC_AX_EFUSE_R_FW &&
	    adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;
#endif

	PLTFM_MSG_TRACE("[TRACE]cfg = %d\n", cfg);

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_BT);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_BT);
	if (ret != 0)
		return ret;
	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_BT);
	if (ret != 0)
		return ret;

	if (efuse_param->bt_log_efuse_map_valid == 0) {
		ret = proc_dump_efuse(adapter, cfg);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]dump efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}

		if (efuse_param->bt_efuse_map_valid == 1) {
			map = (u8 *)PLTFM_MALLOC(efuse_size);
			if (!map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				stat = cnv_efuse_state(adapter,
						       MAC_AX_EFUSE_IDLE);
				if (stat != 0)
					return stat;
				return MACBUFALLOC;
			}

			ret = eeprom_parser(adapter, efuse_param->bt_efuse_map,
					    map, parser_cfg);
			if (ret != 0) {
				PLTFM_FREE(map, efuse_size);
				stat = cnv_efuse_state(adapter,
						       MAC_AX_EFUSE_IDLE);
				if (stat != 0)
					return stat;
				return ret;
			}
			PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
			PLTFM_MEMCPY(efuse_param->bt_log_efuse_map, map,
				     efuse_size);
			efuse_param->bt_log_efuse_map_valid = 1;
			PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
			PLTFM_FREE(map, efuse_size);
		}
	}

	query_status_map(adapter, MAC_AX_DUMP_LOGICAL_EFUSE, efuse_map, 0);

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_read_log_efuse_bt(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			  u8 *val)
{
	u8 *map = NULL;
	u32 ret, stat;
	u32 efuse_size = adapter->hw_info->bt_log_efuse_size;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (addr >= efuse_size || addr + size > efuse_size) {
		PLTFM_MSG_ERR("[ERR] Wrong efuse index\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_BT);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_BT);
	if (ret)
		return ret;
	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_BT);
	if (ret)
		return ret;

	map = (u8 *)PLTFM_MALLOC(efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(map, 0xFF, efuse_size);

	ret = read_log_efuse_map(adapter, map, efuse_size);
	if (ret != 0) {
		PLTFM_FREE(map, efuse_size);
		PLTFM_MSG_ERR("[ERR]read logical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	PLTFM_MEMCPY(val, map + addr, size);
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	PLTFM_FREE(map, efuse_size);

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (stat != 0)
		return stat;

	return MACSUCCESS;
}

u32 mac_write_log_efuse_bt(struct mac_ax_adapter *adapter, u32 addr, u8 val)
{
	u32 ret, stat;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (addr >= adapter->hw_info->log_efuse_size) {
		PLTFM_MSG_ERR("[ERR]addr is too large\n");
		return MACEFUSESIZE;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_LOG_MAP);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_BT);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_BT);
	if (ret)
		return ret;
	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_BT);
	if (ret)
		return ret;

	ret = proc_write_log_efuse(adapter, addr, val);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write logical efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_pg_efuse_by_map_plus(struct mac_ax_adapter *adapter,
			     struct mac_ax_pg_efuse_info *info,
			     enum mac_ax_efuse_read_cfg cfg,
			     bool part, bool is_limit)
{
	u8 chip_id = adapter->hw_info->chip_id;
	u32 ret;
	struct mac_ax_pg_efuse_info info_DAV = {0};
	struct mac_ax_pg_efuse_info info_DDV = {0};
	u32 ver_len = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	u32 map_size = adapter->hw_info->log_efuse_size;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	ret = mac_get_hw_value(adapter,
			       MAC_AX_HW_GET_EFUSE_VERSION_SIZE, &ver_len);
	if (ret)
		return ret;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		ret = mac_pg_efuse_by_map(adapter, info, cfg, part, is_limit);
		if (ret)
			return ret;
	} else {
		if (is_limit) {
			switch (intf) {
			case MAC_AX_INTF_PCIE:
				map_size = hw_info->limit_efuse_size_pcie;
				break;
			case MAC_AX_INTF_USB:
				map_size = hw_info->limit_efuse_size_usb;
				break;
			case MAC_AX_INTF_SDIO:
				map_size = hw_info->limit_efuse_size_sdio;
				break;
			default:
				break;
			}
		}

		/* a-die */
		info_DAV.efuse_map_size = hw_info->dav_log_efuse_size;
		info_DAV.efuse_mask_size = hw_info->dav_log_efuse_size >> 4;

		info_DAV.efuse_map =
			(u8 *)PLTFM_MALLOC(info_DAV.efuse_map_size + ver_len);
		if (!info_DAV.efuse_map) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}
		info_DAV.efuse_mask =
			(u8 *)PLTFM_MALLOC(info_DAV.efuse_mask_size + ver_len);
		if (!info_DAV.efuse_mask) {
			free_map_mask(adapter, ver_len, &info_DAV, &info_DDV);
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}

		PLTFM_MEMCPY(info_DAV.efuse_map, info->efuse_map + map_size,
			     info_DAV.efuse_map_size + ver_len);
		PLTFM_MEMCPY(info_DAV.efuse_mask,
			     info->efuse_mask + (map_size >> 4),
			     info_DAV.efuse_mask_size + ver_len);

		/* d-die */
		info_DDV.efuse_map_size =
			info->efuse_map_size - info_DAV.efuse_map_size;
		info_DDV.efuse_mask_size =
			info->efuse_mask_size - info_DAV.efuse_mask_size;

		info_DDV.efuse_map =
			(u8 *)PLTFM_MALLOC(info_DDV.efuse_map_size + ver_len);
		if (!info_DDV.efuse_map) {
			free_map_mask(adapter, ver_len, &info_DAV, &info_DDV);
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}
		info_DDV.efuse_mask =
			(u8 *)PLTFM_MALLOC(info_DDV.efuse_mask_size + ver_len);
		if (!info_DDV.efuse_mask) {
			free_map_mask(adapter, ver_len, &info_DAV, &info_DDV);
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}

		PLTFM_MEMCPY(info_DDV.efuse_map, info->efuse_map,
			     info_DDV.efuse_map_size);
		PLTFM_MEMCPY(info_DDV.efuse_map + info_DDV.efuse_map_size,
			     info->efuse_map + info->efuse_map_size, ver_len);

		PLTFM_MEMCPY(info_DDV.efuse_mask, info->efuse_mask,
			     info_DDV.efuse_mask_size);
		PLTFM_MEMCPY(info_DDV.efuse_mask + info_DDV.efuse_mask_size,
			     info->efuse_mask + info->efuse_mask_size, ver_len);

		ret = mac_pg_efuse_by_map(adapter,
					  &info_DDV, cfg, part, is_limit);
		if (ret) {
			free_map_mask(adapter, ver_len, &info_DAV, &info_DDV);
			return ret;
		}

		if (dav_exist) {
			switch_dv(adapter, DAV);
			ret = mac_pg_efuse_by_map(adapter,
						  &info_DAV, cfg, part, 0);
			if (ret) {
				free_map_mask(adapter, ver_len, &info_DAV, &info_DDV);
				return ret;
			}
			switch_dv(adapter, DDV);
		}
	}

	free_map_mask(adapter, ver_len, &info_DAV, &info_DDV);
	return MACSUCCESS;
}

static void free_map_mask(struct mac_ax_adapter *adapter, u32 ver_len,
			  struct mac_ax_pg_efuse_info *info_DAV,
			  struct mac_ax_pg_efuse_info *info_DDV)
{
	if (info_DAV->efuse_map)
		PLTFM_FREE(info_DAV->efuse_map,
			   info_DAV->efuse_map_size + ver_len);
	if (info_DAV->efuse_mask)
		PLTFM_FREE(info_DAV->efuse_mask,
			   info_DAV->efuse_mask_size + ver_len);
	if (info_DDV->efuse_map)
		PLTFM_FREE(info_DDV->efuse_map,
			   info_DDV->efuse_map_size + ver_len);
	if (info_DDV->efuse_mask)
		PLTFM_FREE(info_DDV->efuse_mask,
			   info_DDV->efuse_mask_size + ver_len);
}

u32 mac_pg_efuse_by_map(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info,
			enum mac_ax_efuse_read_cfg cfg,
			bool part, bool is_limit)
{
	u32 ret, stat;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	u32 map_size = adapter->hw_info->log_efuse_size;
	u32 ver_len = 0;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (is_limit) {
		switch (intf) {
		case MAC_AX_INTF_PCIE:
			map_size = adapter->hw_info->limit_efuse_size_pcie;
			break;
		case MAC_AX_INTF_USB:
			map_size = adapter->hw_info->limit_efuse_size_usb;
			break;
		case MAC_AX_INTF_SDIO:
			map_size = adapter->hw_info->limit_efuse_size_sdio;
			break;
		default:
			break;
		}
	}

	if (dv_sel == DAV)
		map_size = adapter->hw_info->dav_log_efuse_size;

	if (info->efuse_map_size != map_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_ERR("[ERR]not multiple of 16\n");
		return MACEFUSESIZE;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_ERR("[ERR]mask size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	if (!info->efuse_mask) {
		PLTFM_MSG_ERR("[ERR]mask is NULL\n");
		return MACNPTR;
	}

	ret = mac_get_hw_value(adapter,
			       MAC_AX_HW_GET_EFUSE_VERSION_SIZE, &ver_len);
	if (ret)
		return ret;
	ret = compare_version(adapter, info, ver_len);
	if (ret != 0)
		return ret;

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	if (dv_sel == DAV) {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_DAV);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_DAV);
	} else {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
	}
	if (ret)
		return ret;

	if (part)
		ret = adjust_mask(adapter, info);
	if (ret != 0) {
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = proc_pg_efuse_by_map(adapter, info, cfg);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]pg efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_pg_efuse_by_map_bt(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info,
			   enum mac_ax_efuse_read_cfg cfg)
{
	u32 ret, stat;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (info->efuse_map_size != adapter->hw_info->bt_log_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_ERR("[ERR]not multiple of 16\n");
		return MACEFUSESIZE;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_ERR("[ERR]mask size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	if (!info->efuse_mask) {
		PLTFM_MSG_ERR("[ERR]mask is NULL\n");
		return MACNPTR;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_BT);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = proc_pg_efuse_by_map(adapter, info, cfg);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]pg efuse\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_mask_log_efuse(struct mac_ax_adapter *adapter,
		       struct mac_ax_pg_efuse_info *info)
{
	if (info->efuse_map_size != adapter->hw_info->log_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if ((info->efuse_map_size & 0xF) > 0) {
		PLTFM_MSG_ERR("[ERR]not multiple of 16\n");
		return MACEFUSESIZE;
	}

	if (info->efuse_mask_size != info->efuse_map_size >> 4) {
		PLTFM_MSG_ERR("[ERR]mask size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	if (!info->efuse_mask) {
		PLTFM_MSG_ERR("[ERR]mask is NULL\n");
		return MACNPTR;
	}

	mask_eeprom(adapter, info);

	return MACSUCCESS;
}

u32 mac_pg_sec_data_by_map(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info)
{
	u32 ret, stat;
	u32 addr, efuse_size, sec_data_size;
	u8 *map_pg;
	u32 mac_addr_size = 6;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	/*Soar TBD add MAC address PG*/

	map_pg = info->efuse_map;
	efuse_size = adapter->hw_info->efuse_size;
	sec_data_size = adapter->hw_info->sec_data_efuse_size;

	if (info->efuse_map_size != adapter->hw_info->sec_data_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	p_ops->enable_efuse_sw_pwr_cut(adapter, 1);
	for (addr = mac_addr_size; addr < sec_data_size; addr++) {
		ret = write_hw_efuse(adapter, addr + efuse_size,
				     *(map_pg + addr));
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]write physical efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
			return ret;
		}
	}
	p_ops->disable_efuse_sw_pwr_cut(adapter, 1);

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_cmp_sec_data_by_map(struct mac_ax_adapter *adapter,
			    struct mac_ax_pg_efuse_info *info)
{
	u32 ret, stat;
	u32 addr, start_addr, sec_data_size;
	u16 val16;
	u8 *map_pg;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	map_pg = info->efuse_map;
	start_addr = adapter->hw_info->efuse_size;
	sec_data_size = adapter->hw_info->sec_data_efuse_size;

	if (info->efuse_map_size != adapter->hw_info->sec_data_efuse_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	if (!info->efuse_map) {
		PLTFM_MSG_ERR("[ERR]map is NULL\n");
		return MACNPTR;
	}

	ret = efuse_proc_ck(adapter);
	if (ret != 0)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != 0)
			return stat;
		return ret;
	}

	for (addr = 0;
	     addr < sec_data_size; addr += 2) {
		val16 = *(map_pg + addr) | (*(map_pg + addr + 1) << 8);
		ret = cmp_hw_efuse(adapter, addr + start_addr, val16);
		if (ret == MACEFUSECMP) {
			*(map_pg + addr) = 0xFF;
			*(map_pg + addr + 1) = 0xFF;
		} else if (ret == MACEFUSEREAD) {
			PLTFM_MSG_ERR("[ERR]compare hw efuse\n");
			stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
			if (stat != 0)
				return stat;
		} else if (ret == 0) {
			*(map_pg + addr) = 0x00;
			*(map_pg + addr + 1) = 0x00;
		}
	}

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != 0)
		return ret;

	return MACSUCCESS;
}

u32 mac_get_efuse_info(struct mac_ax_adapter *adapter, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length,
		       u8 *autoload_status)
{
	u32 offset, def_val;
	u32 ret;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	struct efuse_info info;

	switch (intf) {
	case MAC_AX_INTF_USB:
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			info = efuse_info_usb_8852a;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			info = efuse_info_usb_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			info = efuse_info_usb_8851b;
		else
			info = efuse_info_usb_8852b;
		break;
	case MAC_AX_INTF_PCIE:
		info = efuse_info_pcie;
		break;
	case MAC_AX_INTF_SDIO:
		info = efuse_info_sdio;
		break;
	default:
		return MACINTF;
	}

	ret = compare_info_length(&info, id, length);
	if (ret != MACSUCCESS)
		return ret;

	switch (id) {
	case EFUSE_INFO_MAC_ADDR:
		offset = info.offset->mac_addr;
		def_val = info.def_val->mac_addr;
		break;
	case EFUSE_INFO_MAC_PID:
		offset = info.offset->pid;
		def_val = info.def_val->pid;
		break;
	case EFUSE_INFO_MAC_DID:
		offset = info.offset->did;
		def_val = info.def_val->did;
		break;
	case EFUSE_INFO_MAC_VID:
		offset = info.offset->vid;
		def_val = info.def_val->vid;
		break;
	case EFUSE_INFO_MAC_SVID:
		offset = info.offset->svid;
		def_val = info.def_val->svid;
		break;
	case EFUSE_INFO_MAC_SMID:
		offset = info.offset->smid;
		def_val = info.def_val->smid;
		break;
	default:
		return MACNOITEM;
	}

	if (*autoload_status == 0)
		PLTFM_MEMCPY(value, &def_val, 1);
	else
		PLTFM_MEMCPY(value, efuse_map + offset, length);

	return MACSUCCESS;
}

u32 mac_set_efuse_info(struct mac_ax_adapter *adapter, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length)
{
	u32 offset;
	u32 ret;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	struct efuse_info info;

	switch (intf) {
	case MAC_AX_INTF_USB:
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			info = efuse_info_usb_8852a;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			info = efuse_info_usb_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			info = efuse_info_usb_8851b;
		else
			info = efuse_info_usb_8852b;
		break;
	case MAC_AX_INTF_PCIE:
		info = efuse_info_pcie;
		break;
	case MAC_AX_INTF_SDIO:
		info = efuse_info_sdio;
		break;
	default:
		return MACINTF;
	}

	ret = compare_info_length(&info, id, length);
	if (ret != MACSUCCESS)
		return ret;

	switch (id) {
	case EFUSE_INFO_MAC_ADDR:
		offset = info.offset->mac_addr;
		break;
	case EFUSE_INFO_MAC_PID:
		offset = info.offset->pid;
		break;
	case EFUSE_INFO_MAC_DID:
		offset = info.offset->did;
		break;
	case EFUSE_INFO_MAC_VID:
		offset = info.offset->vid;
		break;
	case EFUSE_INFO_MAC_SVID:
		offset = info.offset->svid;
		break;
	case EFUSE_INFO_MAC_SMID:
		offset = info.offset->smid;
		break;
	default:
		return MACNOITEM;
	}

	PLTFM_MEMCPY(efuse_map + offset, value, length);

	return MACSUCCESS;
}

u32 mac_read_hidden_rpt(struct mac_ax_adapter *adapter,
			struct mac_defeature_value *rpt)
{
#if 0
	return MACNOITEM;
#else
	u32 ret, stat;
	struct mac_ax_h2creg_info h2c;
	struct mac_ax_c2hreg_poll c2h;
	struct fwcmd_c2hreg *c2h_content;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	ret = efuse_proc_ck(adapter);
	if (ret != MACSUCCESS)
		return ret;

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_PHY);
	if (ret != MACSUCCESS)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat != MACSUCCESS)
			return stat;
		return ret;
	}

	ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
	if (ret != MACSUCCESS)
		return ret;

	h2c.id = FWCMD_H2CREG_FUNC_HIDDEN_GET;
	h2c.content_len = sizeof(struct mac_efuse_hidden_h2creg);

	c2h.polling_id = FWCMD_C2HREG_FUNC_EFUSE_HIDDEN;
	c2h.retry_cnt = EFUSE_C2HREG_WAIT_CNT;
	c2h.retry_wait_us = EFUSE_C2HREG_RETRY_WAIT_US;

	ret = proc_msg_reg(adapter, &h2c, &c2h);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]read hidden rpt proc msg reg %d\n", ret);
		stat = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
		if (stat)
			return stat;
		return ret;
	}

	c2h_content = &c2h.c2hreg_cont.c2h_content;

	rpt->rx_spatial_stream = GET_FIELD(c2h_content->dword0, FWCMD_C2HREG_EFUSE_HIDDEN_RX_NSS);
	rpt->bandwidth = GET_FIELD(c2h_content->dword0, FWCMD_C2HREG_EFUSE_HIDDEN_BW);
	rpt->tx_spatial_stream = GET_FIELD(c2h_content->dword1, FWCMD_C2HREG_EFUSE_HIDDEN_TX_NSS);
	rpt->protocol_80211 = GET_FIELD(c2h_content->dword1, FWCMD_C2HREG_EFUSE_HIDDEN_PROT80211);
	rpt->NIC_router = GET_FIELD(c2h_content->dword1, FWCMD_C2HREG_EFUSE_HIDDEN_NIC_ROUTER);
	rpt->wl_func_support =
	GET_FIELD(c2h_content->dword1, FWCMD_C2HREG_EFUSE_HIDDEN_WL_FUNC_SUPPORT);
	rpt->hw_special_type =
	GET_FIELD(c2h_content->dword2, FWCMD_C2HREG_EFUSE_HIDDEN_HW_SPECIAL_TYPE);
	rpt->uuid =
	GET_FIELD(c2h_content->dword3, FWCMD_C2HREG_EFUSE_HIDDEN_UUID_BYTE_3) << 24 |
	GET_FIELD(c2h_content->dword2, FWCMD_C2HREG_EFUSE_HIDDEN_UUID_BYTE_2) << 16 |
	GET_FIELD(c2h_content->dword2, FWCMD_C2HREG_EFUSE_HIDDEN_UUID_BYTE_1) << 8 |
	GET_FIELD(c2h_content->dword2, FWCMD_C2HREG_EFUSE_HIDDEN_UUID_BYTE_0);
	rpt->tx_path_num = GET_FIELD(c2h_content->dword3, FWCMD_C2HREG_EFUSE_HIDDEN_TX_PATH_NUM);
	rpt->rx_path_num = GET_FIELD(c2h_content->dword3, FWCMD_C2HREG_EFUSE_HIDDEN_RX_PATH_NUM);

	ret = cnv_efuse_state(adapter, MAC_AX_EFUSE_IDLE);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
#endif
}

u32 mac_check_efuse_autoload(struct mac_ax_adapter *adapter,
			     u8 *autoload_status)
{
#define AUTOLOAD_SUS 1
#define AUTOLOAD_FAIL 0

	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (MAC_REG_R16(R_AX_SYS_WL_EFUSE_CTRL) & B_AX_AUTOLOAD_SUS)
		*autoload_status = AUTOLOAD_SUS;
	else
		*autoload_status = AUTOLOAD_FAIL;

	return MACSUCCESS;

#undef AUTOLOAD_SUS
#undef AUTOLOAD_FAIL
}

u32 mac_pg_simulator_plus(struct mac_ax_adapter *adapter,
			  struct mac_ax_pg_efuse_info *info, u8 *phy_map)
{
	u8 chip_id = adapter->hw_info->chip_id;
	u32 ret;
	struct mac_ax_pg_efuse_info info_dav;
	u32 ver_len = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 map_size = hw_info->log_efuse_size;
	bool dav_exist = (hw_info->dav_full_efuse_size == 0) ? false : true;

	ret = mac_get_hw_value(adapter,
			       MAC_AX_HW_GET_EFUSE_VERSION_SIZE, &ver_len);
	if (ret)
		return ret;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		ret = mac_pg_simulator(adapter, info, phy_map);
	} else {
		info_dav.efuse_map_size = hw_info->dav_log_efuse_size;
		info_dav.efuse_mask_size = hw_info->dav_log_efuse_size >> 4;

		info_dav.efuse_map =
			(u8 *)PLTFM_MALLOC(info_dav.efuse_map_size + ver_len);
		if (!info_dav.efuse_map) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}
		info_dav.efuse_mask =
			(u8 *)PLTFM_MALLOC(info_dav.efuse_mask_size +
					   ver_len);
		if (!info_dav.efuse_mask) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}

		PLTFM_MEMCPY(info_dav.efuse_map, info->efuse_map + map_size,
			     info_dav.efuse_map_size + ver_len);
		PLTFM_MEMCPY(info_dav.efuse_mask,
			     info->efuse_mask + (map_size >> 4),
			     info_dav.efuse_mask_size + ver_len);

		info->efuse_map_size -= info_dav.efuse_map_size;
		info->efuse_mask_size -= info_dav.efuse_mask_size;

		PLTFM_MEMCPY(info->efuse_map + map_size, info->efuse_map +
			     map_size + info_dav.efuse_map_size, ver_len);
		PLTFM_MEMCPY(info->efuse_mask + (map_size >> 4),
			     info->efuse_mask + (map_size >> 4) +
			     info_dav.efuse_mask_size, ver_len);

		ret = mac_pg_simulator(adapter, info, phy_map);
		if (ret)
			return ret;

		if (dav_exist) {
			switch_dv(adapter, DAV);
			ret = mac_pg_simulator(adapter, &info_dav,
					       phy_map + hw_info->wl_efuse_size);
			switch_dv(adapter, DDV);
			PLTFM_MEMCPY(info->efuse_map + map_size, info_dav.efuse_map,
				     info_dav.efuse_map_size + ver_len);
		}
	}

	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 mac_pg_simulator(struct mac_ax_adapter *adapter,
		     struct mac_ax_pg_efuse_info *info, u8 *phy_map)
{
	u8 *updated_mask;
	u32 ret;
	u32 mask_size;
	u8 *log_efuse_map = NULL;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	u32 map_size = adapter->hw_info->log_efuse_size;
	u32 ver_len = 0;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	if (info->efuse_map_size != adapter->hw_info->log_efuse_size) {
		switch (intf) {
		case MAC_AX_INTF_PCIE:
			map_size = adapter->hw_info->limit_efuse_size_pcie;
			break;
		case MAC_AX_INTF_USB:
			map_size = adapter->hw_info->limit_efuse_size_usb;
			break;
		case MAC_AX_INTF_SDIO:
			map_size = adapter->hw_info->limit_efuse_size_sdio;
			break;
		default:
			break;
		}
	}

	if (dv_sel == DAV)
		map_size = adapter->hw_info->dav_log_efuse_size;

	if (info->efuse_map_size != map_size) {
		PLTFM_MSG_ERR("[ERR]map size error\n");
		return MACEFUSESIZE;
	}

	mask_size = map_size >> 4;

	ret = mac_get_hw_value(adapter,
			       MAC_AX_HW_GET_EFUSE_VERSION_SIZE, &ver_len);
	if (ret)
		return ret;

	ret = compare_version(adapter, info, ver_len);
	if (ret != 0)
		return ret;

	ret = p_ops->switch_efuse_bank(adapter, MAC_AX_EFUSE_BANK_WIFI);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]switch efuse bank!!\n");
		return ret;
	}

	if (dv_sel == DAV) {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_DAV);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG_DAV);
	} else {
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_PHY_WL);
		if (ret)
			return ret;
		ret = efuse_map_init(adapter, EFUSE_MAP_SEL_LOG);
	}
	if (ret)
		return ret;

	ret = eeprom_parser(adapter, phy_map, *bank_efuse_info.log_map,
			    MAC_AX_EFUSE_PARSER_MAP);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]logical map parser\n");
		return ret;
	}
	PLTFM_MEMCPY(*bank_efuse_info.phy_map, phy_map,
		     *bank_efuse_info.phy_map_size);

	updated_mask = (u8 *)PLTFM_MALLOC(mask_size);
	if (!updated_mask) {
		PLTFM_MSG_ERR("[ERR]malloc updated mask\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(updated_mask, 0x00, mask_size);

	ret = update_eeprom_mask(adapter, info, updated_mask, 1);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]update eeprom mask\n");
		goto error;
	}

	ret = check_efuse_enough(adapter, info, updated_mask);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]chk efuse enough\n");
		goto error;
	}

	ret = program_efuse(adapter, info, updated_mask, 1);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]pg efuse\n");
		goto error;
	}

	PLTFM_FREE(updated_mask, mask_size);

	PLTFM_MEMCPY(phy_map, *bank_efuse_info.phy_map,
		     *bank_efuse_info.phy_map_size);
	ret = eeprom_parser(adapter, phy_map, info->efuse_map,
			    MAC_AX_EFUSE_PARSER_MAP);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]parser error\n");
		PLTFM_FREE(log_efuse_map, map_size);
		return ret;
	}

	/* For subsequent dump */
	*bank_efuse_info.log_map_valid = 0;
	*bank_efuse_info.phy_map_valid = 0;

	return MACSUCCESS;

error:
	PLTFM_FREE(updated_mask, mask_size);
	return ret;
}

u32 mac_checksum_update(struct mac_ax_adapter *adapter)
{
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 log_efuse_size = hw_info->log_efuse_size;
	u8 *map;
	u16 chksum;
	u32 ret;

	map = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(map, 0xFF, log_efuse_size);

	ret = mac_dump_log_efuse(adapter, MAC_AX_EFUSE_PARSER_MAP,
				 MAC_AX_EFUSE_R_AUTO, map, 0);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]Dump log map\n");
		goto error;
	}

	cal_check_sum(adapter, &chksum);

	ret = set_check_sum_val(adapter, map, chksum);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]write check sum\n");
		goto error;
	}

	PLTFM_FREE(map, log_efuse_size);
	return MACSUCCESS;
error:
	PLTFM_FREE(map, log_efuse_size);
	return ret;
}

u32 mac_checksum_rpt(struct mac_ax_adapter *adapter, u16 *chksum)
{
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 log_efuse_size = hw_info->log_efuse_size;
	u8 *tmp;
	u32 ret;
	u16 real_chksum = 0;

	/* Read chksum val */
	tmp = (u8 *)PLTFM_MALLOC(sizeof(real_chksum));
	if (!tmp) {
		PLTFM_MSG_ERR("[ERR]malloc tmp\n");
		return MACBUFALLOC;
	}
	ret = mac_read_log_efuse(adapter, chksum_offset_1, sizeof(real_chksum),
				 tmp);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]read log efuse\n");
		PLTFM_FREE(tmp, sizeof(real_chksum));
		return ret;
	}

	real_chksum = *(u16 *)tmp;
	PLTFM_FREE(tmp, sizeof(real_chksum));

	if (real_chksum == 0xFFFF)
		return MACCHKSUMEMPTY;

	/* Read log map*/
	tmp = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!tmp) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(tmp, 0xFF, log_efuse_size);

	ret = mac_read_log_efuse(adapter, 0, log_efuse_size, tmp);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]read log efuse\n");
		PLTFM_FREE(tmp, log_efuse_size);
		return ret;
	}
	PLTFM_FREE(tmp, log_efuse_size);

	cal_check_sum(adapter, chksum);
	if (*chksum != real_chksum)
		return MACCHKSUMFAIL;

	return MACSUCCESS;
}

u32 mac_disable_rf_ofld_by_info(struct mac_ax_adapter *adapter,
				struct mac_disable_rf_ofld_info info)
{
	u32 ret;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	u8 *buf;
	struct fwcmd_disable_rf *write_ptr;

	/* H2C */
	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (adapter->sm.disable_rf != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	adapter->sm.disable_rf = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_pkt_ofld_hdr));
	if (!buf) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	write_ptr = (struct fwcmd_disable_rf *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(info.func, FWCMD_H2C_DISABLE_RF_FUNC) |
		    SET_WORD(info.type, FWCMD_H2C_DISABLE_RF_NET_TYPE)
	);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_DISABLE_RF,
			      0, 0);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		h2cb_free(adapter, h2cb);
		return ret;
	}

	h2cb_free(adapter, h2cb);

	if (adapter->sm.disable_rf != MAC_AX_OFLD_H2C_SENDING)
		return MACPROCERR;

	adapter->sm.disable_rf = MAC_AX_OFLD_H2C_IDLE;

	return MACSUCCESS;
}

u32 _patch_otp_power_issue(struct mac_ax_adapter *adapter)
{
#define secure 1
	u8 val8;
	u32 ret, stat;

	ret = mac_set_efuse_ctrl(adapter, secure);
	if (ret != MACSUCCESS)
		return ret;
	read_efuse_cnt = CHK_OTP_WAIT_CNT;

	ret = mac_read_efuse_plus(adapter, CHK_OTP_ADDR, 1, &val8,
				  MAC_AX_EFUSE_BANK_WIFI);
	if (ret != MACSUCCESS) {
		read_efuse_cnt = EFUSE_WAIT_CNT;
		stat = mac_set_efuse_ctrl(adapter, !secure);
		if (stat != MACSUCCESS)
			return stat;
		return ret;
	}

	read_efuse_cnt = EFUSE_WAIT_CNT;
	ret = mac_set_efuse_ctrl(adapter, !secure);
	if (ret != MACSUCCESS)
		return ret;

	return MACSUCCESS;
}

u32 mac_set_efuse_ctrl(struct mac_ax_adapter *adapter, bool is_secure)
{
	if (is_secure)
		efuse_ctrl = R_AX_EFUSE_CTRL_S;
	else
		efuse_ctrl = R_AX_EFUSE_CTRL;

	return MACSUCCESS;
}

u32 mac_otp_test(struct mac_ax_adapter *adapter, bool is_OTP_test)
{
	if (is_OTP_test)
		OTP_test = 1;
	else
		OTP_test = 0;

	return MACSUCCESS;
}

void cfg_efuse_auto_ck(struct mac_ax_adapter *adapter, u8 enable)
{
	/*Soar TBD* move to set hw value */

#ifdef NEVER
	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);
	adapter->efuse_param.auto_ck_en = enable;
	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);
#endif /* NEVER */
}

u32 efuse_tbl_init(struct mac_ax_adapter *adapter)
{
	PLTFM_MUTEX_INIT(&efuse_tbl.lock);

	return MACSUCCESS;
}

u32 efuse_tbl_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_MUTEX_DEINIT(&efuse_tbl.lock);

	return MACSUCCESS;
}

static u32 efuse_map_init(struct mac_ax_adapter *adapter,
			  enum efuse_map_sel map_sel)
{
	u32 size;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;

	switch (map_sel) {
	case EFUSE_MAP_SEL_PHY_WL:
		size = adapter->hw_info->efuse_size;
		if (!efuse_param->efuse_map) {
			efuse_param->efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	case EFUSE_MAP_SEL_PHY_BT:
		size = adapter->hw_info->bt_efuse_size;
		if (!efuse_param->bt_efuse_map) {
			efuse_param->bt_efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->bt_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	case EFUSE_MAP_SEL_LOG:
		size = adapter->hw_info->log_efuse_size;
		if (!efuse_param->log_efuse_map) {
			efuse_param->log_efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->log_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	case EFUSE_MAP_SEL_LOG_BT:
		size = adapter->hw_info->bt_log_efuse_size;
		if (!efuse_param->bt_log_efuse_map) {
			efuse_param->bt_log_efuse_map =
				(u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->bt_log_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	case EFUSE_MAP_SEL_PHY_DAV:
		size = adapter->hw_info->dav_efuse_size;
		if (!efuse_param->dav_efuse_map) {
			efuse_param->dav_efuse_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->dav_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	case EFUSE_MAP_SEL_LOG_DAV:
		size = adapter->hw_info->dav_log_efuse_size;
		if (!efuse_param->dav_log_efuse_map) {
			efuse_param->dav_log_efuse_map =
				(u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->dav_log_efuse_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	case EFUSE_MAP_SEL_HIDDEN_RF:
		size = adapter->hw_info->hidden_efuse_rf_size;
		if (!efuse_param->hidden_rf_map) {
			efuse_param->hidden_rf_map = (u8 *)PLTFM_MALLOC(size);
			if (!efuse_param->hidden_rf_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}
		}
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

static u32 efuse_fwcmd_ck(struct mac_ax_adapter *adapter)
{
/*Soar TBD*/
#ifdef NEVER
	u32 ret;

	ret = fwcmd_wq_idle(adapter,
			    SET_FWCMD_ID(FWCMD_TYPE_H2C,
					 FWCMD_H2C_CAT_MAC,
					 TBD,
					 TBD));
	if (ret != 0) {
		PLTFM_MSG_WARN("[WARN]H2C not idle(efuse)\n");
		return ret;
	}
#endif /* NEVER */
	return MACSUCCESS;
}

static u32 efuse_proc_ck(struct mac_ax_adapter *adapter)
{
/*Soar TBD*/
#ifdef NEVER
	u32 ret;

	ret = efuse_fwcmd_ck(adapter);
	if (ret != 0)
		return ret;
#endif /* NEVER */
	if (adapter->sm.efuse != MAC_AX_EFUSE_IDLE) {
		PLTFM_MSG_WARN("[WARN]Proc not idle(efuse)\n");
		return MACPROCBUSY;
	}

	if (adapter->sm.pwr != MAC_AX_PWR_ON)
		PLTFM_MSG_ERR("[ERR]Access efuse in suspend\n");

	return MACSUCCESS;
}

static u32 cnv_efuse_state(struct mac_ax_adapter *adapter, u8 dest_state)
{
#define BT_DISN_EN 1
#define BT_DISN_DIS 0
	u8 cnt;
	u32 val32, ret, bt_dis_state;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (adapter->sm.efuse >= MAC_AX_EFUSE_MAX)
		return MACPROCERR;

	if (adapter->sm.efuse == dest_state)
		return MACPROCERR;

	if (dest_state != MAC_AX_EFUSE_IDLE) {
		if (adapter->sm.efuse != MAC_AX_EFUSE_IDLE)
			return MACPROCERR;
	}

	adapter->sm.efuse = dest_state;

	if (adapter->sm.efuse != MAC_AX_EFUSE_IDLE) {
		bt_dis_state = mac_get_bt_dis(adapter);
		if (bt_dis_state == BT_DISN_EN) {
			ret = mac_set_bt_dis(adapter, BT_DISN_DIS);
			if (ret != MACSUCCESS)
				return ret;
		}

		cnt = BT_DIS_WAIT_CNT;
		while (cnt) {
			val32 = MAC_REG_R32(R_AX_IC_PWR_STATE);
			val32 = GET_FIELD(val32, B_AX_WHOLE_SYS_PWR_STE);
			if (val32 == MAC_AX_SYS_ACT)
				break;

			PLTFM_DELAY_US(BT_DIS_WAIT_US);
			cnt--;
		}

		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]Whole system PWR state not ACT\n");
			return MACSYSSTEERR;
		}
	} else {
		bt_dis_state = mac_get_bt_dis(adapter);
		if (bt_dis_state == BT_DISN_DIS) {
			ret = mac_set_bt_dis(adapter, BT_DISN_EN);
			if (ret != MACSUCCESS)
				return ret;
		}
	}

	return MACSUCCESS;
}

static u32 proc_dump_efuse(struct mac_ax_adapter *adapter,
			   enum mac_ax_efuse_read_cfg cfg)
{
	u32 ret;

	if (cfg == MAC_AX_EFUSE_R_AUTO) {
		if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
			ret = dump_efuse_drv(adapter);
		else
			ret = dump_efuse_fw(adapter);
	} else if (cfg == MAC_AX_EFUSE_R_FW) {
		ret = dump_efuse_fw(adapter);
	} else {
		ret = dump_efuse_drv(adapter);
	}

	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]dump efsue drv/fw\n");
		return ret;
	}

	return MACSUCCESS;
}

static u32 read_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
			 u8 *map)
{
	u32 addr;
	u32 tmp32 = 0;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 efuse_start = *bank_efuse_info.efuse_start;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	offset += efuse_start;

	if (dv_sel == DDV) {
		p_ops->enable_efuse_sw_pwr_cut(adapter, 0);

		for (addr = offset; addr < offset + size; addr++) {
			MAC_REG_W32(efuse_ctrl,
				    ((addr & B_AX_EF_ADDR_MSK) << B_AX_EF_ADDR_SH)
				    & ~B_AX_EF_RDY);

			cnt = read_efuse_cnt;
			if ((is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
			     is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			     is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
			     is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			     is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) &&
			    efuse_ctrl == R_AX_EFUSE_CTRL)
				cnt = EFUSE_WAIT_CNT_PLUS;

			while (--cnt) {
				tmp32 = MAC_REG_R32(efuse_ctrl);
				if (tmp32 & B_AX_EF_RDY)
					break;
				PLTFM_DELAY_US(1);
			}

			if (!cnt) {
				PLTFM_MSG_ERR("[ERR]read efuse\n");
				return MACEFUSEREAD;
			}

			*(map + addr - offset) = (u8)(tmp32 & 0xFF);
		}

		p_ops->disable_efuse_sw_pwr_cut(adapter, 0);
	} else {
		read_hw_efuse_dav(adapter, offset, size, map);
	}

	return MACSUCCESS;
}

static u32 write_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u8 value)
{
	//const u8 unlock_code = 0x69;
	u8 value_read = 0;
	u32 value32;
	u32 cnt;
	u32 ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;
	u32 efuse_start = *bank_efuse_info.efuse_start;

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	*bank_efuse_info.phy_map_valid = 0;
	*bank_efuse_info.log_map_valid = 0;
	efuse_param->hidden_rf_map_valid = 0;
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);

	offset += efuse_start;

	if (dv_sel == DDV) {
		//MAC_REG_W8(R_AX_PMC_DBG_CTRL2 + 3, unlock_code);

		value32 = value |
			  ((offset & B_AX_EF_ADDR_MSK) << B_AX_EF_ADDR_SH);
		value32 &= ~B_AX_EF_RDY;
		MAC_REG_W32(efuse_ctrl,
			    value32 | (MODE_WRITE << B_AX_EF_MODE_SEL_SH));

		cnt = EFUSE_WAIT_CNT;
		while (--cnt) {
			if (MAC_REG_R32(efuse_ctrl) & B_AX_EF_RDY)
				break;
			PLTFM_DELAY_US(1);
		}

		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]write efuse\n");
			return MACEFUSEWRITE;
		}

		//MAC_REG_W8(R_AX_PMC_DBG_CTRL2 + 3, 0x00);
	} else {
		write_hw_efuse_dav(adapter, offset, value);
	}

	if (efuse_param->auto_ck_en == 1) {
		ret = read_hw_efuse(adapter, offset, 1, &value_read);
		if (ret != 0)
			return ret;

		if (value_read != value) {
			PLTFM_MSG_ERR("[ERR]efuse compare\n");
			return MACEFUSEWRITE;
		}
	}

	return MACSUCCESS;
}

static u32 cmp_hw_efuse(struct mac_ax_adapter *adapter, u32 offset, u16 val)
{
	u32 val32;
	u32 tmp32 = 0;
	u32 cnt;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val32 = val | ((offset & B_AX_EF_ADDR_MSK) << B_AX_EF_ADDR_SH);
	MAC_REG_W32(efuse_ctrl, val32 | (MODE_CMP << B_AX_EF_MODE_SEL_SH));

	cnt = EFUSE_WAIT_CNT;
	while (--cnt) {
		tmp32 = MAC_REG_R32(efuse_ctrl);
		if (tmp32 & B_AX_EF_RDY)
			break;
		PLTFM_DELAY_US(1);
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]compare efuse\n");
		return MACEFUSEREAD;
	}

	if (0 == (tmp32 & B_AX_EF_COMP_RESULT))
		return MACEFUSECMP;

	return MACSUCCESS;
}

static u32 eeprom_parser(struct mac_ax_adapter *adapter, u8 *phy_map,
			 u8 *log_map, enum mac_ax_efuse_parser_cfg cfg)
{
	u8 i;
	u8 value8;
	u8 blk_idx;
	u8 word_en;
	u8 valid;
	u8 hdr = 0;
	u8 hdr2 = 0;
	u32 eeprom_idx;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u32 sec_ctrl_size = hw_info->sec_ctrl_efuse_size;
	u32 efuse_idx;
	u32 efuse_size = *bank_efuse_info.phy_map_size;
	bool is_bt = 0;
	u32 log_efuse_size = *bank_efuse_info.log_map_size;

	PLTFM_MEMSET(log_map, 0xFF, log_efuse_size);

	if (log_efuse_size == hw_info->bt_log_efuse_size) {
		sec_ctrl_size = 0;
		is_bt = 1;
	}
	efuse_idx = sec_ctrl_size;

	do {
		if (efuse_idx >= efuse_size) // secure zone
			break;

		value8 = *(phy_map + efuse_idx);
		hdr = value8;
		if (hdr == 0xff)
			break;
		efuse_idx++;

		if (is_bt) {
			if ((hdr & 0xF) != 0xF) {// 1byte entry
				blk_idx = (hdr & 0xF0) >> 4;
				word_en = hdr & 0x0F;
			} else {// 2byte entry
				value8 = *(phy_map + efuse_idx);
				hdr2 = value8;
				if (hdr2 == 0xff)
					break;

				blk_idx = (((hdr2 & 0xF0) >> 4) << 3) +
					  (((hdr & 0xF0) >> 4) >> 1); // offset
				word_en = hdr2 & 0x0F;

				efuse_idx++;
			}
		} else { // WLAN
			value8 = *(phy_map + efuse_idx);
			hdr2 = value8;
			if (hdr2 == 0xff)
				break;

			blk_idx = ((hdr2 & 0xF0) >> 4) | ((hdr & 0x0F) << 4);
			word_en = hdr2 & 0x0F;

			efuse_idx++;
		}

		if (efuse_idx >= efuse_size - 1)
			return MACEFUSEPARSE;

		for (i = 0; i < 4; i++) {
			valid = (u8)((~(word_en >> i)) & BIT(0));
			if (valid == 1) {
				eeprom_idx = (blk_idx << 3) + (i << 1);

				if ((eeprom_idx + 1) >
				    *bank_efuse_info.log_map_size) {
					PLTFM_MSG_ERR("[ERR]efuse idx:0x%X\n",
						      efuse_idx - 1);
					PLTFM_MSG_ERR("[ERR]read hdr:0x%X\n",
						      hdr);
					PLTFM_MSG_ERR("[ERR]read hdr2:0x%X\n",
						      hdr2);
					return MACEFUSEPARSE;
				}

				if (cfg == MAC_AX_EFUSE_PARSER_MAP) {
					value8 = *(phy_map + efuse_idx);
					*(log_map + eeprom_idx) = value8;
				} else if (cfg == MAC_AX_EFUSE_PARSER_MASK) {
					*(log_map + eeprom_idx) = 0x00;
				}

				eeprom_idx++;// 1 byte
				efuse_idx++;

				if (efuse_idx > efuse_size - 1)
					return MACEFUSEPARSE;

				if (cfg == MAC_AX_EFUSE_PARSER_MAP) {
					value8 = *(phy_map + efuse_idx);
					*(log_map + eeprom_idx) = value8;
				} else if (cfg == MAC_AX_EFUSE_PARSER_MASK) {
					*(log_map + eeprom_idx) = 0x00;
				}

				efuse_idx++;

				if (efuse_idx > efuse_size)
					return MACEFUSEPARSE;
			}
		}
	} while (1);

	*bank_efuse_info.efuse_end = efuse_idx;

	return MACSUCCESS;
}

static u32 read_log_efuse_map(struct mac_ax_adapter *adapter, u8 *map,
			      u32 log_efuse_size)
{
	u8 *local_map = NULL;
	u32 efuse_size = *bank_efuse_info.phy_map_size;
	u32 ret;

	if (*bank_efuse_info.log_map_valid == 0) {
		if (*bank_efuse_info.phy_map_valid == 0) {
			local_map = (u8 *)PLTFM_MALLOC(efuse_size);
			if (!local_map) {
				PLTFM_MSG_ERR("[ERR]malloc map\n");
				return MACBUFALLOC;
			}

			ret = read_efuse(adapter, 0, efuse_size, local_map);
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]read efuse\n");
				goto error;
			}

			PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
			PLTFM_MEMCPY(*bank_efuse_info.phy_map, local_map,
				     efuse_size);
			*bank_efuse_info.phy_map_valid = 1;
			PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);

			PLTFM_FREE(local_map, efuse_size);
		}

		ret = eeprom_parser(adapter, *bank_efuse_info.phy_map, map,
				    MAC_AX_EFUSE_PARSER_MAP);
		if (ret != 0)
			return ret;

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(*bank_efuse_info.log_map, map, log_efuse_size);
		*bank_efuse_info.log_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	} else {
		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(map, *bank_efuse_info.log_map, log_efuse_size);
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	}

	return MACSUCCESS;
error:
	PLTFM_FREE(local_map, efuse_size);
	return ret;
}

static u32 proc_pg_efuse_by_map(struct mac_ax_adapter *adapter,
				struct mac_ax_pg_efuse_info *info,
				enum mac_ax_efuse_read_cfg cfg)
{
	u8 *updated_mask = NULL;
	u32 ret;
	u32 log_efuse_size = *bank_efuse_info.log_map_size;
	u32 mask_size = log_efuse_size >> 4;

	updated_mask = (u8 *)PLTFM_MALLOC(mask_size);
	if (!updated_mask) {
		PLTFM_MSG_ERR("[ERR]malloc updated mask\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(updated_mask, 0x00, mask_size);

	ret = update_eeprom_mask(adapter, info, updated_mask, 0);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]update eeprom mask\n");
		goto error;
	}

	ret = check_efuse_enough(adapter, info, updated_mask);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]chk efuse enough\n");
		goto error;
	}

	ret = program_efuse(adapter, info, updated_mask, 0);
	if (ret != 0) {
		PLTFM_MSG_ERR("[ERR]pg efuse\n");
		goto error;
	}

	PLTFM_FREE(updated_mask, mask_size);
	return MACSUCCESS;
error:
	PLTFM_FREE(updated_mask, mask_size);
	return ret;
}

static u32 dump_efuse_drv(struct mac_ax_adapter *adapter)
{
	u8 *map = NULL;
	u32 efuse_size = *bank_efuse_info.phy_map_size;
	u32 ret;

	PLTFM_MSG_TRACE("[TRACE]dump_efuse_drv\n");

	if (*bank_efuse_info.phy_map_valid == 0) {
		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}

		ret = read_hw_efuse(adapter, 0, efuse_size, map);
		if (ret != 0) {
			PLTFM_FREE(map, efuse_size);
			return ret;
		}

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(*bank_efuse_info.phy_map, map, efuse_size);
		*bank_efuse_info.phy_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
		PLTFM_FREE(map, efuse_size);
	}

	return MACSUCCESS;
}

static u32 dump_efuse_fw(struct mac_ax_adapter *adapter)
{
	u32 ret, cnt;
	struct mac_ax_ops *ops = adapter->ops;
	u8 *map = NULL;
	u32 efuse_size = *bank_efuse_info.phy_map_size;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	u8 ofld_type = 0;

	PLTFM_MSG_TRACE("[TRACE]dump_efuse_fw\n");

	if (*bank_efuse_info.phy_map_valid == 0) {
		if (bank_efuse_info.phy_map_size == &hw_info->dav_efuse_size)
			ofld_type = DUMP_OFLD_TYPE_DAV;

		/* H2C */
		ret = ops->dump_efuse_ofld(adapter, efuse_size, ofld_type);
		if (ret)
			return ret;

		/* Wait for C2H */
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			cnt = EFUSE_FW_DUMP_WAIT_CNT;
		else
			cnt = EFUSE_FW_DUMP_WAIT_CNT_V1;

		while (--cnt) {
			if (adapter->sm.efuse_ofld == MAC_AX_OFLD_H2C_DONE)
				break;
			PLTFM_DELAY_MS(1);
		}
		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]efuse C2H\n");
			adapter->sm.efuse_ofld = MAC_AX_OFLD_H2C_IDLE;
			return MACPROCERR;
		}

		/* cpy map */
		map = (u8 *)PLTFM_MALLOC(efuse_size);
		if (!map) {
			PLTFM_MSG_ERR("[ERR]malloc map\n");
			return MACBUFALLOC;
		}
		PLTFM_MEMSET(map, 0xFF, efuse_size);

		ret = ops->efuse_ofld_map(adapter, map, efuse_size);
		if (ret) {
			PLTFM_FREE(map, efuse_size);
			return ret;
		}

		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(*bank_efuse_info.phy_map, map, efuse_size);
		*bank_efuse_info.phy_map_valid = 1;
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
		PLTFM_FREE(map, efuse_size);
	}

	return MACSUCCESS;
}

static u32 proc_write_log_efuse(struct mac_ax_adapter *adapter, u32 offset,
				u8 value)
{
	u8 byte1;
	u8 byte2;
	u8 blk;
	u8 blk_idx;
	u8 hdr;
	u8 hdr2 = 0;
	u8 *map = NULL;
	u32 log_efuse_size = *bank_efuse_info.log_map_size;
	u32 end, ret;
	bool is_bt = 0;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	map = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}

	if (*bank_efuse_info.log_map_valid == 0) {
		ret = read_log_efuse_map(adapter, map, log_efuse_size);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]read logical efuse\n");
			PLTFM_FREE(map, log_efuse_size);
			return ret;
		}
	} else {
		PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
		PLTFM_MEMCPY(map, *bank_efuse_info.log_map, log_efuse_size);
		PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);
	}

	if (log_efuse_size == adapter->hw_info->bt_log_efuse_size)
		is_bt = 1;

	if (*(map + offset) != value) {
		end = *bank_efuse_info.efuse_end;

		if (is_bt) {
			if (offset < BT_1B_ENTRY_SIZE) { // 1 byte entry
				blk = (u8)(offset >> 3);
				blk_idx = (u8)((offset & (8 - 1)) >> 1);//mod8

				hdr = (u8)((blk << 4) +
					   ((0x1 << blk_idx) ^ 0x0F));
			} else { // 2 byte entry
				blk = (u8)(offset >> 3 >> 3); // large section
				blk_idx = (u8)(((offset >> 3) & (8 - 1)) << 1);
				hdr = (u8)((blk_idx << 4) + 0xF);

				blk_idx = (u8)((offset & (8 - 1)) >> 1);
				hdr2 = (u8)((blk << 4) +
					    ((0x1 << blk_idx) ^ 0x0F));
			}
		} else {
			blk = (u8)(offset >> 3);// offset
			blk_idx = (u8)((offset & (8 - 1)) >> 1); // mod8 0 1 2 3

			hdr = ((blk & 0xF0) >> 4) | 0x30;
			hdr2 = (u8)(((blk & 0x0F) << 4) +
				    ((0x1 << blk_idx) ^ 0x0F));
		}

		if ((offset & 1) == 0) {
			byte1 = value;
			byte2 = *(map + offset + 1);
		} else {
			byte1 = *(map + offset - 1);
			byte2 = value;
		}

		if (*bank_efuse_info.phy_map_size <= 4 + end) { // write 4 bytes
			PLTFM_FREE(map, log_efuse_size);
			return MACEFUSESIZE;
		}

		p_ops->enable_efuse_sw_pwr_cut(adapter, 1);

		ret = write_hw_efuse(adapter, end, hdr);
		if (ret != 0)
			goto error;

		if (!is_bt || offset >= BT_1B_ENTRY_SIZE) {
			ret = write_hw_efuse(adapter, end + 1, hdr2);
			if (ret != 0)
				goto error;
			end++;
		}

		ret = write_hw_efuse(adapter, end + 1, byte1);
		if (ret != 0)
			goto error;

		ret = write_hw_efuse(adapter, end + 2, byte2);
		if (ret != 0)
			goto error;

		p_ops->disable_efuse_sw_pwr_cut(adapter, 1);
	}

	PLTFM_FREE(map, log_efuse_size);
	return MACSUCCESS;
error:
	PLTFM_FREE(map, log_efuse_size);
	return ret;
}

static u32 read_efuse(struct mac_ax_adapter *adapter, u32 offset, u32 size,
		      u8 *map)
{
	u32 ret;

	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}

	if (*bank_efuse_info.phy_map_valid == 1) {
		PLTFM_MEMCPY(map, *bank_efuse_info.phy_map + offset, size);
	} else {
		ret = read_hw_efuse(adapter, offset, size, map);
		if (ret != 0)
			return ret;
	}

	return MACSUCCESS;
}

static u32 update_eeprom_mask(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask, bool pg_sim)
{
	u8 *map = NULL;
	u8 *mask_map = NULL;
	u8 clr_bit = 0;
	u8 *map_pg;
	u8 *efuse_mask;
	u32 i;
	u16 j;
	u16 map_offset;
	u16 mask_offset;
	u32 ret;
	u32 log_efuse_size = *bank_efuse_info.log_map_size;

	map = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!map) {
		PLTFM_MSG_ERR("[ERR]malloc map\n");
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(map, 0xFF, log_efuse_size); // default: 0xFF

	if (pg_sim) { //WL
		PLTFM_MEMCPY(map, *bank_efuse_info.log_map, log_efuse_size);
	} else {
		ret = read_log_efuse_map(adapter, map, log_efuse_size);
		if (ret != 0)
			goto error;
	}

	/*log mask*/
	mask_map = (u8 *)PLTFM_MALLOC(log_efuse_size);
	if (!mask_map) {
		PLTFM_MSG_ERR("[ERR]malloc mask map\n");
		PLTFM_FREE(map, log_efuse_size);
		return MACBUFALLOC;
	}
	PLTFM_MEMSET(mask_map, 0xFF, log_efuse_size);// default: 0xFF

	ret = eeprom_parser(adapter, *bank_efuse_info.phy_map,
			    mask_map, MAC_AX_EFUSE_PARSER_MASK);
	if (ret != 0) {
		PLTFM_FREE(mask_map, log_efuse_size);
		goto error;
	}

	map_pg = info->efuse_map;
	efuse_mask = info->efuse_mask;

	PLTFM_MEMCPY(updated_mask, efuse_mask, info->efuse_mask_size);

	for (i = 0; i < info->efuse_map_size; i += 16) {
		for (j = 0; j < 16; j += 2) {
			map_offset = i + j;
			mask_offset = i >> 4;
			if (*(u16 *)(map_pg + map_offset) ==
			    *(u16 *)(map + map_offset) &&
			    *(mask_map + map_offset) == 0x00) {
				switch (j) {
				case 0:
					clr_bit = BIT(4);
					break;
				case 2:
					clr_bit = BIT(5);
					break;
				case 4:
					clr_bit = BIT(6);
					break;
				case 6:
					clr_bit = BIT(7);
					break;
				case 8:
					clr_bit = BIT(0);
					break;
				case 10:
					clr_bit = BIT(1);
					break;
				case 12:
					clr_bit = BIT(2);
					break;
				case 14:
					clr_bit = BIT(3);
					break;
				default:
					break;
				}
				*(updated_mask + mask_offset) &= ~clr_bit;
			}
		}
	}

	PLTFM_FREE(map, log_efuse_size);
	PLTFM_FREE(mask_map, log_efuse_size);
	return MACSUCCESS;
error:
	PLTFM_FREE(map, log_efuse_size);
	return ret;
}

static u32 check_efuse_enough(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info,
			      u8 *updated_mask)
{
	u8 pre_word_en;
	u32 i;
	u16 j;
	u32 eeprom_offset;
	u32 pg_num = 0;

	for (i = 0; i < info->efuse_map_size; i += 8) {
		eeprom_offset = i;

		if ((eeprom_offset & 0xF) > 0)
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
		else
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);

		if (pre_word_en > 0) { // msk ==1 -> write
			pg_num += 2;
			for (j = 0; j < 4; j++) {
				if (((pre_word_en >> j) & 0x1) > 0)
					pg_num += 2;
			}
		}
	}

	if (*bank_efuse_info.phy_map_size
		< (pg_num + *bank_efuse_info.efuse_end))
		return MACEFUSESIZE;

	return MACSUCCESS;
}

static u32 proc_pg_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 word_en,
			 u8 pre_word_en, u32 eeprom_offset, bool pg_sim)
{
	u8 blk, blk_idx;
	u8 hdr;
	u8 hdr2 = 0xFF;
	u8 i;
	u32 efuse_end, ret;
	u8 *efuse_map = *bank_efuse_info.phy_map;
	bool is_bt = 0;

	if (info->efuse_map_size == adapter->hw_info->bt_log_efuse_size)
		is_bt = 1;

	efuse_end = *bank_efuse_info.efuse_end;

	if (is_bt) {
		if (eeprom_offset < BT_1B_ENTRY_SIZE) {
			blk = (u8)(eeprom_offset >> 3);
			hdr = (u8)((blk << 4) + word_en);
		} else {
			blk = (u8)(eeprom_offset >> 3 >> 3);
			blk_idx = (u8)(((eeprom_offset >> 3) & (8 - 1)) << 1);
			hdr = (u8)((blk_idx << 4) | 0xF);
			hdr2 = (u8)((blk << 4) + word_en);
		}
	} else {
		blk = (u8)(eeprom_offset >> 3);
		hdr = ((blk & 0xF0) >> 4) | 0x30;
		hdr2 = (u8)(((blk & 0x0F) << 4) + word_en);
	}

	if (pg_sim) { //WL
		*(efuse_map + efuse_end) = hdr;
		*(efuse_map + efuse_end + 1) = hdr2;

		efuse_end += 2;

		for (i = 0; i < 4; i++) {
			if (((pre_word_en >> i) & 0x1) > 0) {
				*(efuse_map + efuse_end) = *(info->efuse_map +
					eeprom_offset + (i << 1));
				efuse_end++;

				*(efuse_map + efuse_end) = *(info->efuse_map +
					eeprom_offset + (i << 1) + 1);
				efuse_end++;
			}
		}
	} else {
		ret = write_hw_efuse(adapter, efuse_end, hdr);
		if (ret != 0) {
			PLTFM_MSG_ERR("[ERR]write efuse\n");
			return ret;
		}

		if (is_bt == 0 || eeprom_offset >= BT_1B_ENTRY_SIZE) {
			ret = write_hw_efuse(adapter, efuse_end + 1, hdr2);
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]write efuse(+1)\n");
				return ret;
			}
			efuse_end++;
		}

		efuse_end++;
		for (i = 0; i < 4; i++) {
			if (((pre_word_en >> i) & 0x1) > 0) {
				ret = write_hw_efuse(adapter, efuse_end,
						     *(info->efuse_map +
						     eeprom_offset + (i << 1)));
				if (ret != 0) {
					PLTFM_MSG_ERR("[ERR]write efuse\n");
					return ret;
				}

				ret = write_hw_efuse(adapter, efuse_end + 1,
						     *(info->efuse_map +
						     eeprom_offset +
						     (i << 1) + 1));
				if (ret != 0) {
					PLTFM_MSG_ERR("[ERR]write efuse+1\n");
					return ret;
				}
				efuse_end = efuse_end + 2;
			}
		}
	}
	*bank_efuse_info.efuse_end = efuse_end;
	return MACSUCCESS;
}

static u32 program_efuse(struct mac_ax_adapter *adapter,
			 struct mac_ax_pg_efuse_info *info, u8 *updated_mask,
			 bool pg_sim)
{
	u8 pre_word_en;
	u8 word_en;
	u32 i;
	u32 eeprom_offset, ret;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	p_ops->enable_efuse_sw_pwr_cut(adapter, 1);

	for (i = 0; i < info->efuse_map_size; i += 8) {
		eeprom_offset = i;

		if (((eeprom_offset >> 3) & 1) > 0) {
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
			word_en = pre_word_en ^ 0x0F;
		} else {
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);
			word_en = pre_word_en ^ 0x0F;
		}

		if (pre_word_en > 0) {
			ret = proc_pg_efuse(adapter, info, word_en, pre_word_en,
					    eeprom_offset, pg_sim);
			if (ret != 0) {
				PLTFM_MSG_ERR("[ERR]pg efuse");
				return ret;
			}
		}
	}

	p_ops->disable_efuse_sw_pwr_cut(adapter, 1);

	return MACSUCCESS;
}

static void mask_eeprom(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info)
{
	u8 pre_word_en;
	u8 *updated_mask;
	u8 *efuse_map;
	u32 i;
	u16 j;
	u32 offset;

	updated_mask = info->efuse_mask;
	efuse_map = info->efuse_map;

	for (i = 0; i < info->efuse_map_size; i += 8) {
		offset = i;

		if (((offset >> 3) & 1) > 0)
			pre_word_en = (*(updated_mask + (i >> 4)) & 0x0F);
		else
			pre_word_en = (*(updated_mask + (i >> 4)) >> 4);

		for (j = 0; j < 4; j++) {
			if (((pre_word_en >> j) & 0x1) == 0) {
				*(efuse_map + offset + (j << 1)) = 0xFF;
				*(efuse_map + offset + (j << 1) + 1) = 0xFF;
			}
		}
	}
}

static u32 query_status_map(struct mac_ax_adapter *adapter,
			    enum mac_ax_efuse_feature_id feature_id,
			    u8 *map, bool is_limit)
{
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	enum mac_ax_intf intf = adapter->hw_info->intf;
	u32 map_size = 0;

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	switch (feature_id) {
	case MAC_AX_DUMP_PHYSICAL_EFUSE:
		map_size = *bank_efuse_info.phy_map_size;
		PLTFM_MEMCPY(map, *bank_efuse_info.phy_map, map_size);
		break;
	case MAC_AX_DUMP_LOGICAL_EFUSE:
		if (!is_limit) {
			map_size = *bank_efuse_info.log_map_size;
		} else {// WL
			switch (intf) {
			case MAC_AX_INTF_PCIE:
				map_size = hw_info->limit_efuse_size_pcie;
				break;
			case MAC_AX_INTF_USB:
				map_size = hw_info->limit_efuse_size_usb;
				break;
			case MAC_AX_INTF_SDIO:
				map_size = hw_info->limit_efuse_size_sdio;
				break;
			default:
				break;
			}
		}
		PLTFM_MEMCPY(map, *bank_efuse_info.log_map, map_size);
		break;
	default:
		return MACFUNCINPUT;
	}
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);

	return MACSUCCESS;
}

static u32 adjust_mask(struct mac_ax_adapter *adapter,
		       struct mac_ax_pg_efuse_info *info)
{
	u8 *efuse_mask = info->efuse_mask;
	u8 *efuse_map = info->efuse_map;
	u32 i = 0;
	u16 j = 0;
	u16 map_offset;
	u16 mask_offset;
	u8 adj_bit = 0;

	PLTFM_MEMSET(efuse_mask, 0xFF, info->efuse_mask_size);

	for (i = 0; i < info->efuse_map_size; i += 16) {
		for (j = 0; j < 16; j += 2) {
			map_offset = i + j;
			mask_offset = i >> 4;

			if (*(u16 *)(efuse_map + map_offset) == 0xFFFF) {
				switch (j) {
				case 0:
					adj_bit = BIT(4);
					break;
				case 2:
					adj_bit = BIT(5);
					break;
				case 4:
					adj_bit = BIT(6);
					break;
				case 6:
					adj_bit = BIT(7);
					break;
				case 8:
					adj_bit = BIT(0);
					break;
				case 10:
					adj_bit = BIT(1);
					break;
				case 12:
					adj_bit = BIT(2);
					break;
				case 14:
					adj_bit = BIT(3);
					break;
				}
				*(efuse_mask + mask_offset) &= ~adj_bit;
			}
		}
	}

	return MACSUCCESS;
}

static u32 compare_info_length(struct efuse_info *info,
			       enum rtw_efuse_info id, u32 length)
{
	u32 idle_len;

	switch (id) {
	case EFUSE_INFO_MAC_ADDR:
		idle_len = info->len->mac_addr;
		break;
	case EFUSE_INFO_MAC_PID:
		idle_len = info->len->pid;
		break;
	case EFUSE_INFO_MAC_DID:
		idle_len = info->len->did;
		break;
	case EFUSE_INFO_MAC_VID:
		idle_len = info->len->vid;
		break;
	case EFUSE_INFO_MAC_SVID:
		idle_len = info->len->svid;
		break;
	case EFUSE_INFO_MAC_SMID:
		idle_len = info->len->smid;
		break;
	default:
		return MACNOITEM;
	}

	if (length != idle_len || idle_len == 0)
		return MACLENCMP;

	return MACSUCCESS;
}

static u32 set_check_sum_val(struct mac_ax_adapter *adapter,
			     u8 *map, u16 value)
{
	u8 byte1;
	u8 byte2;
	u8 blk;
	u8 blk_idx;
	u8 hdr;
	u8 hdr2;
	u32 end = *bank_efuse_info.efuse_end, ret;
	u32 offset = chksum_offset_1;
	u8 i = 0;
	u8 value8 = (u8)(value & 0xFF);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	p_ops->enable_efuse_sw_pwr_cut(adapter, 1);

	for (i = 0; i < 2; i++) {
		blk = (u8)(offset >> 3);
		blk_idx = (u8)((offset & (8 - 1)) >> 1);

		hdr = ((blk & 0xF0) >> 4) | 0x30;
		hdr2 = (u8)(((blk & 0x0F) << 4) + ((0x1 << blk_idx) ^ 0x0F));

		if ((offset & 1) == 0) {
			byte1 = value8;
			byte2 = *(map + offset + 1);
		} else {
			byte1 = (u8)(value & 0xFF);
			byte2 = value8;
		}

		if (*bank_efuse_info.phy_map_size <= 4 + end)
			return MACEFUSESIZE;

		ret = write_hw_efuse(adapter, end, hdr);
		if (ret != 0)
			return ret;

		ret = write_hw_efuse(adapter, end + 1, hdr2);
		if (ret != 0)
			return ret;

		ret = write_hw_efuse(adapter, end + 2, byte1);
		if (ret != 0)
			return ret;

		ret = write_hw_efuse(adapter, end + 3, byte2);
		if (ret != 0)
			return ret;

		offset = chksum_offset_2;
		value8 = (u8)((value & 0xFF00) >> 8);
		end += 4;
	}

	p_ops->disable_efuse_sw_pwr_cut(adapter, 1);

	return MACSUCCESS;
}

static void cal_check_sum(struct mac_ax_adapter *adapter, u16 *chksum)
{
	u32 i = 0;
	struct mac_ax_hw_info *hw_info = adapter->hw_info;
	struct mac_ax_efuse_param *efuse_param = &adapter->efuse_param;
	u8 *map = efuse_param->log_efuse_map;
	u16 *data;

	data = (u16 *)map;

	*chksum = 0x0000;
	for (i = 0; i < hw_info->log_efuse_size >> 2; i++) {
		if (i == chksum_offset_1 >> 2)
			*chksum ^= 0x0000 ^ *(data + (2 * i + 1));
		else
			*chksum ^= *(data + 2 * i) ^ *(data + (2 * i + 1));
	}
}

static u32 compare_version(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info, u32 ver_len)
{
	u8 *map = info->efuse_map;
	u8 *mask = info->efuse_mask;
	u32 map_size = info->efuse_map_size;
	u32 i = 0;

	for (i = 0; i < ver_len; i++) {
		if (*(map + map_size + i) != *(mask + (map_size >> 4) + i))
			return MACVERERR;
	}
	return MACSUCCESS;
}

u32 enable_efuse_pwr_cut_dav(struct mac_ax_adapter *adapter,
			     bool is_write)
{
	u32 ret;
	u8 chip_id = adapter->hw_info->chip_id;

	if (!(is_write))
		return MACSUCCESS;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		return MACSUCCESS;
	} else {
		ret = mac_write_xtal_si(adapter, XTAL_SI_PWR_CUT,
					XTAL_SI_SMALL_PWR_CUT,
					XTAL_SI_SMALL_PWR_CUT);
		if (ret)
			return ret;

		ret = mac_write_xtal_si(adapter, XTAL_SI_PWR_CUT,
					XTAL_SI_BIG_PWR_CUT,
					XTAL_SI_BIG_PWR_CUT);
		if (ret)
			return ret;
	}

	return MACSUCCESS;
}

u32 disable_efuse_pwr_cut_dav(struct mac_ax_adapter *adapter,
			      bool is_write)
{
	u32 ret;
	u8 chip_id = adapter->hw_info->chip_id;

	if (!(is_write))
		return MACSUCCESS;

	if (chip_id == MAC_AX_CHIP_ID_8852A) {
		return MACSUCCESS;
	} else {
		ret = mac_write_xtal_si(adapter, XTAL_SI_PWR_CUT, 0,
					XTAL_SI_BIG_PWR_CUT);
		if (ret)
			return ret;

		ret = mac_write_xtal_si(adapter, XTAL_SI_PWR_CUT, 0,
					XTAL_SI_SMALL_PWR_CUT);
		if (ret)
			return ret;
	}

	return MACSUCCESS;
}

static u32 read_hw_efuse_dav(struct mac_ax_adapter *adapter, u32 offset, u32 size,
			     u8 *map)
{
	u32 addr;
	u8 tmp8;
	u32 cnt;
	u32 ret;

	ret = enable_efuse_pwr_cut_dav(adapter, 0);
	if (ret)
		return ret;

	for (addr = offset; addr < offset + size; addr++) {
		/* clear ready bit*/
		ret = mac_write_xtal_si(adapter, XTAL_SI_CTRL, 0x40, FULL_BIT_MASK);
		if (ret)
			return ret;
		/* set addr */
		ret = mac_write_xtal_si(adapter, XTAL_SI_LOW_ADDR,
					(addr & 0xff) << XTAL_SI_LOW_ADDR_SH,
					XTAL_SI_LOW_ADDR_MSK);
		if (ret)
			return ret;
		ret = mac_write_xtal_si(adapter, XTAL_SI_CTRL, (addr >> 8)
					<< XTAL_SI_HIGH_ADDR_SH,
					XTAL_SI_HIGH_ADDR_MSK
					<< XTAL_SI_HIGH_ADDR_SH);
		if (ret)
			return ret;
		/* set ctrl mode sel */
		ret = mac_write_xtal_si(adapter, XTAL_SI_CTRL, 0,
					XTAL_SI_MODE_SEL_MSK
					<< XTAL_SI_MODE_SEL_SH);
		if (ret)
			return ret;

		/* polling */
		cnt = EFUSE_WAIT_CNT;
		while (--cnt) {
			ret = mac_read_xtal_si(adapter, XTAL_SI_CTRL, &tmp8);
			if (ret)
				return ret;
			if (tmp8 & XTAL_SI_RDY)
				break;
			PLTFM_DELAY_US(1);
		}
		if (!cnt) {
			PLTFM_MSG_ERR("[ERR]read efuse\n");
			return MACEFUSEREAD;
		}

		ret = mac_read_xtal_si(adapter, XTAL_SI_READ_VAL, &tmp8);
		if (ret)
			return ret;
		*(map + addr - offset) = tmp8;
	}

	ret = disable_efuse_pwr_cut_dav(adapter, 0);
	if (ret)
		return ret;

	return MACSUCCESS;
}

static u32 write_hw_efuse_dav(struct mac_ax_adapter *adapter, u32 offset,
			      u8 value)
{
	u32 cnt;
	u32 ret;
	u8 tmp8;

	PLTFM_MUTEX_LOCK(&efuse_tbl.lock);
	*bank_efuse_info.phy_map_valid = 0;
	*bank_efuse_info.log_map_valid = 0;
	PLTFM_MUTEX_UNLOCK(&efuse_tbl.lock);

	/* clear ready bit*/
	ret = mac_write_xtal_si(adapter, XTAL_SI_CTRL, 0x40, FULL_BIT_MASK);
	if (ret)
		return ret;
	/* set val */
	ret = mac_write_xtal_si(adapter, XTAL_SI_WRITE_VAL, value <<
				XTAL_SI_WRITE_DATA_SH, XTAL_SI_WRITE_DATA_MSK <<
				XTAL_SI_WRITE_DATA_SH);
	if (ret)
		return ret;
	/* set addr */
	ret = mac_write_xtal_si(adapter, XTAL_SI_LOW_ADDR, (offset & 0xff)
				<< XTAL_SI_LOW_ADDR_SH,
				XTAL_SI_LOW_ADDR_MSK << XTAL_SI_LOW_ADDR_SH);
	if (ret)
		return ret;
	ret = mac_write_xtal_si(adapter, XTAL_SI_CTRL, (offset >> 8)
				<< XTAL_SI_HIGH_ADDR_SH,
				XTAL_SI_HIGH_ADDR_MSK << XTAL_SI_HIGH_ADDR_SH);
	if (ret)
		return ret;
	/* set ctrl mode sel */
	ret = mac_write_xtal_si(adapter, XTAL_SI_CTRL, 0x2
				<< XTAL_SI_MODE_SEL_SH,
				XTAL_SI_MODE_SEL_MSK <<	XTAL_SI_MODE_SEL_SH);
	if (ret)
		return ret;

	/* polling */
	cnt = EFUSE_WAIT_CNT;
	while (--cnt) {
		ret = mac_read_xtal_si(adapter, XTAL_SI_CTRL, &tmp8);
		if (ret)
			return ret;
		if (tmp8 & XTAL_SI_RDY)
			break;
		PLTFM_DELAY_US(1);
	}
	if (!cnt) {
		PLTFM_MSG_ERR("[ERR]write efuse\n");
		return MACEFUSEREAD;
	}

	return MACSUCCESS;
}

static void switch_dv(struct mac_ax_adapter *adapter, enum rtw_dv_sel sel)
{
	dv_sel = sel;
}

