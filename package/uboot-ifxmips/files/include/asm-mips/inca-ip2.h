/************************************************************************
 *
 * Copyright (c) 2005
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/
         
#define INCA_IP2_DMA				(KSEG1+0x14101000)
/***********************************************************************/
#define CONFIGURATION_REGISTERS_CLC (INCA_IP2_DMA + 0x00)
#define CONFIGURATION_REGISTERS_ID (INCA_IP2_DMA + 0x08)
#define GENERAL_REGISTERS_DMA_CTRL (INCA_IP2_DMA + 0x10)
#define CHANNEL_RELATED_REGISTERS_DMA_CS (INCA_IP2_DMA + 0x18)
#define CHANNEL_RELATED_REGISTERS_DMA_CCTRL (INCA_IP2_DMA + 0x1C)
#define CHANNEL_RELATED_REGISTERS_DMA_CDBA (INCA_IP2_DMA + 0x20)
#define CHANNEL_RELATED_REGISTERS_DMA_CDLEN (INCA_IP2_DMA + 0x24)
#define CHANNEL_RELATED_REGISTERS_DMA_CIE (INCA_IP2_DMA + 0x2C)
#define CHANNEL_RELATED_REGISTERS_DMA_CIS (INCA_IP2_DMA + 0x28)
#define CHANNEL_RELATED_REGISTERS_DMA_CPOLL (INCA_IP2_DMA + 0x14)
	
#define PORT_RELATED_REGISTERS_DMA_PS (INCA_IP2_DMA + 0x40)
#define PORT_RELATED_REGISTERS_DMA_PCTRL (INCA_IP2_DMA + 0x44)
	
#define INTERRUPT_NODE_REGISTERS_DMA_IRNEN (INCA_IP2_DMA + 0xF4)
#define INTERRUPT_NODE_REGISTERS_DMA_IRNCR (INCA_IP2_DMA + 0xF8)
#define INTERRUPT_NODE_REGISTERS_DMA_IRNICR (INCA_IP2_DMA + 0xFC)

#if 0
/* ISR */
#define DMA_ISR_RDERR		0x20
#define DMA_ISR_CMDCPT		0x10
#define DMA_ISR_CPT			0x8
#define DMA_ISR_DURR		0x4
#define DMA_ISR_EOP			0x2
#endif
#define DMA_RESET_CHANNEL       0x00000002
#define DMA_ENABLE_CHANNEL       0x00000001
#define DMA_DESC_BYTEOFF_SHIFT		22

#define DMA_POLLING_ENABLE		0x80000000
#define DMA_POLLING_CNT			0x50	/*minimum 0x10, max 0xfff0*/

/***********************************************************************/
/*  Module      :  ICU register address and bits                       */
/***********************************************************************/

#define INCA_IP2_ICU                          (KSEG1+0x1F880200)
/***********************************************************************/

#define INCA_IP2_ICU_IM0_ISR                      ((volatile u32*)(INCA_IP2_ICU + 0x0000))
#define INCA_IP2_ICU_IM0_IER                      ((volatile u32*)(INCA_IP2_ICU + 0x0008))
#define INCA_IP2_ICU_IM0_IOSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0010))
#define INCA_IP2_ICU_IM0_IRSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0018))
#define INCA_IP2_ICU_IM0_IMR                      ((volatile u32*)(INCA_IP2_ICU + 0x0020))
#define INCA_IP2_ICU_IM0_IMR_IID                  (1 << 31)
#define INCA_IP2_ICU_IM0_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define INCA_IP2_ICU_IM0_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define INCA_IP2_ICU_IM0_IR(value)                (1 << (value))

#define INCA_IP2_ICU_IM1_ISR                      ((volatile u32*)(INCA_IP2_ICU + 0x0028))
#define INCA_IP2_ICU_IM1_IER                      ((volatile u32*)(INCA_IP2_ICU + 0x0030))
#define INCA_IP2_ICU_IM1_IOSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0038))
#define INCA_IP2_ICU_IM1_IRSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0040))
#define INCA_IP2_ICU_IM1_IMR                      ((volatile u32*)(INCA_IP2_ICU + 0x0048))
#define INCA_IP2_ICU_IM1_IMR_IID                  (1 << 31)
#define INCA_IP2_ICU_IM1_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define INCA_IP2_ICU_IM1_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define INCA_IP2_ICU_IM1_IR(value)                (1 << (value))

#define INCA_IP2_ICU_IM2_ISR                      ((volatile u32*)(INCA_IP2_ICU + 0x0050))
#define INCA_IP2_ICU_IM2_IER                      ((volatile u32*)(INCA_IP2_ICU + 0x0058))
#define INCA_IP2_ICU_IM2_IOSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0060))
#define INCA_IP2_ICU_IM2_IRSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0068))
#define INCA_IP2_ICU_IM2_IMR                      ((volatile u32*)(INCA_IP2_ICU + 0x0070))
#define INCA_IP2_ICU_IM2_IMR_IID                  (1 << 31)
#define INCA_IP2_ICU_IM2_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define INCA_IP2_ICU_IM2_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define INCA_IP2_ICU_IM2_IR(value)                (1 << (value))

#define INCA_IP2_ICU_IM3_ISR                      ((volatile u32*)(INCA_IP2_ICU + 0x0078))
#define INCA_IP2_ICU_IM3_IER                      ((volatile u32*)(INCA_IP2_ICU + 0x0080))
#define INCA_IP2_ICU_IM3_IOSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0088))
#define INCA_IP2_ICU_IM3_IRSR                     ((volatile u32*)(INCA_IP2_ICU + 0x0090))
#define INCA_IP2_ICU_IM3_IMR                      ((volatile u32*)(INCA_IP2_ICU + 0x0098))
#define INCA_IP2_ICU_IM3_IMR_IID                  (1 << 31)
#define INCA_IP2_ICU_IM3_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define INCA_IP2_ICU_IM3_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define INCA_IP2_ICU_IM3_IR(value)                (1 << (value))

