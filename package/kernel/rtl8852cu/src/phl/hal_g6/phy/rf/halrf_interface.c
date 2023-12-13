/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "halrf_precomp.h"

u32 halrf_get_sys_time(struct rf_info *rf)
{
	return 0;
}

u32 halrf_cal_bit_shift(u32 bit_mask)
{
	u32 i;

	for (i = 0; i <= 31; i++) {
		if ((bit_mask >> i) & BIT0)
			break;
	}
	return i;
}

void halrf_wmac(struct rf_info *rf, u32 addr, u32 mask, u32 val)
{
	u32 ori_val, bit_shift;
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	u32 fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif



	if (fw_ofld == true && rf->fw_ofld_enable == true) {
		rf->fw_ofld_start = true;
		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] MAC   addr=0x%08x   val=0x%08x\n",
			addr, val);

		cmd.src = RTW_MAC_MAC_CMD_OFLD;
		cmd.type = RTW_MAC_WRITE_OFLD;
		cmd.lc = 0;
		cmd.offset = (u16)addr;
		cmd.value = val;
		cmd.mask = mask;

		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}
		rf->fw_w_count++;
	}
	else
#endif
	{
		if (mask != MASKDWORD) {
			ori_val = halrf_rmac32(rf, addr);
			bit_shift = halrf_cal_bit_shift(mask);
			val = ((ori_val) & (~mask)) |( ((val << bit_shift)) & mask);
		}
		halrf_wmac32(rf, addr, val);
	}
	rf->w_count++;
}

u32 halrf_rmac32(struct rf_info *rf, u32 addr)
{
	u32 reg_val = 0;
	
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	if (rf->fw_ofld_start == true)
		halrf_write_fwofld_end(rf);
#endif
	rf->r_count++;
	reg_val = hal_read32((rf)->hal_com, addr);
	return reg_val;
}
void halrf_wreg(struct rf_info *rf, u32 addr, u32 mask, u32 val)
{
	u32 ori_val, bit_shift;
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	u32 fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;
#endif

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif
//	u32 page_temp;
//	u32 offset_temp;	
//	u32 temp = 0;

//	page_temp = addr & 0xff00;
//	offset_temp = addr & 0xff;

//	if ((page_temp != 0x4c00) && (page_temp != 0x4d00)) {
//		if (offset_temp <= 0x9c)
//			temp = halrf_r32(rf, offset_temp | 0x4c00);
//	}
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	if (fw_ofld == true && rf->fw_ofld_enable == true) {
		rf->fw_ofld_start = true;
		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] BB   addr=0x%08x   mask=0x%08x   val=0x%08x\n",
			addr, mask, val);

		cmd.src = RTW_MAC_BB_CMD_OFLD;
		cmd.type = RTW_MAC_WRITE_OFLD;
		cmd.lc = 0;
		cmd.offset = (u16)addr;
		cmd.value = val;
		cmd.mask = mask;

		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}

		rf->fw_w_count++;
	}
	else
#endif
	{
		if (mask != MASKDWORD) {
			ori_val = halrf_r32(rf, addr);
			bit_shift = halrf_cal_bit_shift(mask);
			val = ((ori_val) & (~mask)) |( ((val << bit_shift)) & mask);
		}
		halrf_w32(rf, addr, val);
	}
	rf->w_count++;
//	if ((page_temp != 0x4c00) && (page_temp != 0x4d00)) {
//		if (offset_temp <= 0x9c)
//			halrf_w32(rf, offset_temp | 0x4c00, temp);
//	}
}

u32 halrf_rreg(struct rf_info *rf, u32 addr, u32 mask)
{
	u32 reg_val = 0, ori_val, bit_shift;

#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	if (rf->fw_ofld_start == true)
		halrf_write_fwofld_trigger(rf);

#endif
	ori_val = halrf_r32(rf, addr);
	bit_shift = halrf_cal_bit_shift(mask);
	reg_val = (ori_val & mask) >> bit_shift;
	rf->r_count++;
	return reg_val;
}

u32 halrf_rrf(struct rf_info *rf, enum rf_path path, u32 addr, u32 mask)
{
	static u32 operate2 = 0;
	u32 val = 0;
	
	if (_os_atomic_inc_return(rf->hal_com->drv_priv, (void*)&operate2) > 1) {
		RF_WARNING("RF read race %x %x %x!!!!!!!!!!!!!!", path, addr, mask);
	}

#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	if (rf->fw_ofld_start == true)
		halrf_write_fwofld_trigger(rf);
#endif

	val = rtw_hal_read_rf_reg((rf)->hal_com, path, addr, mask);
	_os_atomic_dec(rf->hal_com->drv_priv, (void*)&operate2);
	rf->r_count++;
	return val;
}

