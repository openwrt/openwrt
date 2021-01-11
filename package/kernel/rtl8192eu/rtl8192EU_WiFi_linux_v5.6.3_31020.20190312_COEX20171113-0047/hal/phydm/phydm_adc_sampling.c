/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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

#include "mp_precomp.h"
#include "phydm_precomp.h"

#if (PHYDM_LA_MODE_SUPPORT)

#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
#if (RTL8197F_SUPPORT || RTL8822B_SUPPORT || RTL8192F_SUPPORT)
#include "rtl8197f/Hal8197FPhyReg.h"
#include "WlanHAL/HalMac88XX/halmac_reg2.h"
#else
#include "WlanHAL/HalHeader/HalComReg.h"
#endif
#elif (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if WPP_SOFTWARE_TRACE
#include "phydm_adc_sampling.tmh"
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE | ODM_AP))
boolean
phydm_la_buffer_allocate(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	void *adapter = dm->adapter;
	#endif
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;
	boolean ret = true;

	pr_debug("[LA mode BufferAllocate]\n");

	if (buf->length == 0) {
	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
		if (PlatformAllocateMemoryWithZero(adapter, (void **)&
						   buf->octet,
						   buf->buffer_size) !=
						   RT_STATUS_SUCCESS)
			ret = false;
	#else
		odm_allocate_memory(dm, (void **)&buf->octet, buf->buffer_size);

		if (!buf->octet)
			ret = false;
	#endif

		if (ret)
			buf->length = buf->buffer_size;
	}

	return ret;
}
#endif

void phydm_la_get_tx_pkt_buf(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;
	u32 i = 0, value32 = 0, data_l = 0, data_h = 0;
	u32 addr = 0, finish_addr = 0;
	boolean is_round_up = false;
	static u32 page = 0xFF;
	u32 smp_cnt = 0, smp_number = 10, addr_8byte = 0;
	#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	#if (RTL8197F_SUPPORT || RTL8198F_SUPPORT)
	u8 backup_dma = 0;
	#endif
	#endif

	odm_memory_set(dm, buf->octet, 0, buf->length);
	pr_debug("GetTxPktBuf\n");

	if (dm->support_ic_type & ODM_RTL8192F) {
		value32 = odm_read_4byte(dm, R_0x7f0);
		is_round_up = (boolean)((value32 & BIT(31)) >> 31);
		/*Reg7F0[30:15]: finish addr (unit: 8byte)*/
		finish_addr = (value32 & 0x7FFF8000) >> 15;
	} else {
		odm_write_1byte(dm, R_0x0106, 0x69);
		value32 = odm_read_4byte(dm, R_0x7c0);
		is_round_up = (boolean)((value32 & BIT(31)) >> 31);
		/*Reg7C0[30:16]: finish addr (unit: 8byte)*/
		if (dm->support_ic_type & (ODM_RTL8822B | ODM_RTL8822C |
		    ODM_RTL8821C | ODM_RTL8814A | ODM_RTL8814B | ODM_RTL8812F |
		    ODM_RTL8195B))
			finish_addr = (value32 & 0x7FFF0000) >> 16;
		/*Reg7C0[30:15]: finish addr (unit: 8byte)*/
		else if (dm->support_ic_type & (ODM_RTL8198F | ODM_RTL8197F))
			finish_addr = (value32 & 0x7FFF8000) >> 15;
	}

	#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	#if (RTL8197F_SUPPORT || RTL8198F_SUPPORT)
	if (dm->support_ic_type & (ODM_RTL8197F | ODM_RTL8198F)) {
		pr_debug("98F GetTxPktBuf from iMEM\n");
		odm_set_bb_reg(dm, R_0x7c0, BIT(0), 0x0);

		/*Stop DMA*/
		backup_dma = odm_get_mac_reg(dm, R_0x300, MASKLWORD);
		odm_set_mac_reg(dm, R_0x300, 0x7fff, 0x7fff);

		/*@move LA mode content from IMEM to TxPktBuffer
			Source : OCPBASE_IMEM 0x00000000
			Destination : OCPBASE_TXBUF 0x18780000
			Length : 64K*/
		GET_HAL_INTERFACE(dm->priv)->init_ddma_handler(dm->priv,
							       OCPBASE_IMEM,
							       OCPBASE_TXBUF
							       + buf->start_pos,
							       0x10000);
	}
	#endif
	#endif

	pr_debug("start_addr = ((0x%x)), end_addr = ((0x%x)), buffer_size = ((0x%x))\n",
		 buf->start_pos, buf->end_pos, buf->buffer_size);
	if (is_round_up) {
		pr_debug("buf_start(0x%x)|----2---->|finish_addr(0x%x)|----1---->|buf_end(0x%x)\n",
			 buf->start_pos, finish_addr << 3, buf->end_pos);
		addr = (finish_addr + 1) << 3;
		pr_debug("is_round_up = ((%d)), finish_addr=((0x%x)), 0x7c0/0x7F0=((0x%x))\n",
			 is_round_up, finish_addr, value32);
		/*@Byte to 8Byte (64bit)*/
		smp_number = (buf->buffer_size) >> 3;
	} else {
		pr_debug("buf_start(0x%x)|------->|finish_addr(0x%x)             |buf_end(0x%x)\n",
			 buf->start_pos, finish_addr << 3, buf->end_pos);
		addr = buf->start_pos;
		addr_8byte = addr >> 3;

		if (addr_8byte > finish_addr)
			smp_number = addr_8byte - finish_addr;
		else
			smp_number = finish_addr - addr_8byte;

		pr_debug("is_round_up = ((%d)), finish_addr=((0x%x * 8Byte)), Start_Addr = ((0x%x * 8Byte)), smp_number = ((%d))\n",
			 is_round_up, finish_addr, addr_8byte, smp_number);
	}
	#if 0
	dbg_print("is_round_up = %d, finish_addr=0x%x, value32=0x%x\n",
		  is_round_up, finish_addr, value32);
	dbg_print(
		  "end_addr = %x, buf->start_pos = 0x%x, buf->buffer_size = 0x%x\n",
		  end_addr, buf->start_pos, buf->buffer_size);
	#endif

	#if (RTL8197F_SUPPORT || RTL8198F_SUPPORT || RTL8814B_SUPPORT)
	if (dm->support_ic_type &
	    (ODM_RTL8197F | ODM_RTL8198F | ODM_RTL8814B)) {
		for (addr = buf->start_pos, i = 0; addr < buf->end_pos;
		     addr += 8, i += 2) {
			if ((addr & 0xfff) == 0)
				odm_set_bb_reg(dm, R_0x0140, MASKLWORD, 0x780 +
					       (addr >> 12));
			data_l = odm_get_bb_reg(dm, 0x8000 + (addr & 0xfff),
						MASKDWORD);
			data_h = odm_get_bb_reg(dm, 0x8000 + (addr & 0xfff) +
						4, MASKDWORD);
			buf->octet[i] = data_h;
			buf->octet[i + 1] = data_l;

			if (smp->is_la_print)
				pr_debug("%08x%08x\n", data_h, data_l);
		}
	} else
	#endif
	{
		for (i = 0; smp_cnt < smp_number; smp_cnt++, i += 2) {
			if (dm->support_ic_type & ODM_RTL8192F) {
			#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
				indirect_access_sdram_8192f(dm->adapter,
							    TX_PACKET_BUFFER,
							    TRUE,
							    (u16)addr >> 3, 0,
							    &data_h, &data_l);
			#else
			odm_write_1byte(dm, R_0x0106, 0x69);
			odm_set_bb_reg(dm, R_0x0140, MASKDWORD, addr >> 3);
			data_l = odm_get_bb_reg(dm, R_0x0144, MASKDWORD);
			data_h = odm_get_bb_reg(dm, R_0x0148, MASKDWORD);
			odm_write_1byte(dm, R_0x0106, 0x0);
			#endif

			} else {
				if (page != (addr >> 12)) {
				/* Reg140=0x780+(addr>>12),
				 * addr=0x30~0x3F, total 16 pages
				 */
					page = addr >> 12;
				}
				odm_set_bb_reg(dm, R_0x0140, MASKLWORD, 0x780 +
					       page);

				/*pDataL = 0x8000+(addr&0xfff);*/
				data_l = odm_get_bb_reg(dm, 0x8000 + (addr &
							0xfff),	MASKDWORD);
				data_h = odm_get_bb_reg(dm, 0x8000 + (addr &
							0xfff) + 4, MASKDWORD);
			}
			#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
			buf->octet[i] = data_h;
			buf->octet[i + 1] = data_l;
			#endif
			if (smp->is_la_print) {
				#if DBG /*WIN driver check build*/
				pr_debug("%08x%08x\n", data_h, data_l);
				#else	/*WIN driver free build*/
				#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
				RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD,
					    ("%08x%08x\n", buf->octet[i],
					    buf->octet[i + 1]));
				#elif (DM_ODM_SUPPORT_TYPE & ODM_IOT)
				pr_debug("%08x%08x\n", data_h, data_l);
				#endif
				#endif
			}
			if ((addr + 8) > buf->end_pos)
				addr = buf->start_pos;
			else
				addr = addr + 8;
		}
		pr_debug("smp_cnt = ((%d))\n", smp_cnt);

		#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
		RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD,
			    ("smp_cnt = ((%d))\n", smp_cnt));
		#endif
	}
	#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	#if (RTL8197F_SUPPORT)
	if (dm->support_ic_type & ODM_RTL8197F)
		odm_set_mac_reg(dm, R_0x300, 0x7fff, backup_dma);/*Resume DMA*/
	#endif
	#endif
}

