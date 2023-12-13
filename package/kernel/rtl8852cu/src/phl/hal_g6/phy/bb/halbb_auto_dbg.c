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
#include "halbb_precomp.h"

#ifdef HALBB_AUTO_DBG_SUPPORT

#define HALBB_CHK_HANG_APIS

#ifdef HALBB_CHK_HANG_APIS
void halbb_auto_chk_hang_reset(struct bb_info *bb)
{
	struct bb_auto_dbg_info *a_dbg = &bb->bb_auto_dbg_i;
	struct bb_chk_hang_info *chk_hang = &a_dbg->bb_chk_hang_i;
	
	halbb_mem_set(bb, chk_hang->dbg_port_val, 0, chk_hang->table_size);
}

void halbb_auto_chk_hang(struct bb_info *bb)
{
	struct bb_auto_dbg_info *a_dbg = &bb->bb_auto_dbg_i;
	struct bb_chk_hang_info *chk_hang = &a_dbg->bb_chk_hang_i;
	u32 dbg_port = 0;
	u32 dbg_port_value = 0;
	u32 i = 0;

	BB_DBG(bb, DBG_AUTO_DBG, "[%s]\n", __func__);

	/*=== Get check hang Information ===============================*/
	/*Get packet counter Report*/

	/*Get BB Register*/

	/*Get RF Register*/

	/*Get Debug Port*/
	for (i = 0; i < chk_hang->table_size; i++) {
		dbg_port = chk_hang->dbg_port_table[i];
		if (halbb_bb_dbg_port_racing(bb, DBGPORT_PRI_3)) {
			halbb_set_bb_dbg_port_ip(bb, (dbg_port & 0xff0000) >> 16);
			halbb_set_bb_dbg_port(bb, dbg_port & 0xffff);
			dbg_port_value = halbb_get_bb_dbg_port_val(bb);
			halbb_release_bb_dbg_port(bb);

			BB_DBG(bb, DBG_AUTO_DBG, "dbg_port[0x%x]=(0x%x)\n",
			       dbg_port, dbg_port_value);
		}  else {
			BB_DBG(bb, DBG_AUTO_DBG, "Dbg_port Racing Fail!\n");
			return;
		}
	}

	/*=== Make check hang decision ===============================*/
	BB_DBG(bb, DBG_AUTO_DBG, "Check Hang Decision\n");


	halbb_auto_chk_hang_reset(bb);
}

void halbb_auto_chk_hang_init(struct bb_info *bb)
{
	struct bb_auto_dbg_info *a_dbg = &bb->bb_auto_dbg_i;
	struct bb_chk_hang_info *chk_hang = &a_dbg->bb_chk_hang_i;
	u32 dbg_port_table[] = {0x0, 0x803, 0x208, 0xab0};
	u32 table_size = sizeof(dbg_port_table);

	chk_hang->table_size = table_size;
	chk_hang->dbg_port_table = halbb_mem_alloc(bb, table_size);
	halbb_mem_cpy(bb, chk_hang->dbg_port_table, dbg_port_table, table_size);
	
	chk_hang->dbg_port_val= halbb_mem_alloc(bb, table_size);
	a_dbg->auto_dbg_type |= AUTO_DBG_CHECK_HANG;
}

#endif

void halbb_auto_debug(struct bb_info *bb)
{
	struct bb_auto_dbg_info *a_dbg = &bb->bb_auto_dbg_i;

	if (!(bb->support_ability & BB_AUTO_DBG))
		return;

	/*check hang*/
	if (a_dbg->auto_dbg_type & AUTO_DBG_CHECK_HANG)
		halbb_auto_chk_hang(bb);
	
	/*check RX Part*/
	/*check TX Part*/
}

void halbb_auto_debug_init(struct bb_info *bb)
{
	//struct bb_auto_dbg_info *a_dbg = &bb->bb_auto_dbg_i;

	/*check hang*/
	halbb_auto_chk_hang_init(bb);
	/*check RX Part*/
	/*check TX Part*/
}
#endif