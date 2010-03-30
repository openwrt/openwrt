/*
 * (C) Copyright 2010
 * Ralph Hempel
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 */
 
/***********************************************************************/
/*  Module      :  PMU register address and bits                       */
/***********************************************************************/
#define AR9_PMU									(0xBF102000)
/* PMU Power down Control Register */
#define AR9_PMU_PWDCR							((volatile u32*)(AR9_PMU + 0x001C))
/* PMU Status Register */
#define AR9_PMU_SR								((volatile u32*)(AR9_PMU + 0x0020))
/** DMA block */
#define AR9_PMU_DMA								(1<<5)
#define AR9_PMU_SDIO							(1<<16)
#define AR9_PMU_USB0							(1<<6)
#define AR9_PMU_USB0_P							(1<<0)
#define AR9_PMU_SWITCH							(1<<28)


/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/
#define AR9_RCU_BASE_ADDR						(0xBF203000)
#define AR9_RCU_RST_REQ							((volatile u32*)(AR9_RCU_BASE_ADDR + 0x0010))
#define AR9_RCU_RST_STAT						((volatile u32*)(AR9_RCU_BASE_ADDR + 0x0014))
#define AR9_RST_ALL								(1 << 30)

/*** Reset Request Register Bits ***/
#define AR9_RCU_RST_REQ_SRST					(1 << 30)
#define AR9_RCU_RST_REQ_ARC_JTAG				(1 << 20)
#define AR9_RCU_RST_REQ_PCI						(1 << 13)
#define AR9_RCU_RST_REQ_AFE						(1 << 11)
#define AR9_RCU_RST_REQ_SDIO					(1 << 19)
#define AR9_RCU_RST_REQ_DMA						(1 << 9)
#define AR9_RCU_RST_REQ_PPE						(1 << 8)
#define AR9_RCU_RST_REQ_DFE						(1 << 7)

