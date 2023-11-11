/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#define _HAL_BTC_DBG_CMD_C_
#include "../hal_headers_le.h"
#include "hal_btc.h"
#include "halbtc_fw.h"
#include "halbtc_fwdef.h"
#include "halbtc_action.h"
#include "halbtc_def.h"

#ifdef CONFIG_BTCOEX

#define	BTC_CNSL(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {							\
		u32 *used_len_tmp = &(used_len);			\
		if (*used_len_tmp < max_buff_len)			\
			*used_len_tmp += _os_snprintf(buff_addr, remain_len,\
						      fmt, ##__VA_ARGS__);\
	} while (0)


struct halbtc_cmd_info {
	char name[16];
	u8 id;
};

enum HALBTC_CMD_ID {
	HALBTC_HELP,
	HALBTC_SHOW,
	HALBTC_READ_BT,
	HALBTC_WRITE_BT,
	HALBTC_SET_COEX,
	HALBTC_UPDATE_POLICY,
	HALBTC_TDMA,
	HALBTC_SLOT,
	HALBTC_SIG_GDBG_EN,
	HALBTC_SGPIO_MAP,
	HALBTC_TRACE_STEP,
	HALBTC_WL_TX_POWER,
	HALBTC_WL_RX_LNA,
	HALBTC_BT_AFH_MAP,
	HALBTC_BT_TX_POWER,
	HALBTC_BT_RX_LNA,
	HALBTC_BT_IGNO_WLAN,
	HALBTC_SET_GNT_WL,
	HALBTC_SET_GNT_BT,
	HALBTC_SET_BT_PSD,
	HALBTC_GET_WL_NHM_DBM,
	HALBTC_DBG,
#ifdef BTC_AISO_SUPPORT
	HALBTC_AISO,
#endif

#ifdef BTC_FDDT_TRAIN_SUPPORT
	HALBTC_FT_SET,
	HALBTC_FT_SHOW,
	HALBTC_FT_CTRL
#endif
};

enum {
	BTC_FDDT_CELL_RANGE_UL = 0x0,
	BTC_FDDT_CELL_RANGE_DL = 0x1,
	BTC_FDDT_CELL_STATE_UL = 0x80,
	BTC_FDDT_CELL_STATE_DL = 0x81
};

struct halbtc_cmd_info halbtc_cmd_i[] = {
	{"-h", HALBTC_HELP},
	{"show", HALBTC_SHOW},
	{"rb", HALBTC_READ_BT},
	{"wb", HALBTC_WRITE_BT},
	{"mode", HALBTC_SET_COEX},
	{"update", HALBTC_UPDATE_POLICY},
	{"tdma", HALBTC_TDMA},
	{"slot", HALBTC_SLOT},
	{"sig", HALBTC_SIG_GDBG_EN},
	{"gpio", HALBTC_SGPIO_MAP},
	{"wpwr", HALBTC_WL_TX_POWER},
	{"wlna", HALBTC_WL_RX_LNA},
	{"afh", HALBTC_BT_AFH_MAP},
	{"bpwr", HALBTC_BT_TX_POWER},
	{"blna", HALBTC_BT_RX_LNA},
	{"igwl", HALBTC_BT_IGNO_WLAN},
	{"gwl", HALBTC_SET_GNT_WL},
	{"gbt", HALBTC_SET_GNT_BT},
	{"bpsd", HALBTC_SET_BT_PSD},
	{"wnhm", HALBTC_GET_WL_NHM_DBM},
	{"dbg", HALBTC_DBG},
#ifdef BTC_AISO_SUPPORT
	{"aiso", HALBTC_AISO},
#endif

#ifdef BTC_FDDT_TRAIN_SUPPORT
	{"ftset", HALBTC_FT_SET},
	{"ftshow", HALBTC_FT_SHOW},
	{"ftctrl", HALBTC_FT_CTRL},
#endif
};

#define _limit_val(val, max, min) \
	val = (val > max ? max: (val < min ? min : val))

static void _cmd_rb(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		    u32 input_num, char *output, u32 out_len)
{
	u32 i = 0, type = 0, addr = 0;

	if (input_num >= 3) {
		_os_sscanf(input[1], "%d", &type);
		_os_sscanf(input[2], "%x", &addr);
	}

	if (type > 4 || input_num < 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " rb <type --- 0:rf, 1:modem, 2:bluewize, 3:vendor,"
			 " 4:LE> <addr:16bits> \n");
		return;
	}

	btc->dbg.rb_done = false;
	btc->dbg.rb_val = 0xffffffff;
	_read_bt_reg(btc, (u8)(type), (u16)addr);
	for (i = 0; i < 50; i++) {
		if (!btc->dbg.rb_done)
			hal_mdelay(btc->hal, 10);
		else
			break;
	}

	if (!btc->dbg.rb_done) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " timeout !! \n");
	} else {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " rb %d(0x%x), val = 0x%x !! \n",
			 type, addr, btc->dbg.rb_val);
	}
}

static void _show_cx_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		          u32 input_num, char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_bt_info *bt = &btc->cx.bt;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct rtw_phl_com_t *p = btc->phl;
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	u32 ver_main = 0, ver_sub = 0, ver_hotfix = 0, id_branch = 0;
	struct btc_module *md = &btc->mdinfo;

	if (!(dm->coex_info_map & BTC_COEX_INFO_CX))
		return;

	dm->cnt_notify[BTC_NCNT_SHOW_COEX_INFO]++;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r========== [BTC COEX INFO (%s)] ==========",
		 id_to_str(BTC_STR_CHIPID, (u32)btc->chip->chip_id));

	ver_main = (coex_ver & bMASKB3) >> 24;
	ver_sub = (coex_ver & bMASKB2) >> 16;
	ver_hotfix = (coex_ver & bMASKB1) >> 8;
	id_branch = coex_ver & bMASKB0;
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : Coex:%d.%d.%d(branch:%d), ",
		 "[coex_version]", ver_main, ver_sub, ver_hotfix, id_branch);

	ver_main = (wl->ver_info.fw_coex & bMASKB3) >> 24;
	ver_sub = (wl->ver_info.fw_coex & bMASKB2) >> 16;
	ver_hotfix = (wl->ver_info.fw_coex & bMASKB1) >> 8;
	id_branch = wl->ver_info.fw_coex & bMASKB0;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "WL_FW_coex:%d.%d.%d(branch:%d)",
		 ver_main, ver_sub, ver_hotfix, id_branch);

	ver_main = (btc->chip->wlcx_desired & bMASKB3) >> 24;
	ver_sub = (btc->chip->wlcx_desired & bMASKB2) >> 16;
	ver_hotfix = (btc->chip->wlcx_desired & bMASKB1) >> 8;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "(%s, desired:%d.%d.%d), ",
		 (wl->ver_info.fw_coex >= btc->chip->wlcx_desired ?
		 "Match" : "Mis-Match"),
		 ver_main, ver_sub, ver_hotfix);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "BT_FW_coex:%d(%s, desired:%d)",
		 bt->ver_info.fw_coex,
		 (bt->ver_info.fw_coex >= btc->chip->btcx_desired ?
		 "Match" : "Mis-Match"), btc->chip->btcx_desired);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : WL_FW:%d.%d.%d.%d, BT_FW:0x%x(%s)",
		 "[sub_module]", (wl->ver_info.fw & bMASKB3) >> 24,
		 (wl->ver_info.fw & bMASKB2) >> 16,
		 (wl->ver_info.fw & bMASKB1) >> 8,
		 (wl->ver_info.fw & bMASKB0), bt->ver_info.fw,
		 (bt->run_patch_code ? "patch" : "ROM"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : kt_ver:%x, rfe_type:0x%x%s, hw_id:0x%x,"
		 " ant_pg:%d, %s",
		 "[hw_info]", btc->mdinfo.kt_ver, btc->mdinfo.rfe_type,
		 (btc->chip->para_ver != 0x1b ?
		 "" : (btc->mdinfo.rfe_type % 3 == 2 ? "(div-on)" : "(div-off)")),
		 ((btc->phl->id.id & 0xff00) >> 8),
		 btc->mdinfo.ant.num,
		 (btc->mdinfo.ant.num > 1? "" : (btc->mdinfo.ant.single_pos?
		 "1Ant_Pos:S1, " : "1Ant_Pos:S0, ")));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "3rd_coex:%d, dbcc:%d, tx[num:%d/ss:%d%s],"
		 " rx[num:%d/ss:%d%s],",
		 btc->cx.other.type, wl->role_info.dbcc_en,
		 p->phy_cap[0].tx_path_num, p->phy_cap[0].txss,
		 (btc->chip->para_ver != 0x1b ? "" :
		 (btc->mdinfo.rfe_type % 3 == 1 ? "" :
		 (hal_read32(h, 0x1586c) & 0xf000) == 0x1000 ?
		 "/ant:aux" : "/ant:main")),
		 p->phy_cap[0].rx_path_num, p->phy_cap[0].rxss,
		 (btc->chip->para_ver != 0x1b ? "" :
		 (btc->mdinfo.rfe_type % 3 == 1 ? "" :
		 (hal_read32(h, 0x1586c) & 0xf0) == 0x10 ?
		 "/ant:aux" : "/ant:main")));

	if (bt_psd_dm->aiso_db_cnt > 0)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " ant_iso:%ddB(cal)", md->ant.isolation);
	else
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " ant_iso:%ddB(def)", md->ant.isolation);
}

static void _show_wl_role_info(struct btc_t *btc, u32 *used,
			       char input[][MAX_ARGV], u32 input_num,
			       char *output, u32 out_len)
{
	struct btc_wl_link_info *plink = NULL;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	struct btc_wl_dbcc_info *wl_dinfo = &wl->dbcc_info;
	struct btc_traffic t;
	u8 i;

	if (wl_rinfo->dbcc_en) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : 2G_PHY:%s, chg_cnt:%d, all_2G_cnt:%d",
			 "[dbcc_info]",
			 (wl_rinfo->dbcc_2g_phy == HW_PHY_MAX ? "None" :
			 (wl_rinfo->dbcc_2g_phy == HW_PHY_0 ? "PHY0" : "PHY1")),
			  btc->cx.cnt_wl[BTC_WCNT_DBCC_CHG],
			  btc->cx.cnt_wl[BTC_WCNT_DBCC_ALL_2G]);

		for (i = HW_PHY_0; i < HW_PHY_MAX; i++) {
			BTC_CNSL(out_len, *used, output + *used, out_len- *used,
			 	 ", PHY-%d(op:%s/scan:%s/real:%s/role:0x%x)",
			 	 i,
				 id_to_str(BTC_STR_BAND,
					   (u32)wl_dinfo->op_band[i]),
				 id_to_str(BTC_STR_BAND,
					   (u32)wl_dinfo->scan_band[i]),
				 id_to_str(BTC_STR_BAND,
					   (u32)wl_dinfo->real_band[i]),
				 wl_dinfo->role[i]);
		}
	}

	for (i = 0; i < BTC_WL_MAX_ROLE_NUMBER; i++) {
		plink = &btc->cx.wl.link_info[i];

		if (!plink->active)
			continue;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r [band%d-port%d]   : role=%s(rid=%d), mstate=%s",
			 plink->phy, plink->pid,
			 id_to_str(BTC_STR_ROLE, (u32)plink->role), i,
			 id_to_str(BTC_STR_MSTATE, (u32)plink->connected));

		if (plink->connected == MLME_NO_LINK)
			continue;

		if (plink->role == PHL_RTYPE_AP ||
		    plink->role == PHL_RTYPE_P2P_GO)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "(client=%d)", plink->client_cnt-1);
		else if (plink->role == PHL_RTYPE_STATION)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "(bssid=0x%02x%02x%02x%02x%02x%02x)",
				 plink->mac_addr[5], plink->mac_addr[4],
				 plink->mac_addr[3], plink->mac_addr[2],
				 plink->mac_addr[1], plink->mac_addr[0]);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", mode=%s, ch(ct:%d/pr:%d), bw=%s, mac_id=%d,"
			 " noa(%d)=%d.%03dms",
			 id_to_str(BTC_STR_WLMODE, (u32)plink->mode),
			 plink->chdef.center_ch, plink->chdef.chan,
			 id_to_str(BTC_STR_WLBW, (u32)plink->chdef.bw),
			 plink->mac_id, plink->noa,
			 plink->noa_duration/1000, plink->noa_duration%1000);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r [port_%d]        :"
			 " rssi=-%ddBm(%d), busy=%d, dir=%s, ",
			 plink->pid, 110-plink->stat.rssi,
			 plink->stat.rssi, plink->busy,
			 (plink->dir == TRAFFIC_UL ? "UL" : "DL"));

		t = plink->stat.traffic;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "tx[rate:%s/busy_level:%d/sts:0x%x/1ss:%s/tp:%d], ",
			 id_to_str(BTC_STR_RATE, (u32)t.tx_rate),
			 t.tx_lvl, t.tx_sts, (t.tx_1ss_limit ? "Y" : "N"),
			 t.tx_tp);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "rx[rate:%s/busy_level:%d/sts:0x%x/drop:%d/tp:%d]",
			 id_to_str(BTC_STR_RATE, (u32)t.rx_rate),
			 t.rx_lvl, t.rx_sts, plink->rx_rate_drop_cnt,
			 t.rx_tp);
	}
}

static void _show_wl_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			  u32 input_num, char *output, u32 out_len)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;

	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_WL))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r========== [WL Status] ==========");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : link_mode:%s, mrole:%s, ",
		 "[status]",
		 id_to_str(BTC_STR_WLLINK, (u32)wl_rinfo->link_mode),
		 id_to_str(BTC_STR_MROLE, wl_rinfo->mrole_type));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "rf_off:%s, power_save:%d,"
		 " init_ok:%s, nhm:%d(status=%d, ratio:%d), cn:%d",
		 (wl->status.map.rf_off? "Y" : "N"), wl->status.map.lps,
		 (wl->status.map.init_ok? "Y" : "N"), wl->nhm.pwr,
		  wl->nhm.current_status, wl->nhm.ratio, wl->cn_report);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : scan:%s(band:%s/phy_map:0x%x),"
		 " connecting:%s, roam:%s, 4way:%s, cx_state:%s", "[scan]",
		 (wl->status.map.scan? "Y" : "N"),
		 id_to_str(BTC_STR_BAND, (u32)wl->scan_info.band[HW_PHY_0]),
		 wl->scan_info.phy_map,
		 (wl->status.map.connecting? "Y" : "N"),
		 (wl->status.map.roaming?  "Y" : "N"),
		 (wl->status.map._4way? "Y" : "N"),
		 id_to_str(BTC_STR_CXSTATE, cx->state_map));

	_show_wl_role_info(btc, used, input, input_num, output, out_len);
}

static void _show_dm_step(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			  u32 input_num, char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	u32 n_begin = 0, n_end = 0, i = 0, cnt = 0;
	u32 step_cnt = dm->dm_step.cnt;

	if (step_cnt == 0)
		return;

	if (step_cnt <= BTC_DM_MAXSTEP)
		n_begin = 1;
	else
		n_begin = step_cnt - BTC_DM_MAXSTEP + 1;

	n_end = step_cnt;

	if (n_begin > n_end)
		return;

	for (i = n_begin; i <= n_end; i++) {
		if (cnt % 6 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len- *used,
				 "\n\r %-15s : ", "[dm_steps]");

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "-> %s", dm->dm_step.step[(i-1) % BTC_DM_MAXSTEP]);
		cnt++;
	}
}

static void _show_bt_profile_info(struct btc_t *btc, u32 *used,
				  char input[][MAX_ARGV], u32 input_num,
				  char *output, u32 out_len)
{
	struct btc_bt_link_info *bt_linfo = &btc->cx.bt.link_info;
	struct btc_bt_hfp_desc hfp = bt_linfo->hfp_desc;
	struct btc_bt_hid_desc hid = bt_linfo->hid_desc;
	struct btc_bt_a2dp_desc a2dp = bt_linfo->a2dp_desc;
	struct btc_bt_pan_desc pan = bt_linfo->pan_desc;

	if (hfp.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : type:%s, sut_pwr:%d, golden-rx:%d",
			 "[HFP]",
			 (hfp.type == 0? "SCO" : "eSCO"),
			 bt_linfo->sut_pwr_level[0],
			 bt_linfo->golden_rx_shift[0]);
	}

	if (hid.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : type:%s%s%s%s%s pair-cnt:%d, sut_pwr:%d,"
			 " golden-rx:%d", "[HID]",
			 (hid.type == BTC_HID_218? "2/18," : ""),
			 (hid.type & BTC_HID_418? "4/18," : ""),
			 (hid.type & BTC_HID_BLE? "BLE," : ""),
			 (hid.type & BTC_HID_RCU? "RCU," : ""),
			 (hid.type & BTC_HID_RCU_VOICE? "RCU-Voice," : ""),
			  hid.pair_cnt, bt_linfo->sut_pwr_level[1],
			  bt_linfo->golden_rx_shift[1]);
	}

	if (a2dp.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : type:%s, bit-pool:%d, flush-time:%d, ",
			 "[A2DP]",
			 (a2dp.type == BTC_A2DP_LEGACY ? "Legacy" : "TWS"),
			  a2dp.bitpool, a2dp.flush_time);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "vid:0x%x, Dev-name:0x%x, sut_pwr:%d, golden-rx:%d",
			 a2dp.vendor_id, a2dp.device_name,
			 bt_linfo->sut_pwr_level[2],
			 bt_linfo->golden_rx_shift[2]);
	}

	if (pan.exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : sut_pwr:%d, golden-rx:%d",
			 "[PAN]",
			 bt_linfo->sut_pwr_level[3],
			 bt_linfo->golden_rx_shift[3]);
	}
}

