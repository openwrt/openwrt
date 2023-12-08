/** @file */
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

#ifndef _MAC_AX_EFUSE_H_
#define _MAC_AX_EFUSE_H_

#include "../type.h"
#include "fwcmd.h"

#define RSVD_EFUSE_SIZE		16
#define RSVD_CS_EFUSE_SIZE	24
#define EFUSE_WAIT_CNT		10000
#define EFUSE_WAIT_CNT_PLUS	30000
#define EFUSE_C2HREG_WAIT_CNT	10000
#define EFUSE_C2HREG_RETRY_WAIT_US 1
#define EFUSE_FW_DUMP_WAIT_CNT	100
#define EFUSE_FW_DUMP_WAIT_CNT_V1 400
#define OTP_PHY_SIZE		0x800
#define CHK_OTP_ADDR		0x4
#define CHK_OTP_WAIT_CNT	50000
#define DUMMY_READ_DELAY	200

#define BT_1B_ENTRY_SIZE	0x80
#define UNLOCK_CODE		0x69

#define XTAL_SI_PWR_CUT		0x10
#define XTAL_SI_SMALL_PWR_CUT	BIT(0)
#define XTAL_SI_BIG_PWR_CUT	BIT(1)

#define XTAL_SI_LOW_ADDR	0x62
#define XTAL_SI_LOW_ADDR_SH	0
#define XTAL_SI_LOW_ADDR_MSK	0xFF

#define XTAL_SI_CTRL		0x63
#define XTAL_SI_MODE_SEL_SH	6
#define XTAL_SI_MODE_SEL_MSK	0x3
#define XTAL_SI_RDY		BIT(5)
#define XTAL_SI_HIGH_ADDR_SH	0
#define XTAL_SI_HIGH_ADDR_MSK	0x7

#define XTAL_SI_READ_VAL	0x7A
#define XTAL_SI_WRITE_VAL	0x60
#define XTAL_SI_WRITE_DATA_SH	0
#define XTAL_SI_WRITE_DATA_MSK	0xFF

#define DUMP_OFLD_TYPE_HIDDEN	1
#define DUMP_OFLD_TYPE_DAV	2

#define BT_DIS_WAIT_CNT	100
#define BT_DIS_WAIT_US	50

extern struct mac_bank_efuse_info bank_efuse_info;
extern enum rtw_dv_sel dv_sel;

/**
 * @struct mac_efuse_tbl
 * @brief mac_efuse_tbl
 *
 * @var mac_efuse_tbl::lock
 * Please Place Description here.
 */
struct mac_efuse_tbl {
	mac_ax_mutex lock;
};

/**
 * @struct mac_efuse_hidden_h2creg
 * @brief mac_efuse_hidden_h2creg
 *
 * @var mac_efuse_hidden_h2creg::rsvd0
 * Please Place Description here.
 */
struct mac_efuse_hidden_h2creg {
	/* dword0 */
	u32 rsvd0:16;
};

/**
 * @enum efuse_map_sel
 *
 * @brief efuse_map_sel
 *
 * @var efuse_map_sel::EFUSE_MAP_SEL_PHY_WL
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_PHY_BT
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_LOG
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_LOG_BT
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_PHY_OTP
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_LAST
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_MAX
 * Please Place Description here.
 * @var efuse_map_sel::EFUSE_MAP_SEL_INVALID
 * Please Place Description here.
 */
enum efuse_map_sel {
	EFUSE_MAP_SEL_PHY_WL,
	EFUSE_MAP_SEL_PHY_BT,
	EFUSE_MAP_SEL_LOG,
	EFUSE_MAP_SEL_LOG_BT,
	EFUSE_MAP_SEL_PHY_OTP,
	EFUSE_MAP_SEL_PHY_DAV,
	EFUSE_MAP_SEL_LOG_DAV,
	EFUSE_MAP_SEL_HIDDEN_RF,

	/* keep last */
	EFUSE_MAP_SEL_LAST,
	EFUSE_MAP_SEL_MAX = EFUSE_MAP_SEL_LAST,
	EFUSE_MAP_SEL_INVALID = EFUSE_MAP_SEL_LAST,
};

/**
 * @struct efuse_info_item
 * @brief efuse_info_item
 *
 * @var efuse_info_item::mac_addr
 * MAC Address
 * @var efuse_info_item::pid
 * Product ID
 * @var efuse_info_item::did
 * Device ID
 * @var efuse_info_item::vid
 * Vendor ID
 * @var efuse_info_item::svid
 * Sybsystem Vendor ID
 * @var efuse_info_offset::smid
 * Sybsystem Device ID
 */