/***********************************************************************/
/*  Module      :  GPIO register address and bits                       */
/***********************************************************************/
#define AR9_GPIO								(0xBE100B00)
/***Port 0 Data Output Register (0010H)***/
#define AR9_GPIO_P0_OUT							((volatile u32 *)(AR9_GPIO+ 0x0010))
/***Port 1 Data Output Register (0040H)***/
#define AR9_GPIO_P1_OUT							((volatile u32 *)(AR9_GPIO+ 0x0040))
/***Port 2 Data Output Register (0070H)***/
#define AR9_GPIO_P2_OUT							((volatile u32 *)(AR9_GPIO+ 0x0070))
/***Port 3 Data Output Register (00A0H)***/
#define AR9_GPIO_P3_OUT							((volatile u32 *)(AR9_GPIO+ 0x00A0))
/***Port 0 Data Input Register (0014H)***/
#define AR9_GPIO_P0_IN							((volatile u32 *)(AR9_GPIO+ 0x0014))
/***Port 1 Data Input Register (0044H)***/
#define AR9_GPIO_P1_IN							((volatile u32 *)(AR9_GPIO+ 0x0044))
/***Port 2 Data Input Register (0074H)***/
#define AR9_GPIO_P2_IN							((volatile u32 *)(AR9_GPIO+ 0x0074))
/***Port 3 Data Input Register (00A4H)***/
#define AR9_GPIO_P3_IN							((volatile u32 *)(AR9_GPIO+ 0x00A4))
/***Port 0 Direction Register (0018H)***/
#define AR9_GPIO_P0_DIR							((volatile u32 *)(AR9_GPIO+ 0x0018))
/***Port 1 Direction Register (0048H)***/
#define AR9_GPIO_P1_DIR							((volatile u32 *)(AR9_GPIO+ 0x0048))
/***Port 2 Direction Register (0078H)***/
#define AR9_GPIO_P2_DIR							((volatile u32 *)(AR9_GPIO+ 0x0078))
/***Port 3 Direction Register (0048H)***/
#define AR9_GPIO_P3_DIR							((volatile u32 *)(AR9_GPIO+ 0x00A8))
/***Port 0 Alternate Function Select Register 0 (001C H) ***/
#define AR9_GPIO_P0_ALTSEL0						((volatile u32 *)(AR9_GPIO+ 0x001C))
/***Port 1 Alternate Function Select Register 0 (004C H) ***/
#define AR9_GPIO_P1_ALTSEL0						((volatile u32 *)(AR9_GPIO+ 0x004C))
/***Port 2 Alternate Function Select Register 0 (007C H) ***/
#define AR9_GPIO_P2_ALTSEL0						((volatile u32 *)(AR9_GPIO+ 0x007C))
/***Port 3 Alternate Function Select Register 0 (00AC H) ***/
#define AR9_GPIO_P3_ALTSEL0						((volatile u32 *)(AR9_GPIO+ 0x00AC))
/***Port 0 Alternate Function Select Register 1 (0020 H) ***/
#define AR9_GPIO_P0_ALTSEL1						((volatile u32 *)(AR9_GPIO+ 0x0020))
/***Port 1 Alternate Function Select Register 0 (0050 H) ***/
#define AR9_GPIO_P1_ALTSEL1					((volatile u32 *)(AR9_GPIO+ 0x0050))
/***Port 2 Alternate Function Select Register 0 (0080 H) ***/
#define AR9_GPIO_P2_ALTSEL1						((volatile u32 *)(AR9_GPIO+ 0x0080))
/***Port 3 Alternate Function Select Register 0 (0064 H) ***/
#define AR9_GPIO_P3_ALTSEL1						((volatile u32 *)(AR9_GPIO+ 0x0064))
/***Port 0 Open Drain Control Register (0024H)***/
#define AR9_GPIO_P0_OD							((volatile u32 *)(AR9_GPIO+ 0x0024))
/***Port 1 Open Drain Control Register (0054H)***/
#define AR9_GPIO_P1_OD							((volatile u32 *)(AR9_GPIO+ 0x0054))
/***Port 2 Open Drain Control Register (0084H)***/
#define AR9_GPIO_P2_OD							((volatile u32 *)(AR9_GPIO+ 0x0084))
/***Port 3 Open Drain Control Register (0034H)***/
#define AR9_GPIO_P3_OD							((volatile u32 *)(AR9_GPIO+ 0x0034))
/***Port 0 Input Schmitt-Trigger Off Register (0028 H) ***/
#define AR9_GPIO_P0_STOFF						((volatile u32 *)(AR9_GPIO+ 0x0028))
/***Port 1 Input Schmitt-Trigger Off Register (0058 H) ***/
#define AR9_GPIO_P1_STOFF						((volatile u32 *)(AR9_GPIO+ 0x0058))
/***Port 2 Input Schmitt-Trigger Off Register (0088 H) ***/
#define AR9_GPIO_P2_STOFF						((volatile u32 *)(AR9_GPIO+ 0x0088))
/***Port 3 Input Schmitt-Trigger Off Register (0094 H) ***/
//#define AR9_GPIO_P3_STOFF						((volatile u32 *)(AR9_GPIO+ 0x0094))
/***Port 0 Pull Up/Pull Down Select Register (002C H)***/
#define AR9_GPIO_P0_PUDSEL						((volatile u32 *)(AR9_GPIO+ 0x002C))
/***Port 1 Pull Up/Pull Down Select Register (005C H)***/
#define AR9_GPIO_P1_PUDSEL						((volatile u32 *)(AR9_GPIO+ 0x005C))
/***Port 2 Pull Up/Pull Down Select Register (008C H)***/
#define AR9_GPIO_P2_PUDSEL						((volatile u32 *)(AR9_GPIO+ 0x008C))
/***Port 3 Pull Up/Pull Down Select Register (0038 H)***/
#define AR9_GPIO_P3_PUDSEL						((volatile u32 *)(AR9_GPIO+ 0x0038))
/***Port 0 Pull Up Device Enable Register (0030 H)***/
#define AR9_GPIO_P0_PUDEN						((volatile u32 *)(AR9_GPIO+ 0x0030))
/***Port 1 Pull Up Device Enable Register (0060 H)***/
#define AR9_GPIO_P1_PUDEN						((volatile u32 *)(AR9_GPIO+ 0x0060))
/***Port 2 Pull Up Device Enable Register (0090 H)***/
#define AR9_GPIO_P2_PUDEN						((volatile u32 *)(AR9_GPIO+ 0x0090))
/***Port 3 Pull Up Device Enable Register (003c H)***/
#define AR9_GPIO_P3_PUDEN						((volatile u32 *)(AR9_GPIO+ 0x003C))