static void _show_bt_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			  u32 input_num, char *output, u32 out_len)
{
	struct btc_cx *cx = &btc->cx;
	struct btc_bt_info *bt = &cx->bt;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_module *module = &btc->mdinfo;
	struct btc_bt_link_info *bt_linfo = &bt->link_info;
	u8 *afh = bt_linfo->afh_map;
	u8 *afh_le = bt_linfo->afh_map_le;

	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_BT))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "%s", "\n\r========== [BT Status] ==========");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : enable:%s, btg:%s%s, connect:%s, ",
		 "[status]", (bt->enable.now? "Y" : "N"),
		 (bt->btg_type? "Y" : "N"),
		 (bt->enable.now && (bt->btg_type != module->bt_pos)?
		  "(efuse-mismatch!!)" : ""),
		  (bt_linfo->status.map.connect? "Y" : "N"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "rssi:%ddBm(rssi_lvl:%d), tx_rate:%dM, ",
		 bt_linfo->rssi-100, bt->rssi_level,
		 (bt_linfo->tx_3M? 3 : 2));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "igno_wl:%s, mailbox_avl:%s, rfk_state:0x%x",
		 (bt->igno_wl? "Y" : "N"),
		 (bt->mbx_avl? "Y" : "N"), bt->rfk_info.val);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : profile:%s%s%s%s%s ",
		 "[profile]",
		 ((bt_linfo->profile_cnt.now == 0) ? "None," : ""),
		  (bt_linfo->hfp_desc.exist? "HFP," : ""),
		  (bt_linfo->hid_desc.exist? "HID," : ""),
		  (bt_linfo->a2dp_desc.exist?
		  (bt_linfo->a2dp_desc.sink ? "A2DP_sink," :"A2DP,") : ""),
		  (bt_linfo->pan_desc.exist? "PAN," : ""));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "multi-link:%s, role:%s, ble-connect:%s, CQDDR:%s,"
		 " A2DP_active:%s, PAN_active:%s",
		 (bt_linfo->multi_link.now? "Y" : "N"),
		 (bt_linfo->slave_role ? "Slave" : "Master"),
		 (bt_linfo->status.map.ble_connect? "Y" : "N"),
		 (bt_linfo->cqddr? "Y" : "N"),
		 (bt_linfo->a2dp_desc.active? "Y" : "N"),
		 (bt_linfo->pan_desc.active? "Y" : "N"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : wl_ch_map[en:%d/ch:%d/bw:%d], ",
		 "[AFH]",
		 wl->afh_info.en, wl->afh_info.ch, wl->afh_info.bw);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "Legacy[%02x%02x_%02x%02x_%02x%02x_%02x%02x_%02x%02x], ",
		 afh[0], afh[1], afh[2], afh[3], afh[4],
		 afh[5], afh[6], afh[7], afh[8], afh[9]);

	if (bt_linfo->status.map.ble_connect)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "LE[%02x%02x_%02x_%02x%02x]",
			 afh_le[0], afh_le[1], afh_le[2], afh_le[3], afh_le[4]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : retry:%d, relink:%d, rate_chg:%d,"
		 " reinit:%d, reenable:%d, ", "[stat_cnt]",
		 cx->cnt_bt[BTC_BCNT_RETRY],
		 cx->cnt_bt[BTC_BCNT_RELINK],
		 cx->cnt_bt[BTC_BCNT_RATECHG],
		 cx->cnt_bt[BTC_BCNT_REINIT],
		 cx->cnt_bt[BTC_BCNT_REENABLE]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "role-sw:%d, afh:%d, inq_page:%d(inq:%d/page:%d), igno_wl:%d",
		 cx->cnt_bt[BTC_BCNT_ROLESW],
		 cx->cnt_bt[BTC_BCNT_AFH],
		 cx->cnt_bt[BTC_BCNT_INQPAG],
		 cx->cnt_bt[BTC_BCNT_INQ],
		 cx->cnt_bt[BTC_BCNT_PAGE],
		 cx->cnt_bt[BTC_BCNT_IGNOWL]);

	_show_bt_profile_info(btc, used, input, input_num, output, out_len);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : raw_data[%02x %02x %02x %02x %02x %02x]"
		 " (type:%s/cnt:%d/same:%d)", "[bt_info]",
		 bt->raw_info[2], bt->raw_info[3],
		 bt->raw_info[4], bt->raw_info[5],
		 bt->raw_info[6], bt->raw_info[7],
		 (bt->raw_info[0] == BTC_BTINFO_AUTO ? "auto" : "reply"),
		 cx->cnt_bt[BTC_BCNT_INFOUPDATE],
		 cx->cnt_bt[BTC_BCNT_INFOSAME]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : Hi-rx = %d, Hi-tx = %d, Lo-rx = %d, Lo-tx = %d"
		 "(bt_polut_wl_tx:%d/bt_scan_Lo-rx_en:%d)",
		 "[trx_req_cnt]",
		 cx->cnt_bt[BTC_BCNT_HIPRI_RX],
		 cx->cnt_bt[BTC_BCNT_HIPRI_TX],
		 cx->cnt_bt[BTC_BCNT_LOPRI_RX],
		 cx->cnt_bt[BTC_BCNT_LOPRI_TX],
		 cx->cnt_bt[BTC_BCNT_POLUT],
		 bt->scan_rx_low_pri);
}

static void _show_fbtc_tdma(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_tdma *t = NULL;

	pcinfo = &pfwinfo->rpt_fbtc_tdma.cinfo;
	if (!pcinfo->valid)
		return;

	t = &pfwinfo->rpt_fbtc_tdma.finfo.tdma;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : policy:%s[type:%s/rx_flow:%d/tx_flow:%d/",
		 "[tdma_policy]",
		 id_to_str(BTC_STR_POLICY, (u32)btc->policy_type),
		 id_to_str(BTC_STR_TDMA, (u32)t->type),
		 t->rxflctrl, t->txflctrl);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "rsvd:%d/leak_n:%d,/ext_ctrl:%d/null_role:0x%x/opt:0x%x]",
		 t->rsvd, t->leak_n, t->ext_ctrl,
		 t->rxflctrl_role, t->option_ctrl);
}

static void _show_dm_info(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			  u32 input_num, char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_bt_info *bt = &btc->cx.bt;
	struct btc_bt_psd_dm *bp = &btc->bt_psd_dm;
	struct btc_aiso_val *av = &bp->aiso_val;
	struct btc_ant_info *ant = &btc->mdinfo.ant;
	u8 cnt, aiso_cur, aiso_ori = ant->isolation;

	if (!(dm->coex_info_map & BTC_COEX_INFO_DM))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r========== [Mechanism Status %s] ==========",
		 (btc->ctrl.manual? "(Manual)":"(Auto)"));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : type:%s, reason:%s(), action:%s(),"
		 " ant_path:%s, run_cnt:%d", "[status]",
		 (ant->type == BTC_ANT_SHARED ? "shared" : "non-shared"),
		 dm->run_reason, dm->run_action,
		 id_to_str(BTC_STR_ANTPATH, dm->set_ant_path & 0xff),
		 dm->cnt_dm[BTC_DCNT_RUN]);

	_show_fbtc_tdma(btc, used, input, input_num, output, out_len);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : wl_only:%d, bt_only:%d, igno_bt:%d,"
		 " free_run:%d, wl_ps_ctrl:%d, ", "[dm_flag]",
		 dm->wl_only, dm->bt_only, btc->ctrl.igno_bt,
		 dm->freerun, btc->hal->btc_ctrl.lps);

#ifdef BTC_FDDT_TRAIN_SUPPORT
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "leak_ap:%d, fddt_train:%d, error_map:0x%x",
		 dm->leak_ap, dm->fddt_train, dm->error.val);
#else
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "leak_ap:%d, fddt_train:No-Support, error_map:0x%x",
		 dm->leak_ap, dm->error.val);
#endif

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : wl_tx_limit[en:%d/max_t:%dus/max_retry:%d],"
		 " bt_slot_req:[%d/%d], bt_stbc_req:%d",
		 "[dm_drv_ctrl]", dm->wl_tx_limit.en,
		 dm->wl_tx_limit.tx_time, dm->wl_tx_limit.tx_retry,
		 btc->bt_req_len[HW_PHY_0], btc->bt_req_len[HW_PHY_1],
		 btc->bt_req_stbc);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : wl[rssi_lvl:%d/para_lvl:%d/tx_pwr:%d/rx_lvl:%d/"
		 "btg_rx:%d/stb_chg:%d] ", "[dm_rf_ctrl]",
		 wl->rssi_level, dm->trx_para_level,
		 dm->rf_trx_para.wl_tx_power, dm->rf_trx_para.wl_rx_gain,
		 dm->wl_btg_rx, dm->wl_stb_chg);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "bt[tx_pwr_dec:%d/rx_lna:%d(%s-tbl)]",
		 dm->rf_trx_para.bt_tx_power, dm->rf_trx_para.bt_rx_gain,
		 (bt->hi_lna_rx? "Hi" : "Ori"));

	if (bp->en || bp->aiso_data_ok || bp->rec_time_out || bp->aiso_db_cnt) {
		cnt = bp->aiso_db_cnt;
		aiso_cur = (cnt > 0 ? bp->aiso_db[(cnt-1) & 0xf] : aiso_ori);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : cur_iso:%d dB(cnt:%d), en:%d, run:%d,"
			 " timeout:%d, data_ok:%d, psd_cnt:%d, exec_cmd_cnt:%d",
			 "[dm_ant_iso]", aiso_cur, cnt, bp->en, bp->rec_start,
			 bp->rec_time_out, bp->aiso_data_ok, av->psd_rec_cnt,
			 bp->aiso_cmd_cnt);
	}
	_show_dm_step(btc, used, input, input_num, output, out_len);
}

static void _show_mreg(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		       u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_mreg_val *pmreg = NULL;
	struct fbtc_gpio_dbg *gdbg = NULL;
	struct btc_cx *cx = &btc->cx;
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_bt_info *bt = &btc->cx.bt;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	struct btc_gnt_ctrl *gnt = NULL;
	u8 i = 0, type = 0, cnt = 0;
	u32 val, offset;

	if (!(dm->coex_info_map & BTC_COEX_INFO_MREG))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r========== [HW Status] ==========");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : "
		 "WL->BT:0x%08x(cnt:%d), BT->WL:0x%08x(total:%d, bt_update:%d)",
		 "[scoreboard]", wl->scbd, cx->cnt_wl[BTC_WCNT_SCBDUPDATE],
		 bt->scbd, cx->cnt_bt[BTC_BCNT_SCBDREAD],
		 cx->cnt_bt[BTC_BCNT_SCBDUPDATE]);

	/* To avoid I/O if WL LPS or power-off  */
	dm->pta_owner = _read_cx_ctrl(btc);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : pta_owner:%s, pta_req_mac:MAC%d",
		 "[gnt_status]",
		 (btc->chip->para_ver == 0x1? "HW" :
		 (dm->pta_owner == BTC_CTRL_BY_WL? "WL" : "BT")),
		  wl->pta_req_mac);

	gnt = &dm->gnt_val[HW_PHY_0];

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " ,phy-0[gnt_wl:%s-%d/gnt_bt:%s-%d/polut_type:%s]",
		 (gnt->gnt_wl_sw_en? "SW" : "HW"), gnt->gnt_wl,
		 (gnt->gnt_bt_sw_en? "SW" : "HW"), gnt->gnt_bt,
		 id_to_str(BTC_STR_POLUT, wl->bt_polut_type[HW_PHY_0]));

	if (wl_rinfo->dbcc_en) {
		gnt = &dm->gnt_val[HW_PHY_1];
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " ,phy-1[gnt_wl:%s-%d/gnt_bt:%s-%d/polut_type:%s]",
			(gnt->gnt_wl_sw_en? "SW" : "HW"),
			 gnt->gnt_wl,
			(gnt->gnt_bt_sw_en? "SW" : "HW"),
			 gnt->gnt_bt,
			 id_to_str(BTC_STR_POLUT, wl->bt_polut_type[HW_PHY_1]));
	}

	pcinfo = &pfwinfo->rpt_fbtc_mregval.cinfo;
	if (!pcinfo->valid)
		return;

	pmreg = &pfwinfo->rpt_fbtc_mregval.finfo;

	for (i = 0; i < pmreg->reg_num; i++) {
		type = (u8)btc->chip->mon_reg[i].type;
		offset = btc->chip->mon_reg[i].offset;
		val = pmreg->mreg_val[i];

		if (cnt % 6 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len-*used,
				 "\n\r %-15s : %s_0x%x=0x%x",
				 "[reg]",
				 id_to_str(BTC_STR_REG, (u32)type),
				 offset, val);
	 	else
			BTC_CNSL(out_len, *used, output + *used, out_len-*used,
				 ", %s_0x%x=0x%x",
				 id_to_str(BTC_STR_REG, (u32)type),
				 offset, val);
		cnt++;
	}

	pcinfo = &pfwinfo->rpt_fbtc_gpio_dbg.cinfo;
	if (!pcinfo->valid)
		return;

	gdbg = &pfwinfo->rpt_fbtc_gpio_dbg.finfo;
	if (!gdbg->en_map)
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : enable_map:0x%08x", "[gpio_dbg]", gdbg->en_map);

	for (i = 0; i < BTC_DBG_MAX1; i++) {
		if (!(gdbg->en_map & BIT(i)))
			continue;
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", %s->GPIO%d", id_to_str(BTC_STR_GDBG, (u32)i),
			 gdbg->gpio_map[i]);
	}
}

static void _show_summary(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			  u32 input_num, char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_rpt_ctrl *prptctrl = NULL;
	struct btc_cx *cx = &btc->cx;
	struct btc_dm *dm = &btc->dm;
	struct btc_wl_info *wl = &cx->wl;
	u32 cnt_sum = 0;
	u32 *cnt = btc->dm.cnt_notify;
	u8 i;

	if (!(dm->coex_info_map & BTC_COEX_INFO_SUMMARY))
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "%s", "\n\r========== [Statistics] ==========");

	pcinfo = &pfwinfo->rpt_ctrl.cinfo;
	if (pcinfo->valid && wl->status.map.lps != BTC_LPS_RF_OFF &&
	    !wl->status.map.rf_off) {
		prptctrl = &pfwinfo->rpt_ctrl.finfo;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : h2c_cnt=%d(fail:%d, fw_recv:%d),"
			 " c2h_cnt=%d(fw_send:%d, len:%d, max:%d), ",
			 "[summary]", pfwinfo->cnt_h2c, pfwinfo->cnt_h2c_fail,
			 prptctrl->rpt_info.cnt_h2c, pfwinfo->cnt_c2h,
			 prptctrl->rpt_info.cnt_c2h, prptctrl->rpt_info.len_c2h,
			 RTW_PHL_BTC_FWINFO_BUF);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "rpt_cnt=%d(fw_send:%d), rpt_map=0x%x",
			 pfwinfo->event[BTF_EVNT_RPT], prptctrl->rpt_info.cnt,
			 prptctrl->rpt_info.en);

		if (dm->error.map.wl_fw_hang)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " (WL FW Hang!!)");

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : send_ok:%d, send_fail:%d, recv:%d, ",
			 "[mailbox]", prptctrl->bt_mbx_info.cnt_send_ok,
			 prptctrl->bt_mbx_info.cnt_send_fail,
			 prptctrl->bt_mbx_info.cnt_recv);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "A2DP_empty:%d(stop:%d/tx:%d/ack:%d/nack:%d)",
			 prptctrl->bt_mbx_info.a2dp.cnt_empty,
			 prptctrl->bt_mbx_info.a2dp.cnt_flowctrl,
			 prptctrl->bt_mbx_info.a2dp.cnt_tx,
			 prptctrl->bt_mbx_info.a2dp.cnt_ack,
			 prptctrl->bt_mbx_info.a2dp.cnt_nack);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s :"
			 " wl_rfk[req:%d/go:%d/reject:%d/tout:%d/time = %dms]",
		     	 "[RFK/LPS]", cx->cnt_wl[BTC_WCNT_RFK_REQ],
		     	 cx->cnt_wl[BTC_WCNT_RFK_GO],
		     	 cx->cnt_wl[BTC_WCNT_RFK_REJECT],
		     	 cx->cnt_wl[BTC_WCNT_RFK_TIMEOUT],
		     	 wl->rfk_info.proc_time);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", bt_rfk[req:%d]",
			 prptctrl->bt_cnt[BTC_BCNT_RFK_REQ]);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", AOAC[RF_on:%d/RF_off:%d]",
			 prptctrl->rpt_info.cnt_aoac_rf_on,
			 prptctrl->rpt_info.cnt_aoac_rf_off);
	} else {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : h2c_cnt=%d(fail:%d), c2h_cnt=%d",
			 "[summary]", pfwinfo->cnt_h2c,
			 pfwinfo->cnt_h2c_fail, pfwinfo->cnt_c2h);
	}

	if (!pcinfo->valid || pfwinfo->len_mismch || pfwinfo->fver_mismch ||
	    pfwinfo->err[BTFRE_EXCEPTION])
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : WL FW rpt error!![rpt_ctrl_valid:%d/len:"
			 "0x%x/ver:0x%x/ex:%d/lps=%d/rf_off=%d]",
			 "[ERROR]", pcinfo->valid, pfwinfo->len_mismch,
			 pfwinfo->fver_mismch, pfwinfo->err[BTFRE_EXCEPTION],
			 wl->status.map.lps, wl->status.map.rf_off);

	for (i = 0; i < BTC_NCNT_MAX; i++)
		cnt_sum += dm->cnt_notify[i];

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : total=%d, show_coex_info=%d,"
		 " power_on=%d, init_coex=%d, ", "[notify_cnt]",
		 cnt_sum, cnt[BTC_NCNT_SHOW_COEX_INFO],
		 cnt[BTC_NCNT_POWER_ON], cnt[BTC_NCNT_INIT_COEX]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "power_off=%d, radio_state=%d, role_info=%d,"
		 " wl_rfk=%d, wl_sta=%d",
		 cnt[BTC_NCNT_POWER_OFF], cnt[BTC_NCNT_RADIO_STATE],
		 cnt[BTC_NCNT_ROLE_INFO], cnt[BTC_NCNT_WL_RFK],
		 cnt[BTC_NCNT_WL_STA]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : scan_start=%d, scan_finish=%d,"
		 " switch_band=%d, special_pkt=%d, ", "[notify_cnt]",
		 cnt[BTC_NCNT_SCAN_START], cnt[BTC_NCNT_SCAN_FINISH],
		 cnt[BTC_NCNT_SWITCH_BAND], cnt[BTC_NCNT_SPECIAL_PACKET]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "timer=%d, control=%d, customerize=%d, hub_msg=%d",
		 cnt[BTC_NCNT_TIMER], cnt[BTC_NCNT_CONTROL],
		 cnt[BTC_NCNT_CUSTOMERIZE], btc->hubmsg_cnt);
}

