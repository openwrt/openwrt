/** @file */
/******************************************************************************
 *
 * Copyright(c) 2016 - 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef __HALMAC_PCIE_REG_H__
#define __HALMAC_PCIE_REG_H__

#define RAC_DIRECT_OFFSET_G1 0x3800
#define RAC_DIRECT_OFFSET_G2 0x3880
#define RAC_MULT 2

/* PCIE PHY register */
#define RAC_CTRL_PPR			0x00
#define RAC_ANA0C			0x0C
#define RAC_ANA0A			0x0A
#define BAC_EQ_SEL			BIT(5)
#define RAC_ANA10			0x10
#define PCIE_BIT_PINOUT_DIS		BIT(3)
#define RAC_ANA19			0x19
#define PCIE_BIT_RD_SEL			BIT(2)

#define RAC_REG_REV2			0x1B
#define BAC_CMU_EN_DLY_SH		12
#define BAC_CMU_EN_DLY_MSK		0xF

#define RAC_REG_FLD_0			0x1D
#define BAC_AUTOK_N_SH			2
#define BAC_AUTOK_N_MSK			0x3

#define RAC_ANA1F			0x1F
#define RAC_SET_PPR			0x20
#define RAC_TRG_PPR			0x21
#define RAC_ANA20			0x20
#define RAC_ANA21			0x21
#define RAC_ANA23			0x23
#define RAC_ANA24			0x24
#define RAC_ANA26			0x26
#define RAC_ANA2F			0x2F

#define RAC_CTRL_PPR_V1			0x30
#define BAC_AUTOK_DIV_SH		14
#define BAC_AUTOK_DIV_MSK		0x3
#define BAC_AUTOK_EN			BIT(13)
#define BAC_AUTOK_ONCE_EN		BIT(12)
#define BAC_AUTOK_HW_TAR_SH		0
#define BAC_AUTOK_HW_TAR_MSK		0xFFF

#define RAC_SET_PPR_V1			0x31
#define BAC_AUTOK_MGN_SH		12
#define BAC_AUTOK_MGN_MSK		0xF
#define BAC_AUTOK_TAR_SH		0
#define BAC_AUTOK_TAR_MSK		0xFFF

/* PCIE CFG register */
#define PCIE_L1_STS			0x80
#define BCFG_LINK_SPEED_SH		16
#define BCFG_LINK_SPEED_MSK		0xF
#define PCIE_PHY_RATE			0x82
#define PCIE_L1SS_CTRL			0x718
#define PCIE_L1_CTRL			0x719
#define PCIE_ACK_NFTS			0x70D
#define PCIE_COM_CLK_NFTS		0x70E
#define PCIE_FTS			0x80C
#define PCIE_ASPM_CTRL			0x70F
#define PCIE_CLK_CTRL			0x725
#define CFG_RST_MSTATE			0xB48
#define PCIE_L1SS_CAP			0x160
#define PCIE_L1SS_SUP			0x164
#define PCIE_L1SS_STS			0x168
#define FILTER_OUT_EQ_SH		10
#define FILTER_OUT_EQ_MSK		0x1F
#define REG_FILTER_OUT_SH		2
#define REG_FILTER_OUT_MSK		0x1F
#define PCIE_LINK_SPEED_32BIT   0x80

/*PCIE SPEED*/
#define PCIE_LINK_SPEED_SH      16
#define PCIE_LINK_SPEED_BITS_MSK 0XF
#define PCIE_CAPABILITY_SPEED    0x7C
#define PCIE_SUPPORT_GEN_SH      0
#define PCIE_LINK_CHANGE_SPEED   0xA0
#define PCIE_POLLING_BIT         BIT(17)

/* 8852C PCIE bit */
#define PCIE_L1_STS_V1			0x2080
#define PCIE_ASPM_CTRL_V1		0x270C
#define PCIE_L1SS_CAP_V1		0x2160
#define PCIE_L1SS_SUP_V1		0x2164
#define PCIE_L1SS_STS_V1		0x2168

/* PCIE CFG bit */
#define PCIE_BIT_STS_L0S		BIT(0)
#define PCIE_BIT_STS_L1			BIT(1)
#define PCIE_BIT_WAKE			BIT(2)
#define PCIE_BIT_L1			BIT(3)
#define PCIE_BIT_CLK			BIT(4)
#define PCIE_BIT_L0S			BIT(7)
#define PCIE_BIT_L1SS			BIT(5)
#define PCIE_BIT_L1SSSUP		BIT(4)
#define PCIE_BIT_ASPM_L11		BIT(3)
#define PCIE_BIT_ASPM_L12		BIT(2)
#define PCIE_BIT_PCI_L11		BIT(1)
#define PCIE_BIT_PCI_L12		BIT(0)

/* 8852C PCIE bit */
#define PCIE_BIT_L1_V1			BIT(17)
#define PCIE_BIT_L0S_V1			BIT(16)
#define PCIE_BIT_PSAVE			BIT(15)

/* PCIE ASPM mask*/
#define SHFT_L1DLY			3
#define SHFT_L0SDLY			0
#define SHFT_L1DLY_V1			27
#define SHFT_L0SDLY_V1			24
#define PCIE_ASPMDLY_MASK		0x07
#define PCIE_L1SS_MASK			0x0F
#define PCIE_L1SS_ID_MASK		0xFFFF

/* PCIE Capability */
#define PCIE_L1SS_ID			0x001E

/* PCIE MAC register */
#define LINK_CTRL2_REG_OFFSET		0xA0
#define GEN2_CTRL_OFFSET		0x80C
#define LINK_STATUS_REG_OFFSET		0x82

#define PCIE_GEN1_SPEED			0x01
#define PCIE_GEN2_SPEED			0x02

#endif/* __HALMAC_PCIE_REG_H__ */
