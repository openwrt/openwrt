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
extern GSW_return_t gsw_reg_rd(void *c, u16 os, u16 s, u16 sz, ur *v);
extern GSW_return_t gsw_reg_wr(void *c, u16 os, u16 s, u16 sz, u32 v);
extern int pce_action_delete(void *pdev, tft_tbl_t *tpt, u32 index);
extern int pce_rule_read(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm);
extern int pce_rule_write(void *pdev, tft_tbl_t *tpt, GSW_PCE_rule_t *parm);
extern int pce_pattern_delete(void *pdev, tft_tbl_t *tpt, u32 index);
GSW_return_t bm_ram_tbl_wr(void *pdev, bm_tbl_prog_t *pb);
GSW_return_t bm_ram_tbl_rd(void *pdev, bm_tbl_prog_t *pb);
void multi_table_init(void *pdev);
/*number of ports */
GSW_return_t npport(void *pdev, u8 port);
GSW_return_t pctrl_bas_busy(void *pdev);
GSW_return_t pctrl_bas_set(void *pdev);
GSW_return_t pctrl_reg_clr(void *pdev);
GSW_return_t pctrl_addr_opmod(void *p, u16 ad, u16 op);
/* Number of supporetd Queues */
GSW_return_t nsqueue(void *pdev, u8 queue);
GSW_return_t bmctrl_bas_set(void *pdev);
GSW_return_t bmctrl_bas_busy(void *pdev);
GSW_return_t config_sw_def_vals(void *pdev);
extern GSW_return_t gsw_uc_code_init(void *pdev);
/**************************************************************************/
/*      DEFINES:                                                          */
/**************************************************************************/
u8 VER_NAME[] = GSW_API_MODULE_NAME " for GSWITCH Platform";
u8 VER_NUM[] = SWAPI_DRV_VERSION;
u8 GSWIP_VER_NAME[] = " GSWIP macro revision ID";
u8 GSWIP_VER_NUM[] = "0x022";
#define GSW_REG_SHIFT		0
#define GSW_REG_SIZE		16
#define MAC_TABLE_SIZE 2048
/*SGMII Auto negotiation Enabled(1)/Dsiabled(0) */
#define SGMII_AUTO_NEG_ENABLED 1
/**************************************************************************/
/*      LOCAL DECLARATIONS:                                               */
/**************************************************************************/
/* Port redirect flag */
static u8 prdflag;
/*Monitor port id*/
static u8 mpnum;
static ur num_ports;
port_config_t gpc[MAX_PORT_NUMBER];
stp8021x_t gstpconfig;
static u32 firstmac, mac_rd_index;
static pstpstate_t stps[] = {
	{	GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_AUTHORIZED,
		1, PORT_STATE_FORWARDING, 1
	},
	{	GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		1, PORT_STATE_LISTENING, 1
	},
	{	GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		1, PS_RENABLE_TDISABLE, 1
	},
	{	GSW_STP_PORT_STATE_FORWARD, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		1, PS_RDISABLE_TENABLE, 1
	},
	{	GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_AUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_DISABLE, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		0, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_AUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{	GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{	GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{	GSW_STP_PORT_STATE_LEARNING, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		1, PORT_STATE_LEARNING, 1
	},
	{	GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_AUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_UNAUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_RX_AUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	},
	{	GSW_STP_PORT_STATE_BLOCKING, GSW_8021X_PORT_STATE_TX_AUTHORIZED,
		1, PORT_STATE_LISTENING, 0
	}
};

static gsw_capdesc_t capdes[] = {
	{ GSW_CAP_TYPE_PORT, "Number of Ethernet ports"},
	{ GSW_CAP_TYPE_VIRTUAL_PORT, "Number of virtual ports"},
	{ GSW_CAP_TYPE_BUFFER_SIZE, "Pcket buffer size[in Bytes]:"},
	{ GSW_CAP_TYPE_SEGMENT_SIZE, "Buffer Segment size:"},
	{ GSW_CAP_TYPE_PRIORITY_QUEUE, "Number of queues:"},
	{ GSW_CAP_TYPE_METER, "Number of traffic meters:"},
	{ GSW_CAP_TYPE_RATE_SHAPER, "Number of rate shapers:"},
	{ GSW_CAP_TYPE_VLAN_GROUP, "Number of VLAN groups:"},
	{ GSW_CAP_TYPE_FID, "Number of FIDs:"},
	{ GSW_CAP_TYPE_MAC_TABLE_SIZE, "Number of MAC entries:"},
	{ GSW_CAP_TYPE_MULTICAST_TABLE_SIZE, "Number of multicast entries"},
	{ GSW_CAP_TYPE_PPPOE_SESSION, "Number of PPPoE sessions:"},
	{ GSW_CAP_TYPE_SVLAN_GROUP, "Number of STAG VLAN groups:"},
	{ GSW_CAP_TYPE_LAST, "Last Capability Index"}
};

static void pce_table_init(tft_tbl_t *tf);
static GSW_return_t en_buffer_res(void *pdev);
static GSW_return_t mac_br_rd(void *pdev,
                              pctbl_prog_t *pt);
static GSW_return_t mac_tbl_key_rd(void *pdev, pctbl_prog_t *pt);
static GSW_return_t mac_tbl_key_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t mac_tbl_addr_wr(void *pdev, pctbl_prog_t *pt);
static GSW_return_t get_hw_cap(void *pdev);
static GSW_return_t port_cfg_init(void *pdev);
/**************************************************************************/
/*      LOCAL FUNCTIONS Implementation::                                  */
/**************************************************************************/
/* Software  Table Init routine */
static void pce_table_init(tft_tbl_t *tf)
{
	int i;
	memset(&tf->tsub_tbl, 0, sizeof(pcetbl_prog_t));
	memset(&tf->sub_tbl, 0, sizeof(sub_tbl_t));
	memset(&tf->pce_act, 0, sizeof(GSW_PCE_action_t));
	for (i = 0; i < PCE_TABLE_SIZE; i++)
		tf->ptblused[i] = 0;
}

void multi_table_init(void *pdev)
{
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	u32 i;
	memset(&pd->iflag, 0, sizeof(gsw_igmp_t));
	pd->iflag.itblsize = pd->mctblsize;
	for (i = 0; i < pd->iflag.itblsize; i++) {
		pd->iflag.mctable[i].slsbindex = 0x7F;
		pd->iflag.mctable[i].dlsbindex = 0x7F;
		pd->iflag.mctable[i].smsbindex = 0x1F;
		pd->iflag.mctable[i].dmsbindex = 0x1F;
	}
}

static GSW_return_t port_cfg_init(void *pdev)
{
	ur r, i;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r; i++) {
		memset(&gpc[i], 0, sizeof(port_config_t));
		gpc[i].llimit = 0xFF;
		gpc[i].penable = 1;
	}
	gstpconfig.sfport = GSW_PORT_FORWARD_DEFAULT;
	gstpconfig.fpid8021x = pd->cport;
	return GSW_statusOk;
}

static GSW_return_t get_hw_cap(void *pdev)
{
	ur r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->pnum = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_VPORTS_OFFSET,
	               ETHSW_CAP_1_VPORTS_SHIFT,
	               ETHSW_CAP_1_VPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->tpnum = r + pd->pnum;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_QUEUE_OFFSET,
	               ETHSW_CAP_1_QUEUE_SHIFT,
	               ETHSW_CAP_1_QUEUE_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->nqueues = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_METERS_OFFSET,
	               ETHSW_CAP_3_METERS_SHIFT,
	               ETHSW_CAP_3_METERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->nmeters = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_3_SHAPERS_OFFSET,
	               ETHSW_CAP_3_SHAPERS_SHIFT,
	               ETHSW_CAP_3_SHAPERS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->nshapers = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_4_PPPOE_OFFSET,
	               ETHSW_CAP_4_PPPOE_SHIFT,
	               ETHSW_CAP_4_PPPOE_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->npppoe = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_4_VLAN_OFFSET,
	               ETHSW_CAP_4_VLAN_SHIFT,
	               ETHSW_CAP_4_VLAN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->avlantsz = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_5_IPPLEN_OFFSET,
	               ETHSW_CAP_5_IPPLEN_SHIFT,
	               ETHSW_CAP_5_IPPLEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->ip_pkt_lnt_size = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_5_PROT_OFFSET,
	               ETHSW_CAP_5_PROT_SHIFT,
	               ETHSW_CAP_5_PROT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->prot_table_size = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_6_MACDASA_OFFSET,
	               ETHSW_CAP_6_MACDASA_SHIFT,
	               ETHSW_CAP_6_MACDASA_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->mac_dasa_table_size = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_6_APPL_OFFSET,
	               ETHSW_CAP_6_APPL_SHIFT,
	               ETHSW_CAP_6_APPL_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->app_table_size = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_7_IPDASAM_OFFSET,
	               ETHSW_CAP_7_IPDASAM_SHIFT,
	               ETHSW_CAP_7_IPDASAM_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->idsmtblsize = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_7_IPDASAL_OFFSET,
	               ETHSW_CAP_7_IPDASAL_SHIFT,
	               ETHSW_CAP_7_IPDASAL_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->idsltblsize = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_8_MCAST_OFFSET,
	               ETHSW_CAP_8_MCAST_SHIFT,
	               ETHSW_CAP_8_MCAST_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->mctblsize = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_9_FLAGG_OFFSET,
	               ETHSW_CAP_9_FLAGG_SHIFT,
	               ETHSW_CAP_9_FLAGG_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->tftblsize = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_10_MACBT_OFFSET,
	               ETHSW_CAP_10_MACBT_SHIFT,
	               ETHSW_CAP_10_MACBT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->mactblsize = r;
	s = gsw_reg_rd(pdev,
	               ETHSW_VERSION_REV_ID_OFFSET,
	               ETHSW_VERSION_REV_ID_SHIFT,
	               ETHSW_VERSION_REV_ID_REG_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pd->gipver = r;
	pd->cport = GSW_2X_SOC_CPU_PORT;
	return GSW_statusOk;
}

/* Enable or Disable LPI mode for all ports */
static GSW_return_t mac_lpi_mode(void *pdev, int en)
{
	ur r, i;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r;  i++) {
		if (en == 1)
			s = gsw_reg_wr(pdev,
			               (MAC_CTRL_4_LPIEN_OFFSET + (0xC * i)),
			               MAC_CTRL_4_LPIEN_SHIFT,
			               MAC_CTRL_4_LPIEN_SIZE, 1);
		else
			s = gsw_reg_wr(pdev,
			               (MAC_CTRL_4_LPIEN_OFFSET + (0xC * i)),
			               MAC_CTRL_4_LPIEN_SHIFT,
			               MAC_CTRL_4_LPIEN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t sgmii_init(void *pdev)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_1_VLANMD_OFFSET,
	               PCE_GCTRL_1_VLANMD_SHIFT,
	               PCE_GCTRL_1_VLANMD_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xfa01, 0, 16, 0x0030);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xfa01, 0, 14, 0x0010);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD009, 0, 16, 0x0009);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD004, 0, 16, 0x053A);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD305, 0, 16, 0x0033);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD305, 0, 16, 0x0032);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD404, 0, 16, 0x0003);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD404, 0, 16, 0x0001);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD401, 0, 16, 0x0003);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD401, 0, 16, 0x0001);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD300, 0, 16, 0x0018);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD301, 0, 16, 0x0001);
	if (s != GSW_statusOk)
		return s;
#if defined(SGMII_AUTO_NEG_ENABLED) && SGMII_AUTO_NEG_ENABLED
	s = gsw_reg_wr(pdev, 0xD304, 0, 16, 0x80bc);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD304, 0, 16, 0x80b4);
	if (s != GSW_statusOk)
		return s;
#else
	s = gsw_reg_wr(pdev, 0xD304, 0, 16, 0x80b8);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD304, 0, 16, 0x80b0);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD30A, 0, 16, 0x0041);
	if (s != GSW_statusOk)
		return s;
#endif /* SGMII_AUTO_NEG_ENABLED */
	s = gsw_reg_wr(pdev, 0xD100, 0, 16, 0x80);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD101, 0, 16, 0x30);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev, 0xD102, 0, 16, 0x1100);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t en_buffer_res(void *pdev)
{
	u8 i;
	ur r;
	GSW_return_t s;
	GSW_QoS_QueueBufferReserveCfg_t br;
	GSW_QoS_WRED_Cfg_t wr;
	GSW_QoS_FlowCtrlPortCfg_t pfc;
	GSW_QoS_FlowCtrlCfg_t fc;
//	GSW_QoS_WRED_QueueCfg_t wq;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&br, 0, sizeof(GSW_QoS_QueueBufferReserveCfg_t));
	memset(&wr, 0, sizeof(GSW_QoS_WRED_Cfg_t));
	memset(&pfc, 0, sizeof(GSW_QoS_FlowCtrlPortCfg_t));
	memset(&fc, 0, sizeof(GSW_QoS_FlowCtrlCfg_t));
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < 32; i++) {
		br.nBufferReserved = 0x1E;
		br.nQueueId = i;
		s = GSW_QoS_QueueBufferReserveCfgSet(pdev, &br);
		if (s != GSW_statusOk)
			return s;
	}
	wr.eProfile = 0;
	wr.eThreshMode = 0;
	wr.nRed_Min = 0x3ff;
	wr.nRed_Max =0x3ff;
	wr.nYellow_Min = 0x3ff;
	wr.nYellow_Max = 0x3ff;
	wr.nGreen_Min = 0x100;
	wr.nGreen_Max =0x100;
	s = GSW_QoS_WredCfgSet(pdev, &wr);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r; i++) {
		pfc.nPortId = i;
		pfc.nFlowCtrl_Min = 0x18;
		pfc.nFlowCtrl_Max = 0x1E;
		s = GSW_QoS_FlowctrlPortCfgSet(pdev, &pfc);
		if (s != GSW_statusOk) {
			return s;
		}
	}
	fc.nFlowCtrlNonConform_Min = 0x3FF;
	fc.nFlowCtrlNonConform_Max = 0x3FF;
	fc.nFlowCtrlConform_Min = 0x3FF;
	fc.nFlowCtrlConform_Max = 0x3FF;
	s = GSW_QoS_FlowctrlCfgSet(pdev, &fc);
	if (s != GSW_statusOk)
		return s;
	/*
		for (i = 0; i < 32; i++) {
			memset(&wq, 0, sizeof(GSW_QoS_WRED_QueueCfg_t));
			wq.nRed_Min = 80;
			wq.nRed_Max = 80;
			wq.nYellow_Min = 80;
			wq.nYellow_Max = 80;
			wq.nGreen_Min = 0x1ff;
			wq.nGreen_Max = 0x1ff;
			wq.nQueueId = i;
			s = GSW_QoS_WredQueueCfgSet(pdev, &wq);
			if (s != GSW_statusOk) {
				return s;
			}
		}
	*/
	return GSW_statusOk;
}

GSW_return_t config_sw_def_vals(void *pdev)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	prdflag = 0;
	mpnum = 0;
	num_ports = 0;
	firstmac = 0;
	mac_rd_index = 0;
	s = get_hw_cap(pdev);
	if (s != GSW_statusOk)
		return s;
	s = port_cfg_init(pdev);
	if (s != GSW_statusOk)
		return s;
#if defined(CONFIG_LTQ_MULTICAST) && CONFIG_LTQ_MULTICAST
	multi_table_init(pdev);
