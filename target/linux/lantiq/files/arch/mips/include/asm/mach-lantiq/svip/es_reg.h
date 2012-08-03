/******************************************************************************

  Copyright (c) 2007
  Infineon Technologies AG
  St. Martin Strasse 53; 81669 Munich, Germany

  Any use of this Software is subject to the conclusion of a respective
  License Agreement. Without such a License Agreement no rights to the
  Software are granted.

 ******************************************************************************/

#ifndef __ES_REG_H
#define __ES_REG_H

#define es_r32(reg) ltq_r32(&es->reg)
#define es_w32(val, reg) ltq_w32(val, &es->reg)
#define es_w32_mask(clear, set, reg) ltq_w32_mask(clear, set, &es->reg)

/** ES register structure */
struct svip_reg_es {
	volatile unsigned long  ps;  /*  0x0000 */
	volatile unsigned long  p0_ctl;  /*  0x0004 */
	volatile unsigned long  p1_ctl;  /*  0x0008 */
	volatile unsigned long  p2_ctl;  /*  0x000C */
	volatile unsigned long  p0_vlan;  /*  0x0010 */
	volatile unsigned long  p1_vlan;  /*  0x0014 */
	volatile unsigned long  p2_vlan;  /*  0x0018 */
	volatile unsigned long  reserved1[1];  /*  0x001C */
	volatile unsigned long  p0_inctl;  /*  0x0020 */
	volatile unsigned long  p1_inctl;  /*  0x0024 */
	volatile unsigned long  p2_inctl;  /*  0x0028 */
	volatile unsigned long  reserved2[1];  /*  0x002C */
	volatile unsigned long  p0_ecs_q32;  /*  0x0030 */
	volatile unsigned long  p0_ecs_q10;  /*  0x0034 */
	volatile unsigned long  p0_ecw_q32;  /*  0x0038 */
	volatile unsigned long  p0_ecw_q10;  /*  0x003C */
	volatile unsigned long  p1_ecs_q32;  /*  0x0040 */
	volatile unsigned long  p1_ecs_q10;  /*  0x0044 */
	volatile unsigned long  p1_ecw_q32;  /*  0x0048 */
	volatile unsigned long  p1_ecw_q10;  /*  0x004C */
	volatile unsigned long  p2_ecs_q32;  /*  0x0050 */
	volatile unsigned long  p2_ecs_q10;  /*  0x0054 */
	volatile unsigned long  p2_ecw_q32;  /*  0x0058 */
	volatile unsigned long  p2_ecw_q10;  /*  0x005C */
	volatile unsigned long  int_ena;  /*  0x0060 */
	volatile unsigned long  int_st;  /*  0x0064 */
	volatile unsigned long  sw_gctl0;  /*  0x0068 */
	volatile unsigned long  sw_gctl1;  /*  0x006C */
	volatile unsigned long  arp;  /*  0x0070 */
	volatile unsigned long  strm_ctl;  /*  0x0074 */
	volatile unsigned long  rgmii_ctl;  /*  0x0078 */
	volatile unsigned long  prt_1p;  /*  0x007C */
	volatile unsigned long  gbkt_szbs;  /*  0x0080 */
	volatile unsigned long  gbkt_szebs;  /*  0x0084 */
	volatile unsigned long  bf_th;  /*  0x0088 */
	volatile unsigned long  pmac_hd_ctl;  /*  0x008C */
	volatile unsigned long  pmac_sa1;  /*  0x0090 */
	volatile unsigned long  pmac_sa2;  /*  0x0094 */
	volatile unsigned long  pmac_da1;  /*  0x0098 */
	volatile unsigned long  pmac_da2;  /*  0x009C */
	volatile unsigned long  pmac_vlan;  /*  0x00A0 */
	volatile unsigned long  pmac_tx_ipg;  /*  0x00A4 */
	volatile unsigned long  pmac_rx_ipg;  /*  0x00A8 */
	volatile unsigned long  adr_tb_ctl0;  /*  0x00AC */
	volatile unsigned long  adr_tb_ctl1;  /*  0x00B0 */
	volatile unsigned long  adr_tb_ctl2;  /*  0x00B4 */
	volatile unsigned long  adr_tb_st0;  /*  0x00B8 */
	volatile unsigned long  adr_tb_st1;  /*  0x00BC */
	volatile unsigned long  adr_tb_st2;  /*  0x00C0 */
	volatile unsigned long  rmon_ctl;  /*  0x00C4 */
	volatile unsigned long  rmon_st;  /*  0x00C8 */
	volatile unsigned long  mdio_ctl;  /*  0x00CC */
	volatile unsigned long  mdio_data;  /*  0x00D0 */
	volatile unsigned long  tp_flt_act;  /*  0x00D4 */
	volatile unsigned long  prtcl_flt_act;  /*  0x00D8 */
	volatile unsigned long  reserved4[9];  /*  0xdc */
	volatile unsigned long  vlan_flt0;  /*  0x0100 */
	volatile unsigned long  vlan_flt1;  /*  0x0104 */
	volatile unsigned long  vlan_flt2;  /*  0x0108 */
	volatile unsigned long  vlan_flt3;  /*  0x010C */
	volatile unsigned long  vlan_flt4;  /*  0x0110 */
	volatile unsigned long  vlan_flt5;  /*  0x0114 */
	volatile unsigned long  vlan_flt6;  /*  0x0118 */
	volatile unsigned long  vlan_flt7;  /*  0x011C */
	volatile unsigned long  vlan_flt8;  /*  0x0120 */
	volatile unsigned long  vlan_flt9;  /*  0x0124 */
	volatile unsigned long  vlan_flt10;  /*  0x0128 */
	volatile unsigned long  vlan_flt11;  /*  0x012C */
	volatile unsigned long  vlan_flt12;  /*  0x0130 */
	volatile unsigned long  vlan_flt13;  /*  0x0134 */
	volatile unsigned long  vlan_flt14;  /*  0x0138 */
	volatile unsigned long  vlan_flt15;  /*  0x013C */
	volatile unsigned long  tp_flt10;  /*  0x0140 */
	volatile unsigned long  tp_flt32;  /*  0x0144 */
	volatile unsigned long  tp_flt54;  /*  0x0148 */
	volatile unsigned long  tp_flt76;  /*  0x014C */
	volatile unsigned long  dfsrv_map0;  /*  0x0150 */
	volatile unsigned long  dfsrv_map1;  /*  0x0154 */
	volatile unsigned long  dfsrv_map2;  /*  0x0158 */
	volatile unsigned long  dfsrv_map3;  /*  0x015C */
	volatile unsigned long  tcp_pf0;  /*  0x0160 */
	volatile unsigned long  tcp_pf1;  /*  0x0164 */
	volatile unsigned long  tcp_pf2;  /*  0x0168 */
	volatile unsigned long  tcp_pf3;  /*  0x016C */
	volatile unsigned long  tcp_pf4;  /*  0x0170 */
	volatile unsigned long  tcp_pf5;  /*  0x0174 */
	volatile unsigned long  tcp_pf6;  /*  0x0178 */
	volatile unsigned long  tcp_pf7;  /*  0x017C */
	volatile unsigned long  ra_03_00;  /*  0x0180 */
	volatile unsigned long  ra_07_04;  /*  0x0184 */
	volatile unsigned long  ra_0b_08;  /*  0x0188 */
	volatile unsigned long  ra_0f_0c;  /*  0x018C */
	volatile unsigned long  ra_13_10;  /*  0x0190 */
	volatile unsigned long  ra_17_14;  /*  0x0194 */
	volatile unsigned long  ra_1b_18;  /*  0x0198 */
	volatile unsigned long  ra_1f_1c;  /*  0x019C */
	volatile unsigned long  ra_23_20;  /*  0x01A0 */
	volatile unsigned long  ra_27_24;  /*  0x01A4 */
	volatile unsigned long  ra_2b_28;  /*  0x01A8 */
	volatile unsigned long  ra_2f_2c;  /*  0x01AC */
	volatile unsigned long  prtcl_f0;  /*  0x01B0 */
	volatile unsigned long  prtcl_f1;  /*  0x01B4 */
};

/*******************************************************************************
 * ES
 ******************************************************************************/
#define LTQ_ES_PS_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0000))
#define LTQ_ES_P0_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0004))
#define LTQ_ES_P1_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0008))
#define LTQ_ES_P2_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x000C))
#define LTQ_ES_P0_VLAN_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0010))
#define LTQ_ES_P1_VLAN_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0014))
#define LTQ_ES_P2_VLAN_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0010))
#define LTQ_ES_P0_INCTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0020))
#define LTQ_ES_P1_INCTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0024))
#define LTQ_ES_P2_INCTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0028))
#define LTQ_ES_P0_ECS_Q32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0030))
#define LTQ_ES_P0_ECS_Q10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0034))
#define LTQ_ES_P0_ECW_Q32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0038))
#define LTQ_ES_P0_ECW_Q10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x003C))
#define LTQ_ES_P1_ECS_Q32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0030))
#define LTQ_ES_P1_ECS_Q10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0034))
#define LTQ_ES_P1_ECW_Q32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0038))
#define LTQ_ES_P1_ECW_Q10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x003C))
#define LTQ_ES_P2_ECS_Q32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0030))
#define LTQ_ES_P2_ECS_Q10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0034))
#define LTQ_ES_P2_ECW_Q32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0038))
#define LTQ_ES_P2_ECW_Q10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x003C))
#define LTQ_ES_INT_ENA_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0060))
#define LTQ_ES_INT_ST_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0064))
#define LTQ_ES_SW_GCTL0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0068))
#define LTQ_ES_SW_GCTL1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x006C))
#define LTQ_ES_ARP_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0070))
#define LTQ_ES_STRM_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0074))
#define LTQ_ES_RGMII_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0078))
#define LTQ_ES_PRT_1P_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x007C))
#define LTQ_ES_GBKT_SZBS_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0080))
#define LTQ_ES_GBKT_SZEBS_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0084))
#define LTQ_ES_BF_TH_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0088))
#define LTQ_ES_PMAC_HD_CTL   ((volatile unsigned int*)(LTQ_ES_BASE + 0x008C))
#define LTQ_ES_PMAC_SA1   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0090))
#define LTQ_ES_PMAC_SA2   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0094))
#define LTQ_ES_PMAC_DA1   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0098))
#define LTQ_ES_PMAC_DA2   ((volatile unsigned int*)(LTQ_ES_BASE + 0x009C))
#define LTQ_ES_PMAC_VLAN   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00A0))
#define LTQ_ES_PMAC_TX_IPG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00A4))
#define LTQ_ES_PMAC_RX_IPG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00A8))
#define LTQ_ES_ADR_TB_CTL0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00AC))
#define LTQ_ES_ADR_TB_CTL1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00B0))
#define LTQ_ES_ADR_TB_CTL2_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00B4))
#define LTQ_ES_ADR_TB_ST0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00B8))
#define LTQ_ES_ADR_TB_ST1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00BC))
#define LTQ_ES_ADR_TB_ST2_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00C0))
#define LTQ_ES_RMON_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00C4))
#define LTQ_ES_RMON_ST_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00C8))
#define LTQ_ES_MDIO_CTL_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00CC))
#define LTQ_ES_MDIO_DATA_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00D0))
#define LTQ_ES_TP_FLT_ACT_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00D4))
#define LTQ_ES_PRTCL_FLT_ACT_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x00D8))
#define LTQ_ES_VLAN_FLT0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0100))
#define LTQ_ES_VLAN_FLT1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0104))
#define LTQ_ES_VLAN_FLT2_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0108))
#define LTQ_ES_VLAN_FLT3_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x010C))
#define LTQ_ES_VLAN_FLT4_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0110))
#define LTQ_ES_VLAN_FLT5_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0114))
#define LTQ_ES_VLAN_FLT6_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0118))
#define LTQ_ES_VLAN_FLT7_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x011C))
#define LTQ_ES_VLAN_FLT8_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0120))
#define LTQ_ES_VLAN_FLT9_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0124))
#define LTQ_ES_VLAN_FLT10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0128))
#define LTQ_ES_VLAN_FLT11_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x012C))
#define LTQ_ES_VLAN_FLT12_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0130))
#define LTQ_ES_VLAN_FLT13_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0134))
#define LTQ_ES_VLAN_FLT14_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0138))
#define LTQ_ES_VLAN_FLT15_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x013C))
#define LTQ_ES_TP_FLT10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0140))
#define LTQ_ES_TP_FLT32_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0144))
#define LTQ_ES_TP_FLT54_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0148))
#define LTQ_ES_TP_FLT76_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x014C))
#define LTQ_ES_DFSRV_MAP0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0150))
#define LTQ_ES_DFSRV_MAP1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0154))
#define LTQ_ES_DFSRV_MAP2_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0158))
#define LTQ_ES_DFSRV_MAP3_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x015C))
#define LTQ_ES_TCP_PF0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0160))
#define LTQ_ES_TCP_PF1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0164))
#define LTQ_ES_TCP_PF2_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0168))
#define LTQ_ES_TCP_PF3_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x016C))
#define LTQ_ES_TCP_PF4_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0170))
#define LTQ_ES_TCP_PF5_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0174))
#define LTQ_ES_TCP_PF6_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0178))
#define LTQ_ES_TCP_PF7_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x017C))
#define LTQ_ES_RA_03_00_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0180))
#define LTQ_ES_RA_07_04_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0184))
#define LTQ_ES_RA_0B_08_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0188))
#define LTQ_ES_RA_0F_0C_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x018C))
#define LTQ_ES_RA_13_10_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0190))
#define LTQ_ES_RA_17_14_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0194))
#define LTQ_ES_RA_1B_18_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x0198))
#define LTQ_ES_RA_1F_1C_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x019C))
#define LTQ_ES_RA_23_20_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x01A0))
#define LTQ_ES_RA_27_24_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x01A4))
#define LTQ_ES_RA_2B_28_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x01A8))
#define LTQ_ES_RA_2F_2C_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x01AC))
#define LTQ_ES_PRTCL_F0_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x01B0))
#define LTQ_ES_PRTCL_F1_REG   ((volatile unsigned int*)(LTQ_ES_BASE + 0x01B4))

/*******************************************************************************
 * Port Status Register
 ******************************************************************************/

/* Port 1 Flow Control Status (12) */
#define LTQ_ES_PS_REG_P1FCS   (0x1 << 12)
#define LTQ_ES_PS_REG_P1FCS_GET(val)   ((((val) & LTQ_ES_PS_REG_P1FCS) >> 12) & 0x1)
/* Port 1 Duplex Status (11) */
#define LTQ_ES_PS_REG_P1DS   (0x1 << 11)
#define LTQ_ES_PS_REG_P1DS_GET(val)   ((((val) & LTQ_ES_PS_REG_P1DS) >> 11) & 0x1)
/* Port 1 Speed High Status (10) */
#define LTQ_ES_PS_REG_P1SHS   (0x1 << 10)
#define LTQ_ES_PS_REG_P1SHS_GET(val)   ((((val) & LTQ_ES_PS_REG_P1SHS) >> 10) & 0x1)
/* Port 1 Speed Status (9) */
#define LTQ_ES_PS_REG_P1SS   (0x1 << 9)
#define LTQ_ES_PS_REG_P1SS_GET(val)   ((((val) & LTQ_ES_PS_REG_P1SS) >> 9) & 0x1)
/* Port 1 Link Status (8) */
#define LTQ_ES_PS_REG_P1LS   (0x1 << 8)
#define LTQ_ES_PS_REG_P1LS_GET(val)   ((((val) & LTQ_ES_PS_REG_P1LS) >> 8) & 0x1)
/* Port 0 Flow Control Status (4) */
#define LTQ_ES_PS_REG_P0FCS   (0x1 << 4)
#define LTQ_ES_PS_REG_P0FCS_GET(val)   ((((val) & LTQ_ES_PS_REG_P0FCS) >> 4) & 0x1)
/* Port 0 Duplex Status (3) */
#define LTQ_ES_PS_REG_P0DS   (0x1 << 3)
#define LTQ_ES_PS_REG_P0DS_GET(val)   ((((val) & LTQ_ES_PS_REG_P0DS) >> 3) & 0x1)
/* Port 0 Speed High Status (2) */
#define LTQ_ES_PS_REG_P0SHS   (0x1 << 2)
#define LTQ_ES_PS_REG_P0SHS_GET(val)   ((((val) & LTQ_ES_PS_REG_P0SHS) >> 2) & 0x1)
/* Port 0 Speed Status (1) */
#define LTQ_ES_PS_REG_P0SS   (0x1 << 1)
#define LTQ_ES_PS_REG_P0SS_GET(val)   ((((val) & LTQ_ES_PS_REG_P0SS) >> 1) & 0x1)
/* Port 0 Link Status (0) */
#define LTQ_ES_PS_REG_P0LS   (0x1)
#define LTQ_ES_PS_REG_P0LS_GET(val)   ((((val) & LTQ_ES_PS_REG_P0LS) >> 0) & 0x1)

/*******************************************************************************
 * P0 Control Register
 ******************************************************************************/