struct efuse_info_item {
	u32 mac_addr;
	u32 pid;
	u32 did;
	u32 vid;
	u32 svid;
	u32 smid;
};

/**
 * @struct efuse_info
 * @brief efuse_info
 *
 * @var efuse_info::offset
 * Efuse information offset
 * @var efuse_info::def_val
 * Efuse information default value
 * @var efuse_info::len
 * Efuse information length
 */
struct efuse_info {
	struct efuse_info_item *offset;
	struct efuse_info_item *def_val;
	struct efuse_info_item *len;
};

/**
 * @enum mac_checksum_offset
 *
 * @brief mac_checksum_offset
 *
 * @var mac_checksum_offset::chksum_offset_1
 * Please Place Description here.
 * @var mac_checksum_offset::chksum_offset_2
 * Please Place Description here.
 */
enum mac_checksum_offset {
	chksum_offset_1 = 0x1AC,
	chksum_offset_2 = 0x1AD,
};

/**
 * @struct mac_bank_efuse_info
 * @brief mac_bank_efuse_info
 *
 * @var mac_bank_efuse_info::phy_map
 * Please Place Description here.
 * @var mac_bank_efuse_info::log_map
 * Please Place Description here.
 * @var mac_bank_efuse_info::phy_map_valid
 * Please Place Description here.
 * @var mac_bank_efuse_info::log_map_valid
 * Please Place Description here.
 * @var mac_bank_efuse_info::efuse_end
 * Please Place Description here.
 * @var mac_bank_efuse_info::phy_map_size
 * Please Place Description here.
 * @var mac_bank_efuse_info::log_map_size
 * Please Place Description here.
 */
struct mac_bank_efuse_info {
	/* efuse_param */
	u8 **phy_map;
	u8 **log_map;
	u8 *phy_map_valid;
	u8 *log_map_valid;
	u32 *efuse_end;
	/* hw_info */
	u32 *phy_map_size;
	u32 *log_map_size;
	u32 *efuse_start;
};

/**
 * @enum mac_defeature_offset
 *
 * @brief mac_defeature_offset
 *
 * @var mac_defeature_offset::rx_spatial_stream
 * Please Place Description here.
 * @var mac_defeature_offset::rx_spatial_stream_sh
 * Please Place Description here.
 * @var mac_defeature_offset::rx_spatial_stream_msk
 * Please Place Description here.
 * @var mac_defeature_offset::bandwidth
 * Please Place Description here.
 * @var mac_defeature_offset::bandwidth_sh
 * Please Place Description here.
 * @var mac_defeature_offset::bandwidth_msk
 * Please Place Description here.
 * @var mac_defeature_offset::tx_spatial_stream
 * Please Place Description here.
 * @var mac_defeature_offset::tx_spatial_stream_sh
 * Please Place Description here.
 * @var mac_defeature_offset::tx_spatial_stream_msk
 * Please Place Description here.
 * @var mac_defeature_offset::protocol_80211
 * Please Place Description here.
 * @var mac_defeature_offset::protocol_80211_sh
 * Please Place Description here.
 * @var mac_defeature_offset::protocol_80211_msk
 * Please Place Description here.
 * @var mac_defeature_offset::NIC_router
 * Please Place Description here.
 * @var mac_defeature_offset::NIC_router_sh
 * Please Place Description here.
 * @var mac_defeature_offset::NIC_router_msk
 * Please Place Description here.
 */
enum mac_defeature_offset {
	rx_spatial_stream = 0xB,
	rx_spatial_stream_sh = 0x4,
	rx_spatial_stream_msk = 0x7,
	bandwidth = 0xD,
	bandwidth_sh = 0x0,
	bandwidth_msk = 0x7,
	tx_spatial_stream = 0xD,
	tx_spatial_stream_sh = 0x4,
	tx_spatial_stream_msk = 0x7,
	protocol_80211 = 0x11,
	protocol_80211_sh = 0x2,
	protocol_80211_msk = 0x3,
	NIC_router = 0x11,
	NIC_router_sh = 0x6,
	NIC_router_msk = 0x3,
};

/**
 * @enum mac_cntlr_mode_sel
 *
 * @brief mac_cntlr_mode_sel
 *
 * @var mac_cntlr_mode_sel::MODE_READ
 * Please Place Description here.
 * @var mac_cntlr_mode_sel::MODE_AUTOLOAD_EN
 * Please Place Description here.
 * @var mac_cntlr_mode_sel::MODE_WRITE
 * Please Place Description here.
 * @var mac_cntlr_mode_sel::MODE_CMP
 * Please Place Description here.
 */
