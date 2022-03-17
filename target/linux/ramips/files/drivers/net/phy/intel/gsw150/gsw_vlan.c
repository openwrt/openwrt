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
#if defined(CONFIG_LTQ_VLAN) && CONFIG_LTQ_VLAN
/**************************************************************************/
/*      EXTERNS Declaration:                                              */
/**************************************************************************/
extern GSW_return_t gsw_reg_rd(void *pdev, u16 os, u16 sf, u16 sz, ur *v);
extern GSW_return_t gsw_reg_wr(void *pdev, u16 os, u16 sf, u16 sz, u32 v);
extern GSW_return_t npport(void *pdev, u8 port);
extern GSW_return_t pctrl_bas_busy(void *pdev);
extern GSW_return_t pctrl_bas_set(void *pdev);
extern GSW_return_t pctrl_reg_clr(void *pdev);
extern GSW_return_t pctrl_addr_opmod(void *p, u16 ad, u16 op);
/**************************************************************************/
/*      DEFINES:                                                          */
/**************************************************************************/
#define VLAN_MAP_TBL_SIZE 4096
#define EG_VLAN_TBL_SIZE 16
#define SVLAN_PCP_TBL_SIZE 16

/**************************************************************************/
/*      LOCAL DECLARATIONS:                                               */
/**************************************************************************/
static GSW_return_t vlan_map_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t vlan_map_tbl_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t eg_port_tbl_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t eg_port_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t stag_tbl_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t stag_tbl_wr(void *pdev, pctbl_prog_t *pt);

/**************************************************************************/
/*      LOCAL FUNCTIONS Implementation::                                  */
/**************************************************************************/
static GSW_return_t vlan_map_tbl_rd(void *pdev,
                                    pctbl_prog_t *pt)
{
	GSW_return_t s;
	ur r;
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
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_1_VAL1_OFFSET,
	               PCE_TBL_VAL_1_VAL1_SHIFT,
	               PCE_TBL_VAL_1_VAL1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[1] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_2_VAL2_OFFSET,
	               PCE_TBL_VAL_2_VAL2_SHIFT,
	               PCE_TBL_VAL_2_VAL2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[2] = r;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	return GSW_statusOk;
}

static GSW_return_t vlan_map_tbl_wr(void *pdev,
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
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               pt->val[0]);
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
	               PCE_TBL_VAL_2_VAL2_OFFSET,
	               PCE_TBL_VAL_2_VAL2_SHIFT,
	               PCE_TBL_VAL_2_VAL2_SIZE,
	               pt->val[2]);
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

static GSW_return_t eg_port_tbl_wr(void *pdev,
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
	               PCE_TBL_VAL_0_VAL0_OFFSET,
	               PCE_TBL_VAL_0_VAL0_SHIFT,
	               PCE_TBL_VAL_0_VAL0_SIZE,
	               pt->val[0]);
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