/* STP/RSTP port state (31:30) */
#define LTQ_ES_P0_CTL_REG_SPS   (0x3 << 30)
#define LTQ_ES_P0_CTL_REG_SPS_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_P0_CTL_REG_SPS_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_SPS) >> 30) & 0x3)
#define LTQ_ES_P0_CTL_REG_SPS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_SPS) | (((val) & 0x3) << 30))
/* TCP/UDP PRIEN (29) */
#define LTQ_ES_P0_CTL_REG_TCPE   (0x1 << 29)
#define LTQ_ES_P0_CTL_REG_TCPE_VAL(val)   (((val) & 0x1) << 29)
#define LTQ_ES_P0_CTL_REG_TCPE_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_TCPE) >> 29) & 0x1)
#define LTQ_ES_P0_CTL_REG_TCPE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_TCPE) | (((val) & 0x1) << 29))
/*  IP over TCP/UDP (28) */
#define LTQ_ES_P0_CTL_REG_IPOVTU   (0x1 << 28)
#define LTQ_ES_P0_CTL_REG_IPOVTU_VAL(val)   (((val) & 0x1) << 28)
#define LTQ_ES_P0_CTL_REG_IPOVTU_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_IPOVTU) >> 28) & 0x1)
#define LTQ_ES_P0_CTL_REG_IPOVTU_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_IPOVTU) | (((val) & 0x1) << 28))
/* VLAN Priority Enable (27) */
#define LTQ_ES_P0_CTL_REG_VPE   (0x1 << 27)
#define LTQ_ES_P0_CTL_REG_VPE_VAL(val)   (((val) & 0x1) << 27)
#define LTQ_ES_P0_CTL_REG_VPE_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_VPE) >> 27) & 0x1)
#define LTQ_ES_P0_CTL_REG_VPE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_VPE) | (((val) & 0x1) << 27))
/* Service Priority Enable (26) */
#define LTQ_ES_P0_CTL_REG_SPE   (0x1 << 26)
#define LTQ_ES_P0_CTL_REG_SPE_VAL(val)   (((val) & 0x1) << 26)
#define LTQ_ES_P0_CTL_REG_SPE_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_SPE) >> 26) & 0x1)
#define LTQ_ES_P0_CTL_REG_SPE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_SPE) | (((val) & 0x1) << 26))
/* IP over VLAN PRI (25) */
#define LTQ_ES_P0_CTL_REG_IPVLAN   (0x1 << 25)
#define LTQ_ES_P0_CTL_REG_IPVLAN_VAL(val)   (((val) & 0x1) << 25)
#define LTQ_ES_P0_CTL_REG_IPVLAN_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_IPVLAN) >> 25) & 0x1)
#define LTQ_ES_P0_CTL_REG_IPVLAN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_IPVLAN) | (((val) & 0x1) << 25))
/* Ether Type Priority Enable (24) */
#define LTQ_ES_P0_CTL_REG_TPE   (0x1 << 24)
#define LTQ_ES_P0_CTL_REG_TPE_VAL(val)   (((val) & 0x1) << 24)
#define LTQ_ES_P0_CTL_REG_TPE_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_TPE) >> 24) & 0x1)
#define LTQ_ES_P0_CTL_REG_TPE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_TPE) | (((val) & 0x1) << 24))
/* Force Link Up (18) */
#define LTQ_ES_P0_CTL_REG_FLP   (0x1 << 18)
#define LTQ_ES_P0_CTL_REG_FLP_VAL(val)   (((val) & 0x1) << 18)
#define LTQ_ES_P0_CTL_REG_FLP_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_FLP) >> 18) & 0x1)
#define LTQ_ES_P0_CTL_REG_FLP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_FLP) | (((val) & 0x1) << 18))
/* Force Link Down (17) */
#define LTQ_ES_P0_CTL_REG_FLD   (0x1 << 17)
#define LTQ_ES_P0_CTL_REG_FLD_VAL(val)   (((val) & 0x1) << 17)
#define LTQ_ES_P0_CTL_REG_FLD_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_FLD) >> 17) & 0x1)
#define LTQ_ES_P0_CTL_REG_FLD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_FLD) | (((val) & 0x1) << 17))
/* Ratio Mode for WFQ (16) */
#define LTQ_ES_P0_CTL_REG_RMWFQ   (0x1 << 16)
#define LTQ_ES_P0_CTL_REG_RMWFQ_VAL(val)   (((val) & 0x1) << 16)
#define LTQ_ES_P0_CTL_REG_RMWFQ_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_RMWFQ) >> 16) & 0x1)
#define LTQ_ES_P0_CTL_REG_RMWFQ_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_RMWFQ) | (((val) & 0x1) << 16))
/* Aging Disable (15) */
#define LTQ_ES_P0_CTL_REG_AD   (0x1 << 15)
#define LTQ_ES_P0_CTL_REG_AD_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_P0_CTL_REG_AD_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_AD) >> 15) & 0x1)
#define LTQ_ES_P0_CTL_REG_AD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_AD) | (((val) & 0x1) << 15))
/* Learning Disable (14) */
#define LTQ_ES_P0_CTL_REG_LD   (0x1 << 14)
#define LTQ_ES_P0_CTL_REG_LD_VAL(val)   (((val) & 0x1) << 14)
#define LTQ_ES_P0_CTL_REG_LD_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_LD) >> 14) & 0x1)
#define LTQ_ES_P0_CTL_REG_LD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_LD) | (((val) & 0x1) << 14))
/* Maximum Number of Addresses (12:8) */
#define LTQ_ES_P0_CTL_REG_MNA024   (0x1f << 8)
#define LTQ_ES_P0_CTL_REG_MNA024_VAL(val)   (((val) & 0x1f) << 8)
#define LTQ_ES_P0_CTL_REG_MNA024_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_MNA024) >> 8) & 0x1f)
#define LTQ_ES_P0_CTL_REG_MNA024_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_MNA024) | (((val) & 0x1f) << 8))
/* PPPOE Port Only (7) */
#define LTQ_ES_P0_CTL_REG_PPPOEP   (0x1 << 7)
#define LTQ_ES_P0_CTL_REG_PPPOEP_VAL(val)   (((val) & 0x1) << 7)
#define LTQ_ES_P0_CTL_REG_PPPOEP_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_PPPOEP) >> 7) & 0x1)
#define LTQ_ES_P0_CTL_REG_PPPOEP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_PPPOEP) | (((val) & 0x1) << 7))
/* PPPOE Manage (6) */
#define LTQ_ES_P0_CTL_REG_PM   (0x1 << 6)
#define LTQ_ES_P0_CTL_REG_PM_VAL(val)   (((val) & 0x1) << 6)
#define LTQ_ES_P0_CTL_REG_PM_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_PM) >> 6) & 0x1)
#define LTQ_ES_P0_CTL_REG_PM_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_PM) | (((val) & 0x1) << 6))
/* Port Mirror Option (5:4) */
#define LTQ_ES_P0_CTL_REG_IPMO   (0x3 << 4)
#define LTQ_ES_P0_CTL_REG_IPMO_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_P0_CTL_REG_IPMO_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_IPMO) >> 4) & 0x3)
#define LTQ_ES_P0_CTL_REG_IPMO_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_IPMO) | (((val) & 0x3) << 4))
/* 802.1x Port Authorized state (3:2) */
#define LTQ_ES_P0_CTL_REG_PAS   (0x3 << 2)
#define LTQ_ES_P0_CTL_REG_PAS_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_P0_CTL_REG_PAS_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_PAS) >> 2) & 0x3)
#define LTQ_ES_P0_CTL_REG_PAS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_PAS) | (((val) & 0x3) << 2))
/* Drop Scheme for voilation 802.1x (1) */
#define LTQ_ES_P0_CTL_REG_DSV8021X   (0x1 << 1)
#define LTQ_ES_P0_CTL_REG_DSV8021X_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_ES_P0_CTL_REG_DSV8021X_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_DSV8021X) >> 1) & 0x1)
#define LTQ_ES_P0_CTL_REG_DSV8021X_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_DSV8021X) | (((val) & 0x1) << 1))
/* ByPass Mode for Output (0) */
#define LTQ_ES_P0_CTL_REG_BYPASS   (0x1)
#define LTQ_ES_P0_CTL_REG_BYPASS_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_ES_P0_CTL_REG_BYPASS_GET(val)   ((((val) & LTQ_ES_P0_CTL_REG_BYPASS) >> 0) & 0x1)
#define LTQ_ES_P0_CTL_REG_BYPASS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_CTL_REG_BYPASS) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Port 0 VLAN Control Register
 ******************************************************************************/

/* Default FID (31:30) */
#define LTQ_ES_P0_VLAN_REG_DFID   (0x3 << 30)
#define LTQ_ES_P0_VLAN_REG_DFID_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_P0_VLAN_REG_DFID_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_DFID) >> 30) & 0x3)
#define LTQ_ES_P0_VLAN_REG_DFID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_DFID) | (((val) & 0x3) << 30))
/* Tagged Base VLAN Enable (29) */
#define LTQ_ES_P0_VLAN_REG_TBVE   (0x1 << 29)
#define LTQ_ES_P0_VLAN_REG_TBVE_VAL(val)   (((val) & 0x1) << 29)
#define LTQ_ES_P0_VLAN_REG_TBVE_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_TBVE) >> 29) & 0x1)
#define LTQ_ES_P0_VLAN_REG_TBVE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_TBVE) | (((val) & 0x1) << 29))
/* Input Force No TAG Enable (28) */
#define LTQ_ES_P0_VLAN_REG_IFNTE   (0x1 << 28)
#define LTQ_ES_P0_VLAN_REG_IFNTE_VAL(val)   (((val) & 0x1) << 28)
#define LTQ_ES_P0_VLAN_REG_IFNTE_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_IFNTE) >> 28) & 0x1)
#define LTQ_ES_P0_VLAN_REG_IFNTE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_IFNTE) | (((val) & 0x1) << 28))
/* VID Check with the VID table (27) */
#define LTQ_ES_P0_VLAN_REG_VC   (0x1 << 27)
#define LTQ_ES_P0_VLAN_REG_VC_VAL(val)   (((val) & 0x1) << 27)
#define LTQ_ES_P0_VLAN_REG_VC_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_VC) >> 27) & 0x1)
#define LTQ_ES_P0_VLAN_REG_VC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_VC) | (((val) & 0x1) << 27))
/* VLAN Security Disable (26) */
#define LTQ_ES_P0_VLAN_REG_VSD   (0x1 << 26)
#define LTQ_ES_P0_VLAN_REG_VSD_VAL(val)   (((val) & 0x1) << 26)
#define LTQ_ES_P0_VLAN_REG_VSD_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_VSD) >> 26) & 0x1)
#define LTQ_ES_P0_VLAN_REG_VSD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_VSD) | (((val) & 0x1) << 26))
/* Admit Only VLAN_Tagged Packet (25) */
#define LTQ_ES_P0_VLAN_REG_AOVTP   (0x1 << 25)
#define LTQ_ES_P0_VLAN_REG_AOVTP_VAL(val)   (((val) & 0x1) << 25)
#define LTQ_ES_P0_VLAN_REG_AOVTP_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_AOVTP) >> 25) & 0x1)
#define LTQ_ES_P0_VLAN_REG_AOVTP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_AOVTP) | (((val) & 0x1) << 25))
/* VLAN Member Check Enable (24) */
#define LTQ_ES_P0_VLAN_REG_VMCE   (0x1 << 24)
#define LTQ_ES_P0_VLAN_REG_VMCE_VAL(val)   (((val) & 0x1) << 24)
#define LTQ_ES_P0_VLAN_REG_VMCE_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_VMCE) >> 24) & 0x1)
#define LTQ_ES_P0_VLAN_REG_VMCE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_VMCE) | (((val) & 0x1) << 24))
/* Reserved (23:19) */
#define LTQ_ES_P0_VLAN_REG_RES   (0x1f << 19)
#define LTQ_ES_P0_VLAN_REG_RES_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_RES) >> 19) & 0x1f)
/* Default VLAN Port Map (18:16) */
#define LTQ_ES_P0_VLAN_REG_DVPM   (0x7 << 16)
#define LTQ_ES_P0_VLAN_REG_DVPM_VAL(val)   (((val) & 0x7) << 16)
#define LTQ_ES_P0_VLAN_REG_DVPM_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_DVPM) >> 16) & 0x7)
#define LTQ_ES_P0_VLAN_REG_DVPM_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_DVPM) | (((val) & 0x7) << 16))
/* Port Priority (15:14) */
#define LTQ_ES_P0_VLAN_REG_PP   (0x3 << 14)
#define LTQ_ES_P0_VLAN_REG_PP_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_P0_VLAN_REG_PP_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_PP) >> 14) & 0x3)
#define LTQ_ES_P0_VLAN_REG_PP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_PP) | (((val) & 0x3) << 14))
/* Port Priority Enable (13) */
#define LTQ_ES_P0_VLAN_REG_PPE   (0x1 << 13)
#define LTQ_ES_P0_VLAN_REG_PPE_VAL(val)   (((val) & 0x1) << 13)
#define LTQ_ES_P0_VLAN_REG_PPE_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_PPE) >> 13) & 0x1)
#define LTQ_ES_P0_VLAN_REG_PPE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_PPE) | (((val) & 0x1) << 13))
/* Portbase VLAN tag member for Port 0 (12) */
#define LTQ_ES_P0_VLAN_REG_PVTAGMP   (0x1 << 12)
#define LTQ_ES_P0_VLAN_REG_PVTAGMP_VAL(val)   (((val) & 0x1) << 12)
#define LTQ_ES_P0_VLAN_REG_PVTAGMP_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_PVTAGMP) >> 12) & 0x1)
#define LTQ_ES_P0_VLAN_REG_PVTAGMP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_PVTAGMP) | (((val) & 0x1) << 12))
/* PVID (11:0) */
#define LTQ_ES_P0_VLAN_REG_PVID   (0xfff)
#define LTQ_ES_P0_VLAN_REG_PVID_VAL(val)   (((val) & 0xfff) << 0)
#define LTQ_ES_P0_VLAN_REG_PVID_GET(val)   ((((val) & LTQ_ES_P0_VLAN_REG_PVID) >> 0) & 0xfff)
#define LTQ_ES_P0_VLAN_REG_PVID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_VLAN_REG_PVID) | (((val) & 0xfff) << 0))

/*******************************************************************************
 * Port 0 Ingress Control Register
 ******************************************************************************/

/* Reserved  (31:13) */
#define LTQ_ES_P0_INCTL_REG_RES   (0x7ffff << 13)
#define LTQ_ES_P0_INCTL_REG_RES_GET(val)   ((((val) & LTQ_ES_P0_INCTL_REG_RES) >> 13) & 0x7ffff)
/* Port 0 Ingress/Egress Timer Tick T selection (12:11) */
#define LTQ_ES_P0_INCTL_REG_P0ITT   (0x3 << 11)
#define LTQ_ES_P0_INCTL_REG_P0ITT_VAL(val)   (((val) & 0x3) << 11)
#define LTQ_ES_P0_INCTL_REG_P0ITT_GET(val)   ((((val) & LTQ_ES_P0_INCTL_REG_P0ITT) >> 11) & 0x3)
#define LTQ_ES_P0_INCTL_REG_P0ITT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_INCTL_REG_P0ITT) | (((val) & 0x3) << 11))
/* Port 0 Igress Token R (10:0) */
#define LTQ_ES_P0_INCTL_REG_P0ITR   (0x7ff)
#define LTQ_ES_P0_INCTL_REG_P0ITR_VAL(val)   (((val) & 0x7ff) << 0)
#define LTQ_ES_P0_INCTL_REG_P0ITR_GET(val)   ((((val) & LTQ_ES_P0_INCTL_REG_P0ITR) >> 0) & 0x7ff)
#define LTQ_ES_P0_INCTL_REG_P0ITR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_INCTL_REG_P0ITR) | (((val) & 0x7ff) << 0))

/*******************************************************************************
 * Port 0 Egress Control for Strict Q32 Register
 ******************************************************************************/

/* Port 0 Egress Token R for Strict Priority Q3 (26:16) */
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ3TR   (0x7ff << 16)
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ3TR_VAL(val)   (((val) & 0x7ff) << 16)
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ3TR_GET(val)   ((((val) & LTQ_ES_P0_ECS_Q32_REG_P0SPQ3TR) >> 16) & 0x7ff)
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ3TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECS_Q32_REG_P0SPQ3TR) | (((val) & 0x7ff) << 16))
/* Port 0 Egress Token R for Strict Priority Q2 (10:0) */
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ2TR   (0x7ff)
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ2TR_VAL(val)   (((val) & 0x7ff) << 0)
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ2TR_GET(val)   ((((val) & LTQ_ES_P0_ECS_Q32_REG_P0SPQ2TR) >> 0) & 0x7ff)
#define LTQ_ES_P0_ECS_Q32_REG_P0SPQ2TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECS_Q32_REG_P0SPQ2TR) | (((val) & 0x7ff) << 0))

/*******************************************************************************
 * Port 0 Egress Control for Strict Q10 Register
 ******************************************************************************/

/* Reserved  (31:27) */
#define LTQ_ES_P0_ECS_Q10_REG_RES   (0x1f << 27)
#define LTQ_ES_P0_ECS_Q10_REG_RES_GET(val)   ((((val) & LTQ_ES_P0_ECS_Q10_REG_RES) >> 27) & 0x1f)
/* Port 0 Egress Token R for Strict Priority Q1 (26:16) */
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ1TR   (0x7ff << 16)
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ1TR_VAL(val)   (((val) & 0x7ff) << 16)
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ1TR_GET(val)   ((((val) & LTQ_ES_P0_ECS_Q10_REG_P0SPQ1TR) >> 16) & 0x7ff)
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ1TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECS_Q10_REG_P0SPQ1TR) | (((val) & 0x7ff) << 16))
/* Port 0 Egress Token R for Strict Priority Q0 (10:0) */
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ0TR   (0x7ff)
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ0TR_VAL(val)   (((val) & 0x7ff) << 0)
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ0TR_GET(val)   ((((val) & LTQ_ES_P0_ECS_Q10_REG_P0SPQ0TR) >> 0) & 0x7ff)
#define LTQ_ES_P0_ECS_Q10_REG_P0SPQ0TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECS_Q10_REG_P0SPQ0TR) | (((val) & 0x7ff) << 0))

/*******************************************************************************
 * Port 0 Egress Control for WFQ Q32 Register
 ******************************************************************************/

/* Reserved  (31:27) */
#define LTQ_ES_P0_ECW_Q32_REG_RES   (0x1f << 27)
#define LTQ_ES_P0_ECW_Q32_REG_RES_GET(val)   ((((val) & LTQ_ES_P0_ECW_Q32_REG_RES) >> 27) & 0x1f)
/* Port 0 Egress Token R for WFQ Q3 (26:16) */
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ3TR   (0x7ff << 16)
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ3TR_VAL(val)   (((val) & 0x7ff) << 16)
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ3TR_GET(val)   ((((val) & LTQ_ES_P0_ECW_Q32_REG_P0WQ3TR) >> 16) & 0x7ff)
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ3TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECW_Q32_REG_P0WQ3TR) | (((val) & 0x7ff) << 16))
/* Port 0 Egress Token R for WFQ Q2 (10:0) */
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ2TR   (0x7ff)
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ2TR_VAL(val)   (((val) & 0x7ff) << 0)
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ2TR_GET(val)   ((((val) & LTQ_ES_P0_ECW_Q32_REG_P0WQ2TR) >> 0) & 0x7ff)
#define LTQ_ES_P0_ECW_Q32_REG_P0WQ2TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECW_Q32_REG_P0WQ2TR) | (((val) & 0x7ff) << 0))

/*******************************************************************************
 * Port 0 Egress Control for WFQ Q10 Register
 ******************************************************************************/

/* Reserved  (31:27) */
#define LTQ_ES_P0_ECW_Q10_REG_RES   (0x1f << 27)
#define LTQ_ES_P0_ECW_Q10_REG_RES_GET(val)   ((((val) & LTQ_ES_P0_ECW_Q10_REG_RES) >> 27) & 0x1f)
/* Port 0 Egress Token R for WFQ Q1 (26:16) */
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ1TR   (0x7ff << 16)
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ1TR_VAL(val)   (((val) & 0x7ff) << 16)
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ1TR_GET(val)   ((((val) & LTQ_ES_P0_ECW_Q10_REG_P0WQ1TR) >> 16) & 0x7ff)
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ1TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECW_Q10_REG_P0WQ1TR) | (((val) & 0x7ff) << 16))
/* Port 0 Egress Token R for WFQ Q0 (10:0) */
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ0TR   (0x7ff)
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ0TR_VAL(val)   (((val) & 0x7ff) << 0)
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ0TR_GET(val)   ((((val) & LTQ_ES_P0_ECW_Q10_REG_P0WQ0TR) >> 0) & 0x7ff)
#define LTQ_ES_P0_ECW_Q10_REG_P0WQ0TR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_P0_ECW_Q10_REG_P0WQ0TR) | (((val) & 0x7ff) << 0))

/*******************************************************************************
 * Interrupt Enable Register
 ******************************************************************************/