static void _show_fbtc_slots(struct btc_t *btc, u32 *used,
			     char input[][MAX_ARGV], u32 input_num,
			     char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	struct fbtc_slot s;
	u8 i = 0;

	for (i = 0; i < CXST_MAX; i++) {
		s = dm->slot_now[i];
		if (i % 5 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len-*used,
				 "\n\r %-15s : %s[%d/0x%x/%d]",
				 "[slot_list]",
				 id_to_str(BTC_STR_SLOT, (u32)i),
				 s.dur, s.cxtbl, s.cxtype);
		else
			BTC_CNSL(out_len, *used, output + *used, out_len-*used,
				 ", %s[%d/0x%x/%d]",
				 id_to_str(BTC_STR_SLOT, (u32)i),
				 s.dur, s.cxtbl, s.cxtype);
	}
}

static void _show_fbtc_cysta(struct btc_t *btc, u32 *used,
			     char input[][MAX_ARGV], u32 input_num,
			     char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_dm *dm = &btc->dm;
	struct btc_bt_a2dp_desc *a2dp = &btc->cx.bt.link_info.a2dp_desc;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_cysta *pcysta = NULL;
	u8 i, cnt = 0, slot_pair, divide_cnt = 12;
	u16 cycle, c_begin, c_end, s_id;

	pcinfo = &pfwinfo->rpt_fbtc_cysta.cinfo;
	if (!pcinfo->valid)
		return;

	pcysta = &pfwinfo->rpt_fbtc_cysta.finfo;
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : bcn[all:%d/all_ok:%d/bt:%d/bt_ok:%d]",
		 "[slot_cnt]", pcysta->bcn_cnt[CXBCN_ALL],
		 pcysta->bcn_cnt[CXBCN_ALL_OK], pcysta->bcn_cnt[CXBCN_BT_SLOT],
		 pcysta->bcn_cnt[CXBCN_BT_OK]);

	for (i = 0; i < CXST_MAX; i++) {
		if (!pcysta->slot_cnt[i])
			continue;
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", %s:%d",
			 id_to_str(BTC_STR_SLOT, (u32)i),
			 pcysta->slot_cnt[i]);
	}

	if (dm->tdma_now.rxflctrl) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", leak_rx:%d", pcysta->leak_slot.cnt_rximr);
	}

	if (pcysta->collision_cnt) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", collision:%d", pcysta->collision_cnt);
	}

	if (pcysta->skip_cnt) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", skip:%d", pcysta->skip_cnt);
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n\r %-15s : cycle:%d, avg_t[wl:%d/bt:%d/lk:%d.%03d]",
		 "[cycle_time]",
		 pcysta->cycles,
		 pcysta->cycle_time.tavg[CXT_WL],
		 pcysta->cycle_time.tavg[CXT_BT],
		 pcysta->leak_slot.tavg/1000, pcysta->leak_slot.tavg%1000);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 ", max_t[wl:%d/bt:%d/lk:%d.%03d]",
		 pcysta->cycle_time.tmax[CXT_WL],
		 pcysta->cycle_time.tmax[CXT_BT],
		 pcysta->leak_slot.tamx/1000, pcysta->leak_slot.tamx%1000);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 ", maxdiff_t[wl:%d/bt:%d]",
		 pcysta->cycle_time.tmaxdiff[CXT_WL],
		 pcysta->cycle_time.tmaxdiff[CXT_BT]);

	if (a2dp->exist) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : a2dp_ept:%d, a2dp_late:%d",
			 "[a2dp_t_sta]",
			 pcysta->a2dp_ept.cnt, pcysta->a2dp_ept.cnt_timeout);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 ", avg_t:%d, max_t:%d",
			 pcysta->a2dp_ept.tavg, pcysta->a2dp_ept.tmax);
	}

	/* cycle statistics exceptions */
	if (pcysta->except_map || pcysta->except_cnt) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : exception type: 0x%x, cnt = %d",
			 "[cycle_ERROR]",
			 pcysta->except_map, pcysta->except_cnt);
	}

	if (pcysta->cycles <= 1)
		return;

	/* 1 cycle = 1 wl-slot + 1 bt-slot */
	slot_pair = BTC_CYCLE_SLOT_MAX/2;

	if (pcysta->cycles <= slot_pair)
		c_begin = 1;
	else
		c_begin = pcysta->cycles - slot_pair + 1;

	c_end = pcysta->cycles;

	if (a2dp->exist)
		divide_cnt = 3;
	else
		divide_cnt = 12;

	if (c_begin > c_end)
		return;

	for (cycle = c_begin; cycle <= c_end; cycle++) {
		cnt++;
		s_id = ((cycle-1) % slot_pair)*2;

		if (cnt % divide_cnt == 1)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "\n\r %-15s : ", "[cycleT_ermtan]");

		BTC_CNSL(out_len, *used, output + *used,
			 out_len - *used,
			 "->b%d", pcysta->slot_step_time[s_id]);

		if (a2dp->exist)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "(%d/%d/%dM/%d/%d/%d)",
				 pcysta->a2dp_trx[s_id].empty_cnt,
				 pcysta->a2dp_trx[s_id].retry_cnt,
				 (pcysta->a2dp_trx[s_id].tx_rate ? 3:2),
				 pcysta->a2dp_trx[s_id].tx_cnt,
				 pcysta->a2dp_trx[s_id].ack_cnt,
				 pcysta->a2dp_trx[s_id].nack_cnt);


		BTC_CNSL(out_len, *used, output + *used,
			 out_len - *used,
			 "->w%d", pcysta->slot_step_time[s_id+1]);

		if (a2dp->exist)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "(%d/%d/%dM/%d/%d/%d)",
				 pcysta->a2dp_trx[s_id+1].empty_cnt,
				 pcysta->a2dp_trx[s_id+1].retry_cnt,
				 (pcysta->a2dp_trx[s_id+1].tx_rate ? 3:2),
				 pcysta->a2dp_trx[s_id+1].tx_cnt,
				 pcysta->a2dp_trx[s_id+1].ack_cnt,
				 pcysta->a2dp_trx[s_id+1].nack_cnt);
	}
}

static void _show_fbtc_nullsta(struct btc_t *btc, u32 *used,
			       char input[][MAX_ARGV], u32 input_num,
			       char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_cynullsta *ns = NULL;
	u8 i = 0;

	if (!btc->dm.tdma_now.rxflctrl)
		return;

	pcinfo = &pfwinfo->rpt_fbtc_nullsta.cinfo;
	if (!pcinfo->valid)
		return;

	ns = &pfwinfo->rpt_fbtc_nullsta.finfo;

	for (i = CXNULL_STATE_0; i <= CXNULL_STATE_1; i++) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "\n\r %-15s : ", "[null_status]");
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "null-%d", i);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "[Tx:%d/", ns->result[i][CXNULL_TX]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "ok:%d/", ns->result[i][CXNULL_OK]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "late:%d/", ns->result[i][CXNULL_LATE]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "fail:%d/", ns->result[i][CXNULL_FAIL]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "retry:%d/", ns->result[i][CXNULL_RETRY]);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "avg_t:%d.%03d/",
			 ns->tavg[i]/1000, ns->tavg[i]%1000);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "max_t:%d.%03d]",
			 ns->tmax[i]/1000, ns->tmax[i]%1000);
	}
}

#ifdef BTC_FW_STEP_DBG
static void _show_fbtc_step(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_steps *pstep = NULL;
	u8 type, val;
	u16 diff_t;
	u32 i, cnt = 0, n_begin = 0, n_end = 0, array_idx = 0;

	PHL_INFO("[BTC],%s():rpt_en_map=0x%x\n", __func__, pfwinfo->rpt_en_map);

	if ((pfwinfo->rpt_en_map & RPT_EN_FW_STEP_INFO) == 0)
		return;

	pcinfo = &pfwinfo->rpt_fbtc_step.cinfo;
	if (!pcinfo->valid)
		return;

	pstep = &pfwinfo->rpt_fbtc_step.finfo;

	if (pcinfo->req_fver != pstep->fver || !pstep->cnt)
		return;

	if (pstep->cnt <= FCXDEF_STEP)
		n_begin = 1;
	else
		n_begin = pstep->cnt - FCXDEF_STEP + 1;

	n_end = pstep->cnt;

	if (n_begin > n_end)
		return;

	/* restore step info by using ring instead of FIFO */
	for (i = n_begin; i <= n_end; i++) {
		array_idx = (i - 1) % FCXDEF_STEP;
		type = pstep->step[array_idx].type;
		val = pstep->step[array_idx].val;
		diff_t = pstep->step[array_idx].difft;

		if (type == CXSTEP_NONE || type >= CXSTEP_MAX)
			continue;

		if (cnt % 10 == 0)
			BTC_CNSL(out_len, *used, output + *used, out_len-*used,
				 "\n\r %-15s : ", "[fw_steps]");

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			  "-> %s(%d)", (type == CXSTEP_SLOT?
			  id_to_str(BTC_STR_SLOT, (u32)val) :
			  id_to_str(BTC_STR_EVENT, (u32)val)),
			  diff_t);
		cnt++;
	}
}
#endif

static void _show_fw_dm_msg(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	if (!(btc->dm.coex_info_map & BTC_COEX_INFO_DM))
		return;

	_show_fbtc_slots(btc, used, input, input_num, output, out_len);
	_show_fbtc_cysta(btc, used, input, input_num, output, out_len);
	_show_fbtc_nullsta(btc, used, input, input_num, output, out_len);
#ifdef BTC_FW_STEP_DBG
	_show_fbtc_step(btc, used, input, input_num, output, out_len);
#endif
}

static void _cmd_show(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 show_map = 0xff;

	if (input_num > 1)
		_os_sscanf(input[1], "%x", &show_map);

	btc->dm.coex_info_map = show_map & 0xff;

	_show_cx_info(btc, used, input, input_num, output, out_len);
	_show_wl_info(btc, used, input, input_num, output, out_len);
	_show_bt_info(btc, used, input, input_num, output, out_len);
	_show_dm_info(btc, used, input, input_num, output, out_len);
	_show_fw_dm_msg(btc, used, input, input_num, output, out_len);
	_show_mreg(btc, used, input, input_num, output, out_len);
	_show_summary(btc, used, input, input_num, output, out_len);
}

static void _cmd_dbg(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		     u32 input_num, char *output, u32 out_len)
{
	struct btc_ops *ops = btc->ops;
	struct btc_wl_info *wl = &btc->cx.wl;
	u8 buf[8] = {0};
	u32 len, n;
	u32 val = 0, type = 0, i;

	if (input_num < 3 || input_num > 10) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " dbg <type> <para1:8bits> <para2:8bits>... \n");
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " dbg = 0: H2C-C2H loopback <para1: loopback data>\n");
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " dbg = 1: H2C-mailbox <para1~para8: mailbox data>\n");
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " dbg = 2: slot request for P2P+BT coex"
			 " <para1: slot length (hex)>\n");
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " dbg = 3: enable fw_step debug <para1:8bits>\n");
		return;
	}

	len = input_num - 1;
	_os_sscanf(input[1], "%d", &type);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " test type=%d", type);

	buf[0] = (u8)(type & 0xff);

	for (n = 2; n <= len; n++) {
		_os_sscanf(input[n], "%x", &val);
		buf[n-1] = (u8)(val & 0xff);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " para%d=0x%x", n-1, buf[n-1]);
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used, " \n");

	switch (type) {
	case 0: /* H2C-C2H loopback */
		ops->fw_cmd(btc, BTFC_SET, SET_H2C_TEST, buf, (u16)len);
		btc->dbg.rb_done = false;
		btc->dbg.rb_val = 0xff;
		for (i = 0; i < 50; i++) {
			if (!btc->dbg.rb_done) {
				hal_mdelay(btc->hal, 10);
			} else {
				BTC_CNSL(out_len, *used, output + *used,
					 out_len - *used,
					 " H2C-C2H loopback data = 0x%x !!\n",
					 btc->dbg.rb_val);
				return;
			}
		}
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " timeout !!\n");
		break;
	case 1: /* H2C-mailbox */
		ops->fw_cmd(btc, BTFC_SET, SET_H2C_TEST, buf, (u16)len);
		break;
	case 2: /* bt slot request */
		btc->bt_req_len[wl->pta_req_mac] = (u32)buf[1];
		hal_btc_send_event(btc,  wl->pta_req_mac,
				   (u8*)&btc->bt_req_len[wl->pta_req_mac],
				   4, BTC_HMSG_SET_BT_REQ_SLOT);
		break;
	case 3: /* fw_step debug */
#ifdef BTC_FW_STEP_DBG
		if (buf[1] > 0)
			hal_btc_fw_en_rpt(btc, RPT_EN_FW_STEP_INFO, 1);
		else
			hal_btc_fw_en_rpt(btc, RPT_EN_FW_STEP_INFO, 0);
#endif
		break;
	}
}

static void _cmd_wb(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		    u32 input_num, char *output, u32 out_len)
{
	u32 type = 0, addr = 0, val = 0;

	if (input_num >= 4) {
		_os_sscanf(input[1], "%d", &type);
		_os_sscanf(input[2], "%x", &addr);
		_os_sscanf(input[3], "%x", &val);
	}

	if (type > 4 || input_num < 4) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " wb <type --- 0:rf, 1:modem, 2:bluewize, 3:vendor,"
			 " 4:LE> <addr:16bits> <val> \n");
		return;
	}

	_write_bt_reg(btc, (u8)type, (u16)addr, val);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " wb type=%d, addr=0x%x, val=0x%x !! \n", type, addr, val);
}

static void _cmd_set_coex(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			  u32 input_num, char *output, u32 out_len)
{
	u32 mode = 0, para = 0;

	if (input_num >= 2)
		_os_sscanf(input[1], "%d", &mode);

	if (mode == 5 && input_num == 3)
		_os_sscanf(input[2], "%d", &para);

	if (input_num < 2 ||
	    (mode == 5 && (para >= BTC_FDDT_MAX || input_num != 3))) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " mode <val--- 0:original, 1:freeze coex,"
			 " 2:always-freerun, 3:always-WLonly,"
			 " 4:always-BTonly, 5:FDD-Train> \n");

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " mode 5 <FDD-Type -- 0:auto, 1:force-stop, 2:fix-tdd,"
			 " 3:fix-half-fdd, 4:fix-full-fdd> \n");
		return;
	}

	switch(mode) {
	case 0: /* original */
		btc->ctrl.manual = 0;
		btc->ctrl.always_freerun = 0;
		btc->dm.wl_only = 0;
		btc->dm.bt_only = 0;
		_set_init_info(btc);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " recovery original coex mechanism !! \n");
		break;
	case 1: /* freeze */
		btc->ctrl.manual = 1;
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " freeze coex mechanism !! \n");
		break;
	case 2: /* fix freerun */
		btc->ctrl.always_freerun = 1;
		hal_btc_fw_set_drv_info(btc, CXDRVINFO_CTRL);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " freerun coex mechanism !! \n");
		break;
	case 3: /* fix wl only */
		btc->dm.wl_only = 1;
		_set_init_info(btc);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " always WL-only coex mechanism!! \n");
		break;
	case 4: /* fix bt only */
		btc->dm.bt_only = 1;
		_set_init_info(btc);
		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " always BT-only coex mechanism!! \n");
		break;
	case 5: /* fdd train */
		btc->dm.fddt_info.type = (u8) para;

		_run_coex(btc, __func__);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " Force FDD-Train mechanism!! \n");
		break;
	}
}

