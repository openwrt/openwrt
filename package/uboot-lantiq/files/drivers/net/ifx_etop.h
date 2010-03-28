/*
 * Lantiq switch ethernet driver for Danube family.
 *
 * Based on INCA-IP driver:
 * (C) Copyright 2003-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#ifndef __DRIVERS_IFX_SW_H__
#define __DRIVERS_IFX_SW_H__

#define DANUBE_PPE32_BASE  0xBE180000
#define DANUBE_PPE32_DATA_MEM_MAP_REG_BASE   (DANUBE_PPE32_BASE + (0x4000 * 4))

#define ETOP_MDIO_CFG           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0600 * 4)))
#define ETOP_MDIO_ACC           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0601 * 4)))
#define ETOP_CFG                ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0602 * 4)))
#define ETOP_IG_VLAN_COS        ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0603 * 4)))
#define ETOP_IG_DSCP_COS3       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0604 * 4)))
#define ETOP_IG_DSCP_COS2       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0605 * 4)))
#define ETOP_IG_DSCP_COS1       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0606 * 4)))
#define ETOP_IG_DSCP_COS0       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0607 * 4)))
#define ETOP_IG_PLEN_CTRL       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0608 * 4)))
#define ETOP_ISR                ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x060A * 4)))
#define ETOP_IER                ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x060B * 4)))
#define ETOP_VPID               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x060C * 4)))
#define ENET_MAC_CFG            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0610 * 4)))
#define ENETS_DBA               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0612 * 4)))
#define ENETS_CBA               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0613 * 4)))
#define ENETS_CFG               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0614 * 4)))
#define ENETS_PGCNT             ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0615 * 4)))
#define ENETS_PKTCNT            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0616 * 4)))
#define ENETS_BUF_CTRL          ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0617 * 4)))
#define ENETS_COS_CFG           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0618 * 4)))
#define ENETS_IGDROP            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0619 * 4)))
#define ENETS_IGERR             ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x061A * 4)))
#define ENET_MAC_DA0            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x061B * 4)))
#define ENET_MAC_DA1            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x061C * 4)))



#define DANUBE_DMA_BASE                 0xBE104100

typedef struct IfxDMA_s
{
	unsigned long  dma_clc;                            /*0x0000*/
	unsigned long  dma_rsvd1[1];   /* for mapping */   /*0x0004*/
	unsigned long  dma_id;                             /*0x0008*/
	unsigned long  dma_rsvd2[1];   /* for mapping */   /*0x000C*/
	unsigned long  dma_ctrl;                           /*0x0010*/
	unsigned long  dma_cpoll;                          /*0x0014*/
	unsigned long  dma_cs;                             /*0x0018*/
	unsigned long  dma_cctrl;                          /*0x001C*/
	unsigned long  dma_cdba;                           /*0x0020*/
	unsigned long  dma_cdlen;                          /*0x0024*/
	unsigned long  dma_cis;                            /*0x0028*/
	unsigned long  dma_cie;                            /*0x002C*/
	unsigned long  dma_rsvd3[4];   /* for mapping */   /*0x0030*/
	unsigned long  dma_ps;                             /*0x0040*/
	unsigned long  dma_pctrl;                          /*0x0044*/
	unsigned long  dma_rsvd4[43];  /* for mapping */   /*0x0048*/
	unsigned long  dma_irnen;                          /*0x00F4*/
	unsigned long  dma_irncr;                          /*0x00F8*/
	unsigned long  dma_irnicr;                         /*0x00FC*/
} IfxDMA_t;

/* Register access macros */
#define dma_readl(reg)				\
	readl(&pDma->reg)
#define dma_writel(reg,value)			\
	writel((value), &pDma->reg)

int lq_eth_initialize(bd_t * bis);

#endif /* __DRIVERS_IFX_SW_H__ */