#define INCA_IP2_ICU_IM4_ISR                      ((volatile u32*)(INCA_IP2_ICU + 0x00A0))
#define INCA_IP2_ICU_IM4_IER                      ((volatile u32*)(INCA_IP2_ICU + 0x00A8))
#define INCA_IP2_ICU_IM4_IOSR                     ((volatile u32*)(INCA_IP2_ICU + 0x00B0))
#define INCA_IP2_ICU_IM4_IRSR                     ((volatile u32*)(INCA_IP2_ICU + 0x00B8))
#define INCA_IP2_ICU_IM4_IMR                      ((volatile u32*)(INCA_IP2_ICU + 0x00C0))
#define INCA_IP2_ICU_IM4_IMR_IID                  (1 << 31)
#define INCA_IP2_ICU_IM4_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define INCA_IP2_ICU_IM4_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define INCA_IP2_ICU_IM4_IR(value)                (1 << (value))

#define INCA_IP2_ICU_IM5_ISR                      ((volatile u32*)(INCA_IP2_ICU + 0x00C8))
#define INCA_IP2_ICU_IM5_IER                      ((volatile u32*)(INCA_IP2_ICU + 0x00D0))
#define INCA_IP2_ICU_IM5_IOSR                     ((volatile u32*)(INCA_IP2_ICU + 0x00D8))
#define INCA_IP2_ICU_IM5_IRSR                     ((volatile u32*)(INCA_IP2_ICU + 0x00E0))
#define INCA_IP2_ICU_IM5_IMR                      ((volatile u32*)(INCA_IP2_ICU + 0x00E8))
#define INCA_IP2_ICU_IM5_IMR_IID                  (1 << 31)
#define INCA_IP2_ICU_IM5_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define INCA_IP2_ICU_IM5_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define INCA_IP2_ICU_IM5_IR(value)                (1 << (value))


/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/

#define INCA_IP2_CGU                          (KSEG1+0x1F100800)
/***********************************************************************/

#define INCA_IP2_CGU_PLL2CR                       ((volatile u32*)(INCA_IP2_CGU + 0x0008))
#define INCA_IP2_CGU_FBSCR                        ((volatile u32*)(INCA_IP2_CGU + 0x0018))
#define INCA_IP2_CGU_FBSCR_LPBSDIV_GET(value)     (((value) >> 6) & ((1 << 2) - 1))
#define INCA_IP2_CGU_FBSCR_DIV0_GET(value)        (((value) >> 0) & ((1 << 3) - 1))
#define INCA_IP2_CGU_FBSCR_DIV1_GET(value)        (((value) >> 4) & ((1 << 2) - 1))

/***********************************************************************/
/*  Module      :  MPS register address and bits                       */
/***********************************************************************/

#define INCA_IP2_MPS                          (KSEG1+0x1F101400)
/***********************************************************************/

#define INCA_IP2_MPS_CHIPID                       ((volatile u32*)(INCA_IP2_MPS + 0x0344))
#define INCA_IP2_MPS_CHIPID_VERSION_GET(value)    (((value) >> 28) & ((1 << 4) - 1))
#define INCA_IP2_MPS_CHIPID_VERSION_SET(value)    (((( 1 << 4) - 1) & (value)) << 28)
#define INCA_IP2_MPS_CHIPID_PARTNUM_GET(value)    (((value) >> 12) & ((1 << 16) - 1))
#define INCA_IP2_MPS_CHIPID_PARTNUM_SET(value)    (((( 1 << 16) - 1) & (value)) << 12)
#define INCA_IP2_MPS_CHIPID_MANID_GET(value)      (((value) >> 1) & ((1 << 10) - 1))
#define INCA_IP2_MPS_CHIPID_MANID_SET(value)      (((( 1 << 10) - 1) & (value)) << 1)