static void _cmd_upd_policy(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	hal_btc_fw_set_policy(btc, FC_EXEC, (u16)BTC_CXP_MANUAL<<8, __func__);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " Update tdma/slot policy !! \n");
}

static void _cmd_tdma(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 para1 = 0, para2 = 0;
	u8 buf[8] = {0};
	u16 len = (u16)sizeof(struct fbtc_tdma);

	if (input_num >= 3) {
		_os_sscanf(input[1], "%x", &para1);
		_os_sscanf(input[2], "%x", &para2);

		buf[0] = (u8)((para1 & bMASKB3) >> 24);
		buf[1] = (u8)((para1 & bMASKB2) >> 16);
		buf[2] = (u8)((para1 & bMASKB1) >> 8);
		buf[3] = (u8)(para1 & bMASKB0);

		buf[4] = (u8)((para2 & bMASKB3) >> 24);
		buf[5] = (u8)((para2 & bMASKB2) >> 16);
		buf[6] = (u8)((para2 & bMASKB1) >> 8);
		buf[7] = (u8)(para2 & bMASKB0);

		if (hal_btc_fw_set_1tdma(btc, len, buf)) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "tdma-para1: type=%s,"
				 " rxflctrl=0x%x, txflctrl=%d, rsvd=%d\n",
				 id_to_str(BTC_STR_TDMA, (u32)buf[0]),
				 buf[1], buf[2], buf[3]);

			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "tdma-para2: leak_n=%d,"
				 " ext_ctrl=%x, rxflctrl_role=0x%x,"
				 " option=0x%x\n",
				 buf[4], buf[5], buf[6], buf[7]);
			return;
		}
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " tdma <para1:32bits> <para2:32bits>\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "       <para1>  bit[31:24]-> type ---"
		 " 0:off, 1:fix, 2:auto, 3:auto2\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[23:16]-> rx_flow_ctrl_type"
		 " --- 0:off, 1:null, 2:Qos-null, 3:cts\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[15:8] -> tx_flow_ctrl --- 0:off, 1:on\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[7:0]  -> resevred\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "       <para2>  bit[31:24]-> leak_n ---"
		 " enter leak slot every n*w2b-slot\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[23:16]-> ext_ctrl ---"
		 " 0:off, 1:bcn-early_protect only 2:ext\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[15:8] -> rxflctrl_role\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "                bit[7:0]  -> option\n");
}

static void _cmd_slot(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 sid = 0, dur = 0, cx_tbl = 0, cx_type = 0, i;
	u32 *tmp32 = NULL;
	u8 buf[9] = {0};
	u16 len = (u16)sizeof(struct fbtc_slot)+1;

	if (input_num >= 5) {
		_os_sscanf(input[1], "%d", &sid);
		_os_sscanf(input[2], "%d", &dur);
		_os_sscanf(input[3], "%x", &cx_tbl);
		_os_sscanf(input[4], "%d", &cx_type);

		buf[0] = (u8) sid;
		buf[1] = (u8)(dur & bMASKB0);
		buf[2] = (u8)((dur & bMASKB1) >> 8);

		tmp32 = (u32 *)&buf[3];
		*tmp32 = cx_tbl;

		buf[7] = (u8)(cx_type & bMASKB0);
		buf[8] = (u8)((cx_type & bMASKB1) >> 8);

		if (hal_btc_fw_set_1slot(btc, len, buf)) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " set slot para: slot:%s,"
				 " duration:%d, table:%08x, type=%s!\n",
				 id_to_str(BTC_STR_SLOT, sid), dur, cx_tbl,
				 (cx_type? "iso" : "mix"));
			return;
		}
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " slot <slot_id:8bits> <slot_duration:16bits>"
		 " <coex_table:32bits> <type:16bits>\n");

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <slot_id> \n");

	for (i = 0; i < CXST_MAX; i++) {
		if ((i+1) % 5 == 1)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                %d: %s",
				 i, id_to_str(BTC_STR_SLOT, i));
		else
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 ", %d: %s", i, id_to_str(BTC_STR_SLOT, i));

		if (((i+1) % 5 == 0) || (i == CXST_MAX - 1))
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "\n");
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <slot_duration> unit: ms\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <coex_table> 32bits coex table\n");
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "      <type> BT packet at WL slot 0: allowed, 1: isolated\n");
}

static void _cmd_sig_gdbg_en(struct btc_t *btc, u32 *used,
			     char input[][MAX_ARGV], u32 input_num,
			     char *output, u32 out_len)
{
	u32 map = 0, i;

	if (input_num >= 2) {
		_os_sscanf(input[1], "%x", &map);

		hal_btc_fw_set_gpio_dbg(btc, CXDGPIO_EN_MAP, map);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " signal to gpio debug map = 0x%08x!!\n", map);
		return;
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " sig <enable_map:hex/32bits> \n");

	for (i = 0; i < BTC_DBG_MAX1; i++) {
		if ((i+1) % 8 == 1)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "      %d: %s",
				 i, id_to_str(BTC_STR_GDBG, i));
		else
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 ", %d: %s", i, id_to_str(BTC_STR_GDBG, i));

		if (((i+1) % 8 == 0) || (i == BTC_DBG_MAX1 - 1))
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "\n");
	}
}

static void _cmd_sgpio_map(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			   u32 input_num, char *output, u32 out_len)
{
	u32 sig = 0, gpio = 0;

	if (input_num >= 3) {
		_os_sscanf(input[1], "%d", &sig);
		_os_sscanf(input[2], "%d", &gpio);
	}

	if ((sig > 31 || gpio > 7) || input_num < 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " gpio <sig:0~31> <gpio:0~7>\n");
		return;
	}

	hal_btc_fw_set_gpio_dbg(btc, CXDGPIO_MUX_MAP, ((gpio << 8) + sig));

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " signal-%s -> gpio-%d\n", id_to_str(BTC_STR_GDBG, sig), gpio);
}

static void _cmd_wl_tx_power(struct btc_t *btc, u32 *used,
			     char input[][MAX_ARGV], u32 input_num,
			     char *output, u32 out_len)
{
	u32 pwr = 0;
	u8 is_negative = 0;

	if (input_num >= 2) {
		_os_sscanf(input[1], "%d", &pwr);
		if (pwr & BIT(31)) {
			pwr = ~pwr + 1;
			is_negative = 1;
		}
		pwr = pwr & bMASKB0;
	}

	if ((pwr != 255 && pwr > 20) || input_num < 2) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " wpwr <wl_tx_power(dBm): -20dbm ~ +20dBm,"
			 " 255-> original tx power\n");
		return;
	}

	if (is_negative)
		pwr |= BIT(7);

	_set_wl_tx_power(btc, pwr); /* pwr --> 1's complement */

	if (pwr == 0xff)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set wl tx power level to original!!\n");
	else if (is_negative)
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set wl tx power level = -%d dBm!!\n", pwr & 0x7f);
	else
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set wl tx power level = +%d dBm!!\n", pwr & 0x7f);
}

static void _cmd_wl_rx_lna(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			   u32 input_num, char *output, u32 out_len)
{
	u32 lna = 0;

	if (input_num < 2) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " wlna <wl_rx_level: 0~7> \n");
		return;
	}

	_os_sscanf(input[1], "%d", &lna);

	_set_wl_rx_gain(btc, lna);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set wl rx level = %d!!\n", lna);
}

static void _cmd_bt_afh_map(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	u32 en = 0, ch = 0, bw = 0;

	if (input_num < 4) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " afh <en: 0/1> <ch: 0~255> <bw: 0~255>\n");
		return;
	}

	_os_sscanf(input[1], "%d", &en);
	_os_sscanf(input[2], "%d", &ch);
	_os_sscanf(input[3], "%d", &bw);

	btc->cx.wl.afh_info.en = (u8)en;
	btc->cx.wl.afh_info.ch = (u8)ch;
	btc->cx.wl.afh_info.bw = (u8)bw;
	hal_btc_fw_set_bt(btc, SET_BT_WL_CH_INFO, 3, (u8*)&btc->cx.wl.afh_info);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set bt afh map: en=%d, ch=%d, map=%d!!\n", en, ch, bw);
}

static void _cmd_bt_tx_power(struct btc_t *btc, u32 *used,
			     char input[][MAX_ARGV], u32 input_num,
			     char *output, u32 out_len)
{
	s32 pwr = 0;
	u32 pwr2;

	if (input_num < 2) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " bpwr <decrease power: 0~255 > \n");
		return;
	}

	_os_sscanf(input[1], "%d", &pwr);

	pwr2 = pwr & bMASKB0;

	_set_bt_tx_power(btc, pwr2);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " decrease bt tx power level = %d!!\n", pwr2);
}

static void _cmd_bt_rx_lna(struct btc_t *btc, u32 *used, char input[][MAX_ARGV],
			   u32 input_num, char *output, u32 out_len)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_bt_info *bt = &btc->cx.bt;
	u32 lna = 0;
	u8 buf;

	if (input_num >= 2)
		_os_sscanf(input[1], "%d", &lna);

	if (input_num < 2 || lna > 7) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " blna <lna_constrain: 0~7> \n");
		return;
	}

	bt->rf_para.rx_gain_freerun = lna;
	btc->dm.rf_trx_para.bt_rx_gain = lna;

	buf = (u8)(lna & bMASKB0);

	hal_btc_fw_set_bt(btc, SET_BT_LNA_CONSTRAIN, 1, &buf);

	_os_delay_us(btc->hal, BTC_SCBD_REWRITE_DELAY*2);

	if (buf == BTC_BT_RX_NORMAL_LVL)
		_write_scbd(btc, BTC_WSCB_RXGAIN, false);
	else
		_write_scbd(btc, BTC_WSCB_RXGAIN, true);

	rtw_hal_mac_set_scoreboard(btc->hal, &wl->scbd);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set bt rx lna constrain level = %d!!\n", buf);
}

static void _cmd_bt_igno_wl(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	u32 igno = 0;

	if (input_num < 2) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " igwl <0: don't ignore wlan, 1: ignore wlan > \n");
		return;
	}

	_os_sscanf(input[1], "%d", &igno);

	igno = igno & BIT(0);

	_set_bt_ignore_wl_act(btc, (u8)igno);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " set bt ignore wlan = %d!!\n", igno);
}

static void _cmd_set_gnt_wl(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	u32 gwl = 0, phy_map = BTC_PHY_ALL;

	if (input_num != 2 && input_num != 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " gwl <0:SW_0, 1:SW_1, 2:HW_PTA>"
			 " <0:PHY-0, 1:PHY-1, none:All> \n");
		return;
	}

	_os_sscanf(input[1], "%d", &gwl);

	if (input_num == 3) {
		_os_sscanf(input[2], "%d", &phy_map);
		phy_map = (phy_map == 0? BTC_PHY_0 : BTC_PHY_1);
	}

	switch(gwl) {
	case 0:
		_set_gnt(btc, (u8)phy_map, BTC_GNT_SW_LO, BTC_GNT_SET_SKIP);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_wl = SW-0 (phy_map=0x%x)\n", phy_map);
		break;
	case 1:
		_set_gnt(btc, (u8)phy_map, BTC_GNT_SW_HI, BTC_GNT_SET_SKIP);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_wl = SW-1 (phy_map=0x%x)\n", phy_map);
		break;
	case 2:
		_set_gnt(btc, (u8)phy_map, BTC_GNT_HW, BTC_GNT_SET_SKIP);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_wl = HW-PTA ctrl (phy_map=0x%x)\n", phy_map);
		break;
	}
}

static void _cmd_set_gnt_bt(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	u32 gbt = 0, phy_map = BTC_PHY_ALL;

	if (input_num != 2 && input_num != 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " gbt <0:SW_0, 1:SW_1, 2:HW_PTA>"
			 " <0:PHY-0, 1:PHY-1, none:All> \n");
		return;

	}

	_os_sscanf(input[1], "%d", &gbt);

	if (input_num == 3) {
		_os_sscanf(input[2], "%d", &phy_map);
		phy_map = (phy_map == 0? BTC_PHY_0 : BTC_PHY_1);
	}

	switch(gbt) {
	case 0:
		_set_gnt(btc, (u8)phy_map, BTC_GNT_SET_SKIP, BTC_GNT_SW_LO);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_bt = SW-0 (phy_map=0x%x)\n", phy_map);
		break;
	case 1:
		_set_gnt(btc, (u8)phy_map, BTC_GNT_SET_SKIP, BTC_GNT_SW_HI);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_bt = SW-1 (phy_map=0x%x)\n", phy_map);
		break;
	case 2:
		_set_gnt(btc, (u8)phy_map, BTC_GNT_SET_SKIP, BTC_GNT_HW);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " set gnt_bt = HW-PTA ctrl (phy_map=0x%x)\n", phy_map);
		break;
	}
}

static void _cmd_set_bt_psd(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	u32 idx = 0, type = 0;

	if (input_num < 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " bpsd <start_idex, dec-8bit>"
			 "<report_type, dec-8bit> \n");
		return;
	}

	_os_sscanf(input[1], "%d", &idx);
	_os_sscanf(input[2], "%d", &type);

	_bt_psd_setup(btc, (u8)idx, (u8)type);
}

s8 _get_bw_att_db(struct btc_t *btc)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	s8 bw_att_db = 13;

	if (wl->afh_info.bw >= CHANNEL_WIDTH_5)
		return bw_att_db;

	if (wl->afh_info.bw != CHANNEL_WIDTH_80_80)
		bw_att_db += 3 * (s8)(wl->afh_info.bw - CHANNEL_WIDTH_20);
	else
		bw_att_db += 3 * 3;

	return bw_att_db;
}

void _get_wl_nhm_dbm(struct btc_t *btc)
{
	struct btc_wl_nhm *wl_nhm = &btc->cx.wl.nhm;
	s8 bw_att_db = 0, nhm_pwr_dbm = 0, pwr = 0, cur_pwr = 0;
	u16 save_index = 0;
	bool new_data_flag = false;
	struct watchdog_nhm_report nhm_rpt = {0};

	bw_att_db = _get_bw_att_db(btc);

	rtw_hal_bb_nhm_mntr_result(btc->hal, &nhm_rpt, HW_PHY_0);

	if (!nhm_rpt.ccx_rpt_result)
		return;

	nhm_pwr_dbm = nhm_rpt.nhm_pwr_dbm;

	wl_nhm->instant_wl_nhm_dbm = nhm_pwr_dbm;

	if (wl_nhm->start_flag == false) {
		wl_nhm->start_flag = true;
		pwr = nhm_pwr_dbm - bw_att_db;
		new_data_flag = true;
	} else {
		pwr = wl_nhm->pwr;
		if (wl_nhm->last_ccx_rpt_stamp != nhm_rpt.ccx_rpt_stamp) {
			new_data_flag = true;
			wl_nhm->current_status = 1; // new data
		} else {
			wl_nhm->current_status = 2; //duplicated data
		}
	}

	if (!new_data_flag)
		return;

	wl_nhm->last_ccx_rpt_stamp = nhm_rpt.ccx_rpt_stamp;
	cur_pwr = nhm_pwr_dbm - bw_att_db;
	wl_nhm->instant_wl_nhm_per_mhz = cur_pwr;

	wl_nhm->valid_record_times += 1;

	if (wl_nhm->valid_record_times == 0) {
		wl_nhm->valid_record_times = 16;
	}

	wl_nhm->ratio = nhm_rpt.nhm_ratio;

	if (cur_pwr < pwr) {
		pwr = cur_pwr;
		wl_nhm->refresh = true;
	} else {
		pwr = (pwr >> 1) + (cur_pwr >> 1);
		wl_nhm->refresh = false;
	}

	save_index = ((wl_nhm->valid_record_times + 16 - 1) % 16) & 0xf;

	wl_nhm->pwr = pwr;
	wl_nhm->record_pwr[save_index] = pwr;
	wl_nhm->record_ratio[save_index] = nhm_rpt.nhm_ratio;
}

