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
#if defined(CONFIG_LTQ_MULTICAST) && CONFIG_LTQ_MULTICAST
/**************************************************************************/
/*      EXTERNS Declaration:                                              */
/**************************************************************************/
extern GSW_return_t gsw_reg_rd(void *pdev, u16 offset, u16 shift, u16 sz, ur *v);
extern GSW_return_t gsw_reg_wr(void *pdev, u16 offset, u16 shift, u16 sz, u32 v);
extern int pce_rule_read(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm);
extern int pce_rule_write(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm);
extern int pce_pattern_delete(void *pdev, tft_tbl_t *tpt, u32 index);
extern GSW_return_t ip_ll_trd(void *ped, pctbl_prog_t *pt);
extern GSW_return_t dasa_ip_msb_twr(void *pdev,
                                    pcetbl_prog_t *tpt, ip_dasa_t *parm);
extern int find_msb_tbl_entry(pcetbl_prog_t *tpt, ip_dasa_t *parm);
extern int find_dasa_tbl_entry(pcetbl_prog_t *tpt, ip_dasa_t *parm);
extern int dasa_lsb_twr(void *pdev, pcetbl_prog_t *tpt, ip_dasa_t *parm);
extern int ip_dasa_msb_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
extern int ip_dasa_lsb_tdel(void *pdev, pcetbl_prog_t *tpt, u32 index);
extern int ipdslsb_tblidx_del(pcetbl_prog_t *tpt, u32 index);
extern int ipdsmsb_tblidx_del(pcetbl_prog_t *tpt, u32 index);
extern int dasa_lsb_tbl_read(pcetbl_prog_t *tpt, int index, ip_dasa_t *parm);
extern GSW_return_t npport(void *pdev, u8 port);
extern GSW_return_t pctrl_bas_busy(void *pdev);
extern GSW_return_t pctrl_bas_set(void *pdev);
extern GSW_return_t pctrl_reg_clr(void *pdev);
extern GSW_return_t pctrl_addr_opmod(void *p, u16 ad, u16 op);

