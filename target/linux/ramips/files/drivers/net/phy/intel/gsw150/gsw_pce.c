/*
 This file is provided under a dual BSD/GPLv2 license.  When using or
 redistributing this file, you may do so under either license.

 GPL LICENSE SUMMARY

 Copyright(c) 2016 - 2017 Intel Corporation.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 Contact Information:
  Intel Corporation
  2200 Mission College Blvd.
  Santa Clara, CA  97052

 BSD LICENSE

 Copyright(c) 2016 - 2017 Intel Corporation.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.
  * Neither the name of Intel Corporation nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/



#include "gsw_sw_init.h"

/**************************************************************************/
/*      EXTERNS Declaration:                                              */
/**************************************************************************/
extern GSW_return_t gsw_reg_rd(void *pdev, u16 os, u16 sf, u16 sz, ur *v);
extern GSW_return_t gsw_reg_wr(void *pdev, u16 os, u16 sf, u16 sz, u32 v);
GSW_return_t dasa_ip_msb_twr(void *pdev,
                             pcetbl_prog_t *tpt, ip_dasa_t *parm);
GSW_return_t ip_ll_twr(void *pdev, pctbl_prog_t *pt);
GSW_return_t ip_ll_trd(void *pdev, pctbl_prog_t *pt);
int pce_action_delete(void *pdev, tft_tbl_t *tpt, u32 index);
GSW_return_t gsw_uc_code_init(void *pdev);
extern GSW_return_t npport(void *pdev, u8 port);
extern GSW_return_t pctrl_bas_busy(void *pdev);
extern GSW_return_t pctrl_bas_set(void *pdev);
extern GSW_return_t pctrl_reg_clr(void *pdev);
extern GSW_return_t pctrl_addr_opmod(void *p, u16 ad, u16 op);
int pce_pattern_delete(void *pdev, tft_tbl_t *tpt, u32 index);
int pce_rule_read(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm);
int pce_rule_write(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm);
int find_msb_tbl_entry(pcetbl_prog_t *tpt, ip_dasa_t *parm);
int find_dasa_tbl_entry(pcetbl_prog_t *tpt, ip_dasa_t *parm);
int dasa_lsb_twr(void *pdev, pcetbl_prog_t *tpt, ip_dasa_t *parm);
int ip_dasa_msb_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
int ip_dasa_lsb_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
int ipdslsb_tblidx_del(pcetbl_prog_t *tpt, u32 index);
int ipdsmsb_tblidx_del(pcetbl_prog_t *tpt, u32 index);
int dasa_lsb_tbl_read(pcetbl_prog_t *tpt, int index, ip_dasa_t *parm);
/**************************************************************************/
/*      DEFINES:                                                          */
/**************************************************************************/
#define PCE_MC_M(val, msk, ns, out, len, type, flags, ipv4_len) \
	{ val, msk, (ns<<10 | out<<4 | len>>1),\
		(len&1)<<15 | type<<13 | flags<<9 | ipv4_len<<8 }
const PCE_MICROCODE pce_mc_sw2_3 = {
	/*-----------------------------------------------------------------*/
	/**   value    mask   ns  out_fields   L  type   flags   ipv4_len **/
	/*-----------------------------------------------------------------*/
	/* V22_2X (IPv6 issue fixed) */
	PCE_MC_M(0x88c3, 0xFFFF, 1, GOUT_ITAG0, 4, INSTR, GFLAG_ITAG, 0),
	PCE_MC_M(0x8100, 0xFFFF, 4, GOUT_STAG0, 2, INSTR, GFLAG_SVLAN, 0),
	PCE_MC_M(0x88A8, 0xFFFF, 4, GOUT_STAG0, 2, INSTR, GFLAG_SVLAN, 0),
	PCE_MC_M(0x9100, 0xFFFF, 4, GOUT_STAG0, 2, INSTR, GFLAG_SVLAN, 0),
	PCE_MC_M(0x8100, 0xFFFF, 5, GOUT_VTAG0, 2, INSTR, GFLAG_VLAN, 0),
	PCE_MC_M(0x88A8, 0xFFFF, 6, GOUT_VTAG0, 2, INSTR, GFLAG_VLAN, 0),
	PCE_MC_M(0x9100, 0xFFFF, 4, GOUT_VTAG0, 2, INSTR, GFLAG_VLAN, 0),
	PCE_MC_M(0x8864, 0xFFFF, 20, GOUT_ETHTYP, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0800, 0xFFFF, 24, GOUT_ETHTYP, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x86DD, 0xFFFF, 25, GOUT_ETHTYP, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x8863, 0xFFFF, 19, GOUT_ETHTYP, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0xF800, 13, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 44, GOUT_ETHTYP, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0600, 0x0600, 44, GOUT_ETHTYP, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 15, GOUT_NONE, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0xAAAA, 0xFFFF, 17, GOUT_NONE, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0300, 0xFF00, 45, GOUT_NONE, 0, INSTR, GFLAG_SNAP, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_DIP7, 3, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 21, GOUT_DIP7, 3, INSTR, GFLAG_PPPOE, 0),
	PCE_MC_M(0x0021, 0xFFFF, 24, GOUT_NONE, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0057, 0xFFFF, 25, GOUT_NONE, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 44, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x4000, 0xF000, 27, GOUT_IP0, 4, INSTR, GFLAG_IPV4, 1),
	PCE_MC_M(0x6000, 0xF000, 30, GOUT_IP0, 3, INSTR, GFLAG_IPV6, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 28, GOUT_IP3, 2, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 29, GOUT_SIP0, 4, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 44, GOUT_NONE, 0, LENACCU, GFLAG_NO, 0),
	PCE_MC_M(0x1100, 0xFF00, 43, GOUT_PROT, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0600, 0xFF00, 43, GOUT_PROT, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0xFF00, 36, GOUT_IP3, 17, INSTR, GFLAG_HOP, 0),
	PCE_MC_M(0x2B00, 0xFF00, 36, GOUT_IP3, 17, INSTR, GFLAG_NN1, 0),
	PCE_MC_M(0x3C00, 0xFF00, 36, GOUT_IP3, 17, INSTR, GFLAG_NN2, 0),
	PCE_MC_M(0x0000, 0x0000, 43, GOUT_PROT, 1, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x00F0, 38, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 44, GOUT_NONE, 0, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0xFF00, 36, GOUT_NONE, 0, IPV6, GFLAG_HOP, 0),
	PCE_MC_M(0x2B00, 0xFF00, 36, GOUT_NONE, 0, IPV6, GFLAG_NN1, 0),
	PCE_MC_M(0x3C00, 0xFF00, 36, GOUT_NONE, 0, IPV6, GFLAG_NN2, 0),
	PCE_MC_M(0x0000, 0x00FC, 44, GOUT_PROT, 0, IPV6, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 44, GOUT_NONE, 0, IPV6, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 44, GOUT_SIP0, 16, INSTR, GFLAG_NO, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_APP0, 4, INSTR, GFLAG_IGMP, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
	PCE_MC_M(0x0000, 0x0000, 45, GOUT_NONE, 0, INSTR, GFLAG_END, 0),
};
/**************************************************************************/
/*      LOCAL DECLARATIONS:                                               */
/**************************************************************************/
static int tbl_write(void *ts, u8 *rcnt, void *parm, u32 tsize, u32 tnum);
static int find_tbl_idx(void *ts, u8 *rcnt, void *parm, u32 tsize, u32 tnum);
static int tbl_idx_delete(u8 *rcnt, u32 index, u32 tsize);
static GSW_return_t parser_ll_twr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t actvlan_ll_twr(void *pdev, pctbl_prog_t *pt);
int actvlan_twr(void *pdev,
                pcetbl_prog_t *tpt, avlan_tbl_t *parm, int rf, u16 rv);
static GSW_return_t actvlan_ll_trd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t pppoe_ll_twr(void *pdev, pctbl_prog_t *pt);
static int pppoe_trd(pcetbl_prog_t *tpt, int index, ppoe_tbl_t *parm);
static int pppoe_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
static GSW_return_t protocol_ll_twr(void *pdev, pctbl_prog_t *pt);
/*
static GSW_return_t ip_msb_ll_tbl_wr(void *pdev, pctbl_prog_t *pt);
*/
static GSW_return_t ip_plen_ll_tbl_wr(void *pdev, pctbl_prog_t *pt);
/*
static GSW_return_t ip_plen_ll_tbl_rd(void *pdev, pctbl_prog_t *pt);
*/
static GSW_return_t tflow_tbl_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t tflow_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t mac_dasa_ll_twr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t app_ll_twr(void *pdev, pctbl_prog_t *pt);
static int pkg_len_twr(void *pdev, pcetbl_prog_t *pst, pkt_len_t *parm);
static int dst_mac_twr(void *pdev, pcetbl_prog_t *pst, mac_tbl_t *parm);
static int src_mac_twr(void *pdev, pcetbl_prog_t *pst, mac_tbl_t *parm);
static int app_twr(void *pdev, pcetbl_prog_t *tpt, app_tbl_t *parm);
static int pkt_len_tdel(void *pdev, pcetbl_prog_t *pst, u32 index);
static int pkg_lng_tbl_read(pcetbl_prog_t *pt,
                            int index, pkt_len_t *parm);
static int src_mac_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
static int dst_mac_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
static int da_mac_trd(pcetbl_prog_t *tpt, int index, mac_tbl_t *parm);
static int sa_mac_tbl_read(pcetbl_prog_t *tpt, int index, mac_tbl_t *parm);
static int app_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
static int app_trd(pcetbl_prog_t *tpt, int index, app_tbl_t *parm);
static int dasa_msb_tbl_read(pcetbl_prog_t *tpt, int index, ip_dasa_t *parm);
static int get_tbl_index(void *ts, void *parm, u32 tsize, u32 tnum);
static int protocol_twr(void *pdev, pcetbl_prog_t *tpt, protocol_tb_t *parm);
int gavlan_tbl_index(pcetbl_prog_t *tpt, u8 index);
static int ptcl_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
static int ptcl_tbl_read(pcetbl_prog_t *tpt, int index, protocol_tb_t *parm);
static int pppoe_twr(void *pdev, pcetbl_prog_t *tpt, ppoe_tbl_t *parm);
static int act_vlan_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);

/**************************************************************************/
/*      LOCAL FUNCTIONS Implementation::                                  */
/**************************************************************************/
/* Static Function Declaration */
static int find_tbl_idx(void *ts, u8 *rcnt,
                        void *parm, u32 tsize, u32 tnum)
{
	int i;
	/* search if the entry is already available and can be re-used */
	for (i = 0; i < tnum; i++) {
		if (rcnt[i] > 0) {
			/* entry is used, check if the entry content fits */
			if (memcmp(((char *)ts) + i * tsize, parm, (u8)tsize) == 0) {
				rcnt[i]++;
				return i;
			}
		}
	}
	return tnum;
}

static int tbl_write(void *ts, u8 *rcnt,
                     void *parm, u32 tsize, u32 tnum)
{
	int i;
	/* find an empty entry and add information */
	for (i = 0; i < tnum; i++) {
		if (rcnt[i] == 0) {
			memcpy(((char *)ts) + i * tsize, parm, (u8)tsize);
			rcnt[i]++;
			return i;
		}
	}
	/* table is full, return an error */
	GSW_PRINT("ERROR:\n\tFile %s\n\tLine %d\n", __FILE__, __LINE__);
	return GSW_statusTblFull;
}

static GSW_return_t tbl_idx_delete(u8 *rcnt,
                                   u32 index, u32 tsize)
{
	PCE_ASSERT(index >= tsize);
	if (rcnt[index] > 0)
		rcnt[index]--;
	return 0;
}