/* voice channel 0 ... 3 interrupt enable register */
#define INCA_IP2_MPS_VC0ENR ((volatile u32*)(INCA_IP2_MPS + 0x0000))
#define INCA_IP2_MPS_VC1ENR ((volatile u32*)(INCA_IP2_MPS + 0x0004))
#define INCA_IP2_MPS_VC2ENR ((volatile u32*)(INCA_IP2_MPS + 0x0008))
#define INCA_IP2_MPS_VC3ENR ((volatile u32*)(INCA_IP2_MPS + 0x000C))
/* voice channel 0 ... 3 interrupt status read register */
#define INCA_IP2_MPS_RVC0SR ((volatile u32*)(INCA_IP2_MPS + 0x0010))
#define INCA_IP2_MPS_RVC1SR ((volatile u32*)(INCA_IP2_MPS + 0x0014))
#define INCA_IP2_MPS_RVC2SR ((volatile u32*)(INCA_IP2_MPS + 0x0018))
#define INCA_IP2_MPS_RVC3SR ((volatile u32*)(INCA_IP2_MPS + 0x001C))
/* voice channel 0 ... 3 interrupt status set register */
#define INCA_IP2_MPS_SVC0SR ((volatile u32*)(INCA_IP2_MPS + 0x0020))
#define INCA_IP2_MPS_SVC1SR ((volatile u32*)(INCA_IP2_MPS + 0x0024))
#define INCA_IP2_MPS_SVC2SR ((volatile u32*)(INCA_IP2_MPS + 0x0028))
#define INCA_IP2_MPS_SVC3SR ((volatile u32*)(INCA_IP2_MPS + 0x002C))
/* voice channel 0 ... 3 interrupt status clear register */
#define INCA_IP2_MPS_CVC0SR ((volatile u32*)(INCA_IP2_MPS + 0x0030))
#define INCA_IP2_MPS_CVC1SR ((volatile u32*)(INCA_IP2_MPS + 0x0034))
#define INCA_IP2_MPS_CVC2SR ((volatile u32*)(INCA_IP2_MPS + 0x0038))
#define INCA_IP2_MPS_CVC3SR ((volatile u32*)(INCA_IP2_MPS + 0x003C))
/* common status 0 and 1 read register */
#define INCA_IP2_MPS_RAD0SR ((volatile u32*)(INCA_IP2_MPS + 0x0040))
#define INCA_IP2_MPS_RAD1SR ((volatile u32*)(INCA_IP2_MPS + 0x0044))
/* common status 0 and 1 set register */
#define INCA_IP2_MPS_SAD0SR ((volatile u32*)(INCA_IP2_MPS + 0x0048))
#define INCA_IP2_MPS_SAD1SR ((volatile u32*)(INCA_IP2_MPS + 0x004C))
/* common status 0 and 1 clear register */
#define INCA_IP2_MPS_CAD0SR ((volatile u32*)(INCA_IP2_MPS + 0x0050))
#define INCA_IP2_MPS_CAD1SR ((volatile u32*)(INCA_IP2_MPS + 0x0054))
/* notification enable register */
#define INCA_IP2_MPS_CPU0_NFER ((volatile u32*)(INCA_IP2_MPS + 0x0060))
#define INCA_IP2_MPS_CPU1_NFER ((volatile u32*)(INCA_IP2_MPS + 0x0064))
/* CPU to CPU interrup request register */
#define INCA_IP2_MPS_CPU0_2_CPU1_IRR ((volatile u32*)(INCA_IP2_MPS + 0x0070))
#define INCA_IP2_MPS_CPU0_2_CPU1_IER ((volatile u32*)(INCA_IP2_MPS + 0x0074))
/* Global interrupt request and request enable register */
#define INCA_IP2_MPS_GIRR ((volatile u32*)(INCA_IP2_MPS + 0x0078))
#define INCA_IP2_MPS_GIER ((volatile u32*)(INCA_IP2_MPS + 0x007C))

/* Addresses of enable registers not yet defined 
#define INCA_IP2_MPS_AD0ENR ((volatile u32*)(INCA_IP2_MPS + 0x????))
#define INCA_IP2_MPS_AD1ENR ((volatile u32*)(INCA_IP2_MPS + 0x????))
*/


/***********************************************************************/
/*  Module      :  ASC0 register address and bits                      */
/***********************************************************************/

#define INCA_IP2_ASC0                          (KSEG1+0x1E000400)
/***********************************************************************/

#define INCA_IP2_ASC0_TBUF                        ((volatile u32*)(INCA_IP2_ASC0 + 0x0020))
#define INCA_IP2_ASC0_RBUF                        ((volatile u32*)(INCA_IP2_ASC0 + 0x0024))
#define INCA_IP2_ASC0_FSTAT                       ((volatile u32*)(INCA_IP2_ASC0 + 0x0048))
#define INCA_IP2_ASC0_FSTAT_TXFREE_GET(value)     (((value) >> 24) & ((1 << 6) - 1))
#define INCA_IP2_ASC0_FSTAT_TXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 24)
#define INCA_IP2_ASC0_FSTAT_RXFREE_GET(value)     (((value) >> 16) & ((1 << 6) - 1))
#define INCA_IP2_ASC0_FSTAT_RXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 16)
#define INCA_IP2_ASC0_FSTAT_TXFFL_GET(value)      (((value) >> 8) & ((1 << 6) - 1))
#define INCA_IP2_ASC0_FSTAT_TXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 8)
#define INCA_IP2_ASC0_FSTAT_RXFFL_GET(value)      (((value) >> 0) & ((1 << 6) - 1))
#define INCA_IP2_ASC0_FSTAT_RXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 0)


/***********************************************************************/
/*  Module      :  ASC1 register address and bits                      */
/***********************************************************************/

#define INCA_IP2_ASC1                          (KSEG1+0x1E000800)
/***********************************************************************/

#define INCA_IP2_ASC1_TBUF                        ((volatile u32*)(INCA_IP2_ASC1 + 0x0020))
#define INCA_IP2_ASC1_RBUF                        ((volatile u32*)(INCA_IP2_ASC1 + 0x0024))
#define INCA_IP2_ASC1_FSTAT                       ((volatile u32*)(INCA_IP2_ASC1 + 0x0048))
#define INCA_IP2_ASC1_FSTAT_TXFREE_GET(value)     (((value) >> 24) & ((1 << 6) - 1))
#define INCA_IP2_ASC1_FSTAT_TXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 24)
#define INCA_IP2_ASC1_FSTAT_RXFREE_GET(value)     (((value) >> 16) & ((1 << 6) - 1))
#define INCA_IP2_ASC1_FSTAT_RXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 16)
#define INCA_IP2_ASC1_FSTAT_TXFFL_GET(value)      (((value) >> 8) & ((1 << 6) - 1))
#define INCA_IP2_ASC1_FSTAT_TXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 8)
#define INCA_IP2_ASC1_FSTAT_RXFFL_GET(value)      (((value) >> 0) & ((1 << 6) - 1))
#define INCA_IP2_ASC1_FSTAT_RXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 0)