#endif /*CONFIG_LTQ_MULTICAST*/
	pce_table_init(&pd->ptft);
	pd->matimer = DEFAULT_AGING_TIMEOUT;
	s = GSW_Disable(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               GSWIP_CFG_SE_OFFSET,
	               GSWIP_CFG_SE_SHIFT,
	               GSWIP_CFG_SE_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	s = gsw_uc_code_init(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_PMAP_2_DMCPMAP_OFFSET,
	               PCE_PMAP_2_DMCPMAP_SHIFT,
	               PCE_PMAP_2_DMCPMAP_SIZE, 0x7F);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_PMAP_3_UUCMAP_OFFSET,
	               PCE_PMAP_3_UUCMAP_SHIFT,
	               PCE_PMAP_3_UUCMAP_SIZE, 0x7F);
	if (s != GSW_statusOk)
		return s;
	s = en_buffer_res(pdev);
	if (s != GSW_statusOk)
		return s;
	s = mac_lpi_mode (pdev, 1);
	if (s != GSW_statusOk)
		return s;
	s = GSW_Enable(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_1_VLANMD_OFFSET,
	               PCE_GCTRL_1_VLANMD_SHIFT,
	               PCE_GCTRL_1_VLANMD_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

static GSW_return_t mac_br_rd(void *pdev,
                              pctbl_prog_t *pt)
{
	ur r;
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_ADDR_OFFSET,
	               PCE_TBL_CTRL_ADDR_SHIFT,
	               PCE_TBL_CTRL_ADDR_REG_SIZE, 0x000B);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_VLD_OFFSET,
	               PCE_TBL_CTRL_VLD_SHIFT,
	               PCE_TBL_CTRL_VLD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->valid = r;
	if (pt->valid == 1) {
		s = gsw_reg_rd(pdev,
		               PCE_TBL_KEY_3_KEY3_OFFSET,
		               PCE_TBL_KEY_3_KEY3_SHIFT,
		               PCE_TBL_KEY_3_KEY3_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		pt->key[3] = r;
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
		               PCE_TBL_MASK_0_MASK0_OFFSET,
		               PCE_TBL_MASK_0_MASK0_SHIFT,
		               PCE_TBL_MASK_0_MASK0_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		pt->mask[0] = r;
	}
	return GSW_statusOk;
}

static GSW_return_t mac_tbl_key_rd(void *pdev,
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
	               PCE_TBL_KEY_0_KEY0_SIZE, pt->key[0]);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
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
	s = gsw_reg_rd(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pt->pcindex = r;
	return GSW_statusOk;
}

static GSW_return_t mac_tbl_key_wr(void *pdev,
                                   pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
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

static GSW_return_t mac_tbl_addr_wr(void *pdev,
                                    pctbl_prog_t *pt)
{
	GSW_return_t s;
	s = pctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_reg_clr(pdev);
	if (s != GSW_statusOk)
		return s;
	s = pctrl_addr_opmod(pdev, pt->table, pt->opmode);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_ADDR_ADDR_OFFSET,
	               PCE_TBL_ADDR_ADDR_SHIFT,
	               PCE_TBL_ADDR_ADDR_SIZE,
	               pt->pcindex);
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

GSW_return_t bm_ram_tbl_wr(void *pdev,
                           bm_tbl_prog_t *pb)
{
	GSW_return_t s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_ADDR_ADDR_OFFSET,
	               BM_RAM_ADDR_ADDR_SHIFT,
	               BM_RAM_ADDR_ADDR_SIZE,
	               pb->bmindex);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_VAL_0_VAL0_OFFSET,
	               BM_RAM_VAL_0_VAL0_SHIFT,
	               BM_RAM_VAL_0_VAL0_SIZE,
	               pb->bmval[0]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_VAL_1_VAL1_OFFSET,
	               BM_RAM_VAL_1_VAL1_SHIFT,
	               BM_RAM_VAL_1_VAL1_SIZE,
	               pb->bmval[1]);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_CTRL_ADDR_OFFSET,
	               BM_RAM_CTRL_ADDR_SHIFT,
	               BM_RAM_CTRL_ADDR_SIZE,
	               pb->bmtable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_CTRL_OPMOD_OFFSET,
	               BM_RAM_CTRL_OPMOD_SHIFT,
	               BM_RAM_CTRL_OPMOD_SIZE,
	               pb->bmopmode);
	if (s != GSW_statusOk)
		return s;
	s = bmctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = bmctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t bm_ram_tbl_rd(void *pdev,
                           bm_tbl_prog_t *pb)
{
	ur r;
	GSW_return_t s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_ADDR_ADDR_OFFSET,
	               BM_RAM_ADDR_ADDR_SHIFT,
	               BM_RAM_ADDR_ADDR_SIZE,
	               pb->bmindex);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_CTRL_ADDR_OFFSET,
	               BM_RAM_CTRL_ADDR_SHIFT,
	               BM_RAM_CTRL_ADDR_SIZE,
	               pb->bmtable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_CTRL_OPMOD_OFFSET,
	               BM_RAM_CTRL_OPMOD_SHIFT,
	               BM_RAM_CTRL_OPMOD_SIZE,
	               pb->bmopmode);
	if (s != GSW_statusOk)
		return s;
	s = bmctrl_bas_set(pdev);
	if (s != GSW_statusOk)
		return s;
	s = bmctrl_bas_busy(pdev);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               BM_RAM_VAL_0_VAL0_OFFSET,
	               BM_RAM_VAL_0_VAL0_SHIFT,
	               BM_RAM_VAL_0_VAL0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pb->bmval[0] = r;
	s = gsw_reg_rd(pdev,
	               BM_RAM_VAL_1_VAL1_OFFSET,
	               BM_RAM_VAL_1_VAL1_SHIFT,
	               BM_RAM_VAL_1_VAL1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	pb->bmval[1] = r;

	return GSW_statusOk;
}

/**************************************************************************/
/*      INTERFACE FUNCTIONS Implementation:                               */
/**************************************************************************/
GSW_return_t npport(void *pdev, u8 port)
{
	ur r;
	GSW_return_t s;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	num_ports = r;
	if (port >= r)
		return GSW_statusNoSupport;
	return GSW_statusOk;
}

GSW_return_t nsqueue(void *pdev, u8 queue)
{
	ur r;
	GSW_return_t s;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_QUEUE_OFFSET,
	               ETHSW_CAP_1_QUEUE_SHIFT,
	               ETHSW_CAP_1_QUEUE_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (queue >= r)
		return GSW_statusValueRange;
	return GSW_statusOk;
}