/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/
#define AR9_CGU									(0xBF103000)
/***CGU Clock PLL0 ***/
#define AR9_CGU_PLL0_CFG						((volatile u32*)(AR9_CGU+ 0x0004))
/***CGU Clock PLL1 ***/
#define AR9_CGU_PLL1_CFG						((volatile u32*)(AR9_CGU+ 0x0008))
/***CGU Clock SYS Mux Register***/
#define AR9_CGU_SYS								((volatile u32*)(AR9_CGU+ 0x0010))
/***CGU Interface Clock Control Register***/
#define AR9_CGU_IFCCR							((volatile u32*)(AR9_CGU+ 0x0018))
/***CGU PCI Clock Control Register**/
#define AR9_CGU_PCICR							((volatile u32*)(AR9_CGU+ 0x0034))
#define CLOCK_60M								60000000
#define CLOCK_83M								83333333
#define CLOCK_111M								111111111
#define CLOCK_133M								133333333
#define CLOCK_166M								166666667
#define CLOCK_196M								196666667
#define CLOCK_333M								333333333
#define CLOCK_366M								366666667
#define CLOCK_500M								500000000

/***********************************************************************/
/*  Module      :  MPS register address and bits                       */
/***********************************************************************/
#define AR9_MPS									(KSEG1+0x1F107000)
#define AR9_MPS_CHIPID							((volatile u32*)(AR9_MPS + 0x0344))
#define AR9_MPS_CHIPID_VERSION_GET(value)		(((value) >> 28) & ((1 << 4) - 1))
#define AR9_MPS_CHIPID_PARTNUM_GET(value)		(((value) >> 12) & ((1 << 16) - 1))
#define AR9_MPS_CHIPID_MANID_GET(value)			(((value) >> 1) & ((1 << 10) - 1))

/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/
#define AR9_EBU									(0xBE105300)

#define AR9_EBU_CLC								((volatile u32*)(AR9_EBU+ 0x0000))
#define AR9_EBU_CLC_DISS						(1 << 1)
#define AR9_EBU_CLC_DISR						(1 << 0)

#define AR9_EBU_ID								((volatile u32*)(AR9_EBU+ 0x0008))

/***EBU Global Control Register***/
#define AR9_EBU_CON								((volatile u32*)(AR9_EBU+ 0x0010))
#define AR9_EBU_CON_DTACS (value)				(((( 1 << 3) - 1) & (value)) << 20)
#define AR9_EBU_CON_DTARW (value)				(((( 1 << 3) - 1) & (value)) << 16)
#define AR9_EBU_CON_TOUTC (value)				(((( 1 << 8) - 1) & (value)) << 8)
#define AR9_EBU_CON_ARBMODE (value)				(((( 1 << 2) - 1) & (value)) << 6)
#define AR9_EBU_CON_ARBSYNC						(1 << 5)
//#define AR9_EBU_CON_1							(1 << 3)

/***EBU Address Select Register 0***/
#define AR9_EBU_ADDSEL0							((volatile u32*)(AR9_EBU + 0x0020))
/***EBU Address Select Register 1***/
#define AR9_EBU_ADDSEL1							((volatile u32*)(AR9_EBU + 0x0024))
/***EBU Address Select Register 2***/
#define AR9_EBU_ADDSEL2							((volatile u32*)(AR9_EBU + 0x0028))
/***EBU Address Select Register 3***/
#define AR9_EBU_ADDSEL3							((volatile u32*)(AR9_EBU + 0x002C))
#define AR9_EBU_ADDSEL_BASE (value)				(((( 1 << 20) - 1) & (value)) << 12)
#define AR9_EBU_ADDSEL_MASK (value)				(((( 1 << 4) - 1) & (value)) << 4)
#define AR9_EBU_ADDSEL_MIRRORE					(1 << 1)
#define AR9_EBU_ADDSEL_REGEN					(1 << 0)