void phydm_la_mode_set_mac_iq_dump(void *dm_void, boolean en_fake_trig)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	u32 reg_value = 0;
	u32 reg1 = 0, reg2 = 0, reg3 = 0;

	if (dm->support_ic_type & ODM_RTL8192F) {
		reg1 = R_0x7f0;
		reg2 = R_0x7f4;
		reg3 = R_0x7f8;
	} else {
		reg1 = R_0x7c0;
		reg2 = R_0x7c4;
		reg3 = R_0x7c8;
	}

	odm_write_1byte(dm, reg1, 0); /*@clear all reg1*/
	/*@Enable LA mode HW block*/
	odm_set_mac_reg(dm, reg1, BIT(0), 1);

	if (smp->la_trig_mode == PHYDM_MAC_TRIG) {
		smp->is_bb_trigger = 0;
		/*polling bit for MAC mode*/
		odm_set_mac_reg(dm, reg1, BIT(2), 1);
		/*trigger mode for MAC*/
		odm_set_mac_reg(dm, reg1, BIT(4) | BIT(3),
				smp->la_trigger_edge);
		pr_debug("[MAC_trig] ref_mask = ((0x%x)), ref_value = ((0x%x)), dbg_port = ((0x%x))\n",
			 smp->la_mac_mask_or_hdr_sel, smp->la_trig_sig_sel,
			 smp->la_dbg_port);
		/*@[Set MAC Debug Port]*/
		odm_set_mac_reg(dm, R_0xf4, BIT(16), 1);
		odm_set_mac_reg(dm, R_0x38, 0xff0000, smp->la_dbg_port);
		odm_set_mac_reg(dm, reg2, MASKDWORD,
				smp->la_mac_mask_or_hdr_sel);
		odm_set_mac_reg(dm, reg3, MASKDWORD, smp->la_trig_sig_sel);

	} else {
		smp->is_bb_trigger = 1;

		if (smp->la_trig_mode == PHYDM_ADC_MAC_TRIG) {
			/*polling bit for MAC trigger event*/
			if (!en_fake_trig)
				odm_set_mac_reg(dm, reg1, BIT(3), 1);

			odm_set_mac_reg(dm, reg1, BIT(7) | BIT(6),
					smp->la_trig_sig_sel);
			if (smp->la_trig_sig_sel == ADCSMP_TRIG_REG)
				/* @manual trigger reg1[5] = 0->1*/
				odm_set_mac_reg(dm, reg1, BIT(5), 1);
		}
		/*polling bit for BB ADC mode*/
		odm_set_mac_reg(dm, reg1, BIT(1), 1);
	}

	reg_value = odm_get_bb_reg(dm, reg1, 0xff);
	pr_debug("4. [Set MAC IQ dump] 0x%x[7:0] = ((0x%x))\n", reg1,
		 reg_value);

	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD,
		    ("4. [Set MAC IQ dump] 0x%x[7:0] = ((0x%x))\n", reg1,
		    reg_value));
	#endif
}