/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/

#define INCA_IP2_RCU                          (KSEG1+0x1E001C00)
/***********************************************************************/

/***Reset Request Register***/ 
#define INCA_IP2_RCU_RST_REQ                      ((volatile u32*)(INCA_IP2_RCU + 0x0000))
#define INCA_IP2_RCU_RST_REQ_CPU0                 (1 << 31)
#define INCA_IP2_RCU_RST_REQ_CPU1                 (1 << 30)
#define INCA_IP2_RCU_RST_REQ_CPUSUB               (1 << 29)
#define INCA_IP2_RCU_RST_REQ_HRST                 (1 << 28)
#define INCA_IP2_RCU_RST_REQ_WDT0                 (1 << 27)
#define INCA_IP2_RCU_RST_REQ_WDT1                 (1 << 26)
#define INCA_IP2_RCU_RST_REQ_CFG_GET(value)       (((value) >> 23) & ((1 << 3) - 1))
#define INCA_IP2_RCU_RST_REQ_CFG_SET(value)       (((( 1 << 3) - 1) & (value)) << 23)
#define INCA_IP2_RCU_RST_REQ_SWTBOOT              (1 << 22)
#define INCA_IP2_RCU_RST_REQ_DMA                  (1 << 21)
#define INCA_IP2_RCU_RST_REQ_ETHPHY1              (1 << 20)
#define INCA_IP2_RCU_RST_REQ_ETHPHY0              (1 << 19)
#define INCA_IP2_RCU_RST_REQ_CPU0_BR              (1 << 18)

/* CPU0, CPU1, CPUSUB, HRST, WDT0, WDT1, DMA, ETHPHY1, ETHPHY0 */
#define INCA_IP2_RCU_RST_REQ_ALL                  0xFC380000 

/***NMI Status Register***/ 
#define INCA_IP2_RCU_NMISR                        ((volatile u32*)(INCA_IP2_RCU + 0x00F4))
#define INCA_IP2_RCU_NMISR_NMIEXT                 (1 << 2)
#define INCA_IP2_RCU_NMISR_NMIPLL2                (1 << 1)
#define INCA_IP2_RCU_NMISR_NMIPLL1                (1 << 0)


/***********************************************************************/
/*  Module      :  WDT register address and bits                       */
/***********************************************************************/
         
#define INCA_IP2_WDT                           (KSEG1+0x1F880000)
/***********************************************************************/

/***Watchdog Timer Control Register ***/ 
#define INCA_IP2_WDT_BIU_WDT_CR                   ((volatile u32*)(INCA_IP2_WDT + 0x03F0))
#define INCA_IP2_WDT_BIU_WDT_CR_GEN               (1 << 31)
#define INCA_IP2_WDT_BIU_WDT_CR_DSEN              (1 << 30)
#define INCA_IP2_WDT_BIU_WDT_CR_LPEN              (1 << 29)
#define INCA_IP2_WDT_BIU_WDT_CR_PWL_GET(value)    (((value) >> 26) & ((1 << 2) - 1))
#define INCA_IP2_WDT_BIU_WDT_CR_PWL_SET(value)    (((( 1 << 2) - 1) & (value)) << 26)
#define INCA_IP2_WDT_BIU_WDT_CR_CLKDIV_GET(value) (((value) >> 24) & ((1 << 2) - 1))
#define INCA_IP2_WDT_BIU_WDT_CR_CLKDIV_SET(value) (((( 1 << 2) - 1) & (value)) << 24)
#define INCA_IP2_WDT_BIU_WDT_CR_PW_GET(value)     (((value) >> 16) & ((1 << 8) - 1))
#define INCA_IP2_WDT_BIU_WDT_CR_PW_SET(value)     (((( 1 << 8) - 1) & (value)) << 16)
#define INCA_IP2_WDT_BIU_WDT_CR_RELOAD_GET(value) (((value) >> 0) & ((1 << 16) - 1))
#define INCA_IP2_WDT_BIU_WDT_CR_RELOAD_SET(value) (((( 1 << 16) - 1) & (value)) << 0)

/***Watchdog Timer Status Register***/ 
#define INCA_IP2_WDT_BIU_WDT_SR                   ((volatile u32*)(INCA_IP2_WDT + 0x03F8))
#define INCA_IP2_WDT_BIU_WDT_SR_EN                (1 << 31)
#define INCA_IP2_WDT_BIU_WDT_SR_AE                (1 << 30)
#define INCA_IP2_WDT_BIU_WDT_SR_PRW               (1 << 29)
#define INCA_IP2_WDT_BIU_WDT_SR_EXP               (1 << 28)
#define INCA_IP2_WDT_BIU_WDT_SR_PWD               (1 << 27)
#define INCA_IP2_WDT_BIU_WDT_SR_DS                (1 << 26)
#define INCA_IP2_WDT_BIU_WDT_SR_VALUE_GET(value)  (((value) >> 0) & ((1 << 16) - 1))
#define INCA_IP2_WDT_BIU_WDT_SR_VALUE_SET(value)  (((( 1 << 16) - 1) & (value)) << 0)


/***********************************************************************/
/*  Module      :  BCU0 register address and bits                      */
/***********************************************************************/

#define INCA_IP2_BCU0                           (KSEG1+0x14100000)
/***********************************************************************/