/***EBU Bus Configuration Register 0***/
#define AR9_EBU_BUSCON0							((volatile u32*)(AR9_EBU+ 0x0060))
#define AR9_EBU_BUSCON0_WRDIS					(1 << 31)
#define AR9_EBU_BUSCON0_ADSWP (value)			(1 << 30)
#define AR9_EBU_BUSCON0_PG_EN (value)			(1 << 29)
#define AR9_EBU_BUSCON0_AGEN (value)			(((( 1 << 3) - 1) & (value)) << 24)
#define AR9_EBU_BUSCON0_SETUP					(1 << 22)
#define AR9_EBU_BUSCON0_WAIT (value)			(((( 1 << 2) - 1) & (value)) << 20)
#define AR9_EBU_BUSCON0_WAITINV					(1 << 19)
#define AR9_EBU_BUSCON0_VN_EN					(1 << 18)
#define AR9_EBU_BUSCON0_PORTW (value)			(((( 1 << 2) - 1) & (value)) << 16)
#define AR9_EBU_BUSCON0_ALEC (value)			(((( 1 << 2) - 1) & (value)) << 14)
#define AR9_EBU_BUSCON0_BCGEN (value)			(((( 1 << 2) - 1) & (value)) << 12)
#define AR9_EBU_BUSCON0_WAITWDC (value)			(((( 1 << 4) - 1) & (value)) << 8)
#define AR9_EBU_BUSCON0_WAITRRC (value)			(((( 1 << 2) - 1) & (value)) << 6)
#define AR9_EBU_BUSCON0_HOLDC (value)			(((( 1 << 2) - 1) & (value)) << 4)
#define AR9_EBU_BUSCON0_RECOVC (value)			(((( 1 << 2) - 1) & (value)) << 2)
#define AR9_EBU_BUSCON0_CMULT (value)			(((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 1***/
#define AR9_EBU_BUSCON1							((volatile u32*)(AR9_EBU+ 0x0064))
#define AR9_EBU_BUSCON1_WRDIS					(1 << 31)
#define AR9_EBU_BUSCON1_ALEC (value)			(((( 1 << 2) - 1) & (value)) << 29)
#define AR9_EBU_BUSCON1_BCGEN (value)			(((( 1 << 2) - 1) & (value)) << 27)
#define AR9_EBU_BUSCON1_AGEN (value)			(((( 1 << 2) - 1) & (value)) << 24)
#define AR9_EBU_BUSCON1_CMULTR (value)			(((( 1 << 2) - 1) & (value)) << 22)
#define AR9_EBU_BUSCON1_WAIT (value)			(((( 1 << 2) - 1) & (value)) << 20)
#define AR9_EBU_BUSCON1_WAITINV					(1 << 19)
#define AR9_EBU_BUSCON1_SETUP					(1 << 18)
#define AR9_EBU_BUSCON1_PORTW (value)			(((( 1 << 2) - 1) & (value)) << 16)
#define AR9_EBU_BUSCON1_WAITRDC (value)			(((( 1 << 7) - 1) & (value)) << 9)
#define AR9_EBU_BUSCON1_WAITWRC (value)			(((( 1 << 3) - 1) & (value)) << 6)
#define AR9_EBU_BUSCON1_HOLDC (value)			(((( 1 << 2) - 1) & (value)) << 4)
#define AR9_EBU_BUSCON1_RECOVC (value)			(((( 1 << 2) - 1) & (value)) << 2)
#define AR9_EBU_BUSCON1_CMULT (value)			(((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 2***/
#define AR9_EBU_BUSCON2							((volatile u32*)(AR9_EBU+ 0x0068))
#define AR9_EBU_BUSCON2_WRDIS					(1 << 31)
#define AR9_EBU_BUSCON2_ALEC (value)			(((( 1 << 2) - 1) & (value)) << 29)
#define AR9_EBU_BUSCON2_BCGEN (value)			(((( 1 << 2) - 1) & (value)) << 27)
#define AR9_EBU_BUSCON2_AGEN (value)			(((( 1 << 2) - 1) & (value)) << 24)
#define AR9_EBU_BUSCON2_CMULTR (value)			(((( 1 << 2) - 1) & (value)) << 22)
#define AR9_EBU_BUSCON2_WAIT (value)			(((( 1 << 2) - 1) & (value)) << 20)
#define AR9_EBU_BUSCON2_WAITINV					(1 << 19)
#define AR9_EBU_BUSCON2_SETUP					(1 << 18)
#define AR9_EBU_BUSCON2_PORTW (value)			(((( 1 << 2) - 1) & (value)) << 16)
#define AR9_EBU_BUSCON2_WAITRDC (value)			(((( 1 << 7) - 1) & (value)) << 9)
#define AR9_EBU_BUSCON2_WAITWRC (value)			(((( 1 << 3) - 1) & (value)) << 6)
#define AR9_EBU_BUSCON2_HOLDC (value)			(((( 1 << 2) - 1) & (value)) << 4)
#define AR9_EBU_BUSCON2_RECOVC (value)			(((( 1 << 2) - 1) & (value)) << 2)
#define AR9_EBU_BUSCON2_CMULT (value)			(((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 2***/
#define AR9_EBU_BUSCON3							((volatile u32*)(AR9_EBU+ 0x006C))
#define AR9_EBU_BUSCON3_WRDIS					(1 << 31)
#define AR9_EBU_BUSCON3_ADSWP (value)			(1 << 30)
#define AR9_EBU_BUSCON3_PG_EN (value)			(1 << 29)
#define AR9_EBU_BUSCON3_AGEN (value)			(((( 1 << 3) - 1) & (value)) << 24)
#define AR9_EBU_BUSCON3_SETUP					(1 << 22)
#define AR9_EBU_BUSCON3_WAIT (value) 			(((( 1 << 2) - 1) & (value)) << 20)
#define AR9_EBU_BUSCON3_WAITINV					(1 << 19)
#define AR9_EBU_BUSCON3_VN_EN					(1 << 18)
#define AR9_EBU_BUSCON3_PORTW (value)			(((( 1 << 2) - 1) & (value)) << 16)
#define AR9_EBU_BUSCON3_ALEC (value)			(((( 1 << 2) - 1) & (value)) << 14)
#define AR9_EBU_BUSCON3_BCGEN (value)			(((( 1 << 2) - 1) & (value)) << 12)
#define AR9_EBU_BUSCON3_WAITWDC (value)			(((( 1 << 4) - 1) & (value)) << 8)
#define AR9_EBU_BUSCON3_WAITRRC (value)			(((( 1 << 2) - 1) & (value)) << 6)
#define AR9_EBU_BUSCON3_HOLDC (value)			(((( 1 << 2) - 1) & (value)) << 4)
#define AR9_EBU_BUSCON3_RECOVC (value)			(((( 1 << 2) - 1) & (value)) << 2)
#define AR9_EBU_BUSCON3_CMULT (value)			(((( 1 << 2) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  SDRAM register address and bits                     */
/***********************************************************************/
#define AR9_SDRAM								(0xBF800000)