/* Reserved (31:8) */
#define LTQ_ES_INT_ENA_REG_RES   (0xffffff << 8)
#define LTQ_ES_INT_ENA_REG_RES_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_RES) >> 8) & 0xffffff)
/* Data Buffer is Full Interrupt Enable (7) */
#define LTQ_ES_INT_ENA_REG_DBFIE   (0x1 << 7)
#define LTQ_ES_INT_ENA_REG_DBFIE_VAL(val)   (((val) & 0x1) << 7)
#define LTQ_ES_INT_ENA_REG_DBFIE_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_DBFIE) >> 7) & 0x1)
#define LTQ_ES_INT_ENA_REG_DBFIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_INT_ENA_REG_DBFIE) | (((val) & 0x1) << 7))
/* Data Buffer is nearly Full Interrupt Enable (6) */
#define LTQ_ES_INT_ENA_REG_DBNFIE   (0x1 << 6)
#define LTQ_ES_INT_ENA_REG_DBNFIE_VAL(val)   (((val) & 0x1) << 6)
#define LTQ_ES_INT_ENA_REG_DBNFIE_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_DBNFIE) >> 6) & 0x1)
#define LTQ_ES_INT_ENA_REG_DBNFIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_INT_ENA_REG_DBNFIE) | (((val) & 0x1) << 6))
/* Learning Table Full Interrupt Enable (5) */
#define LTQ_ES_INT_ENA_REG_LTFIE   (0x1 << 5)
#define LTQ_ES_INT_ENA_REG_LTFIE_VAL(val)   (((val) & 0x1) << 5)
#define LTQ_ES_INT_ENA_REG_LTFIE_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_LTFIE) >> 5) & 0x1)
#define LTQ_ES_INT_ENA_REG_LTFIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_INT_ENA_REG_LTFIE) | (((val) & 0x1) << 5))
/* Leaning Table Access Done Interrupt Enable (4) */
#define LTQ_ES_INT_ENA_REG_LTADIE   (0x1 << 4)
#define LTQ_ES_INT_ENA_REG_LTADIE_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_ES_INT_ENA_REG_LTADIE_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_LTADIE) >> 4) & 0x1)
#define LTQ_ES_INT_ENA_REG_LTADIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_INT_ENA_REG_LTADIE) | (((val) & 0x1) << 4))
/* Port Security Violation Interrupt Enable (3:1) */
#define LTQ_ES_INT_ENA_REG_PSVIE   (0x7 << 1)
#define LTQ_ES_INT_ENA_REG_PSVIE_VAL(val)   (((val) & 0x7) << 1)
#define LTQ_ES_INT_ENA_REG_PSVIE_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_PSVIE) >> 1) & 0x7)
#define LTQ_ES_INT_ENA_REG_PSVIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_INT_ENA_REG_PSVIE) | (((val) & 0x7) << 1))
/* Port Status Change Interrupt Enable (0) */
#define LTQ_ES_INT_ENA_REG_PSCIE   (0x1)
#define LTQ_ES_INT_ENA_REG_PSCIE_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_ES_INT_ENA_REG_PSCIE_GET(val)   ((((val) & LTQ_ES_INT_ENA_REG_PSCIE) >> 0) & 0x1)
#define LTQ_ES_INT_ENA_REG_PSCIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_INT_ENA_REG_PSCIE) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Interrupt Status Register
 ******************************************************************************/

/* Reserved (31:8) */
#define LTQ_ES_INT_ST_REG_RES   (0xffffff << 8)
#define LTQ_ES_INT_ST_REG_RES_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_RES) >> 8) & 0xffffff)
/* Data Buffer is Full (7) */
#define LTQ_ES_INT_ST_REG_DBF   (0x1 << 7)
#define LTQ_ES_INT_ST_REG_DBF_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_DBF) >> 7) & 0x1)
/* Data Buffer is nearly Full (6) */
#define LTQ_ES_INT_ST_REG_DBNF   (0x1 << 6)
#define LTQ_ES_INT_ST_REG_DBNF_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_DBNF) >> 6) & 0x1)
/* Learning Table Full (5) */
#define LTQ_ES_INT_ST_REG_LTF   (0x1 << 5)
#define LTQ_ES_INT_ST_REG_LTF_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_LTF) >> 5) & 0x1)
/* Leaning Table Access Done (4) */
#define LTQ_ES_INT_ST_REG_LTAD   (0x1 << 4)
#define LTQ_ES_INT_ST_REG_LTAD_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_LTAD) >> 4) & 0x1)
/* Port Security Violation (3:1) */
#define LTQ_ES_INT_ST_REG_PSV   (0x7 << 1)
#define LTQ_ES_INT_ST_REG_PSV_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_PSV) >> 1) & 0x7)
/* Port Status Change (0) */
#define LTQ_ES_INT_ST_REG_PSC   (0x1)
#define LTQ_ES_INT_ST_REG_PSC_GET(val)   ((((val) & LTQ_ES_INT_ST_REG_PSC) >> 0) & 0x1)

/*******************************************************************************
 * Switch Global Control Register 0
 ******************************************************************************/

/* Switch Enable (31) */
#define LTQ_ES_SW_GCTL0_REG_SE   (0x1 << 31)
#define LTQ_ES_SW_GCTL0_REG_SE_VAL(val)   (((val) & 0x1) << 31)
#define LTQ_ES_SW_GCTL0_REG_SE_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_SE) >> 31) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_SE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_SE) | (((val) & 0x1) << 31))
/* CRC Check Disable (30) */
#define LTQ_ES_SW_GCTL0_REG_ICRCCD   (0x1 << 30)
#define LTQ_ES_SW_GCTL0_REG_ICRCCD_VAL(val)   (((val) & 0x1) << 30)
#define LTQ_ES_SW_GCTL0_REG_ICRCCD_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_ICRCCD) >> 30) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_ICRCCD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_ICRCCD) | (((val) & 0x1) << 30))
/* Replace VID0 (28) */
#define LTQ_ES_SW_GCTL0_REG_RVID0   (0x1 << 28)
#define LTQ_ES_SW_GCTL0_REG_RVID0_VAL(val)   (((val) & 0x1) << 28)
#define LTQ_ES_SW_GCTL0_REG_RVID0_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_RVID0) >> 28) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_RVID0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_RVID0) | (((val) & 0x1) << 28))
/* Replace VID1 (27) */
#define LTQ_ES_SW_GCTL0_REG_RVID1   (0x1 << 27)
#define LTQ_ES_SW_GCTL0_REG_RVID1_VAL(val)   (((val) & 0x1) << 27)
#define LTQ_ES_SW_GCTL0_REG_RVID1_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_RVID1) >> 27) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_RVID1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_RVID1) | (((val) & 0x1) << 27))
/* Replace VIDFFF (26) */
#define LTQ_ES_SW_GCTL0_REG_RVIDFFF   (0x1 << 26)
#define LTQ_ES_SW_GCTL0_REG_RVIDFFF_VAL(val)   (((val) & 0x1) << 26)
#define LTQ_ES_SW_GCTL0_REG_RVIDFFF_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_RVIDFFF) >> 26) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_RVIDFFF_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_RVIDFFF) | (((val) & 0x1) << 26))
/* Priority Change Rule (25) */
#define LTQ_ES_SW_GCTL0_REG_PCR   (0x1 << 25)
#define LTQ_ES_SW_GCTL0_REG_PCR_VAL(val)   (((val) & 0x1) << 25)
#define LTQ_ES_SW_GCTL0_REG_PCR_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_PCR) >> 25) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_PCR_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_PCR) | (((val) & 0x1) << 25))
/* Priority Change Enable (24) */
#define LTQ_ES_SW_GCTL0_REG_PCE   (0x1 << 24)
#define LTQ_ES_SW_GCTL0_REG_PCE_VAL(val)   (((val) & 0x1) << 24)
#define LTQ_ES_SW_GCTL0_REG_PCE_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_PCE) >> 24) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_PCE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_PCE) | (((val) & 0x1) << 24))
/* Transmit Short IPG Enable (23) */
#define LTQ_ES_SW_GCTL0_REG_TSIPGE   (0x1 << 23)
#define LTQ_ES_SW_GCTL0_REG_TSIPGE_VAL(val)   (((val) & 0x1) << 23)
#define LTQ_ES_SW_GCTL0_REG_TSIPGE_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_TSIPGE) >> 23) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_TSIPGE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_TSIPGE) | (((val) & 0x1) << 23))
/* PHY Base Address (22) */
#define LTQ_ES_SW_GCTL0_REG_PHYBA   (0x1 << 22)
#define LTQ_ES_SW_GCTL0_REG_PHYBA_VAL(val)   (((val) & 0x1) << 22)
#define LTQ_ES_SW_GCTL0_REG_PHYBA_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_PHYBA) >> 22) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_PHYBA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_PHYBA) | (((val) & 0x1) << 22))
/* Drop Packet When Excessive Collision Happen (21) */
#define LTQ_ES_SW_GCTL0_REG_DPWECH   (0x1 << 21)
#define LTQ_ES_SW_GCTL0_REG_DPWECH_VAL(val)   (((val) & 0x1) << 21)
#define LTQ_ES_SW_GCTL0_REG_DPWECH_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_DPWECH) >> 21) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_DPWECH_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_DPWECH) | (((val) & 0x1) << 21))
/* Aging Timer Select (20:18) */
#define LTQ_ES_SW_GCTL0_REG_ATS   (0x7 << 18)
#define LTQ_ES_SW_GCTL0_REG_ATS_VAL(val)   (((val) & 0x7) << 18)
#define LTQ_ES_SW_GCTL0_REG_ATS_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_ATS) >> 18) & 0x7)
#define LTQ_ES_SW_GCTL0_REG_ATS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_ATS) | (((val) & 0x7) << 18))
/* Mirror CRC Also (17) */
#define LTQ_ES_SW_GCTL0_REG_MCA   (0x1 << 17)
#define LTQ_ES_SW_GCTL0_REG_MCA_VAL(val)   (((val) & 0x1) << 17)
#define LTQ_ES_SW_GCTL0_REG_MCA_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_MCA) >> 17) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_MCA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_MCA) | (((val) & 0x1) << 17))
/* Mirror RXER Also (16) */
#define LTQ_ES_SW_GCTL0_REG_MRA   (0x1 << 16)
#define LTQ_ES_SW_GCTL0_REG_MRA_VAL(val)   (((val) & 0x1) << 16)
#define LTQ_ES_SW_GCTL0_REG_MRA_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_MRA) >> 16) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_MRA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_MRA) | (((val) & 0x1) << 16))
/* Mirror PAUSE Also (15) */
#define LTQ_ES_SW_GCTL0_REG_MPA   (0x1 << 15)
#define LTQ_ES_SW_GCTL0_REG_MPA_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_SW_GCTL0_REG_MPA_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_MPA) >> 15) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_MPA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_MPA) | (((val) & 0x1) << 15))
/* Mirror Long Also (14) */
#define LTQ_ES_SW_GCTL0_REG_MLA   (0x1 << 14)
#define LTQ_ES_SW_GCTL0_REG_MLA_VAL(val)   (((val) & 0x1) << 14)
#define LTQ_ES_SW_GCTL0_REG_MLA_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_MLA) >> 14) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_MLA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_MLA) | (((val) & 0x1) << 14))
/* Mirror Short Also (13) */
#define LTQ_ES_SW_GCTL0_REG_MSA   (0x1 << 13)
#define LTQ_ES_SW_GCTL0_REG_MSA_VAL(val)   (((val) & 0x1) << 13)
#define LTQ_ES_SW_GCTL0_REG_MSA_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_MSA) >> 13) & 0x1)
#define LTQ_ES_SW_GCTL0_REG_MSA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_MSA) | (((val) & 0x1) << 13))
/* Sniffer port number (12:11) */
#define LTQ_ES_SW_GCTL0_REG_SNIFFPN   (0x3 << 11)
#define LTQ_ES_SW_GCTL0_REG_SNIFFPN_VAL(val)   (((val) & 0x3) << 11)
#define LTQ_ES_SW_GCTL0_REG_SNIFFPN_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_SNIFFPN) >> 11) & 0x3)
#define LTQ_ES_SW_GCTL0_REG_SNIFFPN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_SNIFFPN) | (((val) & 0x3) << 11))
/* Max Packet Length (MAXPKTLEN) (9:8) */
#define LTQ_ES_SW_GCTL0_REG_MPL   (0x3 << 8)
#define LTQ_ES_SW_GCTL0_REG_MPL_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_SW_GCTL0_REG_MPL_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_MPL) >> 8) & 0x3)
#define LTQ_ES_SW_GCTL0_REG_MPL_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_MPL) | (((val) & 0x3) << 8))
/* Discard Mode (Drop scheme for Packets Classified as Q3) (7:6) */
#define LTQ_ES_SW_GCTL0_REG_DMQ3   (0x3 << 6)
#define LTQ_ES_SW_GCTL0_REG_DMQ3_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_SW_GCTL0_REG_DMQ3_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_DMQ3) >> 6) & 0x3)
#define LTQ_ES_SW_GCTL0_REG_DMQ3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_DMQ3) | (((val) & 0x3) << 6))
/* Discard Mode (Drop scheme for Packets Classified as Q2) (5:4) */
#define LTQ_ES_SW_GCTL0_REG_DMQ2   (0x3 << 4)
#define LTQ_ES_SW_GCTL0_REG_DMQ2_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_SW_GCTL0_REG_DMQ2_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_DMQ2) >> 4) & 0x3)
#define LTQ_ES_SW_GCTL0_REG_DMQ2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_DMQ2) | (((val) & 0x3) << 4))
/* Discard Mode (Drop scheme for Packets Classified as Q1) (3:2) */
#define LTQ_ES_SW_GCTL0_REG_DMQ1   (0x3 << 2)
#define LTQ_ES_SW_GCTL0_REG_DMQ1_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_SW_GCTL0_REG_DMQ1_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_DMQ1) >> 2) & 0x3)
#define LTQ_ES_SW_GCTL0_REG_DMQ1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_DMQ1) | (((val) & 0x3) << 2))
/* Discard Mode (Drop scheme for Packets Classified as Q0) (1:0) */
#define LTQ_ES_SW_GCTL0_REG_DMQ0   (0x3)
#define LTQ_ES_SW_GCTL0_REG_DMQ0_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_SW_GCTL0_REG_DMQ0_GET(val)   ((((val) & LTQ_ES_SW_GCTL0_REG_DMQ0) >> 0) & 0x3)
#define LTQ_ES_SW_GCTL0_REG_DMQ0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL0_REG_DMQ0) | (((val) & 0x3) << 0))

/*******************************************************************************
 * Switch Global Control Register 1
 ******************************************************************************/

/* BIST Done (27) */
#define LTQ_ES_SW_GCTL1_REG_BISTDN   (0x1 << 27)
#define LTQ_ES_SW_GCTL1_REG_BISTDN_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_BISTDN) >> 27) & 0x1)
/* Enable drop scheme of TX and RX (26) */
#define LTQ_ES_SW_GCTL1_REG_EDSTX   (0x1 << 26)
#define LTQ_ES_SW_GCTL1_REG_EDSTX_VAL(val)   (((val) & 0x1) << 26)
#define LTQ_ES_SW_GCTL1_REG_EDSTX_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_EDSTX) >> 26) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_EDSTX_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_EDSTX) | (((val) & 0x1) << 26))
/* Congestion threshold for TX queue (25:24) */
#define LTQ_ES_SW_GCTL1_REG_CTTX   (0x3 << 24)
#define LTQ_ES_SW_GCTL1_REG_CTTX_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_SW_GCTL1_REG_CTTX_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_CTTX) >> 24) & 0x3)
#define LTQ_ES_SW_GCTL1_REG_CTTX_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_CTTX) | (((val) & 0x3) << 24))
/* Input Jam Threshold (23:21) */
#define LTQ_ES_SW_GCTL1_REG_IJT   (0x7 << 21)
#define LTQ_ES_SW_GCTL1_REG_IJT_VAL(val)   (((val) & 0x7) << 21)
#define LTQ_ES_SW_GCTL1_REG_IJT_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_IJT) >> 21) & 0x7)
#define LTQ_ES_SW_GCTL1_REG_IJT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_IJT) | (((val) & 0x7) << 21))
/* Do not Identify VLAN after SNAP (20) */
#define LTQ_ES_SW_GCTL1_REG_DIVS   (0x1 << 20)
#define LTQ_ES_SW_GCTL1_REG_DIVS_VAL(val)   (((val) & 0x1) << 20)
#define LTQ_ES_SW_GCTL1_REG_DIVS_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_DIVS) >> 20) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_DIVS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_DIVS) | (((val) & 0x1) << 20))
/* Do not Identify IPV6 in PPPOE (19) */
#define LTQ_ES_SW_GCTL1_REG_DII6P   (0x1 << 19)
#define LTQ_ES_SW_GCTL1_REG_DII6P_VAL(val)   (((val) & 0x1) << 19)
#define LTQ_ES_SW_GCTL1_REG_DII6P_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_DII6P) >> 19) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_DII6P_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_DII6P) | (((val) & 0x1) << 19))
/* Do not Identify IP in PPPOE after SNAP (18) */
#define LTQ_ES_SW_GCTL1_REG_DIIPS   (0x1 << 18)
#define LTQ_ES_SW_GCTL1_REG_DIIPS_VAL(val)   (((val) & 0x1) << 18)
#define LTQ_ES_SW_GCTL1_REG_DIIPS_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_DIIPS) >> 18) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_DIIPS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_DIIPS) | (((val) & 0x1) << 18))
/* Do not Identify Ether-Type = 0x0800, IP VER = 6 as IPV6 packets (17) */
#define LTQ_ES_SW_GCTL1_REG_DIE   (0x1 << 17)
#define LTQ_ES_SW_GCTL1_REG_DIE_VAL(val)   (((val) & 0x1) << 17)
#define LTQ_ES_SW_GCTL1_REG_DIE_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_DIE) >> 17) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_DIE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_DIE) | (((val) & 0x1) << 17))
/* Do not Identify IP in PPPOE (16) */
#define LTQ_ES_SW_GCTL1_REG_DIIP   (0x1 << 16)
#define LTQ_ES_SW_GCTL1_REG_DIIP_VAL(val)   (((val) & 0x1) << 16)
#define LTQ_ES_SW_GCTL1_REG_DIIP_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_DIIP) >> 16) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_DIIP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_DIIP) | (((val) & 0x1) << 16))
/* Do not Identify SNAP (15) */
#define LTQ_ES_SW_GCTL1_REG_DIS   (0x1 << 15)
#define LTQ_ES_SW_GCTL1_REG_DIS_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_SW_GCTL1_REG_DIS_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_DIS) >> 15) & 0x1)
#define LTQ_ES_SW_GCTL1_REG_DIS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_DIS) | (((val) & 0x1) << 15))
/* Unicast Portmap (14:12) */
#define LTQ_ES_SW_GCTL1_REG_UP   (0x7 << 12)
#define LTQ_ES_SW_GCTL1_REG_UP_VAL(val)   (((val) & 0x7) << 12)
#define LTQ_ES_SW_GCTL1_REG_UP_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_UP) >> 12) & 0x7)
#define LTQ_ES_SW_GCTL1_REG_UP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_UP) | (((val) & 0x7) << 12))
/* Broadcast Portmap (10:8) */
#define LTQ_ES_SW_GCTL1_REG_BP   (0x7 << 8)
#define LTQ_ES_SW_GCTL1_REG_BP_VAL(val)   (((val) & 0x7) << 8)
#define LTQ_ES_SW_GCTL1_REG_BP_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_BP) >> 8) & 0x7)
#define LTQ_ES_SW_GCTL1_REG_BP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_BP) | (((val) & 0x7) << 8))
/* Multicast Portmap (6:4) */
#define LTQ_ES_SW_GCTL1_REG_MP   (0x7 << 4)
#define LTQ_ES_SW_GCTL1_REG_MP_VAL(val)   (((val) & 0x7) << 4)
#define LTQ_ES_SW_GCTL1_REG_MP_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_MP) >> 4) & 0x7)
#define LTQ_ES_SW_GCTL1_REG_MP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_MP) | (((val) & 0x7) << 4))
/* Reserve Portmap (2:0) */
#define LTQ_ES_SW_GCTL1_REG_RP   (0x7)
#define LTQ_ES_SW_GCTL1_REG_RP_VAL(val)   (((val) & 0x7) << 0)
#define LTQ_ES_SW_GCTL1_REG_RP_GET(val)   ((((val) & LTQ_ES_SW_GCTL1_REG_RP) >> 0) & 0x7)
#define LTQ_ES_SW_GCTL1_REG_RP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_SW_GCTL1_REG_RP) | (((val) & 0x7) << 0))