#define INCA_IP2_BCU0_CON                         ((volatile u32*)(INCA_IP2_BCU0 + 0x0010))
#define INCA_IP2_BCU0_ECON                        ((volatile u32*)(INCA_IP2_BCU0 + 0x0020))
#define INCA_IP2_BCU0_EADD                        ((volatile u32*)(INCA_IP2_BCU0 + 0x0024))
#define INCA_IP2_BCU0_EDAT                        ((volatile u32*)(INCA_IP2_BCU0 + 0x0028))
#define INCA_IP2_BCU0_IRNCR1                      ((volatile u32*)(INCA_IP2_BCU0 + 0x00F8))
#define INCA_IP2_BCU0_IRNCR0                      ((volatile u32*)(INCA_IP2_BCU0 + 0x00FC))


/***********************************************************************/
/*  Module      :  BCU1 register address and bits                      */
/***********************************************************************/

#define INCA_IP2_BCU1                           (KSEG1+0x1E000000)
/***********************************************************************/

#define INCA_IP2_BCU1_CON                         ((volatile u32*)(INCA_IP2_BCU1 + 0x0010))
#define INCA_IP2_BCU1_ECON                        ((volatile u32*)(INCA_IP2_BCU1 + 0x0020))
#define INCA_IP2_BCU1_EADD                        ((volatile u32*)(INCA_IP2_BCU1 + 0x0024))
#define INCA_IP2_BCU1_EDAT                        ((volatile u32*)(INCA_IP2_BCU1 + 0x0028))
#define INCA_IP2_BCU1_IRNCR1                      ((volatile u32*)(INCA_IP2_BCU1 + 0x00F8))
#define INCA_IP2_BCU1_IRNCR0                      ((volatile u32*)(INCA_IP2_BCU1 + 0x00FC))


/***********************************************************************/
/*  Module      :  MC register address and bits                        */
/***********************************************************************/

#define INCA_IP2_MC                             (KSEG1+0x1F800000)
/***********************************************************************/

#define INCA_IP2_MC_ERRCAUSE                      ((volatile u32*)(INCA_IP2_MC + 0x0010))
#define INCA_IP2_MC_ERRADDR                       ((volatile u32*)(INCA_IP2_MC + 0x0020))
#define INCA_IP2_MC_CON                           ((volatile u32*)(INCA_IP2_MC + 0x0060))

/***********************************************************************/
/*  Module      :  MC SDRAM register address and bits                        */
/***********************************************************************/
#define INCA_IP2_SDRAM                          (KSEG1+0x1F800200)
/***********************************************************************/
#define INCA_IP2_SDRAM_MC_CFGPB0                  ((volatile u32*)(INCA_IP2_SDRAM + 0x0040))

/***********************************************************************/
/*  Module      :  MC DDR register address and bits                        */
/***********************************************************************/
#define INCA_IP2_DDR                            (KSEG1+0x1F801000)
/***********************************************************************/
#define INCA_IP2_DDR_MC_DC19                      ((volatile u32*)(INCA_IP2_DDR + 0x0130))
#define INCA_IP2_DDR_MC_DC20                      ((volatile u32*)(INCA_IP2_DDR + 0x0140))


/***********************************************************************/
/*  Module      :  PMS register address and bits                       */
/***********************************************************************/

#define INCA_IP2_PMS     (KSEG1 + 0x1F100C00)

#define INCA_IP2_PMS_PMS_SR				((volatile u32*) (INCA_IP2_PMS + 0x0000))
#define INCA_IP2_PMS_PMS_SR_ASC1			(1 << 14)
#define INCA_IP2_PMS_PMS_SR_ASC0			(1 << 13)
#define INCA_IP2_PMS_PMS_GEN				((volatile u32*) (INCA_IP2_PMS + 0x0004))
#define INCA_IP2_PMS_PMS_GEN_DMA			(1 << 16)
#define INCA_IP2_PMS_PMS_GEN_ASC1			(1 << 14)
#define INCA_IP2_PMS_PMS_GEN_ASC0			(1 << 13)
#define INCA_IP2_PMS_PMS_GEN_SPI0			(1 << 11)
#define INCA_IP2_PMS_PMS_GEN_SPI1			(1 << 12)
#define INCA_IP2_PMS_PMS_CFG				((volatile u32*) (INCA_IP2_PMS + 0x0008))


/***********************************************************************/
/*  Module      :  GPIO register address and bits                       */
/***********************************************************************/

#define INCA_IP2_GPIO     (KSEG1 + 0x1F102600)

#define INCA_IP2_GPIO_OUT		((volatile u32*) (INCA_IP2_GPIO + 0x0000))
#define INCA_IP2_GPIO_IN			((volatile u32*) (INCA_IP2_GPIO + 0x0004))
#define INCA_IP2_GPIO_DIR		((volatile u32*) (INCA_IP2_GPIO + 0x0008))
#define INCA_IP2_GPIO_ALTSEL1		((volatile u32*) (INCA_IP2_GPIO + 0x000C))
#define INCA_IP2_GPIO_ALTSEL2		((volatile u32*) (INCA_IP2_GPIO + 0x0010))
#define INCA_IP2_GPIO_STOFF		((volatile u32*) (INCA_IP2_GPIO + 0x0014))
#define INCA_IP2_GPIO_OD			((volatile u32*) (INCA_IP2_GPIO + 0x0018))
#define INCA_IP2_GPIO_PUDEB		((volatile u32*) (INCA_IP2_GPIO + 0x001C))

/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/

#define INCA_IP2_RCU                          (KSEG1+0x1E001C00)
/***********************************************************************/