static GSW_return_t eg_port_tbl_rd(void *pdev,
                                   pctbl_prog_t *pt)
{
	GSW_return_t s;
	ur r;
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
	s = gsw_reg_rd(pdev,
	               PCE_TBL_VAL_1_VAL1_OFFSET,
	               PCE_TBL_VAL_1_VAL1_SHIFT,
	               PCE_TBL_VAL_1_VAL1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->val[1] = r;

	return GSW_statusOk;
}

static GSW_return_t stag_tbl_wr(void *pdev,
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

static GSW_return_t stag_tbl_rd(void *pdev,
                                pctbl_prog_t *pt)
{
	GSW_return_t s;
	ur r;
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
GSW_return_t GSW_VLAN_Member_Init(void *pdev,
                                  GSW_VLAN_memberInit_t *parm)
{
	GSW_return_t s;
	u16 i;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	for (i = 0; i < VLAN_MAP_TBL_SIZE; i++) {
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.pcindex = i;
		pt.table = PCE_VLANMAP_INDEX;
		pt.opmode = PCE_OPMODE_ADRD;
		s = vlan_map_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		pt.pcindex = i;
		pt.table = PCE_VLANMAP_INDEX;
		pt.opmode = PCE_OPMODE_ADWR;
		pt.val[1] = (parm->nPortMemberMap & 0xFFFF);
		pt.val[2] = (parm->nEgressTagMap & 0xFFFF);
		s = vlan_map_tbl_wr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_IdCreate(void *pdev,
                               GSW_VLAN_IdCreate_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.pcindex = parm->nVId;
	pt.table = PCE_VLANMAP_INDEX;
	pt.val[0] = (parm->nFId & 0xFF);
	pt.opmode = PCE_OPMODE_ADWR;
	s = vlan_map_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_IdDelete(void *pdev,
                               GSW_VLAN_IdDelete_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.pcindex = parm->nVId;
	pt.table = PCE_VLANMAP_INDEX;
	pt.opmode = PCE_OPMODE_ADWR;
	s = vlan_map_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_IdGet(void *pdev,
                            GSW_VLAN_IdGet_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.pcindex = parm->nVId;
	pt.table = PCE_VLANMAP_INDEX;
	pt.opmode = PCE_OPMODE_ADRD;
	s = vlan_map_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	parm->nFId = (pt.val[0] & 0xFF);
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_PortCfgGet(void *pdev,
                                 GSW_VLAN_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_DEFPVID_PVID_OFFSET + (10 * pi)),
	               PCE_DEFPVID_PVID_SHIFT,
	               PCE_DEFPVID_PVID_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nPortVId = r;
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_UVR_OFFSET + (10 * pi)),
	               PCE_VCTRL_UVR_SHIFT,
	               PCE_VCTRL_UVR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bVLAN_UnknownDrop = r;
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_VSR_OFFSET + (10 * pi)),
	               PCE_VCTRL_VSR_SHIFT,
	               PCE_VCTRL_VSR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bVLAN_ReAssign = r;
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_VIMR_OFFSET + (10 * pi)),
	               PCE_VCTRL_VIMR_SHIFT,
	               PCE_VCTRL_VIMR_VEMR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->eVLAN_MemberViolation = r;
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_VINR_OFFSET + (10 * pi)),
	               PCE_VCTRL_VINR_SHIFT,
	               PCE_VCTRL_VINR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	switch (r) {
	case 0:
		parm->eAdmitMode = GSW_VLAN_ADMIT_ALL;
		break;
	case 1:
		parm->eAdmitMode = GSW_VLAN_ADMIT_TAGGED;
		break;
	case 2:
		parm->eAdmitMode = GSW_VLAN_ADMIT_UNTAGGED;
		break;
	default:
		parm->eAdmitMode = r;
		break;
	}
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_TVM_OFFSET + (10 * pi)),
	               PCE_PCTRL_0_TVM_SHIFT,
	               PCE_PCTRL_0_TVM_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bTVM = r;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_PortCfgSet(void *pdev,
                                 GSW_VLAN_portCfg_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	r = parm->nPortVId;
	s = gsw_reg_wr(pdev,
	               (PCE_DEFPVID_PVID_OFFSET + (10 * pi)),
	               PCE_DEFPVID_PVID_SHIFT,
	               PCE_DEFPVID_PVID_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = (parm->bVLAN_UnknownDrop & 0x1);
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_UVR_OFFSET + (10 * pi)),
	               PCE_VCTRL_UVR_SHIFT,
	               PCE_VCTRL_UVR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = parm->bVLAN_ReAssign;
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_VSR_OFFSET + (10 * pi)),
	               PCE_VCTRL_VSR_SHIFT,
	               PCE_VCTRL_VSR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = (parm->eVLAN_MemberViolation & 0x3);
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_VIMR_OFFSET + (10 * pi)),
	               PCE_VCTRL_VIMR_SHIFT,
	               PCE_VCTRL_VIMR_VEMR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	switch (parm->eAdmitMode) {
	case GSW_VLAN_ADMIT_ALL:
		r = 0;
		break;
	case GSW_VLAN_ADMIT_TAGGED:
		r = 1;
		break;
	case GSW_VLAN_ADMIT_UNTAGGED:
		r = 2;
		break;
	default:
		r = 0;
	}
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_VINR_OFFSET + (10 * pi)),
	               PCE_VCTRL_VINR_SHIFT,
	               PCE_VCTRL_VINR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = (parm->bTVM);
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_0_TVM_OFFSET + (10 * pi)),
	               PCE_PCTRL_0_TVM_SHIFT,
	               PCE_PCTRL_0_TVM_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_PortMemberAdd(void *pdev,
                                    GSW_VLAN_portMemberAdd_t *parm)
{
	GSW_return_t s;
	u32 pi = parm->nPortId;
	pctbl_prog_t pt;
	u16  pm, tm, val0;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, (pi & 0xFF));
	if (s != GSW_statusOk)
		return s;

	if (parm->nVId >= VLAN_MAP_TBL_SIZE)
		return GSW_statusValueRange;

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADRD;
	s = vlan_map_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	pm	= (pt.val[1]);
	tm = (pt.val[2]);
	val0 = (pt.val[0]);
	/*  Support  portmap information. */
	/*  To differentiate between port index and portmap, */
	/* the MSB (highest data bit) should be 1.*/
	if (pi & 0x80000000) { /*Port Map */
		pm |= (pi & 0xFFFF);
		if (parm->bVLAN_TagEgress)
			tm |= (pi & 0xFFFF);
		else
			tm &= ~(pi & 0xFFFF);
	} else {
		pm |= 1 << pi;
		if (parm->bVLAN_TagEgress)
			tm |= 1 << pi;
		else
			tm &= ~(1 << pi);
	}
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADWR;
	pt.val[0] = val0;
	pt.val[1] = pm;
	pt.val[2] = tm;
	s = vlan_map_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_PortMemberRead(void *pdev,
                                     GSW_VLAN_portMemberRead_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADRD;
	s = vlan_map_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	parm->nPortId = pt.val[1];
	parm->nTagId = pt.val[2];

	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_PortMemberRemove(void *pdev,
                                       GSW_VLAN_portMemberRemove_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	u16  pm, tm, val0;
	u32 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, (pi & 0xFF));
	if (s != GSW_statusOk)
		return s;

	if (parm->nVId >= VLAN_MAP_TBL_SIZE)
		return GSW_statusValueRange;

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADRD;
	s = vlan_map_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	pm	= pt.val[1];
	tm = pt.val[2];
	val0 = pt.val[0];
	if (pi & 0x80000000)
		pm  &= ~((pi) & 0x7FFF);
	else
		pm &= ~(1 << pi);
	tm &= ~(1 << pi);
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADWR;
	pt.val[0] = val0;
	pt.val[1] = pm;
	pt.val[2] = tm;
	s = vlan_map_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_ReservedAdd(void *pdev,
                                  GSW_VLAN_reserved_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	if (parm->nVId >= VLAN_MAP_TBL_SIZE)
		return GSW_statusValueRange;

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADRD;
	s = vlan_map_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;

	pt.val[0] |= (1 << 8);
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADWR;
	s = vlan_map_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_VLAN_ReservedRemove(void *pdev,
                                     GSW_VLAN_reserved_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	if (parm->nVId >= VLAN_MAP_TBL_SIZE)
		return GSW_statusValueRange;

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADRD;
	s = vlan_map_tbl_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	pt.val[0] &= ~(1 << 8);
	pt.table = PCE_VLANMAP_INDEX;
	pt.pcindex = parm->nVId;
	pt.opmode = PCE_OPMODE_ADWR;
	s = vlan_map_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_SVLAN_CfgGet(void *pdev,
                              GSW_SVLAN_cfg_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev, FDMA_SVTETYPE_OFFSET,
	               FDMA_SVTETYPE_ETYPE_SHIFT,
	               FDMA_SVTETYPE_ETYPE_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nEthertype = r;
	return GSW_statusOk;
}

GSW_return_t GSW_SVLAN_CfgSet(void *pdev,
                              GSW_SVLAN_cfg_t *parm)
{
	GSW_return_t s;
	ur r = (parm->nEthertype & 0xFFFF);
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               FDMA_SVTETYPE_OFFSET,
	               FDMA_SVTETYPE_ETYPE_SHIFT,
	               FDMA_SVTETYPE_ETYPE_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_SVLAN_PortCfgGet(void *pdev,
                                  GSW_SVLAN_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_DEFPSVID_PVID_OFFSET + (2 * pi)),
	               PCE_DEFPSVID_PVID_SHIFT,
	               PCE_DEFPSVID_PVID_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nPortVId = r;
	/* bSVLAN_TagSupport */
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_STEN_OFFSET + (10 * pi)),
	               PCE_VCTRL_STEN_SHIFT,
	               PCE_VCTRL_STEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bSVLAN_TagSupport = r;
	/** bVLAN_ReAssign */
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_SVSR_OFFSET + (10 * pi)),
	               PCE_VCTRL_SVSR_SHIFT,
	               PCE_VCTRL_SVSR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bVLAN_ReAssign = r;
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_SVIMR_OFFSET + (10 * pi)),
	               PCE_VCTRL_SVIMR_SHIFT,
	               PCE_VCTRL_SVIMR_SVEMR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->eVLAN_MemberViolation = r;
	/* eAdmitMode:  */
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_SVINR_OFFSET + (10 * pi)),
	               PCE_VCTRL_SVINR_SHIFT,
	               PCE_VCTRL_SVINR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	switch (r) {
	case 0:
		parm->eAdmitMode = GSW_VLAN_ADMIT_ALL;
		break;
	case 1:
		parm->eAdmitMode = GSW_VLAN_ADMIT_TAGGED;
		break;
	case 2:
		parm->eAdmitMode = GSW_VLAN_ADMIT_UNTAGGED;
		break;
	default:
		parm->eAdmitMode = (r & 0x3);
		break;
	}
	/** bSVLAN_MACbasedTag */
	s = gsw_reg_rd(pdev,
	               (PCE_VCTRL_MACEN_OFFSET + (10 * pi)),
	               PCE_VCTRL_MACEN_SHIFT,
	               PCE_VCTRL_MACEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bSVLAN_MACbasedTag = r;
	return GSW_statusOk;
}