void phydm_adc_smp_start(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	u8 tmp_u1b = 0;
	u8 while_cnt = 0;
	u8 target_polling_bit = 0;
	boolean polling_ok = false;

	if (smp->en_fake_trig)
		smp->is_fake_trig = true;
	else
		smp->is_fake_trig = false;

	phydm_la_mode_bb_setting(dm, smp->en_fake_trig);
	phydm_la_mode_set_trigger_time(dm, smp->la_trigger_time);

	if (dm->support_ic_type & (ODM_RTL8197F | ODM_RTL8192F))
		odm_set_bb_reg(dm, R_0xd00, BIT(26), 0x1);
	else if (dm->support_ic_type & ODM_IC_JGR3_SERIES)
		odm_set_bb_reg(dm, R_0x1eb4, BIT(23), 0x1);
	else
		odm_write_1byte(dm, R_0x8b4, 0x80);
#if 0
		/* odm_set_bb_reg(dm, R_0x8b4, BIT(7), 1); */
#endif

	phydm_la_mode_set_mac_iq_dump(dm, smp->en_fake_trig);

	#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	watchdog_stop(dm->priv);
	#endif

	if (smp->en_fake_trig) {
		ODM_delay_ms(100);
		smp->is_fake_trig = false;
		phydm_la_mode_bb_setting(dm, smp->en_fake_trig);

		if (smp->la_trig_mode == PHYDM_ADC_MAC_TRIG) {
			if (dm->support_ic_type & ODM_RTL8192F)
				odm_set_mac_reg(dm, R_0x7f0, BIT(3), 1);
			else
				odm_set_mac_reg(dm, R_0x7c0, BIT(3), 1);
		}
	}
#if RTL8198F_SUPPORT
	phydm_la_pre_run(dm);
#endif

	target_polling_bit = (smp->is_bb_trigger) ? BIT(1) : BIT(2);
	do { /*Polling time always use 100ms, when it exceed 2s, break loop*/
		if (dm->support_ic_type & ODM_RTL8192F) {
			tmp_u1b = odm_read_1byte(dm, R_0x7f0);
			pr_debug("[%d], 0x7F0[7:0] = ((0x%x))\n", while_cnt,
				 tmp_u1b);
		} else {
			tmp_u1b = odm_read_1byte(dm, R_0x7c0);
			pr_debug("[%d], 0x7C0[7:0] = ((0x%x))\n", while_cnt,
				 tmp_u1b);
		}

		if (smp->adc_smp_state != ADCSMP_STATE_SET) {
			pr_debug("[state Error] adc_smp_state != ADCSMP_STATE_SET\n");
			break;

		} else if (tmp_u1b & target_polling_bit) {
			ODM_delay_ms(100);
			while_cnt = while_cnt + 1;
			continue;
		} else {
			pr_debug("[LA Query OK] polling_bit=((0x%x))\n",
				 target_polling_bit);
			polling_ok = true;
			break;
		}
	} while (while_cnt < 20);

	if (smp->adc_smp_state == ADCSMP_STATE_SET) {
		if (polling_ok)
			phydm_la_get_tx_pkt_buf(dm);
		else
			pr_debug("[Polling timeout]\n");
	}

	#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	watchdog_resume(dm->priv);
	#endif

	#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	if (smp->adc_smp_state == ADCSMP_STATE_SET)
		smp->adc_smp_state = ADCSMP_STATE_QUERY;
	#endif

	pr_debug("[LA mode] LA_pattern_count = ((%d))\n", smp->la_count);
	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD,
		    ("[LA mode] la_count = ((%d))\n", smp->la_count));
	#endif

	adc_smp_stop(dm);

	if (smp->la_count == 0) {
		pr_debug("LA Dump finished ---------->\n\n\n");
		phydm_release_bb_dbg_port(dm);

		if ((dm->support_ic_type & ODM_RTL8821C) &&
		    dm->cut_version >= ODM_CUT_B)
			odm_set_bb_reg(dm, R_0x95c, BIT(23), 0);
		else if (dm->support_ic_type & ODM_RTL8195B)
			odm_set_bb_reg(dm, R_0x95c, BIT(23), 0);

	} else {
		smp->la_count--;
		pr_debug("LA Dump more ---------->\n\n\n");
		adc_smp_set(dm, smp->la_trig_mode, smp->la_trig_sig_sel,
			    smp->la_dma_type, smp->la_trigger_time, 0);
	}
}

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
void adc_smp_work_item_callback(void *context)
{
	void *adapter = (void *)context;
	PHAL_DATA_TYPE hal_data = GET_HAL_DATA(((PADAPTER)adapter));
	struct dm_struct *dm = &hal_data->DM_OutSrc;
	struct rt_adcsmp *smp = &dm->adcsmp;

	pr_debug("[WorkItem Call back] LA_State=((%d))\n", smp->adc_smp_state);
	phydm_adc_smp_start(dm);
}
#endif

void adc_smp_set(void *dm_void, u8 trig_mode, u32 trig_sig_sel,
		 u8 dma_data_sig_sel, u32 trig_time, u16 polling_time)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean is_set_success = true;
	struct rt_adcsmp *smp = &dm->adcsmp;

	smp->la_trig_mode = trig_mode;
	smp->la_trig_sig_sel = trig_sig_sel;
	smp->la_dma_type = dma_data_sig_sel;
	smp->la_trigger_time = trig_time;

	#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE | ODM_AP))
	if (smp->adc_smp_state != ADCSMP_STATE_IDLE)
		is_set_success = false;
	else if (smp->adc_smp_buf.length == 0)
		is_set_success = phydm_la_buffer_allocate(dm);
	#endif

	if (is_set_success) {
		smp->adc_smp_state = ADCSMP_STATE_SET;

		pr_debug("[LA Set Success] LA_State=((%d))\n",
			 smp->adc_smp_state);

		#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)

		pr_debug("ADCSmp_work_item_index = ((%d))\n",
			 smp->la_work_item_index);
		if (smp->la_work_item_index != 0) {
			odm_schedule_work_item(&smp->adc_smp_work_item_1);
			smp->la_work_item_index = 0;
		} else {
			odm_schedule_work_item(&smp->adc_smp_work_item);
			smp->la_work_item_index = 1;
		}
		#else
		phydm_adc_smp_start(dm);
		#endif
	} else {
		pr_debug("[LA Set Fail] LA_State=((%d))\n", smp->adc_smp_state);
	}
}

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
enum rt_status
adc_smp_query(void *dm_void, ULONG info_buf_length, void *info_buf,
	      PULONG bytes_written)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	enum rt_status ret_status = RT_STATUS_SUCCESS;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;

	pr_debug("[%s] LA_State=((%d))", __func__, smp->adc_smp_state);

	if (info_buf_length != buf->buffer_size) {
		*bytes_written = 0;
		ret_status = RT_STATUS_RESOURCE;
	} else if (buf->length != buf->buffer_size) {
		*bytes_written = 0;
		ret_status = RT_STATUS_RESOURCE;
	} else if (smp->adc_smp_state != ADCSMP_STATE_QUERY) {
		*bytes_written = 0;
		ret_status = RT_STATUS_PENDING;
	} else {
		odm_move_memory(dm, info_buf, buf->octet, buf->buffer_size);
		*bytes_written = buf->buffer_size;

		smp->adc_smp_state = ADCSMP_STATE_IDLE;
	}

	pr_debug("Return status %d\n", ret_status);

	return ret_status;
}
#elif (DM_ODM_SUPPORT_TYPE & ODM_CE)