/***********************************************************************/
/*  Module      :  ASC0 register address and bits                      */
/***********************************************************************/
#define AR9_ASC0								(KSEG1 | 0x1E100400)
#define AR9_ASC0_TBUF							((volatile u32*)(AR9_ASC0 + 0x0020))
#define AR9_ASC0_RBUF							((volatile u32*)(AR9_ASC0 + 0x0024))
#define AR9_ASC0_FSTAT							((volatile u32*)(AR9_ASC0 + 0x0048))

/***********************************************************************/
/*  Module      :  ASC1 register address and bits                      */
/***********************************************************************/
#define AR9_ASC1								(KSEG1 | 0x1E100C00)
#define AR9_ASC1_TBUF							((volatile u32*)(AR9_ASC1 + 0x0020))
#define AR9_ASC1_RBUF							((volatile u32*)(AR9_ASC1 + 0x0024))
#define AR9_ASC1_FSTAT							((volatile u32*)(AR9_ASC1 + 0x0048))

/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/
#define AR9_DMA_OFFSET 							(0xBE104100)
/***********************************************************************/
#define AR9_DMA_CLC								((volatile u32*)(AR9_DMA_OFFSET + 0x0000))
#define AR9_DMA_ID								((volatile u32*)(AR9_DMA_OFFSET + 0x0008))
#define AR9_DMA_CTRL							(volatile u32*)(AR9_DMA_BASE + 0x10)

/** DMA Port Select Register */
#define AR9_DMA_PS								((volatile u32*)(AR9_DMA_OFFSET + 0x0040))
/** DMA Port Control Register */
#define AR9_DMA_PCTRL							((volatile u32*)(AR9_DMA_OFFSET + 0x0044))
#define AR9_DMA_IRNEN							((volatile u32*)(AR9_DMA_OFFSET + 0x00F4))
#define AR9_DMA_IRNCR							((volatile u32*)(AR9_DMA_OFFSET + 0x00F8))
#define AR9_DMA_IRNICR							((volatile u32*)(AR9_DMA_OFFSET + 0x00FC))