/*******************************************************************************
 * ARP/RARP Register
 ******************************************************************************/

/* MAC Control Action (15:14) */
#define LTQ_ES_ARP_REG_MACA   (0x3 << 14)
#define LTQ_ES_ARP_REG_MACA_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_ARP_REG_MACA_GET(val)   ((((val) & LTQ_ES_ARP_REG_MACA) >> 14) & 0x3)
#define LTQ_ES_ARP_REG_MACA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_MACA) | (((val) & 0x3) << 14))
/* Unicast packet Treated as Cross_VLAN packet (13) */
#define LTQ_ES_ARP_REG_UPT   (0x1 << 13)
#define LTQ_ES_ARP_REG_UPT_VAL(val)   (((val) & 0x1) << 13)
#define LTQ_ES_ARP_REG_UPT_GET(val)   ((((val) & LTQ_ES_ARP_REG_UPT) >> 13) & 0x1)
#define LTQ_ES_ARP_REG_UPT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_UPT) | (((val) & 0x1) << 13))
/* RARP Packet Treated as Cross_VLAN Packet (12) */
#define LTQ_ES_ARP_REG_RPT   (0x1 << 12)
#define LTQ_ES_ARP_REG_RPT_VAL(val)   (((val) & 0x1) << 12)
#define LTQ_ES_ARP_REG_RPT_GET(val)   ((((val) & LTQ_ES_ARP_REG_RPT) >> 12) & 0x1)
#define LTQ_ES_ARP_REG_RPT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_RPT) | (((val) & 0x1) << 12))
/* RARP/ARP Packet Action (11:10) */
#define LTQ_ES_ARP_REG_RAPA   (0x3 << 10)
#define LTQ_ES_ARP_REG_RAPA_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_ARP_REG_RAPA_GET(val)   ((((val) & LTQ_ES_ARP_REG_RAPA) >> 10) & 0x3)
#define LTQ_ES_ARP_REG_RAPA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_RAPA) | (((val) & 0x3) << 10))
/* RARP/ARP Packet Priority Enable (9) */
#define LTQ_ES_ARP_REG_RAPPE   (0x1 << 9)
#define LTQ_ES_ARP_REG_RAPPE_VAL(val)   (((val) & 0x1) << 9)
#define LTQ_ES_ARP_REG_RAPPE_GET(val)   ((((val) & LTQ_ES_ARP_REG_RAPPE) >> 9) & 0x1)
#define LTQ_ES_ARP_REG_RAPPE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_RAPPE) | (((val) & 0x1) << 9))
/* RARP/ARP Packet Priority (8:7) */
#define LTQ_ES_ARP_REG_RAPP   (0x3 << 7)
#define LTQ_ES_ARP_REG_RAPP_VAL(val)   (((val) & 0x3) << 7)
#define LTQ_ES_ARP_REG_RAPP_GET(val)   ((((val) & LTQ_ES_ARP_REG_RAPP) >> 7) & 0x3)
#define LTQ_ES_ARP_REG_RAPP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_RAPP) | (((val) & 0x3) << 7))
/* RARP/ARP Packet Output Tag Handle (6:5) */
#define LTQ_ES_ARP_REG_RAPOTH   (0x3 << 5)
#define LTQ_ES_ARP_REG_RAPOTH_VAL(val)   (((val) & 0x3) << 5)
#define LTQ_ES_ARP_REG_RAPOTH_GET(val)   ((((val) & LTQ_ES_ARP_REG_RAPOTH) >> 5) & 0x3)
#define LTQ_ES_ARP_REG_RAPOTH_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_RAPOTH) | (((val) & 0x3) << 5))
/* ARP Packet Treated as Cross _ VLAN Packet (4) */
#define LTQ_ES_ARP_REG_APT   (0x1 << 4)
#define LTQ_ES_ARP_REG_APT_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_ES_ARP_REG_APT_GET(val)   ((((val) & LTQ_ES_ARP_REG_APT) >> 4) & 0x1)
#define LTQ_ES_ARP_REG_APT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_APT) | (((val) & 0x1) << 4))
/* RARP/ARP Packet Treated as Management Packet (3) */
#define LTQ_ES_ARP_REG_RAPTM   (0x1 << 3)
#define LTQ_ES_ARP_REG_RAPTM_VAL(val)   (((val) & 0x1) << 3)
#define LTQ_ES_ARP_REG_RAPTM_GET(val)   ((((val) & LTQ_ES_ARP_REG_RAPTM) >> 3) & 0x1)
#define LTQ_ES_ARP_REG_RAPTM_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_RAPTM) | (((val) & 0x1) << 3))
/* RARP/ARP Packet Treated as Span Packet (2) */
#define LTQ_ES_ARP_REG_TAPTS   (0x1 << 2)
#define LTQ_ES_ARP_REG_TAPTS_VAL(val)   (((val) & 0x1) << 2)
#define LTQ_ES_ARP_REG_TAPTS_GET(val)   ((((val) & LTQ_ES_ARP_REG_TAPTS) >> 2) & 0x1)
#define LTQ_ES_ARP_REG_TAPTS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_TAPTS) | (((val) & 0x1) << 2))
/* Trap ARP Packet (1) */
#define LTQ_ES_ARP_REG_TAP   (0x1 << 1)
#define LTQ_ES_ARP_REG_TAP_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_ES_ARP_REG_TAP_GET(val)   ((((val) & LTQ_ES_ARP_REG_TAP) >> 1) & 0x1)
#define LTQ_ES_ARP_REG_TAP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_TAP) | (((val) & 0x1) << 1))
/* Trap RARP Packet (0) */
#define LTQ_ES_ARP_REG_TRP   (0x1)
#define LTQ_ES_ARP_REG_TRP_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_ES_ARP_REG_TRP_GET(val)   ((((val) & LTQ_ES_ARP_REG_TRP) >> 0) & 0x1)
#define LTQ_ES_ARP_REG_TRP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ARP_REG_TRP) | (((val) & 0x1) << 0))

/*******************************************************************************
 * Storm control Register
 ******************************************************************************/

/* Reserved (31:29) */
#define LTQ_ES_STRM_CTL_REG_RES   (0x7 << 29)
#define LTQ_ES_STRM_CTL_REG_RES_GET(val)   ((((val) & LTQ_ES_STRM_CTL_REG_RES) >> 29) & 0x7)
/* 10M Threshold (28:16) */
#define LTQ_ES_STRM_CTL_REG_STORM_10_TH   (0x1fff << 16)
#define LTQ_ES_STRM_CTL_REG_STORM_10_TH_VAL(val)   (((val) & 0x1fff) << 16)
#define LTQ_ES_STRM_CTL_REG_STORM_10_TH_GET(val)   ((((val) & LTQ_ES_STRM_CTL_REG_STORM_10_TH) >> 16) & 0x1fff)
#define LTQ_ES_STRM_CTL_REG_STORM_10_TH_SET(reg,val) (reg) = ((reg & ~LTQ_ES_STRM_CTL_REG_STORM_10_TH) | (((val) & 0x1fff) << 16))
/* Storm Enable for Broadcast Packets (15) */
#define LTQ_ES_STRM_CTL_REG_STORM_B   (0x1 << 15)
#define LTQ_ES_STRM_CTL_REG_STORM_B_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_STRM_CTL_REG_STORM_B_GET(val)   ((((val) & LTQ_ES_STRM_CTL_REG_STORM_B) >> 15) & 0x1)
#define LTQ_ES_STRM_CTL_REG_STORM_B_SET(reg,val) (reg) = ((reg & ~LTQ_ES_STRM_CTL_REG_STORM_B) | (((val) & 0x1) << 15))
/* Storm Enable for Multicast Packets (14) */
#define LTQ_ES_STRM_CTL_REG_STORM_M   (0x1 << 14)
#define LTQ_ES_STRM_CTL_REG_STORM_M_VAL(val)   (((val) & 0x1) << 14)
#define LTQ_ES_STRM_CTL_REG_STORM_M_GET(val)   ((((val) & LTQ_ES_STRM_CTL_REG_STORM_M) >> 14) & 0x1)
#define LTQ_ES_STRM_CTL_REG_STORM_M_SET(reg,val) (reg) = ((reg & ~LTQ_ES_STRM_CTL_REG_STORM_M) | (((val) & 0x1) << 14))
/* Storm Enable for Un-learned Unicast Packets (13) */
#define LTQ_ES_STRM_CTL_REG_STORM_U   (0x1 << 13)
#define LTQ_ES_STRM_CTL_REG_STORM_U_VAL(val)   (((val) & 0x1) << 13)
#define LTQ_ES_STRM_CTL_REG_STORM_U_GET(val)   ((((val) & LTQ_ES_STRM_CTL_REG_STORM_U) >> 13) & 0x1)
#define LTQ_ES_STRM_CTL_REG_STORM_U_SET(reg,val) (reg) = ((reg & ~LTQ_ES_STRM_CTL_REG_STORM_U) | (((val) & 0x1) << 13))
/* 100M Threshold (12:0) */
#define LTQ_ES_STRM_CTL_REG_STORM_100_TH   (0x1fff)
#define LTQ_ES_STRM_CTL_REG_STORM_100_TH_VAL(val)   (((val) & 0x1fff) << 0)
#define LTQ_ES_STRM_CTL_REG_STORM_100_TH_GET(val)   ((((val) & LTQ_ES_STRM_CTL_REG_STORM_100_TH) >> 0) & 0x1fff)
#define LTQ_ES_STRM_CTL_REG_STORM_100_TH_SET(reg,val) (reg) = ((reg & ~LTQ_ES_STRM_CTL_REG_STORM_100_TH) | (((val) & 0x1fff) << 0))

/*******************************************************************************
 * RGMII/GMII Port Control Register
 ******************************************************************************/

/* Management Clock Select (31:24) */
#define LTQ_ES_RGMII_CTL_REG_MCS   (0xff << 24)
#define LTQ_ES_RGMII_CTL_REG_MCS_VAL(val)   (((val) & 0xff) << 24)
#define LTQ_ES_RGMII_CTL_REG_MCS_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_MCS) >> 24) & 0xff)
#define LTQ_ES_RGMII_CTL_REG_MCS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_MCS) | (((val) & 0xff) << 24))
/* Interface Selection (19:18) */
#define LTQ_ES_RGMII_CTL_REG_IS   (0x3 << 18)
#define LTQ_ES_RGMII_CTL_REG_IS_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_IS) >> 18) & 0x3)
/* Port 1 RGMII Rx Clock Delay (17:16) */
#define LTQ_ES_RGMII_CTL_REG_P1RDLY   (0x3 << 16)
#define LTQ_ES_RGMII_CTL_REG_P1RDLY_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_RGMII_CTL_REG_P1RDLY_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P1RDLY) >> 16) & 0x3)
#define LTQ_ES_RGMII_CTL_REG_P1RDLY_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P1RDLY) | (((val) & 0x3) << 16))
/* Port 1 RGMII Tx Clock Delay (15:14) */
#define LTQ_ES_RGMII_CTL_REG_P1TDLY   (0x3 << 14)
#define LTQ_ES_RGMII_CTL_REG_P1TDLY_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_RGMII_CTL_REG_P1TDLY_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P1TDLY) >> 14) & 0x3)
#define LTQ_ES_RGMII_CTL_REG_P1TDLY_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P1TDLY) | (((val) & 0x3) << 14))
/* Port 1 Speed (13:12) */
#define LTQ_ES_RGMII_CTL_REG_P1SPD   (0x3 << 12)
#define LTQ_ES_RGMII_CTL_REG_P1SPD_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_RGMII_CTL_REG_P1SPD_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P1SPD) >> 12) & 0x3)
#define LTQ_ES_RGMII_CTL_REG_P1SPD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P1SPD) | (((val) & 0x3) << 12))
/* Port 1 Duplex mode (11) */
#define LTQ_ES_RGMII_CTL_REG_P1DUP   (0x1 << 11)
#define LTQ_ES_RGMII_CTL_REG_P1DUP_VAL(val)   (((val) & 0x1) << 11)
#define LTQ_ES_RGMII_CTL_REG_P1DUP_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P1DUP) >> 11) & 0x1)
#define LTQ_ES_RGMII_CTL_REG_P1DUP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P1DUP) | (((val) & 0x1) << 11))
/* Port 1 Flow Control Enable (10) */
#define LTQ_ES_RGMII_CTL_REG_P1FCE   (0x1 << 10)
#define LTQ_ES_RGMII_CTL_REG_P1FCE_VAL(val)   (((val) & 0x1) << 10)
#define LTQ_ES_RGMII_CTL_REG_P1FCE_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P1FCE) >> 10) & 0x1)
#define LTQ_ES_RGMII_CTL_REG_P1FCE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P1FCE) | (((val) & 0x1) << 10))
/* Port 0 RGMII Rx Clock Delay (7:6) */
#define LTQ_ES_RGMII_CTL_REG_P0RDLY   (0x3 << 6)
#define LTQ_ES_RGMII_CTL_REG_P0RDLY_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_RGMII_CTL_REG_P0RDLY_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P0RDLY) >> 6) & 0x3)
#define LTQ_ES_RGMII_CTL_REG_P0RDLY_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P0RDLY) | (((val) & 0x3) << 6))
/* Port 0 RGMII Tx Clock Delay (5:4) */
#define LTQ_ES_RGMII_CTL_REG_P0TDLY   (0x3 << 4)
#define LTQ_ES_RGMII_CTL_REG_P0TDLY_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_RGMII_CTL_REG_P0TDLY_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P0TDLY) >> 4) & 0x3)
#define LTQ_ES_RGMII_CTL_REG_P0TDLY_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P0TDLY) | (((val) & 0x3) << 4))
/* Port 0 Speed (3:2) */
#define LTQ_ES_RGMII_CTL_REG_P0SPD   (0x3 << 2)
#define LTQ_ES_RGMII_CTL_REG_P0SPD_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_RGMII_CTL_REG_P0SPD_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P0SPD) >> 2) & 0x3)
#define LTQ_ES_RGMII_CTL_REG_P0SPD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P0SPD) | (((val) & 0x3) << 2))
/* Port 0 Duplex mode (1) */
#define LTQ_ES_RGMII_CTL_REG_P0DUP   (0x1 << 1)
#define LTQ_ES_RGMII_CTL_REG_P0DUP_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_ES_RGMII_CTL_REG_P0DUP_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P0DUP) >> 1) & 0x1)
#define LTQ_ES_RGMII_CTL_REG_P0DUP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P0DUP) | (((val) & 0x1) << 1))
/* Port 0 Flow Control Enable (0) */
#define LTQ_ES_RGMII_CTL_REG_P0FCE   (0x1)
#define LTQ_ES_RGMII_CTL_REG_P0FCE_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_ES_RGMII_CTL_REG_P0FCE_GET(val)   ((((val) & LTQ_ES_RGMII_CTL_REG_P0FCE) >> 0) & 0x1)
#define LTQ_ES_RGMII_CTL_REG_P0FCE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RGMII_CTL_REG_P0FCE) | (((val) & 0x1) << 0))

/*******************************************************************************
 * 802.1p Priority Map Register
 ******************************************************************************/

/* Priority Queue 7 (15:14) */
#define LTQ_ES_PRT_1P_REG_1PPQ7   (0x3 << 14)
#define LTQ_ES_PRT_1P_REG_1PPQ7_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_PRT_1P_REG_1PPQ7_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ7) >> 14) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ7_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ7) | (((val) & 0x3) << 14))
/* Priority Queue 6 (13:12) */
#define LTQ_ES_PRT_1P_REG_1PPQ6   (0x3 << 12)
#define LTQ_ES_PRT_1P_REG_1PPQ6_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_PRT_1P_REG_1PPQ6_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ6) >> 12) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ6_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ6) | (((val) & 0x3) << 12))
/* Priority Queue 5 (11:10) */
#define LTQ_ES_PRT_1P_REG_1PPQ5   (0x3 << 10)
#define LTQ_ES_PRT_1P_REG_1PPQ5_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_PRT_1P_REG_1PPQ5_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ5) >> 10) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ5_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ5) | (((val) & 0x3) << 10))
/* Priority Queue 4 (9:8) */
#define LTQ_ES_PRT_1P_REG_1PPQ4   (0x3 << 8)
#define LTQ_ES_PRT_1P_REG_1PPQ4_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_PRT_1P_REG_1PPQ4_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ4) >> 8) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ4_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ4) | (((val) & 0x3) << 8))
/* Priority Queue 3 (7:6) */
#define LTQ_ES_PRT_1P_REG_1PPQ3   (0x3 << 6)
#define LTQ_ES_PRT_1P_REG_1PPQ3_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_PRT_1P_REG_1PPQ3_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ3) >> 6) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ3) | (((val) & 0x3) << 6))
/* Priority Queue 2 (5:4) */
#define LTQ_ES_PRT_1P_REG_1PPQ2   (0x3 << 4)
#define LTQ_ES_PRT_1P_REG_1PPQ2_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_PRT_1P_REG_1PPQ2_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ2) >> 4) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ2) | (((val) & 0x3) << 4))
/* Priority Queue 1 (3:2) */
#define LTQ_ES_PRT_1P_REG_1PPQ1   (0x3 << 2)
#define LTQ_ES_PRT_1P_REG_1PPQ1_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_PRT_1P_REG_1PPQ1_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ1) >> 2) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ1) | (((val) & 0x3) << 2))
/* Priority Queue 0 (1:0) */
#define LTQ_ES_PRT_1P_REG_1PPQ0   (0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ0_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_PRT_1P_REG_1PPQ0_GET(val)   ((((val) & LTQ_ES_PRT_1P_REG_1PPQ0) >> 0) & 0x3)
#define LTQ_ES_PRT_1P_REG_1PPQ0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRT_1P_REG_1PPQ0) | (((val) & 0x3) << 0))

/*******************************************************************************
 * Global Bucket Size Base counter
 ******************************************************************************/

/* Reserved (31:18) */
#define LTQ_ES_GBKT_SZBS_REG_REV   (0x3fff << 18)
#define LTQ_ES_GBKT_SZBS_REG_REV_GET(val)   ((((val) & LTQ_ES_GBKT_SZBS_REG_REV) >> 18) & 0x3fff)
/* Base[17:0] (17:0) */
#define LTQ_ES_GBKT_SZBS_REG_BASE17_0   (0x3ffff)
#define LTQ_ES_GBKT_SZBS_REG_BASE17_0_VAL(val)   (((val) & 0x3ffff) << 0)
#define LTQ_ES_GBKT_SZBS_REG_BASE17_0_GET(val)   ((((val) & LTQ_ES_GBKT_SZBS_REG_BASE17_0) >> 0) & 0x3ffff)
#define LTQ_ES_GBKT_SZBS_REG_BASE17_0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_GBKT_SZBS_REG_BASE17_0) | (((val) & 0x3ffff) << 0))