void adc_smp_query(void *dm_void, void *output, u32 out_len, u32 *pused)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;
	u32 used = *pused;
	u32 i = 0;
#if 0
	/* struct timespec t; */
	/* rtw_get_current_timespec(&t); */
#endif

	pr_debug("%s adc_smp_state %d", __func__, smp->adc_smp_state);

	for (i = 0; i < (buf->length >> 2) - 2; i += 2) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "%08x%08x\n", buf->octet[i], buf->octet[i + 1]);
	}

	PDM_SNPF(out_len, used, output + used, out_len - used, "\n");
	/* PDM_SNPF(output + used, out_len - used, "\n[%lu.%06lu]\n", */
	/*	    t.tv_sec, t.tv_nsec); */
	*pused = used;
}

s32 adc_smp_get_sample_counts(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;

	return (buf->length >> 2) - 2;
}

s32 adc_smp_query_single_data(void *dm_void, void *output, u32 out_len, u32 idx)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;
	u32 used = 0;

	/* @dbg_print("%s adc_smp_state %d\n", __func__,*/
	/*	      smp->adc_smp_state);*/
	if (smp->adc_smp_state != ADCSMP_STATE_QUERY) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Error: la data is not ready yet ...\n");
		return -1;
	}

	if (idx < ((buf->length >> 2) - 2)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "%08x%08x\n", buf->octet[idx], buf->octet[idx + 1]);
	}
	return 0;
}

#endif

void adc_smp_stop(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;

	smp->adc_smp_state = ADCSMP_STATE_IDLE;

	PHYDM_DBG(dm, DBG_TMP, "[LA_Stop] LA_state = %d\n", smp->adc_smp_state);
}

void adc_smp_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;

	smp->adc_smp_state = ADCSMP_STATE_IDLE;
	smp->is_la_print = true;
	smp->is_fake_trig = false;
	smp->en_fake_trig = false;
	phydm_la_set_buff_mode(dm, ADCSMP_BUFF_HALF);

	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	phydm_la_bb_adv_reset_jgr3(dm);
	#endif
}

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
void adc_smp_de_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;

	adc_smp_stop(dm);

	if (buf->length != 0x0) {
		odm_free_memory(dm, buf->octet, buf->length);
		buf->length = 0x0;
	}
}

#endif

#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT

void phydm_la_bb_adv_reset_jgr3(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;

	smp->la_en_new_bbtrigger = false;

	smp->la_ori_bb_dis = false;
	smp->la_and1_sel = 0;
	smp->la_and1_val = 0;
	smp->la_and2_sel = 0;
	smp->la_and2_val = 0;
	smp->la_and3_sel = 0;
	smp->la_and3_val = 0;
	smp->la_and4_en = 0;
	smp->la_and4_val = 0;
}

void phydm_la_bb_adv_cmd_jgr3(void *dm_void, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	u32 var1[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 enable;

	if (!(dm->support_ic_type & ODM_IC_JGR3_SERIES))
		return;

	PHYDM_SSCANF(input[3], DCMD_DECIMAL, &var1[0]);

	enable = var1[0];

	if (enable == 1) {
		smp->la_en_new_bbtrigger = true;

		PHYDM_SSCANF(input[4], DCMD_DECIMAL, &var1[1]);
		PHYDM_SSCANF(input[5], DCMD_DECIMAL, &var1[2]);
		PHYDM_SSCANF(input[6], DCMD_HEX, &var1[3]);
		PHYDM_SSCANF(input[7], DCMD_DECIMAL, &var1[4]);
		PHYDM_SSCANF(input[8], DCMD_HEX, &var1[5]);
		PHYDM_SSCANF(input[9], DCMD_DECIMAL, &var1[6]);
		PHYDM_SSCANF(input[10], DCMD_HEX, &var1[7]);
		PHYDM_SSCANF(input[11], DCMD_HEX, &var1[8]);
		PHYDM_SSCANF(input[12], DCMD_HEX, &var1[9]);

		smp->la_ori_bb_dis = (boolean)var1[1];
		smp->la_and1_sel = (u8)var1[2];
		smp->la_and1_val = (u8)var1[3];
		smp->la_and2_sel = (u8)var1[4];
		smp->la_and2_val = (u8)var1[5];
		smp->la_and3_sel = (u8)var1[6];
		smp->la_and3_val = (u8)var1[7];
		smp->la_and4_en = (u32)var1[8];
		smp->la_and4_val = (u32)var1[9];
	} else {
		phydm_la_bb_adv_reset_jgr3(dm);
	}

	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "{en %d} {C0_dis %d} {C1_sel %d} {C1_val %d} {C2_sel %d} {C2_val %d}\n{C3_sel %d} {C3_val %d} {C4_en %d}{C4_val %d}\n",
		 enable, smp->la_ori_bb_dis, smp->la_and1_sel, smp->la_and1_val,
		 smp->la_and2_sel, smp->la_and2_val,
		 smp->la_and3_sel, smp->la_and3_val,
		 smp->la_and4_en, smp->la_and4_val);
}

#endif