/**************************************************************************/
/*      DEFINES:                                                          */
/**************************************************************************/
static GSW_return_t mcast_tbl_wr(void *pdev, pctbl_prog_t *pt);
/**************************************************************************/
/*      LOCAL DECLARATIONS:                                               */
/**************************************************************************/
static GSW_return_t mcast_tbl_wr(void *pdev,
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

GSW_return_t gsw_mcast_tbl_rd(void *pdev,
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
	               PCE_TBL_KEY_2_KEY2_OFFSET,
	               PCE_TBL_KEY_2_KEY2_SHIFT,
	               PCE_TBL_KEY_2_KEY2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[2] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_1_KEY1_OFFSET,
	               PCE_TBL_KEY_1_KEY1_SHIFT,
	               PCE_TBL_KEY_1_KEY1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[1] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_4_VAL4_OFFSET,
	               PCE_TBL_VAL_4_VAL4_SHIFT,
	               PCE_TBL_VAL_4_VAL4_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[4] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_3_VAL3_OFFSET,
	               PCE_TBL_VAL_3_VAL3_SHIFT,
	               PCE_TBL_VAL_3_VAL3_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[3] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_2_VAL2_OFFSET,
	               PCE_TBL_VAL_2_VAL2_SHIFT,
	               PCE_TBL_VAL_2_VAL2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[2] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_1_VAL1_OFFSET,
	               PCE_TBL_VAL_1_VAL1_SHIFT,
	               PCE_TBL_VAL_1_VAL1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[1] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	return GSW_statusOk;
}
/**************************************************************************/
/*      INTERFACE FUNCTIONS Implementation:                               */
/**************************************************************************/
/* Multicast Software Table Include/Exclude Add function */
static int gsw2x_msw_table_wr(void *pdev,
                              GSW_multicastTable_t *parm)
{
	GSW_return_t s;
	int	i, j, exclude_rule = 0;
	int	dlix = 0x7F, dmix = 0x7F;
	int slix = 0x7F, smix = 0x7F;
	int dlflag = 0, dmflag = 0, slflag = 0, smflag = 0;
	ltq_bool_t new_entry = 0;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	tft_tbl_t *hpctbl = &pd->ptft;
	gsw_igmp_t *hitbl = &pd->iflag;
	pctbl_prog_t pt;
	ip_dasa_t	ltbl;
	ip_dasa_t	mtbl;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	memset(&ltbl, 0, sizeof(ip_dasa_t));
	memset(&mtbl, 0, sizeof(ip_dasa_t));
	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	        || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (pd->iflag.igv3 != 1)
			return GSW_statusParam;
	}
	if ((parm->eIPVersion != GSW_IP_SELECT_IPV4)
	        && (parm->eIPVersion != GSW_IP_SELECT_IPV6)) {
		GSW_PRINT("(IPv4/IPV6 need to enable)\n");
		return GSW_statusErr;
	}
	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		for (i = 0; i < 4; i++)
			ltbl.ip_val[i] =
			    ((parm->uIP_Gda.nIPv4 >> (i * 8)) & 0xFF);
		ltbl.ip_mask[0] = 0xFF00;
	}
	if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.ip_val[j-1] = (parm->uIP_Gda.nIPv6[i] & 0xFF);
			mtbl.ip_val[j] = ((parm->uIP_Gda.nIPv6[i] >> 8) & 0xFF);
		}
		mtbl.ip_mask[0] = 0;
		mtbl.ip_valid = 1;
		dmix = find_msb_tbl_entry(&hpctbl->tsub_tbl, &mtbl);
		if (dmix == 0xFF) {
			dmix = dasa_ip_msb_twr(pdev, &hpctbl->tsub_tbl, &mtbl);
			dmflag = 1;
		}
		if (dmix < 0) {
			GSW_PRINT("%s:%s:%d (IGMP Table full)\n",
			          __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ip_val[j-1] = (parm->uIP_Gda.nIPv6[i+4] & 0xFF);
			ltbl.ip_val[j] = ((parm->uIP_Gda.nIPv6[i+4] >> 8) & 0xFF);
		}
		ltbl.ip_mask[0] = 0;/* DIP LSB Nibble Mask */
	}
	ltbl.ip_valid = 1;
	dlix = find_dasa_tbl_entry(&hpctbl->tsub_tbl, &ltbl);
	if (dlix == 0xFF) {
		dlix = dasa_lsb_twr(pdev, &hpctbl->tsub_tbl, &ltbl);
		dlflag = 1;
	}
	if (dlix < 0) {
		GSW_PRINT("%s:%s:%d (IGMP Table full)\n",
		          __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	        || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			for (i = 0; i < 4; i++)
				ltbl.ip_val[i] =
				    ((parm->uIP_Gsa.nIPv4 >> (i * 8)) & 0xFF);
			/* DIP LSB Nibble Mask */
			ltbl.ip_mask[0] = 0xFF00;
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ip_val[3] == 0) &&
				        (ltbl.ip_val[2] == 0) &&
				        (ltbl.ip_val[1] == 0) &&
				        (ltbl.ip_val[0] == 0)) {
					GSW_PRINT("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
					          __FILE__, __func__, __LINE__);
					return GSW_statusErr;
				}
			}
		}
		if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			int src_zero = 0;
			/*First, search for DIP in the DA/SA table (DIP MSB)*/
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				mtbl.ip_val[j-1] =
				    (parm->uIP_Gsa.nIPv6[i] & 0xFF);
				mtbl.ip_val[j] =
				    ((parm->uIP_Gsa.nIPv6[i] >> 8) & 0xFF);
			}
			mtbl.ip_mask[0] = 0;/* DIP MSB Nibble Mask */
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((mtbl.ip_val[0] == 0) &&
				        (mtbl.ip_val[1] == 0) &&
				        (mtbl.ip_val[2] == 0) &&
				        (mtbl.ip_val[3] == 0) &&
				        (mtbl.ip_val[4] == 0) &&
				        (mtbl.ip_val[5] == 0) &&
				        (mtbl.ip_val[6] == 0) &&
				        (mtbl.ip_val[7] == 0)) {
					src_zero = 1;
				}
			}
			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				ltbl.ip_val[j-1] =
				    (parm->uIP_Gsa.nIPv6[i+4] & 0xFF);
				ltbl.ip_val[j] =
				    ((parm->uIP_Gsa.nIPv6[i+4] >> 8) & 0xFF);
			}
			ltbl.ip_mask[0] = 0;/* DIP LSB Nibble Mask */
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ip_val[0] == 0) &&
				        (ltbl.ip_val[1] == 0) &&
				        (ltbl.ip_val[2] == 0) &&
				        (ltbl.ip_val[3] == 0) &&
				        (ltbl.ip_val[4] == 0) &&
				        (ltbl.ip_val[5] == 0) &&
				        (ltbl.ip_val[6] == 0) &&
				        (ltbl.ip_val[7] == 0)) {
					if (src_zero) {
						GSW_PRINT("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
						          __FILE__, __func__, __LINE__);
						return GSW_statusErr;
					}
				}
			}
			mtbl.ip_valid = 1;
			smix = find_msb_tbl_entry(&hpctbl->tsub_tbl, &mtbl);
			if (smix == 0xFF) {
				smix = dasa_ip_msb_twr(pdev, &hpctbl->tsub_tbl, &mtbl);
				smflag = 1;
			}
			if (smix < 0) {
				GSW_PRINT("%s:%s:%d (IGMP Table full)\n",
				          __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
		}
		ltbl.ip_valid = 1;
		slix = find_dasa_tbl_entry(&hpctbl->tsub_tbl, &ltbl);
		if (slix == 0xFF) {
			slix = dasa_lsb_twr(pdev,
			                    &hpctbl->tsub_tbl, &ltbl);
			slflag = 1;
		}
		if (slix < 0) {
			GSW_PRINT("%s:%s:%d (IGMP Table full)\n",
			          __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
	}
	/* update the entry for another port number if already exists*/
	for (i = 0; i < pd->iflag.itblsize; i++) {
		/* Check if port was already exist */
		if ((hitbl->mctable[i].dlsbindex == dlix) &&
		        (hitbl->mctable[i].dmsbindex == dmix) &&
		        (hitbl->mctable[i].slsbindex == slix) &&
		        (hitbl->mctable[i].smsbindex == smix) &&
		        (hitbl->mctable[i].valid == 1)) {
			if (((hitbl->mctable[i].pmap >> parm->nPortId)
			        & 0x1) == 1)
				return GSW_statusOk;
			switch (hitbl->mctable[i].mcmode) {
			case GSW_IGMP_MEMBER_DONT_CARE:
				SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
				hpctbl->tsub_tbl.iplsbtcnt[dlix]++;
				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
					hpctbl->tsub_tbl.ipmsbtcnt[dmix]++;
				}
				/* Add the port */
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				break;
			case GSW_IGMP_MEMBER_EXCLUDE:
				exclude_rule = 1;
			case GSW_IGMP_MEMBER_INCLUDE:
				/* Add the port */
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
				SWAPI_ASSERT(slix >=  IP_DASA_LSB_SIZE);
				hpctbl->tsub_tbl.iplsbtcnt[dlix]++;
				hpctbl->tsub_tbl.iplsbtcnt[slix]++;
				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
					SWAPI_ASSERT(smix >=  IP_DASA_MSB_SIZE);
					hpctbl->tsub_tbl.ipmsbtcnt[dmix]++;
					hpctbl->tsub_tbl.ipmsbtcnt[smix]++;
				}
				break;
			} /* end switch */
			/* Now, we write into Multicast SW Table */
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_MULTICAST_SW_INDEX;
			pt.pcindex = i;
			pt.key[1] = (hitbl->mctable[i].smsbindex << 8)
			            | hitbl->mctable[i].slsbindex;
			pt.key[0] = (hitbl->mctable[i].dmsbindex << 8)
			            | hitbl->mctable[i].dlsbindex;
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)
				pt.val[0] = (0 << parm->nPortId);
			else
				pt.val[0] = hitbl->mctable[i].pmap;
			pt.valid = hitbl->mctable[i].valid;
			pt.opmode = PCE_OPMODE_ADWR;
			s = mcast_tbl_wr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			new_entry = 1;
			if (exclude_rule == 0)
				return GSW_statusOk;
		}
	}

	/* wildcard entry for EXCLUDE rule for  port number if already exists*/
	if ((exclude_rule == 1) && (new_entry == 1)) {
		for (i = 0; i < pd->iflag.itblsize; i++) {
			/* Check if port was already exist */
			if ((hitbl->mctable[i].dlsbindex == dlix) &&
			        (hitbl->mctable[i].dmsbindex == dmix) &&
			        (hitbl->mctable[i].slsbindex == 0x7F) &&
			        (hitbl->mctable[i].smsbindex == 0x7F) &&
			        (hitbl->mctable[i].valid == 1)) {
				if (((hitbl->mctable[i].pmap >>
				        parm->nPortId) & 0x1) == 1) {
					return GSW_statusOk;
				} else {
					SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
					hpctbl->tsub_tbl.iplsbtcnt[dlix]++;
					if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
						SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
						hpctbl->tsub_tbl.ipmsbtcnt[dmix]++;
					}
					/* Add the port */
					hitbl->mctable[i].pmap |=
					    (1 << parm->nPortId);
				}
				hitbl->mctable[i].mcmode =
				    GSW_IGMP_MEMBER_DONT_CARE;
				memset(&pt, 0, sizeof(pctbl_prog_t));
				pt.table = PCE_MULTICAST_SW_INDEX;
				pt.pcindex = i;
				pt.key[1] =
				    ((hitbl->mctable[i].smsbindex << 8)
				     | (hitbl->mctable[i].slsbindex));
				pt.key[0] =
				    ((hitbl->mctable[i].dmsbindex << 8)
				     | (hitbl->mctable[i].dlsbindex));
				pt.val[0] = hitbl->mctable[i].pmap;
				pt.valid = hitbl->mctable[i].valid;
				pt.opmode = PCE_OPMODE_ADWR;
				s = mcast_tbl_wr(pdev, &pt);
				if (s != GSW_statusOk)
					return s;
				return GSW_statusOk;
			}
		}
	}
	/*	}*/
	/* Create the new DstIP & SrcIP entry */
	if (new_entry == 0) {
		if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
		        || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
			i = 0;
			while (i < pd->iflag.itblsize) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;
				i++;
			}
		} else if (parm->eModeMember == GSW_IGMP_MEMBER_DONT_CARE) {
			i = 63;
			while (i > 0) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;
				i--;
			}
		}
		if (i >= 0 && i < pd->iflag.itblsize) {
			hitbl->mctable[i].dlsbindex = dlix;
			hitbl->mctable[i].dmsbindex = dmix;
			hitbl->mctable[i].pmap |= (1 << parm->nPortId);
			SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
			if (dlflag)
				hpctbl->tsub_tbl.iplsbtcnt[dlix] = 1;
			else
				hpctbl->tsub_tbl.iplsbtcnt[dlix]++;
			if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
				SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
				if (dmflag)
					hpctbl->tsub_tbl.ipmsbtcnt[dmix] = 1;
				else
					hpctbl->tsub_tbl.ipmsbtcnt[dmix]++;
			}
			hitbl->mctable[i].valid = 1;
			hitbl->mctable[i].mcmode = parm->eModeMember;
			if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
			        || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
				hitbl->mctable[i].slsbindex = slix;
				hitbl->mctable[i].smsbindex = smix;
				SWAPI_ASSERT(slix >=  IP_DASA_LSB_SIZE);
				if (slflag)
					hpctbl->tsub_tbl.iplsbtcnt[slix] = 1;
				else
					hpctbl->tsub_tbl.iplsbtcnt[slix]++;
				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					SWAPI_ASSERT(smix >=  IP_DASA_MSB_SIZE);
					if (smflag)
						hpctbl->tsub_tbl.ipmsbtcnt[smix] = 1;
					else
						hpctbl->tsub_tbl.ipmsbtcnt[smix]++;
				}
			} else if (parm->eModeMember ==
			           GSW_IGMP_MEMBER_DONT_CARE) {
				hitbl->mctable[i].slsbindex = 0x7F;
				hitbl->mctable[i].smsbindex = 0x7F;
			}
		}
		memset(&pt, 0, sizeof(pctbl_prog_t));
		/* Now, we write into Multicast SW Table */
		pt.table = PCE_MULTICAST_SW_INDEX;
		pt.pcindex = i;
		pt.key[1] = ((hitbl->mctable[i].smsbindex << 8)
		             | hitbl->mctable[i].slsbindex);
		pt.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
		             | hitbl->mctable[i].dlsbindex);

		if ((parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE))
			pt.val[0] = (0 << parm->nPortId);
		else
			pt.val[0] = hitbl->mctable[i].pmap;
		pt.valid = hitbl->mctable[i].valid;
		pt.opmode = PCE_OPMODE_ADWR;
		s = mcast_tbl_wr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;

		if ((parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
			for (i = 0; i < pd->iflag.itblsize; i++) {
				/* Check if port was already exist */
				if ((hitbl->mctable[i].dlsbindex == dlix) &&
				        (hitbl->mctable[i].dmsbindex == dmix) &&
				        (hitbl->mctable[i].slsbindex == 0x7F) &&
				        (hitbl->mctable[i].smsbindex == 0x7F) &&
				        (hitbl->mctable[i].valid == 1)) {
					if (((hitbl->mctable[i].pmap >>
					        parm->nPortId) & 0x1) == 1)
						return GSW_statusOk;
					SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
					hpctbl->tsub_tbl.iplsbtcnt[dlix]++;
					if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
						SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
						hpctbl->tsub_tbl.ipmsbtcnt[dmix]++;
					}
					hitbl->mctable[i].mcmode =
					    GSW_IGMP_MEMBER_DONT_CARE;
					/* Add the port */
					hitbl->mctable[i].pmap |=
					    (1 << parm->nPortId);
					memset(&pt, 0, sizeof(pctbl_prog_t));
					pt.table = PCE_MULTICAST_SW_INDEX;
					pt.pcindex = i;
					pt.key[1] =
					    ((hitbl->mctable[i].smsbindex << 8)
					     | (hitbl->mctable[i].slsbindex));
					pt.key[0] =
					    ((hitbl->mctable[i].dmsbindex << 8)
					     | (hitbl->mctable[i].dlsbindex));
					pt.val[0] = hitbl->mctable[i].pmap;
					pt.valid = hitbl->mctable[i].valid;
					pt.opmode = PCE_OPMODE_ADWR;
					s = mcast_tbl_wr(pdev, &pt);
					if (s != GSW_statusOk)
						return s;
					return GSW_statusOk;
				}
			}
			i = 63;
			while (i > 0) {
				/* Find a new empty entry to add */
				if (hitbl->mctable[i].valid == 0)
					break;
				i--;
			}
			if (i >= 0 && i < pd->iflag.itblsize) {
				/* Now, we write into Multicast SW Table */
				hitbl->mctable[i].dlsbindex = dlix;
				hitbl->mctable[i].dmsbindex = dmix;
				hitbl->mctable[i].slsbindex = 0x7F;
				hitbl->mctable[i].smsbindex = 0x7F;
				hitbl->mctable[i].pmap |= (1 << parm->nPortId);
				hitbl->mctable[i].mcmode = GSW_IGMP_MEMBER_DONT_CARE;
				hitbl->mctable[i].valid = 1;
				SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
				hpctbl->tsub_tbl.iplsbtcnt[dlix]++;
				if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
					SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
					hpctbl->tsub_tbl.ipmsbtcnt[dmix]++;
				}
				memset(&pt, 0, sizeof(pctbl_prog_t));
				pt.table = PCE_MULTICAST_SW_INDEX;
				pt.pcindex = i;
				pt.key[1] = ((hitbl->mctable[i].smsbindex << 8)
				             | hitbl->mctable[i].slsbindex);
				pt.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
				             | hitbl->mctable[i].dlsbindex);
				pt.val[0] = hitbl->mctable[i].pmap;
				pt.valid = hitbl->mctable[i].valid;
				pt.opmode = PCE_OPMODE_ADWR;
				s = mcast_tbl_wr(pdev, &pt);
				if (s != GSW_statusOk)
					return s;
			} else {
				GSW_PRINT("%s:%s:%d (IGMP Table full)\n",
				          __FILE__, __func__, __LINE__);
			}
		}
	}
	/* Debug */
	return GSW_statusOk;
}