/*******************************************************************************
 * Global Bucket Size Extend Base Counter
 ******************************************************************************/

/* Reserved (31:18) */
#define LTQ_ES_GBKT_SZEBS_REG_REV   (0x3fff << 18)
#define LTQ_ES_GBKT_SZEBS_REG_REV_GET(val)   ((((val) & LTQ_ES_GBKT_SZEBS_REG_REV) >> 18) & 0x3fff)
/* Extend Base[17:0] (17:0) */
#define LTQ_ES_GBKT_SZEBS_REG_EBASE17_0   (0x3ffff)
#define LTQ_ES_GBKT_SZEBS_REG_EBASE17_0_VAL(val)   (((val) & 0x3ffff) << 0)
#define LTQ_ES_GBKT_SZEBS_REG_EBASE17_0_GET(val)   ((((val) & LTQ_ES_GBKT_SZEBS_REG_EBASE17_0) >> 0) & 0x3ffff)
#define LTQ_ES_GBKT_SZEBS_REG_EBASE17_0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_GBKT_SZEBS_REG_EBASE17_0) | (((val) & 0x3ffff) << 0))

/*******************************************************************************
 * Buffer Threshold Register
 ******************************************************************************/

/* Port Unfull Offset 3 (31:30) */
#define LTQ_ES_BF_TH_REG_PUO3   (0x3 << 30)
#define LTQ_ES_BF_TH_REG_PUO3_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_BF_TH_REG_PUO3_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PUO3) >> 30) & 0x3)
#define LTQ_ES_BF_TH_REG_PUO3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PUO3) | (((val) & 0x3) << 30))
/* Port Unfull Offset 2 (29:28) */
#define LTQ_ES_BF_TH_REG_PUO2   (0x3 << 28)
#define LTQ_ES_BF_TH_REG_PUO2_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_BF_TH_REG_PUO2_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PUO2) >> 28) & 0x3)
#define LTQ_ES_BF_TH_REG_PUO2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PUO2) | (((val) & 0x3) << 28))
/* Port Unfull Offset 1 (27:26) */
#define LTQ_ES_BF_TH_REG_PUO1   (0x3 << 26)
#define LTQ_ES_BF_TH_REG_PUO1_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_BF_TH_REG_PUO1_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PUO1) >> 26) & 0x3)
#define LTQ_ES_BF_TH_REG_PUO1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PUO1) | (((val) & 0x3) << 26))
/* Port Unfull Offset 0 (25:24) */
#define LTQ_ES_BF_TH_REG_PUO0   (0x3 << 24)
#define LTQ_ES_BF_TH_REG_PUO0_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_BF_TH_REG_PUO0_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PUO0) >> 24) & 0x3)
#define LTQ_ES_BF_TH_REG_PUO0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PUO0) | (((val) & 0x3) << 24))
/* Port Full Offset 3 (23:22) */
#define LTQ_ES_BF_TH_REG_PFO3   (0x3 << 22)
#define LTQ_ES_BF_TH_REG_PFO3_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_BF_TH_REG_PFO3_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PFO3) >> 22) & 0x3)
#define LTQ_ES_BF_TH_REG_PFO3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PFO3) | (((val) & 0x3) << 22))
/* Port Full Offset 2 (21:20) */
#define LTQ_ES_BF_TH_REG_PFO2   (0x3 << 20)
#define LTQ_ES_BF_TH_REG_PFO2_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_ES_BF_TH_REG_PFO2_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PFO2) >> 20) & 0x3)
#define LTQ_ES_BF_TH_REG_PFO2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PFO2) | (((val) & 0x3) << 20))
/* Port Full Offset 1 (19:18) */
#define LTQ_ES_BF_TH_REG_PFO1   (0x3 << 18)
#define LTQ_ES_BF_TH_REG_PFO1_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_BF_TH_REG_PFO1_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PFO1) >> 18) & 0x3)
#define LTQ_ES_BF_TH_REG_PFO1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PFO1) | (((val) & 0x3) << 18))
/* Port Full Offset 0 (17:16) */
#define LTQ_ES_BF_TH_REG_PFO0   (0x3 << 16)
#define LTQ_ES_BF_TH_REG_PFO0_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_BF_TH_REG_PFO0_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PFO0) >> 16) & 0x3)
#define LTQ_ES_BF_TH_REG_PFO0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PFO0) | (((val) & 0x3) << 16))
/* Reserved (15:14) */
#define LTQ_ES_BF_TH_REG_RES   (0x3 << 14)
#define LTQ_ES_BF_TH_REG_RES_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_RES) >> 14) & 0x3)
/* Total Low Add (13) */
#define LTQ_ES_BF_TH_REG_TLA   (0x1 << 13)
#define LTQ_ES_BF_TH_REG_TLA_VAL(val)   (((val) & 0x1) << 13)
#define LTQ_ES_BF_TH_REG_TLA_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_TLA) >> 13) & 0x1)
#define LTQ_ES_BF_TH_REG_TLA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_TLA) | (((val) & 0x1) << 13))
/* Total High Add (12) */
#define LTQ_ES_BF_TH_REG_THA   (0x1 << 12)
#define LTQ_ES_BF_TH_REG_THA_VAL(val)   (((val) & 0x1) << 12)
#define LTQ_ES_BF_TH_REG_THA_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_THA) >> 12) & 0x1)
#define LTQ_ES_BF_TH_REG_THA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_THA) | (((val) & 0x1) << 12))
/* Total Low Offset (11:10) */
#define LTQ_ES_BF_TH_REG_TLO   (0x3 << 10)
#define LTQ_ES_BF_TH_REG_TLO_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_BF_TH_REG_TLO_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_TLO) >> 10) & 0x3)
#define LTQ_ES_BF_TH_REG_TLO_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_TLO) | (((val) & 0x3) << 10))
/* Total High Offset (9:8) */
#define LTQ_ES_BF_TH_REG_THO   (0x3 << 8)
#define LTQ_ES_BF_TH_REG_THO_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_BF_TH_REG_THO_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_THO) >> 8) & 0x3)
#define LTQ_ES_BF_TH_REG_THO_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_THO) | (((val) & 0x3) << 8))
/* Port Unfull Add (7:4) */
#define LTQ_ES_BF_TH_REG_PUA   (0xf << 4)
#define LTQ_ES_BF_TH_REG_PUA_VAL(val)   (((val) & 0xf) << 4)
#define LTQ_ES_BF_TH_REG_PUA_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PUA) >> 4) & 0xf)
#define LTQ_ES_BF_TH_REG_PUA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PUA) | (((val) & 0xf) << 4))
/* Port Full Add (3:0) */
#define LTQ_ES_BF_TH_REG_PFA   (0xf)
#define LTQ_ES_BF_TH_REG_PFA_VAL(val)   (((val) & 0xf) << 0)
#define LTQ_ES_BF_TH_REG_PFA_GET(val)   ((((val) & LTQ_ES_BF_TH_REG_PFA) >> 0) & 0xf)
#define LTQ_ES_BF_TH_REG_PFA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_BF_TH_REG_PFA) | (((val) & 0xf) << 0))

/*******************************************************************************
 * PMAC Header Control Register
 ******************************************************************************/

/* Reserved (31:22) */
#define LTQ_ES_PMAC_HD_CTL_RES   (0x3ff << 22)
#define LTQ_ES_PMAC_HD_CTL_RES_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_RES) >> 22) & 0x3ff)
/* Remove Layer-2 Header from Packets Going from PMAC to DMA (21) */
#define LTQ_ES_PMAC_HD_CTL_RL2   (0x1 << 21)
#define LTQ_ES_PMAC_HD_CTL_RL2_VAL(val)   (((val) & 0x1) << 21)
#define LTQ_ES_PMAC_HD_CTL_RL2_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_RL2) >> 21) & 0x1)
#define LTQ_ES_PMAC_HD_CTL_RL2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_RL2) | (((val) & 0x1) << 21))
/* Remove CRC from Packets Going from PMAC to DMA (20) */
#define LTQ_ES_PMAC_HD_CTL_RC   (0x1 << 20)
#define LTQ_ES_PMAC_HD_CTL_RC_VAL(val)   (((val) & 0x1) << 20)
#define LTQ_ES_PMAC_HD_CTL_RC_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_RC) >> 20) & 0x1)
#define LTQ_ES_PMAC_HD_CTL_RC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_RC) | (((val) & 0x1) << 20))
/* Status Header for Packets from PMAC to DMA (19) */
#define LTQ_ES_PMAC_HD_CTL_AS   (0x1 << 19)
#define LTQ_ES_PMAC_HD_CTL_AS_VAL(val)   (((val) & 0x1) << 19)
#define LTQ_ES_PMAC_HD_CTL_AS_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_AS) >> 19) & 0x1)
#define LTQ_ES_PMAC_HD_CTL_AS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_AS) | (((val) & 0x1) << 19))
/* Add CRC for packets from DMA to PMAC (18) */
#define LTQ_ES_PMAC_HD_CTL_AC   (0x1 << 18)
#define LTQ_ES_PMAC_HD_CTL_AC_VAL(val)   (((val) & 0x1) << 18)
#define LTQ_ES_PMAC_HD_CTL_AC_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_AC) >> 18) & 0x1)
#define LTQ_ES_PMAC_HD_CTL_AC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_AC) | (((val) & 0x1) << 18))
/* Contains the length/type value to the added to packets from DMA to PMAC (17:2) */
#define LTQ_ES_PMAC_HD_CTL_TYPE_LEN   (0xffff << 2)
#define LTQ_ES_PMAC_HD_CTL_TYPE_LEN_VAL(val)   (((val) & 0xffff) << 2)
#define LTQ_ES_PMAC_HD_CTL_TYPE_LEN_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_TYPE_LEN) >> 2) & 0xffff)
#define LTQ_ES_PMAC_HD_CTL_TYPE_LEN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_TYPE_LEN) | (((val) & 0xffff) << 2))
/* Add TAG to Packets from DMA to PMAC (1) */
#define LTQ_ES_PMAC_HD_CTL_TAG   (0x1 << 1)
#define LTQ_ES_PMAC_HD_CTL_TAG_VAL(val)   (((val) & 0x1) << 1)
#define LTQ_ES_PMAC_HD_CTL_TAG_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_TAG) >> 1) & 0x1)
#define LTQ_ES_PMAC_HD_CTL_TAG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_TAG) | (((val) & 0x1) << 1))
/* ADD Header to Packets from DMA to PMAC (0) */
#define LTQ_ES_PMAC_HD_CTL_ADD   (0x1)
#define LTQ_ES_PMAC_HD_CTL_ADD_VAL(val)   (((val) & 0x1) << 0)
#define LTQ_ES_PMAC_HD_CTL_ADD_GET(val)   ((((val) & LTQ_ES_PMAC_HD_CTL_ADD) >> 0) & 0x1)
#define LTQ_ES_PMAC_HD_CTL_ADD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_HD_CTL_ADD) | (((val) & 0x1) << 0))

/*******************************************************************************
 * PMAC Source Address Register 1
 ******************************************************************************/

/* Source Address to be inserted as a part of the Ethernet header. (15:0) */
#define LTQ_ES_PMAC_SA1_SA_47_32   (0xffff)
#define LTQ_ES_PMAC_SA1_SA_47_32_VAL(val)   (((val) & 0xffff) << 0)
#define LTQ_ES_PMAC_SA1_SA_47_32_GET(val)   ((((val) & LTQ_ES_PMAC_SA1_SA_47_32) >> 0) & 0xffff)
#define LTQ_ES_PMAC_SA1_SA_47_32_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_SA1_SA_47_32) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * PMAC Source Address Register 2
 ******************************************************************************/

/* Source Address (31:0) */
#define LTQ_ES_PMAC_SA2_SA_31_0   (0xFFFFFFFFL)
#define LTQ_ES_PMAC_SA2_SA_31_0_VAL(val)   (((val) & 0xFFFFFFFFL) << 0)
#define LTQ_ES_PMAC_SA2_SA_31_0_GET(val)   ((((val) & LTQ_ES_PMAC_SA2_SA_31_0) >> 0) & 0xFFFFFFFFL)
#define LTQ_ES_PMAC_SA2_SA_31_0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_SA2_SA_31_0) | (((val) & 0xFFFFFFFFL) << 0))

/*******************************************************************************
 * PMAC Destination Address Register 1
 ******************************************************************************/

/* Destination Address (15:0) */
#define LTQ_ES_PMAC_DA1_DA_47_32   (0xffff)
#define LTQ_ES_PMAC_DA1_DA_47_32_VAL(val)   (((val) & 0xffff) << 0)
#define LTQ_ES_PMAC_DA1_DA_47_32_GET(val)   ((((val) & LTQ_ES_PMAC_DA1_DA_47_32) >> 0) & 0xffff)
#define LTQ_ES_PMAC_DA1_DA_47_32_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_DA1_DA_47_32) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * PMAC Destination Address Register 2
 ******************************************************************************/

/* Destination Address to be inserted as a part of the Ethernet header. (31:0) */
#define LTQ_ES_PMAC_DA2_DA_31_0   (0xFFFFFFFFL)
#define LTQ_ES_PMAC_DA2_DA_31_0_VAL(val)   (((val) & 0xFFFFFFFFL) << 0)
#define LTQ_ES_PMAC_DA2_DA_31_0_GET(val)   ((((val) & LTQ_ES_PMAC_DA2_DA_31_0) >> 0) & 0xFFFFFFFFL)
#define LTQ_ES_PMAC_DA2_DA_31_0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_DA2_DA_31_0) | (((val) & 0xFFFFFFFFL) << 0))

/*******************************************************************************
 * PMAC VLAN Register
 ******************************************************************************/

/* Priority to be inserted as a part of VLAN tag (15:13) */
#define LTQ_ES_PMAC_VLAN_PRI   (0x7 << 13)
#define LTQ_ES_PMAC_VLAN_PRI_VAL(val)   (((val) & 0x7) << 13)
#define LTQ_ES_PMAC_VLAN_PRI_GET(val)   ((((val) & LTQ_ES_PMAC_VLAN_PRI) >> 13) & 0x7)
#define LTQ_ES_PMAC_VLAN_PRI_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_VLAN_PRI) | (((val) & 0x7) << 13))
/* CFI bit to be inserted as a part of VLAN tag (12) */
#define LTQ_ES_PMAC_VLAN_CFI   (0x1 << 12)
#define LTQ_ES_PMAC_VLAN_CFI_VAL(val)   (((val) & 0x1) << 12)
#define LTQ_ES_PMAC_VLAN_CFI_GET(val)   ((((val) & LTQ_ES_PMAC_VLAN_CFI) >> 12) & 0x1)
#define LTQ_ES_PMAC_VLAN_CFI_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_VLAN_CFI) | (((val) & 0x1) << 12))
/* VLAN ID to be inserted as a part of VLAN tag (11:0) */
#define LTQ_ES_PMAC_VLAN_VLAN_ID   (0xfff)
#define LTQ_ES_PMAC_VLAN_VLAN_ID_VAL(val)   (((val) & 0xfff) << 0)
#define LTQ_ES_PMAC_VLAN_VLAN_ID_GET(val)   ((((val) & LTQ_ES_PMAC_VLAN_VLAN_ID) >> 0) & 0xfff)
#define LTQ_ES_PMAC_VLAN_VLAN_ID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_VLAN_VLAN ID) | (((val) & 0xfff) << 0))

/*******************************************************************************
 * PMAC TX IPG Counter Register
 ******************************************************************************/

/* IPG Counter (7:0) */
#define LTQ_ES_PMAC_TX_IPG_IPG_CNT   (0xff)
#define LTQ_ES_PMAC_TX_IPG_IPG_CNT_VAL(val)   (((val) & 0xff) << 0)
#define LTQ_ES_PMAC_TX_IPG_IPG_CNT_GET(val)   ((((val) & LTQ_ES_PMAC_TX_IPG_IPG_CNT) >> 0) & 0xff)
#define LTQ_ES_PMAC_TX_IPG_IPG_CNT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_TX_IPG_IPG_CNT) | (((val) & 0xff) << 0))

/*******************************************************************************
 * PMAC RX IPG Counter Register
 ******************************************************************************/

/* IPG Counter (7:0) */
#define LTQ_ES_PMAC_RX_IPG_IPG_CNT   (0xff)
#define LTQ_ES_PMAC_RX_IPG_IPG_CNT_VAL(val)   (((val) & 0xff) << 0)
#define LTQ_ES_PMAC_RX_IPG_IPG_CNT_GET(val)   ((((val) & LTQ_ES_PMAC_RX_IPG_IPG_CNT) >> 0) & 0xff)
#define LTQ_ES_PMAC_RX_IPG_IPG_CNT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PMAC_RX_IPG_IPG_CNT) | (((val) & 0xff) << 0))

/*******************************************************************************
 * Address Table Control 0 Register
 ******************************************************************************/

/* Address [31:0] (31:0) */
#define LTQ_ES_ADR_TB_CTL0_REG_ADDR31_0   (0xFFFFFFFFL)
#define LTQ_ES_ADR_TB_CTL0_REG_ADDR31_0_VAL(val)   (((val) & 0xFFFFFFFFL) << 0)
#define LTQ_ES_ADR_TB_CTL0_REG_ADDR31_0_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL0_REG_ADDR31_0) >> 0) & 0xFFFFFFFFL)
#define LTQ_ES_ADR_TB_CTL0_REG_ADDR31_0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL0_REG_ADDR31_0) | (((val) & 0xFFFFFFFFL) << 0))

/*******************************************************************************
 * Address Table Control 1 Register
 ******************************************************************************/

/* Port Map (22:20) */
#define LTQ_ES_ADR_TB_CTL1_REG_PMAP   (0x7 << 20)
#define LTQ_ES_ADR_TB_CTL1_REG_PMAP_VAL(val)   (((val) & 0x7) << 20)
#define LTQ_ES_ADR_TB_CTL1_REG_PMAP_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL1_REG_PMAP) >> 20) & 0x7)
#define LTQ_ES_ADR_TB_CTL1_REG_PMAP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL1_REG_PMAP) | (((val) & 0x7) << 20))
/* FID group (17:16) */
#define LTQ_ES_ADR_TB_CTL1_REG_FID   (0x3 << 16)
#define LTQ_ES_ADR_TB_CTL1_REG_FID_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_ADR_TB_CTL1_REG_FID_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL1_REG_FID) >> 16) & 0x3)
#define LTQ_ES_ADR_TB_CTL1_REG_FID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL1_REG_FID) | (((val) & 0x3) << 16))
/* Address [47:32] (15:0) */
#define LTQ_ES_ADR_TB_CTL1_REG_ADDR47_32   (0xffff)
#define LTQ_ES_ADR_TB_CTL1_REG_ADDR47_32_VAL(val)   (((val) & 0xffff) << 0)
#define LTQ_ES_ADR_TB_CTL1_REG_ADDR47_32_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL1_REG_ADDR47_32) >> 0) & 0xffff)
#define LTQ_ES_ADR_TB_CTL1_REG_ADDR47_32_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL1_REG_ADDR47_32) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * Address Table Control 2 Register
 ******************************************************************************/