/***Reset Request Register***/ 
#define INCA_IP2_RCU_RST_REQ                      ((volatile u32*)(INCA_IP2_RCU + 0x0000))
#define INCA_IP2_RCU_RST_REQ_CPU0                 (1 << 31)
#define INCA_IP2_RCU_RST_REQ_CPU1                 (1 << 30)
#define INCA_IP2_RCU_RST_REQ_CPUSUB               (1 << 29)
#define INCA_IP2_RCU_RST_REQ_HRST                 (1 << 28)
#define INCA_IP2_RCU_RST_REQ_WDT0                 (1 << 27)
#define INCA_IP2_RCU_RST_REQ_WDT1                 (1 << 26)
#define INCA_IP2_RCU_RST_REQ_CFG_GET(value)       (((value) >> 23) & ((1 << 3) - 1))
#define INCA_IP2_RCU_RST_REQ_CFG_SET(value)       (((( 1 << 3) - 1) & (value)) << 23)
#define INCA_IP2_RCU_RST_REQ_SWTBOOT              (1 << 22)
#define INCA_IP2_RCU_RST_REQ_DMA                  (1 << 21)
#define INCA_IP2_RCU_RST_REQ_ETHPHY1              (1 << 20)
#define INCA_IP2_RCU_RST_REQ_ETHPHY0              (1 << 19)
#define INCA_IP2_RCU_RST_REQ_CPU0_BR              (1 << 18)

/* CPU0, CPU1, CPUSUB, HRST, WDT0, WDT1, DMA, ETHPHY1, ETHPHY0 */
#define INCA_IP2_RCU_RST_REQ_ALL                  0xFC380000 

/***Reset Status Register***/ 
#define INCA_IP2_RCU_SR                           ((volatile u32*)(INCA_IP2_RCU + 0x0008))

/***NMI Status Register***/ 
#define INCA_IP2_RCU_NMISR                        ((volatile u32*)(INCA_IP2_RCU + 0x00F4))
#define INCA_IP2_RCU_NMISR_NMIEXT                 (1 << 2)
#define INCA_IP2_RCU_NMISR_NMIPLL2                (1 << 1)
#define INCA_IP2_RCU_NMISR_NMIPLL1                (1 << 0)

/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/

#define INCA_IP2_EBU                          (KSEG1+0x14102000)
/***********************************************************************/

#define INCA_IP2_EBU_ADDSEL0                      ((volatile u32*)(INCA_IP2_EBU + 0x0020))
#define INCA_IP2_EBU_ADDSEL1                      ((volatile u32*)(INCA_IP2_EBU + 0x0024))
#define INCA_IP2_EBU_ADDSEL2                      ((volatile u32*)(INCA_IP2_EBU + 0x0028))
#define INCA_IP2_EBU_ADDSEL3                      ((volatile u32*)(INCA_IP2_EBU + 0x002C))
#define INCA_IP2_EBU_CON0                      ((volatile u32*)(INCA_IP2_EBU + 0x0060))
#define INCA_IP2_EBU_CON1                      ((volatile u32*)(INCA_IP2_EBU + 0x0064))
#define INCA_IP2_EBU_CON2                      ((volatile u32*)(INCA_IP2_EBU + 0x0068))
#define INCA_IP2_EBU_CON3                      ((volatile u32*)(INCA_IP2_EBU + 0x006C))
#define INCA_IP2_EBU_CON_WRDIS                     (1 << 31)




/***********************************************************************/
/*  Module      :  SWITCH register address and bits                       */
/***********************************************************************/

#define INCA_IP2_SWITCH				(KSEG1+0x18000000)
/***********************************************************************/

/* PR Base address */
#define PR_BASE		(INCA_IP2_SWITCH + 0x00008000)

/* SE Base Address */
#define SE_BASE		(INCA_IP2_SWITCH + 0x00009000)

#define PR_CTRL_REG				(PR_BASE + 0x0000)	
#define MA_LEARN_REG			(PR_BASE + 0x0004)
#define DST_LOOKUP_REG			(PR_BASE + 0x0008)

#define COS_SEL_REG				(PR_BASE + 0x000c)
#define PRI2_COS_REG			(PR_BASE + 0x0010)		
#define UNKNOWN_DEST_REG		(PR_BASE + 0x0014)
	
#define CPU_ACS_CTRL_REG		(PR_BASE + 0x0018)
#define CPU_ACS_DATA_REG		(PR_BASE + 0x001c)

#define MA_READ_REG				(PR_BASE + 0x0020)
#define TB_CTRL_REG				(PR_BASE + 0x0024)
#define RATE_REG				(PR_BASE + 0x0028)
#define BURST_REG				(PR_BASE + 0x0048)
#define EBURST_REG				(PR_BASE + 0x0068)

#define RULE_SEL_REG			(PR_BASE + 0x0088)

#define GEN_SFT_AGE_STB         (PR_BASE + 0x008C)
#define PR_ISR_REG				(PR_BASE + 0x0090)
#define PR_IMR_REG				(PR_BASE + 0x0094)
#define PR_IPR_REG				(PR_BASE + 0x0098)
#define BPDU_REG				(PR_BASE + 0x00A4)

/*	Switching Engine Register Description	*/
#define QLL_CMD_REG				(SE_BASE)
#define QLL_DATA_REG0			(SE_BASE + 0x0004)
#define QLL_DATA_REG1			(SE_BASE + 0x0008)

#define VLAN_MIBS_CMD_REG		(SE_BASE + 0x000c)
#define VLAN_MIBS_DATA_REG		(SE_BASE + 0x0010)