/* Multicast Software Table Include/Exclude Remove function */
static int gsw2x_msw_table_rm(void *pdev,
                              GSW_multicastTable_t *parm)
{
	GSW_return_t s;
	u8	i, j;
	ltq_bool_t MATCH = 0;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	tft_tbl_t *hpctbl = &pd->ptft;
	gsw_igmp_t *hitbl = &pd->iflag;
	pctbl_prog_t pt;
	ip_dasa_t ltbl;
	ip_dasa_t mtbl;
	int dlix = 0x7F, dmix = 0x7F, slix = 0x7F, smix = 0x7F;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	memset(&ltbl, 0, sizeof(ip_dasa_t));
	memset(&mtbl, 0, sizeof(ip_dasa_t));
	if ((parm->eIPVersion != GSW_IP_SELECT_IPV4)
	        && (parm->eIPVersion != GSW_IP_SELECT_IPV6)) {
		GSW_PRINT("%s:%s:%d (IPv4/IPV6 need to enable!!!)\n",
		          __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	        && (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)
	        && (parm->eModeMember == GSW_IGMP_MEMBER_DONT_CARE)) {
		GSW_PRINT("%s:%s:%d (!!!)\n", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0; i < 4; i++)
			ltbl.ip_val[i] = ((parm->uIP_Gda.nIPv4 >> (i * 8)) & 0xFF);
		/* DIP LSB Nibble Mask */
		ltbl.ip_mask[0] = 0xFF00;
	}
	if (parm->eIPVersion == GSW_IP_SELECT_IPV6 /* IPv6 */) {
		/* First, search for DIP in the DA/SA table (DIP MSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			mtbl.ip_val[j-1] = (parm->uIP_Gda.nIPv6[i] & 0xFF);
			mtbl.ip_val[j] = ((parm->uIP_Gda.nIPv6[i] >> 8) & 0xFF);
		}
		mtbl.ip_mask[0] = 0;/* DIP MSB Nibble Mask */
		mtbl.ip_valid = 1;
		dmix = find_msb_tbl_entry(&hpctbl->tsub_tbl, &mtbl);
		if (dmix == 0xFF) {
			GSW_PRINT("%s:%s:%d (IGMP Entry not found)\n",
			          __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
		/* First, search for DIP in the DA/SA table (DIP LSB) */
		for (i = 0, j = 7; i < 4; i++, j -= 2) {
			ltbl.ip_val[j-1] = (parm->uIP_Gda.nIPv6[i+4] & 0xFF);
			ltbl.ip_val[j] = ((parm->uIP_Gda.nIPv6[i+4] >> 8) & 0xFF);
		}
		ltbl.ip_mask[0] = 0;/* DIP LSB Nibble Mask */
	}
	ltbl.ip_valid = 1;
	dlix = find_dasa_tbl_entry(&hpctbl->tsub_tbl, &ltbl);
	if (dlix == 0xFF) {
		GSW_PRINT("%s:%s:%d (IGMP Entry not found)\n",
		          __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	if ((parm->eModeMember == GSW_IGMP_MEMBER_INCLUDE)
	        || (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE)) {
		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0; i < 4; i++)
				ltbl.ip_val[i] = ((parm->uIP_Gsa.nIPv4 >> (i * 8)) & 0xFF);
			/* DIP LSB Nibble Mask */
			ltbl.ip_mask[0] = 0xFF00;
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if (ltbl.ip_val[3] == 0 && ltbl.ip_val[2] == 0
				        && ltbl.ip_val[1] == 0 && ltbl.ip_val[0] == 0) {
					GSW_PRINT("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
					          __FILE__, __func__, __LINE__);
					return GSW_statusErr;
				}
			}
		}
		if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			int src_zero = 0;
			/* First, search for DIP in the DA/SA table (DIP MSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				mtbl.ip_val[j-1] = (parm->uIP_Gsa.nIPv6[i] & 0xFF);
				mtbl.ip_val[j] = ((parm->uIP_Gsa.nIPv6[i] >> 8) & 0xFF);
			}
			mtbl.ip_mask[0] = 0;/* DIP MSB Nibble Mask */
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((mtbl.ip_val[0] == 0) &&
				        (mtbl.ip_val[1] == 0) &&
				        (mtbl.ip_val[2] == 0) &&
				        (mtbl.ip_val[3] == 0) &&
				        (mtbl.ip_val[4] == 0) &&
				        (mtbl.ip_val[5] == 0) &&
				        (mtbl.ip_val[6] == 0) &&
				        (mtbl.ip_val[7] == 0))
					src_zero = 1;
			}
			mtbl.ip_valid = 1;
			smix = find_msb_tbl_entry(&hpctbl->tsub_tbl,
			                          &mtbl);
			if (smix == 0xFF) {
				GSW_PRINT("%s:%s:%d (IGMP Entry not found)\n",
				          __FILE__, __func__, __LINE__);
				return GSW_statusErr;
			}
			/* First, search for DIP in the DA/SA table (DIP LSB) */
			for (i = 0, j = 7; i < 4; i++, j -= 2) {
				ltbl.ip_val[j-1] = (parm->uIP_Gsa.nIPv6[i+4] & 0xFF);
				ltbl.ip_val[j] = ((parm->uIP_Gsa.nIPv6[i+4] >> 8) & 0xFF);
			}
			ltbl.ip_mask[0] = 0;/* DIP LSB Nibble Mask */
			if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
				if ((ltbl.ip_val[0] == 0) &&
				        (ltbl.ip_val[1] == 0) &&
				        (ltbl.ip_val[2] == 0) &&
				        (ltbl.ip_val[3] == 0) &&
				        (ltbl.ip_val[4] == 0) &&
				        (ltbl.ip_val[5] == 0) &&
				        (ltbl.ip_val[6] == 0) &&
				        (ltbl.ip_val[7] == 0)) {
					if (src_zero) {
						GSW_PRINT("%s:%s:%d (Exclude Rule Source IP is Wildcard)\n",
						          __FILE__, __func__, __LINE__);
						return GSW_statusErr;
					}
				}
			}
		}
		ltbl.ip_valid = 1;
		slix = find_dasa_tbl_entry(&hpctbl->tsub_tbl, &ltbl);
		if (slix == 0xFF) {
			GSW_PRINT("%s:%s:%d (IGMP Entry not found)\n",
			          __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}
	}
	for (i = 0; i < pd->iflag.itblsize; i++) {
		if ((hitbl->mctable[i].dlsbindex == dlix) &&
		        (hitbl->mctable[i].slsbindex == slix) &&
		        (hitbl->mctable[i].dmsbindex == dmix) &&
		        (hitbl->mctable[i].smsbindex == smix) &&
		        (hitbl->mctable[i].valid == 1)) {

			switch (hitbl->mctable[i].mcmode) {
			case GSW_IGMP_MEMBER_DONT_CARE:
				if (((hitbl->mctable[i].pmap >> parm->nPortId) & 0x1) == 1) {
					hitbl->mctable[i].pmap &= ~(1 << parm->nPortId);
					SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
					if (hpctbl->tsub_tbl.iplsbtcnt[dlix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->tsub_tbl, dlix);
						if (hpctbl->tsub_tbl.iplsbtcnt[dlix] == 0) {
							/* Delet the sub table */
							ip_dasa_lsb_tdel(pdev,
							                 &hpctbl->tsub_tbl, dlix);
						}
					}
					/* Delet the sub table */
					SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
					if (hpctbl->tsub_tbl.ipmsbtcnt[dmix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->tsub_tbl, dmix);
						if (hpctbl->tsub_tbl.ipmsbtcnt[dmix] == 0) {
							if (parm->eIPVersion == GSW_IP_SELECT_IPV6)
								ip_dasa_msb_tdel(pdev,
								                 &hpctbl->tsub_tbl, dmix);
						}
					}
					/* Check the port map status */
					/* Delet the entry from Multicast sw Table */
					if (hitbl->mctable[i].pmap == 0)
						hitbl->mctable[i].valid = 0;
					MATCH = 1;
				}
				break;
			case GSW_IGMP_MEMBER_INCLUDE:
			case GSW_IGMP_MEMBER_EXCLUDE:
				if (((hitbl->mctable[i].pmap >> parm->nPortId) & 0x1) == 1) {
					hitbl->mctable[i].pmap &= ~(1 << parm->nPortId);
					SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
					if (hpctbl->tsub_tbl.iplsbtcnt[dlix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->tsub_tbl, dlix);
						/* Delet the sub table */
						if (hpctbl->tsub_tbl.iplsbtcnt[dlix] == 0) {
							ip_dasa_lsb_tdel(pdev,
							                 &hpctbl->tsub_tbl, dlix);
						}
					}
					SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
					if (hpctbl->tsub_tbl.ipmsbtcnt[dmix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->tsub_tbl, dmix);
						/* Delet the sub table */
						if (hpctbl->tsub_tbl.ipmsbtcnt[dmix] == 0) {
							ip_dasa_msb_tdel(pdev,
							                 &hpctbl->tsub_tbl, dmix);
						}
					}
					SWAPI_ASSERT(slix >=  IP_DASA_LSB_SIZE);
					if (hpctbl->tsub_tbl.iplsbtcnt[slix] > 0) {
						ipdslsb_tblidx_del(&hpctbl->tsub_tbl, slix);
						/* Delet the sub table */
						if (hpctbl->tsub_tbl.iplsbtcnt[slix] == 0) {
							ip_dasa_lsb_tdel(pdev,
							                 &hpctbl->tsub_tbl, slix);
						}
					}
					SWAPI_ASSERT(smix >=  IP_DASA_MSB_SIZE);
					if (hpctbl->tsub_tbl.ipmsbtcnt[smix] > 0) {
						ipdsmsb_tblidx_del(&hpctbl->tsub_tbl, smix);
						/* Delet the sub table */
						if (hpctbl->tsub_tbl.ipmsbtcnt[smix] == 0) {
							ip_dasa_msb_tdel(pdev,
							                 &hpctbl->tsub_tbl, smix);
						}
					}
					/* Check the port map status */
					/* Delet the entry from Multicast sw Table */
					if (hitbl->mctable[i].pmap == 0)
						hitbl->mctable[i].valid = 0;

					MATCH = 1;
					if (parm->eModeMember == GSW_IGMP_MEMBER_EXCLUDE) {
						for (j = 0; j < pd->iflag.itblsize; j++) {
							if ((hitbl->mctable[j].dlsbindex == dlix) &&
							        (hitbl->mctable[j].slsbindex == 0x7F) &&
							        (hitbl->mctable[j].dmsbindex == dmix) &&
							        (hitbl->mctable[j].smsbindex == 0x7F) &&
							        (hitbl->mctable[j].valid == 1)) {
								if (((hitbl->mctable[j].pmap >> parm->nPortId) & 0x1) == 1) {
									hitbl->mctable[j].pmap &= ~(1 << parm->nPortId);
									SWAPI_ASSERT(dlix >=  IP_DASA_LSB_SIZE);
									if (hpctbl->tsub_tbl.iplsbtcnt[dlix] > 0) {
										ipdslsb_tblidx_del(&hpctbl->tsub_tbl, dlix);
										if (hpctbl->tsub_tbl.iplsbtcnt[dlix] == 0) {
											/* Delet the sub table */
											ip_dasa_lsb_tdel(pdev,
											                 &hpctbl->tsub_tbl, dlix);
										}
									}
									SWAPI_ASSERT(dmix >=  IP_DASA_MSB_SIZE);
									if (hpctbl->tsub_tbl.ipmsbtcnt[dmix] > 0) {
										ipdsmsb_tblidx_del(&hpctbl->tsub_tbl, dmix);
										if (hpctbl->tsub_tbl.ipmsbtcnt[dmix] == 0) {
											/* Delet the sub table */
											ip_dasa_msb_tdel(pdev,
											                 &hpctbl->tsub_tbl, dmix);
										}
									}
									/* Check the port map status */
									if (hitbl->mctable[j].pmap == 0) {
										/* Delet the entry from Multicast sw Table */
										hitbl->mctable[j].valid = 0;
										hitbl->mctable[i].valid = 0;
									}
									memset(&pt, 0, sizeof(pctbl_prog_t));
									pt.table = PCE_MULTICAST_SW_INDEX;
									pt.pcindex = j;
									pt.key[1] = ((0x7F << 8) | 0x7F);
									pt.key[0] = ((hitbl->mctable[j].dmsbindex << 8)
									             | (hitbl->mctable[i].dlsbindex));
									pt.val[0] = hitbl->mctable[j].pmap;
									pt.valid = hitbl->mctable[j].valid;
									pt.opmode = PCE_OPMODE_ADWR;
									s = mcast_tbl_wr(pdev, &pt);
									if (s != GSW_statusOk)
										return s;
								}
							}
						}
					}
				}
				break;
			}
			if (MATCH == 1) {
				memset(&pt, 0, sizeof(pctbl_prog_t));
				pt.table = PCE_MULTICAST_SW_INDEX;
				pt.pcindex = i;
				pt.key[1] = ((hitbl->mctable[i].smsbindex << 8)
				             | (hitbl->mctable[i].slsbindex));
				pt.key[0] = ((hitbl->mctable[i].dmsbindex << 8)
				             | (hitbl->mctable[i].dlsbindex));
				pt.val[0] = hitbl->mctable[i].pmap;
				pt.valid = hitbl->mctable[i].valid;
				pt.opmode = PCE_OPMODE_ADWR;
				s = mcast_tbl_wr(pdev, &pt);
				if (s != GSW_statusOk)
					return s;
			}
		}
	}
	if (MATCH == 0)
		GSW_PRINT("The GIP/SIP not found\n");
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastRouterPortAdd(void *pdev,
                                        GSW_multicastRouter_t *parm)
{
	GSW_return_t s;
	u32 v;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t	*pd = (ethsw_api_dev_t *)pdev;
	gsw_igmp_t *hitbl = &pd->iflag;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pd, pi);
	if (s != GSW_statusOk)
		return s;
	/* Read the Default Router Port Map - DRPM */
	s = gsw_reg_rd(pd, PCE_IGMP_DRPM_DRPM_OFFSET,
	               PCE_IGMP_DRPM_DRPM_SHIFT,
	               PCE_IGMP_DRPM_DRPM_SIZE, &v);
	if (s != GSW_statusOk)
		return s;
	if (((v >> pi) & 0x1) == 1) {
		/*"Error: the prot was already in the member */
		return GSW_statusParam;
	} else {
		v = (v | (1 << pi));
		/* Write the Default Router Port Map - DRPM  */
		s = gsw_reg_wr(pd, PCE_IGMP_DRPM_DRPM_OFFSET,
		               PCE_IGMP_DRPM_DRPM_SHIFT,
		               PCE_IGMP_DRPM_DRPM_SIZE, v);
		if (s != GSW_statusOk)
			return s;
	}
	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		GSW_PCE_rule_t pr;
		u8 i;
		for (i = 0; i < 2; i++) {
			memset(&pr, 0, sizeof(GSW_PCE_rule_t));
			pr.pattern.bEnable = 1;
			pr.pattern.bProtocolEnable = 1;
			switch (i) {
			case 0:
				/*	Management port remaining IGMP packets (forwarding */
				/* them to Router Ports) */
				pr.pattern.nIndex = MPCE_RULES_INDEX;
				pr.pattern.nProtocol = 0x2; /* for IPv4 */
				pr.pattern.bAppMaskRangeMSB_Select = 1;
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.nAppDataMSB = 0x1200;
				pr.pattern.nAppMaskRangeMSB	= 0x1DFF;
				break;
			case 1:
				/* Management Port ICMPv6 Multicast Listerner Report */
				/* & Leave (Avoiding Loopback abd Discard) */
				pr.pattern.nIndex = MPCE_RULES_INDEX + 3;
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.bAppMaskRangeMSB_Select = 1;
				pr.pattern.nAppDataMSB = 0x8300;
				pr.pattern.nAppMaskRangeMSB	= 0x1FF;
				pr.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				pr.action.nForwardPortMap = v;
				break;
			}
			/* Router portmap */
			pr.action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			pr.action.nForwardPortMap = v;
			if (hitbl->igcos != 0) {
				pr.action.eTrafficClassAction = 1;
				pr.action.nTrafficClassAlternate = pd->iflag.igcos;
			}
			/*  Set eForwardPort */
			pr.pattern.bPortIdEnable = 1;
			if (hitbl->igfport == GSW_PORT_FORWARD_PORT)
				pr.pattern.nPortId = hitbl->igfpid;
			else if (hitbl->igfport == GSW_PORT_FORWARD_CPU)
				pr.pattern.nPortId = (pd->cport);

			if (hitbl->igxvlan)
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_DISABLE;
			/* We prepare everything and write into PCE Table */
			if (0 != pce_rule_write(pd, &pd->ptft, &pr))
				return GSW_statusErr;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastRouterPortRead(void *pdev,
        GSW_multicastRouterRead_t *parm)
{
	GSW_return_t s;
	u32 v1, v2;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (parm->bInitial == 1) {
		/* Read the Default Router Port Map - DRPM*/
		s = gsw_reg_rd(pd, PCE_IGMP_DRPM_DRPM_OFFSET,
		               PCE_IGMP_DRPM_DRPM_SHIFT,
		               PCE_IGMP_DRPM_DRPM_SIZE, &v1);
		if (s != GSW_statusOk)
			return s;
		/* Read the Default Router Port Map - IGPM */
		s = gsw_reg_rd(pd, PCE_IGMP_STAT_IGPM_OFFSET,
		               PCE_IGMP_STAT_IGPM_SHIFT,
		               PCE_IGMP_STAT_IGPM_SIZE, &v2);
		if (s != GSW_statusOk)
			return s;
		pd->iflag.igrport = (v1 | v2);
		parm->bInitial = 0;
		pd->mrtpcnt = 0;
	}
	if (parm->bLast == 0) {
		/* Need to clarify the different between DRPM & IGPM */
		while (((pd->iflag.igrport >>
		         pd->mrtpcnt) & 0x1) == 0) {
			pd->mrtpcnt++;
			if (pd->mrtpcnt > (pd->tpnum-1)) {
				parm->bLast = 1;
				return GSW_statusOk;
			}
		}
		parm->nPortId = pd->mrtpcnt;
		if (pd->mrtpcnt < pd->tpnum)
			pd->mrtpcnt++;
		else
			parm->bLast = 1;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastRouterPortRemove(void *pdev,
        GSW_multicastRouter_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t	*pd = (ethsw_api_dev_t *)pdev;
	gsw_igmp_t *hitbl	= &pd->iflag;
	u32 v1, v2;
	u8 pi = parm->nPortId;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pd, pi);
	if (s != GSW_statusOk)
		return s;
	/* Read the Default Router Port Map - DRPM */
	s = gsw_reg_rd(pd, PCE_IGMP_DRPM_DRPM_OFFSET,
	               PCE_IGMP_DRPM_DRPM_SHIFT,
	               PCE_IGMP_DRPM_DRPM_SIZE, &v1);
	if (s != GSW_statusOk)
		return s;
	/* Read the Default Router Port Map - IGPM */
	s = gsw_reg_rd(pd, PCE_IGMP_STAT_IGPM_OFFSET,
	               PCE_IGMP_STAT_IGPM_SHIFT,
	               PCE_IGMP_STAT_IGPM_SIZE, &v2);
	if (s != GSW_statusOk)
		return s;
	if (((v1 >> pi) & 0x1) == 0) {
		GSW_PRINT("Error: the port was not in the member\n");
		return GSW_statusOk;
	} else {
		v1 = (v1 & ~(1 << pi));
		/* Write the Default Router Port Map - DRPM*/
		s = gsw_reg_wr(pd, PCE_IGMP_DRPM_DRPM_OFFSET,
		               PCE_IGMP_DRPM_DRPM_SHIFT,
		               PCE_IGMP_DRPM_DRPM_SIZE, v1);
		if (s != GSW_statusOk)
			return s;
	}
	if ((hitbl->igmode ==	GSW_MULTICAST_SNOOP_MODE_FORWARD) & v1) {
		GSW_PCE_rule_t pr;
		int i;
		for (i = 0; i < 2; i++) {
			memset(&pr, 0, sizeof(GSW_PCE_rule_t));
			pr.pattern.bEnable = 1;
			pr.pattern.bProtocolEnable = 1;
			switch (i) {
			case 0:
				/*	Management port remaining IGMP packets */
				/*(forwarding them to Router Ports) */
				pr.pattern.nIndex = MPCE_RULES_INDEX;
				/* for IPv4 */
				pr.pattern.nProtocol = 0x2;
				pr.pattern.bAppMaskRangeMSB_Select = 1;
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.nAppDataMSB = 0x1200;
				pr.pattern.nAppMaskRangeMSB	= 0x1DFF;
				break;
			case 1:
				/* Management Port ICMPv6 Multicast Listerner Report */
				/* & Leave (Avoiding Loopback abd Discard) */
				pr.pattern.nIndex = MPCE_RULES_INDEX+3;
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.bAppMaskRangeMSB_Select = 1;
				pr.pattern.nAppDataMSB = 0x8300;
				pr.pattern.nAppMaskRangeMSB	= 0x1FF;
				/*for IPv6*/
				pr.pattern.nProtocol = 0x3A;
				break;
			}
			/* Router portmap */
			pr.action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			pr.action.nForwardPortMap = v1;
			if (hitbl->igcos != 0) {
				pr.action.eTrafficClassAction = 1;
				pr.action.nTrafficClassAlternate =
				    pd->iflag.igcos;
			}
			/*  Set eForwardPort */
			pr.pattern.bPortIdEnable = 1;
			if (hitbl->igfport == GSW_PORT_FORWARD_PORT)
				pr.pattern.nPortId = hitbl->igfpid;
			else if (hitbl->igfport == GSW_PORT_FORWARD_CPU)
				pr.pattern.nPortId = (pd->cport);
			if (hitbl->igxvlan)
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_DISABLE;
			/* We prepare everything and write into PCE Table */
			if ((pce_rule_write(pd, &pd->ptft, &pr)) != 0)
				return GSW_statusErr;
		}
	} else if ((hitbl->igmode ==
	            GSW_MULTICAST_SNOOP_MODE_FORWARD) & !v1) {
		GSW_PCE_rule_t pr;
		int i;
		for (i = 0; i < 2; i++) {
			memset(&pr, 0, sizeof(GSW_PCE_rule_t));
			switch (i) {
			case 0:
				pr.pattern.nIndex = MPCE_RULES_INDEX;
				break;
			case 1:
				/* Management Port ICMPv6 Multicast Listerner Report */
				/* & Leave (Avoiding Loopback abd Discard) */
				pr.pattern.nIndex =
				    MPCE_RULES_INDEX + 3;
				break;
			}
			if ((pce_pattern_delete(pd, &pd->ptft,
			                        pr.pattern.nIndex)) != 0)
				return GSW_statusErr;
		}
	}

	return GSW_statusOk;
}

GSW_return_t GSW_MulticastSnoopCfgGet(void *pdev,
                                      GSW_multicastSnoopCfg_t *parm)
{
	u32 d0, d1, v;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	parm->eIGMP_Mode = pd->iflag.igmode;
	parm->bIGMPv3 = pd->iflag.igv3;
	parm->bCrossVLAN = pd->iflag.igxvlan;
	parm->eForwardPort = pd->iflag.igfport;
	parm->nForwardPortId = pd->iflag.igfpid;
	parm->nClassOfService = pd->iflag.igcos;
	s = gsw_reg_rd(pd, PCE_IGMP_CTRL_ROB_OFFSET,
	               PCE_IGMP_CTRL_ROB_SHIFT,
	               PCE_IGMP_CTRL_ROB_SIZE, &v);
	if (s != GSW_statusOk)
		return s;
	parm->nRobust = v;
	s = gsw_reg_rd(pd, PCE_IGMP_CTRL_DMRT_OFFSET,
	               PCE_IGMP_CTRL_DMRT_SHIFT,
	               PCE_IGMP_CTRL_DMRT_SIZE, &v);
	if (s != GSW_statusOk)
		return s;
	parm->nQueryInterval = v;
	s = gsw_reg_rd(pd, PCE_IGMP_CTRL_REPSUP_OFFSET,
	               PCE_IGMP_CTRL_REPSUP_SHIFT,
	               PCE_IGMP_CTRL_REPSUP_SIZE, &d0);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pd, PCE_IGMP_CTRL_JASUP_OFFSET,
	               PCE_IGMP_CTRL_JASUP_SHIFT,
	               PCE_IGMP_CTRL_JASUP_SIZE, &d1);
	if (s != GSW_statusOk)
		return s;
	if (d0 == 0 && d1 == 0)
		parm->eSuppressionAggregation = GSW_MULTICAST_TRANSPARENT;
	else if (d0 == 1 && d1 == 0)
		parm->eSuppressionAggregation = GSW_MULTICAST_REPORT;
	else if (d0 == 1 && d1 == 1)
		parm->eSuppressionAggregation = GSW_MULTICAST_REPORT_JOIN;
	else
		parm->eSuppressionAggregation = GSW_MULTICAST_TRANSPARENT;
	s = gsw_reg_rd(pd, PCE_IGMP_CTRL_FLEAVE_OFFSET,
	               PCE_IGMP_CTRL_FLEAVE_SHIFT,
	               PCE_IGMP_CTRL_FLEAVE_SIZE, &v);
	if (s != GSW_statusOk)
		return s;
	if (v == 1)
		parm->bFastLeave = 1;
	else
		parm->bFastLeave = 0;
	s = gsw_reg_rd(pd, PCE_IGMP_CTRL_SRPEN_OFFSET,
	               PCE_IGMP_CTRL_SRPEN_SHIFT,
	               PCE_IGMP_CTRL_SRPEN_SIZE, &v);
	if (s != GSW_statusOk)
		return s;
	parm->bLearningRouter = v;
	s = gsw_reg_rd(pd, PCE_GCTRL_1_UKIPMC_OFFSET,
	               PCE_GCTRL_1_UKIPMC_SHIFT,
	               PCE_GCTRL_1_UKIPMC_SIZE,
	               &parm->bMulticastUnknownDrop);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastSnoopCfgSet(void *pdev,
                                      GSW_multicastSnoopCfg_t *parm)
{
	GSW_return_t s;
	GSW_PCE_rule_t pr;
	u32 i, d0 = 0, d1 = 0, pmcindex = MPCE_RULES_INDEX;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/* Choose IGMP Mode */
	switch (parm->eIGMP_Mode) {
	case GSW_MULTICAST_SNOOP_MODE_DISABLED:
		/* Snooping of Router Port Disable */
		s = gsw_reg_wr(pd,
		               PCE_IGMP_CTRL_SRPEN_OFFSET,
		               PCE_IGMP_CTRL_SRPEN_SHIFT,
		               PCE_IGMP_CTRL_SRPEN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pd, PCE_GCTRL_0_IGMP_OFFSET,
		               PCE_GCTRL_0_IGMP_SHIFT,
		               PCE_GCTRL_0_IGMP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		for (i = 0; i <= pd->tpnum; i++) {
			s = gsw_reg_wr(pd,
			               PCE_PCTRL_0_MCST_OFFSET + (0xA * i),
			               PCE_PCTRL_0_MCST_SHIFT,
			               PCE_PCTRL_0_MCST_SIZE, 0);
			if (s != GSW_statusOk)
				return s;
		}
		break;
	case GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING:
		/* Snooping of Router Port Enable */
		s = gsw_reg_wr(pd, PCE_GCTRL_0_IGMP_OFFSET,
		               PCE_GCTRL_0_IGMP_SHIFT,
		               PCE_GCTRL_0_IGMP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_SRPEN_OFFSET,
		               PCE_IGMP_CTRL_SRPEN_SHIFT,
		               PCE_IGMP_CTRL_SRPEN_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		for (i = 0; i <= pd->tpnum; i++) {
			s = gsw_reg_wr(pd, PCE_PCTRL_0_MCST_OFFSET + (0xA * i),
			               PCE_PCTRL_0_MCST_SHIFT,
			               PCE_PCTRL_0_MCST_SIZE, 1);
			if (s != GSW_statusOk)
				return s;
		}
		break;
	case GSW_MULTICAST_SNOOP_MODE_FORWARD:
		/* Snooping of Router Port Forward */
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_SRPEN_OFFSET,
		               PCE_IGMP_CTRL_SRPEN_SHIFT,
		               PCE_IGMP_CTRL_SRPEN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pd, PCE_GCTRL_0_IGMP_OFFSET,
		               PCE_GCTRL_0_IGMP_SHIFT,
		               PCE_GCTRL_0_IGMP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		for (i = 0; i <= pd->tpnum; i++) {
			s = gsw_reg_wr(pd, PCE_PCTRL_0_MCST_OFFSET + (0xA * i),
			               PCE_PCTRL_0_MCST_SHIFT,
			               PCE_PCTRL_0_MCST_SIZE, 1);
			if (s != GSW_statusOk)
				return s;
		}
		break;
	default:
		GSW_PRINT("This Mode doesn't exists\n");
		return GSW_statusErr;
	}
	/* Set the Flag for eIGMP_Mode flag*/
	pd->iflag.igmode = parm->eIGMP_Mode;
	/* Set bIGMPv3 flag*/
	pd->iflag.igv3 =  parm->bIGMPv3;
	/* Set bCrossVLAN flag*/
	pd->iflag.igxvlan = parm->bCrossVLAN;
	/* Set eForwardPort flag */
	pd->iflag.igfport = parm->eForwardPort;
	/* Set nForwardPortId */
	if (parm->eForwardPort == GSW_PORT_FORWARD_CPU)
		pd->iflag.igfpid = (1 << pd->cport);
	else
		pd->iflag.igfpid = parm->nForwardPortId;
	pd->iflag.igcos = parm->nClassOfService;
	/* If IGMP mode set to AutoLearning then the following Rule have to add it */
	if (parm->eIGMP_Mode ==
	        GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		for (i = pmcindex; i <= (pmcindex + 7); i++) {
			memset(&pr, 0, sizeof(GSW_PCE_rule_t));
			pr.pattern.nIndex = i;
			pr.pattern.bEnable = 1;
			pr.pattern.bAppDataMSB_Enable = 1;
			if ((i == pmcindex + 0) ||
			        (i == pmcindex + 1) ||
			        (i == pmcindex + 2))
				pr.pattern.nAppDataMSB = 0x1100;
			else if (i == pmcindex + 3)
				pr.pattern.nAppDataMSB = 0x1200;
			else if (i == pmcindex + 4)
				pr.pattern.nAppDataMSB = 0x1600;
			else if (i == pmcindex + 5)
				pr.pattern.nAppDataMSB = 0x1700;
			else if (i == pmcindex + 6)
				pr.pattern.nAppDataMSB = 0x3100;
			else if (i == pmcindex + 7)
				pr.pattern.nAppDataMSB = 0x3000;

			pr.pattern.bAppMaskRangeMSB_Select = 0;
			pr.pattern.nAppMaskRangeMSB = 0x3;
			if ((i == pmcindex + 0) ||
			        (i == pmcindex + 1) ||
			        (i == pmcindex + 6) ||
			        (i == pmcindex + 7))
				pr.pattern.eDstIP_Select = 1;
			if ((i == pmcindex + 0) ||
			        (i == pmcindex + 1))
				pr.pattern.nDstIP.nIPv4 = 0xE0000001;
			else if (i == pmcindex + 6)
				pr.pattern.nDstIP.nIPv4 = 0xE0000002;
			else if (i == pmcindex + 7)
				pr.pattern.nDstIP.nIPv4 = 0xE00000A6;
			pr.pattern.nDstIP_Mask = 0xFF00;
			if (i == pmcindex + 1)
				pr.pattern.eSrcIP_Select = 1;
			else
				pr.pattern.eSrcIP_Select = 0;
			if (i == pmcindex + 1)
				pr.pattern.nSrcIP_Mask = 0xFF00;
			else
				pr.pattern.nSrcIP_Mask = 0xFFFF;
			pr.pattern.bProtocolEnable = 1;
			pr.pattern.nProtocol = 0x2;
			if (pd->iflag.igcos == 0) {
				pr.action.eTrafficClassAction = 0;
				pr.action.nTrafficClassAlternate = 0;
			} else {
				pr.action.eTrafficClassAction = 1;
				pr.action.nTrafficClassAlternate =
				    pd->iflag.igcos;
			}
			if (i == pmcindex + 0)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_QUERY;
			else if (i == pmcindex + 1)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER;
			else if (i == pmcindex + 2)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP;
			else if (i == pmcindex + 3)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_REPORT;
			else if (i == pmcindex + 4)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_REPORT;
			else if (i == pmcindex + 5)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_LEAVE;
			else if (i == pmcindex + 6)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_AD;
			else if (i == pmcindex + 7)
				pr.action.eSnoopingTypeAction =
				    GSW_PCE_ACTION_IGMP_SNOOP_AD;
			pr.action.ePortMapAction =
			    GSW_PCE_ACTION_PORTMAP_MULTICAST_ROUTER;
			if (parm->bCrossVLAN)
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_DISABLE;
			/* We prepare everything and write into PCE Table */
			if (0 != pce_rule_write(pd,
			                        &pd->ptft, &pr))
				return GSW_statusErr;
		}
	}
	/* If IGMP mode set to forwarding then the */
	/* following Rule have to add it */
	if (parm->eIGMP_Mode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		for (i = pmcindex; i <= (pmcindex + 7); i++) {
			memset(&pr, 0, sizeof(GSW_PCE_rule_t));
			pr.pattern.nIndex = i;
			pr.pattern.bEnable = 1;
			pr.pattern.bProtocolEnable = 1;
			switch (i - pmcindex) {
			/*		case 0: */
			/*Rule added by Router port ADD function based on router port for IPv4*/
			/*					break; */
			case 1:
				/*	Avoid IGMP Packets Redirection when seen on Management Port */
				pr.pattern.nProtocol = 0x2; /* for IPv4 */
				pr.pattern.bPortIdEnable = 1;
				/* Action Enabled, no redirection (default portmap) */
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_REGULAR;
				break;
			case 2:
				/* IGMPv1/2/3 IPv4 */
				pr.pattern.nProtocol = 0x2; /* for IPv4 */
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;
			/*		case 3: */
			/*Rules added by Router port ADD function */
			/* based on router port for IPv6 */
			/*			break; */
			case 4:
				/*	Managemnt Port Remaining ICMPv6/MLD packets */
				/* (Avoiding Loopback and Disacard) */
				pr.pattern.bPortIdEnable = 1;
				pr.pattern.nPortId = parm->nForwardPortId;
				pr.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pr.pattern.bPortIdEnable = 1;
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_REGULAR;
				break;
			case 5:
				/* ICMPv6 Multicast Listener Query/Report/Done(Leave) */
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.bAppMaskRangeMSB_Select = 1;
				pr.pattern.nAppDataMSB = 0x8200;
				pr.pattern.nAppMaskRangeMSB	= 0x2FF;
				pr.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;
			case 6:
				/* ICMPv6 Multicast Listener Report */
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.nAppDataMSB = 0x8F00;
				pr.pattern.nAppMaskRangeMSB = 0x3;
				pr.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;
			case 7:
				/* ICMPv6 Multicast Router Advertisement/Solicitation/Termination */
				pr.pattern.bAppDataMSB_Enable	= 1;
				pr.pattern.bAppMaskRangeMSB_Select = 1;
				pr.pattern.nAppDataMSB = 0x9700;
				pr.pattern.nAppMaskRangeMSB	= 0x2FF;
				pr.pattern.nProtocol = 0x3A;  /*for IPv6*/
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
				break;
			default:
				continue;
			}
			if (pd->iflag.igcos != 0) {
				pr.action.eTrafficClassAction = 1;
				pr.action.nTrafficClassAlternate =
				    pd->iflag.igcos;
			}
			/*  Set eForwardPort */
			if (parm->eForwardPort == GSW_PORT_FORWARD_PORT) {
				pr.action.nForwardPortMap =
				    (1 << parm->nForwardPortId);
				pr.pattern.nPortId = parm->nForwardPortId;
			} else if (parm->eForwardPort == GSW_PORT_FORWARD_CPU) {
				pr.action.nForwardPortMap =
				    (1 << pd->cport);
				pr.pattern.nPortId = pd->cport;
			}
			if (parm->bCrossVLAN)
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_CROSS;
			else
				pr.action.eVLAN_CrossAction =
				    GSW_PCE_ACTION_CROSS_VLAN_DISABLE;
			/* We prepare everything and write into PCE Table */
			if ((pce_rule_write(pd,
			                    &pd->ptft, &pr)) != 0)
				return GSW_statusErr;
		}

	}
	if (parm->eIGMP_Mode ==
	        GSW_MULTICAST_SNOOP_MODE_DISABLED) {
		pmcindex = MPCE_RULES_INDEX;
		for (i = pmcindex; i <= (pmcindex + 7); i++) {
			pr.pattern.nIndex = i;
			pr.pattern.bEnable = 0;
			/* We prepare everything and write into PCE Table */
			if (0 != pce_pattern_delete(pd, &pd->ptft, i))
				return GSW_statusErr;
		}
	}
	if (parm->nRobust < 4) {
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_ROB_OFFSET,
		               PCE_IGMP_CTRL_ROB_SHIFT,
		               PCE_IGMP_CTRL_ROB_SIZE,
		               parm->nRobust);
		if (s != GSW_statusOk)
			return s;
	} else {
		GSW_PRINT("The Robust time would only support 0..3\n");
		return GSW_statusErr;
	}
	s = gsw_reg_wr(pd, PCE_IGMP_CTRL_DMRTEN_OFFSET,
	               PCE_IGMP_CTRL_DMRTEN_SHIFT,
	               PCE_IGMP_CTRL_DMRTEN_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pd, PCE_IGMP_CTRL_DMRT_OFFSET,
	               PCE_IGMP_CTRL_DMRT_SHIFT,
	               PCE_IGMP_CTRL_DMRT_SIZE,
	               parm->nQueryInterval);
	if (s != GSW_statusOk)
		return s;

	if (parm->eIGMP_Mode ==
	        GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		switch (parm->eSuppressionAggregation) {
		case GSW_MULTICAST_REPORT_JOIN:
			d1 = 1;
			d0 = 1;
			break;
		case GSW_MULTICAST_REPORT:
			d1 = 0;
			d0 = 1;
			break;
		case GSW_MULTICAST_TRANSPARENT:
			d1 = 0;
			d0 = 0;
			break;
		}
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_REPSUP_OFFSET,
		               PCE_IGMP_CTRL_REPSUP_SHIFT,
		               PCE_IGMP_CTRL_REPSUP_SIZE, d0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_JASUP_OFFSET,
		               PCE_IGMP_CTRL_JASUP_SHIFT,
		               PCE_IGMP_CTRL_JASUP_SIZE, d1);
		if (s != GSW_statusOk)
			return s;
	}

	if (parm->eIGMP_Mode ==
	        GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_SRPEN_OFFSET,
		               PCE_IGMP_CTRL_SRPEN_SHIFT,
		               PCE_IGMP_CTRL_SRPEN_SIZE,
		               parm->bLearningRouter);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_FLEAVE_OFFSET,
		               PCE_IGMP_CTRL_FLEAVE_SHIFT,
		               PCE_IGMP_CTRL_FLEAVE_SIZE, parm->bFastLeave);
		if (s != GSW_statusOk)
			return s;
	} else {
		s = gsw_reg_wr(pd, PCE_IGMP_CTRL_FLEAVE_OFFSET,
		               PCE_IGMP_CTRL_FLEAVE_SHIFT,
		               PCE_IGMP_CTRL_FLEAVE_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	s = gsw_reg_wr(pd, PCE_GCTRL_1_UKIPMC_OFFSET,
	               PCE_GCTRL_1_UKIPMC_SHIFT,
	               PCE_GCTRL_1_UKIPMC_SIZE,
	               parm->bMulticastUnknownDrop);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastTableEntryAdd(void *pdev,
                                        GSW_multicastTable_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	gsw_igmp_t	*hitbl = &pd->iflag;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, parm->nPortId);
	if (s != GSW_statusOk)
		return s;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		u32 index = 0, i, available = 0;
		if ((pd->iflag.igv3 == 1) ||
		        (parm->eIPVersion == GSW_IP_SELECT_IPV6))
			return GSW_statusErr;
		/* Read Out all of the HW Table */
		for (i = 0; i < pd->mctblsize; i++) {
			pt.table = PCE_MULTICAST_HW_INDEX;
			pt.pcindex = i;
			pt.opmode = PCE_OPMODE_ADRD;
			s = gsw_mcast_tbl_rd(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			if (pt.valid) {
				if ((pt.key[0] ==
				        (parm->uIP_Gda.nIPv4 & 0xFFFF)) &&
				        (pt.key[1] ==
				         ((parm->uIP_Gda.nIPv4 >> 16) & 0xFFFF))) {
					index = i;
					available = 1;
					break;
				}
			}
		}
		pt.table = PCE_MULTICAST_HW_INDEX;
		if (available == 0) {
			index = pd->mctblsize;
			for (i = 0; i < pd->mctblsize; i++) {
				pt.pcindex = i;
				pt.opmode = PCE_OPMODE_ADRD;
				s = gsw_mcast_tbl_rd(pdev, &pt);
				if (s != GSW_statusOk)
					return s;
				if (pt.valid == 0) {
					index = i;  /* Free index */
					break;
				}
			}
		}
		if (index < pd->mctblsize) {
			pt.table = PCE_MULTICAST_HW_INDEX;
			pt.pcindex = index;
			pt.key[1] = ((parm->uIP_Gda.nIPv4 >> 16) & 0xFFFF);
			pt.key[0] = (parm->uIP_Gda.nIPv4 & 0xFFFF);
			pt.val[0] |= (1 << pi);
			pt.val[4] |= (1 << 14);
			pt.valid = 1;
			pt.opmode = PCE_OPMODE_ADWR;
			s = mcast_tbl_wr(pd, &pt);
			if (s != GSW_statusOk)
				return s;
		} else {
			GSW_PRINT("Error: (IGMP HW Table is full) %s:%s:%d\n",
			          __FILE__, __func__, __LINE__);
			return GSW_statusErr;
		}

	} else if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		/* Program the Multicast SW Table */
		s = gsw2x_msw_table_wr(pdev, parm);
		if (s != GSW_statusOk)
			return s;
	} else {
		/* Disable All Multicast SW Table */
		GSW_PRINT("Select IGMP mode using Snooping Config API\n");
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastTableEntryRead(void *pdev,
        GSW_multicastTableRead_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	gsw_igmp_t	*hitbl = &pd->iflag;
	SWAPI_ASSERT(pd == NULL);
	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_DISABLED) {
		GSW_PRINT("Error: (IGMP snoop is not enabled) %s:%s:%d\n",
		          __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}
	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		if (parm->bInitial == 1) {
			pd->mhw_rinx = 0; /*Start from the index 0 */
			parm->bInitial = 0;
		}
		if (pd->mhw_rinx >= pd->mctblsize) {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			pd->mhw_rinx = 0;
			return GSW_statusOk;
		}

		do {
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_MULTICAST_HW_INDEX;
			pt.pcindex = pd->mhw_rinx;
			pt.opmode = PCE_OPMODE_ADRD;
			s = gsw_mcast_tbl_rd(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			pd->mhw_rinx++;
			if (pt.valid != 0)
				break;
		} while (pd->mhw_rinx < pd->mctblsize);
		if (pt.valid != 0) {
			parm->nPortId = pt.val[0] | 0x80000000;
			parm->uIP_Gda.nIPv4 =
			    ((pt.key[1] << 16) |
			     pt.key[0]);
			parm->uIP_Gsa.nIPv4 = 0;
			parm->eModeMember = GSW_IGMP_MEMBER_DONT_CARE;
			parm->eIPVersion = GSW_IP_SELECT_IPV4;
			parm->bInitial = 0;
			parm->bLast = 0;
		} else {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			pd->mhw_rinx = 0;
		}
	}
	/*Snooping in Forward mode */
	if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		u32 dlsbid, slsbid, dmsbid, smsbid;
		if (parm->bInitial == 1) {
			pd->msw_rinx = 0; /*Start from the index 0 */
			parm->bInitial = 0;
		}
		if (pd->msw_rinx >= pd->mctblsize) {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			pd->msw_rinx = 0;
			return GSW_statusOk;
		}

		do {
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_MULTICAST_SW_INDEX;
			pt.pcindex = pd->msw_rinx;
			pt.opmode = PCE_OPMODE_ADRD;
			s = gsw_mcast_tbl_rd(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			pd->msw_rinx++;
			if (pt.valid != 0)
				break;
		} while (pd->msw_rinx < pd->mctblsize);
		if (pt.valid == 1) {
			pctbl_prog_t iptbl;
			parm->nPortId = (pt.val[0] | 0x80000000);
			dlsbid = pt.key[0] & 0xFF;
			dmsbid = (pt.key[0] >> 8) & 0xFF;
			slsbid = pt.key[1] & 0xFF;
			smsbid = (pt.key[1] >> 8) & 0xFF;
			if (dlsbid <= 0x3F) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_LSB_INDEX;
				/* Search the DIP */
				iptbl.pcindex = dlsbid;
				iptbl.opmode = PCE_OPMODE_ADRD;
				s = ip_ll_trd(pdev, &iptbl);
				if (s != GSW_statusOk)
					return s;
				if (iptbl.valid == 1) {
					if (iptbl.mask[0] == 0xFF00) {
						parm->uIP_Gda.nIPv4 =
						    ((iptbl.key[1] << 16)
						     | (iptbl.key[0]));
						parm->eIPVersion =
						    GSW_IP_SELECT_IPV4;
					} else if (iptbl.mask[0] == 0x0) {
						parm->uIP_Gda.nIPv6[4] =
						    (iptbl.key[3]);
						parm->uIP_Gda.nIPv6[5] =
						    (iptbl.key[2]);
						parm->uIP_Gda.nIPv6[6] =
						    (iptbl.key[1]);
						parm->uIP_Gda.nIPv6[7] =
						    (iptbl.key[0]);
						parm->eIPVersion =
						    GSW_IP_SELECT_IPV6;
					}
				}
			}
			if (slsbid <= 0x3F) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_LSB_INDEX;
				/* Search the SIP */
				iptbl.pcindex = slsbid;
				iptbl.opmode = PCE_OPMODE_ADRD;
				s = ip_ll_trd(pdev, &iptbl);
				if (s != GSW_statusOk)
					return s;
				if (iptbl.valid == 1) {
					if (iptbl.mask[0] == 0xFF00) {
						parm->uIP_Gsa.nIPv4 =
						    ((iptbl.key[1] << 16)
						     | (iptbl.key[0]));
						parm->eIPVersion =
						    GSW_IP_SELECT_IPV4;
					} else if (iptbl.mask == 0x0) {
						parm->uIP_Gsa.nIPv6[4] =
						    (iptbl.key[3]);
						parm->uIP_Gsa.nIPv6[5] =
						    (iptbl.key[2]);
						parm->uIP_Gsa.nIPv6[6] =
						    (iptbl.key[1]);
						parm->uIP_Gsa.nIPv6[7] =
						    (iptbl.key[0]);
					}
				}
			}
			if (dmsbid <= 0xF) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_MSB_INDEX;
				/* Search the DIP */
				iptbl.pcindex = dmsbid;
				iptbl.opmode = PCE_OPMODE_ADRD;
				s = ip_ll_trd(pdev, &iptbl);
				if (s != GSW_statusOk)
					return s;
				if (iptbl.valid == 1) {
					if (iptbl.mask[0]  == 0) {
						parm->uIP_Gda.nIPv6[0] =
						    (iptbl.key[3]);
						parm->uIP_Gda.nIPv6[1] =
						    (iptbl.key[2]);
						parm->uIP_Gda.nIPv6[2] =
						    (iptbl.key[1]);
						parm->uIP_Gda.nIPv6[3] =
						    (iptbl.key[0]);
					}
				}
			}
			if (smsbid <= 0xF) {
				memset(&iptbl, 0, sizeof(pctbl_prog_t));
				iptbl.table = PCE_IP_DASA_MSB_INDEX;
				/* Search the DIP */
				iptbl.pcindex = smsbid;
				iptbl.opmode = PCE_OPMODE_ADRD;
				s = ip_ll_trd(pdev, &iptbl);
				if (s != GSW_statusOk)
					return s;
				if (iptbl.valid == 1) {
					if (iptbl.mask[0] == 0) {
						parm->uIP_Gsa.nIPv6[0] =
						    (iptbl.key[3]);
						parm->uIP_Gsa.nIPv6[1] =
						    (iptbl.key[2]);
						parm->uIP_Gsa.nIPv6[2] =
						    (iptbl.key[1]);
						parm->uIP_Gsa.nIPv6[3] =
						    (iptbl.key[0]);
					}
				}
			}
			parm->eModeMember =
			    hitbl->mctable[pd->msw_rinx-1].mcmode;
			parm->bInitial = 0;
			parm->bLast = 0;
		} else {
			memset(parm, 0, sizeof(GSW_multicastTableRead_t));
			parm->bLast = 1;
			pd->msw_rinx = 0;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MulticastTableEntryRemove(void *pdev,
        GSW_multicastTable_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	gsw_igmp_t *hitbl = &pd->iflag;
	u8 pi = parm->nPortId;
	pctbl_prog_t pt;
	ltq_bool_t dflag = 0;
	u32 port = 0, i;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, parm->nPortId);
	if (s != GSW_statusOk)
		return s;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	if (hitbl->igmode ==
	        GSW_MULTICAST_SNOOP_MODE_AUTOLEARNING) {
		if (pd->iflag.igv3 == 1)
			return GSW_statusErr;
		/* Read Out all of the HW Table */
		for (i = 0; i < pd->mctblsize; i++) {
			memset(&pt, 0, sizeof(pctbl_prog_t));
			pt.table = PCE_MULTICAST_HW_INDEX;
			pt.pcindex = i;
			pt.opmode = PCE_OPMODE_ADRD;
			s = gsw_mcast_tbl_rd(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			/* Fill into Structure */
			if (((pt.val[0] >> pi) & 0x1) == 1) {
				if (parm->uIP_Gda.nIPv4 ==
				        ((pt.key[1] << 16)
				         | (pt.key[0]))) {
					port = (pt.val[0] & (~(1 << pi)));
					if (port == 0) {
						pt.val[0] = 0;
						pt.key[1] = 0;
						pt.val[4] = 0;
					} else {
						pt.val[0] &= ~(1 << pi);
					}
					dflag = 1;
					pt.opmode = PCE_OPMODE_ADWR;
					s = mcast_tbl_wr(pdev, &pt);
					if (s != GSW_statusOk)
						return s;
				}
			}
		}
		if (dflag == 0)
			GSW_PRINT("The input did not found\n");
	} else if (hitbl->igmode == GSW_MULTICAST_SNOOP_MODE_FORWARD) {
		s = gsw2x_msw_table_rm(pd, parm);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}
#endif /*CONFIG_LTQ_MULTICAST*/