GSW_return_t pctrl_bas_busy(void *pdev)
{
	ur r;
	GSW_return_t s;
	do {
		s = gsw_reg_rd(pdev,
		               PCE_TBL_CTRL_BAS_OFFSET,
		               PCE_TBL_CTRL_BAS_SHIFT,
		               PCE_TBL_CTRL_BAS_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	}	while (r);
	return GSW_statusOk;
}

GSW_return_t pctrl_bas_set(void *pdev)
{
	GSW_return_t s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_BAS_OFFSET,
	               PCE_TBL_CTRL_BAS_SHIFT,
	               PCE_TBL_CTRL_BAS_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t bmctrl_bas_busy(void *pdev)
{
	ur r;
	GSW_return_t s;
	do {
		s = gsw_reg_rd(pdev,
		               BM_RAM_CTRL_BAS_OFFSET,
		               BM_RAM_CTRL_BAS_SHIFT,
		               BM_RAM_CTRL_BAS_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	return GSW_statusOk;
}

GSW_return_t bmctrl_bas_set(void *pdev)
{
	GSW_return_t s;
	s = gsw_reg_wr(pdev,
	               BM_RAM_CTRL_BAS_OFFSET,
	               BM_RAM_CTRL_BAS_SHIFT,
	               BM_RAM_CTRL_BAS_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t pctrl_addr_opmod(void *pdev,
                              u16 addr, u16 opmode)
{
	ur r;
	GSW_return_t s;
	s = gsw_reg_rd(pdev,
	               PCE_TBL_CTRL_ADDR_OFFSET,
	               PCE_TBL_CTRL_ADDR_SHIFT,
	               PCE_TBL_CTRL_ADDR_OPMOD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	r &= ~(0x7F);
	r |= (((opmode & 3) << 5) | (addr & 0x1F));
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_ADDR_OFFSET,
	               PCE_TBL_CTRL_ADDR_SHIFT,
	               PCE_TBL_CTRL_ADDR_OPMOD_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t pctrl_reg_clr(void *pdev)
{
	GSW_return_t s;
	s = gsw_reg_wr(pdev,
	               PCE_TBL_CTRL_ADDR_OFFSET,
	               PCE_TBL_CTRL_ADDR_SHIFT,
	               PCE_TBL_CTRL_ADDR_REG_SIZE, 0);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_MAC_TableEntryRead(void *pdev,
                                    GSW_MAC_tableRead_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ur r = 1;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	memset(&pt, 0, sizeof(pctbl_prog_t));
	if (parm->bInitial == 1) {
		/*Start from the index 0 */
		mac_rd_index = 0;
		firstmac = 0;
		parm->bInitial = 0;
		s = gsw_reg_wr(pdev,
		               PCE_TBL_ADDR_ADDR_OFFSET,
		               PCE_TBL_ADDR_ADDR_SHIFT,
		               PCE_TBL_ADDR_ADDR_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PCE_TBL_CTRL_ADDR_OFFSET,
		               PCE_TBL_CTRL_ADDR_SHIFT,
		               PCE_TBL_CTRL_ADDR_REG_SIZE,
		               0xC00B);
		if (s != GSW_statusOk)
			return s;
		s = pctrl_bas_busy(pdev);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_rd(pdev,
		               PCE_TBL_ADDR_ADDR_OFFSET,
		               PCE_TBL_ADDR_ADDR_SHIFT,
		               PCE_TBL_ADDR_ADDR_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		firstmac = r;
	}
	if ((mac_rd_index >= MAC_TABLE_SIZE /*pd->mactblsize*/) || (r == 0)) {
		memset(parm, 0, sizeof(GSW_MAC_tableRead_t));
		parm->bLast = 1;
		mac_rd_index = 0;
		s = gsw_reg_wr(pdev,
		               PCE_TBL_CTRL_ADDR_OFFSET,
		               PCE_TBL_CTRL_ADDR_SHIFT,
		               PCE_TBL_CTRL_ADDR_REG_SIZE,
		               0x000B);
		if (s != GSW_statusOk)
			return s;
		return GSW_statusOk;
	}
	mac_rd_index++;
	if ((parm->bInitial == 0) && (mac_rd_index >= 1)) {
		s = gsw_reg_wr(pdev,
		               PCE_TBL_CTRL_ADDR_OFFSET,
		               PCE_TBL_CTRL_ADDR_SHIFT,
		               PCE_TBL_CTRL_ADDR_REG_SIZE,
		               0xC00B);
		if (s != GSW_statusOk)
			return s;
		s = pctrl_bas_busy(pdev);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_rd(pdev,
		               PCE_TBL_ADDR_ADDR_OFFSET,
		               PCE_TBL_ADDR_ADDR_SHIFT,
		               PCE_TBL_ADDR_ADDR_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		if (firstmac == r) {
			s = mac_br_rd(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
			if ((pt.valid == 1)) {
				parm->nFId = pt.key[3] & 0x3F;
				parm->bStaticEntry = (pt.val[1] & 0x1);
				if (parm->bStaticEntry == 1) {
					parm->nAgeTimer = 0;
					parm->nPortId = pt.val[0];
				} else {
					u32 timer = 300, mant;
					ur rreg;
					/* Aging Counter Mantissa Value */
					s = gsw_reg_rd(pdev,
					               PCE_AGE_1_MANT_OFFSET,
					               PCE_AGE_1_MANT_SHIFT,
					               PCE_AGE_1_MANT_SIZE,
					               &rreg);
					if (s != GSW_statusOk)
						return s;
					mant = rreg;
					switch (mant) {
					case AGETIMER_1_DAY:
						timer = 86400;
						break;
					case AGETIMER_1_HOUR:
						timer = 3600;
						break;
					case AGETIMER_300_SEC:
						timer = 300;
						break;
					case AGETIMER_10_SEC:
						timer = 10;
						break;
					case AGETIMER_1_SEC:
						timer = 1;
						break;
					}
					parm->nAgeTimer =	pt.val[0] & 0xF;
					parm->nAgeTimer =	(timer * parm->nAgeTimer)/0xF;
					parm->nPortId =	(pt.val[0] >> 4) & 0xF;
				}
				parm->nMAC[0] = pt.key[2] >> 8;
				parm->nMAC[1] = pt.key[2] & 0xFF;
				parm->nMAC[2] = pt.key[1] >> 8;
				parm->nMAC[3] = pt.key[1] & 0xFF;
				parm->nMAC[4] = pt.key[0] >> 8;
				parm->nMAC[5] = pt.key[0] & 0xFF;
				parm->nSVLAN_Id = ((pt.val[1] >> 4) & 0xFFF);
			}
			parm->bLast = 1;
			mac_rd_index = 0;
			s = gsw_reg_wr(pdev,
			               PCE_TBL_CTRL_ADDR_OFFSET,
			               PCE_TBL_CTRL_ADDR_SHIFT,
			               PCE_TBL_CTRL_ADDR_REG_SIZE,
			               0x000B);
			if (s != GSW_statusOk)
				return s;
			return GSW_statusOk;
		}
	}
	s = mac_br_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	if ((pt.valid == 1)) {
		parm->nFId = pt.key[3] & 0x3F;
		parm->bStaticEntry = (pt.val[1] & 0x1);
		if (parm->bStaticEntry == 1) {
			parm->nAgeTimer = 0;
			parm->nPortId = pt.val[0];
		} else {
			u32 timer = 300, mant;
			ur rreg;
			/* Aging Counter Mantissa Value */
			s = gsw_reg_rd(pdev,
			               PCE_AGE_1_MANT_OFFSET,
			               PCE_AGE_1_MANT_SHIFT,
			               PCE_AGE_1_MANT_SIZE, &rreg);
			if (s != GSW_statusOk)
				return s;
			mant = rreg;
			switch (mant) {
			case AGETIMER_1_DAY:
				timer = 86400;
				break;
			case AGETIMER_1_HOUR:
				timer = 3600;
				break;
			case AGETIMER_300_SEC:
				timer = 300;
				break;
			case AGETIMER_10_SEC:
				timer = 10;
				break;
			case AGETIMER_1_SEC:
				timer = 1;
				break;
			}
			parm->nAgeTimer =	pt.val[0] & 0xF;
			parm->nAgeTimer =	(timer * parm->nAgeTimer)/0xF;
			parm->nPortId =	(pt.val[0] >> 4) & 0xF;
		}
		parm->nMAC[0] = pt.key[2] >> 8;
		parm->nMAC[1] = pt.key[2] & 0xFF;
		parm->nMAC[2] = pt.key[1] >> 8;
		parm->nMAC[3] = pt.key[1] & 0xFF;
		parm->nMAC[4] = pt.key[0] >> 8;
		parm->nMAC[5] = pt.key[0] & 0xFF;
		parm->nSVLAN_Id = ((pt.val[1] >> 4) & 0xFFF);
		parm->bInitial = 0;
		parm->bLast = 0;
	} else {
		memset(parm, 0, sizeof(GSW_MAC_tableRead_t));
		parm->bLast = 1;
		s = gsw_reg_wr(pdev,
		               PCE_TBL_CTRL_ADDR_OFFSET,
		               PCE_TBL_CTRL_ADDR_SHIFT,
		               PCE_TBL_CTRL_ADDR_REG_SIZE, 0x000B);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MAC_TableEntryQuery(void *pdev,
                                     GSW_MAC_tableQuery_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	parm->bFound = 0;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.table = PCE_MAC_BRIDGE_INDEX;
	pt.key[0] = parm->nMAC[4] << 8 | parm->nMAC[5];
	pt.key[1] = parm->nMAC[2] << 8 | parm->nMAC[3];
	pt.key[2] = parm->nMAC[0] << 8 | parm->nMAC[1];
	pt.key[3] = parm->nFId;
	pt.opmode = PCE_OPMODE_KSRD;
	s = mac_tbl_key_rd(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	if (pt.valid == 1) {
		parm->bFound = 1;
		parm->bStaticEntry = (pt.val[1] & 0x1);
		parm->nSVLAN_Id = ((pt.val[1] >> 4) & 0xFFF);
		if ((pt.val[1] & 0x1) == 1) {
			parm->nAgeTimer = 0;
			parm->nPortId = (pt.val[0]);
		} else {
			u32 mant, timer = 300;
			/* Aging Counter Mantissa Value */
			s = gsw_reg_rd(pdev,
			               PCE_AGE_1_MANT_OFFSET,
			               PCE_AGE_1_MANT_SHIFT,
			               PCE_AGE_1_MANT_SIZE,
			               &mant);
			if (s != GSW_statusOk)
				return s;
			switch (mant) {
			case AGETIMER_1_DAY:
				timer = 86400;
				break;
			case AGETIMER_1_HOUR:
				timer = 3600;
				break;
			case AGETIMER_300_SEC:
				timer = 300;
				break;
			case AGETIMER_10_SEC:
				timer = 10;
				break;
			case AGETIMER_1_SEC:
				timer = 1;
				break;
			}
			parm->nAgeTimer = pt.val[0] & 0xF;
			parm->nAgeTimer = (timer * parm->nAgeTimer)/0xF;
			parm->nPortId = (pt.val[0] >> 4) & 0xF;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MAC_TableEntryAdd(void *pdev,
                                   GSW_MAC_tableAdd_t *parm)
{
	GSW_return_t s;
	u32 pi = parm->nPortId;
	pctbl_prog_t pt;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, (pi & 0xF));
	if (s != GSW_statusOk)
		return s;
	memset(&pt, 0, sizeof(pctbl_prog_t));
	pt.key[0] = parm->nMAC[4] << 8 | parm->nMAC[5];
	pt.key[1] = parm->nMAC[2] << 8 | parm->nMAC[3];
	pt.key[2] = parm->nMAC[0] << 8 | parm->nMAC[1];
	pt.key[3] = parm->nFId;
	pt.val[1] = ((parm->nSVLAN_Id & 0xFFF) << 4);
	pt.val[1] |= (1 << 1);
	if (parm->bStaticEntry) {
		if (pi & 0x80000000) { /*Port Map */
			pt.val[0] = (pi & 0x7FFF);
		} else {
			pt.val[0] = (1 << pi);
		}
		pt.val[1] |= 1;
	} else {
		pt.val[0] = (((pi & 0xF) << 4) | (parm->nAgeTimer & 0xF));
	}
	pt.valid = 1;
	pt.table = PCE_MAC_BRIDGE_INDEX;
	pt.opmode = PCE_OPMODE_KSWR;
	s = mac_tbl_key_wr(pdev, &pt);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_MAC_TableEntryRemove(void *pdev,
                                      GSW_MAC_tableRemove_t *parm)
{
	GSW_return_t s;
	pctbl_prog_t pt;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/* Learning Limit Port Lock */
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_MTFL_OFFSET,
	               PCE_GCTRL_0_MTFL_SHIFT,
	               PCE_GCTRL_0_MTFL_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (!r) {
		memset(&pt, 0, sizeof(pctbl_prog_t));
		pt.table = PCE_MAC_BRIDGE_INDEX;
		pt.opmode = PCE_OPMODE_KSRD;
		pt.key[0] = parm->nMAC[4] << 8 | parm->nMAC[5];
		pt.key[1] = parm->nMAC[2] << 8 | parm->nMAC[3];
		pt.key[2] = parm->nMAC[0] << 8 | parm->nMAC[1];
		pt.key[3] = parm->nFId;
		s = mac_tbl_key_rd(pdev, &pt);
		if (s != GSW_statusOk)
			return s;
		if (pt.valid == 1) {
			pctbl_prog_t pt1;
			memset(&pt1, 0, sizeof(pctbl_prog_t));
			pt1.table = PCE_MAC_BRIDGE_INDEX;
			pt1.pcindex = pt.pcindex;
			pt.opmode = PCE_OPMODE_ADWR;
			s = mac_tbl_addr_wr(pdev, &pt);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MAC_TableClear(void *pdev)
{
	GSW_return_t s;
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_MTFL_OFFSET,
	               PCE_GCTRL_0_MTFL_SHIFT,
	               PCE_GCTRL_0_MTFL_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_HW_Init(void *pdev,
                         GSW_HW_Init_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	switch (parm->eInitMode) {
	case GSW_HW_INIT_WR:
		s = gsw_reg_wr(pdev,
		               ETHSW_SWRES_R0_OFFSET,
		               ETHSW_SWRES_R0_SHIFT,
		               ETHSW_SWRES_R0_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = GSW_Disable(pdev);
		if (s != GSW_statusOk)
			return s;
		s = config_sw_def_vals(pdev);
		if (s != GSW_statusOk)
			return s;
		s = GSW_Enable(pdev);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_HW_INIT_RO:
		s = GSW_Disable(pdev);
		if (s != GSW_statusOk)
			return s;
		s = config_sw_def_vals(pdev);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_HW_INIT_NO:
		break;
	default:
		break;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_VersionGet(void *pdev,
                            GSW_version_t *parm)
{
	if (parm->nId == 0) {
		memcpy(parm->cName, VER_NAME, sizeof(VER_NAME));
		memcpy(parm->cVersion, VER_NUM, sizeof(VER_NUM));
	} else if (parm->nId == 1) {
		memcpy(parm->cName, GSWIP_VER_NAME,
		       sizeof(GSWIP_VER_NAME));
		memcpy(parm->cVersion, GSWIP_VER_NUM,
		       sizeof(GSWIP_VER_NUM));
	} else {
		memcpy(parm->cName, "", 0);
		memcpy(parm->cVersion, "", 0);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_CapGet(void *pdev, GSW_cap_t *parm)
{
	ur r, d1, d2;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (parm->nCapType >= GSW_CAP_TYPE_LAST)
		return GSW_statusParam;
	else
		strcpy(parm->cDesc, capdes[parm->nCapType].desci);
	switch (parm->nCapType) {
	case GSW_CAP_TYPE_PORT:
		s = gsw_reg_rd(pdev,
		               ETHSW_CAP_1_PPORTS_OFFSET,
		               ETHSW_CAP_1_PPORTS_SHIFT,
		               ETHSW_CAP_1_PPORTS_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nCap = r;
		break;
	case GSW_CAP_TYPE_VIRTUAL_PORT:
		s = gsw_reg_rd(pdev,
		               ETHSW_CAP_1_VPORTS_OFFSET,
		               ETHSW_CAP_1_VPORTS_SHIFT,
		               ETHSW_CAP_1_VPORTS_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nCap = r;
		break;
	case GSW_CAP_TYPE_BUFFER_SIZE:
		s = gsw_reg_rd(pdev,
		               ETHSW_CAP_11_BSIZEL_OFFSET,
		               ETHSW_CAP_11_BSIZEL_SHIFT,
		               ETHSW_CAP_11_BSIZEL_SIZE, &d1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_rd(pdev,
		               ETHSW_CAP_12_BSIZEH_OFFSET,
		               ETHSW_CAP_12_BSIZEH_SHIFT,
		               ETHSW_CAP_12_BSIZEH_SIZE, &d2);
		if (s != GSW_statusOk)
			return s;
		parm->nCap = (d2 << 16 | d1);
		break;
	case GSW_CAP_TYPE_SEGMENT_SIZE:
		/* This is Hard coded */
		parm->nCap = GSW_MEM_SEGMENT;
		break;
	case GSW_CAP_TYPE_PRIORITY_QUEUE:
		parm->nCap = pd->nqueues;
		break;
	case GSW_CAP_TYPE_METER:
		parm->nCap = pd->nmeters;
		break;
	case GSW_CAP_TYPE_RATE_SHAPER:
		parm->nCap = pd->nshapers;
		break;
	case GSW_CAP_TYPE_VLAN_GROUP:
		parm->nCap = pd->avlantsz;
		break;
	case GSW_CAP_TYPE_FID:
		/* This is Hard coded */
		parm->nCap = VRX_PLATFORM_CAP_FID;
		break;
	case GSW_CAP_TYPE_MAC_TABLE_SIZE:
		parm->nCap = pd->mactblsize;
		break;
	case GSW_CAP_TYPE_MULTICAST_TABLE_SIZE:
		parm->nCap = pd->mctblsize;
		break;
	case GSW_CAP_TYPE_PPPOE_SESSION:
		parm->nCap = pd->npppoe;
		break;
	case GSW_CAP_TYPE_SVLAN_GROUP:
		parm->nCap = pd->avlantsz;
		break;
	case GSW_CAP_TYPE_LAST:
		parm->nCap = GSW_CAP_TYPE_LAST;
		break;
	default:
		parm->nCap = 0;
		break;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_CfgSet(void *pdev,
                        GSW_cfg_t *parm)
{
	ur r;
	GSW_return_t s;
	u32 mant, exp, i;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);

	switch (parm->eMAC_TableAgeTimer) {
	case GSW_AGETIMER_1_SEC:
		mant = AGETIMER_1_SEC;
		exp = 0x2;
		pd->matimer = 1;
		break;
	case GSW_AGETIMER_10_SEC:
		mant = AGETIMER_10_SEC;
		exp = 0x2;
		pd->matimer = 10;
		break;
	case GSW_AGETIMER_300_SEC:
		mant = AGETIMER_300_SEC;
		exp = 0x2;
		pd->matimer = 300;
		break;
	case GSW_AGETIMER_1_HOUR:
		mant = AGETIMER_1_HOUR;
		exp = 0x6;
		pd->matimer = 3600;
		break;
	case GSW_AGETIMER_1_DAY:
		mant = AGETIMER_1_DAY;
		exp = 0xA;
		pd->matimer = 86400;
		break;
	default:
		mant = AGETIMER_300_SEC;
		exp = 0x2;
		pd->matimer = 300;
	}
	/* Aging Counter Exponent Value */
	s = gsw_reg_wr(pdev,
	               PCE_AGE_0_EXP_OFFSET,
	               PCE_AGE_0_EXP_SHIFT,
	               PCE_AGE_0_EXP_SIZE, exp);
	if (s != GSW_statusOk)
		return s;
	/* Aging Counter Mantissa Value */
	s = gsw_reg_wr(pdev,
	               PCE_AGE_1_MANT_OFFSET,
	               PCE_AGE_1_MANT_SHIFT,
	               PCE_AGE_1_MANT_SIZE, mant);
	if (s != GSW_statusOk)
		return s;
	/* Maximum Ethernet packet length */
	if (parm->nMaxPacketLen < 0xFFFF)
		r = parm->nMaxPacketLen;
	else
		r = MAX_PACKET_LENGTH;
	s = gsw_reg_wr(pdev,
	               MAC_FLEN_LEN_OFFSET,
	               MAC_FLEN_LEN_SHIFT,
	               MAC_FLEN_LEN_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	if (parm->nMaxPacketLen > 0x5EE) {
		for (i = 0; i < 7; i++) {
			s = gsw_reg_wr(pdev,
			               (MAC_CTRL_2_MLEN_OFFSET + (i * 0xC)),
			               MAC_CTRL_2_MLEN_SHIFT,
			               MAC_CTRL_2_MLEN_SIZE, 1);
			if (s != GSW_statusOk)
				return s;
		}
	}
	/* MAC Address Learning Limitation Mode */
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_PLIMMOD_OFFSET,
	               PCE_GCTRL_0_PLIMMOD_SHIFT,
	               PCE_GCTRL_0_PLIMMOD_SIZE,
	               parm->bLearningLimitAction);
	if (s != GSW_statusOk)
		return s;
	/*Accept or discard MAC spoofing and port */
	/* MAC locking violation packets */
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_PLCKMOD_OFFSET,
	               PCE_GCTRL_0_PLCKMOD_SHIFT,
	               PCE_GCTRL_0_PLCKMOD_SIZE,
	               parm->bMAC_SpoofingAction);
	if (s != GSW_statusOk)
		return s;
	/* VLAN-aware Switching */
	s = gsw_reg_wr(pdev,
	               PCE_GCTRL_0_VLAN_OFFSET,
	               PCE_GCTRL_0_VLAN_SHIFT,
	               PCE_GCTRL_0_VLAN_SIZE,
	               parm->bVLAN_Aware);
	if (s != GSW_statusOk)
		return s;
	/* MAC Source Address Mode */
	if (parm->bPauseMAC_ModeSrc == 1) {
		s = gsw_reg_wr(pdev,
		               MAC_PFAD_CFG_SAMOD_OFFSET,
		               MAC_PFAD_CFG_SAMOD_SHIFT,
		               MAC_PFAD_CFG_SAMOD_SIZE,
		               parm->bPauseMAC_ModeSrc);
		if (s != GSW_statusOk)
			return s;
		r = parm->nPauseMAC_Src[4] << 8
		    | parm->nPauseMAC_Src[5];
		s = gsw_reg_wr(pdev,
		               MAC_PFSA_0_PFAD_OFFSET,
		               MAC_PFSA_0_PFAD_SHIFT,
		               MAC_PFSA_0_PFAD_SIZE, r);
		if (s != GSW_statusOk)
			return s;
		r = parm->nPauseMAC_Src[2] << 8
		    | parm->nPauseMAC_Src[3];
		s = gsw_reg_wr(pdev,
		               MAC_PFSA_1_PFAD_OFFSET,
		               MAC_PFSA_1_PFAD_SHIFT,
		               MAC_PFSA_1_PFAD_SIZE, r);
		if (s != GSW_statusOk)
			return s;
		r = parm->nPauseMAC_Src[0] << 8
		    | parm->nPauseMAC_Src[1];
		s = gsw_reg_wr(pdev,
		               MAC_PFSA_2_PFAD_OFFSET,
		               MAC_PFSA_2_PFAD_SHIFT,
		               MAC_PFSA_2_PFAD_SIZE, r);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_CfgGet(void *pdev,
                        GSW_cfg_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/* Aging Counter Mantissa Value */
	s = gsw_reg_rd(pdev,
	               PCE_AGE_1_MANT_OFFSET,
	               PCE_AGE_1_MANT_SHIFT,
	               PCE_AGE_1_MANT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	switch (r) {
	case AGETIMER_1_DAY:
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_1_DAY;
		break;
	case AGETIMER_1_HOUR:
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_1_HOUR;
		break;
	case AGETIMER_300_SEC:
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_300_SEC;
		break;
	case AGETIMER_10_SEC:
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_10_SEC;
		break;
	case AGETIMER_1_SEC:
		parm->eMAC_TableAgeTimer = GSW_AGETIMER_1_SEC;
		break;
	default:
		parm->eMAC_TableAgeTimer = 0;
		break;
	}
	s = gsw_reg_rd(pdev,
	               MAC_FLEN_LEN_OFFSET,
	               MAC_FLEN_LEN_SHIFT,
	               MAC_FLEN_LEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nMaxPacketLen = r;
	s = gsw_reg_rd(pdev,
	               MAC_PFAD_CFG_SAMOD_OFFSET,
	               MAC_PFAD_CFG_SAMOD_SHIFT,
	               MAC_PFAD_CFG_SAMOD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bPauseMAC_ModeSrc = r;
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_VLAN_OFFSET,
	               PCE_GCTRL_0_VLAN_SHIFT,
	               PCE_GCTRL_0_VLAN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bVLAN_Aware = r;
	/* MAC Address Learning Limitation Mode */
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_PLIMMOD_OFFSET,
	               PCE_GCTRL_0_PLIMMOD_SHIFT,
	               PCE_GCTRL_0_PLIMMOD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bLearningLimitAction = r;
	s = gsw_reg_rd(pdev,
	               PCE_GCTRL_0_PLCKMOD_OFFSET,
	               PCE_GCTRL_0_PLCKMOD_SHIFT,
	               PCE_GCTRL_0_PLCKMOD_SIZE,
	               &parm->bMAC_SpoofingAction);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               MAC_PFSA_0_PFAD_OFFSET,
	               MAC_PFSA_0_PFAD_SHIFT,
	               MAC_PFSA_0_PFAD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nPauseMAC_Src[5] = r & 0xFF;
	parm->nPauseMAC_Src[4] = (r >> 8) & 0xFF;
	s = gsw_reg_rd(pdev,
	               MAC_PFSA_1_PFAD_OFFSET,
	               MAC_PFSA_1_PFAD_SHIFT,
	               MAC_PFSA_1_PFAD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nPauseMAC_Src[3] = r & 0xFF;
	parm->nPauseMAC_Src[2] = (r >> 8) & 0xFF;
	s = gsw_reg_rd(pdev,
	               MAC_PFSA_2_PFAD_OFFSET,
	               MAC_PFSA_2_PFAD_SHIFT,
	               MAC_PFSA_2_PFAD_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nPauseMAC_Src[1] = r & 0xFF;
	parm->nPauseMAC_Src[0] = (r >> 8) & 0xFF;
	return GSW_statusOk;
}

GSW_return_t GSW_Enable(void *pdev)
{
	ur r, i;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r;  i++) {
		s = gsw_reg_wr(pdev,
		               (FDMA_PCTRL_EN_OFFSET + (i * 0x6)),
		               FDMA_PCTRL_EN_SHIFT,
		               FDMA_PCTRL_EN_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (SDMA_PCTRL_PEN_OFFSET + (i * 0x6)),
		               SDMA_PCTRL_PEN_SHIFT,
		               SDMA_PCTRL_PEN_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (BM_PCFG_CNTEN_OFFSET + (i * 2)),
		               BM_PCFG_CNTEN_SHIFT,
		               BM_PCFG_CNTEN_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_Disable(void *pdev)
{
	ur r, i;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < r; i++) {
		s = gsw_reg_wr(pdev,
		               (FDMA_PCTRL_EN_OFFSET + (i * 0x6)),
		               FDMA_PCTRL_EN_SHIFT,
		               FDMA_PCTRL_EN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (SDMA_PCTRL_PEN_OFFSET + (i * 0x6)),
		               SDMA_PCTRL_PEN_SHIFT,
		               SDMA_PCTRL_PEN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_PortCfgGet(void *pdev,
                            GSW_portCfg_t *parm)
{
	u8 pi = parm->nPortId;
	GSW_return_t s;
	ur r, sn, fn;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/* See if PORT enable or not */
	s = gsw_reg_rd(pdev,
	               (SDMA_PCTRL_PEN_OFFSET + (0x6 * pi)),
	               SDMA_PCTRL_PEN_SHIFT,
	               SDMA_PCTRL_PEN_SIZE, &sn);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (FDMA_PCTRL_EN_OFFSET + (0x6 * pi)),
	               FDMA_PCTRL_EN_SHIFT,
	               FDMA_PCTRL_EN_SIZE, &fn);
	if (s != GSW_statusOk)
		return s;
	/* Port Enable feature only support 6 port */
	if (pi >= num_ports) {
		/* ToDo*/
		parm->eEnable = 1;
	} else {
		if ((sn == 1) && (fn == 1))
			parm->eEnable = GSW_PORT_ENABLE_RXTX;
		else if ((sn == 1) && (fn == 0))
			parm->eEnable = GSW_PORT_ENABLE_RX;
		else if ((sn == 0) && (fn == 1))
			parm->eEnable = GSW_PORT_ENABLE_TX;
		else
			parm->eEnable = GSW_PORT_DISABLE;
	}
	/* Learning Limit */
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pi)),
	               PCE_PCTRL_1_LRNLIM_SHIFT,
	               PCE_PCTRL_1_LRNLIM_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nLearningLimit = r;

	/* Learning Limit Port Lock */
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_PLOCK_OFFSET + (0xA * pi)),
	               PCE_PCTRL_0_PLOCK_SHIFT,
	               PCE_PCTRL_0_PLOCK_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bLearningMAC_PortLock = r;
	/* Aging */
	s = gsw_reg_rd(pdev,
	               PCE_PCTRL_0_AGEDIS_OFFSET + (0xA * pi),
	               PCE_PCTRL_0_AGEDIS_SHIFT,
	               PCE_PCTRL_0_AGEDIS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bAging = r;

	/** MAC address table learning on the port specified. */
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_LNDIS_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_LNDIS_SHIFT,
	               PCE_PCTRL_3_LNDIS_SIZE,
	               &parm->bLearning);
	if (s != GSW_statusOk)
		return s;
	/** MAC spoofing detection. */
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_SPFDIS_OFFSET + (0xA * pi)),
	               PCE_PCTRL_0_SPFDIS_SHIFT,
	               PCE_PCTRL_0_SPFDIS_SIZE,
	               &parm->bMAC_SpoofingDetection);
	if (s != GSW_statusOk)
		return s;
	/* UnicastUnknownDrop */
	s = gsw_reg_rd(pdev,
	               PCE_PMAP_3_UUCMAP_OFFSET,
	               PCE_PMAP_3_UUCMAP_SHIFT,
	               PCE_PMAP_3_UUCMAP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	/* UnicastUnknownDrop feature  support  */
	if ((r & (1 << pi)) == 0)
		parm->bUnicastUnknownDrop = 1;
	else
		parm->bUnicastUnknownDrop = 0;
	/* MulticastUnknownDrop */
	s = gsw_reg_rd(pdev,
	               PCE_PMAP_2_DMCPMAP_OFFSET,
	               PCE_PMAP_2_DMCPMAP_SHIFT,
	               PCE_PMAP_2_DMCPMAP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	/* MulticastUnknownDrop feature  support  */
	if ((r & (1 << pi)) == 0) {
		parm->bMulticastUnknownDrop = 1;
		parm->bBroadcastDrop = 1;
	} else {
		parm->bMulticastUnknownDrop = 0;
		parm->bBroadcastDrop = 0;
	}
	/* Require to check later - 3M */
	parm->bReservedPacketDrop = 0;
	/* Port Monitor */
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_TXMIR_SHIFT,
	               PCE_PCTRL_3_RX_TXMIR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->ePortMonitor = r;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_VIO_2_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_VIO_2_SHIFT,
	               PCE_PCTRL_3_VIO_2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_VLAN_UNKNOWN;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_VIO_4_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_VIO_4_SHIFT,
	               PCE_PCTRL_3_VIO_4_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_VLAN_MEMBERSHIP;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_VIO_5_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_VIO_5_SHIFT,
	               PCE_PCTRL_3_VIO_5_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_PORT_STATE;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_VIO_6_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_VIO_6_SHIFT,
	               PCE_PCTRL_3_VIO_6_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_LEARNING_LIMIT;
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_3_VIO_7_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_VIO_7_SHIFT,
	               PCE_PCTRL_3_VIO_7_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r == 1)
		parm->ePortMonitor |= GSW_PORT_MONITOR_PORT_LOCK;
	s = gsw_reg_rd(pdev,
	               (MAC_PSTAT_TXPAUEN_OFFSET + (0xC * pi)),
	               MAC_PSTAT_TXPAUEN_SHIFT,
	               MAC_PSTAT_TXRXPAUEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->eFlowCtrl = r;
	return GSW_statusOk;
}

GSW_return_t GSW_PortCfgSet(void *pdev,
                            GSW_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ur r, fn, sn, pact;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/* Learning Limit Port Lock */
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_0_PLOCK_OFFSET + (0xA * pi)),
	               PCE_PCTRL_0_PLOCK_SHIFT,
	               PCE_PCTRL_0_PLOCK_SIZE,
	               parm->bLearningMAC_PortLock);
	if (s != GSW_statusOk)
		return s;
	/* Learning Limit Action */
	if (parm->nLearningLimit == 0)
		r = 0;
	else if (parm->nLearningLimit == 0xFFFF)
		r = 0xFF;
	else
		r = parm->nLearningLimit;
	gpc[pi].llimit = r;
	/* Learning Limit */
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pi)),
	               PCE_PCTRL_1_LRNLIM_SHIFT,
	               PCE_PCTRL_1_LRNLIM_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/** MAC address table learning on the port specified */
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_3_LNDIS_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_LNDIS_SHIFT,
	               PCE_PCTRL_3_LNDIS_SIZE,
	               parm->bLearning);
	if (s != GSW_statusOk)
		return s;
	/** MAC spoofing detection. */
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_0_SPFDIS_OFFSET + (0xA * pi)),
	               PCE_PCTRL_0_SPFDIS_SHIFT,
	               PCE_PCTRL_0_SPFDIS_SIZE,
	               parm->bMAC_SpoofingDetection);
	if (s != GSW_statusOk)
		return s;
	/* Aging */
	s = gsw_reg_wr(pdev,
	               PCE_PCTRL_0_AGEDIS_OFFSET + (0xA * pi),
	               PCE_PCTRL_0_AGEDIS_SHIFT,
	               PCE_PCTRL_0_AGEDIS_SIZE,
	               parm->bAging);
	if (s != GSW_statusOk)
		return s;
	/* UnicastUnknownDrop Read first */
	s = gsw_reg_rd(pdev,
	               PCE_PMAP_3_UUCMAP_OFFSET,
	               PCE_PMAP_3_UUCMAP_SHIFT,
	               PCE_PMAP_3_UUCMAP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (parm->bUnicastUnknownDrop == 1)
		r &= ~(1 << pi);
	else
		r |= 1 << pi;
	/* UnicastUnknownDrop write back */
	s = gsw_reg_wr(pdev,
	               PCE_PMAP_3_UUCMAP_OFFSET,
	               PCE_PMAP_3_UUCMAP_SHIFT,
	               PCE_PMAP_3_UUCMAP_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/* MulticastUnknownDrop */
	s = gsw_reg_rd(pdev,
	               PCE_PMAP_2_DMCPMAP_OFFSET,
	               PCE_PMAP_2_DMCPMAP_SHIFT,
	               PCE_PMAP_2_DMCPMAP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (parm->bMulticastUnknownDrop == 1)
		r &= ~(1 << pi);
	else
		r |= 1 << pi;
	/* MulticastUnknownDrop */
	s = gsw_reg_wr(pdev,
	               PCE_PMAP_2_DMCPMAP_OFFSET,
	               PCE_PMAP_2_DMCPMAP_SHIFT,
	               PCE_PMAP_2_DMCPMAP_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	/* Flow Control */
	if (pi < num_ports) {
		s = gsw_reg_rd(pdev,
		               MMDC_CFG_0_PEN_0_OFFSET,
		               (MMDC_CFG_0_PEN_0_SHIFT + pi),
		               MMDC_CFG_0_PEN_0_SIZE,
		               &sn);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_rd(pdev,
		               (MMDIO_STAT_0_PACT_OFFSET + pi),
		               MMDIO_STAT_0_PACT_SHIFT,
		               MMDIO_STAT_0_PACT_SIZE,
		               &pact);
		if (s != GSW_statusOk)
			return s;
		if ((sn == 1) && (pact == 1)) {
			GSW_MDIO_data_t md;
			s = gsw_reg_rd(pdev,
			               (PHY_ADDR_0_ADDR_OFFSET - pi),
			               PHY_ADDR_0_ADDR_SHIFT,
			               PHY_ADDR_0_ADDR_SIZE,
			               &r);
			if (s != GSW_statusOk)
				return s;
			md.nAddressDev = r;
			md.nAddressReg = 0x4;
			s = GSW_MDIO_DataRead(pdev, &md);
			if (s != GSW_statusOk)
				return s;
			md.nData &= ~(0xC00);
			switch (parm->eFlowCtrl) {
			case GSW_FLOW_OFF:
				break;
			case GSW_FLOW_TX:
				md.nData |= 0x800;
				break;
			case GSW_FLOW_RXTX:
				md.nData |= 0x400;
				break;
			case GSW_FLOW_RX:
			case GSW_FLOW_AUTO:
				md.nData |= 0xC00;
				break;
			}
			s = GSW_MDIO_DataWrite(pdev, &md);
			if (s != GSW_statusOk)
				return s;
			/* Restart Auto negotiation */
			md.nAddressReg = 0x0;
			s = GSW_MDIO_DataRead(pdev, &md);
			if (s != GSW_statusOk)
				return s;
			md.nData |= 0x1200;
			s = GSW_MDIO_DataWrite(pdev, &md);
			if (s != GSW_statusOk)
				return s;
		} else {
			u32 r, t;
			switch (parm->eFlowCtrl) {
			case GSW_FLOW_AUTO:
			case GSW_FLOW_OFF:
				r = 0;
				t = 0;
				break;
			case GSW_FLOW_RXTX:
				r = 1;
				t = 1;
				break;
			case GSW_FLOW_RX:
				r = 1;
				t = 0;
				break;
			case GSW_FLOW_TX:
				r = 0;
				t = 1;
				break;
			default:
				r = 0;
				t = 0;
			}
			s = gsw_reg_wr(pdev,
			               (MAC_CTRL_0_FCON_OFFSET + (0xC * pi)),
			               MAC_CTRL_0_FCON_SHIFT,
			               MAC_CTRL_0_FCON_SIZE,
			               parm->eFlowCtrl);
			if (s != GSW_statusOk)
				return s;
			s = gsw_reg_wr(pdev,
			               (PHY_ADDR_0_FCONTX_OFFSET - (0x1 * pi)),
			               PHY_ADDR_0_FCONTX_SHIFT,
			               PHY_ADDR_0_FCONTX_SIZE,
			               t);
			if (s != GSW_statusOk)
				return s;
			s = gsw_reg_wr(pdev,
			               (PHY_ADDR_0_FCONRX_OFFSET - (0x1 * pi)),
			               PHY_ADDR_0_FCONRX_SHIFT,
			               PHY_ADDR_0_FCONRX_SIZE,
			               r);
			if (s != GSW_statusOk)
				return s;
		}
	}

	switch (parm->ePortMonitor) {
	case GSW_PORT_MONITOR_NONE:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_RXVMIR_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_RXVMIR_SHIFT,
		               PCE_PCTRL_3_RXVMIR_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_TXMIR_SHIFT,
		               PCE_PCTRL_3_TXMIR_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_2_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_2_SHIFT,
		               PCE_PCTRL_3_VIO_2_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_4_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_4_SHIFT,
		               PCE_PCTRL_3_VIO_4_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_5_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_5_SHIFT,
		               PCE_PCTRL_3_VIO_5_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_6_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_6_SHIFT,
		               PCE_PCTRL_3_VIO_6_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_7_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_7_SHIFT,
		               PCE_PCTRL_3_VIO_7_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_RX:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_TXMIR_SHIFT,
		               PCE_PCTRL_3_RX_TXMIR_SIZE, 2);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_TX:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_TXMIR_SHIFT,
		               PCE_PCTRL_3_RX_TXMIR_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_RXTX:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_TXMIR_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_TXMIR_SHIFT,
		               PCE_PCTRL_3_RX_TXMIR_SIZE, 3);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_VLAN_UNKNOWN:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_2_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_2_SHIFT,
		               PCE_PCTRL_3_VIO_2_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_VLAN_MEMBERSHIP:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_4_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_4_SHIFT,
		               PCE_PCTRL_3_VIO_4_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_PORT_STATE:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_5_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_5_SHIFT,
		               PCE_PCTRL_3_VIO_5_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_LEARNING_LIMIT:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_6_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_6_SHIFT,
		               PCE_PCTRL_3_VIO_6_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_PORT_MONITOR_PORT_LOCK:
		s = gsw_reg_wr(pdev,
		               (PCE_PCTRL_3_VIO_7_OFFSET + (0xA * pi)),
		               PCE_PCTRL_3_VIO_7_SHIFT,
		               PCE_PCTRL_3_VIO_7_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	}
	if (parm->eEnable == GSW_PORT_ENABLE_RXTX) {
		sn = 1;
		fn = 1;
	} else if (parm->eEnable == GSW_PORT_ENABLE_RX) {
		sn = 1;
		fn = 0;
	} else if (parm->eEnable == GSW_PORT_ENABLE_TX) {
		sn = 0;
		fn = 1;
	} else {
		sn = 0;
		fn = 0;
	}
	/* Set SDMA_PCTRL_PEN PORT enable */
	s = gsw_reg_wr(pdev,
	               (SDMA_PCTRL_PEN_OFFSET + (6 * pi)),
	               SDMA_PCTRL_PEN_SHIFT,
	               SDMA_PCTRL_PEN_SIZE, sn);
	if (s != GSW_statusOk)
		return s;
	/* Set FDMA_PCTRL_EN PORT enable  */
	s = gsw_reg_wr(pdev,
	               (FDMA_PCTRL_EN_OFFSET + (0x6 * pi)),
	               FDMA_PCTRL_EN_SHIFT,
	               FDMA_PCTRL_EN_SIZE, fn);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_CPU_PortCfgSet(void *pdev,
                                GSW_CPU_PortCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	mpnum = pi;
	/* Special Tag Egress*/
	s = gsw_reg_wr(pdev,
	               (FDMA_PCTRL_STEN_OFFSET + (0x6 * pi)),
	               FDMA_PCTRL_STEN_SHIFT,
	               FDMA_PCTRL_STEN_SIZE,
	               parm->bSpecialTagEgress);
	if (s != GSW_statusOk)
		return s;
	/* Special Tag Igress*/
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_0_IGSTEN_OFFSET + (0xa * pi)),
	               PCE_PCTRL_0_IGSTEN_SHIFT,
	               PCE_PCTRL_0_IGSTEN_SIZE,
	               parm->bSpecialTagIngress);
	if (s != GSW_statusOk)
		return s;
	/* FCS Generate */
	s = gsw_reg_wr(pdev,
	               (MAC_CTRL_0_FCS_OFFSET + (0xC * pi)),
	               MAC_CTRL_0_FCS_SHIFT,
	               MAC_CTRL_0_FCS_SIZE,
	               parm->bFcsGenerate);
	if (s != GSW_statusOk)
		return s;
	/* FCS Check */
	s = gsw_reg_wr(pdev,
	               (SDMA_PCTRL_FCSIGN_OFFSET + (0x6 * pi)),
	               SDMA_PCTRL_FCSIGN_SHIFT,
	               SDMA_PCTRL_FCSIGN_SIZE,
	               parm->bFcsCheck);
	if (s != GSW_statusOk)
		return s;
	if (parm->bSpecialTagEthType == GSW_CPU_ETHTYPE_FLOWID) {
		s = gsw_reg_wr(pdev,
		               (FDMA_PCTRL_ST_TYPE_OFFSET + (0x6 * pi)),
		               FDMA_PCTRL_ST_TYPE_SHIFT,
		               FDMA_PCTRL_ST_TYPE_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
	} else {
		s = gsw_reg_wr(pdev,
		               (FDMA_PCTRL_ST_TYPE_OFFSET + (0x6 * pi)),
		               FDMA_PCTRL_ST_TYPE_SHIFT,
		               FDMA_PCTRL_ST_TYPE_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_CPU_PortCfgGet(void *pdev,
                                GSW_CPU_PortCfg_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/* Special Tag Egress*/
	s = gsw_reg_rd(pdev,
	               (FDMA_PCTRL_STEN_OFFSET + (0x6 * pi)),
	               FDMA_PCTRL_STEN_SHIFT,
	               FDMA_PCTRL_STEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bSpecialTagEgress = r;
	/* Special Tag Igress*/
	s = gsw_reg_rd(pdev,
	               (PCE_PCTRL_0_IGSTEN_OFFSET + (0xa * pi)),
	               PCE_PCTRL_0_IGSTEN_SHIFT,
	               PCE_PCTRL_0_IGSTEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bSpecialTagIngress = r;
	/* FCS Check */
	s = gsw_reg_rd(pdev,
	               (SDMA_PCTRL_FCSIGN_OFFSET + (0x6 * pi)),
	               SDMA_PCTRL_FCSIGN_SHIFT,
	               SDMA_PCTRL_FCSIGN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bFcsCheck = r;
	/* FCS Generate */
	s = gsw_reg_rd(pdev,
	               (MAC_CTRL_0_FCS_OFFSET + (0xC * pi)),
	               MAC_CTRL_0_FCS_SHIFT,
	               MAC_CTRL_0_FCS_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bFcsGenerate = r;
	s = gsw_reg_rd(pdev,
	               (FDMA_PCTRL_ST_TYPE_OFFSET + (0x6 * pi)),
	               FDMA_PCTRL_ST_TYPE_SHIFT,
	               FDMA_PCTRL_ST_TYPE_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bSpecialTagEthType = r;
	return GSW_statusOk;
}

GSW_return_t GSW_CPU_PortExtendCfgSet(void *pdev,
                                      GSW_CPU_PortExtendCfg_t *parm)
{
	return GSW_statusNoSupport;
}

GSW_return_t GSW_CPU_PortExtendCfgGet(void *pdev,
                                      GSW_CPU_PortExtendCfg_t *parm)
{
	return GSW_statusNoSupport;
}

GSW_return_t GSW_PortLinkCfgSet(void *pdev,
                                GSW_portLinkCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	GSW_MDIO_data_t md;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (MAC_PSTAT_PACT_OFFSET + (0xC * pi)),
	               MAC_PSTAT_PACT_SHIFT,
	               MAC_PSTAT_PACT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r) {
		ur pa, fd, pc;
		s = gsw_reg_rd(pdev,
		               (PHY_ADDR_0_ADDR_OFFSET - pi),
		               PHY_ADDR_0_ADDR_SHIFT,
		               PHY_ADDR_0_ADDR_SIZE, &pa);
		if (s != GSW_statusOk)
			return s;
		if (parm->bDuplexForce == 1) {
			if (parm->eDuplex == GSW_DUPLEX_FULL)
				fd = PHY_DUPLEX_FULL;
			else if (parm->eDuplex == GSW_DUPLEX_HALF)
				fd = PHY_DUPLEX_HALF;
			else
				fd = PHY_DUPLEX_AUTO;
		} else {
			fd = PHY_DUPLEX_AUTO;
		}
		if (parm->bSpeedForce == 1) {
			switch (parm->eSpeed) {
			case GSW_PORT_SPEED_10:
				if (fd == PHY_DUPLEX_HALF)
					pc = PHY_AN_ADV_10HDX;
				else if (fd == PHY_DUPLEX_FULL)
					pc = PHY_AN_ADV_10FDX;
				else
					pc = (PHY_AN_ADV_10FDX
					      | PHY_AN_ADV_10HDX);
				md.nAddressDev = pa;
				md.nAddressReg = 4;
				s = GSW_MDIO_DataRead(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nData &= ~(PHY_AN_ADV_10HDX
				              | PHY_AN_ADV_10FDX
				              | PHY_AN_ADV_100HDX | PHY_AN_ADV_100FDX);
				md.nData |= pc;
				md.nAddressDev = pa;
				md.nAddressReg = 4;
				s = GSW_MDIO_DataWrite(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nAddressReg = 9;
				md.nAddressDev = pa;
				s = GSW_MDIO_DataRead(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nAddressReg = 9;
				md.nAddressDev = pa;
				md.nData &= ~(PHY_AN_ADV_1000HDX
				              | PHY_AN_ADV_1000FDX);
				s = GSW_MDIO_DataWrite(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				break;

			case GSW_PORT_SPEED_100:
				if (fd == PHY_DUPLEX_HALF)
					pc = PHY_AN_ADV_100HDX;
				else if (fd == PHY_DUPLEX_FULL)
					pc = PHY_AN_ADV_100FDX;
				else
					pc = (PHY_AN_ADV_100FDX
					      | PHY_AN_ADV_100HDX);
				md.nAddressDev = pa;
				md.nAddressReg = 4;
				s = GSW_MDIO_DataRead(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nData &= ~(PHY_AN_ADV_10HDX
				              | PHY_AN_ADV_10FDX
				              | PHY_AN_ADV_100HDX
				              | PHY_AN_ADV_100FDX);
				md.nData |= pc;
				md.nAddressDev = pa;
				md.nAddressReg = 4;
				s = GSW_MDIO_DataWrite(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nAddressReg = 9;
				md.nAddressDev = pa;
				s = GSW_MDIO_DataRead(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nAddressReg = 9;
				md.nAddressDev = pa;
				md.nData &= ~(PHY_AN_ADV_1000HDX
				              | PHY_AN_ADV_1000FDX);
				s = GSW_MDIO_DataWrite(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				break;
			case GSW_PORT_SPEED_200:
			case GSW_PORT_SPEED_2500:
				/* ToDo*/
				return GSW_statusNoSupport;
			case GSW_PORT_SPEED_1000:
				if (fd == PHY_DUPLEX_HALF)
					pc = PHY_AN_ADV_1000HDX;
				else if (fd == PHY_DUPLEX_FULL)
					pc = PHY_AN_ADV_1000FDX;
				else
					pc = (PHY_AN_ADV_1000FDX
					      | PHY_AN_ADV_1000HDX);
				md.nAddressReg = 9;
				md.nAddressDev = pa;
				s = GSW_MDIO_DataRead(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nData &= ~(PHY_AN_ADV_1000HDX
				              | PHY_AN_ADV_1000FDX);
				md.nData |= pc;
				md.nAddressDev = pa;
				md.nAddressReg = 9;
				s = GSW_MDIO_DataWrite(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nAddressReg = 4;
				md.nAddressDev = pa;
				s = GSW_MDIO_DataRead(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				md.nData &= ~(PHY_AN_ADV_10HDX
				              | PHY_AN_ADV_10FDX
				              | PHY_AN_ADV_100HDX
				              | PHY_AN_ADV_100FDX);
				md.nAddressReg = 4;
				md.nAddressDev = pa;
				s = GSW_MDIO_DataWrite(pdev, &md);
				if (s != GSW_statusOk)
					return s;
				break;
			}
		} else {
			md.nAddressDev = pa;
			md.nAddressReg = 4;
			s = GSW_MDIO_DataRead(pdev, &md);
			if (s != GSW_statusOk)
				return s;
			md.nData &= ~(PHY_AN_ADV_10HDX
			              | PHY_AN_ADV_10FDX
			              | PHY_AN_ADV_100HDX
			              | PHY_AN_ADV_100FDX);
			if (fd == PHY_DUPLEX_HALF) {
				md.nData |= (PHY_AN_ADV_10HDX
				             | PHY_AN_ADV_100HDX);
			} else if (fd == PHY_DUPLEX_FULL) {
				md.nData |= (PHY_AN_ADV_10FDX
				             | PHY_AN_ADV_100FDX);
			} else {
				md.nData |= (PHY_AN_ADV_10HDX
				             | PHY_AN_ADV_10FDX
				             | PHY_AN_ADV_100HDX
				             | PHY_AN_ADV_100FDX);
			}
			md.nAddressDev = pa;
			md.nAddressReg = 4;
			s = GSW_MDIO_DataWrite(pdev, &md);
			if (s != GSW_statusOk)
				return s;
			md.nAddressDev = pa;
			md.nAddressReg = 9;
			s = GSW_MDIO_DataRead(pdev, &md);
			if (s != GSW_statusOk)
				return s;
			md.nData &= ~(PHY_AN_ADV_1000HDX
			              | PHY_AN_ADV_1000FDX);
			if (fd == PHY_DUPLEX_HALF) {
				md.nData |= (PHY_AN_ADV_1000HDX);
			} else if (fd == PHY_DUPLEX_FULL) {
				md.nData |= (PHY_AN_ADV_1000HDX
				             | PHY_AN_ADV_1000FDX);
			} else {
				md.nData |= (PHY_AN_ADV_1000FDX);
			}
			md.nAddressDev = pa;
			md.nAddressReg = 9;
			s = GSW_MDIO_DataWrite(pdev, &md);
			if (s != GSW_statusOk)
				return s;
		}
		md.nAddressDev = pa;
		md.nAddressReg = 0;
		s = GSW_MDIO_DataRead(pdev, &md);
		if (s != GSW_statusOk)
			return s;
		md.nData = 0x1200;
		if (parm->bLinkForce == 1) {
			if (parm->eLink == GSW_PORT_LINK_DOWN)
				md.nData = 0x800;
		}
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;
	} else {
		ur pr;
		s = gsw_reg_rd(pdev,
		               (PHY_ADDR_0_ADDR_OFFSET - pi),
		               PHY_ADDR_0_ADDR_SHIFT,
		               PHY_ADDR_0_REG_SIZE,
		               &pr);
		if (s != GSW_statusOk)
			return s;
		if (parm->bDuplexForce == 1) {
			pr &= ~(3 << 9);
			if (parm->eDuplex == GSW_DUPLEX_FULL)
				pr |= (1 << 9);
			else if (parm->eDuplex == GSW_DUPLEX_HALF)
				pr |= (3 << 9);
		}
		if (parm->bLinkForce == 1) {
			pr &= ~(3 << 13);
			if (parm->eLink == GSW_PORT_LINK_UP)
				pr |= (1 << 13);
			else if (parm->eLink == GSW_PORT_LINK_DOWN)
				pr |= (2 << 13);
		}
		if (parm->bSpeedForce == 1) {
			pr &= ~(3 << 11);
			switch (parm->eSpeed) {
			case GSW_PORT_SPEED_10:
				break;
			case GSW_PORT_SPEED_100:
				pr |= (1 << 11);
				break;
			case GSW_PORT_SPEED_200:
			case GSW_PORT_SPEED_2500:
				/*ToDo*/
				return GSW_statusNoSupport;
			case GSW_PORT_SPEED_1000:
				pr |= (2 << 11);
				break;
			default:
				break;
			}
		}
		s = gsw_reg_wr(pdev,
		               (PHY_ADDR_0_ADDR_OFFSET - pi),
		               PHY_ADDR_0_ADDR_SHIFT,
		               PHY_ADDR_0_REG_SIZE,
		               pr);
		if (s != GSW_statusOk)
			return s;
	}
	s = gsw_reg_wr(pdev,
	               (MAC_CTRL_4_LPIEN_OFFSET + (0xC * pi)),
	               MAC_CTRL_4_LPIEN_SHIFT,
	               MAC_CTRL_4_LPIEN_SIZE, parm->bLPI);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_PortLinkCfgGet(void *pdev,
                                GSW_portLinkCfg_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (MAC_PSTAT_FDUP_OFFSET + (0xC * pi)),
	               MAC_PSTAT_FDUP_SHIFT,
	               MAC_PSTAT_FDUP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r)
		parm->eDuplex = GSW_DUPLEX_FULL;
	else
		parm->eDuplex = GSW_DUPLEX_HALF;
	s = gsw_reg_rd(pdev,
	               (MAC_PSTAT_GBIT_OFFSET + (0xC * pi)),
	               MAC_PSTAT_GBIT_SHIFT,
	               MAC_PSTAT_GBIT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r) {
		parm->eSpeed = GSW_PORT_SPEED_1000;
	} else {
		s = gsw_reg_rd(pdev,
		               (MAC_PSTAT_MBIT_OFFSET + (0xC * pi)),
		               MAC_PSTAT_MBIT_SHIFT,
		               MAC_PSTAT_MBIT_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		if (r)
			parm->eSpeed = GSW_PORT_SPEED_100;
		else
			parm->eSpeed = GSW_PORT_SPEED_10;
	}
	/* Low-power Idle Mode  configuration*/
	s = gsw_reg_rd(pdev,
	               (MAC_CTRL_4_LPIEN_OFFSET + (0xC * pi)),
	               MAC_CTRL_4_LPIEN_SHIFT,
	               MAC_CTRL_4_LPIEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bLPI = r;
	s = gsw_reg_rd(pdev,
	               (MAC_PSTAT_LSTAT_OFFSET + (0xC * pi)),
	               MAC_PSTAT_LSTAT_SHIFT,
	               MAC_PSTAT_LSTAT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (r)
		parm->eLink = GSW_PORT_LINK_UP;
	else
		parm->eLink = GSW_PORT_LINK_DOWN;
	return GSW_statusOk;
}

GSW_return_t GSW_PortRGMII_ClkCfgSet(void *pdev,
                                     GSW_portRGMII_ClkCfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (parm->nPortId == 5) {
		s = gsw_reg_wr(pdev,
		               PCDU_5_RXDLY_OFFSET,
		               PCDU_5_RXDLY_SHIFT,
		               PCDU_5_RXDLY_SIZE,
		               parm->nDelayRx);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PCDU_5_TXDLY_OFFSET,
		               PCDU_5_TXDLY_SHIFT,
		               PCDU_5_TXDLY_SIZE,
		               parm->nDelayTx);
		if (s != GSW_statusOk)
			return s;
	} else if (parm->nPortId == 6) {
		s = gsw_reg_wr(pdev,
		               PCDU_6_RXDLY_OFFSET,
		               PCDU_6_RXDLY_SHIFT,
		               PCDU_6_RXDLY_SIZE,
		               parm->nDelayRx);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               PCDU_6_TXDLY_OFFSET,
		               PCDU_6_TXDLY_SHIFT,
		               PCDU_6_TXDLY_SIZE,
		               parm->nDelayTx);
		if (s != GSW_statusOk)
			return s;
	} else {
		return GSW_statusNoSupport;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_PortRGMII_ClkCfgGet(void *pdev,
                                     GSW_portRGMII_ClkCfg_t *parm)
{
	ur r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if (parm->nPortId == 5) {
		s = gsw_reg_rd(pdev,
		               PCDU_5_RXDLY_OFFSET,
		               PCDU_5_RXDLY_SHIFT,
		               PCDU_5_RXDLY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nDelayRx = r;
		s = gsw_reg_rd(pdev,
		               PCDU_5_TXDLY_OFFSET,
		               PCDU_5_TXDLY_SHIFT,
		               PCDU_5_TXDLY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nDelayTx = r;
	} else if (parm->nPortId == 6) {
		s = gsw_reg_rd(pdev,
		               PCDU_6_RXDLY_OFFSET,
		               PCDU_6_RXDLY_SHIFT,
		               PCDU_6_RXDLY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nDelayRx = r;
		s = gsw_reg_rd(pdev,
		               PCDU_6_TXDLY_OFFSET,
		               PCDU_6_TXDLY_SHIFT,
		               PCDU_6_TXDLY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
		parm->nDelayTx = r;
	} else {
		return GSW_statusNoSupport;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_PortPHY_Query(void *pdev,
                               GSW_portPHY_Query_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (MAC_PSTAT_PACT_OFFSET + (0xC * pi)),
	               MAC_PSTAT_PACT_SHIFT,
	               MAC_PSTAT_PACT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bPHY_Present = r;
	return GSW_statusOk;
}

GSW_return_t GSW_PortPHY_AddrGet(void *pdev,
                                 GSW_portPHY_Addr_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	switch (pi) {
	case 0:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_0_ADDR_OFFSET,
		               PHY_ADDR_0_ADDR_SHIFT,
		               PHY_ADDR_0_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	case 1:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_1_ADDR_OFFSET,
		               PHY_ADDR_1_ADDR_SHIFT,
		               PHY_ADDR_1_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	case 2:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_2_ADDR_OFFSET,
		               PHY_ADDR_2_ADDR_SHIFT,
		               PHY_ADDR_2_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	case 3:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_3_ADDR_OFFSET,
		               PHY_ADDR_3_ADDR_SHIFT,
		               PHY_ADDR_3_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	case 4:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_4_ADDR_OFFSET,
		               PHY_ADDR_4_ADDR_SHIFT,
		               PHY_ADDR_4_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	case 5:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_5_ADDR_OFFSET,
		               PHY_ADDR_5_ADDR_SHIFT,
		               PHY_ADDR_5_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	case 6:
		s = gsw_reg_rd(pdev,
		               PHY_ADDR_6_ADDR_OFFSET,
		               PHY_ADDR_6_ADDR_SHIFT,
		               PHY_ADDR_6_ADDR_SIZE,
		               &r);
		if (s != GSW_statusOk)
			return s;
		break;
	default:
		r = 0xFF;
		return GSW_statusNoSupport;
	}
	parm->nAddressDev = r;
	return GSW_statusOk;
}

GSW_return_t GSW_PortRedirectGet(void *pdev,
                                 GSW_portRedirectCfg_t *parm)
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
	               (PCE_PCTRL_3_EDIR_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_EDIR_SHIFT,
	               PCE_PCTRL_3_EDIR_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bRedirectEgress = r;
	if (prdflag > 0)
		parm->bRedirectIngress = 1;
	else
		parm->bRedirectIngress = 0;
	return GSW_statusOk;
}

GSW_return_t GSW_PortRedirectSet(void *pdev,
                                 GSW_portRedirectCfg_t *parm)
{
	GSW_return_t s;
	ur r = 0, pn;
	GSW_PCE_rule_t pr;
	u8 i, pi = parm->nPortId;
	u32 rdport;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &pn);
	if (s != GSW_statusOk)
		return s;
	r = (1 << mpnum);
	rdport = r;
	s = gsw_reg_wr(pdev,
	               PCE_PMAP_1_MPMAP_OFFSET,
	               PCE_PMAP_1_MPMAP_SHIFT,
	               PCE_PMAP_1_MPMAP_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = parm->bRedirectEgress;
	s = gsw_reg_wr(pdev,
	               (PCE_PCTRL_3_EDIR_OFFSET + (0xA * pi)),
	               PCE_PCTRL_3_EDIR_SHIFT,
	               PCE_PCTRL_3_EDIR_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	if (parm->bRedirectIngress == 1)
		prdflag |= (1 << pi);
	else
		prdflag &= ~(1 << pi);
	for (i = 0; i < pn; i++) {
		if (((prdflag >> i) & 0x1) == 1) {
			memset(&pr, 0, sizeof(GSW_PCE_rule_t));
			pr.pattern.nIndex = (PRD_PRULE_INDEX + i);
			pr.pattern.bEnable = 1;
			pr.pattern.bPortIdEnable = 1;
			pr.pattern.nPortId = i;
			if (parm->bRedirectIngress == 1)
				pr.action.ePortMapAction =
				    GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
			pr.action.nForwardPortMap = rdport;
			/* We prepare everything and write into PCE Table */
			s = pce_rule_write(pdev, &pd->ptft, &pr);
			if (s != GSW_statusOk)
				return s;
		}  else {
			s = pce_pattern_delete(pdev, &pd->ptft,
			                       PRD_PRULE_INDEX + i);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MonitorPortCfgGet(void *pdev,
                                   GSW_monitorPortCfg_t *parm)
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
	               PCE_PMAP_1_MPMAP_OFFSET,
	               PCE_PMAP_1_MPMAP_SHIFT,
	               PCE_PMAP_1_MPMAP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (((r & (1 << pi)) >> pi) == 1)
		parm->bMonitorPort = 1;
	else
		parm->bMonitorPort = 0;
	return GSW_statusOk;
}

GSW_return_t GSW_MonitorPortCfgSet(void *pdev,
                                   GSW_monitorPortCfg_t *parm)
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
	               PCE_PMAP_1_MPMAP_OFFSET,
	               PCE_PMAP_1_MPMAP_SHIFT,
	               PCE_PMAP_1_MPMAP_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	if (parm->bMonitorPort == 1)
		r |= (parm->bMonitorPort << pi);
	else
		r = (r & ~(1 << pi));
	s = gsw_reg_wr(pdev,
	               PCE_PMAP_1_MPMAP_OFFSET,
	               PCE_PMAP_1_MPMAP_SHIFT,
	               PCE_PMAP_1_MPMAP_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_RMON_Port_Get(void *pdev,
                               GSW_RMON_Port_cnt_t *parm)
{
	u8 i, pi = parm->nPortId;
	ur r0, r1, bc;
	GSW_return_t s;
	bm_tbl_prog_t pb;
	u32 data, rf = 0, ru = 0, rm = 0, tf = 0, tu = 0, tm = 0;
	u32 rgbcl = 0, rbbcl = 0, tgbcl = 0;
	unsigned long long int rgbch = 0, rbbch = 0, tgbch = 0;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               BM_PCFG_CNTEN_OFFSET + (pi * 2),
	               BM_PCFG_CNTEN_SHIFT,
	               BM_PCFG_CNTEN_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	memset(parm, 0, sizeof(GSW_RMON_Port_cnt_t));
	parm->nPortId = pi;
	s = gsw_reg_rd(pdev,
	               BM_RMON_CTRL_BCAST_CNT_OFFSET + (pi * 2),
	               BM_RMON_CTRL_BCAST_CNT_SHIFT,
	               BM_RMON_CTRL_BCAST_CNT_SIZE, &bc);
	if (s != GSW_statusOk)
		return s;
	for (i = 0; i < RMON_COUNTER_OFFSET; i++) {
		memset(&pb, 0, sizeof(bm_tbl_prog_t));
		pb.bmindex = i;
		pb.bmtable = pi;
		pb.bmopmode = BM_OPMODE_RD;
		s = bm_ram_tbl_rd(pdev, &pb);
		if (s != GSW_statusOk)
			return s;
		r0 = pb.bmval[0];
		r1 = pb.bmval[1];
		data = (r1 << 16 | r0);
		switch (i) {
		case 0x1F: /* Receive Frme Count */
			if (bc == 1)
				parm->nRxBroadcastPkts = data;
			else
				parm->nRxGoodPkts = data;
			rf = data;
			break;
		case 0x23:
			/* Receive Unicast Frame Count */
			parm->nRxUnicastPkts = data;
			ru = data;
			break;
		case 0x22:
			/* Receive Multicast Frame Count1 */
			parm->nRxMulticastPkts = data;
			rm = data;
			break;
		case 0x21:
			/* Receive CRC Errors Count */
			parm->nRxFCSErrorPkts = data;
			break;
		case 0x1D:
			/* Receive Undersize Good Count */
			parm->nRxUnderSizeGoodPkts = data;
			break;
		case 0x1B:
			/* Receive Oversize Good Count */
			parm->nRxOversizeGoodPkts = data;
			break;
		case 0x1E:
			/* Receive Undersize Bad Count */
			parm->nRxUnderSizeErrorPkts = data;
			break;
		case 0x20:
			/* Receive Pause Good Count */
			parm->nRxGoodPausePkts = data;
			break;
		case 0x1C:
			/* Receive Oversize Bad Count */
			parm->nRxOversizeErrorPkts = data;
			break;
		case 0x1A:
			/* Receive Alignment Errors Count */
			parm->nRxAlignErrorPkts = data;
			break;
		case 0x12:
			/* Receive Size 64 Frame Count1 */
			parm->nRx64BytePkts = data;
			break;
		case 0x13:
			/* Receive Size 65-127 Frame Count */
			parm->nRx127BytePkts = data;
			break;
		case 0x14:
			/* Receive Size 128-255 Frame Count */
			parm->nRx255BytePkts = data;
			break;
		case 0x15:
			/* Receive Size 256-511 Frame Count */
			parm->nRx511BytePkts = data;
			break;
		case 0x16:
			/* Receive Size 512-1023 Frame Count */
			parm->nRx1023BytePkts = data;
			break;
		case 0x17:
			/* Receive Size Greater 1023 Frame Count */
			parm->nRxMaxBytePkts = data;
			break;
		case 0x18:
			/* Receive Discard (Tail-Drop) Frame Count */
			parm->nRxDroppedPkts = data;
			break;
		case 0x19:
			/* Receive Drop (Filter) Frame Count */
			parm->nRxFilteredPkts = data;
			break;
		case 0x24:
			/* Receive Good Byte Count (Low) */
			rgbcl = data;
			break;
		case 0x25:
			/* Receive Good Byte Count (High) */
			rgbch = data;
			break;
		case 0x26:
			/* Receive Bad Byte Count (Low) */
			rbbcl = data;
			break;
		case 0x27:
			/* Receive Bad Byte Count (High) */
			rbbch = data;
			break;
		case 0x0C: /* Transmit Frame Count */
			if (bc == 1)
				parm->nTxBroadcastPkts = data;
			else
				parm->nTxGoodPkts = data;
			tf = data;
			break;
		case 0x06:
			/* Transmit Unicast Frame Count */
			parm->nTxUnicastPkts = data;
			tu = data;
			break;
		case 0x07:
			/* Transmit Multicast Frame Count1 */
			parm->nTxMulticastPkts = data;
			tm = data;
			break;
		case 0x00:
			/* Transmit Size 64 Frame Count */
			parm->nTx64BytePkts = data;
			break;
		case 0x01:
			/* Transmit Size 65-127 Frame Count */
			parm->nTx127BytePkts = data;
			break;
		case 0x02:
			/* Transmit Size 128-255 Frame Count */
			parm->nTx255BytePkts = data;
			break;
		case 0x03:
			/* Transmit Size 256-511 Frame Count */
			parm->nTx511BytePkts = data;
			break;
		case 0x04:
			/* Transmit Size 512-1023 Frame Count */
			parm->nTx1023BytePkts = data;
			break;
		case 0x05:
			/* Transmit Size Greater 1024 Frame Count */
			parm->nTxMaxBytePkts = data;
			break;
		case 0x08:
			/* Transmit Single Collision Count. */
			parm->nTxSingleCollCount = data;
			break;
		case 0x09:
			/* Transmit Multiple Collision Count */
			parm->nTxMultCollCount = data;
			break;
		case 0x0A:
			/* Transmit Late Collision Count */
			parm->nTxLateCollCount = data;
			break;
		case 0x0B:
			/* Transmit Excessive Collision.*/
			parm->nTxExcessCollCount = data;
			break;
		case 0x0D:
			/* Transmit Pause Frame Count */
			parm->nTxPauseCount = data;
			break;
		case 0x10:
			/* Transmit Drop Frame Count */
			parm->nTxDroppedPkts = data;
			break;
		case 0x0E:
			/* Transmit Good Byte Count (Low) */
			tgbcl = data;
			break;
		case 0x0F:
			/* Transmit Good Byte Count (High) */
			tgbch = data;
			break;
		case 0x11:
			/* Tx Dropped Pkt cnt, based on Congestion Management.*/
			parm->nTxAcmDroppedPkts = data;
			break;
		}
	}
	if (bc == 1) {
		parm->nRxGoodPkts = rf + ru + rm;
		parm->nTxGoodPkts = tf + tu + tm;
	} else {
		/* Receive Broadcase Frme Count */
		parm->nRxBroadcastPkts = rf - ru - rm;
		/* Transmit Broadcase Frme Count */
		parm->nTxBroadcastPkts = tf - tu - tm;
	}
	/* Receive Good Byte Count */
	parm->nRxGoodBytes = (u64)(((rgbch & 0xFFFFFFFF) << 32)
	                           | (rgbcl & 0xFFFFFFFF));
	/* Receive Bad Byte Count */
	parm->nRxBadBytes = (u64)(((rbbch & 0xFFFFFFFF) << 32)
	                          | (rbbcl & 0xFFFFFFFF));
	/* Transmit Good Byte Count */
	parm->nTxGoodBytes = (u64)(((tgbch & 0xFFFFFFFF) << 32)
	                           | (tgbcl & 0xFFFFFFFF));
	return GSW_statusOk;
}

GSW_return_t GSW_RMON_Clear(void *pdev,
                            GSW_RMON_clear_t *parm)
{
	ur pn;
	GSW_return_t s;
	u8 i, pi = parm->nRmonId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &pn);
	if (s != GSW_statusOk)
		return s;
	switch (parm->eRmonType) {
	case GSW_RMON_ALL_TYPE:
		/* Reset all port based RMON counter */
		for (i = 0; i < pn; i++) {
			s = gsw_reg_wr(pdev,
			               BM_RMON_CTRL_RAM1_RES_OFFSET + (i * 2),
			               BM_RMON_CTRL_RAM1_RES_SHIFT,
			               BM_RMON_CTRL_RAM1_RES_SIZE, 0x1);
			if (s != GSW_statusOk)
				return s;
			s = gsw_reg_wr(pdev,
			               BM_RMON_CTRL_RAM2_RES_OFFSET + (i * 2),
			               BM_RMON_CTRL_RAM2_RES_SHIFT,
			               BM_RMON_CTRL_RAM2_RES_SIZE, 0x1);
			if (s != GSW_statusOk)
				return s;
		}
		break;
	case GSW_RMON_PORT_TYPE:
		s = npport(pdev, pi);
		if (s != GSW_statusOk)
			return s;
		/* Reset all RMON counter */
		s = gsw_reg_wr(pdev,
		               BM_RMON_CTRL_RAM1_RES_OFFSET + (pi * 2),
		               BM_RMON_CTRL_RAM1_RES_SHIFT,
		               BM_RMON_CTRL_RAM1_RES_SIZE, 0x1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               BM_RMON_CTRL_RAM2_RES_OFFSET + (pi * 2),
		               BM_RMON_CTRL_RAM2_RES_SHIFT,
		               BM_RMON_CTRL_RAM2_RES_SIZE, 0x1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_RMON_METER_TYPE:
	case GSW_RMON_IF_TYPE:
	case GSW_RMON_PMAC_TYPE:
	case GSW_RMON_ROUTE_TYPE:
		break;
	default:
		break;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MDIO_CfgSet(void *pdev,
                             GSW_MDIO_cfg_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               MMDC_CFG_1_FREQ_OFFSET,
	               MMDC_CFG_1_FREQ_SHIFT,
	               MMDC_CFG_1_FREQ_SIZE,
	               parm->nMDIO_Speed);
	if (s != GSW_statusOk)
		return s;
	if (parm->bMDIO_Enable)
		r = 0x7F;
	else
		r = 0;
	/* Set Auto-Polling For all ports */
	s = gsw_reg_wr(pdev,
	               MMDC_CFG_0_PEN_ALL_OFFSET,
	               MMDC_CFG_0_PEN_ALL_SHIFT,
	               MMDC_CFG_0_PEN_ALL_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               MMDC_CFG_1_MCEN_OFFSET,
	               MMDC_CFG_1_MCEN_SHIFT,
	               MMDC_CFG_1_MCEN_SIZE,
	               parm->bMDIO_Enable);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_MDIO_CfgGet(void *pdev,
                             GSW_MDIO_cfg_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               MMDC_CFG_1_FREQ_OFFSET,
	               MMDC_CFG_1_FREQ_SHIFT,
	               MMDC_CFG_1_FREQ_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nMDIO_Speed = (r & 0xFF);
	s = gsw_reg_rd(pdev,
	               MMDC_CFG_1_MCEN_OFFSET,
	               MMDC_CFG_1_MCEN_SHIFT,
	               MMDC_CFG_1_MCEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bMDIO_Enable = r;
	if (r == 1)
		parm->bMDIO_Enable = 1;
	else
		parm->bMDIO_Enable = 0;
	return GSW_statusOk;
}

GSW_return_t GSW_MDIO_DataRead(void *pdev,
                               GSW_MDIO_data_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	do {
		s = gsw_reg_rd(pdev,
		               MMDIO_CTRL_MBUSY_OFFSET,
		               MMDIO_CTRL_MBUSY_SHIFT,
		               MMDIO_CTRL_MBUSY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	r = ((0x2 << 10) | ((parm->nAddressDev & 0x1F) << 5)
	     | (parm->nAddressReg & 0x1F));
	s = gsw_reg_wr(pdev,
	               MMDIO_CTRL_MBUSY_OFFSET,
	               MMDIO_CTRL_REGAD_SHIFT,
	               MMDIO_CTRL_REG_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	do {
		s = gsw_reg_rd(pdev,
		               MMDIO_CTRL_MBUSY_OFFSET,
		               MMDIO_CTRL_MBUSY_SHIFT,
		               MMDIO_CTRL_MBUSY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	s = gsw_reg_rd(pdev,
	               MMDIO_READ_RDATA_OFFSET,
	               MMDIO_READ_RDATA_SHIFT,
	               MMDIO_READ_RDATA_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nData = (r & 0xFFFF);
	return GSW_statusOk;
}

GSW_return_t GSW_MDIO_DataWrite(void *pdev,
                                GSW_MDIO_data_t *parm)
{
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	do {
		s = gsw_reg_rd(pdev,
		               MMDIO_CTRL_MBUSY_OFFSET,
		               MMDIO_CTRL_MBUSY_SHIFT,
		               MMDIO_CTRL_MBUSY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	r = parm->nData & 0xFFFF;
	s = gsw_reg_wr(pdev,
	               MMDIO_WRITE_WDATA_OFFSET,
	               MMDIO_WRITE_WDATA_SHIFT,
	               MMDIO_WRITE_WDATA_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	r = ((0x1 << 10) | ((parm->nAddressDev & 0x1F) << 5)
	     | (parm->nAddressReg & 0x1F));
	s = gsw_reg_wr(pdev,
	               MMDIO_CTRL_MBUSY_OFFSET,
	               MMDIO_CTRL_REGAD_SHIFT,
	               MMDIO_CTRL_REG_SIZE, r);
	if (s != GSW_statusOk)
		return s;
	do {
		s = gsw_reg_rd(pdev,
		               MMDIO_CTRL_MBUSY_OFFSET,
		               MMDIO_CTRL_MBUSY_SHIFT,
		               MMDIO_CTRL_MBUSY_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	return GSW_statusOk;
}

GSW_return_t GSW_MmdDataRead(void *pdev,
                             GSW_MMD_data_t *parm)
{
	GSW_return_t s;
	GSW_MDIO_data_t md;
	u8 found = 0, dev, pi;
	ur pn, pa, mr;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &pn);
	if (s != GSW_statusOk)
		return s;
	for (pi = 0; pi < (pn - 1); pi++) {
		s = gsw_reg_rd(pdev,
		               (PHY_ADDR_0_ADDR_OFFSET - pi),
		               PHY_ADDR_0_ADDR_SHIFT,
		               PHY_ADDR_0_ADDR_SIZE, &pa);
		if (s != GSW_statusOk)
			return s;
		if (pa == parm->nAddressDev) {
			found = 1;
			break;
		}
	}
	if (found) {
		s = gsw_reg_rd(pdev,
		               MMDC_CFG_0_PEN_ALL_OFFSET,
		               MMDC_CFG_0_PEN_ALL_SHIFT,
		               MMDC_CFG_0_PEN_ALL_SIZE, &mr);
		if (s != GSW_statusOk)
			return s;
		mr &= ~(1 << pi);
		dev = ((parm->nAddressReg >> 16) & 0x1F);
		s = gsw_reg_wr(pdev,
		               MMDC_CFG_0_PEN_ALL_OFFSET,
		               MMDC_CFG_0_PEN_ALL_SHIFT,
		               MMDC_CFG_0_PEN_ALL_SIZE, mr);
		if (s != GSW_statusOk)
			return s;
		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xd;
		md.nData = dev;
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;

		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xe;
		md.nData = parm->nAddressReg & 0xFFFF;
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;

		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xd;
		md.nData = ((0x4000) | dev);
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;

		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xe;
		s = GSW_MDIO_DataRead(pdev, &md);
		if (s != GSW_statusOk)
			return s;
		parm->nData = md.nData;

		mr |= (1 << pi);
		s = gsw_reg_wr(pdev,
		               MMDC_CFG_0_PEN_ALL_OFFSET,
		               MMDC_CFG_0_PEN_ALL_SHIFT,
		               MMDC_CFG_0_PEN_ALL_SIZE, mr);
		if (s != GSW_statusOk)
			return s;
	} else {
		return GSW_statusNoSupport;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_MmdDataWrite(void *pdev,
                              GSW_MMD_data_t *parm)
{
	GSW_return_t s;
	GSW_MDIO_data_t md;
	u32 found = 0, pn, pa, mr, dev, pi;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               ETHSW_CAP_1_PPORTS_OFFSET,
	               ETHSW_CAP_1_PPORTS_SHIFT,
	               ETHSW_CAP_1_PPORTS_SIZE, &pn);
	if (s != GSW_statusOk)
		return s;
	for (pi = 0; pi < pn; pi++) {
		s = gsw_reg_rd(pdev,
		               (PHY_ADDR_0_ADDR_OFFSET - pi),
		               PHY_ADDR_0_ADDR_SHIFT,
		               PHY_ADDR_0_ADDR_SIZE, &pa);
		if (s != GSW_statusOk)
			return s;
		if (pa == parm->nAddressDev) {
			found = 1;
			break;
		}
	}
	if (found) {
		s = gsw_reg_rd(pdev,
		               MMDC_CFG_0_PEN_ALL_OFFSET,
		               MMDC_CFG_0_PEN_ALL_SHIFT,
		               MMDC_CFG_0_PEN_ALL_SIZE, &mr);
		if (s != GSW_statusOk)
			return s;
		mr &= ~(1 << pi);
		dev = ((parm->nAddressReg >> 16) & 0x1F);
		s = gsw_reg_wr(pdev,
		               MMDC_CFG_0_PEN_ALL_OFFSET,
		               MMDC_CFG_0_PEN_ALL_SHIFT,
		               MMDC_CFG_0_PEN_ALL_SIZE, mr);
		if (s != GSW_statusOk)
			return s;
		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xd;
		md.nData = dev;
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;

		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xe;
		md.nData = parm->nAddressReg & 0xFFFF;
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;

		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xd;
		md.nData = ((0x4000) | dev);
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;

		md.nAddressDev = parm->nAddressDev;
		md.nAddressReg = 0xe;
		md.nData = parm->nData;
		s = GSW_MDIO_DataWrite(pdev, &md);
		if (s != GSW_statusOk)
			return s;
		mr |= (1 << pi);
		s = gsw_reg_wr(pdev,
		               MMDC_CFG_0_PEN_ALL_OFFSET,
		               MMDC_CFG_0_PEN_ALL_SHIFT,
		               MMDC_CFG_0_PEN_ALL_SIZE, mr);
		if (s != GSW_statusOk)
			return s;
	} else {
		return GSW_statusNoSupport;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_TrunkingCfgGet(void *pdev,
                                GSW_trunkingCfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/* Destination IP Mask */
	s = gsw_reg_rd(pdev,
	               PCE_TRUNK_CONF_DIP_OFFSET,
	               PCE_TRUNK_CONF_DIP_SHIFT,
	               PCE_TRUNK_CONF_DIP_SIZE,
	               &parm->bIP_Dst);
	if (s != GSW_statusOk)
		return s;
	/* 'Source IP Mask */
	s = gsw_reg_rd(pdev,
	               PCE_TRUNK_CONF_SIP_OFFSET,
	               PCE_TRUNK_CONF_SIP_SHIFT,
	               PCE_TRUNK_CONF_SIP_SIZE,
	               &parm->bIP_Src);
	if (s != GSW_statusOk)
		return s;
	/* Destination MAC Mask */
	s = gsw_reg_rd(pdev,
	               PCE_TRUNK_CONF_DA_OFFSET,
	               PCE_TRUNK_CONF_DA_SHIFT,
	               PCE_TRUNK_CONF_DA_SIZE,
	               &parm->bMAC_Dst);
	if (s != GSW_statusOk)
		return s;
	/* 'Source MAC Mask */
	s = gsw_reg_rd(pdev,
	               PCE_TRUNK_CONF_SA_OFFSET,
	               PCE_TRUNK_CONF_SA_SHIFT,
	               PCE_TRUNK_CONF_SA_SIZE,
	               &parm->bMAC_Src);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_TrunkingCfgSet(void *pdev,
                                GSW_trunkingCfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/* Destination IP Mask */
	s = gsw_reg_wr(pdev,
	               PCE_TRUNK_CONF_DIP_OFFSET,
	               PCE_TRUNK_CONF_DIP_SHIFT,
	               PCE_TRUNK_CONF_DIP_SIZE,
	               parm->bIP_Dst);
	if (s != GSW_statusOk)
		return s;
	/* 'Source IP Mask */
	s = gsw_reg_wr(pdev,
	               PCE_TRUNK_CONF_SIP_OFFSET,
	               PCE_TRUNK_CONF_SIP_SHIFT,
	               PCE_TRUNK_CONF_SIP_SIZE,
	               parm->bIP_Src);
	if (s != GSW_statusOk)
		return s;
	/* Destination MAC Mask */
	s = gsw_reg_wr(pdev,
	               PCE_TRUNK_CONF_DA_OFFSET,
	               PCE_TRUNK_CONF_DA_SHIFT,
	               PCE_TRUNK_CONF_DA_SIZE,
	               parm->bMAC_Dst);
	if (s != GSW_statusOk)
		return s;
	/* 'Source MAC Mask */
	s = gsw_reg_wr(pdev,
	               PCE_TRUNK_CONF_SA_OFFSET,
	               PCE_TRUNK_CONF_SA_SHIFT,
	               PCE_TRUNK_CONF_SA_SIZE,
	               parm->bMAC_Src);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_TrunkingPortCfgGet(void *pdev,
                                    GSW_trunkingPortCfg_t *parm)
{
	u8 pi = parm->nPortId;
	GSW_return_t s;
	ur r;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/** Ports are aggregated. the 'nPortId' and the */
	/* 'nAggrPortId' ports form an aggregated link. */
	s = gsw_reg_rd(pdev,
	               (PCE_PTRUNK_EN_OFFSET + (pi * 0x2)),
	               PCE_PTRUNK_EN_SHIFT,
	               PCE_PTRUNK_EN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bAggregateEnable = r;
	s = gsw_reg_rd(pdev,
	               (PCE_PTRUNK_PARTER_OFFSET + (pi * 0x2)),
	               PCE_PTRUNK_PARTER_SHIFT,
	               PCE_PTRUNK_PARTER_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nAggrPortId = r;
	return GSW_statusOk;
}

GSW_return_t GSW_TrunkingPortCfgSet(void *pdev,
                                    GSW_trunkingPortCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/** Ports are aggregated. the 'nPortId' and the */
	/* 'nAggrPortId' ports form an aggregated link.*/
	if (parm->bAggregateEnable == 1) {
		s = gsw_reg_wr(pdev,
		               (PCE_PTRUNK_EN_OFFSET +	(pi * 0x2)),
		               PCE_PTRUNK_EN_SHIFT,
		               PCE_PTRUNK_EN_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PTRUNK_PARTER_OFFSET + (pi * 0x2)),
		               PCE_PTRUNK_PARTER_SHIFT,
		               PCE_PTRUNK_PARTER_SIZE,
		               (parm->nAggrPortId & 0xF));
		if (s != GSW_statusOk)
			return s;
	} else {
		s = gsw_reg_wr(pdev,
		               (PCE_PTRUNK_EN_OFFSET +	(pi * 0x2)),
		               PCE_PTRUNK_EN_SHIFT,
		               PCE_PTRUNK_EN_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
		s = gsw_reg_wr(pdev,
		               (PCE_PTRUNK_PARTER_OFFSET + (pi * 0x2)),
		               PCE_PTRUNK_PARTER_SHIFT,
		               PCE_PTRUNK_PARTER_SIZE, 0);
		if (s != GSW_statusOk)
			return s;
	}
	return GSW_statusOk;
}

GSW_return_t GSW_RegisterSet(void *pdev,
                             GSW_register_t *parm)
{
	GSW_return_t s;
	u32 rv = parm->nData, ra = parm->nRegAddr;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev, ra, GSW_REG_SHIFT,
	               GSW_REG_SIZE, rv);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_RegisterGet(void *pdev,
                             GSW_register_t *parm)
{
	GSW_return_t s;
	u32 rv, ra = parm->nRegAddr;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev, ra, GSW_REG_SHIFT,
	               GSW_REG_SIZE, &rv);
	if (s != GSW_statusOk)
		return s;
	parm->nData = rv;
	return GSW_statusOk;
}

GSW_return_t GSW_IrqMaskGet(void *pdev,
                            GSW_irq_t *parm)
{
	ur r;
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (PCE_PIER_WOL_OFFSET + (0xA * pi)),
	               PCE_PIER_REG_SHIFT,
	               PCE_PIER_REG_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->eIrqSrc = r;
	return GSW_statusOk;
}

GSW_return_t GSW_IrqMaskSet(void *pdev,
                            GSW_irq_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	if (parm->eIrqSrc == GSW_IRQ_WOL) {
		s = gsw_reg_wr(pdev,
		               (PCE_PIER_WOL_OFFSET + (0xA * pi)),
		               PCE_PIER_WOL_SHIFT,
		               PCE_PIER_WOL_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
	} else if (parm->eIrqSrc == GSW_IRQ_LIMIT_ALERT) {
		s = gsw_reg_wr(pdev,
		               (PCE_PIER_LOCK_OFFSET + (0xA * pi)),
		               PCE_PIER_LOCK_SHIFT,
		               PCE_PIER_LOCK_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
	} else if (parm->eIrqSrc == GSW_IRQ_LOCK_ALERT) {
		s = gsw_reg_wr(pdev,
		               (PCE_PIER_LIM_OFFSET + (0xA * pi)),
		               PCE_PIER_LIM_SHIFT,
		               PCE_PIER_LIM_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
	} else
		return GSW_statusNoSupport;
	return GSW_statusOk;
}

GSW_return_t GSW_IrqGet(void *pdev, GSW_irq_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/* ToDo: Require future clarify for how to display */
	return GSW_statusNoSupport;
}

GSW_return_t GSW_IrqStatusClear(void *pdev,
                                GSW_irq_t *parm)
{
	/* ToDo: Request future clarify */
	return GSW_statusNoSupport;
}

GSW_return_t GSW_PceRuleWrite(void *pdev,
                              GSW_PCE_rule_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = pce_rule_write(pdev, &pd->ptft, parm);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_PceRuleRead(void *pdev,
                             GSW_PCE_rule_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = pce_rule_read(pdev, &pd->ptft, parm);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_PceRuleDelete(void *pdev,
                               GSW_PCE_ruleDelete_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = pce_pattern_delete(pdev, &pd->ptft, parm->nIndex);
	if (s != GSW_statusOk)
		return s;
	s =  pce_action_delete(pd, &pd->ptft, parm->nIndex);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_Reset(void *pdev, GSW_reset_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	switch (parm->eReset) {
	case GSW_RESET_SWITCH:
		s = gsw_reg_wr(pdev,
		               ETHSW_SWRES_R0_OFFSET,
		               ETHSW_SWRES_R0_SHIFT,
		               ETHSW_SWRES_R0_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	case GSW_RESET_MDIO:
		s = gsw_reg_wr(pdev,
		               MMDC_CFG_1_RST_OFFSET,
		               MMDC_CFG_1_RST_SHIFT,
		               MMDC_CFG_1_RST_SIZE, 1);
		if (s != GSW_statusOk)
			return s;
		break;
	default:
		break;
	}
	s = GSW_Disable(pdev);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_RMON_ExtendGet(void *pdev,
                                GSW_RMON_extendGet_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId, i;
	u16 r0, r1;
	bm_tbl_prog_t pb;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	memset(parm, 0, sizeof(GSW_RMON_extendGet_t));
	for (i = 0; i < GSW_RMON_EXTEND_NUM; i++) {
		pb.bmindex = (i + REX_TFLOW_CNT_1);
		pb.bmtable = pi;
		pb.bmopmode = BM_OPMODE_RD;
		s = bm_ram_tbl_rd(pdev, &pb);
		if (s != GSW_statusOk)
			return s;
		r0 = pb.bmval[0];
		r1 = pb.bmval[1];
		parm->nTrafficFlowCnt[i] = (r1 << 16 | r0);
	}
	return GSW_statusOk;
}

GSW_return_t GSW_TimestampTimerSet(void *pdev,
                                   GSW_TIMESTAMP_Timer_t *parm)
{
	ur r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	/** Second. Absolute second timer count. */
	s = gsw_reg_wr(pdev,
	               (TIMER_SEC_LSB_SECLSB_OFFSET),
	               TIMER_SEC_LSB_SECLSB_SHIFT,
	               TIMER_SEC_LSB_SECLSB_SIZE,
	               (parm->nSec & 0xFFFF));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (TIMER_SEC_MSB_SECMSB_OFFSET),
	               TIMER_SEC_MSB_SECMSB_SHIFT,
	               TIMER_SEC_MSB_SECMSB_SIZE,
	               ((parm->nSec >> 16) & 0xFFFF));
	if (s != GSW_statusOk)
		return s;
	/** Nano Second. Absolute nano second timer count.*/
	s = gsw_reg_wr(pdev,
	               (TIMER_NS_LSB_NSLSB_OFFSET),
	               TIMER_NS_LSB_NSLSB_SHIFT,
	               TIMER_NS_LSB_NSLSB_SIZE,
	               (parm->nNanoSec & 0xFFFF));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (TIMER_NS_MSB_NSMSB_OFFSET),
	               TIMER_NS_MSB_NSMSB_SHIFT,
	               TIMER_NS_MSB_NSMSB_SIZE,
	               ((parm->nNanoSec >> 16) & 0xFFFF));
	if (s != GSW_statusOk)
		return s;
	/** Fractional Nano Second. Absolute fractional nano */
	/* second timer count. This counter specifis a */
	/* 2^32 fractional 'nNanoSec'. */
	s = gsw_reg_wr(pdev,
	               (TIMER_FS_LSB_FSLSB_OFFSET),
	               TIMER_FS_LSB_FSLSB_SHIFT,
	               TIMER_FS_LSB_FSLSB_SIZE,
	               (parm->nFractionalNanoSec & 0xFFFF));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (TIMER_FS_MSB_FSMSB_OFFSET),
	               TIMER_FS_MSB_FSMSB_SHIFT,
	               TIMER_FS_MSB_FSMSB_SIZE,
	               ((parm->nFractionalNanoSec >> 16) & 0xFFFF));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (TIMER_CTRL_WR_OFFSET),
	               TIMER_CTRL_WR_SHIFT,
	               TIMER_CTRL_WR_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	do {
		s = gsw_reg_rd(pdev,
		               (TIMER_CTRL_WR_OFFSET),
		               TIMER_CTRL_WR_SHIFT,
		               TIMER_CTRL_WR_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	return GSW_statusOk;
}

GSW_return_t GSW_TimestampTimerGet(void *pdev,
                                   GSW_TIMESTAMP_Timer_t *parm)
{
	ur r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               (TIMER_CTRL_RD_OFFSET),
	               TIMER_CTRL_RD_SHIFT,
	               TIMER_CTRL_RD_SIZE, 1);
	if (s != GSW_statusOk)
		return s;
	do {
		s = gsw_reg_rd(pdev,
		               (TIMER_CTRL_RD_OFFSET),
		               TIMER_CTRL_RD_SHIFT,
		               TIMER_CTRL_RD_SIZE, &r);
		if (s != GSW_statusOk)
			return s;
	} while (r);
	/** Second. Absolute second timer count. */
	s = gsw_reg_rd(pdev,
	               (TIMER_SEC_LSB_SECLSB_OFFSET),
	               TIMER_SEC_LSB_SECLSB_SHIFT,
	               TIMER_SEC_LSB_SECLSB_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nSec = (r & 0xFFFF);
	s = gsw_reg_rd(pdev,
	               (TIMER_SEC_MSB_SECMSB_OFFSET),
	               TIMER_SEC_MSB_SECMSB_SHIFT,
	               TIMER_SEC_MSB_SECMSB_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nSec |= ((r & 0xFFFF) << 16);
	/** Nano Second. Absolute nano second timer count. */
	s = gsw_reg_rd(pdev,
	               (TIMER_NS_LSB_NSLSB_OFFSET),
	               TIMER_NS_LSB_NSLSB_SHIFT,
	               TIMER_NS_LSB_NSLSB_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nNanoSec = (r & 0xFFFF);
	s = gsw_reg_rd(pdev,
	               (TIMER_NS_MSB_NSMSB_OFFSET),
	               TIMER_NS_MSB_NSMSB_SHIFT,
	               TIMER_NS_MSB_NSMSB_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nNanoSec |= ((r & 0xFFFF) << 16);
	/** Fractional Nano Second. Absolute fractional */
	/* nano second timer count. */
	/*	This counter specifis a 2^32 fractional 'nNanoSec'. */
	s = gsw_reg_rd(pdev,
	               (TIMER_FS_LSB_FSLSB_OFFSET),
	               TIMER_FS_LSB_FSLSB_SHIFT,
	               TIMER_FS_LSB_FSLSB_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nFractionalNanoSec = (r & 0xFFFF);
	s = gsw_reg_rd(pdev,
	               (TIMER_FS_MSB_FSMSB_OFFSET),
	               TIMER_FS_MSB_FSMSB_SHIFT,
	               TIMER_FS_MSB_FSMSB_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nFractionalNanoSec |= ((r & 0xFFFF) << 16);
	return GSW_statusOk;
}

GSW_return_t GSW_TimestampPortRead(void *pdev,
                                   GSW_TIMESTAMP_PortRead_t *parm)
{
	u8 pi = parm->nPortId;
	GSW_return_t s;
	ur ts0, ts1;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	/** Second. Absolute second timer count. */
	s = gsw_reg_rd(pdev,
	               (FDMA_TSTAMP0_TSTL_OFFSET + (pi * 0x6)),
	               FDMA_TSTAMP0_TSTL_SHIFT,
	               FDMA_TSTAMP0_TSTL_SIZE, &ts0);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (FDMA_TSTAMP1_TSTH_OFFSET + (pi * 0x6)),
	               FDMA_TSTAMP1_TSTH_SHIFT,
	               FDMA_TSTAMP1_TSTH_SIZE, &ts1);
	if (s != GSW_statusOk)
		return s;
	parm->nEgressSec = ((ts0 | (ts1 << 16))) >> 30;
	parm->nEgressNanoSec = (((ts0 | (ts1 << 16))) & 0x7FFFFFFF);
	/** Nano Second. Absolute nano second timer count. */
	s = gsw_reg_rd(pdev,
	               (SDMA_TSTAMP0_TSTL_OFFSET + (pi * 0x6)),
	               SDMA_TSTAMP0_TSTL_SHIFT,
	               SDMA_TSTAMP0_TSTL_SIZE, &ts0);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (SDMA_TSTAMP1_TSTH_OFFSET + (pi * 0x6)),
	               SDMA_TSTAMP1_TSTH_SHIFT,
	               SDMA_TSTAMP1_TSTH_SIZE, &ts1);
	if (s != GSW_statusOk)
		return s;
	parm->nIngressSec = ((ts0 | (ts1 << 16))) >> 30;
	parm->nIngressNanoSec = (((ts0 | (ts1 << 16))) & 0x7FFFFFFF);
	return GSW_statusOk;
}

#if defined(CONFIG_LTQ_WOL) && CONFIG_LTQ_WOL
GSW_return_t GSW_WoL_CfgGet(void *pdev, GSW_WoL_Cfg_t *parm)
{
	ur r;
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_rd(pdev,
	               WOL_GLB_CTRL_PASSEN_OFFSET,
	               WOL_GLB_CTRL_PASSEN_SHIFT,
	               WOL_GLB_CTRL_PASSEN_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bWolPasswordEnable = r;
	s = gsw_reg_rd(pdev,
	               WOL_DA_2_DA2_OFFSET,
	               WOL_DA_2_DA2_SHIFT,
	               WOL_DA_2_DA2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nWolMAC[0] = (r >> 8 & 0xFF);
	parm->nWolMAC[1] = (r & 0xFF);
	s = gsw_reg_rd(pdev,
	               WOL_DA_1_DA1_OFFSET,
	               WOL_DA_1_DA1_SHIFT,
	               WOL_DA_1_DA1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nWolMAC[2] = (r >> 8 & 0xFF);
	parm->nWolMAC[3] = (r & 0xFF);
	s = gsw_reg_rd(pdev,
	               WOL_DA_0_DA0_OFFSET,
	               WOL_DA_0_DA0_SHIFT,
	               WOL_DA_0_DA0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nWolMAC[4] = (r >> 8 & 0xFF);
	parm->nWolMAC[5] = (r & 0xFF);
	s = gsw_reg_rd(pdev,
	               WOL_PW_2_PW2_OFFSET,
	               WOL_PW_2_PW2_SHIFT,
	               WOL_PW_2_PW2_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nWolPassword[0] = (r >> 8 & 0xFF);
	parm->nWolPassword[1] = (r & 0xFF);
	s = gsw_reg_rd(pdev,
	               WOL_PW_1_PW1_OFFSET,
	               WOL_PW_1_PW1_SHIFT,
	               WOL_PW_1_PW1_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nWolPassword[2] = (r >> 8 & 0xFF);
	parm->nWolPassword[3] = (r & 0xFF);
	s = gsw_reg_rd(pdev,
	               WOL_PW_0_PW0_OFFSET,
	               WOL_PW_0_PW0_SHIFT,
	               WOL_PW_0_PW0_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->nWolPassword[4] = (r >> 8 & 0xFF);
	parm->nWolPassword[5] = (r & 0xFF);
	return GSW_statusOk;
}

GSW_return_t GSW_WoL_CfgSet(void *pdev, GSW_WoL_Cfg_t *parm)
{
	GSW_return_t s;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = gsw_reg_wr(pdev,
	               WOL_GLB_CTRL_PASSEN_OFFSET,
	               WOL_GLB_CTRL_PASSEN_SHIFT,
	               WOL_GLB_CTRL_PASSEN_SIZE,
	               parm->bWolPasswordEnable);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               WOL_DA_2_DA2_OFFSET,
	               WOL_DA_2_DA2_SHIFT,
	               WOL_DA_2_DA2_SIZE,
	               (((parm->nWolMAC[0] & 0xFF) << 8)
	                | (parm->nWolMAC[1] & 0xFF)));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               WOL_DA_1_DA1_OFFSET,
	               WOL_DA_1_DA1_SHIFT,
	               WOL_DA_1_DA1_SIZE,
	               (((parm->nWolMAC[2] & 0xFF) << 8)
	                | (parm->nWolMAC[3] & 0xFF)));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               WOL_DA_0_DA0_OFFSET,
	               WOL_DA_0_DA0_SHIFT,
	               WOL_DA_0_DA0_SIZE,
	               (((parm->nWolMAC[4] & 0xFF) << 8)
	                | (parm->nWolMAC[5] & 0xFF)));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               WOL_PW_2_PW2_OFFSET,
	               WOL_PW_2_PW2_SHIFT,
	               WOL_PW_2_PW2_SIZE,
	               (((parm->nWolPassword[0] & 0xFF) << 8)
	                | (parm->nWolPassword[1] & 0xFF)));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               WOL_PW_1_PW1_OFFSET,
	               WOL_PW_1_PW1_SHIFT,
	               WOL_PW_1_PW1_SIZE,
	               (((parm->nWolPassword[2] & 0xFF) << 8)
	                | (parm->nWolPassword[3] & 0xFF)));
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               WOL_PW_0_PW0_OFFSET,
	               WOL_PW_0_PW0_SHIFT,
	               WOL_PW_0_PW0_SIZE,
	               (((parm->nWolPassword[4] & 0xFF) << 8)
	                | (parm->nWolPassword[5] & 0xFF)));
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_WoL_PortCfgGet(void *pdev,
                                GSW_WoL_PortCfg_t *parm)
{
	GSW_return_t s;
	ur r;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_rd(pdev,
	               (WOL_CTRL_PORT_OFFSET + (0xA * pi)),
	               WOL_CTRL_PORT_SHIFT,
	               WOL_CTRL_PORT_SIZE, &r);
	if (s != GSW_statusOk)
		return s;
	parm->bWakeOnLAN_Enable = r;
	return GSW_statusOk;
}

GSW_return_t GSW_WoL_PortCfgSet(void *pdev,
                                GSW_WoL_PortCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	s = gsw_reg_wr(pdev,
	               (WOL_CTRL_PORT_OFFSET + (0xA * pi)),
	               WOL_CTRL_PORT_SHIFT,
	               WOL_CTRL_PORT_SIZE,
	               parm->bWakeOnLAN_Enable);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
#endif /* CONFIG_LTQ_WOL */


#if	((defined(CONFIG_LTQ_STP) && CONFIG_LTQ_STP) || (defined(CONFIG_LTQ_8021X) && CONFIG_LTQ_8021X))
static GSW_return_t set_port_state(void *pdev, u32 pid,
                                   u32 stpstate, u32 st8021)
{
	GSW_return_t s;
	pstpstate_t *pststate;
	u32 i;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	for (i = 0; i < sizeof(stps)/sizeof(pstpstate_t); i++) {
		pststate = &stps[i];
		if ((pststate->psstate == stpstate) &&
		        (pststate->ps8021x == st8021)) {
			gpc[pid].penable = pststate->pen_reg;
			gpc[pid].ptstate = pststate->pstate_reg;
			/* Learning Limit */
			if (pststate->lrnlim == 0) {
				s = gsw_reg_wr(pdev,
				               (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pid)),
				               PCE_PCTRL_1_LRNLIM_SHIFT,
				               PCE_PCTRL_1_LRNLIM_SIZE, 0);
				if (s != GSW_statusOk)
					return s;
			} else {
				s = gsw_reg_wr(pdev,
				               (PCE_PCTRL_1_LRNLIM_OFFSET + (0xA * pid)),
				               PCE_PCTRL_1_LRNLIM_SHIFT,
				               PCE_PCTRL_1_LRNLIM_SIZE,
				               gpc[pid].llimit);
				if (s != GSW_statusOk)
					return s;
			}
			/* Port State */
			s = gsw_reg_wr(pdev,
			               (PCE_PCTRL_0_PSTATE_OFFSET + (0xA * pid)),
			               PCE_PCTRL_0_PSTATE_SHIFT,
			               PCE_PCTRL_0_PSTATE_SIZE,
			               gpc[pid].ptstate);
			if (s != GSW_statusOk)
				return s;
			/* Port Enable */
			s = gsw_reg_wr(pdev,
			               (SDMA_PCTRL_PEN_OFFSET + (0xA * pid)),
			               SDMA_PCTRL_PEN_SHIFT,
			               SDMA_PCTRL_PEN_SIZE,
			               gpc[pid].penable);
			if (s != GSW_statusOk)
				return s;
		}
	}
	return GSW_statusOk;
}
#endif /* CONFIG_LTQ_STP / CONFIG_LTQ_8021X */


#if defined(CONFIG_LTQ_STP) && CONFIG_LTQ_STP
GSW_return_t GSW_STP_BPDU_RuleGet(void *pdev,
                                  GSW_STP_BPDU_Rule_t *parm)
{
	stp8021x_t *scfg = &gstpconfig;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	parm->eForwardPort = scfg->spstate;
	parm->nForwardPortId = scfg->stppid;
	return GSW_statusOk;
}

GSW_return_t GSW_STP_BPDU_RuleSet(void *pdev,
                                  GSW_STP_BPDU_Rule_t *parm)
{
	GSW_return_t s;
	GSW_PCE_rule_t pr;
	stp8021x_t *scfg = &gstpconfig;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	scfg->spstate = parm->eForwardPort;
	scfg->stppid = parm->nForwardPortId;
	memset(&pr, 0, sizeof(GSW_PCE_rule_t));
	/* Attached the PCE rule for BPDU packet */
	pr.pattern.nIndex	= BPDU_PCE_RULE_INDEX;
	pr.pattern.bEnable = 1;
	pr.pattern.bMAC_DstEnable	= 1;
	pr.pattern.nMAC_Dst[0] = 0x01;
	pr.pattern.nMAC_Dst[1] = 0x80;
	pr.pattern.nMAC_Dst[2] = 0xC2;
	pr.pattern.nMAC_Dst[3] = 0x00;
	pr.pattern.nMAC_Dst[4] = 0x00;
	pr.pattern.nMAC_Dst[5] = 0x00;
	pr.action.eCrossStateAction	= GSW_PCE_ACTION_CROSS_STATE_CROSS;
	switch(scfg->spstate) {
	case 0:
	case 1:
	case 2:
	case 3:
		pr.action.ePortMapAction = scfg->spstate + 1;
		break;
	default:
		return GSW_statusValueRange;
	}
	/*
		if ((scfg->spstate < 4) &&
			(scfg->spstate > 0))
			pr.action.ePortMapAction = scfg->spstate + 1;
		else
			return GSW_statusValueRange;
	*/
	pr.action.nForwardPortMap = (1 << scfg->stppid);
	/* We prepare everything and write into PCE Table */
	s = pce_rule_write(pdev, &pd->ptft, &pr);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_STP_PortCfgGet(void *pdev,
                                GSW_STP_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	parm->ePortState = gpc[pi].pcstate;
	return GSW_statusOk;
}

GSW_return_t GSW_STP_PortCfgSet(void *pdev,
                                GSW_STP_portCfg_t *parm)
{
	GSW_return_t s;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	gpc[pi].pcstate = parm->ePortState;
	/* Config the Table */
	set_port_state(pdev, pi, gpc[pi].pcstate,	gpc[pi].p8021xs);
	return GSW_statusOk;
}
#endif /* CONFIG_LTQ_STP */


#if defined(CONFIG_LTQ_8021X) && CONFIG_LTQ_8021X
GSW_return_t GSW_8021X_EAPOL_RuleGet(void *pdev,
                                     GSW_8021X_EAPOL_Rule_t *parm)
{
	GSW_return_t s;
	stp8021x_t *scfg = &gstpconfig;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, parm->nForwardPortId);
	if (s != GSW_statusOk)
		return s;
	parm->eForwardPort = scfg->sfport;
	parm->nForwardPortId = scfg->fpid8021x;
	return GSW_statusOk;
}

GSW_return_t GSW_8021X_EAPOL_RuleSet(void *pdev,
                                     GSW_8021X_EAPOL_Rule_t *parm)
{
	GSW_return_t s;
	stp8021x_t *scfg = &gstpconfig;
	GSW_PCE_rule_t pr;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, parm->nForwardPortId);
	if (s != GSW_statusOk)
		return s;
	scfg->sfport = parm->eForwardPort;
	scfg->fpid8021x = parm->nForwardPortId;
	memset(&pr, 0, sizeof(GSW_PCE_rule_t));
	pr.pattern.nIndex = EAPOL_PCE_RULE_INDEX;
	pr.pattern.bEnable = 1;
	pr.pattern.bMAC_DstEnable	= 1;
	pr.pattern.nMAC_Dst[0] = 0x01;
	pr.pattern.nMAC_Dst[1] = 0x80;
	pr.pattern.nMAC_Dst[2] = 0xC2;
	pr.pattern.nMAC_Dst[3] = 0x00;
	pr.pattern.nMAC_Dst[4] = 0x00;
	pr.pattern.nMAC_Dst[5] = 0x03;
	pr.pattern.nMAC_Src[5] = 0;
	pr.pattern.bEtherTypeEnable	= 1;
	pr.pattern.nEtherType	= 0x888E;
	pr.action.eCrossStateAction = GSW_PCE_ACTION_CROSS_STATE_CROSS;
	switch(scfg->sfport) {
	case 0:
	case 1:
	case 2:
	case 3:
		pr.action.ePortMapAction = scfg->sfport + 1;
		break;
	default:
		return GSW_statusNoSupport;
	}
	/*
		if ((scfg->sfport < 4) && (scfg->sfport > 0))
			pr.action.ePortMapAction = scfg->sfport + 1;
		else
			return GSW_statusNoSupport;
	*/
	pr.action.nForwardPortMap = (1 << parm->nForwardPortId);
	/* We prepare everything and write into PCE Table */
	if (0 != pce_rule_write(pdev, &pd->ptft, &pr))
		return s;
	return GSW_statusOk;
}

GSW_return_t GSW_8021X_PortCfgGet(void *pdev,
                                  GSW_8021X_portCfg_t *parm)
{
	GSW_return_t s;
	port_config_t *pcfg;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	pcfg = &gpc[pi];
	parm->eState = pcfg->p8021xs;
	return GSW_statusOk;
}

GSW_return_t GSW_8021X_PortCfgSet(void *pdev,
                                  GSW_8021X_portCfg_t *parm)
{
	GSW_return_t s;
	port_config_t *pcfg;
	u8 pi = parm->nPortId;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	s = npport(pdev, pi);
	if (s != GSW_statusOk)
		return s;
	pcfg = &gpc[pi];
	pcfg->p8021xs	= parm->eState;
	s = set_port_state(pdev, pi, pcfg->pcstate, pcfg->p8021xs);
	if (s != GSW_statusOk)
		return s;
	return GSW_statusOk;
}
#endif /*CONFIG_LTQ_8021X  */