static void _cmd_get_wl_nhm(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btc_wl_nhm *wl_nhm = &wl->nhm;
	u16 start_index = 0, stop_index = 0, i;
	u32 show_record_num = 16;

	if (input_num >= 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " wnhm <last_record_num, dec,1-16, default:16> \n");
		return;
	}

	_get_wl_nhm_dbm(btc);

	_os_sscanf(input[1], "%d", &show_record_num);

	if (show_record_num >= 16)
		show_record_num = 16;
	else if (show_record_num == 0)
		show_record_num = 1;

	if (wl_nhm->valid_record_times == 0) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "wl nhm not ready\n");
		return;
	}

	if (wl_nhm->current_status == 0) {
		BTC_CNSL(out_len, *used, output + *used,
			 out_len - *used, "wl nhm failed this time\n");
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "nhm_psd = %d dBm/MHz, nhm_ratio = %d, status = %d, "
		 "valid_record_times = %d\n",
		 wl_nhm->pwr, wl_nhm->ratio,
		 wl_nhm->current_status, wl_nhm->valid_record_times);

	stop_index = wl_nhm->valid_record_times;
	if (wl_nhm->valid_record_times > (u16)show_record_num)
		start_index = wl_nhm->valid_record_times - (u16)show_record_num;

	for (i = start_index; i < stop_index; i++) {
		if (i == start_index) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "record_pwr(old->new)= %4d",
				 wl_nhm->record_pwr[(i + 16) & 0xF]);
		} else if (i == (stop_index - 1)) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, ", %4d dBm/MHz\n",
				 wl_nhm->record_pwr[(i + 16) & 0xF]);
		} else {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, ", %4d",
				 wl_nhm->record_pwr[(i + 16) & 0xF]);
		}
	}

	for (i = start_index; i < stop_index; i++) {
		if (i == start_index) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "record_ratio(old->new)= %4d",
				 wl_nhm->record_ratio[(i + 16) & 0xF]);
		} else if (i == (stop_index - 1)) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, ", %4d percent\n",
				 wl_nhm->record_ratio[(i + 16) & 0xF]);
		} else {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, ", %4d",
				 wl_nhm->record_ratio[(i + 16) & 0xF]);
		}
	}
}

void _get_wl_cn_report(struct btc_t *btc)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	wl->cn_report = rtw_hal_ex_cn_report(btc->hal);
}


void _get_wl_evm_report(struct btc_t *btc)
{
#if 0
	struct btc_wl_info *wl = &btc->cx.wl;
	wl->evm_1ss_rpt = rtw_hal_ex_evm_1ss_report(btc->hal);
	wl->evm_2ss_max_rpt = rtw_hal_ex_evm_max_report(btc->hal);
	wl->evm_2ss_min_rpt = rtw_hal_ex_evm_min_report(btc->hal);
#endif
}

#ifdef BTC_FDDT_TRAIN_SUPPORT
static void _cmd_fddt_cell(struct btc_t *btc, u32 *used,
			   char input[][MAX_ARGV], u32 type,
			   char *output, u32 out_len)
{
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_cysta *pcysta = NULL;
	u8 i, j, r_lvl, m_lvl = BTC_BT_RSSI_THMAX;
	s8 rssi_th, rssi_th2, para1, para2, para3, para4;
	struct btc_fddt_train_info *train_now = &btc->dm.fddt_info.train_now;
	struct btc_fddt_cell cell[5][5];
	struct fbtc_fddt_cell_status cell_c2h[5][5];

	if (type == BTC_FDDT_CELL_STATE_UL || type == BTC_FDDT_CELL_STATE_DL) {
		pcinfo = &pfwinfo->rpt_fbtc_cysta.cinfo;
		if (!pcinfo->valid)
			return;

		pcysta = &pfwinfo->rpt_fbtc_cysta.finfo;
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n============ ");

	switch (type) {
	case BTC_FDDT_CELL_RANGE_UL:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "Cell-Range-UL (WL_Tx_min/WL_Tx_max/BT_Tx_min/BT_Rx)");
		hal_mem_cpy(btc->hal, cell, train_now->cell_ul, sizeof(cell));
		break;
	case BTC_FDDT_CELL_STATE_UL:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "Cell-State-UL (WL_Tx/BT_Tx_Dec/BT_Rx/Cell_Status)");
		hal_mem_cpy(btc->hal, cell_c2h, pcysta->fddt_cells[0],
			    sizeof(cell_c2h));
		break;
	case BTC_FDDT_CELL_STATE_DL:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "Cell-State-DL (WL_Tx/BT_Tx_Dec/BT_Rx/Cell_Status)");
		hal_mem_cpy(btc->hal, cell_c2h, pcysta->fddt_cells[1],
			    sizeof(cell_c2h));
		break;
	default:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "Cell-Range-DL (WL_Tx_min/WL_Tx_max/BT_Tx_min/BT_Rx)");
		hal_mem_cpy(btc->hal, cell, train_now->cell_dl, sizeof(cell));
		break;
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " ============\nWL_RSSI               > %3d    ",
		 btc->chip->wl_rssi_thres[0] - 110);

	for (i = 0; i < m_lvl; i++) {
		rssi_th = btc->chip->wl_rssi_thres[i] - 110;
		if (i == m_lvl - 1) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "   < %3d   \n", rssi_th);
		} else {
			rssi_th2 = btc->chip->wl_rssi_thres[i+1] - 110;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " %3d ~ %3d  ",
				 rssi_th, rssi_th2);
		}
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "BT_RSSI > %d:    ", btc->chip->bt_rssi_thres[0] - 100);

	for (i = 0; i <= m_lvl; i++) {
		if (type == BTC_FDDT_CELL_STATE_UL ||
		    type == BTC_FDDT_CELL_STATE_DL) {
			para1 = cell_c2h[m_lvl][m_lvl-i].wl_tx_pwr;
			para2 = cell_c2h[m_lvl][m_lvl-i].bt_tx_pwr;
			para3 = cell_c2h[m_lvl][m_lvl-i].bt_rx_gain;
			para4 = (cell_c2h[m_lvl][m_lvl-i].state << 4) |
				 cell_c2h[m_lvl][m_lvl-i].phase;
		} else {
			para1 = cell[m_lvl][m_lvl-i].wl_pwr_min;
			para2 = cell[m_lvl][m_lvl-i].wl_pwr_max;
			para3 = cell[m_lvl][m_lvl-i].bt_pwr_dec_max;
			para4 = cell[m_lvl][m_lvl-i].bt_rx_gain;
		}

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " %2d/%2d/%2d/%2x", para1, para2, para3, para4);
	}

	BTC_CNSL(out_len, *used, output + *used, out_len - *used, "\n");

	for (j = 0; j < m_lvl; j++) {
		rssi_th = btc->chip->bt_rssi_thres[j] - 100;
		r_lvl = m_lvl - j - 1;

		if (j == m_lvl - 1) {
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "BT_RSSI < %d:    ", rssi_th);
		} else {
			rssi_th2 = btc->chip->bt_rssi_thres[j+1] - 100;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "BT_RSSI %d ~ %d:", rssi_th, rssi_th2);
		}

		for (i = 0; i <= 4; i++) {
			if (type == BTC_FDDT_CELL_STATE_UL ||
			    type == BTC_FDDT_CELL_STATE_DL) {
				para1 = cell_c2h[r_lvl][m_lvl-i].wl_tx_pwr;
				para2 = cell_c2h[r_lvl][m_lvl-i].bt_tx_pwr;
				para3 = cell_c2h[r_lvl][m_lvl-i].bt_rx_gain;
				para4 = (cell_c2h[r_lvl][m_lvl-i].state << 4) |
					 cell_c2h[r_lvl][m_lvl-i].phase;
			} else {
				para1 = cell[r_lvl][m_lvl-i].wl_pwr_min;
				para2 = cell[r_lvl][m_lvl-i].wl_pwr_max;
				para3 = cell[r_lvl][m_lvl-i].bt_pwr_dec_max;
				para4 = cell[r_lvl][m_lvl-i].bt_rx_gain;
			}

			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " %2d/%2d/%2d/%2x",
				 para1, para2, para3, para4);
		}

		BTC_CNSL(out_len, *used, output + *used, out_len - *used, "\n");
	}
}

static void _show_fddt_cycle(struct btc_t *btc, u32 *used,
			     char input[][MAX_ARGV], u32 input_num,
			     char *output, u32 out_len)
{
	struct btc_wl_info *wl = &btc->cx.wl;
	struct btf_fwinfo *pfwinfo = &btc->fwinfo;
	struct btc_rpt_cmn_info *pcinfo = NULL;
	struct fbtc_cysta *pcysta = NULL;
	u8 cnt = 0;
	u16 cycle, c_begin, c_end, s_id;

	pcinfo = &pfwinfo->rpt_fbtc_cysta.cinfo;
	if (!pcinfo->valid)
		return;

	pcysta = &pfwinfo->rpt_fbtc_cysta.finfo;

	if (pcysta->cycles <= 1)
		return;

	if (pcysta->cycles <= BTC_CYCLE_SLOT_MAX)
		c_begin = 1;
	else
		c_begin = pcysta->cycles - BTC_CYCLE_SLOT_MAX + 1;

	c_end = pcysta->cycles;

	if (c_begin > c_end)
		return;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n============"
		 "Cycle ([RSSI]-[Train]-[TRx_Para]-[mW_KPI]-[mB_KPI])"
		 "============\n");

	for (cycle = c_begin; cycle <= c_end; cycle++) {
		cnt++;
		s_id = (cycle-1) % BTC_CYCLE_SLOT_MAX;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "No.%d", cycle);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "([W%d/B%d/CN%d]-[Cy%d/St%d->S%d/P%d/R0x%x]-"
			 "[WT%d/WR%d/BT%d/BR%d]-[TP:%d]-[ne:%d])-->",
			 pcysta->fddt_trx[s_id].rssi & 0xf,
			 (pcysta->fddt_trx[s_id].rssi & 0xf0) >> 4,
			 pcysta->fddt_trx[s_id].cn,
			 pcysta->fddt_trx[s_id].phase_cycle,
			 pcysta->fddt_trx[s_id].train_step,
			 (pcysta->fddt_trx[s_id].train_status & 0xf0) >> 4,
			 pcysta->fddt_trx[s_id].train_status & 0xf,
			 pcysta->fddt_trx[s_id].train_result,
			 pcysta->fddt_trx[s_id].tx_power,
			 wl->rf_para.rx_gain_freerun,
			 pcysta->fddt_trx[s_id].bt_tx_power,
			 pcysta->fddt_trx[s_id].bt_rx_gain,
			 pcysta->fddt_trx[s_id].tp,
			 pcysta->fddt_trx[s_id].no_empty_cnt);

		if (cnt % 2 == 0)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, "\n");
	}
}

static void _cmd_fddt_show(struct btc_t *btc, u32 *used,
			   char input[][MAX_ARGV], u32 input_num,
			   char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_cx *cx = &btc->cx;
	struct btc_wl_info *wl = &cx->wl;
	struct btc_bt_info *bt = &cx->bt;
	struct btc_module *module = &btc->mdinfo;
	struct btc_wl_role_info *wl_rinfo = &wl->role_info;
	struct btc_bt_link_info *bt_linfo = &bt->link_info;
	struct btc_wl_link_info *plink = &wl->link_info[0];
	struct fbtc_rpt_ctrl *finfo = &btc->fwinfo.rpt_ctrl.finfo;
	struct btc_rpt_ctrl_a2dp_empty *a2dp = &finfo->bt_mbx_info.a2dp;
	struct btc_traffic t = plink->stat.traffic;
	struct btc_fddt_info *fdinfo = &dm->fddt_info;
	struct btc_fddt_train_info *train_now = &fdinfo->train_now;
	struct btc_rpt_ctrl_a2dp_empty *a2dp_last = &fdinfo->bt_stat.a2dp_last;
	u32 ratio_ack = 0, ratio_nack = 0, show_map = BTC_FDDT_INFO_MAP_DEF;
	u32 n;

	if (input_num > 1)
		_os_sscanf(input[1], "%x", &show_map);

	if (show_map & BIT(0)) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "============ %s()  ============\n", __func__);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "enable=%d, count=%d, type=%s, state=%s, result=%d,"
			 " rsn_map=0x%x, tdma_policy=%s, aiso_sort_avg=%d\n",
			 dm->fddt_train, dm->cnt_dm[BTC_DCNT_FDDT_TRIG],
			 id_to_str(BTC_STR_FDDT_TYPE, (u32)fdinfo->type),
			 id_to_str(BTC_STR_FDDT_STATE, (u32)fdinfo->state),
			 fdinfo->result, fdinfo->nrsn_map,
			 id_to_str(BTC_STR_POLICY, (u32)btc->policy_type),
			 module->ant.isolation);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "[Pass_Ctrl] map=0x%x, no_empty_cnt=%d, tp_ratio=%d, "
			 "kpibtr_ratio=%d\n",
			 train_now->t_ctrl.m_cycle,
			 train_now->t_ctrl.w_cycle,
			 train_now->t_ctrl.k_cycle,
			 train_now->f_chk.wl_kpibtr_ratio);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "[Pass_Ctrl] map=0x%x, no_empty_cnt=%d, tp_ratio=%d\n" ,
			 train_now->f_chk.check_map,
			 train_now->f_chk.bt_no_empty_cnt,
			 train_now->f_chk.wl_tp_ratio);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "[Break_Ctrl] map=0x%x, no_empty_cnt=%d, tp_ratio=%d",
			 train_now->b_chk.check_map,
			 train_now->b_chk.bt_no_empty_cnt,
			 train_now->b_chk.wl_tp_ratio);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "tp_low=%d, cn=%d, cell_chg=%d, cn_limit=%d, nhm_limit=%d\n",
			 train_now->b_chk.wl_tp_low_bound,
			 train_now->b_chk.cn,
			 train_now->b_chk.cell_chg,
			 train_now->b_chk.cn_limit,
			 train_now->b_chk.nhm_limit);
	}

	if (show_map & BIT(1))
		_cmd_fddt_cell(btc, used, input, plink->dir, output, out_len);

	if (show_map & BIT(2)) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n============ WL ============\n");

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "link_mode=%s, rssi=-%ddBm(%d, level=%d), busy=%d,"
			 " nhm=%d, cn=%d\n",
			 id_to_str(BTC_STR_WLLINK, (u32)wl_rinfo->link_mode),
			 110-plink->stat.rssi, plink->stat.rssi, wl->rssi_level,
			 plink->busy, wl->nhm.pwr, wl->cn_report);
#if 0
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "evm(1ss:%d, 2ss_max:%d, 2ss_min:%d)\n",
			 wl->evm_1ss_rpt,
			 wl->evm_2ss_max_rpt, wl->evm_2ss_min_rpt);
#endif

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "dir=%s, tx[tp:%d Mbps/byte:%d/rate:%s/busy_level:%d],"
			 " rx[tp:%d Mbps/byte:%d/rate:%s/busy_level:%d]\n",
			 (plink->dir == TRAFFIC_UL ? "UL" : "DL"),
			 t.tx_tp, (u32)t.tx_byte,
			 id_to_str(BTC_STR_RATE, (u32)t.tx_rate), t.tx_lvl,
			 t.rx_tp, (u32)t.rx_byte,
			 id_to_str(BTC_STR_RATE, (u32)t.rx_rate), t.rx_lvl);
	}

	if (show_map & BIT(3)) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "\n============ BT ============\n");

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "state=0x%x, rssi=%ddBm(%d, level=%d), role:%s,"
			 " tx_rate:%dM, rate_chg:%d\n",
			 bt->raw_info[2], bt_linfo->rssi-100,
			 bt_linfo->rssi, bt->rssi_level,
			 (bt_linfo->slave_role ? "Slave" : "Master"),
			 (bt_linfo->tx_3M? 3 : 2),
			 cx->cnt_bt[BTC_BCNT_RATECHG]);

		if (a2dp->cnt_tx != 0) {
			 ratio_ack = a2dp->cnt_ack * 100 / a2dp->cnt_tx;
			 ratio_nack = a2dp->cnt_nack * 100 / a2dp->cnt_tx;
		}

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "Retry:%d, A2DP_empty:%d"
			 "[stop:%d/tx:%d/ack:%d(%d)/nack:%d(%d)]\n",
			 cx->cnt_bt[BTC_BCNT_RETRY],
			 a2dp->cnt_empty, a2dp->cnt_flowctrl, a2dp->cnt_tx,
			 a2dp->cnt_ack, ratio_ack, a2dp->cnt_nack, ratio_nack);

		n = a2dp->cnt_tx - a2dp_last->cnt_tx;

		if (n > 0) {
			ratio_ack = (a2dp->cnt_ack - a2dp_last->cnt_ack) * 100;
			ratio_ack /= n;
			ratio_nack = (a2dp->cnt_nack - a2dp_last->cnt_nack)*100;
			ratio_nack /= n;
		}

		n = cx->cnt_bt[BTC_BCNT_RETRY] - fdinfo->bt_stat.retry_last;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "Retry_diff:%d, A2DP_empty_diff:%d"
			 "[stop:%d/tx:%d/ack:%d(%d)/nack:%d(%d)]\n",
			 n, a2dp->cnt_empty - a2dp_last->cnt_empty,
			 a2dp->cnt_flowctrl - a2dp_last->cnt_flowctrl,
			 a2dp->cnt_tx - a2dp_last->cnt_tx,
			 a2dp->cnt_ack - a2dp_last->cnt_ack, ratio_ack,
			 a2dp->cnt_nack - a2dp_last->cnt_nack, ratio_nack);

                n = sizeof(struct btc_rpt_ctrl_a2dp_empty);
		fdinfo->bt_stat.retry_last = cx->cnt_bt[BTC_BCNT_RETRY];
		hal_mem_cpy(btc->hal, a2dp_last, a2dp, n);
	}

	if (show_map & BIT(4))
		_show_fddt_cycle(btc, used, input, input_num, output, out_len);

	n = plink->dir | 0x80;
	if (show_map & BIT(5))
		_cmd_fddt_cell(btc, used, input, n, output, out_len);
}