void phydm_la_mode_bb_setting(void *dm_void, boolean en_fake_trig)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;

	u8	trig_mode = smp->la_trig_mode;
	u32	trig_sel = smp->la_trig_sig_sel;
	u32	dbg_port = smp->la_dbg_port;
	u8	edge = smp->la_trigger_edge;
	u8	smp_rate = smp->la_smp_rate;
	u8	dma_type = smp->la_dma_type;
	u8	is_fake_trig = smp->is_fake_trig;
	u32	dbg_port_hdr_sel = 0;
	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	boolean en_new_bbtrigger = smp->la_en_new_bbtrigger;
	boolean ori_bb_dis = smp->la_ori_bb_dis;
	u8	and1_sel = smp->la_and1_sel;
	u8	and1_val = smp->la_and1_val;
	u8	and2_sel = smp->la_and2_sel;
	u8	and2_val = smp->la_and2_val;
	u8	and3_sel = smp->la_and3_sel;
	u8	and3_val = smp->la_and3_val;
	u32	and4_en = smp->la_and4_en;
	u32	and4_val = smp->la_and4_val;
	#endif

	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD,
		    ("1. [LA mode bb_setting]trig_mode = ((%d)), dbg_port = ((0x%x)), Trig_Edge = ((%d)), smp_rate = ((%d)), Trig_Sel = ((0x%x)), Dma_type = ((%d))\n",
		    trig_mode, dbg_port, edge, smp_rate, trig_sel, dma_type));
	#endif

	if (trig_mode == PHYDM_MAC_TRIG)
		trig_sel = 0; /*@ignore this setting*/

	/*set BB debug port*/
	if (is_fake_trig) {
		if (phydm_set_bb_dbg_port(dm, DBGPORT_PRI_3, 0xf))
			pr_debug("Set fake dbg_port success\n");
		/*@BB debug port bit*/
		if (dm->support_ic_type & ODM_IC_11AC_SERIES) {
			odm_set_bb_reg(dm, R_0x95c, 0x1f, 0x0);
		#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
		} else if (dm->support_ic_type & ODM_IC_JGR3_SERIES) {
			if (!(en_new_bbtrigger))
				odm_set_bb_reg(dm, R_0x1ce4, 0x3e000, 0x0);
			else if (!(ori_bb_dis))
				odm_set_bb_reg(dm, R_0x1ce4, 0x3e000, 0x0);
		#endif
		} else {
			odm_set_bb_reg(dm, R_0x9a0, 0x1f, 0x0);
		}

		pr_debug("0. [BB Setting] fake-trigger!\n");
	} else {
		if (en_fake_trig)
			phydm_release_bb_dbg_port(dm);
		if (phydm_set_bb_dbg_port(dm, DBGPORT_PRI_3, dbg_port))
			pr_debug("Set dbg_port((0x%x)) success\n", dbg_port);
		else
			pr_debug("Set dbg_port fail!\n");
		/*@debug port bit*/
		if (dm->support_ic_type & ODM_IC_11AC_SERIES) {
			odm_set_bb_reg(dm, R_0x95c, 0x1f, trig_sel);
		#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
		} else if (dm->support_ic_type & ODM_IC_JGR3_SERIES) {
			if (!(en_new_bbtrigger))
				odm_set_bb_reg(dm, R_0x1ce4, 0x3e000, trig_sel);
			else if (!(ori_bb_dis))
				odm_set_bb_reg(dm, R_0x1ce4, 0x3e000, trig_sel);
		#endif
		} else {
			odm_set_bb_reg(dm, R_0x9a0, 0x1f, trig_sel);
		}
		pr_debug("1. [BB Setting] trig_mode = ((%d)), dbg_port = ((0x%x)), Trig_Edge = ((%d)), smp_rate = ((%d)), Trig_Sel = ((0x%x)), Dma_type = ((%d))\n",
			 trig_mode, dbg_port, edge, smp_rate, trig_sel,
			 dma_type);
		if (en_fake_trig)
			return;
	}

	if (dm->support_ic_type & ODM_IC_11AC_SERIES) {
		if (trig_mode == PHYDM_ADC_RF0_TRIG)
			dbg_port_hdr_sel = 9; /*@DBGOUT_RFC_a[31:0]*/
		else if (trig_mode == PHYDM_ADC_RF1_TRIG)
			dbg_port_hdr_sel = 8; /*@DBGOUT_RFC_b[31:0]*/
		else if ((trig_mode == PHYDM_ADC_BB_TRIG) ||
			 (trig_mode == PHYDM_ADC_MAC_TRIG)) {
			if (smp->la_mac_mask_or_hdr_sel <= 0xf)
				dbg_port_hdr_sel = smp->la_mac_mask_or_hdr_sel;
			else
				dbg_port_hdr_sel = 0;
		}

		phydm_bb_dbg_port_header_sel(dm, dbg_port_hdr_sel);

		/*@0x95C[11:8]*/
		odm_set_bb_reg(dm, R_0x95c, 0xf00, dma_type);
		/*@0: posedge, 1: negedge*/
		odm_set_bb_reg(dm, R_0x95c, BIT(31), edge);
		odm_set_bb_reg(dm, R_0x95c, 0xe0, smp_rate);
		/*	@(0:) '80MHz'
		 *	(1:) '40MHz'
		 *	(2:) '20MHz'
		 *	(3:) '10MHz'
		 *	(4:) '5MHz'
		 *	(5:) '2.5MHz'
		 *	(6:) '1.25MHz'
		 *	(7:) '160MHz (for BW160 ic)'
		 */
		if ((dm->support_ic_type & ODM_RTL8821C) &&
		    (dm->cut_version >= ODM_CUT_B))
			odm_set_bb_reg(dm, R_0x95c, BIT(23), 1);
		else if (dm->support_ic_type & ODM_RTL8195B)
			odm_set_bb_reg(dm, R_0x95c, BIT(23), 1);

	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	} else if (dm->support_ic_type & ODM_IC_JGR3_SERIES) {
		/*@MAC-PHY timing*/
		odm_set_bb_reg(dm, R_0x1ce4, BIT(7) | BIT(6), 0);
		odm_set_bb_reg(dm, R_0x1cf4, BIT(23), 1); /*@LA mode on*/
		odm_set_bb_reg(dm, R_0x1ce4, 0x3f, dma_type);
		/*@0: posedge, 1: negedge ??*/
		odm_set_bb_reg(dm, R_0x1ce4, BIT(26), edge);
		odm_set_bb_reg(dm, R_0x1ce4, 0x700, smp_rate);

		if (!en_new_bbtrigger) { /*normal LA mode & back to default*/

			pr_debug("Set bb default setting\n");

			/*path 1 default: enable ori. BB trigger*/
			odm_set_bb_reg(dm, R_0x1ce4, BIT(27), 0);

			/*@AND1~AND4 default: off*/
			odm_set_bb_reg(dm, R_0x1ce4, MASKH4BITS, 0); /*@AND 1*/
			odm_set_bb_reg(dm, R_0x1ce8, 0x1f, 0); /*@AND 1 val*/
			odm_set_bb_reg(dm, R_0x1ce8, BIT(5), 0); /*@AND 1 inv*/

			odm_set_bb_reg(dm, R_0x1ce8, 0x3c0, 0); /*@AND 2*/
			odm_set_bb_reg(dm, R_0x1ce8, 0x7c00, 0); /*@AND 2 val*/
			/*@AND 2 inv*/
			odm_set_bb_reg(dm, R_0x1ce8, BIT(15), 0);

			odm_set_bb_reg(dm, R_0x1ce8, 0xf0000, 0); /*@AND 3*/
			/*@AND 3 val*/
			odm_set_bb_reg(dm, R_0x1ce8, 0x1f00000, 0);
			/*@AND 3 inv*/
			odm_set_bb_reg(dm, R_0x1ce8, BIT(25), 0);

			/*@AND 4 en*/
			odm_set_bb_reg(dm, R_0x1cf0, MASKDWORD, 0);
			/*@AND 4 val*/
			odm_set_bb_reg(dm, R_0x1cec, MASKDWORD, 0);
			/*@AND 4 inv*/
			odm_set_bb_reg(dm, R_0x1ce8, BIT(26), 0);

			pr_debug("Set bb default setting finished\n");

		} else if (en_new_bbtrigger) {
			/*path 1 default: enable ori. BB trigger*/
			if (ori_bb_dis)
				odm_set_bb_reg(dm, R_0x1ce4, BIT(27), 1);
			else
				odm_set_bb_reg(dm, R_0x1ce4, BIT(27), 0);

			/* @AND1 */
			odm_set_bb_reg(dm, R_0x1ce8, BIT(5), 0); /*@invert*/

			if (and1_sel == 0x4 || and1_sel == 0x5 ||
			    and1_sel == 0x6) {
				/* rx_state, rx_state_freq, field */
				odm_set_bb_reg(dm, R_0x1ce4, MASKH4BITS,
					       and1_sel);
				odm_set_bb_reg(dm, R_0x1ce8, 0x1f, and1_val);

			} else if (and1_sel == 0x7) {
				/* @mux state */
				odm_set_bb_reg(dm, R_0x1ce4, MASKH4BITS,
					       and1_sel);
				odm_set_bb_reg(dm, R_0x1ce8, 0xf, and1_val);

			} else {
				odm_set_bb_reg(dm, R_0x1ce4, MASKH4BITS,
					       and1_sel);
			}

			/* @AND2 */
			odm_set_bb_reg(dm, R_0x1ce8, BIT(15), 0); /*@invert*/

			if (and2_sel == 0x4 || and2_sel == 0x5 ||
			    and2_sel == 0x6) {
				/* rx_state, rx_state_freq, field */
				odm_set_bb_reg(dm, R_0x1ce8, 0x3c0, and2_sel);
				odm_set_bb_reg(dm, R_0x1ce8, 0x7c00, and2_val);

			} else if (and2_sel == 0x7) {
				/* @mux state */
				odm_set_bb_reg(dm, R_0x1ce8, 0x3c0, and2_sel);
				odm_set_bb_reg(dm, R_0x1ce8, 0x3c00, and2_val);

			} else {
				odm_set_bb_reg(dm, R_0x1ce8, 0x3c0, and2_sel);
			}

			/* @AND3 */
			odm_set_bb_reg(dm, R_0x1ce8, BIT(25), 0); /*@invert*/

			if (and3_sel == 0x4 || and3_sel == 0x5 ||
			    and3_sel == 0x6) {
				/* rx_state, rx_state_freq, field */
				odm_set_bb_reg(dm, R_0x1ce8, 0xf0000, and3_sel);
				odm_set_bb_reg(dm, R_0x1ce8, 0x1f00000,
					       and3_val);

			} else if (and3_sel == 0x7) {
				/* @mux state */
				odm_set_bb_reg(dm, R_0x1ce8, 0xf0000, and3_sel);
				odm_set_bb_reg(dm, R_0x1ce8, 0xf00000,
					       and3_val);
			} else {
				odm_set_bb_reg(dm, R_0x1ce8, 0xf0000, and3_sel);
			}

			/* @AND4 */
			odm_set_bb_reg(dm, R_0x1ce8, BIT(26), 0); /*@invert*/
			odm_set_bb_reg(dm, R_0x1cf0, MASKDWORD, and4_en);
			odm_set_bb_reg(dm, R_0x1cec, MASKDWORD, and4_val);
			}
	#endif
	} else {
		#if (RTL8192F_SUPPORT)
		if ((dm->support_ic_type & ODM_RTL8192F))
			/*@LA reset HW block enable for true-mac asic*/
			odm_set_bb_reg(dm, R_0x9a0, BIT(15), 1);
		#endif
		/*@0x9A0[11:8]*/
		odm_set_bb_reg(dm, R_0x9a0, 0xf00, dma_type);
		/*@0: posedge, 1: negedge*/
		odm_set_bb_reg(dm, R_0x9a0, BIT(31), edge);
		odm_set_bb_reg(dm, R_0x9a0, 0xe0, smp_rate);
		/*	@(0:) '80MHz'
		 *	(1:) '40MHz'
		 *	(2:) '20MHz'
		 *	(3:) '10MHz'
		 *	(4:) '5MHz'
		 *	(5:) '2.5MHz'
		 *	(6:) '1.25MHz'
		 *	(7:) '160MHz (for BW160 ic)'
		 */
	}
}