#define SD_CMD_REG				(SE_BASE + 0x0014)
#define SD_DATA_REGS0			(SE_BASE + 0x0018)
#define SD_DATA_REGS1			(SE_BASE + 0x001C)
#define SD_DATA_REGS2			(SE_BASE + 0x0020)

#define VLAN_TBL_CMD_REG		(SE_BASE + 0x0024)
#define VLAN_TBL_DATA_REG		(SE_BASE + 0x0028)

#define FD_TBL_CMD_REG			(SE_BASE + 0x002c)
#define FD_TBL_DATA_REG			(SE_BASE + 0x0030)

#define SYMM_VLAN_REG			(SE_BASE + 0x0038)
#define PORT_AUTH				(SE_BASE + 0x0048)
#define CPU_LINK_OK_REG     	(SE_BASE + 0x0050)
/* #define TRUNK_CTRL_REGS			(SE_BASE + 0x0054)  */
#define MIRROR_PORT_REG			(SE_BASE + 0x0064)

#define ST_PT_REG				(SE_BASE + 0x0068)
#define JUMBO_ENABLE_REG		(SE_BASE + 0x006C)
#define STACK_PORT_REG			(SE_BASE + 0x0074)
#define EG_MON_REG              (SE_BASE + 0x007C)
#define VR_MIB_REG		        (SE_BASE + 0x0080)
#define QUEUE_CMD_REGS			(SE_BASE + 0x0090)

#define GLOBAL_RX_WM_REG		(SE_BASE + 0x0200)
#define PORT0_RX_WM_REG0		(SE_BASE + 0x0204)
#define PORT1_RX_WM_REG0		(SE_BASE + 0x0208)
#define PORT2_RX_WM_REG0		(SE_BASE + 0x020C)

#define PORT_RX_WM_REGS			(SE_BASE + 0x0200)
#define PORT_TX_WM_REGS			(SE_BASE + 0x0300)
#define PORT0_TX_WM_REG0		(SE_BASE + 0x0330)
#define PORT1_TX_WM_REG0		(SE_BASE + 0x0338)
#define PORT2_TX_WM_REG0		(SE_BASE + 0x0340)
#define PORT0_TX_WM_REG1		(SE_BASE + 0x0334)
#define PORT1_TX_WM_REG1		(SE_BASE + 0x033C)
#define PORT2_TX_WM_REG1		(SE_BASE + 0x0344)


#define QUEUE_STATUS_REGS		(SE_BASE + 0x0400)

#define SE_INT_STS_REG			(SE_BASE + 0x08e0)
#define SE_INT_MSK_REG_RD	    (SE_BASE + 0x08e4)
#define SE_INT_MSK_REG_WR		(SE_BASE + 0x08e8)
#define SE_INT_PRI_REG_RD		(SE_BASE + 0x08ec)
#define SE_INT_PRI_REG_WR		(SE_BASE + 0x08f0)  /* address too be defined*/

/***********************************************************************/
/*  Module      :  Ethernet Switch port related addresses and bits     */
/***********************************************************************/
#define GPORT0_BASE				(KSEG1+0x18006000)
#define GPORT1_BASE				(KSEG1+0x18007000)
#define GPORT2_BASE				(KSEG1+0x1800C000)

#define PORTREG_BASE GPORT0_BASE

#define SWITCH_P0_GMAC_REG (GPORT0_BASE + 0x0004)
#define SWITCH_P0_GMAC_CTRL (GPORT0_BASE + 0x000C)
#define SWITCH_P0_RTX_INT_STATUS (GPORT0_BASE + 0x0010)
#define SWITCH_P0_RTX_INT_MASK (GPORT0_BASE + 0x0014)
#define SWITCH_P0_INT_PRIORITY (GPORT0_BASE + 0x0018)
#define SWITCH_P0_RX_CONF (GPORT0_BASE + 0x0400)
#define SWITCH_P0_OFFSET0_REG (GPORT0_BASE + 0x0404)
#define SWITCH_P0_OFFSET1_REG (GPORT0_BASE + 0x0408)
#define SWITCH_P0_PORT_MASK0_REG (GPORT0_BASE + 0x0420)
#define SWITCH_P0_PORT_MASK1_REG (GPORT0_BASE + 0x0424)
#define SWITCH_P0_PORT_MASK2_REG (GPORT0_BASE + 0x0428)
#define SWITCH_P0_PORT_MASK3_REG (GPORT0_BASE + 0x042C)
#define SWITCH_P0_PORT_RULE0_REG (GPORT0_BASE + 0x0430)
#define SWITCH_P0_PORT_RULE1_REG (GPORT0_BASE + 0x0434)
#define SWITCH_P0_PORT_RULE2_REG (GPORT0_BASE + 0x0438)
#define SWITCH_P0_PORT_RULE3_REG (GPORT0_BASE + 0x043C)
#define SWITCH_P0_PORT_IKEY_SEL (GPORT0_BASE + 0x0440)
#define SWITCH_P0_PORT_RX_VLAN_ID (GPORT0_BASE + 0x0450)
#define SWITCH_P0_TX_CONF (GPORT0_BASE + 0x0800)
#define SWITCH_P0_PORT_TX_VLAN_ID (GPORT0_BASE + 0x0804)
#define SWITCH_P0_PORT_MIB_REG_0 (GPORT0_BASE + 0x0C00)
#define SWITCH_P0_GMAC_MIB_REG_0 (GPORT0_BASE + 0x0C54)