enum mac_cntlr_mode_sel {
	MODE_READ,
	MODE_AUTOLOAD_EN,
	MODE_WRITE,
	MODE_CMP,
};

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_dump_efuse_map_wl
 *
 * @param *adapter
 * @param cfg
 * @param *efuse_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dump_efuse_map_wl_plus(struct mac_ax_adapter *adapter,
			       enum mac_ax_efuse_read_cfg cfg, u8 *efuse_map);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_dump_efuse_map_wl
 *
 * @param *adapter
 * @param cfg
 * @param *efuse_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dump_efuse_map_wl(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_read_cfg cfg, u8 *efuse_map);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_dump_efuse_map_bt
 *
 * @param *adapter
 * @param cfg
 * @param *efuse_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dump_efuse_map_bt(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_read_cfg cfg, u8 *efuse_map);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_write_efuse
 *
 * @param *adapter
 * @param addr
 * @param val
 * @param bank
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u8 val,
			 enum mac_ax_efuse_bank bank);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_write_efuse
 *
 * @param *adapter
 * @param addr
 * @param val
 * @param bank
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val,
		    enum mac_ax_efuse_bank bank);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_efuse_plus
 *
 * @param *adapter
 * @param addr
 * @param size
 * @param *val
 * @param bank
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			u8 *val, enum mac_ax_efuse_bank bank);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_efuse
 *
 * @param *adapter
 * @param addr
 * @param size
 * @param *val
 * @param bank
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size, u8 *val,
		   enum mac_ax_efuse_bank bank);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_hidden_efuse
 *
 * @param *adapter
 * @param addr
 * @param size
 * @param *val
 * @param hidden_cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_hidden_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			  u8 *val, enum mac_ax_efuse_hidden_cfg hidden_cfg);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_get_efuse_avl_size
 *
 * @param *adapter
 * @param *size
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_efuse_avl_size(struct mac_ax_adapter *adapter, u32 *size);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_get_efuse_avl_size_bt
 *
 * @param *adapter
 * @param *size
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_efuse_avl_size_bt(struct mac_ax_adapter *adapter, u32 *size);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_dump_log_efuse
 *
 * @param *adapter
 * @param parser_cfg
 * @param cfg
 * @param *efuse_map
 * @param is_limit
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dump_log_efuse_plus(struct mac_ax_adapter *adapter,
			    enum mac_ax_efuse_parser_cfg parser_cfg,
			    enum mac_ax_efuse_read_cfg cfg,
			    u8 *efuse_map, bool is_limit);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_dump_log_efuse
 *
 * @param *adapter
 * @param parser_cfg
 * @param cfg
 * @param *efuse_map
 * @param is_limit
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dump_log_efuse(struct mac_ax_adapter *adapter,
		       enum mac_ax_efuse_parser_cfg parser_cfg,
		       enum mac_ax_efuse_read_cfg cfg,
		       u8 *efuse_map, bool is_limit);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_log_efuse_plus
 *
 * @param *adapter
 * @param addr
 * @param size
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_log_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			    u8 *val);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_log_efuse
 *
 * @param *adapter
 * @param addr
 * @param size
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u32 size,
		       u8 *val);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_write_log_efuse_plus
 *
 * @param *adapter
 * @param addr
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_log_efuse_plus(struct mac_ax_adapter *adapter, u32 addr, u8 val);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_write_log_efuse
 *
 * @param *adapter
 * @param addr
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_log_efuse(struct mac_ax_adapter *adapter, u32 addr, u8 val);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_dump_log_efuse_bt
 *
 * @param *adapter
 * @param parser_cfg
 * @param cfg
 * @param *efuse_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dump_log_efuse_bt(struct mac_ax_adapter *adapter,
			  enum mac_ax_efuse_parser_cfg parser_cfg,
			  enum mac_ax_efuse_read_cfg cfg,
			  u8 *efuse_map);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_log_efuse_bt
 *
 * @param *adapter
 * @param addr
 * @param size
 * @param *val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_log_efuse_bt(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			  u8 *val);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_write_log_efuse_bt
 *
 * @param *adapter
 * @param addr
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_write_log_efuse_bt(struct mac_ax_adapter *adapter, u32 addr, u8 val);

/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_pg_efuse_by_map_plus
 *
 * @param *adapter
 * @param *info
 * @param cfg
 * @param part
 * @param is_limit
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pg_efuse_by_map_plus(struct mac_ax_adapter *adapter,
			     struct mac_ax_pg_efuse_info *info,
			     enum mac_ax_efuse_read_cfg cfg,
			     bool part, bool is_limit);

