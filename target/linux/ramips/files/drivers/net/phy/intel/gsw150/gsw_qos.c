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

#if defined(CONFIG_LTQ_QOS) && CONFIG_LTQ_QOS
/**************************************************************************/
/*      EXTERNS Declaration:                                              */
/**************************************************************************/
extern GSW_return_t gsw_reg_rd(void *pdev, u16 os, u16 sf, u16 sz, ur *v);
extern GSW_return_t gsw_reg_wr(void *pdev, u16 os, u16 sf, u16 sz, u32 v);
extern GSW_return_t bm_ram_tbl_wr(void *pdev, bm_tbl_prog_t *pb);
extern GSW_return_t bm_ram_tbl_rd(void *pdev, bm_tbl_prog_t *pb);
GSW_return_t npport(void *pdev, u8 port);
GSW_return_t pctrl_bas_busy(void *pdev);
GSW_return_t pctrl_bas_set(void *pdev);
GSW_return_t pctrl_reg_clr(void *pdev);
GSW_return_t pctrl_addr_opmod(void *p, u16 ad, u16 op);
GSW_return_t nsqueue(void *pdev, u8 queue);

/**************************************************************************/
/*      DEFINES:                                                          */
/**************************************************************************/
#define MAX_TRAFFIC_METER 16
#define PQM_CONTEXT_TBL 0x09
#define DSCP_TBL_SIZE 64
#define REMARKING_TC_TBL_SIZE 16
#define CTAG_TBL_SIZE 8
#define TC_SIZE	16

/**************************************************************************/
/*      LOCAL DECLARATIONS:                                               */
/**************************************************************************/
static u32 mratecalc(u32 ibsid, u32 expont, u32 mants);
static int calc_mtoken(u32 mr, u32 *ibsid, u32 *expont, u32 *mants);
static GSW_return_t meter_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t meter_tbl_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t remark_qmap_tbl_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t remark_qmap_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t ctag_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t ctag_tbl_wr(void *pdev, pctbl_prog_t *pt);
/**************************************************************************/
/*      LOCAL FUNCTIONS Implementation::                                  */
/**************************************************************************/
static u32 mratecalc(u32 ibsid, u32 expont, u32 mants)
{
	static const u16 ibstbl[] = {8*8, 32*8, 64*8, 96*8};
	u16 ibs;
	u32 mr = 0;
	if ((ibsid == 0) && (expont == 0) && (mants == 0))
		return 0;
	ibs = ibstbl[ibsid];
	if (mants)
		mr = ((ibs * 25000) >> expont) / mants;
	return mr;
}

static int calc_mtoken(u32 mr, u32 *ibsid,
                       u32 *expont, u32 *mants)
{
	static const u16 ibstbl[] = {8*8, 32*8, 64*8, 96*8};
	int i;
	for (i = 3; i >= 0; i--) {
		u32 exp;
		u16 ibs = ibstbl[i];
		/* target is to get the biggest mantissa value */
		/* that can be used for the 10-Bit register */
		for (exp = 0; exp < 16; exp++) {
			u32 mant = ((ibs * 25000) >> exp) / mr;
			if (mant < (1 << 10))  {
				*ibsid = i;
				*expont = exp;
				*mants = mant;
				return 0;
			}
		}
	}
	return 1;
}