#define SWITCH_P1_GMAC_REG (GPORT1_BASE + 0x0004)
#define SWITCH_P1_GMAC_CTRL (GPORT1_BASE + 0x000C)
#define SWITCH_P1_RTX_INT_STATUS (GPORT1_BASE + 0x0010)
#define SWITCH_P1_RTX_INT_MASK (GPORT1_BASE + 0x0014)
#define SWITCH_P1_INT_PRIORITY (GPORT1_BASE + 0x0018)
#define SWITCH_P1_RX_CONF (GPORT1_BASE + 0x0400)
#define SWITCH_P1_OFFSET0_REG (GPORT1_BASE + 0x0404)
#define SWITCH_P1_OFFSET1_REG (GPORT1_BASE + 0x0408)
#define SWITCH_P1_PORT_MASK0_REG (GPORT1_BASE + 0x0420)
#define SWITCH_P1_PORT_MASK1_REG (GPORT1_BASE + 0x0424)
#define SWITCH_P1_PORT_MASK2_REG (GPORT1_BASE + 0x0428)
#define SWITCH_P1_PORT_MASK3_REG (GPORT1_BASE + 0x042C)
#define SWITCH_P1_PORT_RULE0_REG (GPORT1_BASE + 0x0430)
#define SWITCH_P1_PORT_RULE1_REG (GPORT1_BASE + 0x0434)
#define SWITCH_P1_PORT_RULE2_REG (GPORT1_BASE + 0x0438)
#define SWITCH_P1_PORT_RULE3_REG (GPORT1_BASE + 0x043C)
#define SWITCH_P1_PORT_IKEY_SEL (GPORT1_BASE + 0x0440)
#define SWITCH_P1_PORT_RX_VLAN_ID (GPORT1_BASE + 0x0450)
#define SWITCH_P1_TX_CONF (GPORT1_BASE + 0x0800)
#define SWITCH_P1_PORT_TX_VLAN_ID (GPORT1_BASE + 0x0804)
#define SWITCH_P1_PORT_MIB_REG_0 (GPORT1_BASE + 0x0C00)
#define SWITCH_P1_GMAC_MIB_REG_0 (GPORT1_BASE + 0x0C54)

#define SWITCH_P2_GMAC_REG (GPORT2_BASE + 0x0004)
#define SWITCH_P2_GMAC_CTRL (GPORT2_BASE + 0x000C)
#define SWITCH_P2_RTX_INT_STATUS (GPORT2_BASE + 0x0010)
#define SWITCH_P2_RTX_INT_MASK (GPORT2_BASE + 0x0014)
#define SWITCH_P2_INT_PRIORITY (GPORT2_BASE + 0x0018)
#define SWITCH_P2_MDIO_ID_1 (GPORT2_BASE + 0x00A8)
#define SWITCH_P2_PAUSE_CTL_1 (GPORT2_BASE + 0x00B0)
#define SWITCH_P2_MDIO_MOD_SEL (GPORT2_BASE + 0x00B4)
#define SWITCH_P2_MDIO_ACC_0 (GPORT2_BASE + 0x00B8)
#define SWITCH_P2_RX_CONF (GPORT2_BASE + 0x0400)
#define SWITCH_P2_OFFSET0_REG (GPORT2_BASE + 0x0404)
#define SWITCH_P2_OFFSET1_REG (GPORT2_BASE + 0x0408)
#define SWITCH_P2_PORT_MASK0_REG (GPORT2_BASE + 0x0420)
#define SWITCH_P2_PORT_MASK1_REG (GPORT2_BASE + 0x0424)
#define SWITCH_P2_PORT_MASK2_REG (GPORT2_BASE + 0x0428)
#define SWITCH_P2_PORT_MASK3_REG (GPORT2_BASE + 0x042C)
#define SWITCH_P2_PORT_RULE0_REG (GPORT2_BASE + 0x0430)
#define SWITCH_P2_PORT_RULE1_REG (GPORT2_BASE + 0x0434)
#define SWITCH_P2_PORT_RULE2_REG (GPORT2_BASE + 0x0438)
#define SWITCH_P2_PORT_RULE3_REG (GPORT2_BASE + 0x043C)
#define SWITCH_P2_PORT_IKEY_SEL (GPORT2_BASE + 0x0440)
#define SWITCH_P2_PORT_RX_VLAN_ID (GPORT2_BASE + 0x0450)
#define SWITCH_P2_TX_CONF (GPORT2_BASE + 0x0800)
#define SWITCH_P2_PORT_TX_VLAN_ID (GPORT2_BASE + 0x0804)
#define SWITCH_P2_PORT_MIB_REG_0 (GPORT2_BASE + 0x0C00)
#define SWITCH_P2_GMAC_MIB_REG_0 (GPORT2_BASE + 0x0C54)

#define MDIO_MOD_SEL SWITCH_P2_MDIO_MOD_SEL
#define SWITCH_MDIO_ACC SWITCH_P2_MDIO_ACC_0
#define SWITCH_MDIO_ID SWITCH_P2_MDIO_ID_1
/* #define TX_CONFIG_REG SWITCH_P0_TX_CONF  */

#define SWITCH_PMAC_HD_CTL (GPORT2_BASE + 0x0070)
#define SWITCH_PMAC_SA1 (GPORT2_BASE + 0x0074)
#define SWITCH_PMAC_SA2 (GPORT2_BASE + 0x0078)
#define SWITCH_PMAC_DA1 (GPORT2_BASE + 0x007C)
#define SWITCH_PMAC_DA2 (GPORT2_BASE + 0x0080)
#define SWITCH_PMAC_VLAN (GPORT2_BASE + 0x0084)
#define SWITCH_PMAC_TX_IPG (GPORT2_BASE + 0x0088)
#define SWITCH_PMAC_RX_IPG (GPORT2_BASE + 0x008C)