/* Command (22:20) */
#define LTQ_ES_ADR_TB_CTL2_REG_CMD   (0x7 << 20)
#define LTQ_ES_ADR_TB_CTL2_REG_CMD_VAL(val)   (((val) & 0x7) << 20)
#define LTQ_ES_ADR_TB_CTL2_REG_CMD_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL2_REG_CMD) >> 20) & 0x7)
#define LTQ_ES_ADR_TB_CTL2_REG_CMD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL2_REG_CMD) | (((val) & 0x7) << 20))
/* Access Control (19:16) */
#define LTQ_ES_ADR_TB_CTL2_REG_AC   (0xf << 16)
#define LTQ_ES_ADR_TB_CTL2_REG_AC_VAL(val)   (((val) & 0xf) << 16)
#define LTQ_ES_ADR_TB_CTL2_REG_AC_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL2_REG_AC) >> 16) & 0xf)
#define LTQ_ES_ADR_TB_CTL2_REG_AC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL2_REG_AC) | (((val) & 0xf) << 16))
/* Info Type: Static address (12) */
#define LTQ_ES_ADR_TB_CTL2_REG_INFOT   (0x1 << 12)
#define LTQ_ES_ADR_TB_CTL2_REG_INFOT_VAL(val)   (((val) & 0x1) << 12)
#define LTQ_ES_ADR_TB_CTL2_REG_INFOT_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL2_REG_INFOT) >> 12) & 0x1)
#define LTQ_ES_ADR_TB_CTL2_REG_INFOT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL2_REG_INFOT) | (((val) & 0x1) << 12))
/* Info_Ctrl/Age Timer (10:0) */
#define LTQ_ES_ADR_TB_CTL2_REG_ITAT   (0x7ff)
#define LTQ_ES_ADR_TB_CTL2_REG_ITAT_VAL(val)   (((val) & 0x7ff) << 0)
#define LTQ_ES_ADR_TB_CTL2_REG_ITAT_GET(val)   ((((val) & LTQ_ES_ADR_TB_CTL2_REG_ITAT) >> 0) & 0x7ff)
#define LTQ_ES_ADR_TB_CTL2_REG_ITAT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_ADR_TB_CTL2_REG_ITAT) | (((val) & 0x7ff) << 0))

/*******************************************************************************
 * Address Table Status 0 Register
 ******************************************************************************/

/* Address [31:0] (31:0) */
#define LTQ_ES_ADR_TB_ST0_REG_ADDRS31_0   (0xFFFFFFFFL)
#define LTQ_ES_ADR_TB_ST0_REG_ADDRS31_0_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST0_REG_ADDRS31_0) >> 0) & 0xFFFFFFFFL)

/*******************************************************************************
 * Address Table Status 1 Register
 ******************************************************************************/

/* Port Map (22:20) */
#define LTQ_ES_ADR_TB_ST1_REG_PMAPS   (0x7 << 20)
#define LTQ_ES_ADR_TB_ST1_REG_PMAPS_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST1_REG_PMAPS) >> 20) & 0x7)
/* FID group (17:16) */
#define LTQ_ES_ADR_TB_ST1_REG_FIDS   (0x3 << 16)
#define LTQ_ES_ADR_TB_ST1_REG_FIDS_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST1_REG_FIDS) >> 16) & 0x3)
/* Address [47:32] (15:0) */
#define LTQ_ES_ADR_TB_ST1_REG_ADDRS47_32   (0xffff)
#define LTQ_ES_ADR_TB_ST1_REG_ADDRS47_32_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST1_REG_ADDRS47_32) >> 0) & 0xffff)

/*******************************************************************************
 * Address Table Status 2 Register
 ******************************************************************************/

/* Busy (31) */
#define LTQ_ES_ADR_TB_ST2_REG_BUSY   (0x1 << 31)
#define LTQ_ES_ADR_TB_ST2_REG_BUSY_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_BUSY) >> 31) & 0x1)
/* Result (30:28) */
#define LTQ_ES_ADR_TB_ST2_REG_RSLT   (0x7 << 28)
#define LTQ_ES_ADR_TB_ST2_REG_RSLT_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_RSLT) >> 28) & 0x7)
/* Command (22:20) */
#define LTQ_ES_ADR_TB_ST2_REG_CMD   (0x7 << 20)
#define LTQ_ES_ADR_TB_ST2_REG_CMD_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_CMD) >> 20) & 0x7)
/* Access Control (19:16) */
#define LTQ_ES_ADR_TB_ST2_REG_AC   (0xf << 16)
#define LTQ_ES_ADR_TB_ST2_REG_AC_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_AC) >> 16) & 0xf)
/* Bad Status (14) */
#define LTQ_ES_ADR_TB_ST2_REG_BAD   (0x1 << 14)
#define LTQ_ES_ADR_TB_ST2_REG_BAD_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_BAD) >> 14) & 0x1)
/* Occupy (13) */
#define LTQ_ES_ADR_TB_ST2_REG_OCP   (0x1 << 13)
#define LTQ_ES_ADR_TB_ST2_REG_OCP_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_OCP) >> 13) & 0x1)
/* Info Type: Static address (12) */
#define LTQ_ES_ADR_TB_ST2_REG_INFOTS   (0x1 << 12)
#define LTQ_ES_ADR_TB_ST2_REG_INFOTS_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_INFOTS) >> 12) & 0x1)
/* Info_Ctrl/Age Timer Status (10:0) */
#define LTQ_ES_ADR_TB_ST2_REG_ITATS   (0x7ff)
#define LTQ_ES_ADR_TB_ST2_REG_ITATS_GET(val)   ((((val) & LTQ_ES_ADR_TB_ST2_REG_ITATS) >> 0) & 0x7ff)

/*******************************************************************************
 * RMON Counter Control Register
 ******************************************************************************/

/* Reserved (31:12) */
#define LTQ_ES_RMON_CTL_REG_RES   (0xfffff << 12)
#define LTQ_ES_RMON_CTL_REG_RES_GET(val)   ((((val) & LTQ_ES_RMON_CTL_REG_RES) >> 12) & 0xfffff)
/* Busy/Access Start (11) */
#define LTQ_ES_RMON_CTL_REG_BAS   (0x1 << 11)
#define LTQ_ES_RMON_CTL_REG_BAS_VAL(val)   (((val) & 0x1) << 11)
#define LTQ_ES_RMON_CTL_REG_BAS_GET(val)   ((((val) & LTQ_ES_RMON_CTL_REG_BAS) >> 11) & 0x1)
#define LTQ_ES_RMON_CTL_REG_BAS_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RMON_CTL_REG_BAS) | (((val) & 0x1) << 11))
/* Command for access counter (10:9) */
#define LTQ_ES_RMON_CTL_REG_CAC   (0x3 << 9)
#define LTQ_ES_RMON_CTL_REG_CAC_VAL(val)   (((val) & 0x3) << 9)
#define LTQ_ES_RMON_CTL_REG_CAC_GET(val)   ((((val) & LTQ_ES_RMON_CTL_REG_CAC) >> 9) & 0x3)
#define LTQ_ES_RMON_CTL_REG_CAC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RMON_CTL_REG_CAC) | (((val) & 0x3) << 9))
/* Port (8:6) */
#define LTQ_ES_RMON_CTL_REG_PORTC   (0x7 << 6)
#define LTQ_ES_RMON_CTL_REG_PORTC_VAL(val)   (((val) & 0x7) << 6)
#define LTQ_ES_RMON_CTL_REG_PORTC_GET(val)   ((((val) & LTQ_ES_RMON_CTL_REG_PORTC) >> 6) & 0x7)
#define LTQ_ES_RMON_CTL_REG_PORTC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RMON_CTL_REG_PORTC) | (((val) & 0x7) << 6))
/* Counter Offset (5:0) */
#define LTQ_ES_RMON_CTL_REG_OFFSET   (0x3f)
#define LTQ_ES_RMON_CTL_REG_OFFSET_VAL(val)   (((val) & 0x3f) << 0)
#define LTQ_ES_RMON_CTL_REG_OFFSET_GET(val)   ((((val) & LTQ_ES_RMON_CTL_REG_OFFSET) >> 0) & 0x3f)
#define LTQ_ES_RMON_CTL_REG_OFFSET_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RMON_CTL_REG_OFFSET) | (((val) & 0x3f) << 0))

/*******************************************************************************
 * RMON Counter Status Register
 ******************************************************************************/

/* Counter [31:0] or Counter[63:32] for byte count (31:0) */
#define LTQ_ES_RMON_ST_REG_COUNTER   (0xFFFFFFFFL)
#define LTQ_ES_RMON_ST_REG_COUNTER_GET(val)   ((((val) & LTQ_ES_RMON_ST_REG_COUNTER) >> 0) & 0xFFFFFFFFL)

/*******************************************************************************
 * MDIO Indirect Access Control
 ******************************************************************************/

/* The Write Data to the MDIO register (31:16) */
#define LTQ_ES_MDIO_CTL_REG_WD   (0xffff << 16)
#define LTQ_ES_MDIO_CTL_REG_WD_VAL(val)   (((val) & 0xffff) << 16)
#define LTQ_ES_MDIO_CTL_REG_WD_GET(val)   ((((val) & LTQ_ES_MDIO_CTL_REG_WD) >> 16) & 0xffff)
#define LTQ_ES_MDIO_CTL_REG_WD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_MDIO_CTL_REG_WD) | (((val) & 0xffff) << 16))
/* Busy state (15) */
#define LTQ_ES_MDIO_CTL_REG_MBUSY   (0x1 << 15)
#define LTQ_ES_MDIO_CTL_REG_MBUSY_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_MDIO_CTL_REG_MBUSY_GET(val)   ((((val) & LTQ_ES_MDIO_CTL_REG_MBUSY) >> 15) & 0x1)
#define LTQ_ES_MDIO_CTL_REG_MBUSY_SET(reg,val) (reg) = ((reg & ~LTQ_ES_MDIO_CTL_REG_MBUSY) | (((val) & 0x1) << 15))
/* Reserved (14:12) */
#define LTQ_ES_MDIO_CTL_REG_RES   (0x7 << 12)
#define LTQ_ES_MDIO_CTL_REG_RES_GET(val)   ((((val) & LTQ_ES_MDIO_CTL_REG_RES) >> 12) & 0x7)
/* Operation Code (11:10) */
#define LTQ_ES_MDIO_CTL_REG_OP   (0x3 << 10)
#define LTQ_ES_MDIO_CTL_REG_OP_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_MDIO_CTL_REG_OP_GET(val)   ((((val) & LTQ_ES_MDIO_CTL_REG_OP) >> 10) & 0x3)
#define LTQ_ES_MDIO_CTL_REG_OP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_MDIO_CTL_REG_OP) | (((val) & 0x3) << 10))
/* PHY Address (9:5) */
#define LTQ_ES_MDIO_CTL_REG_PHYAD   (0x1f << 5)
#define LTQ_ES_MDIO_CTL_REG_PHYAD_VAL(val)   (((val) & 0x1f) << 5)
#define LTQ_ES_MDIO_CTL_REG_PHYAD_GET(val)   ((((val) & LTQ_ES_MDIO_CTL_REG_PHYAD) >> 5) & 0x1f)
#define LTQ_ES_MDIO_CTL_REG_PHYAD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_MDIO_CTL_REG_PHYAD) | (((val) & 0x1f) << 5))
/* Register Address (4:0) */
#define LTQ_ES_MDIO_CTL_REG_REGAD   (0x1f)
#define LTQ_ES_MDIO_CTL_REG_REGAD_VAL(val)   (((val) & 0x1f) << 0)
#define LTQ_ES_MDIO_CTL_REG_REGAD_GET(val)   ((((val) & LTQ_ES_MDIO_CTL_REG_REGAD) >> 0) & 0x1f)
#define LTQ_ES_MDIO_CTL_REG_REGAD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_MDIO_CTL_REG_REGAD) | (((val) & 0x1f) << 0))

/*******************************************************************************
 * MDIO Indirect Read Data
 ******************************************************************************/

/* Reserved (31:16) */
#define LTQ_ES_MDIO_DATA_REG_RES   (0xffff << 16)
#define LTQ_ES_MDIO_DATA_REG_RES_GET(val)   ((((val) & LTQ_ES_MDIO_DATA_REG_RES) >> 16) & 0xffff)
/* The Read Data (15:0) */
#define LTQ_ES_MDIO_DATA_REG_RD   (0xffff)
#define LTQ_ES_MDIO_DATA_REG_RD_GET(val)   ((((val) & LTQ_ES_MDIO_DATA_REG_RD) >> 0) & 0xffff)

/*******************************************************************************
 * Type Filter Action
 ******************************************************************************/

/* Destination Queue for Type Filter 7 (31:30) */
#define LTQ_ES_TP_FLT_ACT_REG_QATF7   (0x3 << 30)
#define LTQ_ES_TP_FLT_ACT_REG_QATF7_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_TP_FLT_ACT_REG_QATF7_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QATF7) >> 30) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QATF7_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QATF7) | (((val) & 0x3) << 30))
/* Destination Queue for Type Filter 6 (29:28) */
#define LTQ_ES_TP_FLT_ACT_REG_QATF6   (0x3 << 28)
#define LTQ_ES_TP_FLT_ACT_REG_QATF6_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_TP_FLT_ACT_REG_QATF6_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QATF6) >> 28) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QATF6_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QATF6) | (((val) & 0x3) << 28))
/* Destination Queue for Type Filter 5 (27:26) */
#define LTQ_ES_TP_FLT_ACT_REG_QTF5   (0x3 << 26)
#define LTQ_ES_TP_FLT_ACT_REG_QTF5_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_TP_FLT_ACT_REG_QTF5_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QTF5) >> 26) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QTF5_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QTF5) | (((val) & 0x3) << 26))
/* Destination Queue for Type Filter 4 (25:24) */
#define LTQ_ES_TP_FLT_ACT_REG_QTF4   (0x3 << 24)
#define LTQ_ES_TP_FLT_ACT_REG_QTF4_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_TP_FLT_ACT_REG_QTF4_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QTF4) >> 24) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QTF4_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QTF4) | (((val) & 0x3) << 24))
/* Destination Queue for Type Filter 3 (23:22) */
#define LTQ_ES_TP_FLT_ACT_REG_QTF3   (0x3 << 22)
#define LTQ_ES_TP_FLT_ACT_REG_QTF3_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_TP_FLT_ACT_REG_QTF3_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QTF3) >> 22) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QTF3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QTF3) | (((val) & 0x3) << 22))
/* Destination Queue for Type Filter 2 (21:20) */
#define LTQ_ES_TP_FLT_ACT_REG_QTF2   (0x3 << 20)
#define LTQ_ES_TP_FLT_ACT_REG_QTF2_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_ES_TP_FLT_ACT_REG_QTF2_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QTF2) >> 20) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QTF2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QTF2) | (((val) & 0x3) << 20))
/* Destination Queue for Type Filter 1 (19:18) */
#define LTQ_ES_TP_FLT_ACT_REG_QTF1   (0x3 << 18)
#define LTQ_ES_TP_FLT_ACT_REG_QTF1_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_TP_FLT_ACT_REG_QTF1_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QTF1) >> 18) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QTF1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QTF1) | (((val) & 0x3) << 18))
/* Destination Queue for Type Filter 0 (17:16) */
#define LTQ_ES_TP_FLT_ACT_REG_QTF0   (0x3 << 16)
#define LTQ_ES_TP_FLT_ACT_REG_QTF0_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_TP_FLT_ACT_REG_QTF0_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_QTF0) >> 16) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_QTF0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_QTF0) | (((val) & 0x3) << 16))
/* Action for Type Filter 7 (15:14) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF7   (0x3 << 14)
#define LTQ_ES_TP_FLT_ACT_REG_ATF7_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_TP_FLT_ACT_REG_ATF7_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF7) >> 14) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF7_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF7) | (((val) & 0x3) << 14))
/* Action for Type Filter 6 (13:12) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF6   (0x3 << 12)
#define LTQ_ES_TP_FLT_ACT_REG_ATF6_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_TP_FLT_ACT_REG_ATF6_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF6) >> 12) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF6_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF6) | (((val) & 0x3) << 12))
/* Action for Type Filter 5 (11:10) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF5   (0x3 << 10)
#define LTQ_ES_TP_FLT_ACT_REG_ATF5_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_TP_FLT_ACT_REG_ATF5_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF5) >> 10) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF5_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF5) | (((val) & 0x3) << 10))
/* Action for Type Filter 4 (9:8) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF4   (0x3 << 8)
#define LTQ_ES_TP_FLT_ACT_REG_ATF4_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_TP_FLT_ACT_REG_ATF4_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF4) >> 8) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF4_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF4) | (((val) & 0x3) << 8))
/* Action for Type Filter 3 (7:6) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF3   (0x3 << 6)
#define LTQ_ES_TP_FLT_ACT_REG_ATF3_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_TP_FLT_ACT_REG_ATF3_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF3) >> 6) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF3) | (((val) & 0x3) << 6))
/* Action for Type Filter 2 (5:4) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF2   (0x3 << 4)
#define LTQ_ES_TP_FLT_ACT_REG_ATF2_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_TP_FLT_ACT_REG_ATF2_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF2) >> 4) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF2) | (((val) & 0x3) << 4))
/* Action for Type Filter 1 (3:2) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF1   (0x3 << 2)
#define LTQ_ES_TP_FLT_ACT_REG_ATF1_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_TP_FLT_ACT_REG_ATF1_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF1) >> 2) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF1) | (((val) & 0x3) << 2))
/* Action for Type Filter 0 (1:0) */
#define LTQ_ES_TP_FLT_ACT_REG_ATF0   (0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF0_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_TP_FLT_ACT_REG_ATF0_GET(val)   ((((val) & LTQ_ES_TP_FLT_ACT_REG_ATF0) >> 0) & 0x3)
#define LTQ_ES_TP_FLT_ACT_REG_ATF0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT_ACT_REG_ATF0) | (((val) & 0x3) << 0))

/*******************************************************************************
 * Protocol Filter Action
 ******************************************************************************/