void phydm_la_mode_set_trigger_time(void *dm_void, u32 trigger_time_mu_sec)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 time_unit_num = 0;
	u32 unit = 0;

	if (trigger_time_mu_sec < 128)
		unit = 0; /*unit: 1mu sec*/
	else if (trigger_time_mu_sec < 256)
		unit = 1; /*unit: 2mu sec*/
	else if (trigger_time_mu_sec < 512)
		unit = 2; /*unit: 4mu sec*/
	else if (trigger_time_mu_sec < 1024)
		unit = 3; /*unit: 8mu sec*/
	else if (trigger_time_mu_sec < 2048)
		unit = 4; /*unit: 16mu sec*/
	else if (trigger_time_mu_sec < 4096)
		unit = 5; /*unit: 32mu sec*/
	else if (trigger_time_mu_sec < 8192)
		unit = 6; /*unit: 64mu sec*/

	time_unit_num = (u8)(trigger_time_mu_sec >> unit);

	pr_debug("2. [Set Trigger Time] Trig_Time = ((%d)) * unit = ((2^%d us))\n",
		 time_unit_num, unit);
	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD, (
		    "3. [Set Trigger Time] Trig_Time = ((%d)) * unit = ((2^%d us))\n",
		    time_unit_num, unit));
	#endif

	if (dm->support_ic_type & ODM_RTL8192F) {
		odm_set_mac_reg(dm, R_0x7fc, BIT(2) | BIT(1) | BIT(0), unit);
		odm_set_mac_reg(dm, R_0x7f0, 0x7f00, (time_unit_num & 0x7f));
	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	} else if (dm->support_ic_type & ODM_IC_JGR3_SERIES) {
		odm_set_mac_reg(dm, R_0x7cc, BIT(18) | BIT(17) | BIT(16), unit);
		odm_set_mac_reg(dm, R_0x7c0, 0x7f00, (time_unit_num & 0x7f));
	#endif
	} else {
		odm_set_mac_reg(dm, R_0x7cc, BIT(20) | BIT(19) | BIT(18), unit);
		odm_set_mac_reg(dm, R_0x7c0, 0x7f00, (time_unit_num & 0x7f));
	}
}