static void _cmd_fddt_set(struct btc_t *btc, u32 *used,
			  char input[][MAX_ARGV], u32 input_num,
			  char *output, u32 out_len)
{
	u32 para = 0;

	if (input_num > 1)
		_os_sscanf(input[1], "%d", &para);

	if (input_num <= 1 || para >= BTC_FDDT_MAX) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " ftset <val--- 0:force-stop, 1:auto,"
			 " 2:fix-tdd-phase, 3:fix-fdd-phase> \n");
		return;
	}

	btc->dm.fddt_info.type = (u8) para;

	_run_coex(btc, __func__);
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " FDD-Train para = %d!!\n", para);
}

static void _cmd_fddt_ctrl(struct btc_t *btc, u32 *used,
			   char input[][MAX_ARGV], u32 input_num,
			   char *output, u32 out_len)
{
	struct btc_dm *dm = &btc->dm;
	struct btc_module *module = &btc->mdinfo;
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_fddt_train_info *t = &dm->fddt_info.train;
	u32 type = 0, para = 0, para1 = 0;
	u8 buf[7] = {0};

	_os_sscanf(input[1], "%d", &type);

	if (input_num <= 1 || type > 4) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " ftctrl <val--- 0:time_cycle, 1:break_check,"
			 " 2:fail_check, 3:cell_para, 4:cell_update>\n");
		return;
	} else if (input_num <= 3) {
		switch (type) {
		case 0:
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " time_cycle: ftctrl 0 "
				 "<1:m_cycle, 2:w_cycle, 3:k_cycle>\n");
			return;
		case 1:
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " break_check: ftctrl 1 "
				 "<1:bt_no_ept_cnt, 2:wl_tp_ratio,"
				 " 3:wl_tp_low_bound, 4:cn, 5:cell_chg,"
				 " 6: cn_limit, 7: nhm_limit> \n");
			return;
		case 2:
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " fail_check: ftctrl 2 "
				 "<1:bt_no_ept_cnt, 2:wl_tp_ratio, 3:wl_kpibtr_ratio> \n");
			return;
		case 3:
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " cell_para: ftctrl 3 "
				 "<cell_index:24bits> <cell_para:32bits> \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                 <cell_index> bit[23:16] ->"
				 " wl_dir --- 0:UL, 1:DL \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                       bit[15:8] ->"
				 " bt_rssi --- 0~4 \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                       bit[7:0] ->"
				 " wl_rssi --- 0~4 \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                <cell_para> bit[31:24] ->"
				 " wl_pwr_max --- 0~0x0f \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                        bit[23:16] ->"
				 " wl_pwr_min --- 0~0x0f \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                        bit[15:8] ->"
				 " bt_pwr_dec_max --- 0~0x28 \n");
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 "                        bit[7:0] ->"
				 " bt_rx_gain --- 0x07~0x04 \n");
			return;
		case 4:
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " cell_update: ftctrl 4 "
				 "<0:reset to default, 1:update by abt_iso>\n");
			return;
		}
	}

	if (type != 3) {
		_os_sscanf(input[2], "%d", &para);
		_os_sscanf(input[3], "%d", &para1);
	} else {
		_os_sscanf(input[2], "%x", &para);
		_os_sscanf(input[3], "%x", &para1);

		/*buf[0] buf[1] buf[2] : UL/DL, bt_rssi->0~4, wl_rssi->0~4*/
		buf[0] = (u8)((para & bMASKB2) >> 16);
		buf[1] = (u8)((para & bMASKB1) >> 8);
		buf[2] = (u8)(para & bMASKB0);

		/*buf[3] buf[4] buf[5] buf[6] :
		 * wl_pwr_max wl_pwr_min bt_pwr_dec_max bt_rx_gain
		 */
		buf[3] = (u8)((para1 & bMASKB3) >> 24);
		buf[4] = (u8)((para1 & bMASKB2) >> 16);
		buf[5] = (u8)((para1 & bMASKB1) >> 8);
		buf[6] = (u8)(para1 & bMASKB0);
	}

	switch (type) {
	case 0:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 		 " FDDT Time_cycle setup\n");
		if (para == 1) {
			t->t_ctrl.m_cycle = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " m_cycle=%d\n", para1);
		} else if (para == 2) {
			t->t_ctrl.w_cycle = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " w_cycle=%d\n", para1);
		} else {
			t->t_ctrl.k_cycle = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " k_cycle=%d\n", para1);
		}
		break;
	case 1:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 		 " FDDT Break_check setup\n");
		if (para == 1) {
			t->b_chk.bt_no_empty_cnt = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " bt_no_ept_cnt=%d\n", para1);
		} else if (para == 2) {
			t->b_chk.wl_tp_ratio = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " wl_tp_ratio=%d\n", para1);
		} else if (para == 3) {
			t->b_chk.wl_tp_low_bound = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " wl_tp_low=%d\n", para1);
		} else if (para == 4) {
			t->b_chk.cn = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " cn=%d\n", para1);
		} else if (para == 5) {
			t->b_chk.cell_chg = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " cell_chg_cnt=%d\n", para1);
		} else if (para == 6) {
			t->b_chk.cn_limit = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " cn_limit=%d\n", para1);
		} else {
			t->b_chk.nhm_limit = (s8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " nhm_limit=%d\n", para1);
        }
		break;
	case 2:
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 		 " FDDT Fail_check setup\n");
		if (para == 1) {
			t->f_chk.bt_no_empty_cnt = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " bt_no_ept_cnt=%d\n", para1);
		} else if (para == 2) {
			t->f_chk.wl_tp_ratio = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " wl_tp_ratio=%d\n", para1);
		} else {
			t->f_chk.wl_kpibtr_ratio = (u8) para1;
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " wl_kpibtr_ratio=%d\n", para1);
		}
		break;
	case 3:
		if (!buf[0]) {
			t->cell_ul[buf[1]][buf[2]].wl_pwr_min = (u8) buf[3];
			t->cell_ul[buf[1]][buf[2]].wl_pwr_max = (u8) buf[4];
			t->cell_ul[buf[1]][buf[2]].bt_pwr_dec_max = (u8) buf[5];
			t->cell_ul[buf[1]][buf[2]].bt_rx_gain = (u8) buf[6];
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " FDDT Cell_para "
		 		 "[UL:%d] [bt:%d]/[wl:%d] = %d/%d/%d/%d!!\n",
		 		 buf[0], buf[1], buf[2], buf[3], buf[4],
		 		 buf[5], buf[6]);
		} else {
			t->cell_dl[buf[1]][buf[2]].wl_pwr_min = (u8) buf[3];
			t->cell_dl[buf[1]][buf[2]].wl_pwr_max = (u8) buf[4];
			t->cell_dl[buf[1]][buf[2]].bt_pwr_dec_max = (u8) buf[5];
			t->cell_dl[buf[1]][buf[2]].bt_rx_gain = (u8) buf[6];
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used, " FDDT Cell_para "
		 		 "[DL:%d] [bt:%d]/[wl:%d] = %d/%d/%d/%d!!\n",
		 		 buf[0], buf[1], buf[2], buf[3], buf[4],
		 		 buf[5], buf[6]);
		}
		break;
	case 4:
		if (para1 == 1) {
			_set_fddt_cell_by_antiso(btc);
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
		 		 " FDDT Cell update by ant_iso = %d!\n",
		 		 module->ant.isolation);
		} else {
			hal_mem_cpy(h, t->cell_ul, cell_ul_def,
				    sizeof(cell_ul_def));
			hal_mem_cpy(h, t->cell_dl, cell_dl_def,
				    sizeof(cell_dl_def));
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
		 		 " FDDT Cell reset to default!\n");
		}
		break;
	default:
		break;
	}

	_run_coex(btc, __func__);
}
#endif

#ifdef BTC_AISO_SUPPORT
static void _bt_psd_enable(struct btc_t *btc, bool enable)
{
	u8 buf[8] = {0};

	buf[0] = 1;
	buf[1] = 0x22;
	buf[2] = 0x2;

	if (enable)
		buf[3] |= BIT(0);
#if 0
	if (fix_gain) {
		buf[3] |= BIT(1);
		buf[4] = (gain_idx & 0x3F);
	}
#endif

	hal_btc_fw_set_bt(btc, SET_H2C_TEST, 5, buf);
}

void _bt_psd_setup(struct btc_t *btc, u8 start_idx, u8 auto_rpt_type)
{
	u8 buf[2] = {0};

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_,
		  "[BTC], %s(): set bt psd start_idx = %d, rpt_type = %d\n",
		  __func__, start_idx, auto_rpt_type);

	buf[0] = start_idx;
	buf[1] = (auto_rpt_type & 0x3) | ((BT_PSD_SRC_RAW & 0x3) << 2);

	hal_btc_fw_set_bt(btc, SET_BT_PSD_REPORT, 2, buf);
}

static bool _bt_psd_rpt_ctrl(struct btc_t *btc, bool en_psd)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	u8 cur_wl_ch = btc->cx.wl.afh_info.ch, start_index;
	bool result = false;

	if (en_psd && (cur_wl_ch >= 1 && cur_wl_ch <= 14) &&
	    (!bt_psd_dm->aiso_data_ok && !bt_psd_dm->en)) {

		if (cur_wl_ch < 14)
			start_index = 3 + 5 * (cur_wl_ch - 1);
		else
			start_index = 75;

		bt_psd_dm->en = true;
		bt_psd_dm->rec_time_out = false;
		result = true;

		/* set query start index to BT */
		_bt_psd_setup(btc, start_index, BT_PSD_RPT_TYPE_CAL);

		_bt_psd_enable(btc, true);

		PHL_INFO("[BTC], %s(): enable=%d: cur_wl_ch=%d, start_idx=%d\n",
			 __func__, en_psd, cur_wl_ch, start_index);
	} else if (!en_psd && bt_psd_dm->en) { /* disable */
		_bt_psd_enable(btc, false);
		bt_psd_dm->en = false;
		result = true;

		PHL_INFO("[BTC], %s(): disable!!\n", __func__);
	}

	return result;
}

static s8 _bt_psd_s16du8r(s16 sum, u8 num)
{
	s16 ans_int;
	s16 ans_fra;

	if (num == 0) {
		PHL_INFO("[BTC], %s() Divider is zero\n", __func__);
		return 1;
	}

	ans_int = (s16)(sum / num);
	ans_fra = sum - ans_int * num;

	if (ans_fra >= 0) {
		if (ans_fra * 2 >= (s16)num)
			ans_int++;
	} else {
		if (-ans_fra * 2 >= (s16)num)
			ans_int--;
	}
	return (s8)ans_int;
}

static s16 _bt_psd_s32du16r(s32 sum, u16 num)
{
	s32 ans_int;
	s32 ans_fra;

	if (num == 0) {
		PHL_INFO("[BTC], %s() Divider is zero\n", __func__);
		return 1;
	}

	ans_int = (s32)(sum / num);
	ans_fra = sum - ans_int * num;

	if (ans_fra >= 0) {
		if (ans_fra * 2 >= (s32)num)
			ans_int++;
	} else {
		if (-ans_fra * 2 >= (s32)num)
			ans_int--;
	}
	return (s16)ans_int;
}

static s8 _bt_psd_db_sum(s8 db_a, s8 db_b)
{
	s8 db_major;
	s8 db_minor;
	s8 db_sum;

	if (db_a >= db_b) {
		db_major = db_a;
		db_minor = db_b;
	} else {
		db_major = db_b;
		db_minor = db_a;
	}

	db_sum = db_major - db_minor; /* diff db */
	if (db_sum <= 1) /* 0~1 */
		db_sum = db_major + 3;
	else if (db_major <= 3) /* 2~3 */
		db_sum = db_major + 2;
	else if (db_major <= 9) /* 4~9 */
		db_sum = db_major + 1;
	else /* >=10 */
		db_sum = db_major;

	return db_sum;
}

static u16 _bt_psd_get_tx_rate(struct btc_t *btc)
{
	struct btc_wl_link_info *plink = NULL;
	u8 i;
	bool is_found = false;

	for (i = 0; i < BTC_WL_MAX_ROLE_NUMBER; i++) {
		plink = &btc->cx.wl.link_info[i];
		if (plink->active && plink->connected != MLME_NO_LINK) {
			is_found = true;
			break;
		}
	}

	if (!is_found)
		PHL_INFO("[BTC], %s() wl no valid port!!!\n", __func__);

	return (plink->stat.traffic.tx_rate);
}

u8 _bt_psd_get_tx_ss_num(struct btc_t *btc)
{
	struct btc_wl_link_info *plink = NULL;
	u8 ss = 0, i;
	u16 tx_rate = RTW_DATA_RATE_MAX;
	bool is_found = false;

	for (i = 0; i < BTC_WL_MAX_ROLE_NUMBER; i++) {
		plink = &btc->cx.wl.link_info[i];
		if (plink->role == PHL_RTYPE_STATION &&
		    plink->active && plink->connected != MLME_NO_LINK) {
			is_found = true;
			tx_rate = plink->stat.traffic.tx_rate;
			break;
		}
	}

	if (is_found && tx_rate != RTW_DATA_RATE_MAX) {
		if (tx_rate <= RTW_DATA_RATE_OFDM54)
			ss = 1;
		else if (tx_rate <= RTW_DATA_RATE_MCS31) /* bit[4:3] */
			ss = ((tx_rate >> 3) & 0x3) + 1;
		else /* bit[5:4] */
			ss = ((tx_rate >> 4) & 0x3) + 1;
	}

	return ss;
}

s8 _bt_psd_get_tx_pwr_dbm(struct btc_t *btc, u8 rf_path)
{
	s16 wl_txp_dbm_tssi;
	u32 reg_add;
	u16 reg_val;

	if (rf_path == RF_PATH_A)
		reg_add = 0x1C44;
	else
		reg_add = 0x3C44;

	reg_val = (u16)(rtw_hal_bb_read_cr(btc->hal, reg_add, 0x1FF) & 0x1FF);

	wl_txp_dbm_tssi = 16 + (((s16)reg_val - 300) / 8);

	return ((s8)wl_txp_dbm_tssi);
}

static s8 _bt_psd_get_tx_pwr(struct btc_t *btc)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	s8 wl_tx_power_dbm = 0, tx_power_a, tx_power_b;

	bt_psd_dm->wl_tx_ss = _bt_psd_get_tx_ss_num(btc);
	bt_psd_dm->wl_tx_rate = _bt_psd_get_tx_rate(btc);

	tx_power_a = _bt_psd_get_tx_pwr_dbm(btc, RF_PATH_A);
	tx_power_b = _bt_psd_get_tx_pwr_dbm(btc, RF_PATH_B);

	if (bt_psd_dm->wl_tx_ss < 2)
		wl_tx_power_dbm = tx_power_a;
	else
		wl_tx_power_dbm = _bt_psd_db_sum(tx_power_a, tx_power_b);

	wl_tx_power_dbm -= _get_bw_att_db(btc); /* Tx power per MHz */

	return (wl_tx_power_dbm);
}

static void _bt_psd_aiso_var_reset(struct btc_t *btc)
{
	struct rtw_hal_com_t *h = (struct rtw_hal_com_t *)btc->hal;
	struct btc_aiso_val *av = &btc->bt_psd_dm.aiso_val;

	PHL_INFO("[BTC], %s():\n", __func__);
	hal_mem_set(h, av, 0, sizeof(struct btc_aiso_val));

	hal_mem_set(h, av->psd_max, 0x80, sizeof(av->psd_max));
	hal_mem_set(h, av->psd_min, 0x7F, sizeof(av->psd_min));

	hal_mem_set(h, av->txbusy_psd_max, 0x80, sizeof(av->txbusy_psd_max));
	hal_mem_set(h, av->txbusy_psd_min, 0x7F, sizeof(av->txbusy_psd_min));
	hal_mem_set(h, av->txbusy_psd_avg, 0x80, sizeof(av->txbusy_psd_avg));

	hal_mem_set(h, av->txidle_psd_max, 0x80, sizeof(av->txidle_psd_max));
	hal_mem_set(h, av->txidle_psd_min, 0x7F, sizeof(av->txidle_psd_min));
	hal_mem_set(h, av->txidle_psd_avg, 0x80, sizeof(av->txidle_psd_avg));

	hal_mem_set(h, av->aiso_md, -128, sizeof(av->aiso_md));
}