/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_pg_efuse_by_map
 *
 * @param *adapter
 * @param *info
 * @param cfg
 * @param part
 * @param is_limit
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pg_efuse_by_map(struct mac_ax_adapter *adapter,
			struct mac_ax_pg_efuse_info *info,
			enum mac_ax_efuse_read_cfg cfg, bool part,
			bool is_limit);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_pg_efuse_by_map_bt
 *
 * @param *adapter
 * @param *info
 * @param cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pg_efuse_by_map_bt(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info,
			   enum mac_ax_efuse_read_cfg cfg);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_mask_log_efuse
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_mask_log_efuse(struct mac_ax_adapter *adapter,
		       struct mac_ax_pg_efuse_info *info);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_pg_sec_data_by_map
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pg_sec_data_by_map(struct mac_ax_adapter *adapter,
			   struct mac_ax_pg_efuse_info *info);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_cmp_sec_data_by_map
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_cmp_sec_data_by_map(struct mac_ax_adapter *adapter,
			    struct mac_ax_pg_efuse_info *info);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_get_efuse_info
 *
 * @param *adapter
 * @param *efuse_map
 * @param id
 * @param *value
 * @param length
 * @param *autoload_status
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_efuse_info(struct mac_ax_adapter *adapter, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length,
		       u8 *autoload_status);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_set_efuse_info
 *
 * @param *adapter
 * @param *efuse_map
 * @param id
 * @param *value
 * @param length
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_efuse_info(struct mac_ax_adapter *adapter, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_read_hidden_rpt
 *
 * @param *adapter
 * @param *rpt
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_read_hidden_rpt(struct mac_ax_adapter *adapter,
			struct mac_defeature_value *rpt);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_check_efuse_autoload
 *
 * @param *adapter
 * @param *autoload_status
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_check_efuse_autoload(struct mac_ax_adapter *adapter,
			     u8 *autoload_status);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_pg_simulator_plus
 *
 * @param *adapter
 * @param *info
 * @param *phy_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pg_simulator_plus(struct mac_ax_adapter *adapter,
			  struct mac_ax_pg_efuse_info *info, u8 *phy_map);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_pg_simulator
 *
 * @param *adapter
 * @param *info
 * @param *phy_map
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pg_simulator(struct mac_ax_adapter *adapter,
		     struct mac_ax_pg_efuse_info *info, u8 *phy_map);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_checksum_update
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_checksum_update(struct mac_ax_adapter *adapter);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_checksum_rpt
 *
 * @param *adapter
 * @param *chksum
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_checksum_rpt(struct mac_ax_adapter *adapter, u16 *chksum);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_disable_rf_ofld_by_info
 *
 * @param *adapter
 * @param info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_disable_rf_ofld_by_info(struct mac_ax_adapter *adapter,
				struct mac_disable_rf_ofld_info info);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief _patch_otp_power_issue
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 _patch_otp_power_issue(struct mac_ax_adapter *adapter);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_set_efuse_ctrl
 *
 * @param *adapter
 * @param is_secure
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_efuse_ctrl(struct mac_ax_adapter *adapter, bool is_secure);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief mac_otp_test
 *
 * @param *adapter
 * @param is_OTP_test
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_otp_test(struct mac_ax_adapter *adapter, bool is_OTP_test);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief cfg_efuse_auto_ck
 *
 * @param *adapter
 * @param enable
 * @return Please Place Description here.
 * @retval void
 */
void cfg_efuse_auto_ck(struct mac_ax_adapter *adapter, u8 enable);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief efuse_tbl_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 efuse_tbl_init(struct mac_ax_adapter *adapter);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief efuse_tbl_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 efuse_tbl_exit(struct mac_ax_adapter *adapter);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief enable_efuse_pwr_cut_dav
 *
 * @param *adapter
 * @param is_write
 * @return Please Place Description here.
 * @retval u32
 */
u32 enable_efuse_pwr_cut_dav(struct mac_ax_adapter *adapter,
			     bool is_write);
/**
 * @}
 */

/**
 * @addtogroup Efuse
 * @{
 */

/**
 * @brief enable_efuse_pwr_cut_dav
 *
 * @param *adapter
 * @param is_write
 * @return Please Place Description here.
 * @retval u32
 */
u32 disable_efuse_pwr_cut_dav(struct mac_ax_adapter *adapter,
			      bool is_write);
/**
 * @}
 */

#endif