void phydm_la_set_buff_mode(void *dm_void, enum la_buff_mode mode)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	struct rtl8192cd_priv		*priv = dm->priv;
#endif
	u32 buff_size_base = 0;
	u32 end_pos_tmp = 0;
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	u8 normal_LA_on = priv->pmib->miscEntry.normal_LA_on;
#endif
	smp->la_buff_mode = mode;
#if 0
	if (dm->support_ic_type & ODM_RTL8814A)
		buf->start_pos = 0x30000;
	else if (dm->support_ic_type &
		 (ODM_RTL8822B | ODM_RTL8822C | ODM_RTL8812F))
		buf->start_pos = 0x20000;
	else if (dm->support_ic_type & ODM_RTL8814B)
		buf->start_pos = 0x30000;
	else if (dm->support_ic_type & (ODM_RTL8197F | ODM_RTL8198F))
		buf->start_pos = 0x00000;
	else if (dm->support_ic_type & ODM_RTL8192F)
		buf->start_pos = 0x2000;
	else if (dm->support_ic_type & ODM_RTL8821C)
		buf->start_pos = 0x8000;
	else if (dm->support_ic_type & ODM_RTL8195B)
		buf->start_pos = 0x4000;
#endif
	switch (dm->support_ic_type) {
	case ODM_RTL8814A:
		buff_size_base = 0x10000;
		end_pos_tmp = 0x40000;
		break;
	case ODM_RTL8822B:
	case ODM_RTL8822C:
	case ODM_RTL8812F:
		buff_size_base = 0x20000;
		end_pos_tmp = 0x40000;
		break;
	case ODM_RTL8814B:
		buff_size_base = 0x30000;
		end_pos_tmp = 0x60000;
		break;
	case ODM_RTL8197F:
	case ODM_RTL8198F:
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
		buff_size_base = 0x10000;
		end_pos_tmp = (normal_LA_on == 1) ? 0x20000 : 0x10000;
		break;
#endif
	case ODM_RTL8192F:
		buff_size_base = 0xE000;
		end_pos_tmp = 0x10000;
		break;
	case ODM_RTL8821C:
		buff_size_base = 0x8000;
		end_pos_tmp = 0x10000;
		break;
	case ODM_RTL8195B:
		buff_size_base = 0x4000;
		end_pos_tmp = 0x8000;
		break;
	default:
		pr_debug("[%s] Warning!", __func__);
		break;
	}

	buf->buffer_size = buff_size_base;

	if (dm->support_ic_type & FULL_BUFF_MODE_SUPPORT) {
		if (mode == ADCSMP_BUFF_HALF) {
			odm_set_mac_reg(dm, R_0x7cc, BIT(30), 0);
		} else {
			buf->buffer_size = buf->buffer_size << 1;
			odm_set_mac_reg(dm, R_0x7cc, BIT(30), 1);
		}
	}

	buf->end_pos = end_pos_tmp;
	buf->start_pos = end_pos_tmp - buf->buffer_size;

	PHYDM_DBG(dm, DBG_TMP,
		  "start_addr = ((0x%x)), end_addr = ((0x%x)), buffer_size = ((0x%x))\n",
		  buf->start_pos, buf->end_pos, buf->buffer_size);
}