static void _bt_psd_aiso_db_update(struct btc_t *btc)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	struct btc_aiso_val *av = &bt_psd_dm->aiso_val;
	u8 aiso_method = bt_psd_dm->aiso_method;
	u8 aiso_method_final = bt_psd_dm->aiso_method_final;
	u8 idx_offset = 0, i, cnt;
	s8 rx_psd_final = 0;
	s16 aiso_db_final = -128, sum;

	for (i = 0; i < 10; i++) {
		cnt = av->txidle_psd_cnt[i];
		sum = av->txidle_psd_sum[i];

		if (cnt == 0) {
			av->txidle_psd_avg[i] = -128;
			continue;
		}

		av->txidle_psd_avg[i] = _bt_psd_s16du8r(sum, cnt);
	}

	if (aiso_method != BTC_AISO_M01_AVG && aiso_method != BTC_AISO_M0 &&
	    aiso_method != BTC_AISO_M1 && aiso_method != BTC_AISO_M_ALL)
		aiso_db_final = -128;

	_limit_val(aiso_db_final, 127, -128);

	if (aiso_method_final < BTC_AISO_M_MAX) {
		aiso_db_final = av->aiso_md[aiso_method_final];
		rx_psd_final = av->rx_psd[aiso_method_final];
	}

	PHL_INFO("[BTC], %s(): Goal %d tms m:%d aiso = %d (%d-%d) dB\n",
		 __func__, av->psd_rec_cnt, aiso_method_final,
		 aiso_db_final, av->wl_air_psd_avg, rx_psd_final);

	/* Update ant iso to array: 1st_data -> index = 0  */
	bt_psd_dm->aiso_db[bt_psd_dm->aiso_db_cnt & 0xF] = (s8)aiso_db_final;
	bt_psd_dm->aiso_db_cnt++;

	if (bt_psd_dm->aiso_db_cnt < 16)
		idx_offset = 0;
	else
		idx_offset = bt_psd_dm->aiso_db_cnt - 16;

	for (i = idx_offset; i < bt_psd_dm->aiso_db_cnt; i++) {
		PHL_INFO("[BTC], %s(): %d times ant_iso = %d\n",
			 __func__, i+1, bt_psd_dm->aiso_db[i & 0xF]);
	}
}

static s16 _bt_psd_aiso_db_calc(struct btc_t *btc, s16 rx_psd_sum, u8 cnt)
{
	struct btc_aiso_val *av = &btc->bt_psd_dm.aiso_val;
	s32 res_part = 0;
	s16 aiso_db = -128;

	if (av->psd_rec_cnt == 0)
		return aiso_db;

	res_part = av->wl_air_psd_sum - av->wl_air_psd_avg * av->psd_rec_cnt;
	res_part = (res_part * cnt) / av->psd_rec_cnt;
	res_part = rx_psd_sum - res_part;

	_limit_val(res_part, 32767, -32768);

	aiso_db = av->wl_air_psd_avg - _bt_psd_s16du8r((s16)res_part, cnt);

	return aiso_db;
}

static void _bt_psd_group_calc(struct btc_t *btc, u8 md)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	struct btc_aiso_val *av = &bt_psd_dm->aiso_val;
	s8 psd_max = 0, psd_min = 0;
	s8 *psd_avg = NULL;
	s16 rx_psd_sum = 0, psd_sum = 0, res_part = 0, val = 0;
	u8 cnt = 0, i, psd_cnt = 0;

	/* Excute for the following cases:
	 * 1. aiso_method = BTC_AISO_M_ALL
	 * 2. aiso_method = BTC_AISO_M4 && md = BTC_AISO_M1
	 * 3. aiso_method = BTC_AISO_M6 && md = BTC_AISO_M5
	 * 4. aiso_method = md
	 */
	if (bt_psd_dm->aiso_method != BTC_AISO_M_ALL &&
	    (!(bt_psd_dm->aiso_method == BTC_AISO_M4 && md == BTC_AISO_M1)) &&
	    (!(bt_psd_dm->aiso_method == BTC_AISO_M6 && md == BTC_AISO_M5)) &&
	    bt_psd_dm->aiso_method != md)
		return;

	for (i = 0; i < 10; i++) {
		psd_max = av->txbusy_psd_max[i];
		psd_min = av->txbusy_psd_min[i];
		psd_cnt = av->txbusy_psd_cnt[i];
		psd_sum = av->txbusy_psd_sum[i];
		psd_avg = &av->txbusy_psd_avg[i];

		/* Skip if the freq-point not reach the threshold */
		if (md <= BTC_AISO_M4 && psd_cnt < BT_PSD_TX_BUSY_CNT_MIN)
			continue;

		switch(md) {
		case BTC_AISO_M0: /* m0: busy avg */
			*psd_avg = _bt_psd_s16du8r(psd_sum, psd_cnt);
			rx_psd_sum += *psd_avg;
			cnt++;
			break;
		case BTC_AISO_M1: /* m1: busy max */
			rx_psd_sum += psd_max;
			cnt++;
			break;
		case BTC_AISO_M2: /* m2: busy remove max and min */
			val = psd_sum - psd_max - psd_min;
			*psd_avg = _bt_psd_s16du8r(val, psd_cnt - 2);
			rx_psd_sum += *psd_avg;
			cnt++;
			break;
		case BTC_AISO_M3: /* m3: busy remove max */
			val = psd_sum - psd_max;
			*psd_avg = _bt_psd_s16du8r(val, psd_cnt - 1);
			rx_psd_sum += *psd_avg;
			cnt++;
			break;
		case BTC_AISO_M4: /* m4:filter m1 */
			if (psd_max >= av->rx_psd[BTC_AISO_M1])
				rx_psd_sum += psd_max;
			cnt++;
			break;
		case BTC_AISO_M5: /* m5: hold max value */
			rx_psd_sum += psd_max;
			cnt++;
			break;
		case BTC_AISO_M6: /* m6: filter m5 */
			if (psd_max >= av->rx_psd[BTC_AISO_M5])
				rx_psd_sum += psd_max;
			cnt++;
			break;
		case BTC_AISO_M01_AVG: /* avg (m0 + m1) */
			res_part = av->aiso_md[BTC_AISO_M0] +
				   av->aiso_md[BTC_AISO_M1];
			rx_psd_sum = av->rx_psd[BTC_AISO_M0] +
				     av->rx_psd[BTC_AISO_M1];

			cnt = 2;
			break;
		case BTC_AISO_M12_AVG: /* avg (m1 + m2) */
			res_part = av->aiso_md[BTC_AISO_M1] +
				   av->aiso_md[BTC_AISO_M2];
			rx_psd_sum = av->rx_psd[BTC_AISO_M1] +
				     av->rx_psd[BTC_AISO_M2];
			cnt = 2;
			break;
		case BTC_AISO_M13_AVG: /* avg (m1 + m3) */
			res_part = av->aiso_md[BTC_AISO_M1] +
				   av->aiso_md[BTC_AISO_M3];
			rx_psd_sum = av->rx_psd[BTC_AISO_M1] +
				     av->rx_psd[BTC_AISO_M3];
			cnt = 2;
			break;
		case BTC_AISO_M46_AVG: /* avg (m4 + m6) */
			res_part = av->aiso_md[BTC_AISO_M4] +
				   av->aiso_md[BTC_AISO_M6];
			rx_psd_sum = av->rx_psd[BTC_AISO_M4] +
				     av->rx_psd[BTC_AISO_M6];
			cnt = 2;
			break;
		}
	}

	if (cnt == 0) {
		PHL_INFO("[BTC], %s(): m:%d return by cnt = 0\n", __func__, md);
		return;
	}

	av->rx_psd[md] = _bt_psd_s16du8r(rx_psd_sum, cnt);

	if (md <= BTC_AISO_M6)
		av->aiso_md[md] = _bt_psd_aiso_db_calc(btc, rx_psd_sum, cnt);
	else
		av->aiso_md[md] = _bt_psd_s16du8r(res_part, cnt);
}

static void _bt_psd_rec_refresh(struct btc_t *btc, bool is_2nd_half)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	struct btc_aiso_val *av = &bt_psd_dm->aiso_val;
	u8 i = 0, rec_idx = 0;
	u8 *half_data = NULL;
	u8 wl_tx_busy_status = bt_psd_dm->raw_info[2];
	s8 bt_psd_dbm_offset = 0; /* if lna-constrain 5 = 28*/
	s8 cur_psd_rpt_dbm;

	if (is_2nd_half)
		half_data = &av->last_2nd_half[0];
	else
		half_data = &av->last_1st_half[0];

	hal_mem_cpy(btc->hal, half_data, &bt_psd_dm->raw_info[2], 5);

	for (i = 0; i < 5; i++) { /* psd_data_0 ~ psd_data_4 */
		rec_idx = (is_2nd_half ? i + 5 : i);

		cur_psd_rpt_dbm = bt_psd_dm->raw_info[i + 3];
		cur_psd_rpt_dbm += bt_psd_dbm_offset;

		if (cur_psd_rpt_dbm > av->psd_max[rec_idx])
			av->psd_max[rec_idx] = cur_psd_rpt_dbm;

		if (cur_psd_rpt_dbm < av->psd_min[rec_idx])
			av->psd_min[rec_idx] = cur_psd_rpt_dbm;

		if (wl_tx_busy_status & BIT(i)) {
			if (cur_psd_rpt_dbm > av->txbusy_psd_max[rec_idx])
				av->txbusy_psd_max[rec_idx] = cur_psd_rpt_dbm;

			if (cur_psd_rpt_dbm < av->txbusy_psd_min[rec_idx])
				av->txbusy_psd_min[rec_idx] = cur_psd_rpt_dbm;

			/* avoid overflow */
			if (av->txbusy_psd_cnt[rec_idx] < 200) {
				av->txbusy_psd_sum[rec_idx] += cur_psd_rpt_dbm;
				av->txbusy_psd_cnt[rec_idx]++;
			}
		} else {
			if (cur_psd_rpt_dbm > av->txidle_psd_max[rec_idx])
				av->txidle_psd_max[rec_idx] = cur_psd_rpt_dbm;

			if (cur_psd_rpt_dbm < av->txidle_psd_min[rec_idx])
				av->txidle_psd_min[rec_idx] = cur_psd_rpt_dbm;

			/* avoid overflow */
			if (av->txidle_psd_cnt[rec_idx] < 200) {
				av->txidle_psd_sum[rec_idx] += cur_psd_rpt_dbm;
				av->txidle_psd_cnt[rec_idx]++;
			}
		}
	}
}

static bool _bt_psd_ready_check(struct btc_t *btc)
{
	struct btc_aiso_val *av = &btc->bt_psd_dm.aiso_val;

	u8 half1_ch = 0, half2_ch = 0, i = 0;

	/* calculate 1st_half_data_enough_flag */
	for (i = 0; i < 5; i++)
		if (av->txbusy_psd_cnt[i] >= BT_PSD_TX_BUSY_CNT_MIN)
			half1_ch++;

	/* calculate 2nd_half_data_enough_flag */
	for (i = 5; i < 10; i++)
		if (av->txbusy_psd_cnt[i] >= BT_PSD_TX_BUSY_CNT_MIN)
			half2_ch++;

	if (av->psd_rec_cnt % BT_PSD_PRINT_PERIOD == 1)
		PHL_INFO("[BTC], %s(): TxBusy_PSD_CNT[ch]"
			 " = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", __func__,
			  av->txbusy_psd_cnt[0], av->txbusy_psd_cnt[1],
			  av->txbusy_psd_cnt[2], av->txbusy_psd_cnt[3],
			  av->txbusy_psd_cnt[4], av->txbusy_psd_cnt[5],
			  av->txbusy_psd_cnt[6], av->txbusy_psd_cnt[7],
			  av->txbusy_psd_cnt[8], av->txbusy_psd_cnt[9]);

	if (half1_ch >= BT_PSD_VALID_CH_MIN && half2_ch >= BT_PSD_VALID_CH_MIN)
		return true;
	else
		return false;
}

/* post-process bt psd data and change flag to stop it */
static void _bt_psd_process(struct btc_t *btc, bool is_reset)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	struct btc_aiso_val *av = &bt_psd_dm->aiso_val;

	u8 i = 0, start_index, cur_wl_ch = btc->cx.wl.afh_info.ch;
	u8 seq_idx = (bt_psd_dm->raw_info[1] >> 4) & 0xF;
	u8 idx_1st_half = 0, idx_2nd_half = 0;
	u16 cnt = 0;

	/* bt_psd_dm->raw_info format:
	 * (buf + 0) : malbox id
	 * (buf + 1) :  [1:0] : data_src
	 *		[3:2] : auto_rpt_type
	 *		[7:4] : seq_idx
	 * (buf + 2) : 	[4:0] : wl_s0_txbusy_mask
	 *		[7:5] : reserved for wl_rxbusy
	 * (buf + 3) : psd_data_0
	 * (buf + 4) : psd_data_1
	 * (buf + 5) : psd_data_2
	 * (buf + 6) : psd_data_3
	 * (buf + 7) : psd_data_4
	 */

	if (is_reset)
		_bt_psd_aiso_var_reset(btc);

	/* cur_wl_ch decide the psd-start index
	 * ex: cur_wl_ch = 1 --> start index = 3 (2405MHz)
	 * idx_1st_half = 3 ~ 7 (2405MHz~2409MHz)
	 * idx_2nd_half = 13 ~ 17 (2415MHz~2419MHz)
	 */
	if (cur_wl_ch < 14)
		start_index = 3 + 5 * (cur_wl_ch - 1);
	else
		start_index = 75;

	idx_1st_half = start_index & 0xF;
	idx_2nd_half = (start_index + 10) & 0xF;

	if (seq_idx != idx_1st_half && seq_idx != idx_2nd_half) {
		PHL_INFO("[BTC], %s(): wrong psd report index\n", __func__);
		return;
	} else {
		/* refreh record  if data is NOT duplicate */
		av->psd_rec_cnt++;
		cnt = av->psd_rec_cnt;

		PHL_INFO("[BTC], %s(): psd_rec_cnt = %d\n", __func__, cnt);

		if (seq_idx == idx_1st_half)
			_bt_psd_rec_refresh(btc, false);
		else if (seq_idx == idx_2nd_half)
			_bt_psd_rec_refresh(btc, true);
	}

	/* record wl tx psd */
	av->wl_air_psd_sum += _bt_psd_get_tx_pwr(btc); /* dbm per MHz */

	av->wl_air_psd_avg = (s8)_bt_psd_s32du16r(av->wl_air_psd_sum, cnt);

	/* Check if data ready */
	if (!_bt_psd_ready_check(btc)) {
		if (cnt >= BT_PSD_WAIT_CNT) { /* timeout */
			PHL_INFO("[BTC], %s(): wait rec timeout!\n", __func__);

			bt_psd_dm->aiso_data_ok = false;
			bt_psd_dm->rec_start = false;
			bt_psd_dm->rec_time_out = true;
			_bt_psd_rpt_ctrl(btc, false);
		}

		return;
	}

	/* calculate antenna isolation if data ready */
	for (i = 0; i < BTC_AISO_M_MAX; i++) {

		/* Must execute the other methos first for M4/M6 */
		if (i == BTC_AISO_M4)
			_bt_psd_group_calc(btc, BTC_AISO_M1);
		else if (i == BTC_AISO_M6)
			_bt_psd_group_calc(btc, BTC_AISO_M5);

		_bt_psd_group_calc(btc, i);

		PHL_INFO("[BTC], %s(): m%d = %d (%d-(%d)) dB\n", __func__,
			 i, av->aiso_md[i], av->wl_air_psd_avg, av->rx_psd[i]);
	}

	bt_psd_dm->aiso_data_ok = true;
	bt_psd_dm->rec_start = false;

	_bt_psd_aiso_db_update(btc);

	if (bt_psd_dm->aiso_db_cnt < bt_psd_dm->aiso_cmd_cnt) {
		PHL_INFO("[BTC], %s(): aiso_db_cnt = %d, aiso_cmd_cnt = %d\n",
			 __func__, bt_psd_dm->aiso_db_cnt,
			 bt_psd_dm->aiso_cmd_cnt);

		bt_psd_dm->aiso_data_ok = false;
		bt_psd_dm->rec_start = false;
		bt_psd_dm->rec_time_out = false;
		return;
	}

	_bt_psd_rpt_ctrl(btc, false);

	PHL_INFO("[BTC], %s(): Good-Ending!!\n", __func__);
}

/* post-process and change flag to stop it */
void _bt_psd_update(struct btc_t *btc, u8 *buf, u32 len)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	u8 cur_wl_ch = btc->cx.wl.afh_info.ch;
	bool reset_data_flag = false;

	if (!bt_psd_dm->en) {
		/* PHL_INFO("[BTC], %s(): drop by en = false\n", __func__); */
		return;
	}

	if (!btc->dm.freerun) {
		/* return; */
	}

	PHL_INFO("[BTC], %s(len=%d): seq_idx:0x%x/type:0x%x/src:0x%x/tx:0x%x"
		 " [%x %x %x %x %x]\n", __func__, len,
		 *(buf + 1) >> 4, (*(buf + 1) >> 2) & 0x3,
		 *(buf + 1) & 0x3, *(buf + 2) & 0x1F,
		 *(buf + 3), *(buf + 4), *(buf + 5), *(buf + 6), *(buf + 7));

	hal_mem_cpy(h, bt_psd_dm->raw_info, buf, BTC_BTINFO_MAX);

	/* before start */
	if (!bt_psd_dm->rec_start && !bt_psd_dm->aiso_data_ok) {
		bt_psd_dm->rec_start = true;
		reset_data_flag = true;
	} else if (bt_psd_dm->wl_ch_last != 0xFF &&
		   (cur_wl_ch != bt_psd_dm->wl_ch_last)) {
		reset_data_flag = true; /* rec_start & Not-Ready -> change ch */
	}

	if (bt_psd_dm->rec_start) {
		_bt_psd_process(btc, reset_data_flag);
		bt_psd_dm->wl_ch_last = cur_wl_ch;
	}
}