/* Action for Protocol Filter 7 (15:14) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF7   (0x3 << 14)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF7_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF7_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF7) >> 14) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF7_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF7) | (((val) & 0x3) << 14))
/* Action for Protocol Filter 6 (13:12) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF6   (0x3 << 12)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF6_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF6_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF6) >> 12) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF6_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF6) | (((val) & 0x3) << 12))
/* Action for Protocol Filter 5 (11:10) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF5   (0x3 << 10)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF5_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF5_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF5) >> 10) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF5_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF5) | (((val) & 0x3) << 10))
/* Action for Protocol Filter 4 (9:8) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF4   (0x3 << 8)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF4_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF4_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF4) >> 8) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF4_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF4) | (((val) & 0x3) << 8))
/* Action for Protocol Filter 3 (7:6) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF3   (0x3 << 6)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF3_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF3_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF3) >> 6) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF3) | (((val) & 0x3) << 6))
/* Action for Protocol Filter 2 (5:4) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF2   (0x3 << 4)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF2_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF2_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF2) >> 4) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF2) | (((val) & 0x3) << 4))
/* Action for Protocol Filter 1 (3:2) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF1   (0x3 << 2)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF1_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF1_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF1) >> 2) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF1) | (((val) & 0x3) << 2))
/* Action for Protocol Filter 0 (1:0) */
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF0   (0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF0_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF0_GET(val)   ((((val) & LTQ_ES_PRTCL_FLT_ACT_REG_APF0) >> 0) & 0x3)
#define LTQ_ES_PRTCL_FLT_ACT_REG_APF0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_FLT_ACT_REG_APF0) | (((val) & 0x3) << 0))

/*******************************************************************************
 * VLAN Filter 0
 ******************************************************************************/

/* Res (31:24) */
#define LTQ_ES_VLAN_FLT0_REG_RES   (0xff << 24)
#define LTQ_ES_VLAN_FLT0_REG_RES_VAL(val)   (((val) & 0xff) << 24)
#define LTQ_ES_VLAN_FLT0_REG_RES_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_RES) >> 24) & 0xff)
#define LTQ_ES_VLAN_FLT0_REG_RES_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_RES) | (((val) & 0xff) << 24))
/* FID (23:22) */
#define LTQ_ES_VLAN_FLT0_REG_FID   (0x3 << 22)
#define LTQ_ES_VLAN_FLT0_REG_FID_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_VLAN_FLT0_REG_FID_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_FID) >> 22) & 0x3)
#define LTQ_ES_VLAN_FLT0_REG_FID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_FID) | (((val) & 0x3) << 22))
/* Tagged Member (21:19) */
#define LTQ_ES_VLAN_FLT0_REG_TM   (0x7 << 19)
#define LTQ_ES_VLAN_FLT0_REG_TM_VAL(val)   (((val) & 0x7) << 19)
#define LTQ_ES_VLAN_FLT0_REG_TM_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_TM) >> 19) & 0x7)
#define LTQ_ES_VLAN_FLT0_REG_TM_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_TM) | (((val) & 0x7) << 19))
/* Member (18:16) */
#define LTQ_ES_VLAN_FLT0_REG_M   (0x7 << 16)
#define LTQ_ES_VLAN_FLT0_REG_M_VAL(val)   (((val) & 0x7) << 16)
#define LTQ_ES_VLAN_FLT0_REG_M_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_M) >> 16) & 0x7)
#define LTQ_ES_VLAN_FLT0_REG_M_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_M) | (((val) & 0x7) << 16))
/* VLAN_Valid (15) */
#define LTQ_ES_VLAN_FLT0_REG_VV   (0x1 << 15)
#define LTQ_ES_VLAN_FLT0_REG_VV_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_VLAN_FLT0_REG_VV_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_VV) >> 15) & 0x1)
#define LTQ_ES_VLAN_FLT0_REG_VV_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_VV) | (((val) & 0x1) << 15))
/* VLAN PRI (14:12) */
#define LTQ_ES_VLAN_FLT0_REG_VP   (0x7 << 12)
#define LTQ_ES_VLAN_FLT0_REG_VP_VAL(val)   (((val) & 0x7) << 12)
#define LTQ_ES_VLAN_FLT0_REG_VP_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_VP) >> 12) & 0x7)
#define LTQ_ES_VLAN_FLT0_REG_VP_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_VP) | (((val) & 0x7) << 12))
/* VID (11:0) */
#define LTQ_ES_VLAN_FLT0_REG_VID   (0xfff)
#define LTQ_ES_VLAN_FLT0_REG_VID_VAL(val)   (((val) & 0xfff) << 0)
#define LTQ_ES_VLAN_FLT0_REG_VID_GET(val)   ((((val) & LTQ_ES_VLAN_FLT0_REG_VID) >> 0) & 0xfff)
#define LTQ_ES_VLAN_FLT0_REG_VID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_VLAN_FLT0_REG_VID) | (((val) & 0xfff) << 0))

/*******************************************************************************
 * Type Filter 10
 ******************************************************************************/

/* Value 1 Compared with Ether-Type (31:16) */
#define LTQ_ES_TP_FLT10_REG_VCET1   (0xffff << 16)
#define LTQ_ES_TP_FLT10_REG_VCET1_VAL(val)   (((val) & 0xffff) << 16)
#define LTQ_ES_TP_FLT10_REG_VCET1_GET(val)   ((((val) & LTQ_ES_TP_FLT10_REG_VCET1) >> 16) & 0xffff)
#define LTQ_ES_TP_FLT10_REG_VCET1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT10_REG_VCET1) | (((val) & 0xffff) << 16))
/* Value 0 Compared with Ether-Type (15:0) */
#define LTQ_ES_TP_FLT10_REG_VCET0   (0xffff)
#define LTQ_ES_TP_FLT10_REG_VCET0_VAL(val)   (((val) & 0xffff) << 0)
#define LTQ_ES_TP_FLT10_REG_VCET0_GET(val)   ((((val) & LTQ_ES_TP_FLT10_REG_VCET0) >> 0) & 0xffff)
#define LTQ_ES_TP_FLT10_REG_VCET0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TP_FLT10_REG_VCET0) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * DiffServMapping 0
 ******************************************************************************/

/* Priority Queue F (31:30) */
#define LTQ_ES_DFSRV_MAP0_REG_PQF   (0x3 << 30)
#define LTQ_ES_DFSRV_MAP0_REG_PQF_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_DFSRV_MAP0_REG_PQF_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQF) >> 30) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQF_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQF) | (((val) & 0x3) << 30))
/* Priority Queue E (29:28) */
#define LTQ_ES_DFSRV_MAP0_REG_PQE   (0x3 << 28)
#define LTQ_ES_DFSRV_MAP0_REG_PQE_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_DFSRV_MAP0_REG_PQE_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQE) >> 28) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQE_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQE) | (((val) & 0x3) << 28))
/* Priority Queue D (27:26) */
#define LTQ_ES_DFSRV_MAP0_REG_PQD   (0x3 << 26)
#define LTQ_ES_DFSRV_MAP0_REG_PQD_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_DFSRV_MAP0_REG_PQD_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQD) >> 26) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQD_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQD) | (((val) & 0x3) << 26))
/* Priority Queue C (25:24) */
#define LTQ_ES_DFSRV_MAP0_REG_PQC   (0x3 << 24)
#define LTQ_ES_DFSRV_MAP0_REG_PQC_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_DFSRV_MAP0_REG_PQC_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQC) >> 24) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQC_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQC) | (((val) & 0x3) << 24))
/* Priority Queue B (23:22) */
#define LTQ_ES_DFSRV_MAP0_REG_PQB   (0x3 << 22)
#define LTQ_ES_DFSRV_MAP0_REG_PQB_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_DFSRV_MAP0_REG_PQB_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQB) >> 22) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQB_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQB) | (((val) & 0x3) << 22))
/* Priority Queue A (21:20) */
#define LTQ_ES_DFSRV_MAP0_REG_PQA   (0x3 << 20)
#define LTQ_ES_DFSRV_MAP0_REG_PQA_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_ES_DFSRV_MAP0_REG_PQA_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQA) >> 20) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQA_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQA) | (((val) & 0x3) << 20))
/* Priority Queue 9 (19:18) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ9   (0x3 << 18)
#define LTQ_ES_DFSRV_MAP0_REG_PQ9_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_DFSRV_MAP0_REG_PQ9_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ9) >> 18) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ9_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ9) | (((val) & 0x3) << 18))
/* Priority Queue 8 (17:16) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ8   (0x3 << 16)
#define LTQ_ES_DFSRV_MAP0_REG_PQ8_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_DFSRV_MAP0_REG_PQ8_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ8) >> 16) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ8_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ8) | (((val) & 0x3) << 16))
/* Priority Queue 7 (15:14) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ7   (0x3 << 14)
#define LTQ_ES_DFSRV_MAP0_REG_PQ7_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_DFSRV_MAP0_REG_PQ7_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ7) >> 14) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ7_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ7) | (((val) & 0x3) << 14))
/* Priority Queue 6 (13:12) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ6   (0x3 << 12)
#define LTQ_ES_DFSRV_MAP0_REG_PQ6_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_DFSRV_MAP0_REG_PQ6_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ6) >> 12) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ6_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ6) | (((val) & 0x3) << 12))
/* Priority Queue 5 (11:10) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ5   (0x3 << 10)
#define LTQ_ES_DFSRV_MAP0_REG_PQ5_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_DFSRV_MAP0_REG_PQ5_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ5) >> 10) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ5_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ5) | (((val) & 0x3) << 10))
/* Priority Queue 4 (9:8) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ4   (0x3 << 8)
#define LTQ_ES_DFSRV_MAP0_REG_PQ4_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_DFSRV_MAP0_REG_PQ4_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ4) >> 8) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ4_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ4) | (((val) & 0x3) << 8))
/* Priority Queue 3 (7:6) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ3   (0x3 << 6)
#define LTQ_ES_DFSRV_MAP0_REG_PQ3_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_DFSRV_MAP0_REG_PQ3_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ3) >> 6) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ3) | (((val) & 0x3) << 6))
/* Priority Queue 2 (5:4) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ2   (0x3 << 4)
#define LTQ_ES_DFSRV_MAP0_REG_PQ2_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_DFSRV_MAP0_REG_PQ2_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ2) >> 4) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ2) | (((val) & 0x3) << 4))
/* Priority Queue 1 (3:2) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ1   (0x3 << 2)
#define LTQ_ES_DFSRV_MAP0_REG_PQ1_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_DFSRV_MAP0_REG_PQ1_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ1) >> 2) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ1) | (((val) & 0x3) << 2))
/* Priority Queue 0 (1:0) */
#define LTQ_ES_DFSRV_MAP0_REG_PQ0   (0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ0_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_DFSRV_MAP0_REG_PQ0_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP0_REG_PQ0) >> 0) & 0x3)
#define LTQ_ES_DFSRV_MAP0_REG_PQ0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP0_REG_PQ0) | (((val) & 0x3) << 0))

/*******************************************************************************
 * DiffServMapping 1
 ******************************************************************************/

/* Priority Queue 1F (31:30) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ1F   (0x3 << 30)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1F_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1F_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ1F) >> 30) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1F_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ1F) | (((val) & 0x3) << 30))
/* Priority Queue 1E (29:28) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ1E   (0x3 << 28)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1E_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1E_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ1E) >> 28) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1E_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ1E) | (((val) & 0x3) << 28))
/* Priority Queue 1D (27:26) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ1D   (0x3 << 26)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1D_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1D_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ1D) >> 26) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1D_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ1D) | (((val) & 0x3) << 26))
/* Priority Queue 1C (25:24) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ1C   (0x3 << 24)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1C_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1C_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ1C) >> 24) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1C_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ1C) | (((val) & 0x3) << 24))
/* Priority Queue 1B (23:22) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ1B   (0x3 << 22)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1B_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1B_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ1B) >> 22) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1B_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ1B) | (((val) & 0x3) << 22))
/* Priority Queue 1A (21:20) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ1A   (0x3 << 20)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1A_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1A_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ1A) >> 20) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ1A_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ1A) | (((val) & 0x3) << 20))
/* Priority Queue 19 (19:18) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ19   (0x3 << 18)
#define LTQ_ES_DFSRV_MAP1_REG_PQ19_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_DFSRV_MAP1_REG_PQ19_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ19) >> 18) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ19_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ19) | (((val) & 0x3) << 18))
/* Priority Queue 18 (17:16) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ18   (0x3 << 16)
#define LTQ_ES_DFSRV_MAP1_REG_PQ18_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_DFSRV_MAP1_REG_PQ18_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ18) >> 16) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ18_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ18) | (((val) & 0x3) << 16))
/* Priority Queue 17 (15:14) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ17   (0x3 << 14)
#define LTQ_ES_DFSRV_MAP1_REG_PQ17_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_DFSRV_MAP1_REG_PQ17_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ17) >> 14) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ17_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ17) | (((val) & 0x3) << 14))
/* Priority Queue 16 (13:12) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ16   (0x3 << 12)
#define LTQ_ES_DFSRV_MAP1_REG_PQ16_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_DFSRV_MAP1_REG_PQ16_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ16) >> 12) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ16_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ16) | (((val) & 0x3) << 12))
/* Priority Queue 15 (11:10) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ15   (0x3 << 10)
#define LTQ_ES_DFSRV_MAP1_REG_PQ15_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_DFSRV_MAP1_REG_PQ15_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ15) >> 10) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ15_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ15) | (((val) & 0x3) << 10))
/* Priority Queue 14 (9:8) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ14   (0x3 << 8)
#define LTQ_ES_DFSRV_MAP1_REG_PQ14_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_DFSRV_MAP1_REG_PQ14_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ14) >> 8) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ14_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ14) | (((val) & 0x3) << 8))
/* Priority Queue 13 (7:6) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ13   (0x3 << 6)
#define LTQ_ES_DFSRV_MAP1_REG_PQ13_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_DFSRV_MAP1_REG_PQ13_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ13) >> 6) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ13_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ13) | (((val) & 0x3) << 6))
/* Priority Queue 12 (5:4) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ12   (0x3 << 4)
#define LTQ_ES_DFSRV_MAP1_REG_PQ12_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_DFSRV_MAP1_REG_PQ12_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ12) >> 4) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ12_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ12) | (((val) & 0x3) << 4))
/* Priority Queue 11 (3:2) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ11   (0x3 << 2)
#define LTQ_ES_DFSRV_MAP1_REG_PQ11_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_DFSRV_MAP1_REG_PQ11_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ11) >> 2) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ11_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ11) | (((val) & 0x3) << 2))
/* Priority Queue 10 (1:0) */
#define LTQ_ES_DFSRV_MAP1_REG_PQ10   (0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ10_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_DFSRV_MAP1_REG_PQ10_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP1_REG_PQ10) >> 0) & 0x3)
#define LTQ_ES_DFSRV_MAP1_REG_PQ10_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP1_REG_PQ10) | (((val) & 0x3) << 0))

/*******************************************************************************
 * DiffServMapping 2
 ******************************************************************************/

/* Priority Queue 2F (31:30) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ2F   (0x3 << 30)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2F_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2F_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ2F) >> 30) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2F_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ2F) | (((val) & 0x3) << 30))
/* Priority Queue 2E (29:28) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ2E   (0x3 << 28)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2E_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2E_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ2E) >> 28) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2E_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ2E) | (((val) & 0x3) << 28))
/* Priority Queue 2D (27:26) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ2D   (0x3 << 26)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2D_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2D_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ2D) >> 26) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2D_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ2D) | (((val) & 0x3) << 26))
/* Priority Queue 2C (25:24) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ2C   (0x3 << 24)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2C_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2C_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ2C) >> 24) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2C_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ2C) | (((val) & 0x3) << 24))
/* Priority Queue 2B (23:22) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ2B   (0x3 << 22)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2B_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2B_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ2B) >> 22) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2B_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ2B) | (((val) & 0x3) << 22))
/* Priority Queue 2A (21:20) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ2A   (0x3 << 20)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2A_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2A_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ2A) >> 20) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ2A_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ2A) | (((val) & 0x3) << 20))
/* Priority Queue 29 (19:18) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ29   (0x3 << 18)
#define LTQ_ES_DFSRV_MAP2_REG_PQ29_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_DFSRV_MAP2_REG_PQ29_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ29) >> 18) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ29_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ29) | (((val) & 0x3) << 18))
/* Priority Queue 28 (17:16) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ28   (0x3 << 16)
#define LTQ_ES_DFSRV_MAP2_REG_PQ28_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_DFSRV_MAP2_REG_PQ28_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ28) >> 16) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ28_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ28) | (((val) & 0x3) << 16))
/* Priority Queue 27 (15:14) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ27   (0x3 << 14)
#define LTQ_ES_DFSRV_MAP2_REG_PQ27_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_DFSRV_MAP2_REG_PQ27_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ27) >> 14) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ27_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ27) | (((val) & 0x3) << 14))
/* Priority Queue 26 (13:12) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ26   (0x3 << 12)
#define LTQ_ES_DFSRV_MAP2_REG_PQ26_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_DFSRV_MAP2_REG_PQ26_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ26) >> 12) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ26_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ26) | (((val) & 0x3) << 12))
/* Priority Queue 25 (11:10) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ25   (0x3 << 10)
#define LTQ_ES_DFSRV_MAP2_REG_PQ25_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_DFSRV_MAP2_REG_PQ25_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ25) >> 10) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ25_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ25) | (((val) & 0x3) << 10))
/* Priority Queue 24 (9:8) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ24   (0x3 << 8)
#define LTQ_ES_DFSRV_MAP2_REG_PQ24_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_DFSRV_MAP2_REG_PQ24_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ24) >> 8) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ24_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ24) | (((val) & 0x3) << 8))
/* Priority Queue 23 (7:6) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ23   (0x3 << 6)
#define LTQ_ES_DFSRV_MAP2_REG_PQ23_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_DFSRV_MAP2_REG_PQ23_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ23) >> 6) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ23_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ23) | (((val) & 0x3) << 6))
/* Priority Queue 22 (5:4) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ22   (0x3 << 4)
#define LTQ_ES_DFSRV_MAP2_REG_PQ22_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_DFSRV_MAP2_REG_PQ22_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ22) >> 4) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ22_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ22) | (((val) & 0x3) << 4))
/* Priority Queue 21 (3:2) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ21   (0x3 << 2)
#define LTQ_ES_DFSRV_MAP2_REG_PQ21_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_DFSRV_MAP2_REG_PQ21_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ21) >> 2) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ21_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ21) | (((val) & 0x3) << 2))
/* Priority Queue 20 (1:0) */
#define LTQ_ES_DFSRV_MAP2_REG_PQ20   (0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ20_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_DFSRV_MAP2_REG_PQ20_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP2_REG_PQ20) >> 0) & 0x3)
#define LTQ_ES_DFSRV_MAP2_REG_PQ20_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP2_REG_PQ20) | (((val) & 0x3) << 0))

/*******************************************************************************
 * DiffServMapping 3
 ******************************************************************************/

/* Priority Queue 3F (31:30) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ3F   (0x3 << 30)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3F_VAL(val)   (((val) & 0x3) << 30)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3F_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ3F) >> 30) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3F_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ3F) | (((val) & 0x3) << 30))
/* Priority Queue 3E (29:28) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ3E   (0x3 << 28)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3E_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3E_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ3E) >> 28) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3E_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ3E) | (((val) & 0x3) << 28))
/* Priority Queue 3D (27:26) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ3D   (0x3 << 26)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3D_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3D_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ3D) >> 26) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3D_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ3D) | (((val) & 0x3) << 26))
/* Priority Queue 3C (25:24) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ3C   (0x3 << 24)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3C_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3C_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ3C) >> 24) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3C_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ3C) | (((val) & 0x3) << 24))
/* Priority Queue 3B (23:22) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ3B   (0x3 << 22)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3B_VAL(val)   (((val) & 0x3) << 22)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3B_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ3B) >> 22) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3B_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ3B) | (((val) & 0x3) << 22))
/* Priority Queue 3A (21:20) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ3A   (0x3 << 20)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3A_VAL(val)   (((val) & 0x3) << 20)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3A_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ3A) >> 20) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ3A_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ3A) | (((val) & 0x3) << 20))
/* Priority Queue 39 (19:18) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ39   (0x3 << 18)
#define LTQ_ES_DFSRV_MAP3_REG_PQ39_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_DFSRV_MAP3_REG_PQ39_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ39) >> 18) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ39_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ39) | (((val) & 0x3) << 18))
/* Priority Queue 38 (17:16) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ38   (0x3 << 16)
#define LTQ_ES_DFSRV_MAP3_REG_PQ38_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_DFSRV_MAP3_REG_PQ38_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ38) >> 16) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ38_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ38) | (((val) & 0x3) << 16))
/* Priority Queue 37 (15:14) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ37   (0x3 << 14)
#define LTQ_ES_DFSRV_MAP3_REG_PQ37_VAL(val)   (((val) & 0x3) << 14)
#define LTQ_ES_DFSRV_MAP3_REG_PQ37_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ37) >> 14) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ37_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ37) | (((val) & 0x3) << 14))
/* Priority Queue 36 (13:12) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ36   (0x3 << 12)
#define LTQ_ES_DFSRV_MAP3_REG_PQ36_VAL(val)   (((val) & 0x3) << 12)
#define LTQ_ES_DFSRV_MAP3_REG_PQ36_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ36) >> 12) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ36_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ36) | (((val) & 0x3) << 12))
/* Priority Queue 35 (11:10) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ35   (0x3 << 10)
#define LTQ_ES_DFSRV_MAP3_REG_PQ35_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_DFSRV_MAP3_REG_PQ35_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ35) >> 10) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ35_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ35) | (((val) & 0x3) << 10))
/* Priority Queue 34 (9:8) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ34   (0x3 << 8)
#define LTQ_ES_DFSRV_MAP3_REG_PQ34_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_DFSRV_MAP3_REG_PQ34_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ34) >> 8) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ34_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ34) | (((val) & 0x3) << 8))
/* Priority Queue 33 (7:6) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ33   (0x3 << 6)
#define LTQ_ES_DFSRV_MAP3_REG_PQ33_VAL(val)   (((val) & 0x3) << 6)
#define LTQ_ES_DFSRV_MAP3_REG_PQ33_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ33) >> 6) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ33_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ33) | (((val) & 0x3) << 6))
/* Priority Queue 32 (5:4) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ32   (0x3 << 4)
#define LTQ_ES_DFSRV_MAP3_REG_PQ32_VAL(val)   (((val) & 0x3) << 4)
#define LTQ_ES_DFSRV_MAP3_REG_PQ32_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ32) >> 4) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ32_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ32) | (((val) & 0x3) << 4))
/* Priority Queue 31 (3:2) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ31   (0x3 << 2)
#define LTQ_ES_DFSRV_MAP3_REG_PQ31_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_DFSRV_MAP3_REG_PQ31_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ31) >> 2) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ31_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ31) | (((val) & 0x3) << 2))
/* Priority Queue 30 (1:0) */
#define LTQ_ES_DFSRV_MAP3_REG_PQ30   (0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ30_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_DFSRV_MAP3_REG_PQ30_GET(val)   ((((val) & LTQ_ES_DFSRV_MAP3_REG_PQ30) >> 0) & 0x3)
#define LTQ_ES_DFSRV_MAP3_REG_PQ30_SET(reg,val) (reg) = ((reg & ~LTQ_ES_DFSRV_MAP3_REG_PQ30) | (((val) & 0x3) << 0))

/*******************************************************************************
 * TCP/UDP Port Filter 0
 ******************************************************************************/

/* Reserved (31:30) */
#define LTQ_ES_TCP_PF0_REG_RES   (0x3 << 30)
#define LTQ_ES_TCP_PF0_REG_RES_GET(val)   ((((val) & LTQ_ES_TCP_PF0_REG_RES) >> 30) & 0x3)
/* Action for TCP/UDP Port Filter 0 (29:28) */
#define LTQ_ES_TCP_PF0_REG_ATUF0   (0x3 << 28)
#define LTQ_ES_TCP_PF0_REG_ATUF0_VAL(val)   (((val) & 0x3) << 28)
#define LTQ_ES_TCP_PF0_REG_ATUF0_GET(val)   ((((val) & LTQ_ES_TCP_PF0_REG_ATUF0) >> 28) & 0x3)
#define LTQ_ES_TCP_PF0_REG_ATUF0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TCP_PF0_REG_ATUF0) | (((val) & 0x3) << 28))
/* TCP/UDP PRI for TCP/UDP Port Filter 0 (27:26) */
#define LTQ_ES_TCP_PF0_REG_TUPF0   (0x3 << 26)
#define LTQ_ES_TCP_PF0_REG_TUPF0_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_TCP_PF0_REG_TUPF0_GET(val)   ((((val) & LTQ_ES_TCP_PF0_REG_TUPF0) >> 26) & 0x3)
#define LTQ_ES_TCP_PF0_REG_TUPF0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TCP_PF0_REG_TUPF0) | (((val) & 0x3) << 26))
/* Compare TCP/UDP Source Port or Destination Port (25:24) */
#define LTQ_ES_TCP_PF0_REG_COMP0   (0x3 << 24)
#define LTQ_ES_TCP_PF0_REG_COMP0_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_TCP_PF0_REG_COMP0_GET(val)   ((((val) & LTQ_ES_TCP_PF0_REG_COMP0) >> 24) & 0x3)
#define LTQ_ES_TCP_PF0_REG_COMP0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TCP_PF0_REG_COMP0) | (((val) & 0x3) << 24))
/* Port Range in TCP/UDP (23:16) */
#define LTQ_ES_TCP_PF0_REG_PRANGE0   (0xff << 16)
#define LTQ_ES_TCP_PF0_REG_PRANGE0_VAL(val)   (((val) & 0xff) << 16)
#define LTQ_ES_TCP_PF0_REG_PRANGE0_GET(val)   ((((val) & LTQ_ES_TCP_PF0_REG_PRANGE0) >> 16) & 0xff)
#define LTQ_ES_TCP_PF0_REG_PRANGE0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TCP_PF0_REG_PRANGE0) | (((val) & 0xff) << 16))
/* Base Port number 0 (15:0) */
#define LTQ_ES_TCP_PF0_REG_BASEPT0   (0xffff)
#define LTQ_ES_TCP_PF0_REG_BASEPT0_VAL(val)   (((val) & 0xffff) << 0)
#define LTQ_ES_TCP_PF0_REG_BASEPT0_GET(val)   ((((val) & LTQ_ES_TCP_PF0_REG_BASEPT0) >> 0) & 0xffff)
#define LTQ_ES_TCP_PF0_REG_BASEPT0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_TCP_PF0_REG_BASEPT0) | (((val) & 0xffff) << 0))

/*******************************************************************************
 * Reserved DA(0180C2000003~0180C2000000) control register
 ******************************************************************************/

/* Valid bit for 0180C2000003 (31) */
#define LTQ_ES_RA_03_00_REG_RA03_VALID   (0x1 << 31)
#define LTQ_ES_RA_03_00_REG_RA03_VALID_VAL(val)   (((val) & 0x1) << 31)
#define LTQ_ES_RA_03_00_REG_RA03_VALID_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA03_VALID) >> 31) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA03_VALID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA03_VALID) | (((val) & 0x1) << 31))
/* Span bit for 0180C2000003 (30) */
#define LTQ_ES_RA_03_00_REG_RA03_SPAN   (0x1 << 30)
#define LTQ_ES_RA_03_00_REG_RA03_SPAN_VAL(val)   (((val) & 0x1) << 30)
#define LTQ_ES_RA_03_00_REG_RA03_SPAN_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA03_SPAN) >> 30) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA03_SPAN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA03_SPAN) | (((val) & 0x1) << 30))
/* Management bit for 0180C2000003 (29) */
#define LTQ_ES_RA_03_00_REG_RA03_MG   (0x1 << 29)
#define LTQ_ES_RA_03_00_REG_RA03_MG_VAL(val)   (((val) & 0x1) << 29)
#define LTQ_ES_RA_03_00_REG_RA03_MG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA03_MG) >> 29) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA03_MG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA03_MG) | (((val) & 0x1) << 29))
/* Cross_VLAN bit for 0180C2000003 (28) */
#define LTQ_ES_RA_03_00_REG_RA03_CV   (0x1 << 28)
#define LTQ_ES_RA_03_00_REG_RA03_CV_VAL(val)   (((val) & 0x1) << 28)
#define LTQ_ES_RA_03_00_REG_RA03_CV_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA03_CV) >> 28) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA03_CV_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA03_CV) | (((val) & 0x1) << 28))
/* TXTAG bit for 0180C2000003 (27:26) */
#define LTQ_ES_RA_03_00_REG_RA03_TXTAG   (0x3 << 26)
#define LTQ_ES_RA_03_00_REG_RA03_TXTAG_VAL(val)   (((val) & 0x3) << 26)
#define LTQ_ES_RA_03_00_REG_RA03_TXTAG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA03_TXTAG) >> 26) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA03_TXTAG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA03_TXTAG) | (((val) & 0x3) << 26))
/* Action bit for 0180C2000003 (25:24) */
#define LTQ_ES_RA_03_00_REG_RA03_ACT   (0x3 << 24)
#define LTQ_ES_RA_03_00_REG_RA03_ACT_VAL(val)   (((val) & 0x3) << 24)
#define LTQ_ES_RA_03_00_REG_RA03_ACT_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA03_ACT) >> 24) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA03_ACT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA03_ACT) | (((val) & 0x3) << 24))
/* Valid bit for 0180C2000002 (23) */
#define LTQ_ES_RA_03_00_REG_RA02_VALID   (0x1 << 23)
#define LTQ_ES_RA_03_00_REG_RA02_VALID_VAL(val)   (((val) & 0x1) << 23)
#define LTQ_ES_RA_03_00_REG_RA02_VALID_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA02_VALID) >> 23) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA02_VALID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA02_VALID) | (((val) & 0x1) << 23))
/* Span bit for 0180C2000002 (22) */
#define LTQ_ES_RA_03_00_REG_RA02_SPAN   (0x1 << 22)
#define LTQ_ES_RA_03_00_REG_RA02_SPAN_VAL(val)   (((val) & 0x1) << 22)
#define LTQ_ES_RA_03_00_REG_RA02_SPAN_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA02_SPAN) >> 22) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA02_SPAN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA02_SPAN) | (((val) & 0x1) << 22))
/* Management bit for 0180C2000002 (21) */
#define LTQ_ES_RA_03_00_REG_RA02_MG   (0x1 << 21)
#define LTQ_ES_RA_03_00_REG_RA02_MG_VAL(val)   (((val) & 0x1) << 21)
#define LTQ_ES_RA_03_00_REG_RA02_MG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA02_MG) >> 21) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA02_MG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA02_MG) | (((val) & 0x1) << 21))
/* Cross_VLAN bit for 0180C2000002 (20) */
#define LTQ_ES_RA_03_00_REG_RA02_CV   (0x1 << 20)
#define LTQ_ES_RA_03_00_REG_RA02_CV_VAL(val)   (((val) & 0x1) << 20)
#define LTQ_ES_RA_03_00_REG_RA02_CV_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA02_CV) >> 20) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA02_CV_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA02_CV) | (((val) & 0x1) << 20))
/* TXTAG bit for 0180C2000002 (19:18) */
#define LTQ_ES_RA_03_00_REG_RA02_TXTAG   (0x3 << 18)
#define LTQ_ES_RA_03_00_REG_RA02_TXTAG_VAL(val)   (((val) & 0x3) << 18)
#define LTQ_ES_RA_03_00_REG_RA02_TXTAG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA02_TXTAG) >> 18) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA02_TXTAG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA02_TXTAG) | (((val) & 0x3) << 18))
/* Action bit for 0180C2000002 (17:16) */
#define LTQ_ES_RA_03_00_REG_RA02_ACT   (0x3 << 16)
#define LTQ_ES_RA_03_00_REG_RA02_ACT_VAL(val)   (((val) & 0x3) << 16)
#define LTQ_ES_RA_03_00_REG_RA02_ACT_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA02_ACT) >> 16) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA02_ACT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA02_ACT) | (((val) & 0x3) << 16))
/* Valid bit for 0180C2000001 (15) */
#define LTQ_ES_RA_03_00_REG_RA01_VALID   (0x1 << 15)
#define LTQ_ES_RA_03_00_REG_RA01_VALID_VAL(val)   (((val) & 0x1) << 15)
#define LTQ_ES_RA_03_00_REG_RA01_VALID_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA01_VALID) >> 15) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA01_VALID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA01_VALID) | (((val) & 0x1) << 15))
/* Span bit for 0180C2000001 (14) */
#define LTQ_ES_RA_03_00_REG_RA01_SPAN   (0x1 << 14)
#define LTQ_ES_RA_03_00_REG_RA01_SPAN_VAL(val)   (((val) & 0x1) << 14)
#define LTQ_ES_RA_03_00_REG_RA01_SPAN_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA01_SPAN) >> 14) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA01_SPAN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA01_SPAN) | (((val) & 0x1) << 14))
/* Management bit for 0180C2000001 (13) */
#define LTQ_ES_RA_03_00_REG_RA01_MG   (0x1 << 13)
#define LTQ_ES_RA_03_00_REG_RA01_MG_VAL(val)   (((val) & 0x1) << 13)
#define LTQ_ES_RA_03_00_REG_RA01_MG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA01_MG) >> 13) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA01_MG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA01_MG) | (((val) & 0x1) << 13))
/* Cross_VLAN bit for 0180C2000001 (12) */
#define LTQ_ES_RA_03_00_REG_RA01_CV   (0x1 << 12)
#define LTQ_ES_RA_03_00_REG_RA01_CV_VAL(val)   (((val) & 0x1) << 12)
#define LTQ_ES_RA_03_00_REG_RA01_CV_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA01_CV) >> 12) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA01_CV_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA01_CV) | (((val) & 0x1) << 12))
/* TXTAG bit for 0180C2000001 (11:10) */
#define LTQ_ES_RA_03_00_REG_RA01_TXTAG   (0x3 << 10)
#define LTQ_ES_RA_03_00_REG_RA01_TXTAG_VAL(val)   (((val) & 0x3) << 10)
#define LTQ_ES_RA_03_00_REG_RA01_TXTAG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA01_TXTAG) >> 10) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA01_TXTAG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA01_TXTAG) | (((val) & 0x3) << 10))
/* Action bit for 0180C2000001 (9:8) */
#define LTQ_ES_RA_03_00_REG_RA01_ACT   (0x3 << 8)
#define LTQ_ES_RA_03_00_REG_RA01_ACT_VAL(val)   (((val) & 0x3) << 8)
#define LTQ_ES_RA_03_00_REG_RA01_ACT_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA01_ACT) >> 8) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA01_ACT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA01_ACT) | (((val) & 0x3) << 8))
/* Valid bit for 0180C2000000 (7) */
#define LTQ_ES_RA_03_00_REG_RA00_VALID   (0x1 << 7)
#define LTQ_ES_RA_03_00_REG_RA00_VALID_VAL(val)   (((val) & 0x1) << 7)
#define LTQ_ES_RA_03_00_REG_RA00_VALID_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA00_VALID) >> 7) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA00_VALID_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA00_VALID) | (((val) & 0x1) << 7))
/* Span bit for 0180C2000000 (6) */
#define LTQ_ES_RA_03_00_REG_RA00_SPAN   (0x1 << 6)
#define LTQ_ES_RA_03_00_REG_RA00_SPAN_VAL(val)   (((val) & 0x1) << 6)
#define LTQ_ES_RA_03_00_REG_RA00_SPAN_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA00_SPAN) >> 6) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA00_SPAN_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA00_SPAN) | (((val) & 0x1) << 6))
/* Management bit for 0180C2000000 (5) */
#define LTQ_ES_RA_03_00_REG_RA00_MG   (0x1 << 5)
#define LTQ_ES_RA_03_00_REG_RA00_MG_VAL(val)   (((val) & 0x1) << 5)
#define LTQ_ES_RA_03_00_REG_RA00_MG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA00_MG) >> 5) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA00_MG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA00_MG) | (((val) & 0x1) << 5))
/* Cross_VLAN bit for 0180C2000000 (4) */
#define LTQ_ES_RA_03_00_REG_RA00_CV   (0x1 << 4)
#define LTQ_ES_RA_03_00_REG_RA00_CV_VAL(val)   (((val) & 0x1) << 4)
#define LTQ_ES_RA_03_00_REG_RA00_CV_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA00_CV) >> 4) & 0x1)
#define LTQ_ES_RA_03_00_REG_RA00_CV_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA00_CV) | (((val) & 0x1) << 4))
/* TXTAG bit for 0180C2000000 (3:2) */
#define LTQ_ES_RA_03_00_REG_RA00_TXTAG   (0x3 << 2)
#define LTQ_ES_RA_03_00_REG_RA00_TXTAG_VAL(val)   (((val) & 0x3) << 2)
#define LTQ_ES_RA_03_00_REG_RA00_TXTAG_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA00_TXTAG) >> 2) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA00_TXTAG_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA00_TXTAG) | (((val) & 0x3) << 2))
/* Action bit for 0180C2000000 (1:0) */
#define LTQ_ES_RA_03_00_REG_RA00_ACT   (0x3)
#define LTQ_ES_RA_03_00_REG_RA00_ACT_VAL(val)   (((val) & 0x3) << 0)
#define LTQ_ES_RA_03_00_REG_RA00_ACT_GET(val)   ((((val) & LTQ_ES_RA_03_00_REG_RA00_ACT) >> 0) & 0x3)
#define LTQ_ES_RA_03_00_REG_RA00_ACT_SET(reg,val) (reg) = ((reg & ~LTQ_ES_RA_03_00_REG_RA00_ACT) | (((val) & 0x3) << 0))

/*******************************************************************************
 * Protocol Filter 0
 ******************************************************************************/

/* Value Compared with Protocol in IP Header (31:24) */
#define LTQ_ES_PRTCL_F0_REG_PFR3   (0xff << 24)
#define LTQ_ES_PRTCL_F0_REG_PFR3_VAL(val)   (((val) & 0xff) << 24)
#define LTQ_ES_PRTCL_F0_REG_PFR3_GET(val)   ((((val) & LTQ_ES_PRTCL_F0_REG_PFR3) >> 24) & 0xff)
#define LTQ_ES_PRTCL_F0_REG_PFR3_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_F0_REG_PFR3) | (((val) & 0xff) << 24))
/* Value Compared with Protocol in IP Header (23:16) */
#define LTQ_ES_PRTCL_F0_REG_PFR2   (0xff << 16)
#define LTQ_ES_PRTCL_F0_REG_PFR2_VAL(val)   (((val) & 0xff) << 16)
#define LTQ_ES_PRTCL_F0_REG_PFR2_GET(val)   ((((val) & LTQ_ES_PRTCL_F0_REG_PFR2) >> 16) & 0xff)
#define LTQ_ES_PRTCL_F0_REG_PFR2_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_F0_REG_PFR2) | (((val) & 0xff) << 16))
/* Value Compared with Protocol in IP Header (15:8) */
#define LTQ_ES_PRTCL_F0_REG_PFR1   (0xff << 8)
#define LTQ_ES_PRTCL_F0_REG_PFR1_VAL(val)   (((val) & 0xff) << 8)
#define LTQ_ES_PRTCL_F0_REG_PFR1_GET(val)   ((((val) & LTQ_ES_PRTCL_F0_REG_PFR1) >> 8) & 0xff)
#define LTQ_ES_PRTCL_F0_REG_PFR1_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_F0_REG_PFR1) | (((val) & 0xff) << 8))
/* Value Compared with Protocol in IP Header (7:0) */
#define LTQ_ES_PRTCL_F0_REG_PFR0   (0xff)
#define LTQ_ES_PRTCL_F0_REG_PFR0_VAL(val)   (((val) & 0xff) << 0)
#define LTQ_ES_PRTCL_F0_REG_PFR0_GET(val)   ((((val) & LTQ_ES_PRTCL_F0_REG_PFR0) >> 0) & 0xff)
#define LTQ_ES_PRTCL_F0_REG_PFR0_SET(reg,val) (reg) = ((reg & ~LTQ_ES_PRTCL_F0_REG_PFR0) | (((val) & 0xff) << 0))

#endif