GSW_return_t GSW_SVLAN_PortCfgSet(void *pdev,
                                  GSW_SVLAN_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;

	r = parm->nPortVId;
	s = gsw_reg_wr(pdev,
	               (PCE_DEFPSVID_PVID_OFFSET + (2 * pi)),
	               PCE_DEFPSVID_PVID_SHIFT,
	               PCE_DEFPSVID_PVID_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/* bSVLAN_TagSupport */
	r = parm->bSVLAN_TagSupport;
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_STEN_OFFSET + (10 * pi)),
	               PCE_VCTRL_STEN_SHIFT,
	               PCE_VCTRL_STEN_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	if (parm->bSVLAN_TagSupport == 1) {
		/* Update to default values */
		s = gsw_reg_wr(pdev,
		               (FDMA_PCTRL_SVLANMOD_OFFSET + (6 * pi)),
		               FDMA_PCTRL_SVLANMOD_SHIFT,
		               FDMA_PCTRL_SVLANMOD_SIZE, 3);
		if (s != GSW_statusOk)
			return s;
	} else {
		s = gsw_reg_wr(pdev,
		               (FDMA_PCTRL_SVLANMOD_OFFSET + (6 * pi)),
		               FDMA_PCTRL_SVLANMOD_SHIFT,
		               FDMA_PCTRL_SVLANMOD_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	/** bVLAN_ReAssign */
	r = parm->bVLAN_ReAssign;
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_SVSR_OFFSET + (10 * pi)),
	               PCE_VCTRL_SVSR_SHIFT,
	               PCE_VCTRL_SVSR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = (parm->eVLAN_MemberViolation & 0x3);
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_SVIMR_OFFSET + (10 * pi)),
	               PCE_VCTRL_SVIMR_SHIFT,
	               PCE_VCTRL_SVIMR_SVEMR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	switch (parm->eAdmitMode) {
	case GSW_VLAN_ADMIT_ALL:
		r = 0;
		break;
	case GSW_VLAN_ADMIT_TAGGED:
		r = 1;
		break;
	case GSW_VLAN_ADMIT_UNTAGGED:
		r = 2;
		break;
	default:
		r = 0;
	}
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_SVINR_OFFSET + (10 * pi)),
	               PCE_VCTRL_SVINR_SHIFT,
	               PCE_VCTRL_SVINR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/** bSVLAN_MACbasedTag */
	r = parm->bSVLAN_MACbasedTag;
	s = gsw_reg_wr(pdev,
	               (PCE_VCTRL_MACEN_OFFSET + (10 * pi)),
	               PCE_VCTRL_MACEN_SHIFT,
	               PCE_VCTRL_MACEN_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_SVLAN_ClassPCP_PortGet(void *pdev,
        GSW_QoS_SVLAN_ClassPCP_PortCfg_t *parm)
{
	GSW_return_t s;
	u8 i, pi = parm->nPortId;
	pctbl_prog_t pt;
	u16 dei;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;

	for (i = 0; i < EG_VLAN_TBL_SIZE; i++) {
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.pcindex = (((parm->nPortId & 0xF) << 4) | (i));
		pt.table = PCE_EGREMARK_INDEX;
		pt.opmode = PCE_OPMODE_ADRD;
		s = eg_port_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nDSCP[i] = (pt.val[0] & 0x3F);
		parm->nCPCP[i] = ((pt.val[0] >> 8) & 0x7);
		parm->nSPCP[i] = ((pt.val[1] >> 8) & 0x7);
		dei = ((pt.val[1]) & 0x1);
		parm->nSPCP[i]	|= (dei << 7);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_SVLAN_ClassPCP_PortSet(void *pdev,
        GSW_QoS_SVLAN_ClassPCP_PortCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	pctbl_prog_t pt;
	u8 cpcp, dscp, spcp, dei;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;

	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.pcindex = (((pi & 0xF) << 4) | (parm->nTrafficClass));
	dscp = (parm->nDSCP[parm->nTrafficClass] & 0x3F);
	spcp = (parm->nSPCP[parm->nTrafficClass] & 0x7);
	cpcp = (parm->nCPCP[parm->nTrafficClass] & 0x7);
	dei = ((parm->nSPCP[parm->nTrafficClass] >> 7) & 1);
	pt.val[1] = ((spcp << 8) | dei);
	pt.val[0] = (dscp | (cpcp << 8));
	pt.table = PCE_EGREMARK_INDEX;
	pt.opmode = PCE_OPMODE_ADWR;
	s = eg_port_tbl_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_SVLAN_PCP_ClassGet(void *pdev,
                                        GSW_QoS_SVLAN_PCP_ClassCfg_t *parm)
{
	GSW_return_t s;
	u8 i;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	memset(&pt, 0, sizeof(pctbl_prog_t));
	for (i = 0; i < SVLAN_PCP_TBL_SIZE; i++) {
		pt.table = PCE_SPCP_INDEX;
		pt.pcindex = i;
		pt.opmode = PCE_OPMODE_ADRD;
		s = stag_tbl_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		parm->nTrafficClass[i] = pt.val[0] & 0xF;
		parm->nTrafficColor[i] = ((pt.val[0] >> 6) & 0x3);
		parm->nPCP_Remark_Enable[i] = ((pt.val[0] >> 4) & 0x1);
		parm->nDEI_Remark_Enable[i] = ((pt.val[0] >> 5) & 0x1);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_QoS_SVLAN_PCP_ClassSet(void *pdev,
                                        GSW_QoS_SVLAN_PCP_ClassCfg_t *parm)
{
	GSW_return_t s;
	u8 i;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	for (i = 0; i < SVLAN_PCP_TBL_SIZE; i++) {
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_SPCP_INDEX;
		pt.pcindex = i;
		pt.val[0] = parm->nTrafficClass[i] & 0xF;
		pt.val[0] |=
		    (parm->nTrafficColor[i] & 0x3) << 6;
		pt.val[0] |=
		    (parm->nPCP_Remark_Enable[i] & 0x1) << 4;
		pt.val[0] |=
		    (parm->nDEI_Remark_Enable[i] & 0x1) << 5;
		pt.opmode = PCE_OPMODE_ADWR;
		s = stag_tbl_wr(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}
#endif /*CONFIG_LTQ_VLAN */