static GSW_return_t meter_tbl_wr(void *pdev,
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

static GSW_return_t meter_tbl_rd(void *pdev,
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
	               PCE_TBL_KEY_0_KEY0_OFFSET,
	               PCE_TBL_KEY_0_KEY0_SHIFT,
	               PCE_TBL_KEY_0_KEY0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->key[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_MASK_0_MASK0_OFFSET,
	               PCE_TBL_MASK_0_MASK0_SHIFT,
	               PCE_TBL_MASK_0_MASK0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->mask[0] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;

	return GSW_statusOk;
}

static GSW_return_t remark_qmap_tbl_wr(void *pdev,
                                       pctbl_prog_t *pt)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
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

static GSW_return_t remark_qmap_tbl_rd(void *pdev,
                                       pctbl_prog_t *pt)
{
	ur r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
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
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[0] = r;
	return GSW_statusOk;
}

static GSW_return_t ctag_tbl_wr(void *pdev,
                                pctbl_prog_t *pt)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
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

static GSW_return_t ctag_tbl_rd(void *pdev,
                                pctbl_prog_t *pt)
{
	u32 r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = pctrl_bas_busy(pdev);
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
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[0] = r;
	return GSW_statusOk;
}

/**************************************************************************/
/*      INTERFACE FUNCTIONS Implementation:                               */
/**************************************************************************/

GSW_return_t GSW_QoS_MeterCfgGet(void *pdev,
                                 GSW_QoS_meterCfg_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 mid = parm->nMeterId;
	u32 exp, mant, ibs;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	if (mid >= MAX_TRAFFIC_METER)
		return GSW_statusValueRange;
	/* Enable/Disable the meter shaper */
	s = gsw_reg_rd(pdev,
	               (PCE_TCM_CTRL_TCMEN_OFFSET + (mid * 7)),
	               PCE_TCM_CTRL_TCMEN_SHIFT,
	               PCE_TCM_CTRL_TCMEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bEnable = r;
	/* Committed Burst Size */
	s = gsw_reg_rd(pdev,
	               (PCE_TCM_CBS_CBS_OFFSET + (mid * 7)),
	               PCE_TCM_CBS_CBS_SHIFT,
	               PCE_TCM_CBS_CBS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nCbs = (r * 64);
	/* Excess Burst Size (EBS [bytes]) */
	s = gsw_reg_rd(pdev,
	               (PCE_TCM_EBS_EBS_OFFSET + (mid * 7)),
	               PCE_TCM_EBS_EBS_SHIFT,
	               PCE_TCM_EBS_EBS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nEbs = (r * 64);
	/* Rate Counter Exponent */
	s = gsw_reg_rd(pdev,
	               (PCE_TCM_CIR_EXP_EXP_OFFSET + (mid * 7)),
	               PCE_TCM_CIR_EXP_EXP_SHIFT,
	               PCE_TCM_CIR_EXP_EXP_SIZE, &exp);
	if (s != GSW_statusOk)
		return s;
	/* Rate Counter Mantissa */
	s = gsw_reg_rd(pdev,
	               (PCE_TCM_CIR_MANT_MANT_OFFSET + (mid * 7)),
	               PCE_TCM_CIR_MANT_MANT_SHIFT,
	               PCE_TCM_CIR_MANT_MANT_SIZE, &mant);
	if (s != GSW_statusOk)
		return s;
	/* Rate Counter iBS */
	s = gsw_reg_rd(pdev,
	               (PCE_TCM_IBS_IBS_OFFSET + (mid * 7)),
	               PCE_TCM_IBS_IBS_SHIFT,
	               PCE_TCM_IBS_IBS_SIZE, &ibs);
	if (s != GSW_statusOk)
		return s;
	/* calc the Rate */
	parm->nRate = mratecalc(ibs, exp, mant);
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_MeterCfgSet(void *pdev,
                                 GSW_QoS_meterCfg_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 mid = parm->nMeterId;
	u32 exp = 0, mant = 0, rate, ibs = 0;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	if (mid >= MAX_TRAFFIC_METER)
		return GSW_statusValueRange;

	/* Committed Burst Size */
	if (parm->nCbs > 0xFFC0)
		r = 0x3FF;
	else
		r = ((parm->nCbs + 63) / 64);
	s = gsw_reg_wr(pdev,
	               (PCE_TCM_CBS_CBS_OFFSET + (mid * 7)),
	               PCE_TCM_CBS_CBS_SHIFT,
	               PCE_TCM_CBS_CBS_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/* Excess Burst Size (EBS [bytes]) */
	if (parm->nEbs > 0xFFC0)
		r = 0x3FF;
	else
		r = ((parm->nEbs + 63) / 64);
	s = gsw_reg_wr(pdev,
	               (PCE_TCM_EBS_EBS_OFFSET + (mid * 7)),
	               PCE_TCM_EBS_EBS_SHIFT,
	               PCE_TCM_EBS_EBS_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/* Calc the Rate and convert to MANT and EXP*/
	rate = parm->nRate;
	if (rate)
		calc_mtoken(rate, &ibs, &exp, &mant);
	/* Rate Counter Exponent */
	s = gsw_reg_wr(pdev,
	               (PCE_TCM_CIR_EXP_EXP_OFFSET + (mid * 7)),
	               PCE_TCM_CIR_EXP_EXP_SHIFT,
	               PCE_TCM_CIR_EXP_EXP_SIZE, exp);
	if (s != GSW_statusOk)
		return s;
	/* Rate Counter Mantissa */
	s = gsw_reg_wr(pdev,
	               (PCE_TCM_CIR_MANT_MANT_OFFSET + (mid * 7)),
	               PCE_TCM_CIR_MANT_MANT_SHIFT,
	               PCE_TCM_CIR_MANT_MANT_SIZE, mant);
	if (s != GSW_statusOk)
		return s;
	/* Rate Counter iBS */
	s = gsw_reg_wr(pdev,
	               (PCE_TCM_IBS_IBS_OFFSET + (mid * 7)),
	               PCE_TCM_IBS_IBS_SHIFT,
	               PCE_TCM_IBS_IBS_SIZE, ibs);
	if (s != GSW_statusOk)
		return s;
	/* Enable/Disable the meter shaper */
	s = gsw_reg_wr(pdev,
	               (PCE_TCM_CTRL_TCMEN_OFFSET + (mid * 7)),
	               PCE_TCM_CTRL_TCMEN_SHIFT,
	               PCE_TCM_CTRL_TCMEN_SIZE,
	               parm->bEnable);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_MeterPortAssign(void *pdev,
                                     GSW_QoS_meterPort_t *parm)
{
	GSW_return_t s;
	ur r;
	pctbl_prog_t pt;
	u8 i, ep = parm->nPortEgressId, igp = parm->nPortIngressId;
	u8 mid = parm->nMeterId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_METERS_OFFSET,
	               ETHSW_CAP_3_METERS_SHIFT,
	               ETHSW_CAP_3_METERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r; i++) {
		pt.table = PCE_METER_INS_0_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = meter_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		if (pt.valid == 1) {
			u8 ig, eg, md;
			ig = (pt.key[0] & 0xF);
			eg = ((pt.key[0] >> 8) & 0xF);
			md = (pt.val[0] & 0x3F);
			if ((ep == eg) &&  (igp == ig) && (mid == md))
				return GSW_statusErr;
		}
	}
	for (i = 0; i < r; i++) {
		pt.table = PCE_METER_INS_0_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = meter_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		if (pt.valid == 0) {
			switch (parm->eDir) {
			case GSW_DIRECTION_BOTH:
				pt.key[0] =	(((ep & 0xF) << 8)
				             | (igp & 0xF));
				pt.mask[0] = 0;
				break;
			case GSW_DIRECTION_EGRESS:
				pt.key[0] = (((ep & 0xF) << 8) | 0xF);
				pt.mask[0] = 1;
				break;
			case GSW_DIRECTION_INGRESS:
				pt.key[0] = (0xF00 | (igp & 0xF));
				pt.mask[0] = 4;
				break;
			default:
				return GSW_statusErr;
			}
			pt.table = PCE_METER_INS_0_INDEX;
			pt.pcindex = i;
			pt.val[0] = (mid & 0x3F);
			pt.valid = 1;
			pt.opmode = PCE_OPMODE_ADWR;
			s = meter_tbl_wr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			return GSW_statusOk;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_MeterPortDeassign(void *pdev,
                                       GSW_QoS_meterPort_t *parm)
{
	GSW_return_t s;
	ur r;
	pctbl_prog_t pt;
	u8 i, ep = parm->nPortEgressId, igp = parm->nPortIngressId;
	u8 mid = parm->nMeterId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_METERS_OFFSET,
	               ETHSW_CAP_3_METERS_SHIFT,
	               ETHSW_CAP_3_METERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r; i++) {
		pt.table = PCE_METER_INS_0_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = meter_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		if (pt.valid == 1) {
			u8 ig, eg, md;
			ig = (pt.key[0] & 0xF);
			eg = ((pt.key[0] >> 8) & 0xF);
			md = (pt.val[0] & 0x3F);
			if ((ep == eg) &&  (igp == ig) && (mid == md)) {
				pt.table = PCE_METER_INS_0_INDEX;
				pt.key[0] = 0;
				pt.val[0] = 0;
				pt.valid = 0;
				pt.opmode = PCE_OPMODE_ADWR;
				s = meter_tbl_wr(pdev, &pt);
				if (s != GSW_statusOk)
					return s;
			}
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_MeterPortGet(void *pdev,
                                  GSW_QoS_meterPortGet_t *parm)
{
	ur r;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_METERS_OFFSET,
	               ETHSW_CAP_3_METERS_SHIFT,
	               ETHSW_CAP_3_METERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (parm->bInitial == 1) {
		pd->meter_cnt = 0;
		parm->bInitial = 0;
	} else {
		if (pd->meter_cnt >= (r * 1))
			parm->bLast = 1;
	}
	pt.table = PCE_METER_INS_0_INDEX;
	pt.pcindex = (pd->meter_cnt & 0xF);
	pt.opmode = PCE_OPMODE_ADRD;
	s = meter_tbl_rd(pd, &pt);
	if (s != GSW_statusOk)
		return s;
	if (pt.valid) {
		parm->nMeterId = (pt.val[0] & 0x3F);
		parm->nPortEgressId = ((pt.key[0] >> 8) & 0xF);
		parm->nPortIngressId = (pt.key[0] & 0xF);
		if ((pt.mask[0] & 0x5) == 0)
			parm->eDir = GSW_DIRECTION_BOTH;
		else if ((pt.mask[0] & 0x5) == 1)
			parm->eDir = GSW_DIRECTION_EGRESS;
		else if ((pt.mask[0] & 0x5) == 4)
			parm->eDir = GSW_DIRECTION_INGRESS;
		else
			parm->eDir = GSW_DIRECTION_NONE;
	}
	pd->meter_cnt++;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_DSCP_ClassGet(void *pdev,
                                   GSW_QoS_DSCP_ClassCfg_t *parm)
{
	u8 i;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	for (i = 0; i <= 63; i++) {
		pt.table = PCE_DSCP_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = ctag_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nTrafficClass[i] = (pt.val[0] & 0xF);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_DSCP_ClassSet(void *pdev,
                                   GSW_QoS_DSCP_ClassCfg_t *parm)
{
	u8 dp = parm->nDSCP;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (dp >= DSCP_TBL_SIZE)
		return GSW_statusValueRange;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_DSCP_INDEX;
	pt.pcindex = dp;
	pt.opmode = PCE_OPMODE_ADRD;
	s = ctag_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	pt.val[0] &= ~(0xFF);
	pt.val[0] |= (parm->nTrafficClass[dp] & 0xFF);
	pt.table = PCE_DSCP_INDEX;
	pt.pcindex = dp;
	pt.opmode = PCE_OPMODE_ADWR;
	s = ctag_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ClassDSCP_Get(void *pdev,
                                   GSW_QoS_ClassDSCP_Cfg_t *parm)
{
	u8 i;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	for (i = 0; i < REMARKING_TC_TBL_SIZE; i++) {
		pt.table = PCE_REMARKING_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = remark_qmap_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nDSCP[i] = pt.val[0] & 0x3F;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ClassDSCP_Set(void *pdev,
                                   GSW_QoS_ClassDSCP_Cfg_t *parm)
{
	GSW_return_t s;
	u8 dscp, pcp, tc = parm->nTrafficClass;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (tc >= REMARKING_TC_TBL_SIZE)
		return GSW_statusValueRange;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_REMARKING_INDEX;
	pt.pcindex = tc;
	pt.opmode = PCE_OPMODE_ADRD;
	s = remark_qmap_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	pcp = (pt.val[0] >> 8) & 0x7;
	dscp	= (parm->nDSCP[tc] & 0x3F);
	pt.val[0] = ((pcp << 8) | dscp);
	pt.table = PCE_REMARKING_INDEX;
	pt.pcindex = tc;
	pt.opmode = PCE_OPMODE_ADWR;
	s = remark_qmap_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_DSCP_DropPrecedenceCfgGet(void *pdev,
        GSW_QoS_DSCP_DropPrecedenceCfg_t *parm)
{
	u8 i;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	for (i = 0; i < DSCP_TBL_SIZE; i++) {
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_DSCP_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = ctag_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nDSCP_DropPrecedence[i]	= ((pt.val[0] >> 4) & 0x3);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_DSCP_DropPrecedenceCfgSet(void *pdev,
        GSW_QoS_DSCP_DropPrecedenceCfg_t *parm)
{
	GSW_return_t s;
	u8 dp = parm->nDSCP;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (dp >= DSCP_TBL_SIZE)
		return GSW_statusValueRange;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_DSCP_INDEX;
	pt.pcindex = dp;
	pt.opmode = PCE_OPMODE_ADRD;
	s = ctag_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	pt.val[0] &= ~(0x3 << 4);
	pt.val[0] |= ((parm->nDSCP_DropPrecedence[dp] & 0x3) << 4);
	pt.table = PCE_DSCP_INDEX;
	pt.pcindex = dp;
	pt.opmode = PCE_OPMODE_ADWR;
	s = ctag_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_PortRemarkingCfgGet(void *pdev,
        GSW_QoS_portRemarkingCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r, r1, r2, r3;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_CLPEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_0_CLPEN_SHIFT,
	               PCE_PCTRL_0_CLPEN_SIZE, &r1);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_DPEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_0_DPEN_SHIFT,
	               PCE_PCTRL_0_DPEN_SIZE, &r2);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_DSCPMOD_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_DSCPMOD_SHIFT,
	               PCE_PCTRL_2_DSCPMOD_SIZE, &r3);
	if (s != GSW_statusOk)
		return s;

	if ((r1 == 0) && (r2 == 0) && (r3 == 0))
		parm->eDSCP_IngressRemarkingEnable = GSW_DSCP_REMARK_DISABLE;
	else if ((r1 == 1) && (r2 == 0) && (r3 == 1))
		parm->eDSCP_IngressRemarkingEnable = GSW_DSCP_REMARK_TC6;
	else if ((r1 == 1) && (r2 == 1) && (r3 == 1))
		parm->eDSCP_IngressRemarkingEnable = GSW_DSCP_REMARK_TC3;
	else if ((r1 == 0) && (r2 == 1) && (r3 == 1))
		parm->eDSCP_IngressRemarkingEnable = GSW_DSCP_REMARK_DP3;
	else if ((r1 == 1) && (r2 == 1) && (r3 == 1))
		parm->eDSCP_IngressRemarkingEnable = GSW_DSCP_REMARK_DP3_TC3;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_PCPEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_0_PCPEN_SHIFT,
	               PCE_PCTRL_0_PCPEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bPCP_IngressRemarkingEnable = r;
	s = gsw_reg_rd(pdev,
	               (FDMA_PCTRL_DSCPRM_OFFSET + (6 * pi)),
	               FDMA_PCTRL_DSCPRM_SHIFT,
	               FDMA_PCTRL_DSCPRM_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bDSCP_EgressRemarkingEnable = r;
	s = gsw_reg_rd(pdev,
	               (FDMA_PCTRL_VLANMOD_OFFSET + (6 * pi)),
	               FDMA_PCTRL_VLANMOD_SHIFT,
	               FDMA_PCTRL_VLANMOD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 3)
		parm->bPCP_EgressRemarkingEnable = 1;
	else
		parm->bPCP_EgressRemarkingEnable = 0;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_SPCPEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_SPCPEN_SHIFT,
	               PCE_PCTRL_2_SPCPEN_SIZE,
	               &parm->bSTAG_PCP_IngressRemarkingEnable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_SDEIEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_SDEIEN_SHIFT,
	               PCE_PCTRL_2_SDEIEN_SIZE,
	               &parm->bSTAG_DEI_IngressRemarkingEnable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (FDMA_PCTRL_SVLANMOD_OFFSET + (6 * pi)),
	               FDMA_PCTRL_SVLANMOD_SHIFT,
	               FDMA_PCTRL_SVLANMOD_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	if (r == 3)
		parm->bSTAG_PCP_DEI_EgressRemarkingEnable = 1;
	else
		parm->bSTAG_PCP_DEI_EgressRemarkingEnable = 0;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_PortRemarkingCfgSet(void *pdev,
        GSW_QoS_portRemarkingCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r1, r2, r3;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pd, pi);
	if (s != GSW_statusOk)
		return s;
	switch (parm->eDSCP_IngressRemarkingEnable) {
	case GSW_DSCP_REMARK_DISABLE:
		r1 = 0;
		r2 = 0;
		r3 = 0;
		break;
	case GSW_DSCP_REMARK_TC6:
		r1 = 1;
		r2 = 0;
		r3 = 1;
		break;
	case GSW_DSCP_REMARK_TC3:
		r1 = 1;
		r2 = 0;
		r3 = 0;
		break;
	case GSW_DSCP_REMARK_DP3:
		r1 = 0;
		r2 = 1;
		r3 = 0;
		break;
	case GSW_DSCP_REMARK_DP3_TC3:
		r1 = 1;
		r2 = 1;
		r3 = 0;
		break;
	default:
		r1 = 0;
		r2 = 0;
		r3 = 0;
	}
	s = gsw_reg_wr(pdev,
	               PCE_PCTRL_0_CLPEN_OFFSET + (10 * pi),
	               PCE_PCTRL_0_CLPEN_SHIFT,
	               PCE_PCTRL_0_CLPEN_SIZE, r1);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_PCTRL_0_DPEN_OFFSET + (10 * pi),
	               PCE_PCTRL_0_DPEN_SHIFT,
	               PCE_PCTRL_0_DPEN_SIZE, r2);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_PCTRL_2_DSCPMOD_OFFSET + (10 * pi),
	               PCE_PCTRL_2_DSCPMOD_SHIFT,
	               PCE_PCTRL_2_DSCPMOD_SIZE, r3);
	if (s != GSW_statusOk)
		return s;
	if (parm->bDSCP_EgressRemarkingEnable > 0)
		r1 = parm->bDSCP_EgressRemarkingEnable;
	else
		r1 = 0;
	s = gsw_reg_wr(pdev,
	               (FDMA_PCTRL_DSCPRM_OFFSET + (6 * pi)),
	               FDMA_PCTRL_DSCPRM_SHIFT,
	               FDMA_PCTRL_DSCPRM_SIZE, r1);
	if (s != GSW_statusOk)
		return s;
	if (parm->bPCP_IngressRemarkingEnable > 0)
		r1 = parm->bPCP_IngressRemarkingEnable;
	else
		r1 = 0;
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_0_PCPEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_0_PCPEN_SHIFT,
	               PCE_PCTRL_0_PCPEN_SIZE, r1);
	if (s != GSW_statusOk)
		return s;
	if (parm->bPCP_EgressRemarkingEnable > 0)
		r1 = 3;
	else
		r1 = 0;
	s = gsw_reg_wr(pdev,
	               (FDMA_PCTRL_VLANMOD_OFFSET + (6 * pi)),
	               FDMA_PCTRL_VLANMOD_SHIFT,
	               FDMA_PCTRL_VLANMOD_SIZE, r1);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_2_SPCPEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_SPCPEN_SHIFT,
	               PCE_PCTRL_2_SPCPEN_SIZE,
	               parm->bSTAG_PCP_IngressRemarkingEnable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_2_SDEIEN_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_SDEIEN_SHIFT,
	               PCE_PCTRL_2_SDEIEN_SIZE,
	               parm->bSTAG_DEI_IngressRemarkingEnable);
	if (s != GSW_statusOk)
		return s;
	if (parm->bSTAG_PCP_DEI_EgressRemarkingEnable > 0)
		r1 = 3;
	else
		r1 = 0;
	/*	s = gsw_reg_wr(pdev,
			(FDMA_PCTRL_DEIMOD_OFFSET + (6 * pi)), */
	/*	FDMA_PCTRL_DEIMOD_SHIFT,
			FDMA_PCTRL_DEIMOD_SIZE,*/
	/*	parm->bSTAG_PCP_DEI_EgressRemarkingEnable);
		if (s != GSW_statusOk)
			return s;	*/
	s = gsw_reg_wr(pdev,
	               (FDMA_PCTRL_SVLANMOD_OFFSET + (6 * pi)),
	               FDMA_PCTRL_SVLANMOD_SHIFT,
	               FDMA_PCTRL_SVLANMOD_SIZE, r1);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ClassPCP_Get(void *pdev,
                                  GSW_QoS_ClassPCP_Cfg_t *parm)
{
	u8 i;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	for (i = 0; i < REMARKING_TC_TBL_SIZE; i++) {
		pt.table = PCE_REMARKING_INDEX;
		pt.pcindex = i /*parm->nTrafficClass*/;
		pt.opmode = PCE_OPMODE_ADRD;
		s = remark_qmap_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nPCP[i] = (pt.val[0] >> 8) & 0x7;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ClassPCP_Set(void *pdev,
                                  GSW_QoS_ClassPCP_Cfg_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	u8 tc = parm->nTrafficClass, dscp, pcp;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (tc >= REMARKING_TC_TBL_SIZE)
		return GSW_statusValueRange;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_REMARKING_INDEX;
	pt.pcindex = tc;
	pt.opmode = PCE_OPMODE_ADRD;
	s = remark_qmap_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	dscp = pt.val[0] & 0x3F;
	pcp = parm->nPCP[tc] & 0x7;
	pt.val[0] = (pcp << 8) | dscp;
	pt.table = PCE_REMARKING_INDEX;
	pt.pcindex = tc;
	pt.opmode = PCE_OPMODE_ADWR;
	s = remark_qmap_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_PCP_ClassGet(void *pdev,
                                  GSW_QoS_PCP_ClassCfg_t *parm)
{
	int i;
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	for (i = 0; i < CTAG_TBL_SIZE; i++) {
		pt.table = PCE_PCP_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = ctag_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nTrafficClass[i] = (pt.val[0] & 0x1F);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_PCP_ClassSet(void *pdev,
                                  GSW_QoS_PCP_ClassCfg_t *parm)
{
	u8 pv = parm->nPCP;
	pctbl_prog_t pt;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (pv >= CTAG_TBL_SIZE)
		return GSW_statusValueRange;
	pt.table = PCE_PCP_INDEX;
	pt.pcindex = pv;
	pt.val[0] = (parm->nTrafficClass[pv] & 0x1F);
	pt.opmode = PCE_OPMODE_ADWR;
	s = ctag_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_PortCfgGet(void *pdev,
                                GSW_QoS_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r, ds, cp, sp;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_DSCP_SHIFT,
	               PCE_PCTRL_2_DSCP_SIZE, &ds);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_PCP_SHIFT,
	               PCE_PCTRL_2_PCP_SIZE, &cp);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_SPCP_SHIFT,
	               PCE_PCTRL_2_SPCP_SIZE, &sp);
	if (s != GSW_statusOk)
		return s;
	if ((ds == 0) && (cp == 0) && (sp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_NO;
	else if ((ds == 2) && (cp == 0) && (sp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP;
	else if ((ds == 0) && (cp == 1) && (sp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_PCP;
	else if ((ds == 2) && (cp == 1) && (sp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP_PCP;
	else if ((ds == 1) && (cp == 1) && (sp == 0))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_PCP_DSCP;
	else if ((ds == 0) && (cp == 0) && (sp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP;
	else if ((ds == 1) && (cp == 0) && (sp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP_DSCP;
	else if ((ds == 2) && (cp == 0) && (sp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP_SPCP;
	else if ((ds == 0) && (cp == 1) && (sp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP_PCP;
	else if ((ds == 1) && (cp == 1) && (sp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_SPCP_PCP_DSCP;
	else if ((ds == 2) && (cp == 1) && (sp == 1))
		parm->eClassMode = GSW_QOS_CLASS_SELECT_DSCP_SPCP_PCP;
	else
		parm->eClassMode = GSW_QOS_CLASS_SELECT_NO;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_2_PCLASS_OFFSET + (10 * pi)),
	               PCE_PCTRL_2_PCLASS_SHIFT,
	               PCE_PCTRL_2_PCLASS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nTrafficClass = r;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_PortCfgSet(void *pdev,
                                GSW_QoS_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	if (parm->nTrafficClass > TC_SIZE)
		return GSW_statusNoSupport;
	s = gsw_reg_wr(pdev,
	               PCE_PCTRL_2_PCLASS_OFFSET + (10 * pi),
	               PCE_PCTRL_2_PCLASS_SHIFT,
	               PCE_PCTRL_2_PCLASS_SIZE,
	               parm->nTrafficClass);
	if (s != GSW_statusOk)
		return s;
	switch (parm->eClassMode) {
	case GSW_QOS_CLASS_SELECT_NO:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_DSCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 2);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_PCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_DSCP_PCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 2);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_PCP_DSCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_SPCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_SPCP_DSCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_DSCP_SPCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 2);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_SPCP_PCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_SPCP_PCP_DSCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_QOS_CLASS_SELECT_DSCP_SPCP_PCP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_DSCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_DSCP_SHIFT,
		               PCE_PCTRL_2_DSCP_SIZE, 2);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_PCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_PCP_SHIFT,
		               PCE_PCTRL_2_PCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_2_SPCP_OFFSET + (10 * pi)),
		               PCE_PCTRL_2_SPCP_SHIFT,
		               PCE_PCTRL_2_SPCP_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_QueuePortGet(void *pdev,
                                  GSW_QoS_queuePort_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_QUEUE_MAP_INDEX;
	pt.pcindex = ((((parm->nPortId << 4) & 0xF0)
	               | (parm->nTrafficClassId & 0xF)));
	pt.opmode = PCE_OPMODE_ADRD;
	s = remark_qmap_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	parm->nQueueId = (pt.val[0] & 0x3F);
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_QueuePortSet(void *pdev,
                                  GSW_QoS_queuePort_t *parm)
{
	pctbl_prog_t pt;
	u16 ep;
	GSW_return_t s;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_QUEUE_MAP_INDEX;
	pt.pcindex = (((parm->nPortId << 4) & 0xF0)
	              | (parm->nTrafficClassId & 0xF));
	pt.val[0] = (parm->nQueueId & 0x1F);
	pt.opmode = PCE_OPMODE_ADWR;
	s = remark_qmap_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	/* Assign the Egress Port Id and Enable the Queue */
	ep = (parm->nPortId & 0x7);
	ep |= (((parm->nPortId >> 3) & 0x1) << 4);

	memset(&pb, 0, sizeof(bm_tbl_prog_t));
	pb.bmindex = parm->nQueueId;
	pb.bmtable = 0x0E;
	pb.bmval[0] = ep;
	pb.bmopmode = BM_OPMODE_WR;
	s = bm_ram_tbl_wr(pdev, &pb);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_SchedulerCfgGet(void *pdev,
                                     GSW_QoS_schedulerCfg_t *parm)
{
	GSW_return_t s;
	u8 qid = parm->nQueueId;
	u16  v;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	memset(&pb, 0, sizeof(bm_tbl_prog_t));
	pb.bmindex = qid;
	pb.bmtable = 0x08;
	pb.bmopmode = BM_OPMODE_RD;
	s = bm_ram_tbl_rd(pdev, &pb);
	if (s != GSW_statusOk)
		return s;
	v = pb.bmval[0];
	parm->nWeight = v;

	if (v == 0xFFFF || v == 0x1800)
		parm->eType = GSW_QOS_SCHEDULER_STRICT;
	else
		parm->eType = GSW_QOS_SCHEDULER_WFQ;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_SchedulerCfgSet(void *pdev,
                                     GSW_QoS_schedulerCfg_t *parm)
{
	GSW_return_t s;
	u8 qid = parm->nQueueId;
	ur r;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	if ((parm->eType == GSW_QOS_SCHEDULER_WFQ) &&
	        (parm->nWeight == 0))
		return GSW_statusErr;
	if (parm->eType == GSW_QOS_SCHEDULER_STRICT)
		r = 0xFFFF;
	else
		r = parm->nWeight;
	memset(&pb, 0, sizeof(bm_tbl_prog_t));
	pb.bmindex = qid;
	pb.bmtable = 0x08;
	pb.bmval[0] = r;
	pb.bmopmode = BM_OPMODE_WR;
	s = bm_ram_tbl_wr(pdev, &pb);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ShaperCfgGet(void *pdev,
                                  GSW_QoS_ShaperCfg_t *parm)
{
	GSW_return_t s;
	u8 rs = parm->nRateShaperId;
	ur r;
	u32 exp, mant, ibs;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_SHAPERS_OFFSET,
	               ETHSW_CAP_3_SHAPERS_SHIFT,
	               ETHSW_CAP_3_SHAPERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (rs >= r)
		return GSW_statusValueRange;
	/* Enable/Disable the rate shaper  */
	s = gsw_reg_rd(pdev,
	               RS_CTRL_RSEN_OFFSET + (rs * 0x5),
	               RS_CTRL_RSEN_SHIFT,
	               RS_CTRL_RSEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bEnable = r;
	/* Committed Burst Size (CBS [bytes]) */
	s = gsw_reg_rd(pdev,
	               RS_CBS_CBS_OFFSET + (rs * 0x5),
	               RS_CBS_CBS_SHIFT,
	               RS_CBS_CBS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nCbs = (r * 64);
	/** Rate [Mbit/s] */
	s = gsw_reg_rd(pdev,
	               RS_CIR_EXP_EXP_OFFSET + (rs * 0x5),
	               RS_CIR_EXP_EXP_SHIFT,
	               RS_CIR_EXP_EXP_SIZE, &exp);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               RS_CIR_MANT_MANT_OFFSET + (rs * 0x5),
	               RS_CIR_MANT_MANT_SHIFT,
	               RS_CIR_MANT_MANT_SIZE, &mant);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               RS_IBS_IBS_OFFSET + (rs * 0x5),
	               RS_IBS_IBS_SHIFT,
	               RS_IBS_IBS_SIZE, &ibs);
	if (s != GSW_statusOk)
		return s;
	/* calc the Rate */
	parm->nRate = mratecalc(ibs, exp, mant);
	s = gsw_reg_rd(pdev,
	               RS_CTRL_RSMOD_OFFSET + (rs * 0x5),
	               RS_CTRL_RSMOD_SHIFT,
	               RS_CTRL_RSMOD_SIZE,
	               &parm->bAVB);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ShaperCfgSet(void *pdev,
                                  GSW_QoS_ShaperCfg_t *parm)
{
	GSW_return_t s;
	u8 rs = parm->nRateShaperId;
	ur r;
	u32 exp = 0, mant = 0, rate = 0, ibs = 0;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_SHAPERS_OFFSET,
	               ETHSW_CAP_3_SHAPERS_SHIFT,
	               ETHSW_CAP_3_SHAPERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (rs >= r)
		return GSW_statusErr;
	/* Committed Burst Size */
	if (parm->nCbs > 0xFFC0)
		r = 0x3FF;
	else
		r = ((parm->nCbs + 63) / 64);
	/* Committed Burst Size (CBS [bytes]) */
	s = gsw_reg_wr(pdev,
	               RS_CBS_CBS_OFFSET + (rs * 0x5),
	               RS_CBS_CBS_SHIFT,
	               RS_CBS_CBS_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/* Rate [kbit/s] */
	/* Calc the Rate */
	rate = parm->nRate;
	if (rate)
		calc_mtoken(rate, &ibs, &exp, &mant);
	s = gsw_reg_wr(pdev,
	               RS_CIR_EXP_EXP_OFFSET + (rs * 0x5),
	               RS_CIR_EXP_EXP_SHIFT,
	               RS_CIR_EXP_EXP_SIZE, exp);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               RS_CIR_MANT_MANT_OFFSET + (rs * 0x5),
	               RS_CIR_MANT_MANT_SHIFT,
	               RS_CIR_MANT_MANT_SIZE, mant);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               RS_IBS_IBS_OFFSET + (rs * 0x5),
	               RS_IBS_IBS_SHIFT,
	               RS_IBS_IBS_SIZE, ibs);
	if (s != GSW_statusOk)
		return s;
	/* Enable/Disable the rate shaper  */
	s = gsw_reg_wr(pdev,
	               RS_CTRL_RSEN_OFFSET + (rs * 0x5),
	               RS_CTRL_RSEN_SHIFT,
	               RS_CTRL_RSEN_SIZE,
	               parm->bEnable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               RS_CTRL_RSMOD_OFFSET + (rs * 0x5),
	               RS_CTRL_RSMOD_SHIFT,
	               RS_CTRL_RSMOD_SIZE,
	               parm->bAVB);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ShaperQueueAssign(void *pdev,
                                       GSW_QoS_ShaperQueue_t *parm)
{
	GSW_return_t s;
	u8 rs = parm->nRateShaperId, qid = parm->nQueueId;
	ur r, r1, r2, r3;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_SHAPERS_OFFSET,
	               ETHSW_CAP_3_SHAPERS_SHIFT,
	               ETHSW_CAP_3_SHAPERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (rs >= r)
		return GSW_statusNoSupport;
	/* Check Rate Shaper 1 Enable  */
	s = gsw_reg_rd(pdev,
	               PQM_RS_EN1_OFFSET + (qid * 2),
	               PQM_RS_EN1_SHIFT,
	               PQM_RS_EN1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PQM_RS_RS1_OFFSET + (rs * 2),
	               PQM_RS_RS1_SHIFT,
	               PQM_RS_RS1_SIZE, &r1);
	if (s != GSW_statusOk)
		return s;
	/* Check Rate Shaper 2 Enable  */
	s = gsw_reg_rd(pdev,
	               PQM_RS_EN2_OFFSET + (qid * 2),
	               PQM_RS_EN2_SHIFT,
	               PQM_RS_EN2_SIZE, &r2);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PQM_RS_RS2_OFFSET + (rs * 2),
	               PQM_RS_RS2_SHIFT,
	               PQM_RS_RS2_SIZE, &r3);
	if (s != GSW_statusOk)
		return s;
	if ((r == 1) && (r1 == rs))
		return GSW_statusOk;
	else if ((r2 == 1) &&
	         (r3 == rs))
		return GSW_statusOk;
	else if (r == 0) {
		s = gsw_reg_wr(pdev,
		               PQM_RS_RS1_OFFSET + (qid * 2),
		               PQM_RS_RS1_SHIFT,
		               PQM_RS_RS1_SIZE, rs);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PQM_RS_EN1_OFFSET + (qid * 2),
		               PQM_RS_EN1_SHIFT,
		               PQM_RS_EN1_SIZE, 0x1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RS_CTRL_RSEN_OFFSET + (rs * 0x5),
		               RS_CTRL_RSEN_SHIFT,
		               RS_CTRL_RSEN_SIZE, 0x1);
		if (s != GSW_statusOk)
			return s;
	} else if (r2 == 0) {
		s = gsw_reg_wr(pdev,
		               PQM_RS_RS2_OFFSET + (qid * 2),
		               PQM_RS_RS2_SHIFT,
		               PQM_RS_RS2_SIZE, rs);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PQM_RS_EN2_OFFSET + (qid * 2),
		               PQM_RS_EN2_SHIFT,
		               PQM_RS_EN2_SIZE, 0x1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               RS_CTRL_RSEN_OFFSET + (rs * 0x5),
		               RS_CTRL_RSEN_SHIFT,
		               RS_CTRL_RSEN_SIZE, 0x1);
		if (s != GSW_statusOk)
			return s;
	} else {
		return GSW_statusErr;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ShaperQueueDeassign(void *pdev,
        GSW_QoS_ShaperQueue_t *parm)
{
	GSW_return_t s;
	u8 rs = parm->nRateShaperId, qid = parm->nQueueId;
	ur r, r1, r2, r3;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_SHAPERS_OFFSET,
	               ETHSW_CAP_3_SHAPERS_SHIFT,
	               ETHSW_CAP_3_SHAPERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (rs >= r)
		return GSW_statusErr;
	/* Rate Shaper 1 Read  */
	s = gsw_reg_rd(pdev,
	               PQM_RS_EN1_OFFSET + (qid * 2),
	               PQM_RS_EN1_SHIFT,
	               PQM_RS_EN1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PQM_RS_RS1_OFFSET + (qid * 2),
	               PQM_RS_RS1_SHIFT,
	               PQM_RS_RS1_SIZE, &r1);
	if (s != GSW_statusOk)
		return s;
	/* Rate Shaper 2 Read  */
	s = gsw_reg_rd(pdev,
	               PQM_RS_EN2_OFFSET + (qid * 2),
	               PQM_RS_EN2_SHIFT,
	               PQM_RS_EN2_SIZE, &r2);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PQM_RS_RS2_OFFSET + (qid * 2),
	               PQM_RS_RS2_SHIFT,
	               PQM_RS_RS2_SIZE, &r3);
	if (s != GSW_statusOk)
		return s;
	if ((r == 1) && (r1 == rs)) {
		s = gsw_reg_wr(pdev,
		               PQM_RS_EN1_OFFSET + (qid * 2),
		               PQM_RS_EN1_SHIFT,
		               PQM_RS_EN1_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PQM_RS_RS1_OFFSET + (qid * 2),
		               PQM_RS_RS1_SHIFT,
		               PQM_RS_RS1_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		return GSW_statusOk;
	} else if ((r2 == 1) && (r3 == rs)) {
		s = gsw_reg_wr(pdev,
		               PQM_RS_EN2_OFFSET + (qid * 2),
		               PQM_RS_EN2_SHIFT,
		               PQM_RS_EN2_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PQM_RS_RS2_OFFSET + (qid * 2),
		               PQM_RS_RS2_SHIFT,
		               PQM_RS_RS2_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		return GSW_statusOk;
	} else {
		return GSW_statusErr;
	}
	if ((r == 0) && (r2 == 0)) {
		s = gsw_reg_wr(pdev,
		               RS_CTRL_RSEN_OFFSET + (rs * 0x5),
		               RS_CTRL_RSEN_SHIFT,
		               RS_CTRL_RSEN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_ShaperQueueGet(void *pdev,
                                    GSW_QoS_ShaperQueueGet_t *parm)
{
	GSW_return_t s;
	ur r, r1;
	u8 qid = parm->nQueueId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	parm->bAssigned = 0;
	parm->nRateShaperId = 0;
	s = gsw_reg_rd(pdev,
	               PQM_RS_EN1_OFFSET + (qid * 2),
	               PQM_RS_EN1_SHIFT,
	               PQM_RS_EN1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1) {
		parm->bAssigned = 1;
		s = gsw_reg_rd(pdev,
		               PQM_RS_RS1_OFFSET + (qid * 2),
		               PQM_RS_RS1_SHIFT,
		               PQM_RS_RS1_SIZE, &r1);
		if (s != GSW_statusOk)
			return s;
		parm->nRateShaperId = r1;
	}
	s = gsw_reg_rd(pdev,
	               PQM_RS_EN2_OFFSET + (qid * 2),
	               PQM_RS_EN2_SHIFT,
	               PQM_RS_EN2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1) {
		s = gsw_reg_rd(pdev,
		               PQM_RS_RS2_OFFSET + (qid * 2),
		               PQM_RS_RS2_SHIFT,
		               PQM_RS_RS2_SIZE, &r1);
		if (s != GSW_statusOk)
			return s;
		parm->nRateShaperId = r1;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_StormCfgSet(void *pdev,
                                 GSW_QoS_stormCfg_t *parm)
{
	GSW_return_t s;
	ur bs, ms, uc, mid;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if ((parm->bBroadcast == 0) &&
	        (parm->bMulticast == 0)	&&
	        (parm->bUnknownUnicast == 0)) {
		/*  Storm Control Mode  */
		s = gsw_reg_wr(pdev,
		               PCE_GCTRL_0_SCONMOD_OFFSET,
		               PCE_GCTRL_0_SCONMOD_SHIFT,
		               PCE_GCTRL_0_SCONMOD_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		/* Meter instances used for broadcast traffic */
		s = gsw_reg_wr(pdev,
		               PCE_GCTRL_0_SCONBC_OFFSET,
		               PCE_GCTRL_0_SCONBC_SHIFT,
		               PCE_GCTRL_0_SCONBC_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		/* Meter instances used for multicast traffic */
		s = gsw_reg_wr(pdev,
		               PCE_GCTRL_0_SCONMC_OFFSET,
		               PCE_GCTRL_0_SCONMC_SHIFT,
		               PCE_GCTRL_0_SCONMC_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		/* Meter instances used for unknown unicast traffic */
		s = gsw_reg_wr(pdev,
		               PCE_GCTRL_0_SCONUC_OFFSET,
		               PCE_GCTRL_0_SCONUC_SHIFT,
		               PCE_GCTRL_0_SCONUC_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	/*  Meter ID */
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_SCONMET_OFFSET,
	               PCE_GCTRL_0_SCONMET_SHIFT,
	               PCE_GCTRL_0_SCONMET_SIZE, &mid);
	if (s != GSW_statusOk)
		return s;
	/* Meter instances used for broadcast traffic */
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_SCONBC_OFFSET,
	               PCE_GCTRL_0_SCONBC_SHIFT,
	               PCE_GCTRL_0_SCONBC_SIZE, &bs);
	if (s != GSW_statusOk)
		return s;
	/* Meter instances used for multicast traffic */
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_SCONMC_OFFSET,
	               PCE_GCTRL_0_SCONMC_SHIFT,
	               PCE_GCTRL_0_SCONMC_SIZE, &ms);
	if (s != GSW_statusOk)
		return s;
	/* Meter instances used for unknown unicast traffic */
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_SCONUC_OFFSET,
	               PCE_GCTRL_0_SCONUC_SHIFT,
	               PCE_GCTRL_0_SCONUC_SIZE, &uc);
	if (s != GSW_statusOk)
		return s;

	if ((bs == 1) || (ms == 1) || (uc == 1)) {
		if (parm->nMeterId == (mid + 1)) {
			/*  Storm Control Mode  */
			s = gsw_reg_wr(pdev,
			               PCE_GCTRL_0_SCONMOD_OFFSET,
			               PCE_GCTRL_0_SCONMOD_SHIFT,
			               PCE_GCTRL_0_SCONMOD_SIZE, 3);
			if (s != GSW_statusOk)
				return s;
		} else if (parm->nMeterId != mid)
			return GSW_statusParam;
	} else {
		/*  Storm Control Mode */
		s = gsw_reg_wr(pdev,
		               PCE_GCTRL_0_SCONMOD_OFFSET,
		               PCE_GCTRL_0_SCONMOD_SHIFT,
		               PCE_GCTRL_0_SCONMOD_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
	}
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_SCONMET_OFFSET,
	               PCE_GCTRL_0_SCONMET_SHIFT,
	               PCE_GCTRL_0_SCONMET_SIZE,
	               parm->nMeterId);
	if (s != GSW_statusOk)
		return s;
	/* Meter instances used for broadcast traffic */
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_SCONBC_OFFSET,
	               PCE_GCTRL_0_SCONBC_SHIFT,
	               PCE_GCTRL_0_SCONBC_SIZE,
	               parm->bBroadcast);
	if (s != GSW_statusOk)
		return s;
	/* Meter instances used for multicast traffic */
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_SCONMC_OFFSET,
	               PCE_GCTRL_0_SCONMC_SHIFT,
	               PCE_GCTRL_0_SCONMC_SIZE,
	               parm->bMulticast);
	if (s != GSW_statusOk)
		return s;
	/* Meter instances used for unknown unicast traffic */
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_SCONUC_OFFSET,
	               PCE_GCTRL_0_SCONUC_SHIFT,
	               PCE_GCTRL_0_SCONUC_SIZE,
	               parm->bUnknownUnicast);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_StormCfgGet(void *pdev,
                                 GSW_QoS_stormCfg_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_SCONMOD_OFFSET,
	               PCE_GCTRL_0_SCONMOD_SHIFT,
	               PCE_GCTRL_0_SCONMOD_SIZE,
	               &r);
	/*  Storm Control Mode  */
	if (s != GSW_statusOk)
		return s;
	if (r == 0) {
		parm->nMeterId = 0;
		parm->bBroadcast = 0;
		parm->bMulticast = 0;
		parm->bUnknownUnicast =  0;
	} else {
		s = gsw_reg_rd(pdev,
		               PCE_GCTRL_0_SCONMET_OFFSET,
		               PCE_GCTRL_0_SCONMET_SHIFT,
		               PCE_GCTRL_0_SCONMET_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nMeterId = r;
		/* Meter instances used for broadcast traffic */
		s = gsw_reg_rd(pdev,
		               PCE_GCTRL_0_SCONBC_OFFSET,
		               PCE_GCTRL_0_SCONBC_SHIFT,
		               PCE_GCTRL_0_SCONBC_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->bBroadcast = r;
		/* Meter instances used for multicast traffic */
		s = gsw_reg_rd(pdev,
		               PCE_GCTRL_0_SCONMC_OFFSET,
		               PCE_GCTRL_0_SCONMC_SHIFT,
		               PCE_GCTRL_0_SCONMC_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->bMulticast = r;
		/* Meter instances used for unknown unicast traffic */
		s = gsw_reg_rd(pdev,
		               PCE_GCTRL_0_SCONUC_OFFSET,
		               PCE_GCTRL_0_SCONUC_SHIFT,
		               PCE_GCTRL_0_SCONUC_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->bUnknownUnicast = r;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_WredCfgGet(void *pdev,
                                GSW_QoS_WRED_Cfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	/* Description: 'Drop Probability Profile' */
	s = gsw_reg_rd(pdev,
	               BM_QUEUE_GCTRL_DPROB_OFFSET,
	               BM_QUEUE_GCTRL_DPROB_SHIFT,
	               BM_QUEUE_GCTRL_DPROB_SIZE,
	               &parm->eProfile);
	if (s != GSW_statusOk)
		return s;
	/* Get the Local/Global threshold */
	s = gsw_reg_rd(pdev,
	               BM_QUEUE_GCTRL_GL_MOD_OFFSET,
	               BM_QUEUE_GCTRL_GL_MOD_SHIFT,
	               BM_QUEUE_GCTRL_GL_MOD_SIZE,
	               &parm->eThreshMode);
	if (s != GSW_statusOk)
		return s;
	/* WRED Red Threshold - Minimum */
	s = gsw_reg_rd(pdev,
	               BM_WRED_RTH_0_MINTH_OFFSET,
	               BM_WRED_RTH_0_MINTH_SHIFT,
	               BM_WRED_RTH_0_MINTH_SIZE,
	               &parm->nRed_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Red Threshold - Maximum */
	s = gsw_reg_rd(pdev,
	               BM_WRED_RTH_1_MAXTH_OFFSET,
	               BM_WRED_RTH_1_MAXTH_SHIFT,
	               BM_WRED_RTH_1_MAXTH_SIZE,
	               &parm->nRed_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Minimum */
	s = gsw_reg_rd(pdev,
	               BM_WRED_YTH_0_MINTH_OFFSET,
	               BM_WRED_YTH_0_MINTH_SHIFT,
	               BM_WRED_YTH_0_MINTH_SIZE,
	               &parm->nYellow_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Maximum */
	s = gsw_reg_rd(pdev,
	               BM_WRED_YTH_1_MAXTH_OFFSET,
	               BM_WRED_YTH_1_MAXTH_SHIFT,
	               BM_WRED_YTH_1_MAXTH_SIZE,
	               &parm->nYellow_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Minimum */
	s = gsw_reg_rd(pdev,
	               BM_WRED_GTH_0_MINTH_OFFSET,
	               BM_WRED_GTH_0_MINTH_SHIFT,
	               BM_WRED_GTH_0_MINTH_SIZE,
	               &parm->nGreen_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Maximum */
	s = gsw_reg_rd(pdev,
	               BM_WRED_GTH_1_MAXTH_OFFSET,
	               BM_WRED_GTH_1_MAXTH_SHIFT,
	               BM_WRED_GTH_1_MAXTH_SIZE,
	               &parm->nGreen_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_WredCfgSet(void *pdev,
                                GSW_QoS_WRED_Cfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/* Description: 'Drop Probability Profile' */
	s = gsw_reg_wr(pdev,
	               BM_QUEUE_GCTRL_DPROB_OFFSET,
	               BM_QUEUE_GCTRL_DPROB_SHIFT,
	               BM_QUEUE_GCTRL_DPROB_SIZE,
	               parm->eProfile);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_QUEUE_GCTRL_GL_MOD_OFFSET,
	               BM_QUEUE_GCTRL_GL_MOD_SHIFT,
	               BM_QUEUE_GCTRL_GL_MOD_SIZE,
	               parm->eThreshMode);
	if (s != GSW_statusOk)
		return s;
	/* WRED Red Threshold - Minimum */
	s = gsw_reg_wr(pdev,
	               BM_WRED_RTH_0_MINTH_OFFSET,
	               BM_WRED_RTH_0_MINTH_SHIFT,
	               BM_WRED_RTH_0_MINTH_SIZE,
	               parm->nRed_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Red Threshold - Maximum */
	s = gsw_reg_wr(pdev,
	               BM_WRED_RTH_1_MAXTH_OFFSET,
	               BM_WRED_RTH_1_MAXTH_SHIFT,
	               BM_WRED_RTH_1_MAXTH_SIZE,
	               parm->nRed_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Minimum */
	s = gsw_reg_wr(pdev,
	               BM_WRED_YTH_0_MINTH_OFFSET,
	               BM_WRED_YTH_0_MINTH_SHIFT,
	               BM_WRED_YTH_0_MINTH_SIZE,
	               parm->nYellow_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Maximum */
	s = gsw_reg_wr(pdev,
	               BM_WRED_YTH_1_MAXTH_OFFSET,
	               BM_WRED_YTH_1_MAXTH_SHIFT,
	               BM_WRED_YTH_1_MAXTH_SIZE,
	               parm->nYellow_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Minimum */
	s = gsw_reg_wr(pdev,
	               BM_WRED_GTH_0_MINTH_OFFSET,
	               BM_WRED_GTH_0_MINTH_SHIFT,
	               BM_WRED_GTH_0_MINTH_SIZE,
	               parm->nGreen_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Maximum */
	s = gsw_reg_wr(pdev,
	               BM_WRED_GTH_1_MAXTH_OFFSET,
	               BM_WRED_GTH_1_MAXTH_SHIFT,
	               BM_WRED_GTH_1_MAXTH_SIZE,
	               parm->nGreen_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_WredQueueCfgGet(void *pdev,
                                     GSW_QoS_WRED_QueueCfg_t *parm)
{
	GSW_return_t s;
	bm_tbl_prog_t pb;
	u8 qid = parm->nQueueId, i;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	/* For different color 0(not drop) 1(Green) 2(Yellow) 3(Red) */
	for (i = 1; i < 4; i++) {
		memset(&pb, 0, sizeof(bm_tbl_prog_t));
		pb.bmindex = ((qid << 3) | i);
		pb.bmtable = PQM_CONTEXT_TBL;
		pb.bmopmode = BM_OPMODE_RD;
		s = bm_ram_tbl_rd(pdev, &pb);
		if (s != GSW_statusOk)
			return s;
		switch (i) {
		case 3:
			parm->nRed_Max = pb.bmval[1];
			parm->nRed_Min = pb.bmval[0];
			break;
		case 2:
			parm->nYellow_Max = pb.bmval[1];
			parm->nYellow_Min = pb.bmval[0];
			break;
		case 1:
			parm->nGreen_Max = pb.bmval[1];
			parm->nGreen_Min = pb.bmval[0];
			break;
		case 0:
			break;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_WredQueueCfgSet(void *pdev,
                                     GSW_QoS_WRED_QueueCfg_t *parm)
{
	GSW_return_t s;
	u8 qid = parm->nQueueId, i;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	/* For different color 0(not drop) 1(Green) 2(Yellow) 3(Red) */
	for (i = 1; i < 4; i++) {
		memset(&pb, 0, sizeof(bm_tbl_prog_t));
		switch (i) {
		case 3:
			pb.bmval[1] = parm->nRed_Max;
			pb.bmval[0] = parm->nRed_Min;
			break;
		case 2:
			pb.bmval[1] = parm->nYellow_Max;
			pb.bmval[0] = parm->nYellow_Min;
			break;
		case 1:
			pb.bmval[1] = parm->nGreen_Max;
			pb.bmval[0] = parm->nGreen_Min;
			break;
		case 0:
			pb.bmval[0] = 0;
			pb.bmval[1] = 0;
			break;
		}
		pb.bmindex = (((qid << 3) & 0xF8) | i);
		pb.bmtable = PQM_CONTEXT_TBL;
		pb.bmopmode = BM_OPMODE_WR;
		s = bm_ram_tbl_wr(pdev, &pb);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_WredPortCfgGet(void *pdev,
                                    GSW_QoS_WRED_PortCfg_t *parm)
{
	GSW_return_t s;
	u8 pn = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pn);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (BM_PWRED_RTH_0_MINTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_RTH_0_MINTH_SHIFT,
	               BM_PWRED_RTH_0_MINTH_SIZE,
	               &parm->nRed_Min);
	if (s != GSW_statusOk)
		return s;
	/** WRED Red Threshold Max [number of segments].*/
	s = gsw_reg_rd(pdev,
	               (BM_PWRED_RTH_1_MAXTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_RTH_1_MAXTH_SHIFT,
	               BM_PWRED_RTH_1_MAXTH_SIZE,
	               &parm->nRed_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Minimum */
	s = gsw_reg_rd(pdev,
	               (BM_PWRED_YTH_0_MINTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_YTH_0_MINTH_SHIFT,
	               BM_PWRED_YTH_0_MINTH_SIZE,
	               &parm->nYellow_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Maximum */
	s = gsw_reg_rd(pdev,
	               (BM_PWRED_YTH_1_MAXTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_YTH_1_MAXTH_SHIFT,
	               BM_PWRED_YTH_1_MAXTH_SIZE,
	               &parm->nYellow_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Minimum */
	s = gsw_reg_rd(pdev,
	               (BM_PWRED_GTH_0_MINTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_GTH_0_MINTH_SHIFT,
	               BM_PWRED_GTH_0_MINTH_SIZE,
	               &parm->nGreen_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Maximum */
	s = gsw_reg_rd(pdev,
	               (BM_PWRED_GTH_1_MAXTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_GTH_1_MAXTH_SHIFT,
	               BM_PWRED_GTH_1_MAXTH_SIZE,
	               &parm->nGreen_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_WredPortCfgSet(void *pdev,
                                    GSW_QoS_WRED_PortCfg_t *parm)
{
	GSW_return_t s;
	u8 pn = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pn);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (BM_PWRED_RTH_0_MINTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_RTH_0_MINTH_SHIFT,
	               BM_PWRED_RTH_0_MINTH_SIZE,
	               parm->nRed_Min);
	if (s != GSW_statusOk)
		return s;
	/** WRED Red Threshold Max [number of segments]. */
	s = gsw_reg_wr(pdev,
	               (BM_PWRED_RTH_1_MAXTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_RTH_1_MAXTH_SHIFT,
	               BM_PWRED_RTH_1_MAXTH_SIZE,
	               parm->nRed_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Minimum */
	s = gsw_reg_wr(pdev,
	               (BM_PWRED_YTH_0_MINTH_OFFSET
	                + (pn * 0x6)),
	               BM_PWRED_YTH_0_MINTH_SHIFT,
	               BM_PWRED_YTH_0_MINTH_SIZE,
	               parm->nYellow_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Yellow Threshold - Maximum */
	s = gsw_reg_wr(pdev,
	               (BM_PWRED_YTH_1_MAXTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_YTH_1_MAXTH_SHIFT,
	               BM_PWRED_YTH_1_MAXTH_SIZE,
	               parm->nYellow_Max);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Minimum */
	s = gsw_reg_wr(pdev,
	               (BM_PWRED_GTH_0_MINTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_GTH_0_MINTH_SHIFT,
	               BM_PWRED_GTH_0_MINTH_SIZE,
	               parm->nGreen_Min);
	if (s != GSW_statusOk)
		return s;
	/* WRED Green Threshold - Maximum */
	s = gsw_reg_wr(pdev,
	               (BM_PWRED_GTH_1_MAXTH_OFFSET + (pn * 0x6)),
	               BM_PWRED_GTH_1_MAXTH_SHIFT,
	               BM_PWRED_GTH_1_MAXTH_SIZE,
	               parm->nGreen_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_FlowctrlCfgGet(void *pdev,
                                    GSW_QoS_FlowCtrlCfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               (SDMA_FCTHR1_THR1_OFFSET),
	               SDMA_FCTHR1_THR1_SHIFT,
	               SDMA_FCTHR1_THR1_SIZE,
	               &parm->nFlowCtrlNonConform_Min);
	if (s != GSW_statusOk)
		return s;
	/** Global Buffer Non Conforming Flow Control */
	/* Threshold Maximum [number of segments]. */
	s = gsw_reg_rd(pdev,
	               (SDMA_FCTHR2_THR2_OFFSET),
	               SDMA_FCTHR2_THR2_SHIFT,
	               SDMA_FCTHR2_THR2_SIZE,
	               &parm->nFlowCtrlNonConform_Max);
	if (s != GSW_statusOk)
		return s;
	/** Global Buffer Conforming Flow Control Threshold */
	/*  Minimum [number of segments]. */
	s = gsw_reg_rd(pdev,
	               (SDMA_FCTHR3_THR3_OFFSET),
	               SDMA_FCTHR3_THR3_SHIFT,
	               SDMA_FCTHR3_THR3_SIZE,
	               &parm->nFlowCtrlConform_Min);
	if (s != GSW_statusOk)
		return s;
	/** Global Buffer Conforming Flow Control */
	/* Threshold Maximum [number of segments]. */
	s = gsw_reg_rd(pdev,
	               (SDMA_FCTHR4_THR4_OFFSET),
	               SDMA_FCTHR4_THR4_SHIFT,
	               SDMA_FCTHR4_THR4_SIZE,
	               &parm->nFlowCtrlConform_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_FlowctrlCfgSet(void *pdev,
                                    GSW_QoS_FlowCtrlCfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               (SDMA_FCTHR1_THR1_OFFSET),
	               SDMA_FCTHR1_THR1_SHIFT,
	               SDMA_FCTHR1_THR1_SIZE,
	               parm->nFlowCtrlNonConform_Min);
	if (s != GSW_statusOk)
		return s;
	/** Global Buffer Non Conforming Flow Control */
	/* Threshold Maximum [number of segments]. */
	s = gsw_reg_wr(pdev,
	               (SDMA_FCTHR2_THR2_OFFSET),
	               SDMA_FCTHR2_THR2_SHIFT,
	               SDMA_FCTHR2_THR2_SIZE,
	               parm->nFlowCtrlNonConform_Max);
	if (s != GSW_statusOk)
		return s;
	/** Global Buffer Conforming Flow Control Threshold */
	/*  Minimum [number of segments]. */
	s = gsw_reg_wr(pdev,
	               (SDMA_FCTHR3_THR3_OFFSET),
	               SDMA_FCTHR3_THR3_SHIFT,
	               SDMA_FCTHR3_THR3_SIZE,
	               parm->nFlowCtrlConform_Min);
	if (s != GSW_statusOk)
		return s;
	/** Global Buffer Conforming Flow Control Threshold */
	/*  Maximum [number of segments]. */
	s = gsw_reg_wr(pdev,
	               (SDMA_FCTHR4_THR4_OFFSET),
	               SDMA_FCTHR4_THR4_SHIFT,
	               SDMA_FCTHR4_THR4_SIZE,
	               parm->nFlowCtrlConform_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_FlowctrlPortCfgGet(void *pdev,
                                        GSW_QoS_FlowCtrlPortCfg_t *parm)
{
	GSW_return_t s;
	u8 pn = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pn);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (SDMA_PFCTHR8_THR8_OFFSET22 + (pn * 0x4)),
	               SDMA_PFCTHR8_THR8_SHIFT,
	               SDMA_PFCTHR8_THR8_SIZE,
	               &parm->nFlowCtrl_Min);
	if (s != GSW_statusOk)
		return s;
	/** Ingress Port occupied Buffer Flow Control */
	/* Threshold Maximum [number of segments]. */
	s = gsw_reg_rd(pdev,
	               (SDMA_PFCTHR9_THR9_OFFSET22 + (pn * 0x4)),
	               SDMA_PFCTHR9_THR9_SHIFT,
	               SDMA_PFCTHR9_THR9_SIZE,
	               &parm->nFlowCtrl_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_FlowctrlPortCfgSet(void *pdev,
                                        GSW_QoS_FlowCtrlPortCfg_t *parm)
{
	GSW_return_t s;
	u8 pn = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pn);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (SDMA_PFCTHR8_THR8_OFFSET22 + (pn * 0x4)),
	               SDMA_PFCTHR8_THR8_SHIFT,
	               SDMA_PFCTHR8_THR8_SIZE,
	               parm->nFlowCtrl_Min);
	if (s != GSW_statusOk)
		return s;
	/** Ingress Port occupied Buffer Flow Control Threshold */
	/* Maximum [number of segments]. */
	s = gsw_reg_wr(pdev,
	               (SDMA_PFCTHR9_THR9_OFFSET22 + (pn * 0x4)),
	               SDMA_PFCTHR9_THR9_SHIFT,
	               SDMA_PFCTHR9_THR9_SIZE,
	               parm->nFlowCtrl_Max);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_QueueBufferReserveCfgGet(void *pdev,
        GSW_QoS_QueueBufferReserveCfg_t *parm)
{
	GSW_return_t s;
	u16 ind;
	u8 qid = parm->nQueueId;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	memset(&pb, 0, sizeof(bm_tbl_prog_t));
	/* Colourcode = 0  and fixed offset = 0 */
	/* Set the BM RAM ADDR  */
	ind = (((qid << 3) & 0xF8));
	pb.bmindex = ind;
	pb.bmtable = PQM_CONTEXT_TBL;
	pb.bmopmode = BM_OPMODE_RD;
	s = bm_ram_tbl_rd(pdev, &pb);
	if (s != GSW_statusOk)
		return s;
	parm->nBufferReserved = pb.bmval[0];
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_QueueBufferReserveCfgSet(void *pdev,
        GSW_QoS_QueueBufferReserveCfg_t *parm)
{
	u16 ind;
	GSW_return_t s;
	u8 qid = parm->nQueueId;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = nsqueue(pdev, qid);
	if (s != GSW_statusOk)
		return s;
	memset(&pb, 0, sizeof(bm_tbl_prog_t));
	/* Colourcode = 0  and fixed offset = 0 */
	ind = (((qid << 3) & 0xF8));
	pb.bmindex = ind;
	pb.bmtable = PQM_CONTEXT_TBL;
	pb.bmval[0] = parm->nBufferReserved;
	pb.bmopmode = BM_OPMODE_WR;
	s = bm_ram_tbl_wr(pdev, &pb);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
#endif  /* CONFIG_LTQ_QOS */