void halrf_wrf(struct rf_info *rf, enum rf_path path, u32 addr, u32 mask, u32 val)
{
	static u32 operate2 = 0;
	
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	u32 fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;
	u32 direct_addr, offset_write_rf[2] = {0xe000, 0xf000};

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	if (_os_atomic_inc_return(rf->hal_com->drv_priv, (void*)&operate2) > 1) {
		RF_WARNING("RF write race %x %x %x %x!!!!!!!!!!!!!!", path, addr, mask, val);
	}


	if (fw_ofld == true && rf->fw_ofld_enable == true) {
		rf->fw_ofld_start = true;
		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] RF   addr=0x%08x   mask=0x%08x   val=0x%08x   path=%d\n",
			addr, mask, val, path);

		if (addr & BIT(16)) {	/*RF D-die*/
			/*==== Calculate offset ====*/
			addr &= 0xff;
			direct_addr = offset_write_rf[path] + (addr << 2);

			cmd.src = RTW_MAC_BB_CMD_OFLD;
			cmd.type = RTW_MAC_WRITE_OFLD;
			cmd.lc = 0;
			cmd.offset = (u16)direct_addr;
			cmd.value = val;
			cmd.mask = mask;
		} else {
			cmd.src = RTW_MAC_RF_CMD_OFLD;
			cmd.type = RTW_MAC_WRITE_OFLD;
			cmd.lc = 0;
			cmd.rf_path = path;
			cmd.offset = (u16)addr;
			cmd.value = val;
			cmd.mask = mask;
		}

		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] cmd.src=0x%x   addr=0x%x\n", cmd.src, addr);

		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}

		rf->fw_w_count++;
	} else
#endif
		rtw_hal_write_rf_reg((rf)->hal_com, path, addr, mask, val);

	_os_atomic_dec(rf->hal_com->drv_priv, (void*)&operate2);
	rf->w_count++;
}

bool halrf_polling_bb(struct rf_info *rf, u32 addr, u32 mask, u32 val, u32 count)
{
	bool result = true;
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	u32 fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	if (fw_ofld == true && rf->fw_ofld_enable == true) {
		rf->fw_ofld_start = true;
		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] Polling BB addr=0x%08x   mask=0x%08x   val=0x%08x\n",
			addr, mask, val);

		cmd.src = RTW_MAC_BB_CMD_OFLD;
		cmd.type = RTW_MAC_COMPARE_OFLD;
		cmd.lc = 0;
		cmd.offset = (u16)addr;
		cmd.value = val;
		cmd.mask = mask;

		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}
		if (rtn == 0)
			result = true;
		else
			result = false;
	}
	else
#endif
	{
		u32 c = 0;
		while (halrf_rreg(rf, addr, mask) != val) {
			c++;
			halrf_delay_us(rf, 1);
			if (c > count) {
				result = false;
				break;
			}
		}
	}

	return result;
}

bool halrf_polling_rf(struct rf_info *rf, u32 path, u32 addr, u32 mask, u32 val, u32 count)
{
	bool result = true;
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	u32 fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	if (fw_ofld == true && rf->fw_ofld_enable == true) {
		rf->fw_ofld_start = true;
		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] Polling RF path=%d   addr=0x%08x   mask=0x%08x   val=0x%08x\n",
			path, addr, mask, val);

		if (addr & BIT(16))
			cmd.src = RTW_MAC_RF_DDIE_CMD_OFLD;
		else
			cmd.src = RTW_MAC_RF_CMD_OFLD;


		cmd.type = RTW_MAC_COMPARE_OFLD;
		cmd.lc = 0;
		cmd.rf_path = path;
		cmd.offset = (u16)addr;
		cmd.value = val;
		cmd.mask = mask;

		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}
		if (rtn == 0)
			result = true;
		else
			result = false;
	}
	else
#endif
	{
		u32 c = 0;
		while (halrf_rrf(rf, path, addr, mask) != val) {
			c++;
			halrf_delay_us(rf, 1);
			if (c > count) {
				result = false;
				break;
			}
		}
	}

	return result;
}


void halrf_delay_us(struct rf_info *rf, u32 count)
{
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	u32 fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	if (fw_ofld == true && rf->fw_ofld_enable == true) {
		rf->fw_ofld_start= true;
		RF_DBG(rf, DBG_RF_FW,
			"[FW_Ofld] ======>%s count=%d\n",
			__func__, count);

		cmd.type = RTW_MAC_DELAY_OFLD;
		cmd.lc = 0;
		cmd.value = count;

		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}

		rf->fw_delay_us_count = rf->fw_delay_us_count + count;
	}
	else
#endif
	{
		u32 i;

		for (i = 0; i < count; i++)
			halrf_os_delay_us(rf, 1);
	}
}

void halrf_fill_h2c_cmd(struct rf_info *rf, u16 cmdlen, u8 cmdid,
			u8 classid, u32 cmdtype, u32 *pval)
{
	u32 rt_val = 0;
	struct rtw_g6_h2c_hdr hdr = {0};
	struct rtw_hal_com_t *hal_com = NULL;

	hdr.h2c_class = classid;
	hdr.h2c_func = cmdid;
	hdr.type = cmdtype;
	hdr.content_len = cmdlen;
	hal_com = rf->hal_com;
	RF_DBG(rf, DBG_RF_IQK, "[IQK]======>%s   H2C: %x %x %x\n",
		__func__, classid, cmdid, cmdlen);
	rt_val =  rtw_hal_mac_send_h2c(hal_com, &hdr, pval);
	if (rt_val != 0) {
		RF_WARNING("Error H2C CLASS=%d, ID=%d\n", classid, cmdid);
		RF_DBG(rf, DBG_RF_IQK, "Error H2C CLASS=%d, ID=%d\n", classid, cmdid);
	}
}