#define AR9_DMA_CS								((volatile u32*)(AR9_DMA_OFFSET + 0x0018))
#define AR9_DMA_CCTRL							((volatile u32*)(AR9_DMA_OFFSET + 0x001C))
#define AR9_DMA_CDBA							((volatile u32*)(AR9_DMA_OFFSET + 0x0020))
#define AR9_DMA_CIE								((volatile u32*)(AR9_DMA_OFFSET + 0x002C))
#define AR9_DMA_CIS								((volatile u32*)(AR9_DMA_OFFSET + 0x0028))
#define AR9_DMA_CDLEN							((volatile u32*)(AR9_DMA_OFFSET + 0x0024))
#define AR9_DMA_CPOLL							((volatile u32*)(AR9_DMA_OFFSET + 0x0014))

/***********************************************************************/
/*  Module      :  GPORT switch register                               */
/***********************************************************************/
#define AR9_SW									(0xBE108000)
#define AR9_SW_PS								(AR9_SW + 0x000)
#define AR9_SW_P0_CTL							(AR9_SW + 0x004)
#define AR9_SW_P1_CTL							(AR9_SW + 0x008)
#define AR9_SW_P2_CTL							(AR9_SW + 0x00C)
#define AR9_SW_P0_VLAN							(AR9_SW + 0x010)
#define AR9_SW_P1_VLAN							(AR9_SW + 0x014)
#define AR9_SW_P2_VLAN							(AR9_SW + 0x018)
#define AR9_SW_P0_INCTL							(AR9_SW + 0x020)
#define AR9_SW_P1_INCTL							(AR9_SW + 0x024)
#define AR9_SW_P2_INCTL							(AR9_SW + 0x028)
#define AR9_SW_DF_PORTMAP						(AR9_SW + 0x02C)
#define AR9_SW_P0_ECS_Q32						(AR9_SW + 0x030)
#define AR9_SW_P0_ECS_Q10						(AR9_SW + 0x034)
#define AR9_SW_P0_ECW_Q32						(AR9_SW + 0x038)
#define AR9_SW_P0_ECW_Q10						(AR9_SW + 0x03C)
#define AR9_SW_P1_ECS_Q32						(AR9_SW + 0x040)
#define AR9_SW_P1_ECS_Q10						(AR9_SW + 0x044)
#define AR9_SW_P1_ECW_Q32						(AR9_SW + 0x048)
#define AR9_SW_P1_ECW_Q10						(AR9_SW + 0x04C)
#define AR9_SW_P2_ECS_Q32						(AR9_SW + 0x050)
#define AR9_SW_P2_ECS_Q10						(AR9_SW + 0x054)
#define AR9_SW_P2_ECW_Q32						(AR9_SW + 0x058)
#define AR9_SW_P2_ECW_Q10						(AR9_SW + 0x05C)
#define AR9_SW_INT_ENA							(AR9_SW + 0x060)
#define AR9_SW_INT_ST							(AR9_SW + 0x064)
#define AR9_SW_GCTL0							(AR9_SW + 0x068)
#define AR9_SW_GCTL1							(AR9_SW + 0x06C)
#define AR9_SW_ARP								(AR9_SW + 0x070)
#define AR9_SW_STRM_CTL							(AR9_SW + 0x074)
#define AR9_SW_RGMII_CTL						(AR9_SW + 0x078)
#define AR9_SW_1P_PRT							(AR9_SW + 0x07C)
#define AR9_SW_GBKT_SZBS						(AR9_SW + 0x080)
#define AR9_SW_GBKT_SZEBS						(AR9_SW + 0x084)
#define AR9_SW_BF_TH							(AR9_SW + 0x088)
#define AR9_SW_PMAC_HD_CTL						(AR9_SW + 0x08C)
#define AR9_SW_PMAC_SA1							(AR9_SW + 0x090)
#define AR9_SW_PMAC_SA2							(AR9_SW + 0x094)
#define AR9_SW_PMAC_DA1							(AR9_SW + 0x098)
#define AR9_SW_PMAC_DA2							(AR9_SW + 0x09C)
#define AR9_SW_PMAC_VLAN						(AR9_SW + 0x0A0)
#define AR9_SW_PMAC_TX_IPG						(AR9_SW + 0x0A4)
#define AR9_SW_PMAC_RX_IPG						(AR9_SW + 0x0A8)
#define AR9_SW_ADR_TB_CTL0						(AR9_SW + 0x0AC)
#define AR9_SW_ADR_TB_CTL1						(AR9_SW + 0x0B0)
#define AR9_SW_ADR_TB_CTL2						(AR9_SW + 0x0B4)
#define AR9_SW_ADR_TB_ST0						(AR9_SW + 0x0B8)
#define AR9_SW_ADR_TB_ST1						(AR9_SW + 0x0BC)
#define AR9_SW_ADR_TB_ST2						(AR9_SW + 0x0C0)
#define AR9_SW_RMON_CTL							(AR9_SW + 0x0C4)
#define AR9_SW_RMON_ST							(AR9_SW + 0x0C8)
#define AR9_SW_MDIO_CTL							(AR9_SW + 0x0CC)
#define AR9_SW_MDIO_DATA						(AR9_SW + 0x0D0)
#define AR9_SW_TP_FLT_ACT						(AR9_SW + 0x0D4)
#define AR9_SW_PRTCL_FLT_ACT					(AR9_SW + 0x0D8)
#define AR9_SW_VLAN_FLT0						(AR9_SW + 0x100)
#define AR9_SW_VLAN_FLT1						(AR9_SW + 0x104)
#define AR9_SW_VLAN_FLT2						(AR9_SW + 0x108)
#define AR9_SW_VLAN_FLT3						(AR9_SW + 0x10C)
#define AR9_SW_VLAN_FLT4						(AR9_SW + 0x110)
#define AR9_SW_VLAN_FLT5						(AR9_SW + 0x114)
#define AR9_SW_VLAN_FLT6						(AR9_SW + 0x118)
#define AR9_SW_VLAN_FLT7						(AR9_SW + 0x11C)
#define AR9_SW_VLAN_FLT8						(AR9_SW + 0x120)
#define AR9_SW_VLAN_FLT9						(AR9_SW + 0x124)
#define AR9_SW_VLAN_FLT10						(AR9_SW + 0x128)
#define AR9_SW_VLAN_FLT11						(AR9_SW + 0x12C)
#define AR9_SW_VLAN_FLT12						(AR9_SW + 0x130)
#define AR9_SW_VLAN_FLT13						(AR9_SW + 0x134)
#define AR9_SW_VLAN_FLT14						(AR9_SW + 0x138)
#define AR9_SW_VLAN_FLT15						(AR9_SW + 0x13C)
#define AR9_SW_TP_FLT10							(AR9_SW + 0x140)
#define AR9_SW_TP_FLT32							(AR9_SW + 0x144)
#define AR9_SW_TP_FLT54							(AR9_SW + 0x148)
#define AR9_SW_TP_FLT76							(AR9_SW + 0x14C)
#define AR9_SW_DFSRV_MAP0						(AR9_SW + 0x150)
#define AR9_SW_DFSRV_MAP1						(AR9_SW + 0x154)
#define AR9_SW_DFSRV_MAP2						(AR9_SW + 0x158)
#define AR9_SW_DFSRV_MAP3						(AR9_SW + 0x15C)
#define AR9_SW_TCP_PF0							(AR9_SW + 0x160)
#define AR9_SW_TCP_PF1							(AR9_SW + 0x164)
#define AR9_SW_TCP_PF2							(AR9_SW + 0x168)
#define AR9_SW_TCP_PF3							(AR9_SW + 0x16C)
#define AR9_SW_TCP_PF4							(AR9_SW + 0x170)
#define AR9_SW_TCP_PF5							(AR9_SW + 0x174)
#define AR9_SW_TCP_PF6							(AR9_SW + 0x178)
#define AR9_SW_TCP_PF7							(AR9_SW + 0x17C)
#define AR9_SW_RA_03_00							(AR9_SW + 0x180)
#define AR9_SW_RA_07_04							(AR9_SW + 0x184)
#define AR9_SW_RA_0B_08							(AR9_SW + 0x188)
#define AR9_SW_RA_0F_0C							(AR9_SW + 0x18C)
#define AR9_SW_RA_13_10							(AR9_SW + 0x190)
#define AR9_SW_RA_17_14							(AR9_SW + 0x194)
#define AR9_SW_RA_1B_18							(AR9_SW + 0x198)
#define AR9_SW_RA_1F_1C							(AR9_SW + 0x19C)
#define AR9_SW_RA_23_20							(AR9_SW + 0x1A0)
#define AR9_SW_RA_27_24							(AR9_SW + 0x1A4)
#define AR9_SW_RA_2B_28							(AR9_SW + 0x1A8)
#define AR9_SW_RA_2F_2C							(AR9_SW + 0x1AC)
#define AR9_SW_F0								(AR9_SW + 0x1B0)
#define AR9_SW_F1								(AR9_SW + 0x1B4)

#define REG32(addr)								*((volatile u32 *)(addr))