static void _bt_psd_show_record(struct btc_t *btc, u32 *used,
				char input[][MAX_ARGV], u32 input_num,
				char *output, u32 out_len)
{
	struct btc_aiso_val *av = &btc->bt_psd_dm.aiso_val;

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "============ %s()  ============\n", __func__);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " PSD_MAX = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->psd_max[0], av->psd_max[1], av->psd_max[2], av->psd_max[3],
		 av->psd_max[4], av->psd_max[5], av->psd_max[6], av->psd_max[7],
		 av->psd_max[8], av->psd_max[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " PSD_MIN = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->psd_min[0], av->psd_min[1], av->psd_min[2], av->psd_min[3],
		 av->psd_min[4], av->psd_min[5], av->psd_min[6], av->psd_min[7],
		 av->psd_min[8], av->psd_min[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxBusy_PSD_MAX = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->txbusy_psd_max[0], av->txbusy_psd_max[1],
		 av->txbusy_psd_max[2], av->txbusy_psd_max[3],
		 av->txbusy_psd_max[4], av->txbusy_psd_max[5],
		 av->txbusy_psd_max[6], av->txbusy_psd_max[7],
		 av->txbusy_psd_max[8], av->txbusy_psd_max[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxBusy_PSD_MIN = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->txbusy_psd_min[0], av->txbusy_psd_min[1],
		 av->txbusy_psd_min[2], av->txbusy_psd_min[3],
		 av->txbusy_psd_min[4], av->txbusy_psd_min[5],
		 av->txbusy_psd_min[6], av->txbusy_psd_min[7],
		 av->txbusy_psd_min[8], av->txbusy_psd_min[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxBusy_PSD_AVG = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->txbusy_psd_avg[0], av->txbusy_psd_avg[1],
		 av->txbusy_psd_avg[2], av->txbusy_psd_avg[3],
		 av->txbusy_psd_avg[4], av->txbusy_psd_avg[5],
		 av->txbusy_psd_avg[6], av->txbusy_psd_avg[7],
		 av->txbusy_psd_avg[8], av->txbusy_psd_avg[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxBusy_PSD_CNT = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		 av->txbusy_psd_cnt[0], av->txbusy_psd_cnt[1],
		 av->txbusy_psd_cnt[2], av->txbusy_psd_cnt[3],
		 av->txbusy_psd_cnt[4], av->txbusy_psd_cnt[5],
		 av->txbusy_psd_cnt[6], av->txbusy_psd_cnt[7],
		 av->txbusy_psd_cnt[8], av->txbusy_psd_cnt[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxIdle_PSD_MAX = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->txidle_psd_max[0], av->txidle_psd_max[1],
		 av->txidle_psd_max[2], av->txidle_psd_max[3],
		 av->txidle_psd_max[4], av->txidle_psd_max[5],
		 av->txidle_psd_max[6], av->txidle_psd_max[7],
		 av->txidle_psd_max[8], av->txidle_psd_max[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxIdle_PSD_MIN = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->txidle_psd_min[0], av->txidle_psd_min[1],
		 av->txidle_psd_min[2], av->txidle_psd_min[3],
		 av->txidle_psd_min[4], av->txidle_psd_min[5],
		 av->txidle_psd_min[6], av->txidle_psd_min[7],
		 av->txidle_psd_min[8], av->txidle_psd_min[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxIdle_PSD_AVG = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d dBm\n",
		 av->txidle_psd_avg[0], av->txidle_psd_avg[1],
		 av->txidle_psd_avg[2], av->txidle_psd_avg[3],
		 av->txidle_psd_avg[4], av->txidle_psd_avg[5],
		 av->txidle_psd_avg[6], av->txidle_psd_avg[7],
		 av->txidle_psd_avg[8], av->txidle_psd_avg[9]);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 " TxIdle_PSD_CNT = %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
		 av->txidle_psd_cnt[0], av->txidle_psd_cnt[1],
		 av->txidle_psd_cnt[2], av->txidle_psd_cnt[3],
		 av->txidle_psd_cnt[4], av->txidle_psd_cnt[5],
		 av->txidle_psd_cnt[6], av->txidle_psd_cnt[7],
		 av->txidle_psd_cnt[8], av->txidle_psd_cnt[9]);
}

static void _bt_psd_aiso_sort(struct btc_t *btc, u32 *used,
            		      char *output, u32 out_len)
{
	struct btc_bt_psd_dm *bp = &btc->bt_psd_dm;
	struct btc_module *module = &btc->mdinfo;
	u8 i, rec_idx, cnt = bp->aiso_db_cnt;
	u8 max_aiso = 0, mid_cnt = 0, th = 3, min_lim = 0, max_lim = 0,
	   avg_sort_cnt = 0;
	u32 aiso_sort_tol = 0;

	if (cnt > 16)
		return;

	mid_cnt = cnt % 2 ? cnt : cnt + 1;

	hal_mem_cpy(btc->hal, bp->aiso_sort_db, bp->aiso_db, 16);

	//sort raw data of antenna-isolation results
	for (i = 0; i < cnt-1; i++) {
		for (rec_idx = 0; rec_idx < cnt-1-i; rec_idx++) {
			if (bp->aiso_sort_db[rec_idx] <=
			    bp->aiso_sort_db[rec_idx+1])
			    continue;

			max_aiso = bp->aiso_sort_db[rec_idx];
			bp->aiso_sort_db[rec_idx] = bp->aiso_sort_db[rec_idx+1];
			bp->aiso_sort_db[rec_idx+1] = max_aiso;
		}
	}

	min_lim = (bp->aiso_sort_db[(mid_cnt/2)-1]) - th;
	max_lim = (bp->aiso_sort_db[(mid_cnt/2)-1]) + th;
	// list the sorting data of antenna-isolation results
	for (rec_idx = 0; rec_idx < cnt; rec_idx++) {
		BTC_CNSL(out_len, *used, output + *used, out_len- *used,
			 " #%3d: ant_iso sort result = %3d dB\n",
			 rec_idx+1, bp->aiso_sort_db[rec_idx]);
	}

	// list mid data of sorting antenna-isolation results
	BTC_CNSL(out_len, *used, output + *used, out_len- *used,
		 " #%3d: ant_iso sort mid result = %3d dB\n",
		 (mid_cnt/2), bp->aiso_sort_db[(mid_cnt/2)-1]);

	// calculate average of sorting antenna-isolation results
	for (rec_idx = 0; rec_idx < cnt; rec_idx++) {
		if (bp->aiso_sort_db[rec_idx] >= min_lim &&
		    bp->aiso_sort_db[rec_idx] <= max_lim) {
			avg_sort_cnt ++;
			aiso_sort_tol += bp->aiso_sort_db[rec_idx];
		}
	}

	//list average result
	if (avg_sort_cnt != 0)
		bp->aiso_sort_avg = aiso_sort_tol / avg_sort_cnt;
	module->ant.isolation = bp->aiso_sort_avg;
	BTC_CNSL(out_len, *used, output + *used, out_len- *used,
		 " ant_iso sort avg result = %3d dB, avg_count = %3d\n",
		 bp->aiso_sort_avg, avg_sort_cnt);
}

static void _cmd_get_tx_pwr(struct btc_t *btc, u32 *used,
			    char input[][MAX_ARGV], u32 input_num,
			    char *output, u32 out_len)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	s8 txp_dbm = 127, txp_dbm_mhz = 127;

	txp_dbm_mhz = _bt_psd_get_tx_pwr(btc);
	txp_dbm = txp_dbm_mhz + _get_bw_att_db(btc);

	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "tx_rate=%s, ss=%d, txp=%d dBm, txp_MHz=%d dBm/MHz\n",
		 id_to_str(BTC_STR_RATE, (u32)bt_psd_dm->wl_tx_rate),
		 bt_psd_dm->wl_tx_ss, txp_dbm, txp_dbm_mhz);
}

static void _cmd_trig_aiso(struct btc_t *btc, u32 *used,
			   char input[][MAX_ARGV], u32 input_num,
			   char *output, u32 out_len)
{
	struct rtw_hal_com_t *h = btc->hal;
	struct btc_bt_psd_dm *bp = &btc->bt_psd_dm;
	u32 enable = 0, exec_cnt = 1;

	if (input_num >= 3)
		_os_sscanf(input[2], "%d", &enable);

	if (input_num > 3)
		_os_sscanf(input[3], "%d", &exec_cnt);

	if (input_num < 3 || exec_cnt == 0 || exec_cnt > 16) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " aiso 1 <0:force stop, 1:trig start>\n");
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " aiso 1 1 <execute count:1~16>\n");
		return;
	}

	if (enable && bp->en) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "ant isolation calculation is already running!!\n");
		return;
	}

	if (enable) {
		bp->aiso_data_ok = false;
		bp->rec_start = false;
		bp->aiso_cmd_cnt = (u8)exec_cnt;

		if (bp->aiso_cmd_cnt > 1) {
			bp->aiso_db_cnt = 0;
			hal_mem_set(h, bp->aiso_db, 0, sizeof(bp->aiso_db));
		}

		_bt_psd_rpt_ctrl(btc, true);

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "trig ant isolation calculation (exec_cnt=%d)!!\n",
			 bp->aiso_cmd_cnt);

	} else {
		_bt_psd_rpt_ctrl(btc, false);
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 "stop ant isolation calculation!!!!\n");
	}

	_run_coex(btc, __func__);
}

static void _cmd_get_aiso(struct btc_t *btc, u32 *used,
			  char input[][MAX_ARGV], u32 input_num,
			  char *output, u32 out_len)
{
	struct btc_bt_psd_dm *bt_psd_dm = &btc->bt_psd_dm;
	struct btc_aiso_val *av = &bt_psd_dm->aiso_val;
	u8 idx_offset, i, rec_idx, md, cnt;
	u32 debug_level = 0;

	if (input_num < 3) {
		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " aiso 0 <0:current result, 1:history result>\n");
		return;
	}

	_os_sscanf(input[2], "%d", &debug_level);

	if (!bt_psd_dm->aiso_data_ok) {
		if (bt_psd_dm->rec_time_out)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 " bt psd rpt record timeout!!\n");
		else if (!bt_psd_dm->en)
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 " not start to enable bt psd rpt!!\n");
		else
			BTC_CNSL(out_len, *used, output + *used,
				 out_len - *used,
				 " ant isolation is under calculation!!\n");

		return;
	}

	cnt = bt_psd_dm->aiso_db_cnt;

	if (cnt == 0) {
		BTC_CNSL(out_len, *used, output + *used, out_len- *used,
			 " no ant_iso result!!\n");
		return;
	}

	/* List the late antenna-isolation result */
	if (debug_level == 0) {
		rec_idx = (cnt - 1) & 0xF;
		BTC_CNSL(out_len, *used, output + *used, out_len- *used,
			 " current ant_iso result = %3d dB\n",
			 bt_psd_dm->aiso_db[rec_idx]);
		return;
	}

	/* List the latest 16 antenna-isolation results */
	idx_offset = (cnt < 16 ? 0 : cnt - 16);

	for (i = idx_offset; i < cnt; i++) {
		rec_idx = i & 0xF;
		BTC_CNSL(out_len, *used, output + *used, out_len- *used,
			 " #%3d: ant_iso result = %3d dB\n",
			 i+1, bt_psd_dm->aiso_db[rec_idx]);
	}

	_bt_psd_aiso_sort(btc, used, output, out_len);

	md = bt_psd_dm->aiso_method_final;
	BTC_CNSL(out_len, *used, output + *used, out_len -*used,
		 " psd_rpt: rec_cnt=%d, method=%d, ant_iso_db=%d (%d-(%d))dB\n",
		 av->psd_rec_cnt, md, av->aiso_md[md],
		 av->wl_air_psd_avg, av->rx_psd[md]);

	for (i = 0; i < BTC_AISO_M_MAX; i++) {

		if (i != BTC_AISO_M4 && i != BTC_AISO_M5 &&
		    i != BTC_AISO_M6 && i != BTC_AISO_M46_AVG)
			continue;

		BTC_CNSL(out_len, *used, output + *used, out_len - *used,
			 " ant_iso_md (m:%d) = %d (%d - (%d))dB\n", i,
			 av->aiso_md[i], av->wl_air_psd_avg, av->rx_psd[i]);
	}

	_bt_psd_show_record(btc, used, input, input_num, output, out_len);
}

static void _cmd_ant_iso(struct btc_t *btc, u32 *used,
			 char input[][MAX_ARGV], u32 input_num,
			 char *output, u32 out_len)
{
	u32 cmd_set = 0;

	if (input_num < 2)
		goto help;

	_os_sscanf(input[1], "%d", &cmd_set);

	switch (cmd_set) {
	case 0:
		_cmd_get_aiso(btc, used, input, input_num, output, out_len);
		break;
	case 1:
		_cmd_trig_aiso(btc, used, input, input_num, output, out_len);
		break;
	case 2:
		_cmd_get_tx_pwr(btc, used, input, input_num, output, out_len);
		break;
	default:
		goto help;
	}

	return;
help:
	BTC_CNSL(out_len, *used, output + *used, out_len - *used,
		 "aiso <0:show result 1:trig 2:show wl tx power>\n");
}
#else
void _bt_psd_setup(struct btc_t *btc, u8 start_idx, u8 rpt_type)
{
	u8 buf[2] = {0};

	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s(): set bt psd\n",
		  __func__);

	buf[0] = start_idx;
	buf[1] = rpt_type;
	hal_btc_fw_set_bt(btc, SET_BT_PSD_REPORT, 2, buf);
}

void _bt_psd_update(struct btc_t *btc, u8 *buf, u32 len)
{
	PHL_TRACE(COMP_PHL_BTC, _PHL_DEBUG_, "[BTC], %s():\n", __func__);
}
#endif

void halbtc_cmd_parser(struct btc_t *btc, char input[][MAX_ARGV],
		       u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	u8 id = 0;
	u32 i;
	u32 array_size = sizeof(halbtc_cmd_i) / sizeof(struct halbtc_cmd_info);

	BTC_CNSL(out_len, used, output + used, out_len - used, "\n");

	/* Parsing Cmd ID */
	if (input_num) {
		/* Avoid input number and debug cmd mismatch */
		input_num--;
		for (i = 0; i < array_size; i++) {
			if (_os_strcmp(halbtc_cmd_i[i].name, input[0]) == 0) {
				id = halbtc_cmd_i[i].id;
				break;
			}
		}
	}

	switch (id) {
	case HALBTC_DBG:
		_cmd_dbg(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SHOW:
		_cmd_show(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_WRITE_BT:
		_cmd_wb(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_READ_BT:
		_cmd_rb(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_COEX:
		_cmd_set_coex(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_UPDATE_POLICY:
		_cmd_upd_policy(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_TDMA:
		_cmd_tdma(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SLOT:
		_cmd_slot(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SIG_GDBG_EN:
		_cmd_sig_gdbg_en(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SGPIO_MAP:
		_cmd_sgpio_map(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_WL_TX_POWER:
		_cmd_wl_tx_power(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_WL_RX_LNA:
		_cmd_wl_rx_lna(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_AFH_MAP:
		_cmd_bt_afh_map(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_TX_POWER:
		_cmd_bt_tx_power(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_RX_LNA:
		_cmd_bt_rx_lna(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_BT_IGNO_WLAN:
		_cmd_bt_igno_wl(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_GNT_WL:
		_cmd_set_gnt_wl(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_GNT_BT:
		_cmd_set_gnt_bt(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_SET_BT_PSD:
		_cmd_set_bt_psd(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_GET_WL_NHM_DBM:
		_cmd_get_wl_nhm(btc, &used, input, input_num, output, out_len);
		break;
#ifdef BTC_FDDT_TRAIN_SUPPORT
	case HALBTC_FT_CTRL:
		_cmd_fddt_ctrl(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_FT_SET:
		_cmd_fddt_set(btc, &used, input, input_num, output, out_len);
		break;
	case HALBTC_FT_SHOW:
		_cmd_fddt_show(btc, &used, input, input_num, output, out_len);
		break;
#endif
#ifdef BTC_AISO_SUPPORT
	case HALBTC_AISO:
		_cmd_ant_iso(btc, &used, input, input_num, output, out_len);
		break;
#endif
	default:
		BTC_CNSL(out_len, used, output + used, out_len - used,
			 "command not supported !!\n");

	/* fall through */
	case HALBTC_HELP:
		BTC_CNSL(out_len, used, output + used, out_len - used,
			 "BTC cmd ==>\n");

		for (i = 0; i < array_size - 1; i++)
			BTC_CNSL(out_len, used, output + used, out_len - used,
				 " %s\n", halbtc_cmd_i[i + 1].name);
		break;
	}
}

s32 halbtc_cmd(struct btc_t *btc, char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = _os_strsep(&input, ", ");
		if (token) {
			if (_os_strlen((u8 *)token) <= MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);
#if 0
	if (argc == 1)
		argv[0][_os_strlen((u8 *)argv[0]) - 1] = '\0';
#endif

	halbtc_cmd_parser(btc, argv, argc, output, out_len);
	return 0;
}

#endif