static GSW_return_t parser_ll_twr(void *pdev,
                                  pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_3_VAL3_OFFSET,
	               PCE_TBL_VAL_3_VAL3_SHIFT,
	               PCE_TBL_VAL_3_VAL3_SIZE,
	               pt->val[3]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_2_VAL2_OFFSET,
	               PCE_TBL_VAL_2_VAL2_SHIFT,
	               PCE_TBL_VAL_2_VAL2_SIZE,
	               pt->val[2]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_1_VAL1_OFFSET,
	               PCE_TBL_VAL_1_VAL1_SHIFT,
	               PCE_TBL_VAL_1_VAL1_SIZE,
	               pt->val[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               pt->val[0]);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

static GSW_return_t actvlan_ll_twr(void *pdev,
                                   pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               pt->val[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               pt->type);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

static GSW_return_t actvlan_ll_trd(void *pdev,
                                   pctbl_prog_t *pt)
{
	GSW_return_t s;
	ur r;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->mask[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->type = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	return GSW_statusOk;
}

static GSW_return_t pppoe_ll_twr(void *pdev,
                                 pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

static GSW_return_t protocol_ll_twr(void *pdev,
                                    pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
/*
static GSW_return_t ip_msb_ll_tbl_wr(void *pdev,
	pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	PCE_TBL_ADDR_ADDR_OFFSET,
		PCE_TBL_ADDR_ADDR_SHIFT,
		PCE_TBL_ADDR_ADDR_SIZE,
		pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	PCE_TBL_KEY_3_KEY3_OFFSET,
		PCE_TBL_KEY_3_KEY3_SHIFT,
		PCE_TBL_KEY_3_KEY3_SIZE,
		pt->key[3]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, PCE_TBL_KEY_2_KEY2_OFFSET,
		PCE_TBL_KEY_2_KEY2_SHIFT, PCE_TBL_KEY_2_KEY2_SIZE,
		pt->key[2]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, PCE_TBL_KEY_1_KEY1_OFFSET,
		PCE_TBL_KEY_1_KEY1_SHIFT, PCE_TBL_KEY_1_KEY1_SIZE,
		pt->key[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, PCE_TBL_KEY_0_KEY0_OFFSET,
		PCE_TBL_KEY_0_KEY0_SHIFT, PCE_TBL_KEY_0_KEY0_SIZE,
		pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, PCE_TBL_MASK_0_MASK0_OFFSET,
		PCE_TBL_MASK_0_MASK0_SHIFT, PCE_TBL_MASK_0_MASK0_SIZE,
		pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, PCE_TBL_CTRL_VLD_OFFSET,
		PCE_TBL_CTRL_VLD_SHIFT, PCE_TBL_CTRL_VLD_SIZE,
		pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
*/
GSW_return_t ip_ll_twr(void *pdev,
                       pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_3_KEY3_OFFSET,
	               PCE_TBL_KEY_3_KEY3_SHIFT,
	               PCE_TBL_KEY_3_KEY3_SIZE,
	               pt->key[3]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_2_KEY2_OFFSET,
	               PCE_TBL_KEY_2_KEY2_SHIFT,
	               PCE_TBL_KEY_2_KEY2_SIZE,
	               pt->key[2]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_1_KEY1_OFFSET,
	               PCE_TBL_KEY_1_KEY1_SHIFT,
	               PCE_TBL_KEY_1_KEY1_SIZE,
	               pt->key[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t ip_ll_trd(void *pdev, pctbl_prog_t *pt)
{
	ur r;
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_3_KEY3_OFFSET,
	               PCE_TBL_KEY_3_KEY3_SHIFT,
	               PCE_TBL_KEY_3_KEY3_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[3] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_2_KEY2_OFFSET,
	               PCE_TBL_KEY_2_KEY2_SHIFT,
	               PCE_TBL_KEY_2_KEY2_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[2] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_1_KEY1_OFFSET,
	               PCE_TBL_KEY_1_KEY1_SHIFT,
	               PCE_TBL_KEY_1_KEY1_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[1] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->mask[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->type = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_GMAP_OFFSET,
	               PCE_TBL_CTRL_GMAP_SHIFT,
	               PCE_TBL_CTRL_GMAP_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->group = r;
	return GSW_statusOk;
}

static GSW_return_t ip_plen_ll_tbl_wr(void *pdev,
                                      pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE, pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
/*
static GSW_return_t ip_plen_ll_tbl_rd(void *pdev,
	pctbl_prog_t *pt)
{
	ur r;
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
		PCE_TBL_ADDR_ADDR_OFFSET,
		PCE_TBL_ADDR_ADDR_SHIFT,
		PCE_TBL_ADDR_ADDR_SIZE,
		pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table,
		pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
		PCE_TBL_KEY_0_KEY0_OFFSET,
		PCE_TBL_KEY_0_KEY0_SHIFT,
		PCE_TBL_KEY_0_KEY0_SIZE,
		&r);
	if (s != GSW_statusOk)
		return s;
	pt->key[0] = r;
	s = gsw_reg_rd(pdev,
		PCE_TBL_MASK_0_MASK0_OFFSET,
		PCE_TBL_MASK_0_MASK0_SHIFT,
		PCE_TBL_MASK_0_MASK0_SIZE,
		&r);
	if (s != GSW_statusOk)
		return s;
	pt->mask[0] = r;
	s = gsw_reg_rd(pdev,
		PCE_TBL_CTRL_VLD_OFFSET,
		PCE_TBL_CTRL_VLD_SHIFT,
		PCE_TBL_CTRL_VLD_SIZE,
		&r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	return GSW_statusOk;
}
*/
static GSW_return_t tflow_tbl_wr(void *pdev,
                                 pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_8_KEY8_OFFSET,
	               PCE_TBL_KEY_8_KEY8_SHIFT,
	               PCE_TBL_KEY_8_KEY8_SIZE,
	               pt->key[8]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_7_KEY7_OFFSET,
	               PCE_TBL_KEY_7_KEY7_SHIFT,
	               PCE_TBL_KEY_7_KEY7_SIZE,
	               pt->key[7]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_6_KEY6_OFFSET,
	               PCE_TBL_KEY_6_KEY6_SHIFT,
	               PCE_TBL_KEY_6_KEY6_SIZE,
	               pt->key[6]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_5_KEY5_OFFSET,
	               PCE_TBL_KEY_5_KEY5_SHIFT,
	               PCE_TBL_KEY_5_KEY5_SIZE,
	               pt->key[5]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_4_KEY4_OFFSET,
	               PCE_TBL_KEY_4_KEY4_SHIFT,
	               PCE_TBL_KEY_4_KEY4_SIZE,
	               pt->key[4]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_3_KEY3_OFFSET,
	               PCE_TBL_KEY_3_KEY3_SHIFT,
	               PCE_TBL_KEY_3_KEY3_SIZE,
	               pt->key[3]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_2_KEY2_OFFSET,
	               PCE_TBL_KEY_2_KEY2_SHIFT,
	               PCE_TBL_KEY_2_KEY2_SIZE,
	               pt->key[2]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_1_KEY1_OFFSET,
	               PCE_TBL_KEY_1_KEY1_SHIFT,
	               PCE_TBL_KEY_1_KEY1_SIZE,
	               pt->key[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_6_VAL6_OFFSET,
	               PCE_TBL_VAL_6_VAL6_SHIFT,
	               PCE_TBL_VAL_6_VAL6_SIZE,
	               pt->val[6]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_5_VAL5_OFFSET,
	               PCE_TBL_VAL_5_VAL5_SHIFT,
	               PCE_TBL_VAL_5_VAL5_SIZE,
	               pt->val[5]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_4_VAL4_OFFSET,
	               PCE_TBL_VAL_4_VAL4_SHIFT,
	               PCE_TBL_VAL_4_VAL4_SIZE,
	               pt->val[4]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_3_VAL3_OFFSET,
	               PCE_TBL_VAL_3_VAL3_SHIFT,
	               PCE_TBL_VAL_3_VAL3_SIZE,
	               pt->val[3]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_2_VAL2_OFFSET,
	               PCE_TBL_VAL_2_VAL2_SHIFT,
	               PCE_TBL_VAL_2_VAL2_SIZE,
	               pt->val[2]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_1_VAL1_OFFSET,
	               PCE_TBL_VAL_1_VAL1_SHIFT,
	               PCE_TBL_VAL_1_VAL1_SIZE,
	               pt->val[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               pt->val[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_KEYFORM_OFFSET,
	               PCE_TBL_CTRL_KEYFORM_SHIFT,
	               PCE_TBL_CTRL_KEYFORM_SIZE,
	               pt->kformat);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               pt->type);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_GMAP_OFFSET,
	               PCE_TBL_CTRL_GMAP_SHIFT,
	               PCE_TBL_CTRL_GMAP_SIZE,
	               pt->group);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

static GSW_return_t tflow_tbl_rd(void *pdev,
                                 pctbl_prog_t *pt)
{
	ur r;
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_8_KEY8_OFFSET,
	               PCE_TBL_KEY_8_KEY8_SHIFT,
	               PCE_TBL_KEY_8_KEY8_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[8] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_7_KEY7_OFFSET,
	               PCE_TBL_KEY_7_KEY7_SHIFT,
	               PCE_TBL_KEY_7_KEY7_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[7] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_6_KEY6_OFFSET,
	               PCE_TBL_KEY_6_KEY6_SHIFT,
	               PCE_TBL_KEY_6_KEY6_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[6] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_5_KEY5_OFFSET,
	               PCE_TBL_KEY_5_KEY5_SHIFT,
	               PCE_TBL_KEY_5_KEY5_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[5] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_4_KEY4_OFFSET,
	               PCE_TBL_KEY_4_KEY4_SHIFT,
	               PCE_TBL_KEY_4_KEY4_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[4] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_3_KEY3_OFFSET,
	               PCE_TBL_KEY_3_KEY3_SHIFT,
	               PCE_TBL_KEY_3_KEY3_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[3] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_2_KEY2_OFFSET,
	               PCE_TBL_KEY_2_KEY2_SHIFT,
	               PCE_TBL_KEY_2_KEY2_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[2] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_1_KEY1_OFFSET,
	               PCE_TBL_KEY_1_KEY1_SHIFT,
	               PCE_TBL_KEY_1_KEY1_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[1] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_6_VAL6_OFFSET,
	               PCE_TBL_VAL_6_VAL6_SHIFT,
	               PCE_TBL_VAL_6_VAL6_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[6] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_5_VAL5_OFFSET,
	               PCE_TBL_VAL_5_VAL5_SHIFT,
	               PCE_TBL_VAL_5_VAL5_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[5] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_4_VAL4_OFFSET,
	               PCE_TBL_VAL_4_VAL4_SHIFT,
	               PCE_TBL_VAL_4_VAL4_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[4] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_3_VAL3_OFFSET,
	               PCE_TBL_VAL_3_VAL3_SHIFT,
	               PCE_TBL_VAL_3_VAL3_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[3] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_2_VAL2_OFFSET,
	               PCE_TBL_VAL_2_VAL2_SHIFT,
	               PCE_TBL_VAL_2_VAL2_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[2] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_1_VAL1_OFFSET,
	               PCE_TBL_VAL_1_VAL1_SHIFT,
	               PCE_TBL_VAL_1_VAL1_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[1] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->mask[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->type = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_GMAP_OFFSET,
	               PCE_TBL_CTRL_GMAP_SHIFT,
	               PCE_TBL_CTRL_GMAP_SIZE,
	               &r);
	if (s != GSW_statusOk)
		return s;
	pt->group = r;
	return GSW_statusOk;
}

static GSW_return_t mac_dasa_ll_twr(void *pdev,
                                    pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_1_KEY1_OFFSET,
	               PCE_TBL_KEY_1_KEY1_SHIFT,
	               PCE_TBL_KEY_1_KEY1_SIZE,
	               pt->key[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_2_KEY2_OFFSET,
	               PCE_TBL_KEY_2_KEY2_SHIFT,
	               PCE_TBL_KEY_2_KEY2_SIZE,
	               pt->key[2]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_KEYFORM_OFFSET,
	               PCE_TBL_CTRL_KEYFORM_SHIFT,
	               PCE_TBL_CTRL_KEYFORM_SIZE,
	               pt->kformat);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               pt->type);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_GMAP_OFFSET,
	               PCE_TBL_CTRL_GMAP_SHIFT,
	               PCE_TBL_CTRL_GMAP_SIZE,
	               pt->group);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

static GSW_return_t app_ll_twr(void *pdev,
                               pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE,
	               pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE,
	               pt->mask[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_TYPE_OFFSET,
	               PCE_TBL_CTRL_TYPE_SHIFT,
	               PCE_TBL_CTRL_TYPE_SIZE,
	               pt->type);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE,
	               pt->valid);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

/* Packet Length Table write */
static int pkg_len_twr(void *pdev,
                       pcetbl_prog_t *pst, pkt_len_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret;
	ret = find_tbl_idx(pst->ct_pkg_lng, pst->trc_pkg_lng,
	                   parm, sizeof(pkt_len_t), PKG_LNG_TSIZE);
	if (ret == PKG_LNG_TSIZE) {
		ret = tbl_write(pst->ct_pkg_lng, pst->trc_pkg_lng,
		                parm, sizeof(pkt_len_t), PKG_LNG_TSIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_PACKET_INDEX;
		pt.pcindex = ret;
		pt.key[0]	= parm->pkt_len;
		pt.mask[0] = parm->pkt_len_range;
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = ip_plen_ll_tbl_wr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* MAC DA Table index write */
static int dst_mac_twr(void *pdev,
                       pcetbl_prog_t *pst, mac_tbl_t *parm)
{
	pctbl_prog_t pt;
	GSW_return_t s;
	int ret;
	ret = find_tbl_idx(pst->da_mac_tbl, pst->dst_mac_cnt,
	                   parm, sizeof(mac_tbl_t), DASA_MAC_TBL_SIZE);
	if (ret == DASA_MAC_TBL_SIZE) {
		ret = tbl_write(pst->da_mac_tbl, pst->dst_mac_cnt,
		                parm, sizeof(mac_tbl_t), DASA_MAC_TBL_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_MACDA_INDEX;
		pt.pcindex = ret;
		pt.key[0] = (parm->mac[4] << 8 | parm->mac[5]);
		pt.key[1] = (parm->mac[2] << 8 | parm->mac[3]);
		pt.key[2] = (parm->mac[0] << 8 | parm->mac[1]);
		pt.mask[0] = parm->mmask;
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = mac_dasa_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* MAC SA Table index write */
static int src_mac_twr(void *pdev,
                       pcetbl_prog_t *pst, mac_tbl_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret;
	ret = find_tbl_idx(pst->ct_src_mac, pst->src_mac_cnt,
	                   parm, sizeof(mac_tbl_t), DASA_MAC_TBL_SIZE);
	if (ret == DASA_MAC_TBL_SIZE) {
		ret = tbl_write(pst->ct_src_mac, pst->src_mac_cnt,
		                parm, sizeof(mac_tbl_t), DASA_MAC_TBL_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_MACSA_INDEX;
		pt.pcindex = ret;
		pt.key[0] = (parm->mac[4] << 8 | parm->mac[5]);
		pt.key[1] = (parm->mac[2] << 8 | parm->mac[3]);
		pt.key[2] = (parm->mac[0] << 8 | parm->mac[1]);
		pt.mask[0] = parm->mmask;
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = mac_dasa_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* Application Table write */
static int app_twr(void *pdev,
                   pcetbl_prog_t *tpt, app_tbl_t *parm)
{
	pctbl_prog_t pt;
	GSW_return_t s;
	int ret;
	ret = find_tbl_idx(tpt->appl_tbl, tpt->appl_tbl_cnt,
	                   parm, sizeof(app_tbl_t), PCE_APPL_TBL_SIZE);
	if (ret == PCE_APPL_TBL_SIZE) {
		ret = tbl_write(tpt->appl_tbl, tpt->appl_tbl_cnt,
		                parm, sizeof(app_tbl_t), PCE_APPL_TBL_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table	= PCE_APPLICATION_INDEX;
		pt.pcindex	= ret;
		pt.key[0] = parm->app_data;
		pt.mask[0] = parm->app_mr_data;
		pt.type = parm->app_mr_sel;
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = app_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* IP DA/SA msb Table write */
GSW_return_t dasa_ip_msb_twr(void *pdev,
                             pcetbl_prog_t *tpt, ip_dasa_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret, i;
	ret = find_tbl_idx(tpt->ip_dasa_msb_tbl, tpt->ipmsbtcnt,
	                   parm, sizeof(ip_dasa_t), IP_DASA_MSB_SIZE);
	if (ret == IP_DASA_MSB_SIZE) {
		ret = tbl_write(tpt->ip_dasa_msb_tbl, tpt->ipmsbtcnt,
		                parm, sizeof(ip_dasa_t), IP_DASA_MSB_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_IP_DASA_MSB_INDEX;
		pt.pcindex	= ret;
		for (i = 0; i < 4; i++)
			pt.key[i] = ((parm->ip_val[((i*2)+1)] << 8)
			             | parm->ip_val[(i*2)]);
		pt.mask[0] = parm->ip_mask[0];
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = ip_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* Packet Length Table delete */
static int pkt_len_tdel(void *pdev,
                        pcetbl_prog_t *pst, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= PKG_LNG_TSIZE);
	if (pst->trc_pkg_lng[index] > 0) {
		pst->trc_pkg_lng[index]--;
		if (pst->trc_pkg_lng[index] == 0) {
			memset((((char *)pst->ct_pkg_lng)
			        + (index * sizeof(pkt_len_t))),
			       0, sizeof(pkt_len_t));
			/* initialize the data structure before using it */
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_PACKET_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = ip_plen_ll_tbl_wr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* Packet Length Table read */
static int pkg_lng_tbl_read(pcetbl_prog_t *pt,
                            int index, pkt_len_t *parm)
{
	PCE_ASSERT(index >= PKG_LNG_TSIZE);
	memcpy(parm, &pt->ct_pkg_lng[index], sizeof(pkt_len_t));
	return 0;
}

/* MAC SA Table delete */
static int src_mac_tdel(void *pdev,
                        pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= DASA_MAC_TBL_SIZE);
	if (tpt->src_mac_cnt[index] > 0) {
		tpt->src_mac_cnt[index]--;
		if (tpt->src_mac_cnt[index] == 0) {
			memset((((char *)tpt->ct_src_mac)
			        + (index * sizeof(mac_tbl_t))),
			       0, sizeof(mac_tbl_t));
			/* initialize the data structure before using it */
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_MACSA_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = mac_dasa_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* MAC DA Table delete */
static int dst_mac_tdel(void *pdev,
                        pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= DASA_MAC_TBL_SIZE);
	if (tpt->dst_mac_cnt[index] > 0) {
		tpt->dst_mac_cnt[index]--;
		if (tpt->dst_mac_cnt[index] == 0) {
			memset((((char *)tpt->da_mac_tbl)
			        + (index * sizeof(mac_tbl_t))),
			       0, sizeof(mac_tbl_t));
			/* initialize the data structure before using it */
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_MACDA_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = mac_dasa_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* MAC DA Table Read */
static int da_mac_trd(pcetbl_prog_t *tpt,
                      int index, mac_tbl_t *parm)
{
	PCE_ASSERT(index >= DASA_MAC_TBL_SIZE);
	memcpy(parm, &tpt->da_mac_tbl[index], sizeof(mac_tbl_t));
	return 0;
}

/* MAC SA Table Read */
static int sa_mac_tbl_read(pcetbl_prog_t *tpt,
                           int index, mac_tbl_t *parm)
{
	PCE_ASSERT(index >= DASA_MAC_TBL_SIZE);
	memcpy(parm, &tpt->ct_src_mac[index], sizeof(mac_tbl_t));
	return 0;
}

/* Application Table Delete */
static int app_tdel(void *pdev,
                    pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= PCE_APPL_TBL_SIZE);
	if (tpt->appl_tbl_cnt[index] > 0) {
		tpt->appl_tbl_cnt[index]--;
		if (tpt->appl_tbl_cnt[index] == 0) {
			memset((((char *)tpt->appl_tbl)
			        + (index * sizeof(app_tbl_t))),
			       0, sizeof(app_tbl_t));
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table	= PCE_APPLICATION_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = app_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* Application Table Read */
static int app_trd(pcetbl_prog_t *tpt,
                   int index, app_tbl_t *parm)
{
	PCE_ASSERT(index >= PCE_APPL_TBL_SIZE);
	memcpy(parm, &tpt->appl_tbl[index], sizeof(app_tbl_t));
	return 0;
}

/* IP DA/SA msb Table delete */
int ip_dasa_msb_tdel(void *pdev,
                     pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= IP_DASA_MSB_SIZE);
	if (tpt->ipmsbtcnt[index] > 0) {
		tpt->ipmsbtcnt[index]--;
		if (tpt->ipmsbtcnt[index] == 0) {
			memset((((char *)tpt->ip_dasa_msb_tbl)
			        + (index * sizeof(ip_dasa_t))),
			       0, sizeof(ip_dasa_t));
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_IP_DASA_MSB_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = ip_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* IP DA/SA msb Table read */
static int dasa_msb_tbl_read(pcetbl_prog_t *tpt,
                             int index, ip_dasa_t *parm)
{
	PCE_ASSERT(index >= IP_DASA_MSB_SIZE);
	memcpy(parm, &tpt->ip_dasa_msb_tbl[index], sizeof(ip_dasa_t));
	return 0;
}

static int get_tbl_index(void *ts, void *parm,
                         u32 tsize, u32 tnum)
{
	int i;
	/* search if the entry is already available and can be re-used */
	for (i = 0; i < tnum; i++) {
		/* entry is used, check if the entry content fits */
		if (memcmp(((char *)ts) + i * tsize, parm, (u8)tsize) == 0)
			return i;
	}
	return 0xFF;
}

/* Static Function Declaration */
int find_dasa_tbl_entry(pcetbl_prog_t *tpt,
                        ip_dasa_t *parm)
{
	return get_tbl_index(tpt->ip_dasa_lsb_tbl, parm,
	                     sizeof(ip_dasa_t), IP_DASA_LSB_SIZE);
}

/* Static Function Declaration */
int find_msb_tbl_entry(pcetbl_prog_t *tpt,
                       ip_dasa_t *parm)
{
	return get_tbl_index(tpt->ip_dasa_msb_tbl, parm,
	                     sizeof(ip_dasa_t), IP_DASA_MSB_SIZE);
}

/* IP DA/SA lsb Table Write */
int dasa_lsb_twr(void *pdev,
                 pcetbl_prog_t *tpt, ip_dasa_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret, i;
	ret = find_tbl_idx(tpt->ip_dasa_lsb_tbl, tpt->iplsbtcnt,
	                   parm, sizeof(ip_dasa_t), IP_DASA_LSB_SIZE);
	if (ret == IP_DASA_LSB_SIZE) {
		ret = tbl_write(tpt->ip_dasa_lsb_tbl, tpt->iplsbtcnt,
		                parm, sizeof(ip_dasa_t), IP_DASA_LSB_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_IP_DASA_LSB_INDEX;
		pt.pcindex = ret;
		for (i = 0; i < 4; i++)
			pt.key[i]	= ((parm->ip_val[((i*2)+1)] << 8)
			               | parm->ip_val[(i*2)]);
		pt.mask[0] = parm->ip_mask[0];
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = ip_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* IP DA/SA lsb Table delete */
int ip_dasa_lsb_tdel(void *pdev,
                     pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= IP_DASA_LSB_SIZE);
	if (tpt->iplsbtcnt[index] > 0) {
		tpt->iplsbtcnt[index]--;
		if (tpt->iplsbtcnt[index] == 0) {
			memset((((char *)tpt->ip_dasa_lsb_tbl)
			        + (index * sizeof(ip_dasa_t))),
			       0, sizeof(ip_dasa_t));
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_IP_DASA_LSB_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = ip_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* IP DA/SA lsb Table index delete */
int ipdslsb_tblidx_del(pcetbl_prog_t *tpt,
                       u32 index)
{
	return tbl_idx_delete(tpt->iplsbtcnt,
	                      index, IP_DASA_LSB_SIZE);
}

/* IP DA/SA msb Table index delete */
int ipdsmsb_tblidx_del(pcetbl_prog_t *tpt,
                       u32 index)
{
	return tbl_idx_delete(tpt->ipmsbtcnt,
	                      index, IP_DASA_MSB_SIZE);
}

/* IP DA/SA lsb Table read */
int dasa_lsb_tbl_read(pcetbl_prog_t *tpt,
                      int index, ip_dasa_t *parm)
{
	PCE_ASSERT(index >= IP_DASA_LSB_SIZE);
	memcpy(parm, &tpt->ip_dasa_lsb_tbl[index], sizeof(ip_dasa_t));
	return 0;
}

/* Protocal Table write */
static int protocol_twr(void *pdev,
                        pcetbl_prog_t *tpt, protocol_tb_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret;
	ret = find_tbl_idx(tpt->ptcl_tbl, tpt->ptcl_tbl_cnt,
	                   parm, sizeof(protocol_tb_t), PCE_PTCL_TBL_SIZE);
	if (ret == PCE_PTCL_TBL_SIZE) {
		ret = tbl_write(tpt->ptcl_tbl, tpt->ptcl_tbl_cnt,
		                parm, sizeof(protocol_tb_t), PCE_PTCL_TBL_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_PROTOCOL_INDEX;
		pt.pcindex = ret;
		pt.key[0] = parm->protocol_type;
		pt.mask[0] = parm->protocol_mask;
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = protocol_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* Get the vlan flow table index*/
int gavlan_tbl_index(pcetbl_prog_t *tpt, u8 index)
{
	PCE_ASSERT(index >= PCE_VLAN_ACT_TBL_SIZE);
	if (tpt->vlan_act_tbl_cnt[index] == 0)
		return 0;
	else
		return -1;
}

/* Protocal Table delete */
static int ptcl_tdel(void *pdev,
                     pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= PCE_PTCL_TBL_SIZE);
	if (tpt->ptcl_tbl_cnt[index] > 0) {
		tpt->ptcl_tbl_cnt[index]--;
		if (tpt->ptcl_tbl_cnt[index] == 0) {
			memset((((char *)tpt->ptcl_tbl)
			        + (index * sizeof(protocol_tb_t))),
			       0, sizeof(protocol_tb_t));
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_PROTOCOL_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = protocol_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* Protocal Table Read */
static int ptcl_tbl_read(pcetbl_prog_t *tpt,
                         int index, protocol_tb_t *parm)
{
	PCE_ASSERT(index >= PCE_PTCL_TBL_SIZE);
	memcpy(parm, &tpt->ptcl_tbl[index], sizeof(protocol_tb_t));
	return 0;
}

/* PPPoE Table Write */
static int pppoe_twr(void *pdev,
                     pcetbl_prog_t *tpt, ppoe_tbl_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret;
	ret = find_tbl_idx(tpt->pppoe_tbl, tpt->pppoe_tbl_cnt,
	                   parm,	sizeof(ppoe_tbl_t), PCE_PPPOE_TBL_SIZE);
	if (ret == PCE_PPPOE_TBL_SIZE) {
		ret = tbl_write(tpt->pppoe_tbl, tpt->pppoe_tbl_cnt,
		                parm,	sizeof(ppoe_tbl_t), PCE_PPPOE_TBL_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_PPPOE_INDEX;
		pt.pcindex = ret;
		pt.key[0] = parm->ppsoe_sid;
		pt.valid = 1;
		pt.opmode = PCE_OPMODE_ADWR;
		s = pppoe_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

/* PPPoE Table Delete */
static int pppoe_tdel(void *pdev,
                      pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= PCE_PPPOE_TBL_SIZE);
	if (tpt->pppoe_tbl_cnt[index] > 0) {
		tpt->pppoe_tbl_cnt[index]--;
		if (tpt->pppoe_tbl_cnt[index] == 0) {
			memset((((char *)tpt->pppoe_tbl)
			        + (index * sizeof(ppoe_tbl_t))),
			       0, sizeof(ppoe_tbl_t));
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_PPPOE_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = pppoe_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

/* PPPoE Table Read */
static int pppoe_trd(pcetbl_prog_t *tpt,
                     int index, ppoe_tbl_t *parm)
{
	PCE_ASSERT(index >= PCE_PPPOE_TBL_SIZE);
	memcpy(parm, &tpt->pppoe_tbl[index], sizeof(ppoe_tbl_t));
	return 0;
}

/* VLAN Table Delete */
static int act_vlan_tdel(void *pdev,
                         pcetbl_prog_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= PCE_VLAN_ACT_TBL_SIZE);
	if (tpt->vlan_act_tbl_cnt[index] > 0) {
		tpt->vlan_act_tbl_cnt[index]--;
		if (tpt->vlan_act_tbl_cnt[index] == 0) {
			memset((((char *)tpt->avlan_tbl)
			        + (index * sizeof(avlan_tbl_t))),
			       0, sizeof(avlan_tbl_t));
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_ACTVLAN_INDEX;
			pt.pcindex = index;
			pt.opmode = PCE_OPMODE_ADWR;
			s = actvlan_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return 0;
}

int actvlan_twr(void *pdev, pcetbl_prog_t *tpt,
                avlan_tbl_t *parm, int range_flag, u16 range_val)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	int ret;
	ret = find_tbl_idx(tpt->avlan_tbl, tpt->vlan_act_tbl_cnt,
	                   parm, sizeof(avlan_tbl_t), PCE_VLAN_ACT_TBL_SIZE);
	if (ret == PCE_VLAN_ACT_TBL_SIZE) {
		ret = tbl_write(tpt->avlan_tbl, tpt->vlan_act_tbl_cnt,
		                parm, sizeof(avlan_tbl_t), PCE_VLAN_ACT_TBL_SIZE);
		if (ret < 0)
			return ret;
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.pcindex = ret;
		pt.table = PCE_ACTVLAN_INDEX;
		pt.key[0] = parm->act_vid;
		pt.mask[0] = range_val;
		pt.valid = 1;
		pt.type = range_flag;
		pt.opmode = PCE_OPMODE_ADWR;
		s = actvlan_ll_twr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return ret;
}

int pce_action_delete(void *pdev, tft_tbl_t *tpt, u32 index)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	PCE_ASSERT(index >= PCE_TABLE_SIZE);
	memset(&tpt->pce_act[index], 0, sizeof(GSW_PCE_action_t));
	pt.pcindex = index;
	pt.table = PCE_TFLOW_INDEX;
	pt.valid = 0;
	pt.opmode = PCE_OPMODE_ADWR;
	s = tflow_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return 0;
}

/* PCE Table Micro Code Init routine */
GSW_return_t gsw_uc_code_init(void *pdev)
{
	GSW_return_t s;
	u32 vn1, lcnt = 0;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_MC_VALID_OFFSET,
	               PCE_GCTRL_0_MC_VALID_SHIFT,
	               PCE_GCTRL_0_MC_VALID_SIZE, 0x0);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               MANU_ID_PNUML_OFFSET,
	               MANU_ID_PNUML_SHIFT,
	               MANU_ID_PNUML_SIZE, &vn1);
	if (s != GSW_statusOk)
		return s;
	if (vn1 == 1) {
		/* 7085/7082 (S) */
		u32 r, p, t;
		s = gsw_reg_rd(pdev,
		               RST_REQ_RD4_OFFSET,
		               RST_REQ_RD4_SHIFT,
		               RST_REQ_RD4_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_rd(pdev,
		               GPHY4_FCR_FCR_OFFSET,
		               GPHY4_FCR_FCR_SHIFT,
		               GPHY4_FCR_FCR_SIZE, &p);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RST_REQ_RD4_OFFSET,
		               RST_REQ_RD4_SHIFT,
		               RST_REQ_RD4_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               GPHY4_FCR_FCR_OFFSET,
		               GPHY4_FCR_FCR_SHIFT,
		               GPHY4_FCR_FCR_SIZE,
		               0x1000);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RST_REQ_RD4_OFFSET,
		               RST_REQ_RD4_SHIFT,
		               RST_REQ_RD4_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		do {
			s = gsw_reg_rd(pdev,
			               RST_REQ_RD4_OFFSET,
			               RST_REQ_RD4_SHIFT,
			               RST_REQ_RD4_SIZE,
			               &t);
			if (s != GSW_statusOk)
				return s;
			lcnt++;
		} while ((!t) && (lcnt < 10));
		s = gsw_reg_wr(pdev,
		               GPHY4_FCR_FCR_OFFSET,
		               GPHY4_FCR_FCR_SHIFT,
		               GPHY4_FCR_FCR_SIZE,
		               p);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RST_REQ_RD4_OFFSET,
		               RST_REQ_RD4_SHIFT,
		               RST_REQ_RD4_SIZE,
		               r);
		if (s != GSW_statusOk)
			return s;
	} else if (vn1 == 0) {
		/*7084 (G)*/
		u32 r, p, t;
		s = gsw_reg_rd(pdev,
		               RST_REQ_RD0_OFFSET,
		               RST_REQ_RD0_SHIFT,
		               RST_REQ_RD0_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_rd(pdev,
		               GPHY0_FCR_FCR_OFFSET,
		               GPHY0_FCR_FCR_SHIFT,
		               GPHY0_FCR_FCR_SIZE,
		               &p);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RST_REQ_RD0_OFFSET,
		               RST_REQ_RD0_SHIFT,
		               RST_REQ_RD0_SIZE,
		               1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               GPHY0_FCR_FCR_OFFSET,
		               GPHY0_FCR_FCR_SHIFT,
		               GPHY0_FCR_FCR_SIZE,
		               0x1000);

		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RST_REQ_RD0_OFFSET,
		               RST_REQ_RD0_SHIFT,
		               RST_REQ_RD0_SIZE,
		               0);
		if (s != GSW_statusOk)
			return s;
		do {
			s = gsw_reg_rd(pdev,
			               RST_REQ_RD0_OFFSET,
			               RST_REQ_RD0_SHIFT,
			               RST_REQ_RD0_SIZE,
			               &t);
			if (s != GSW_statusOk)
				return s;
			lcnt++;
		} while ((!t) && (lcnt < 10));
		s = gsw_reg_wr(pdev,
		               GPHY0_FCR_FCR_OFFSET,
		               GPHY0_FCR_FCR_SHIFT,
		               GPHY0_FCR_FCR_SIZE,
		               p);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RST_REQ_RD0_OFFSET,
		               RST_REQ_RD0_SHIFT,
		               RST_REQ_RD0_SIZE,
		               r);
		if (s != GSW_statusOk)
			return s;
	}
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_MC_VALID_OFFSET,
	               PCE_GCTRL_0_MC_VALID_SHIFT,
	               PCE_GCTRL_0_MC_VALID_SIZE,
	               &vn1);
	if (s != GSW_statusOk)
		return s;
	if ((lcnt >= 10) && !vn1) {
		int i;
		/* Download the microcode over SMDIO interface */
		for (i = 0; i < 64; i++) {
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.val[3] = pce_mc_sw2_3[i].val_3;
			pt.val[2] = pce_mc_sw2_3[i].val_2;
			pt.val[1] = pce_mc_sw2_3[i].val_1;
			pt.val[0] = pce_mc_sw2_3[i].val_0;
			pt.pcindex = i;
			pt.table = PCE_PARS_INDEX;
			pt.opmode = PCE_OPMODE_ADWR;
			s = parser_ll_twr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
		s = gsw_reg_wr(pdev,
		               PCE_GCTRL_0_MC_VALID_OFFSET,
		               PCE_GCTRL_0_MC_VALID_SHIFT,
		               PCE_GCTRL_0_MC_VALID_SIZE,
		               0x1);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

int pce_pattern_delete(void *pdev, tft_tbl_t *tpt,
                       u32 index)
{
	sub_tbl_t *ptable;
	PCE_ASSERT(index >= PCE_TABLE_SIZE);
	/* Check if an entry is currently programmed and remove that one. */
	if (tpt->ptblused[index] == 0)
		return 0;
	ptable = &(tpt->sub_tbl[index]);
#define TFT_IDX_DEL(x, y, z) { \
	if (x != y) \
		if (0 != z(pdev, &tpt->tsub_tbl, y)) \
			return -1; \
	}
	/* Packet length */
	TFT_IDX_DEL(0x1F, ptable->pkt_len_idx, pkt_len_tdel)
	/* Destination MAC address */
	TFT_IDX_DEL(0xFF, ptable->dst_mac_idx, dst_mac_tdel)
	/* Source MAC address */
	TFT_IDX_DEL(0xFF, ptable->src_mac_idx, src_mac_tdel)
	/* Destination Application field */
	TFT_IDX_DEL(0xFF, ptable->dst_appl_idx, app_tdel)
	/* Source Application field */
	TFT_IDX_DEL(0xFF, ptable->src_appl_idx, app_tdel)
	/* DIP MSB */
	TFT_IDX_DEL(0xFF, ptable->dip_msb_idx, ip_dasa_msb_tdel)
	/* DIP LSB */
	TFT_IDX_DEL(0xFF, ptable->dip_lsb_idx, ip_dasa_lsb_tdel)
	/* SIP MSB */
	TFT_IDX_DEL(0xFF, ptable->sip_msb_idx, ip_dasa_msb_tdel)
	/* SIP LSB */
	TFT_IDX_DEL(0xFF, ptable->sip_lsb_idx, ip_dasa_lsb_tdel)
	/* IP protocol */
	TFT_IDX_DEL(0xFF, ptable->ip_prot_idx, ptcl_tdel)
	/* Ethertype */
	TFT_IDX_DEL(0xFF, ptable->ethertype_idx, ptcl_tdel)
	/* PPPoE */
	TFT_IDX_DEL(0x1F, ptable->pppoe_idx, pppoe_tdel)
	/* VLAN */
	TFT_IDX_DEL(0x7F, ptable->vlan_idx, act_vlan_tdel)
	/* SVLAN */
	TFT_IDX_DEL(0x7F, ptable->svlan_idx, act_vlan_tdel)
	/* reset the flag that this rule line is used */
	tpt->ptblused[index] = 0;
	/* reset the rule line */
	memset(ptable, 0xFF, sizeof(sub_tbl_t));
	return pce_action_delete(pdev, tpt, index);
}

int pce_rule_read(void *pdev, tft_tbl_t *tpt,
                  GSW_PCE_rule_t *parm)
{
	GSW_return_t s;
	u32   i, idx = parm->pattern.nIndex;
	sub_tbl_t  *ptable;
	pctbl_prog_t pt;
	PCE_ASSERT(idx >= PCE_TABLE_SIZE);
	/* initialize to zero the structure */
	/* before writing the parameters */
	memset(parm, 0, sizeof(GSW_PCE_rule_t));
	memset(&pt, 0, sizeof(pctbl_prog_t));
	parm->pattern.nIndex = idx;
	if (tpt->ptblused[idx] == 0)
		return 0;
	else
		parm->pattern.bEnable = 1;
	pt.table	= PCE_TFLOW_INDEX;
	pt.pcindex = idx;
	pt.opmode = PCE_OPMODE_ADRD;
	s = tflow_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	/* Port ID */
	if (((pt.key[0] & 0x0F) == 0x0F)) {
		parm->pattern.nPortId = 0;
		parm->pattern.bPortIdEnable = 0;
	} else {
		parm->pattern.nPortId = (pt.key[0] & 0xFF);
		parm->pattern.bPortIdEnable = 1;
	}
	if (((pt.key[6]) & 0xF) == 0xF) {
		parm->pattern.nPCP = 0;
		parm->pattern.bPCP_Enable = 0;
	} else {
		parm->pattern.nPCP = (pt.key[6]) & 0xF;
		parm->pattern.bPCP_Enable = 1;
	}
	if (((pt.key[6] >> 8) & 0x7F) == 0x7F) {
		parm->pattern.nDSCP = 0;
		parm->pattern.bDSCP_Enable = 0;
	} else {
		parm->pattern.nDSCP = (pt.key[6] >> 8) & 0x7F;
		parm->pattern.bDSCP_Enable = 1;
	}
	if (((pt.key[8]) & 0x1F) == 0x1F) {
		parm->pattern.nSTAG_PCP_DEI = 0;
		parm->pattern.bSTAG_PCP_DEI_Enable = 0;
	} else {
		parm->pattern.nSTAG_PCP_DEI = (pt.key[8]) & 0x1F;
		parm->pattern.bSTAG_PCP_DEI_Enable = 1;
	}
	if ((pt.val[0] >> 0) & 0x1) {
		if ((pt.val[1] == 0x0) && (((pt.val[4] >> 2) & 0x3) == 0x3)) {
			parm->action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_DISCARD;
			parm->action.nForwardPortMap = 0;
		} else if ((pt.val[1] != 0x0)
		           && (((pt.val[4] >> 2) & 0x3) == 0x3)) {
			parm->action.nForwardPortMap = pt.val[1];
			parm->action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
		} else if ((((pt.val[4] >> 2) & 0x3) == 0x0)) {
			parm->action.nForwardPortMap = 0;
			parm->action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_REGULAR;
		} else if ((((pt.val[4] >> 2) & 0x3) == 0x1)) {
			parm->action.nForwardPortMap = pt.val[1];
			parm->action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_CPU;
		}
	} else {
		parm->action.ePortMapAction = 0;
		parm->action.nForwardPortMap = 0;
	}
	if ((pt.val[0] >> 1) & 0x1) {
		parm->action.eSVLAN_Action = 1;
		parm->action.nSVLAN_Id = (pt.val[6] >> 4) & 0xFFF;
	} else {
		parm->action.eSVLAN_Action = 0;
		parm->action.nSVLAN_Id = 0;
	}
	if ((pt.val[0] >> 1) & 0x1) {
		parm->action.eVLAN_Action = 1;
		parm->action.nVLAN_Id = (pt.val[5] >> 4) & 0xFFF;
		parm->action.nFId = ((pt.val[2] >> 8) & 0xFF);
	} else {
		parm->action.eVLAN_Action = 0;
		parm->action.nVLAN_Id = 0;
		parm->action.nFId = 0;
	}
	if (((pt.val[5] >> 2) & 0x1) && ((pt.val[0] >> 1) & 0x1))
		parm->action.bCVLAN_Ignore_Control = 1;
	else
		parm->action.bCVLAN_Ignore_Control = 0;
	if ((pt.val[0] >> 2) & 1) {
		if ((pt.val[0] >> 14) & 1)
			parm->action.eTrafficClassAction
			    = GSW_PCE_ACTION_TRAFFIC_CLASS_ALTERNATIVE;
		else
			parm->action.eTrafficClassAction
			    = GSW_PCE_ACTION_TRAFFIC_CLASS_REGULAR;
		if (((pt.val[3] >> 8) & 0xF) == 0xF)
			parm->action.nTrafficClassAlternate = 0;
		else
			parm->action.nTrafficClassAlternate
			    = ((pt.val[3] >> 8) & 0xF);
	}
	if ((pt.val[0] >> 3) & 0x1)
		parm->action.bRemarkAction = 1;
	else
		parm->action.bRemarkAction = 0;
	if ((pt.val[0] >> 4) & 0x1) {
		if ((pt.val[3] >> 15) & 0x1)
			parm->action.eVLAN_CrossAction = GSW_PCE_ACTION_CROSS_VLAN_CROSS;
		else
			parm->action.eVLAN_CrossAction = GSW_PCE_ACTION_CROSS_VLAN_REGULAR;
	} else
		parm->action.eVLAN_CrossAction = GSW_PCE_ACTION_CROSS_VLAN_DISABLE;
	if ((pt.val[0] >> 5) & 0x1)
		parm->action.eCrossStateAction = ((pt.val[4] >> 13) & 0x1);
	if ((pt.val[0] >> 6) & 0x1)
		parm->action.eCritFrameAction = ((pt.val[4] >> 14) & 0x1);
	if ((pt.val[0] >> 7) & 0x1)
		parm->action.eTimestampAction = ((pt.val[4] >> 15) & 0x1);
	if ((pt.val[0] >> 8) & 0x1)
		parm->action.eIrqAction = ((pt.val[0] >> 15) & 0x1);
	if ((pt.val[0] >> 9) & 0x1)
		parm->action.eLearningAction = ((pt.val[4]) & 0x3);

	if ((pt.val[0] >> 11) & 0x1) {
		parm->action.eMeterAction = ((pt.val[3] >> 6) & 0x3);
		parm->action.nMeterId = (pt.val[3] & 0x1F);
	} else {
		parm->action.eMeterAction = 0;
		parm->action.nMeterId = 0;
	}
	if ((pt.val[0] >> 12) & 0x1) {
		parm->action.bRMON_Action = 1;
		parm->action.nRMON_Id = (((pt.val[4] >> 8) & 0x1F) - 1);
	} else {
		parm->action.bRMON_Action = 0;
		parm->action.nRMON_Id = 0;
	}
	parm->action.eSnoopingTypeAction = ((pt.val[4] >> 5) & 0x7);
	if ((pt.val[3] >> 13) & 0x1)
		parm->action.bRemarkClass = 0;
	else
		parm->action.bRemarkClass = 1;
	if ((pt.val[3] >> 12) & 0x1)
		parm->action.bRemarkDSCP = 0;
	else
		parm->action.bRemarkDSCP = 1;
	if ((pt.val[6] >> 2) & 0x1)
		parm->action.bRemarkSTAG_DEI = 0;
	else
		parm->action.bRemarkSTAG_DEI = 1;
	if ((pt.val[6] >> 1) & 0x1)
		parm->action.bRemarkSTAG_PCP = 0;
	else
		parm->action.bRemarkSTAG_PCP = 1;
	if ((pt.val[3] >> 14) & 0x1)
		parm->action.bRemarkPCP = 0;
	else
		parm->action.bRemarkPCP = 1;
	parm->action.bPortLinkSelection =  ((pt.val[5] >> 1) & 0x1);
	parm->action.bPortTrunkAction = ((pt.val[5] >> 0) & 0x1);
	parm->action.bPortBitMapMuxControl = ((pt.val[6] >> 0) & 0x1);
	if (parm->action.bPortBitMapMuxControl)
		parm->action.nForwardPortMap = pt.val[1];
	if ((pt.val[4] >> 4) & 0x1) {
		parm->action.bFlowID_Action = 1;
		parm->action.nFlowID = pt.val[1];
	} else {
		parm->action.bFlowID_Action = 0;
		parm->action.nFlowID = 0;
	}
	ptable = &(tpt->sub_tbl[idx]);
	if (ptable->pkt_len_idx != 0x1F) {
		pkt_len_t pkt;
		memset(&pkt, 0, sizeof(pkt_len_t));
		/* Packet length used */
		parm->pattern.bPktLngEnable = 1;
		if (0 != pkg_lng_tbl_read(&tpt->tsub_tbl,
		                          ptable->pkt_len_idx, &pkt))
			return -1;
		/* Packet length */
		parm->pattern.nPktLng = pkt.pkt_len;
		/* Packet length Range */
		parm->pattern.nPktLngRange = pkt.pkt_len_range;
	}
	if (ptable->dst_mac_idx != 0xFF) {
		mac_tbl_t dmac;
		memset(&dmac, 0, sizeof(mac_tbl_t));
		if (0 != da_mac_trd(&tpt->tsub_tbl,
		                    ptable->dst_mac_idx, &dmac))
			return -1;
		/* Destination MAC address used */
		parm->pattern.bMAC_DstEnable = 1;
		/* Destination MAC address */
		for (i = 0; i < 6; i++)
			parm->pattern.nMAC_Dst[i] = dmac.mac[i];
		/* Destination MAC address mask */
		parm->pattern.nMAC_DstMask = dmac.mmask;
	}
	if (ptable->src_mac_idx != 0xFF) {
		mac_tbl_t ct_src_mac;
		memset(&ct_src_mac, 0, sizeof(mac_tbl_t));
		if (0 != sa_mac_tbl_read(&tpt->tsub_tbl,
		                         ptable->src_mac_idx, &ct_src_mac))
			return -1;
		/* Destination MAC address used */
		parm->pattern.bMAC_SrcEnable = 1;
		/* Destination MAC address */
		for (i = 0; i < 6; i++)
			parm->pattern.nMAC_Src[i] = ct_src_mac.mac[i];

		/* Destination MAC address mask */
		parm->pattern.nMAC_SrcMask = ct_src_mac.mmask;
	}

	if (ptable->dst_appl_idx != 0xFF) {
		app_tbl_t dapp;
		memset(&dapp, 0, sizeof(app_tbl_t));
		if (0 != app_trd(&tpt->tsub_tbl,
		                 ptable->dst_appl_idx, &dapp))
			return -1;
		/* Destination Application used */
		parm->pattern.bAppDataMSB_Enable = 1;
		/* Destination Application field */
		parm->pattern.nAppDataMSB = dapp.app_data;
		/* Destination Application mask/range used */
		parm->pattern.bAppMaskRangeMSB_Select =
		    dapp.app_mr_sel;
		/* Destination Application mask/range */
		parm->pattern.nAppMaskRangeMSB = dapp.app_mr_data;
	}
	if (ptable->src_appl_idx != 0xFF) {
		app_tbl_t sapp;
		memset(&sapp, 0, sizeof(app_tbl_t));
		if (0 != app_trd(&tpt->tsub_tbl,
		                 ptable->src_appl_idx, &sapp))
			return -1;
		/* Source Application used */
		parm->pattern.bAppDataLSB_Enable = 1;
		/* Source Application field */
		parm->pattern.nAppDataLSB = sapp.app_data;
		/* Destination Application mask/range used */
		parm->pattern.bAppMaskRangeLSB_Select =
		    sapp.app_mr_sel;
		/* Source Application mask/range */
		parm->pattern.nAppMaskRangeLSB = sapp.app_mr_data;
	}
	if ((ptable->dip_msb_idx != 0xFF)
	        && (ptable->dip_lsb_idx != 0xFF)) { /*for IPv6  */
		ip_dasa_t mdip;
		ip_dasa_t ldip;
		int i, j;
		memset(&mdip, 0, sizeof(ip_dasa_t));
		memset(&ldip, 0, sizeof(ip_dasa_t));
		/* DIP MSB used */
		parm->pattern.eDstIP_Select = GSW_PCE_IP_V6;
		if (0 != dasa_msb_tbl_read(&tpt->tsub_tbl,
		                           ptable->dip_msb_idx, &mdip))
			return -1;
		/* First, search for DIP in the DA/SA table (DIP MSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2)
			parm->pattern.nDstIP.nIPv6[i]	=
			    (((mdip.ip_val[j] & 0xFF) << 8)
			     | (mdip.ip_val[j-1] & 0xFF));
		parm->pattern.nDstIP_Mask = (mdip.ip_mask[0] & 0xFFFF) << 16;
		/* DIP LSB used */
		parm->pattern.eDstIP_Select = GSW_PCE_IP_V6;
		if (0 != dasa_lsb_tbl_read(&tpt->tsub_tbl,
		                           ptable->dip_lsb_idx, &ldip))
			return -1;
		for (i = 0, j = 7; i < 4; i++, j -= 2)
			parm->pattern.nDstIP.nIPv6[i+4]	=
			    (((ldip.ip_val[j] & 0xFF) << 8)
			     | (ldip.ip_val[j-1] & 0xFF));
		parm->pattern.nDstIP_Mask |= ldip.ip_mask[0] & 0xFFFF;
	} else if (ptable->dip_lsb_idx != 0xFF) {  /*For IPv4 only */
		ip_dasa_t ldip;
		memset(&ldip, 0, sizeof(ip_dasa_t));
		/* DIP LSB used */
		parm->pattern.eDstIP_Select = GSW_PCE_IP_V4;
		if (0 != dasa_lsb_tbl_read(&tpt->tsub_tbl,
		                           ptable->dip_lsb_idx, &ldip))
			return -1;
		/* DIP LSB */
		parm->pattern.nDstIP.nIPv4 =
		    (ldip.ip_val[0]
		     | (ldip.ip_val[1] << 8)
		     | (ldip.ip_val[2] << 16)
		     | (ldip.ip_val[3] << 24));
		/* DIP LSB Nibble Mask */
		parm->pattern.nDstIP_Mask = ldip.ip_mask[0];
	}
	if ((ptable->sip_msb_idx != 0xFF)
	        && (ptable->sip_lsb_idx != 0xFF)) { /* for IPv6 */
		ip_dasa_t msip;
		ip_dasa_t lsip;
		int i, j;
		memset(&msip, 0, sizeof(ip_dasa_t));
		memset(&lsip, 0, sizeof(ip_dasa_t));
		/* SIP MSB used */
		parm->pattern.eSrcIP_Select = GSW_PCE_IP_V6;
		if (0 != dasa_msb_tbl_read(&tpt->tsub_tbl,
		                           ptable->sip_msb_idx, &msip))
			return -1;
		for (i = 0, j = 7; i < 4; i++, j -= 2)
			parm->pattern.nSrcIP.nIPv6[i]	=
			    (((msip.ip_val[j] & 0xFF) << 8)
			     | (msip.ip_val[j-1] & 0xFF));
		/* SIP MSB Nibble Mask */
		parm->pattern.nSrcIP_Mask = (msip.ip_mask[0] & 0xFFFF) << 16;
		/* SIP LSB used */
		parm->pattern.eSrcIP_Select = GSW_PCE_IP_V6;
		if (0 != dasa_lsb_tbl_read(&tpt->tsub_tbl,
		                           ptable->sip_lsb_idx, &lsip))
			return -1;
		for (i = 0, j = 7; i < 4; i++, j -= 2)
			parm->pattern.nSrcIP.nIPv6[i+4]	=
			    (((lsip.ip_val[j] & 0xFF) << 8)
			     | (lsip.ip_val[j-1] & 0xFF));
		/* SIP LSB Nibble Mask */
		parm->pattern.nSrcIP_Mask |= lsip.ip_mask[0] & 0xFFFF;
	} else if (ptable->sip_lsb_idx != 0xFF) {
		/* for IPv4 only */
		ip_dasa_t lsip;
		memset(&lsip, 0, sizeof(ip_dasa_t));
		/* SIP LSB used */
		parm->pattern.eSrcIP_Select = GSW_PCE_IP_V4;
		if (0 != dasa_lsb_tbl_read(&tpt->tsub_tbl,
		                           ptable->sip_lsb_idx, &lsip))
			return -1;
		/* SIP LSB */
		parm->pattern.nSrcIP.nIPv4 = (lsip.ip_val[0]
		                              | (lsip.ip_val[1] << 8)
		                              | (lsip.ip_val[2] << 16)
		                              | (lsip.ip_val[3] << 24));
		/* SIP LSB Nibble Mask */
		parm->pattern.nSrcIP_Mask = lsip.ip_mask[0];
	}
	if (ptable->ethertype_idx != 0xFF) {
		protocol_tb_t etype;
		memset(&etype, 0, sizeof(protocol_tb_t));
		/* Ethertype used */
		parm->pattern.bEtherTypeEnable = 1;
		if (0 != ptcl_tbl_read(&tpt->tsub_tbl,
		                       ptable->ethertype_idx, &etype))
			return -1;
		/* Ethertype */
		parm->pattern.nEtherType = etype.protocol_type;
		/* Ethertype Mask */
		parm->pattern.nEtherTypeMask = etype.protocol_mask;
	}
	if (ptable->ip_prot_idx != 0xFF) {
		protocol_tb_t prot;
		memset(&prot, 0, sizeof(protocol_tb_t));
		/* IP protocol used */
		if (0 != ptcl_tbl_read(&tpt->tsub_tbl,
		                       ptable->ip_prot_idx, &prot))
			return -1;
		if (((prot.protocol_type >> 8) & 0xFF) == 0xFF)
			parm->pattern.bProtocolEnable = 0;
		else
			parm->pattern.bProtocolEnable = 1;
		/* IP protocol */
		parm->pattern.nProtocol = (prot.protocol_type >> 8) & 0xFF;
		/* IP protocol Mask */
		parm->pattern.nProtocolMask = (prot.protocol_mask >> 8) & 0x3;
	}
	if (ptable->pppoe_idx != 0x1F) {
		ppoe_tbl_t ppoe;
		memset(&ppoe, 0, sizeof(ppoe_tbl_t));
		/* PPPoE used */
		parm->pattern.bSessionIdEnable = 1;
		if (0 != pppoe_trd(&tpt->tsub_tbl,
		                   ptable->pppoe_idx, &ppoe))
			return -1;
		/* PPPoE */
		parm->pattern.nSessionId = ppoe.ppsoe_sid;
	}
	if (ptable->vlan_idx != 0x7F) {
		pctbl_prog_t vpt;
		memset(&vpt, 0, sizeof(pctbl_prog_t));
		/* VLAN used */
		parm->pattern.bVid = 1;
		vpt.table = PCE_ACTVLAN_INDEX;
		/*index of the VLAN ID configuration */
		vpt.pcindex = ptable->vlan_idx;
		/* vpt.key[0] = parm->nVId; */
		vpt.opmode = PCE_OPMODE_ADRD;
		s = actvlan_ll_trd(pdev, &vpt);
		if (s != GSW_statusOk)
			return s;
		if (vpt.valid == 1) {
			parm->pattern.nVid = vpt.key[0] & 0xFFF;
			if ((pt.key[0] >> 8) & 0xFE)
				parm->pattern.bVidRange_Select = 2;
			else
				parm->pattern.bVidRange_Select = vpt.type;
			parm->pattern.nVidRange = vpt.mask[0] & 0xFFFF;
		}
	}
	if (ptable->svlan_idx != 0x7F) {
		pctbl_prog_t vpt;
		memset(&vpt, 0, sizeof(pctbl_prog_t));
		/* VLAN used */
		parm->pattern.bSLAN_Vid = 1;
		vpt.table = PCE_ACTVLAN_INDEX;
		/*index of the VLAN ID configuration */
		vpt.pcindex = ptable->svlan_idx;
		/* vpt.key[0] = parm->nVId; */
		vpt.opmode = PCE_OPMODE_ADRD;
		s = actvlan_ll_trd(pdev, &vpt);
		if (s != GSW_statusOk)
			return s;
		if (vpt.valid == 1)
			parm->pattern.nSLAN_Vid = vpt.key[0] & 0xFFF;
	}
	return 0;
}

int pce_rule_write(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	u8 idx = parm->pattern.nIndex;
	sub_tbl_t  *ptable;
	GSW_PCE_action_t *paction;
	int ti, i;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	PCE_ASSERT(idx >= PCE_TABLE_SIZE);
	if (parm->pattern.bEnable == 0)
		return pce_pattern_delete(pdev, tpt, idx);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	/* Delete the old entry before adding the new one. */
	if (tpt->ptblused[idx] != 0) {
		s = pce_pattern_delete(pdev, tpt, idx);
		if (s != GSW_statusOk)
			return s;
	}
	/* Mark the entry as taken already and then program it. */
	tpt->ptblused[idx] = 1;
	/* Now convert the parameter and add to the table. */
	ptable = &(tpt->sub_tbl[idx]);
	/* Port ID */
	if (parm->pattern.bPortIdEnable == 1)
		pt.key[0] = (parm->pattern.nPortId & 0xFF);
	else
		pt.key[0] = 0xFF;
	/* DSCP value used */
	if (parm->pattern.bDSCP_Enable == 1)
		pt.key[6] |= ((parm->pattern.nDSCP & 0x3F) << 8);
	else
		pt.key[6] |= (0x7F << 8);
	/* PCP value used */
	if (parm->pattern.bPCP_Enable == 1)
		pt.key[6] |= (parm->pattern.nPCP & 0x7);
	else
		pt.key[6] |= 0xF;
	/* PCP DEI value used */
	pt.key[8] &= ~(0x1F);
	if (parm->pattern.bSTAG_PCP_DEI_Enable == 1)
		pt.key[8] |= (parm->pattern.nSTAG_PCP_DEI & 0x1F);
	else
		pt.key[8] |= 0x1F;

	/* Packet length used */
	if (parm->pattern.bPktLngEnable == 1) {
		pkt_len_t ln;
		/* Packet length */
		ln.pkt_len = parm->pattern.nPktLng;
		/* Packet length range, in number of bytes */
		ln.pkt_len_range = parm->pattern.nPktLngRange;
		ln.pkt_valid = 1;
		ti = pkg_len_twr(pdev, &tpt->tsub_tbl, &ln);
		if (ti < 0)
			return ti;
		ptable->pkt_len_idx = ti;
	} else {
		ptable->pkt_len_idx =  0x1F;
	}
	pt.key[7] = ptable->pkt_len_idx;

	/* Destination MAC address used */
	if (parm->pattern.bMAC_DstEnable == 1) {
		mac_tbl_t dmac;
		for (i = 0; i < 6; i++)
			dmac.mac[i] = parm->pattern.nMAC_Dst[i];
		dmac.mmask = parm->pattern.nMAC_DstMask;
		dmac.mac_valid = 1;
		ti = dst_mac_twr(pdev, &tpt->tsub_tbl, &dmac);
		if (ti < 0)
			return ti;
		ptable->dst_mac_idx = ti;
	} else {
		ptable->dst_mac_idx = 0xFF;
	}
	pt.key[5] |=  (ptable->dst_mac_idx << 8);

	/* Source MAC address used */
	if (parm->pattern.bMAC_SrcEnable == 1) {
		mac_tbl_t smac;
		for (i = 0; i < 6; i++)
			smac.mac[i] = parm->pattern.nMAC_Src[i];
		/* Source MAC address mask */
		smac.mmask = parm->pattern.nMAC_SrcMask;
		smac.mac_valid = 1;
		ti = src_mac_twr(pdev, &tpt->tsub_tbl, &smac);
		if (ti < 0)
			return ti;
		ptable->src_mac_idx = ti;
	} else {
		ptable->src_mac_idx = 0xFF;
	}
	pt.key[5] |= ptable->src_mac_idx;

	/* Destination Application used */
	if (parm->pattern.bAppDataMSB_Enable == 1) {
		app_tbl_t appm;
		appm.app_mr_sel = parm->pattern.bAppMaskRangeMSB_Select;
		/* Destination Application field */
		appm.app_data = parm->pattern.nAppDataMSB;
		/* Destination Application mask/range */
		appm.app_mr_data = parm->pattern.nAppMaskRangeMSB;
		appm.app_valid = 1;
		ti = app_twr(pdev, &tpt->tsub_tbl, &appm);
		if (ti < 0)
			return ti;
		ptable->dst_appl_idx = ti;
	} else {
		ptable->dst_appl_idx = 0xFF;
	}
	pt.key[4] |= ptable->dst_appl_idx;

	/* Source Application field used */
	if (parm->pattern.bAppDataLSB_Enable == 1) {
		app_tbl_t appl;
		appl.app_mr_sel =  parm->pattern.bAppMaskRangeLSB_Select;
		/* Source Application field */
		appl.app_data = parm->pattern.nAppDataLSB;
		/* Source Application mask/range */
		appl.app_mr_data = parm->pattern.nAppMaskRangeLSB;
		appl.app_valid = 1;
		ti = app_twr(pdev, &tpt->tsub_tbl, &appl);
		if (ti < 0)
			return ti;
		ptable->src_appl_idx = ti;
	} else {
		ptable->src_appl_idx = 0xFF;
	}
	pt.key[4] |= (ptable->src_appl_idx << 8);

	ptable->dip_msb_idx = 0xFF;
	ptable->dip_lsb_idx = 0xFF;
	/* DIP MSB used */
	if (parm->pattern.eDstIP_Select == /*2*/GSW_PCE_IP_V6) {
		ip_dasa_t  mtbl;
		ip_dasa_t  ltbl;
		int	j;
		memset(&mtbl, 0, sizeof(ip_dasa_t));
		memset(&ltbl, 0, sizeof(ip_dasa_t));
		/* First, search for DIP in the DA/SA table (DIP MSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.ip_val[j-1]
			    = (parm->pattern.nDstIP.nIPv6[i] & 0xFF);
			mtbl.ip_val[j]
			    = ((parm->pattern.nDstIP.nIPv6[i] >> 8) & 0xFF);
		}
		mtbl.ip_mask[0] = ((parm->pattern.nDstIP_Mask >> 16) & 0xFFFF);
		mtbl.ip_valid = 1;
		ti = dasa_ip_msb_twr(pdev, &tpt->tsub_tbl, &mtbl);
		if (ti < 0)
			return ti;
		ptable->dip_msb_idx = ti;

		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ip_val[j-1] = (parm->pattern.nDstIP.nIPv6[i+4]
			                    & 0xFF);
			ltbl.ip_val[j] = ((parm->pattern.nDstIP.nIPv6[i+4] >> 8)
			                  & 0xFF);
		}
		ltbl.ip_mask[0] = (parm->pattern.nDstIP_Mask & 0xFFFF);
		ltbl.ip_valid = 1;
		ti = dasa_lsb_twr(pdev,	&tpt->tsub_tbl, &ltbl);
		if (ti < 0)
			return ti;
		ptable->dip_lsb_idx = ti;
	} else  if (parm->pattern.eDstIP_Select == GSW_PCE_IP_V4) {
		/* DIP LSB used */
		ip_dasa_t  ltbl;
		memset(&ltbl, 0, sizeof(ip_dasa_t));
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0; i < 4; i++)
			ltbl.ip_val[i]
			    = (((parm->pattern.nDstIP.nIPv4) >> (i * 8)) & 0xFF);
		/* DIP LSB Nibble Mask */
		ltbl.ip_mask[0] = (parm->pattern.nDstIP_Mask & 0xFFFF);
		ltbl.ip_valid = 1;
		ti = dasa_lsb_twr(pdev, &tpt->tsub_tbl, &ltbl);
		if (ti < 0)
			return ti;
		ptable->dip_lsb_idx = ti;
	}
	pt.key[3] |= (ptable->dip_msb_idx << 8);
	pt.key[3] |= ptable->dip_lsb_idx;

	/* SIP MSB used */
	ptable->sip_msb_idx = 0xFF;
	ptable->sip_lsb_idx = 0xFF;
	if (parm->pattern.eSrcIP_Select == GSW_PCE_IP_V6) {
		ip_dasa_t  mtbl;
		ip_dasa_t  ltbl;
		int	j;
		memset(&mtbl, 0, sizeof(ip_dasa_t));
		memset(&ltbl, 0, sizeof(ip_dasa_t));
		/* First, search for DIP in the DA/SA table (DIP MSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.ip_val[j-1] = (parm->pattern.nSrcIP.nIPv6[i]
			                    & 0xFF);
			mtbl.ip_val[j]	= ((parm->pattern.nSrcIP.nIPv6[i] >> 8)
			                   & 0xFF);
		}
		mtbl.ip_mask[0] = ((parm->pattern.nSrcIP_Mask >> 16) & 0xFFFF);
		mtbl.ip_valid = 1;
		ti = dasa_ip_msb_twr(pdev, &tpt->tsub_tbl, &mtbl);
		if (ti < 0)
			return ti;
		ptable->sip_msb_idx = ti;
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ip_val[j-1] = (parm->pattern.nSrcIP.nIPv6[i+4]
			                    & 0xFF);
			ltbl.ip_val[j] = ((parm->pattern.nSrcIP.nIPv6[i+4] >> 8)
			                  & 0xFF);
		}
		ltbl.ip_mask[0] = (parm->pattern.nSrcIP_Mask & 0xFFFF);
		ltbl.ip_valid = 1;
		ti = dasa_lsb_twr(pdev, &tpt->tsub_tbl, &ltbl);
		if (ti < 0)
			return ti;
		ptable->sip_lsb_idx = ti;
	} else  if (parm->pattern.eSrcIP_Select == GSW_PCE_IP_V4) {
		/* SIP LSB used */
		ip_dasa_t  ltbl;
		memset(&ltbl, 0, sizeof(ip_dasa_t));
		/* Second, search for SIP in the DA/SA table (SIP LSB) */
		for (i = 0; i < 4; i++)
			ltbl.ip_val[i]
			    = ((parm->pattern.nSrcIP.nIPv4 >> (i * 8)) & 0xFF);
		/* DIP LSB Nibble Mask */
		ltbl.ip_mask[0] = (parm->pattern.nSrcIP_Mask | 0xFF00) & 0xFFFF;
		ltbl.ip_valid = 1;
		ti = dasa_lsb_twr(pdev, &tpt->tsub_tbl, &ltbl);
		if (ti < 0)
			return ti;
		ptable->sip_lsb_idx = ti;
	}
	pt.key[2] |= (ptable->sip_msb_idx << 8);
	pt.key[2] |= ptable->sip_lsb_idx;
	/* Ethertype used */
	if (parm->pattern.bEtherTypeEnable == 1) {
		protocol_tb_t et;
		memset(&et, 0, sizeof(protocol_tb_t));
		et.protocol_type = parm->pattern.nEtherType;
		et.protocol_mask = parm->pattern.nEtherTypeMask;
		et.protocol_valid = 1;
		ti = protocol_twr(pdev,	&tpt->tsub_tbl, &et);
		if (ti < 0)
			return ti;
		ptable->ethertype_idx = ti;
	} else {
		ptable->ethertype_idx = 0xFF;
	}
	pt.key[1] |= ptable->ethertype_idx;
	if (parm->pattern.bProtocolEnable == 1) {
		protocol_tb_t pctl;
		memset(&pctl, 0, sizeof(protocol_tb_t));
		pctl.protocol_type = ((parm->pattern.nProtocol & 0xFF) << 8);
		pctl.protocol_type |= (0xFF);
		pctl.protocol_mask
		    = ((parm->pattern.nProtocolMask  & 0x3) << 2);
		pctl.protocol_mask |= (0x3);
		pctl.protocol_valid = 1;
		ti = protocol_twr(pdev, &tpt->tsub_tbl, &pctl);
		if (ti < 0)
			return ti;
		ptable->ip_prot_idx = ti;
	} else {
		ptable->ip_prot_idx = 0xFF;
	}
	pt.key[1] |= (ptable->ip_prot_idx << 8);

	/* PPPoE used */
	if (parm->pattern.bSessionIdEnable == 1) {
		ppoe_tbl_t ppe;
		ppe.ppsoe_sid = parm->pattern.nSessionId;
		ppe.pppoe_valid = 1;
		ti = pppoe_twr(pdev, &tpt->tsub_tbl, &ppe);
		if (ti < 0)
			return ti;
		ptable->pppoe_idx = ti;
	} else {
		ptable->pppoe_idx = 0x1F;
	}
	pt.key[7] |= (ptable->pppoe_idx << 8);
	/* VLAN used */
	ti = 0x7F;
	if (parm->pattern.bVid == 1) {
		avlan_tbl_t avt;
		memset(&avt, 0, sizeof(avlan_tbl_t));
		avt.act_vid = parm->pattern.nVid;
		avt.avlan_valid = 1;
		ti = actvlan_twr(pdev, &tpt->tsub_tbl, &avt,
		                 parm->pattern.bVidRange_Select,
		                 parm->pattern.nVidRange);
		if (ti < 0)
			return ti;
	}
	ptable->vlan_idx = ti;
	pt.key[0] &= ~(0xFF << 8);
	/* Exclude mode for the VLAN range*/
	if ((parm->pattern.bVidRange_Select == 2) && (parm->pattern.bVid == 1))
		pt.key[0] |= (0xFE << 8);
	else
		pt.key[0] |= (ptable->vlan_idx << 8);

	ti = 0x7F;
	if (parm->pattern.bSLAN_Vid == 1) {
		avlan_tbl_t avt;
		memset(&avt, 0, sizeof(avlan_tbl_t));
		avt.act_vid = parm->pattern.nSLAN_Vid;
		avt.avlan_valid = 1;
		ti = actvlan_twr(pdev, &tpt->tsub_tbl,	&avt, 0, 0);
		if (ti < 0)
			return ti;
	}
	ptable->svlan_idx = ti;
	pt.key[8] &= ~(0xFF << 8);
	pt.key[8] |= (ptable->svlan_idx << 8);

	paction = &(tpt->pce_act[idx]);
	memcpy(paction, &parm->action, sizeof(GSW_PCE_action_t));
	if (paction->ePortMapAction
	        != GSW_PCE_ACTION_PORTMAP_DISABLE) {
		if (paction->eSnoopingTypeAction
		        == GSW_PCE_ACTION_IGMP_SNOOP_DISABLE) {
			pt.val[0] = 1;
			pt.val[4] |= (0x3 << 2);
			switch (paction->ePortMapAction) {
			case GSW_PCE_ACTION_PORTMAP_REGULAR:
				pt.val[4] &= ~(0x3 << 2);
				break;
			case GSW_PCE_ACTION_PORTMAP_DISCARD:
				pt.val[1] = 0;
				break;
			case GSW_PCE_ACTION_PORTMAP_CPU:
				pt.val[1] = (paction->nForwardPortMap
				             & 0xFFFF);
				pt.val[4] &= ~(0x3 << 2);
				pt.val[4] |= (0x1 << 2);
				break;
			case GSW_PCE_ACTION_PORTMAP_ALTERNATIVE:
				pt.val[1] = (paction->nForwardPortMap
				             & 0xFFFF);
				break;
			/* To fix compilation warnings*/
			case GSW_PCE_ACTION_PORTMAP_DISABLE:
			case GSW_PCE_ACTION_PORTMAP_MULTICAST_ROUTER:
			case GSW_PCE_ACTION_PORTMAP_MULTICAST_HW_TABLE:
			case GSW_PCE_ACTION_PORTMAP_ALTERNATIVE_VLAN:
			case GSW_PCE_ACTION_PORTMAP_ALTERNATIVE_STAG_VLAN:
				break;
			}
		} else {
			switch (paction->eSnoopingTypeAction) {
			case GSW_PCE_ACTION_IGMP_SNOOP_REPORT:
			case GSW_PCE_ACTION_IGMP_SNOOP_LEAVE:
				pt.val[0] = 1;
				pt.val[4] &= ~(0x3 << 2);
				/* Multicast router portmap */
				pt.val[4] |= (0x1 << 2);
				break;
			default:
				pt.val[0] = 1;
				pt.val[4] &= ~(0x3 << 2);
				/* default port map */
				pt.val[4] |= (0x0 << 2);
				break;
			}
		}
	} else {
		pt.val[0] = 0;
		pt.val[1] = 0xFFFF;
		pt.val[4] &= ~(0x3 << 2);
	}
	if (paction->bFlowID_Action != 0) {
		if (paction->ePortMapAction
		        == GSW_PCE_ACTION_PORTMAP_DISABLE) {
			/* enable Flow ID action */
			pt.val[4] |= (0x1 << 4);
			pt.val[1] = paction->nFlowID & 0xFFFF;
		} else {
			return GSW_statusNoSupport;
		}
	}
	/** Action "VLAN" Group. VLAN action enable */
	pt.val[2] = 0;
	/* for GSW_PCE_ACTION_VLAN_REGULAR  also*/
	pt.val[0] &= ~(1 << 13);
	if (paction->eVLAN_Action
	        != GSW_PCE_ACTION_VLAN_DISABLE) {
		pt.val[0] |= (1 << 1);
		if (paction->eVLAN_Action
		        == GSW_PCE_ACTION_VLAN_ALTERNATIVE) {
			pt.val[5] |=
			    ((paction->nVLAN_Id & 0xFFF) << 4);
			pt.val[2]
			|= ((paction->nFId & 0xFF) << 8);
			/* alternative CTAG VLAN ID and FID */
			pt.val[5] |= (1 << 3);
			pt.val[0] |= (1 << 13);
		}
	}
	/** Action "SVLAN" Group. SVLAN action enable */
	pt.val[6] = 0;
	if (paction->eSVLAN_Action
	        != GSW_PCE_ACTION_VLAN_DISABLE) {
		pt.val[0] |= (1 << 1);
		/*  default CTAG VLAN ID and FID */
		pt.val[6] &= ~(1 << 3);
		if (paction->eSVLAN_Action
		        == GSW_PCE_ACTION_VLAN_ALTERNATIVE) {
			pt.val[6]
			|= ((paction->nSVLAN_Id & 0xFFF) << 4);
			pt.val[0] |= (1 << 13);
			pt.val[6] |= (1 << 3);
		}
	}
	/** Action "Traffic Class" Group. Traffic class action enable */
	if (paction->eTrafficClassAction
	        != GSW_PCE_ACTION_TRAFFIC_CLASS_DISABLE) {
		pt.val[0] |= (1 << 2);
		switch (paction->eTrafficClassAction) {
		case GSW_PCE_ACTION_TRAFFIC_CLASS_REGULAR:
			pt.val[0] &= ~(1 << 14);
			break;
		case GSW_PCE_ACTION_TRAFFIC_CLASS_ALTERNATIVE:
			pt.val[0] |= (1 << 14);
			pt.val[3] &= ~(0xF << 8);
			pt.val[3]
			|= (paction->nTrafficClassAlternate & 0xF) << 8;
			break;
		case GSW_PCE_ACTION_TRAFFIC_CLASS_DISABLE:
			break;
		}
	} else {
		pt.val[0] &= ~((1 << 2) | (1 << 14));
		pt.val[3] |= (0xF << 8);
	}
	/** Action "Remarking" Group. Remarking action enable */
	if (paction->bRemarkAction != 0)
		pt.val[0] |= (1 << 3);
	else
		pt.val[0] &= ~(1 << 3);
	/** Action "Cross VLAN" Group. Cross VLAN action enable */
	if (paction->eVLAN_CrossAction
	        != GSW_PCE_ACTION_CROSS_VLAN_DISABLE) {
		pt.val[0] |= (1 << 4);
		if (paction->eVLAN_CrossAction
		        == GSW_PCE_ACTION_CROSS_VLAN_REGULAR)
			pt.val[3] &= ~(1 << 15);
		else
			pt.val[3] |= (1 << 15);
	} else {
		pt.val[0] &= ~(1 << 4);
		pt.val[3] &= ~(1 << 15);
	}
	/** Action "Cross State" Group. Cross state action control and enable */
	if (paction->eCrossStateAction
	        != GSW_PCE_ACTION_CROSS_STATE_DISABLE) {
		pt.val[0] |= (1 << 5);
		if (paction->eCrossStateAction
		        == GSW_PCE_ACTION_CROSS_STATE_CROSS)
			pt.val[4] |= (1 << 13);
		else
			pt.val[4] &= ~(1 << 13);
	} else {
		pt.val[4] &= ~(1 << 13);
		pt.val[0] &= ~(1 << 5);
	}
	/* Action "Critical Frames" Group. Critical Frame action control and enable*/
	if (paction->eCritFrameAction
	        != GSW_PCE_ACTION_CRITICAL_FRAME_DISABLE) {
		pt.val[0] |= (1 << 6);
		if (paction->eCritFrameAction
		        == GSW_PCE_ACTION_CRITICAL_FRAME_CRITICAL)
			pt.val[4] |= (1 << 14);
		else
			pt.val[4] &= ~(1 << 14);
	} else {
		pt.val[0] &= ~(1 << 6);
		pt.val[4] &= ~(1 << 14);
	}
	/** Action "Timestamp" Group. Time stamp action control and enable */
	if (paction->eTimestampAction
	        != GSW_PCE_ACTION_TIMESTAMP_DISABLE) {
		pt.val[0] |= (1 << 7);
		if (paction->eTimestampAction
		        == GSW_PCE_ACTION_TIMESTAMP_STORED)
			pt.val[4] |= (1 << 15);
		else
			pt.val[4] &= ~(1 << 15);
	} else {
		pt.val[0] &= ~(1 << 7);
		pt.val[4] &= ~(1 << 15);
	}
	/** Action "Interrupt" Group. Interrupt action generate and enable */
	if (paction->eIrqAction != GSW_PCE_ACTION_IRQ_DISABLE) {
		pt.val[0] |= (1 << 8);
		if (paction->eIrqAction == GSW_PCE_ACTION_IRQ_EVENT)
			pt.val[0] |= (1 << 15);
		else
			pt.val[0] &= ~(1 << 15);
	} else {
		pt.val[0] &= ~((1 << 8) | (1 << 15));
	}
	/** Action "Learning" Group. Learning action control and enable */
	if (paction->eLearningAction
	        != GSW_PCE_ACTION_LEARNING_DISABLE) {
		pt.val[0] |= (1 << 9);
		/* Todo: Learning Rule need to be check */
		switch (paction->eLearningAction) {
		case GSW_PCE_ACTION_LEARNING_REGULAR:
			pt.val[4] &= ~0x3;
			break;
		case GSW_PCE_ACTION_LEARNING_FORCE_NOT:
			pt.val[4] =
			    (pt.val[4] & ~0x3) | 0x2;
			break;
		case GSW_PCE_ACTION_LEARNING_FORCE:
			pt.val[4] |= 0x3;
			break;
		case GSW_PCE_ACTION_LEARNING_DISABLE:
			break;
		}
	} else {
		pt.val[0] &= ~(1 << 9);
		pt.val[4] &= ~0x3;
	}
	/** Action "IGMP Snooping" Group. */
	if (paction->eSnoopingTypeAction
	        != GSW_PCE_ACTION_IGMP_SNOOP_DISABLE) {
		pt.val[0] |= (1 << 10);
		pt.val[4] &= ~(0x7 << 5);
		switch (paction->eSnoopingTypeAction) {
		case GSW_PCE_ACTION_IGMP_SNOOP_REGULAR:
			pt.val[4] |= (0 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_REPORT:
			pt.val[4] |= (1 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_LEAVE:
			pt.val[4] |= (2 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_AD:
			pt.val[4] |= (3 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_QUERY:
			pt.val[4] |= (4 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP:
			pt.val[4] |= (5 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER:
			pt.val[4] |= (6 << 5);
			break;
		case GSW_PCE_ACTION_IGMP_SNOOP_DISABLE:
			break;
		}
	} else {
		pt.val[0] &= ~(1 << 10);
		pt.val[4] &= ~(0x7 << 5);
	}
	/** Action "Meter" Group. Meter action control and enable. */
	if (paction->eMeterAction
	        != GSW_PCE_ACTION_METER_DISABLE) {
		pt.val[0] |= (1 << 11);
		pt.val[3] = (paction->nMeterId & 0x1F);
		switch (paction->eMeterAction) {
		case GSW_PCE_ACTION_METER_REGULAR:
			pt.val[3] |= 0 << 6;
			break;
		case GSW_PCE_ACTION_METER_1:
			pt.val[3] |= 1 << 6;
			break;
		/*	case GSW_PCE_ACTION_METER_2: */
		/*				pt.val[3] |= 2 << 6; */
		/*				break; */
		case GSW_PCE_ACTION_METER_1_2:
			pt.val[3] |= 3 << 6;
			break;
		case GSW_PCE_ACTION_METER_DISABLE:
			break;
		}
	} else {
		pt.val[0] &= ~(1 << 11);
		pt.val[3] |= 0x1F;
	}
	/** Action "RMON" Group. RMON action enable */
	if (paction->bRMON_Action != 0) {
		pt.val[0] |= (1 << 12);
		pt.val[4] &= ~(0x1F << 8);
		if (paction->nRMON_Id < 24)  {
			/*RMON_ID will support from 1 to 24 */
			pt.val[4] |= ((paction->nRMON_Id + 1) << 8);
		} else  {
			return -1;
		}
	} else {
		pt.val[0] &= ~(1 << 12);
		pt.val[4] &= ~(0x1F << 8);
	}
	pt.val[3] |= (0x7 << 12);
	if (paction->bRemarkDSCP == 1)
		pt.val[3] &= ~(1 << 12);
	if (paction->bRemarkClass == 1)
		pt.val[3] &= ~(1 << 13);
	if (paction->bRemarkPCP == 1)
		pt.val[3] &= ~(1 << 14);
	if (paction->bRemarkSTAG_PCP == 1)
		pt.val[6] &= ~(1 << 1);
	else
		pt.val[6] |= (1 << 1);
	if (paction->bRemarkSTAG_DEI == 1)
		pt.val[6] &= ~(1 << 2);
	else
		pt.val[6] |= (1 << 2);
	if (paction->bPortBitMapMuxControl == 1) {
		pt.val[6] |= (1 << 0);
		pt.val[1] = (paction->nForwardPortMap & 0xFFFF);
	}
	else
		pt.val[6] &= ~(1 << 0);
	/* Trunking action enable */
	if (paction->bPortTrunkAction == 1)
		pt.val[5] |= (1 << 0);
	else
		pt.val[5] &= ~(1 << 0);
	/* Port Link Selection control */
	if (paction->bPortLinkSelection == 1)
		pt.val[5] |= (1 << 1);
	else
		pt.val[5] &= ~(1 << 1);
	/*	CVLAN Ignore control */
	if (paction->bCVLAN_Ignore_Control == 1) {
		pt.val[5] |= (1 << 2);
		pt.val[0] |= (1 << 1);
	}	else {
		pt.val[5] &= ~(1 << 2);
	}
	pt.pcindex = idx;
	pt.table = PCE_TFLOW_INDEX;
	pt.valid = 1;
	pt.opmode = PCE_OPMODE_ADWR;
#if 0
	GSW_PRINT("\n");
	for (i = 0; i < 16; i++)
		GSW_PRINT("Val[%d]: 0x%08x, key[%d]: 0x%08x\n",
		          i, pt.val[i], i, pt.key[i]);
#endif
	s = tflow_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