void phydm_lamode_trigger_cmd(void *dm_void, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	u8 trig_mode = 0, dma_data_sig_sel = 0;
	u32 trig_sig_sel = 0;
	u8 enable_la_mode = 0;
	u32 trigger_time_mu_sec = 0;
	char help[] = "-h";
	u32 var1[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;

	if (!(dm->support_ic_type & PHYDM_IC_SUPPORT_LA_MODE))
		return;

	PHYDM_SSCANF(input[1], DCMD_DECIMAL, &var1[0]);
	enable_la_mode = (u8)var1[0];

	/*@dbg_print("echo cmd input_num = %d\n", input_num);*/

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{En} {0:BB,1:BB_MAC,2:RF0,3:RF1,4:MAC}\n{BB:dbg_port[bit],BB_MAC:0-ok/1-fail/2-cca,MAC:ref} {DMA type} {TrigTime}\n{DbgPort_head/ref_mask} {dbg_port} {0:P_Edge, 1:N_Edge} {SpRate:0-80M,1-40M,2-20M} {Capture num}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "set {1:tx_buff_size} {0: half, 1:all}\n");
		#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "set {2:adv_bb_trig(JGR3)} {en} {C0_dis} {C1_sel} {C1_val} {C2_sel} {C2_val}\n{C3_sel} {C3_val} {C4_en} {C4_val}\n");
		#endif
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "set {3:fake_trigger} {0: dis, 1:en}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "set {4:is_la_print} {0: dis, 1:en}\n");
	} else if ((strcmp(input[1], "set") == 0)) {
		PHYDM_SSCANF(input[2], DCMD_DECIMAL, &var1[1]);

		if (var1[1] == 1) {
			PHYDM_SSCANF(input[3], DCMD_DECIMAL, &var1[2]);
			phydm_la_set_buff_mode(dm, (enum la_buff_mode)var1[2]);
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "buff_mode = (%d/2)\n", smp->la_buff_mode + 1);
		} else if (var1[1] == 2) {
			#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
			phydm_la_bb_adv_cmd_jgr3(dm, input,
						 &used, output, &out_len);
			#else
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "Not Support\n");
			#endif
		} else if (var1[1] == 3) {
			PHYDM_SSCANF(input[3], DCMD_DECIMAL, &var1[2]);
			if (var1[2] == 1) {
				smp->en_fake_trig = true;
				PDM_SNPF(out_len, used, output + used,
					 out_len - used,
					 "Enable fake-trigger\n");
			} else {
				smp->en_fake_trig = false;
				PDM_SNPF(out_len, used, output + used,
					 out_len - used,
					 "Disable fake-trigger\n");
			}
		} else if (var1[1] == 4) {
			PHYDM_SSCANF(input[3], DCMD_DECIMAL, &var1[2]);
			smp->is_la_print = (boolean)var1[2];
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "is_la_print = %d\n", smp->is_la_print);
		}
	} else if (enable_la_mode == 1) {
		PHYDM_SSCANF(input[2], DCMD_DECIMAL, &var1[1]);

		trig_mode = (u8)var1[1];

		if (trig_mode == PHYDM_MAC_TRIG)
			PHYDM_SSCANF(input[3], DCMD_HEX, &var1[2]);
		else
			PHYDM_SSCANF(input[3], DCMD_DECIMAL, &var1[2]);
		trig_sig_sel = var1[2];

		PHYDM_SSCANF(input[4], DCMD_DECIMAL, &var1[3]);
		PHYDM_SSCANF(input[5], DCMD_DECIMAL, &var1[4]);
		PHYDM_SSCANF(input[6], DCMD_HEX, &var1[5]);
		PHYDM_SSCANF(input[7], DCMD_HEX, &var1[6]);
		PHYDM_SSCANF(input[8], DCMD_DECIMAL, &var1[7]);
		PHYDM_SSCANF(input[9], DCMD_DECIMAL, &var1[8]);
		PHYDM_SSCANF(input[10], DCMD_DECIMAL, &var1[9]);

		dma_data_sig_sel = (u8)var1[3];
		trigger_time_mu_sec = var1[4]; /*unit: us*/

		smp->la_mac_mask_or_hdr_sel = var1[5];
		smp->la_dbg_port = var1[6];
		smp->la_trigger_edge = (u8)var1[7];
		smp->la_smp_rate = (u8)(var1[8] & 0x7);
		smp->la_count = var1[9];

		pr_debug("echo lamode %d %d %d %d %d %d %x %d %d %d\n",
			 var1[0], var1[1], var1[2], var1[3], var1[4],
			 var1[5], var1[6], var1[7], var1[8], var1[9]);
		#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
		RT_TRACE_EX(COMP_LA_MODE, DBG_LOUD,
			    ("echo lamode %d %d %d %d %d %d %x %d %d %d\n",
			    var1[0], var1[1], var1[2], var1[3],
			    var1[4], var1[5], var1[6], var1[7],
			    var1[8], var1[9]));
		#endif

		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "a.En= ((1)),  b.mode = ((%d)), c.Trig_Sel = ((0x%x)), d.Dma_type = ((%d))\n",
			 trig_mode, trig_sig_sel, dma_data_sig_sel);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "e.Trig_Time = ((%dus)), f.Dbg_head/mac_ref_mask = ((0x%x)), g.dbg_port = ((0x%x))\n",
			 trigger_time_mu_sec,
			 smp->la_mac_mask_or_hdr_sel, smp->la_dbg_port);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "h.Trig_edge = ((%d)), i.smp rate = ((%d MHz)), j.Cap_num = ((%d))\n",
			 smp->la_trigger_edge, (80 >> smp->la_smp_rate),
			 smp->la_count);

		#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "k.en_new_bbtrigger = ((%d))\n",
			 smp->la_en_new_bbtrigger);
		#endif

		adc_smp_set(dm, trig_mode, trig_sig_sel,
			    dma_data_sig_sel, trigger_time_mu_sec, 0);
	} else {
		adc_smp_stop(dm);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Disable LA mode\n");
	}

	*_used = used;
	*_out_len = out_len;
}

void phydm_la_pre_run(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct rt_adcsmp *smp = &dm->adcsmp;
	struct rt_adcsmp_string *buf = &smp->adc_smp_buf;
	u8 while_cnt = 0;
	u8 tmp = 0;
	u8 target_polling_bit = BIT(1);

	if (dm->support_ic_type & ODM_RTL8198F) {
		/*pre run */
		/*force to bb trigger*/
		odm_set_mac_reg(dm, R_0x7c0, BIT(3), 0);
		/*dma_trig_and(AND1) output 1*/
		odm_set_bb_reg(dm, R_0x1ce4, 0xf0000000, 0x0);
		/*r_dma_trigger_AND1_inv = 1*/
		odm_set_bb_reg(dm, R_0x1ce8, BIT5, 1); /*@AND 1 val*/

		target_polling_bit = BIT(1);
		/* polling bit for BB ADC mode */
		odm_set_mac_reg(dm, 0x7c0, BIT(1), 1);

		pr_debug("buf_start(0x%x)buf_end(0x%x)\n",
			 buf->start_pos, buf->end_pos);

		do {
			tmp = odm_read_1byte(dm, R_0x7c0);
			if ((tmp & target_polling_bit) == false) {
				pr_debug("LA pre-run fail.\n");
				adc_smp_stop(dm);
				phydm_release_bb_dbg_port(dm);
			} else {
				ODM_delay_ms(100);
				pr_debug("LA pre-run while_cnt = %d.\n",
					 while_cnt);
				while_cnt = while_cnt + 1;
			}
		} while (while_cnt < 3);

		/*r_dma_trigger_AND1_inv = 0*/
		odm_set_bb_reg(dm, R_0x1ce8, BIT5, 0); /*@AND 1 val*/

		if (smp->la_trig_mode == PHYDM_ADC_MAC_TRIG) {
			if (dm->support_ic_type & ODM_RTL8192F)
				odm_set_mac_reg(dm, R_0x7f0, BIT(3), 1);
			else
				odm_set_mac_reg(dm, R_0x7c0, BIT(3), 1);
		}
	}
}

#endif /*@endif PHYDM_LA_MODE_SUPPORT*/
