#ifndef DANUBE_H
#define DANUBE_H
/******************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
******************************************************************************/

/***********************************************************************/
/*  Module      :  MEI register address and bits                       */
/***********************************************************************/
#define MEI_SPACE_ACCESS	0xB0100C00
#define MEI_DATA_XFR				(0x0000 + MEI_SPACE_ACCESS)
#define	MEI_VERSION				(0x0200 + MEI_SPACE_ACCESS)
#define	ARC_GP_STAT				(0x0204 + MEI_SPACE_ACCESS)
#define	MEI_XFR_ADDR				(0x020C + MEI_SPACE_ACCESS)
#define	MEI_TO_ARC_INT				(0x021C + MEI_SPACE_ACCESS)
#define	ARC_TO_MEI_INT				(0x0220 + MEI_SPACE_ACCESS)
#define	ARC_TO_MEI_INT_MASK			(0x0224 + MEI_SPACE_ACCESS)
#define	MEI_DEBUG_WAD				(0x0228 + MEI_SPACE_ACCESS)
#define MEI_DEBUG_RAD				(0x022C + MEI_SPACE_ACCESS)
#define	MEI_DEBUG_DATA				(0x0230 + MEI_SPACE_ACCESS)
#define	MEI_DEBUG_DEC				(0x0234 + MEI_SPACE_ACCESS)
#define	MEI_CONTROL				(0x0238 + MEI_SPACE_ACCESS)
#define	AT_CELLRDY_BC0				(0x023C + MEI_SPACE_ACCESS)
#define	AT_CELLRDY_BC1				(0x0240 + MEI_SPACE_ACCESS)
#define	AR_CELLRDY_BC0				(0x0244 + MEI_SPACE_ACCESS)
#define	AR_CELLRDY_BC1				(0x0248 + MEI_SPACE_ACCESS)
#define	AAI_ACCESS				(0x024C + MEI_SPACE_ACCESS)
#define	AAITXCB0				(0x0300 + MEI_SPACE_ACCESS)
#define	AAITXCB1				(0x0304 + MEI_SPACE_ACCESS)
#define	AAIRXCB0				(0x0308 + MEI_SPACE_ACCESS)
#define	AAIRXCB1				(0x030C + MEI_SPACE_ACCESS)


/***********************************************************************/
/*  Module      :  WDT register address and bits                       */
/***********************************************************************/
#define DANUBE_BIU_WDT_BASE             (0xBf8803F0)
#define DANUBE_BIU_WDT_CR     		(0x0000 + DANUBE_BIU_WDT_BASE)
#define DANUBE_BIU_WDT_SR     		(0x0008 + DANUBE_BIU_WDT_BASE)


/***********************************************************************/
/*  Module      :  PMU register address and bits                       */
/***********************************************************************/
#define DANUBE_PMU_BASE_ADDR 				(KSEG1+0x1F102000)

/***PM Control Register***/
#define DANUBE_PMU_CR 					((volatile u32*)(0x001C + DANUBE_PMU_BASE_ADDR))
#define DANUBE_PMU_PWDCR				DANUBE_PMU_CR
#define DANUBE_PMU_SR 					((volatile u32*)(0x0020 + DANUBE_PMU_BASE_ADDR))

#define DANUBE_PMU_DMA_SHIFT                    5
#define DANUBE_PMU_PPE_SHIFT                    13
#define DANUBE_PMU_ETOP_SHIFT                   22
#define DANUBE_PMU_ENET0_SHIFT                  24
#define DANUBE_PMU_ENET1_SHIFT                  25


#define DANUBE_PMU 					DANUBE_PMU_BASE_ADDR
/***PM Global Enable Register***/
#define DANUBE_PMU_PM_GEN                       ((volatile u32*)(DANUBE_PMU+ 0x0000))
#define DANUBE_PMU_PM_GEN_EN16                            (1 << 16)
#define DANUBE_PMU_PM_GEN_EN15                            (1 << 15)
#define DANUBE_PMU_PM_GEN_EN14                            (1 << 14)
#define DANUBE_PMU_PM_GEN_EN13                            (1 << 13)
#define DANUBE_PMU_PM_GEN_EN12                            (1 << 12)
#define DANUBE_PMU_PM_GEN_EN11                            (1 << 11)
#define DANUBE_PMU_PM_GEN_EN10                            (1 << 10)
#define DANUBE_PMU_PM_GEN_EN9                              (1 << 9)
#define DANUBE_PMU_PM_GEN_EN8                              (1 << 8)
#define DANUBE_PMU_PM_GEN_EN7                              (1 << 7)
#define DANUBE_PMU_PM_GEN_EN6                              (1 << 6)
#define DANUBE_PMU_PM_GEN_EN5                              (1 << 5)
#define DANUBE_PMU_PM_GEN_EN4                              (1 << 4)
#define DANUBE_PMU_PM_GEN_EN3                              (1 << 3)
#define DANUBE_PMU_PM_GEN_EN2                              (1 << 2)
#define DANUBE_PMU_PM_GEN_EN0                              (1 << 0)

/***PM Power Down Enable Register***/
#define DANUBE_PMU_PM_PDEN                      ((volatile u32*)(DANUBE_PMU+ 0x0008))
#define DANUBE_PMU_PM_PDEN_EN16                            (1 << 16)
#define DANUBE_PMU_PM_PDEN_EN15                            (1 << 15)
#define DANUBE_PMU_PM_PDEN_EN14                            (1 << 14)
#define DANUBE_PMU_PM_PDEN_EN13                            (1 << 13)
#define DANUBE_PMU_PM_PDEN_EN12                            (1 << 12)
#define DANUBE_PMU_PM_PDEN_EN11                            (1 << 11)
#define DANUBE_PMU_PM_PDEN_EN10                            (1 << 10)
#define DANUBE_PMU_PM_PDEN_EN9                              (1 << 9)
#define DANUBE_PMU_PM_PDEN_EN8                              (1 << 8)
#define DANUBE_PMU_PM_PDEN_EN7                              (1 << 7)
#define DANUBE_PMU_PM_PDEN_EN5                              (1 << 5)
#define DANUBE_PMU_PM_PDEN_EN4                              (1 << 4)
#define DANUBE_PMU_PM_PDEN_EN3                              (1 << 3)
#define DANUBE_PMU_PM_PDEN_EN2                              (1 << 2)
#define DANUBE_PMU_PM_PDEN_EN0                              (1 << 0)

/***PM Wake-Up from Power Down Register***/
#define DANUBE_PMU_PM_WUP                       ((volatile u32*)(DANUBE_PMU+ 0x0010))
#define DANUBE_PMU_PM_WUP_WUP16                          (1 << 16)
#define DANUBE_PMU_PM_WUP_WUP15                          (1 << 15)
#define DANUBE_PMU_PM_WUP_WUP14                          (1 << 14)
#define DANUBE_PMU_PM_WUP_WUP13                          (1 << 13)
#define DANUBE_PMU_PM_WUP_WUP12                          (1 << 12)
#define DANUBE_PMU_PM_WUP_WUP11                          (1 << 11)
#define DANUBE_PMU_PM_WUP_WUP10                          (1 << 10)
#define DANUBE_PMU_PM_WUP_WUP9                            (1 << 9)
#define DANUBE_PMU_PM_WUP_WUP8                            (1 << 8)
#define DANUBE_PMU_PM_PDEN_EN7                              (1 << 7)
#define DANUBE_PMU_PM_PDEN_EN5                              (1 << 5)
#define DANUBE_PMU_PM_PDEN_EN4                              (1 << 4)
#define DANUBE_PMU_PM_PDEN_EN3                              (1 << 3)
#define DANUBE_PMU_PM_PDEN_EN2                              (1 << 2)
#define DANUBE_PMU_PM_PDEN_EN0                              (1 << 0)

/***PM Wake-Up from Power Down Register***/
#define DANUBE_PMU_PM_WUP                       ((volatile u32*)(DANUBE_PMU+ 0x0010))
#define DANUBE_PMU_PM_WUP_WUP16                          (1 << 16)
#define DANUBE_PMU_PM_WUP_WUP15                          (1 << 15)
#define DANUBE_PMU_PM_WUP_WUP14                          (1 << 14)
#define DANUBE_PMU_PM_WUP_WUP13                          (1 << 13)
#define DANUBE_PMU_PM_WUP_WUP12                          (1 << 12)
#define DANUBE_PMU_PM_WUP_WUP11                          (1 << 11)
#define DANUBE_PMU_PM_WUP_WUP10                          (1 << 10)
#define DANUBE_PMU_PM_WUP_WUP9                            (1 << 9)
#define DANUBE_PMU_PM_WUP_WUP8                            (1 << 8)
#define DANUBE_PMU_PM_WUP_WUP7                            (1 << 7)
#define DANUBE_PMU_PM_WUP_WUP5                            (1 << 5)
#define DANUBE_PMU_PM_WUP_WUP4                            (1 << 4)
#define DANUBE_PMU_PM_WUP_WUP3                            (1 << 3)
#define DANUBE_PMU_PM_WUP_WUP2                            (1 << 2)
#define DANUBE_PMU_PM_WUP_WUP0                            (1 << 0)

/***PM Control Register***/
#define DANUBE_PMU_PM_CR                        ((volatile u32*)(DANUBE_PMU+ 0x0014))
#define DANUBE_PMU_PM_CR_AWEN                            (1 << 31)
#define DANUBE_PMU_PM_CR_SWRST                          (1 << 30)
#define DANUBE_PMU_PM_CR_SWCR                            (1 << 2)
#define DANUBE_PMU_PM_CR_CRD (value)                (((( 1 << 2) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/
#define DANUBE_RCU_BASE_ADDR 		(0xBF203000)

#define DANUBE_RCU_REQ 			(0x0010 + DANUBE_RCU_BASE_ADDR)
#define DANUBE_RCU_RST_REQ                      ((volatile u32*)(DANUBE_RCU_REQ))
#define DANUBE_RCU_STAT		        (0x0014 + DANUBE_RCU_BASE_ADDR)
#define DANUBE_RCU_RST_SR	        ( (volatile u32 *)(DANUBE_RCU_STAT))
#define DANUBE_RCU_PCI_RDY	        ( (volatile u32 *)(DANUBE_RCU_BASE_ADDR+0x28))
#define DANUBE_RCU_MON                  (0x0030 + DANUBE_RCU_BASE_ADDR)


/***********************************************************************/
/*  Module      :  BCU  register address and bits                       */
/***********************************************************************/
#define DANUBE_BCU_BASE_ADDR 	(0xB0100000)
/***BCU Control Register (0010H)***/
#define DANUBE_BCU_CON 		(0x0010 + DANUBE_BCU_BASE_ADDR)
#define DANUBE_BCU_BCU_CON_SPC (value)                (((( 1 << 8) - 1) & (value)) << 24)
#define DANUBE_BCU_BCU_CON_SPE                              (1 << 19)
#define DANUBE_BCU_BCU_CON_PSE                              (1 << 18)
#define DANUBE_BCU_BCU_CON_DBG                              (1 << 16)
#define DANUBE_BCU_BCU_CON_TOUT (value)               (((( 1 << 16) - 1) & (value)) << 0)


/***BCU Error Control Capture Register (0020H)***/
#define DANUBE_BCU_ECON 	(0x0020 + DANUBE_BCU_BASE_ADDR)
#define DANUBE_BCU_BCU_ECON_TAG (value)                (((( 1 << 4) - 1) & (value)) << 24)
#define DANUBE_BCU_BCU_ECON_RDN                              (1 << 23)
#define DANUBE_BCU_BCU_ECON_WRN                              (1 << 22)
#define DANUBE_BCU_BCU_ECON_SVM                              (1 << 21)
#define DANUBE_BCU_BCU_ECON_ACK (value)                (((( 1 << 2) - 1) & (value)) << 19)
#define DANUBE_BCU_BCU_ECON_ABT                              (1 << 18)
#define DANUBE_BCU_BCU_ECON_RDY                              (1 << 17)
#define DANUBE_BCU_BCU_ECON_TOUT                            (1 << 16)
#define DANUBE_BCU_BCU_ECON_ERRCNT (value)             (((( 1 << 16) - 1) & (value)) << 0)
#define DANUBE_BCU_BCU_ECON_OPC (value)                (((( 1 << 4) - 1) & (value)) << 28)

/***BCU Error Address Capture Register (0024 H)***/
#define DANUBE_BCU_EADD 	(0x0024 + DANUBE_BCU_BASE_ADDR)

/***BCU Error Data Capture Register (0028H)***/
#define DANUBE_BCU_EDAT 	(0x0028 + DANUBE_BCU_BASE_ADDR)

#define DANUBE_BCU_IRNEN 	(0x00F4 + DANUBE_BCU_BASE_ADDR)
#define DANUBE_BCU_IRNICR 	(0x00F8 + DANUBE_BCU_BASE_ADDR)
#define DANUBE_BCU_IRNCR 	(0x00FC + DANUBE_BCU_BASE_ADDR)


/***********************************************************************/
/*  Module      :  MBC register address and bits                       */
/***********************************************************************/

#define DANUBE_MBC                          (0xBF103000)
/***********************************************************************/


/***Mailbox CPU Configuration Register***/
#define DANUBE_MBC_MBC_CFG                      ((volatile u32*)(DANUBE_MBC+ 0x0080))
#define DANUBE_MBC_MBC_CFG_SWAP (value)               (((( 1 << 2) - 1) & (value)) << 6)
#define DANUBE_MBC_MBC_CFG_RES                              (1 << 5)
#define DANUBE_MBC_MBC_CFG_FWID (value)               (((( 1 << 4) - 1) & (value)) << 1)
#define DANUBE_MBC_MBC_CFG_SIZE                            (1 << 0)

/***Mailbox CPU Interrupt Status Register***/
#define DANUBE_MBC_MBC_ISR                      ((volatile u32*)(DANUBE_MBC+ 0x0084))
#define DANUBE_MBC_MBC_ISR_B3DA                            (1 << 31)
#define DANUBE_MBC_MBC_ISR_B2DA                            (1 << 30)
#define DANUBE_MBC_MBC_ISR_B1E                              (1 << 29)
#define DANUBE_MBC_MBC_ISR_B0E                              (1 << 28)
#define DANUBE_MBC_MBC_ISR_WDT                              (1 << 27)
#define DANUBE_MBC_MBC_ISR_DS260 (value)             (((( 1 << 27) - 1) & (value)) << 0)

/***Mailbox CPU Mask Register***/
#define DANUBE_MBC_MBC_MSK                      ((volatile u32*)(DANUBE_MBC+ 0x0088))
#define DANUBE_MBC_MBC_MSK_B3DA                            (1 << 31)
#define DANUBE_MBC_MBC_MSK_B2DA                            (1 << 30)
#define DANUBE_MBC_MBC_MSK_B1E                              (1 << 29)
#define DANUBE_MBC_MBC_MSK_B0E                              (1 << 28)
#define DANUBE_MBC_MBC_MSK_WDT                              (1 << 27)
#define DANUBE_MBC_MBC_MSK_DS260 (value)             (((( 1 << 27) - 1) & (value)) << 0)

/***Mailbox CPU Mask 01 Register***/
#define DANUBE_MBC_MBC_MSK01                    ((volatile u32*)(DANUBE_MBC+ 0x008C))
#define DANUBE_MBC_MBC_MSK01_B3DA                            (1 << 31)
#define DANUBE_MBC_MBC_MSK01_B2DA                            (1 << 30)
#define DANUBE_MBC_MBC_MSK01_B1E                              (1 << 29)
#define DANUBE_MBC_MBC_MSK01_B0E                              (1 << 28)
#define DANUBE_MBC_MBC_MSK01_WDT                              (1 << 27)
#define DANUBE_MBC_MBC_MSK01_DS260 (value)             (((( 1 << 27) - 1) & (value)) << 0)

/***Mailbox CPU Mask 10 Register***/
#define DANUBE_MBC_MBC_MSK10                    ((volatile u32*)(DANUBE_MBC+ 0x0090))
#define DANUBE_MBC_MBC_MSK10_B3DA                            (1 << 31)
#define DANUBE_MBC_MBC_MSK10_B2DA                            (1 << 30)
#define DANUBE_MBC_MBC_MSK10_B1E                              (1 << 29)
#define DANUBE_MBC_MBC_MSK10_B0E                              (1 << 28)
#define DANUBE_MBC_MBC_MSK10_WDT                              (1 << 27)
#define DANUBE_MBC_MBC_MSK10_DS260 (value)             (((( 1 << 27) - 1) & (value)) << 0)

/***Mailbox CPU Short Command Register***/
#define DANUBE_MBC_MBC_CMD                      ((volatile u32*)(DANUBE_MBC+ 0x0094))
#define DANUBE_MBC_MBC_CMD_CS270 (value)             (((( 1 << 28) - 1) & (value)) << 0)

/***Mailbox CPU Input Data of Buffer 0***/
#define DANUBE_MBC_MBC_ID0                      ((volatile u32*)(DANUBE_MBC+ 0x0000))
#define DANUBE_MBC_MBC_ID0_INDATA

/***Mailbox CPU Input Data of Buffer 1***/
#define DANUBE_MBC_MBC_ID1                      ((volatile u32*)(DANUBE_MBC+ 0x0020))
#define DANUBE_MBC_MBC_ID1_INDATA

/***Mailbox CPU Output Data of Buffer 2***/
#define DANUBE_MBC_MBC_OD2                      ((volatile u32*)(DANUBE_MBC+ 0x0040))
#define DANUBE_MBC_MBC_OD2_OUTDATA

/***Mailbox CPU Output Data of Buffer 3***/
#define DANUBE_MBC_MBC_OD3                      ((volatile u32*)(DANUBE_MBC+ 0x0060))
#define DANUBE_MBC_MBC_OD3_OUTDATA

/***Mailbox CPU Control Register of Buffer 0***/
#define DANUBE_MBC_MBC_CR0                      ((volatile u32*)(DANUBE_MBC+ 0x0004))
#define DANUBE_MBC_MBC_CR0_RDYABTFLS (value)          (((( 1 << 3) - 1) & (value)) << 0)

/***Mailbox CPU Control Register of Buffer 1***/
#define DANUBE_MBC_MBC_CR1                      ((volatile u32*)(DANUBE_MBC+ 0x0024))
#define DANUBE_MBC_MBC_CR1_RDYABTFLS (value)          (((( 1 << 3) - 1) & (value)) << 0)

/***Mailbox CPU Control Register of Buffer 2***/
#define DANUBE_MBC_MBC_CR2                      ((volatile u32*)(DANUBE_MBC+ 0x0044))
#define DANUBE_MBC_MBC_CR2_RDYABTFLS (value)          (((( 1 << 3) - 1) & (value)) << 0)

/***Mailbox CPU Control Register of Buffer 3***/
#define DANUBE_MBC_MBC_CR3                      ((volatile u32*)(DANUBE_MBC+ 0x0064))
#define DANUBE_MBC_MBC_CR3_RDYABTFLS (value)          (((( 1 << 3) - 1) & (value)) << 0)

/***Mailbox CPU Free Space of Buffer 0***/
#define DANUBE_MBC_MBC_FS0                      ((volatile u32*)(DANUBE_MBC+ 0x0008))
#define DANUBE_MBC_MBC_FS0_FS

/***Mailbox CPU Free Space of Buffer 1***/
#define DANUBE_MBC_MBC_FS1                      ((volatile u32*)(DANUBE_MBC+ 0x0028))
#define DANUBE_MBC_MBC_FS1_FS

/***Mailbox CPU Free Space of Buffer 2***/
#define DANUBE_MBC_MBC_FS2                      ((volatile u32*)(DANUBE_MBC+ 0x0048))
#define DANUBE_MBC_MBC_FS2_FS

/***Mailbox CPU Free Space of Buffer 3***/
#define DANUBE_MBC_MBC_FS3                      ((volatile u32*)(DANUBE_MBC+ 0x0068))
#define DANUBE_MBC_MBC_FS3_FS

/***Mailbox CPU Data Available in Buffer 0***/
#define DANUBE_MBC_MBC_DA0                      ((volatile u32*)(DANUBE_MBC+ 0x000C))
#define DANUBE_MBC_MBC_DA0_DA

/***Mailbox CPU Data Available in Buffer 1***/
#define DANUBE_MBC_MBC_DA1                      ((volatile u32*)(DANUBE_MBC+ 0x002C))
#define DANUBE_MBC_MBC_DA1_DA

/***Mailbox CPU Data Available in Buffer 2***/
#define DANUBE_MBC_MBC_DA2                      ((volatile u32*)(DANUBE_MBC+ 0x004C))
#define DANUBE_MBC_MBC_DA2_DA

/***Mailbox CPU Data Available in Buffer 3***/
#define DANUBE_MBC_MBC_DA3                      ((volatile u32*)(DANUBE_MBC+ 0x006C))
#define DANUBE_MBC_MBC_DA3_DA

/***Mailbox CPU Input Absolute Pointer of Buffer 0***/
#define DANUBE_MBC_MBC_IABS0                    ((volatile u32*)(DANUBE_MBC+ 0x0010))
#define DANUBE_MBC_MBC_IABS0_IABS

/***Mailbox CPU Input Absolute Pointer of Buffer 1***/
#define DANUBE_MBC_MBC_IABS1                    ((volatile u32*)(DANUBE_MBC+ 0x0030))
#define DANUBE_MBC_MBC_IABS1_IABS

/***Mailbox CPU Input Absolute Pointer of Buffer 2***/
#define DANUBE_MBC_MBC_IABS2                    ((volatile u32*)(DANUBE_MBC+ 0x0050))
#define DANUBE_MBC_MBC_IABS2_IABS

/***Mailbox CPU Input Absolute Pointer of Buffer 3***/
#define DANUBE_MBC_MBC_IABS3                    ((volatile u32*)(DANUBE_MBC+ 0x0070))
#define DANUBE_MBC_MBC_IABS3_IABS

/***Mailbox CPU Input Temporary Pointer of Buffer 0***/
#define DANUBE_MBC_MBC_ITMP0                    ((volatile u32*)(DANUBE_MBC+ 0x0014))
#define DANUBE_MBC_MBC_ITMP0_ITMP

/***Mailbox CPU Input Temporary Pointer of Buffer 1***/
#define DANUBE_MBC_MBC_ITMP1                    ((volatile u32*)(DANUBE_MBC+ 0x0034))
#define DANUBE_MBC_MBC_ITMP1_ITMP

/***Mailbox CPU Input Temporary Pointer of Buffer 2***/
#define DANUBE_MBC_MBC_ITMP2                    ((volatile u32*)(DANUBE_MBC+ 0x0054))
#define DANUBE_MBC_MBC_ITMP2_ITMP

/***Mailbox CPU Input Temporary Pointer of Buffer 3***/
#define DANUBE_MBC_MBC_ITMP3                    ((volatile u32*)(DANUBE_MBC+ 0x0074))
#define DANUBE_MBC_MBC_ITMP3_ITMP

/***Mailbox CPU Output Absolute Pointer of Buffer 0***/
#define DANUBE_MBC_MBC_OABS0                    ((volatile u32*)(DANUBE_MBC+ 0x0018))
#define DANUBE_MBC_MBC_OABS0_OABS

/***Mailbox CPU Output Absolute Pointer of Buffer 1***/
#define DANUBE_MBC_MBC_OABS1                    ((volatile u32*)(DANUBE_MBC+ 0x0038))
#define DANUBE_MBC_MBC_OABS1_OABS

/***Mailbox CPU Output Absolute Pointer of Buffer 2***/
#define DANUBE_MBC_MBC_OABS2                    ((volatile u32*)(DANUBE_MBC+ 0x0058))
#define DANUBE_MBC_MBC_OABS2_OABS

/***Mailbox CPU Output Absolute Pointer of Buffer 3***/
#define DANUBE_MBC_MBC_OABS3                    ((volatile u32*)(DANUBE_MBC+ 0x0078))
#define DANUBE_MBC_MBC_OABS3_OABS

/***Mailbox CPU Output Temporary Pointer of Buffer 0***/
#define DANUBE_MBC_MBC_OTMP0                    ((volatile u32*)(DANUBE_MBC+ 0x001C))
#define DANUBE_MBC_MBC_OTMP0_OTMP

/***Mailbox CPU Output Temporary Pointer of Buffer 1***/
#define DANUBE_MBC_MBC_OTMP1                    ((volatile u32*)(DANUBE_MBC+ 0x003C))
#define DANUBE_MBC_MBC_OTMP1_OTMP

/***Mailbox CPU Output Temporary Pointer of Buffer 2***/
#define DANUBE_MBC_MBC_OTMP2                    ((volatile u32*)(DANUBE_MBC+ 0x005C))
#define DANUBE_MBC_MBC_OTMP2_OTMP

/***Mailbox CPU Output Temporary Pointer of Buffer 3***/
#define DANUBE_MBC_MBC_OTMP3                    ((volatile u32*)(DANUBE_MBC+ 0x007C))
#define DANUBE_MBC_MBC_OTMP3_OTMP

/***DSP Control Register***/
#define DANUBE_MBC_DCTRL                        ((volatile u32*)(DANUBE_MBC+ 0x00A0))
#define DANUBE_MBC_DCTRL_BA                              (1 << 0)
#define DANUBE_MBC_DCTRL_BMOD (value)               (((( 1 << 3) - 1) & (value)) << 1)
#define DANUBE_MBC_DCTRL_IDL                              (1 << 4)
#define DANUBE_MBC_DCTRL_RES                              (1 << 15)

/***DSP Status Register***/
#define DANUBE_MBC_DSTA                         ((volatile u32*)(DANUBE_MBC+ 0x00A4))
#define DANUBE_MBC_DSTA_IDLE                            (1 << 0)
#define DANUBE_MBC_DSTA_PD                              (1 << 1)

/***DSP Test 1 Register***/
#define DANUBE_MBC_DTST1                        ((volatile u32*)(DANUBE_MBC+ 0x00A8))
#define DANUBE_MBC_DTST1_ABORT                          (1 << 0)
#define DANUBE_MBC_DTST1_HWF32                          (1 << 1)
#define DANUBE_MBC_DTST1_HWF4M                          (1 << 2)
#define DANUBE_MBC_DTST1_HWFOP                          (1 << 3)


/***********************************************************************/
/*  Module      :  SSC1 register address and bits                      */
/***********************************************************************/
#define DANUBE_SSC1                       	(KSEG1+0x1e100800)
/***********************************************************************/
/***SSC Clock Control Register***/
#define DANUBE_SSC_CLC                      	(0x0000)
#define DANUBE_SSC_CLC_RMC(value)               (((( 1 << 8) - 1) & (value)) << 8)
#define DANUBE_SSC_CLC_DISS                     (1 << 1)
#define DANUBE_SSC_CLC_DISR                     (1 << 0)
/***SSC Port Input Selection Register***/
#define DANUBE_SSC_PISEL                        (0x0004)
/***SSC Identification Register***/
#define DANUBE_SSC_ID                           (0x0008)
/***Control Register (Programming Mode)***/
#define DANUBE_SSC_CON                  		(0x0010)
#define DANUBE_SSC_CON_RUEN                            (1 << 12)
#define DANUBE_SSC_CON_TUEN                              (1 << 11)
#define DANUBE_SSC_CON_AEN                              (1 << 10)
#define DANUBE_SSC_CON_REN                              (1 << 9)
#define DANUBE_SSC_CON_TEN                              (1 << 8)
#define DANUBE_SSC_CON_LB                              (1 << 7)
#define DANUBE_SSC_CON_PO                              (1 << 6)
#define DANUBE_SSC_CON_PH                              (1 << 5)
#define DANUBE_SSC_CON_HB                              (1 << 4)
#define DANUBE_SSC_CON_BM(value)                	(((( 1 << 5) - 1) & (value)) << 16)
#define DANUBE_SSC_CON_RX_OFF                          (1 << 1)
#define DANUBE_SSC_CON_TX_OFF                          (1 << 0)
/***SCC Status Register***/
#define DANUBE_SSC_STATE                  (0x0014)
#define DANUBE_SSC_STATE_EN                              (1 << 0)
#define DANUBE_SSC_STATE_MS                              (1 << 1)
#define DANUBE_SSC_STATE_BSY                              (1 << 13)
#define DANUBE_SSC_STATE_RUE                              (1 << 12)
#define DANUBE_SSC_STATE_TUE                              (1 << 11)
#define DANUBE_SSC_STATE_AE                              (1 << 10)
#define DANUBE_SSC_STATE_RE                              (1 << 9)
#define DANUBE_SSC_STATE_TE                              (1 << 8)
#define DANUBE_SSC_STATE_BC(value)                (((( 1 << 5) - 1) & (value)) << 16)
/***SSC Write Hardware Modified Control Register***/
#define DANUBE_SSC_WHBSTATE                   ( 0x0018)
#define DANUBE_SSC_WHBSTATE_SETBE                          (1 << 15)
#define DANUBE_SSC_WHBSTATE_SETPE                          (1 << 14)
#define DANUBE_SSC_WHBSTATE_SETRE                          (1 << 13)
#define DANUBE_SSC_WHBSTATE_SETTE                          (1 << 12)
#define DANUBE_SSC_WHBSTATE_CLRBE                          (1 << 11)
#define DANUBE_SSC_WHBSTATE_CLRPE                          (1 << 10)
#define DANUBE_SSC_WHBSTATE_CLRRE                          (1 << 9)
#define DANUBE_SSC_WHBSTATE_CLRTE                          (1 << 8)
/***SSC Transmitter Buffer Register***/
#define DANUBE_SSC_TB                       (0x0020)
#define DANUBE_SSC_TB_TB_VALUE(value)          (((( 1 << 16) - 1) & (value)) << 0)
/***SSC Receiver Buffer Register***/
#define DANUBE_SSC_RB                       (0x0024)
#define DANUBE_SSC_RB_RB_VALUE(value)          (((( 1 << 16) - 1) & (value)) << 0)
/***SSC Receive FIFO Control Register***/
#define DANUBE_SSC_RXFCON                   (0x0030)
#define DANUBE_SSC_RXFCON_RXFITL(value)             (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_SSC_RXFCON_RXTMEN                        (1 << 2)
#define DANUBE_SSC_RXFCON_RXFLU                          (1 << 1)
#define DANUBE_SSC_RXFCON_RXFEN                          (1 << 0)
/***SSC Transmit FIFO Control Register***/
#define DANUBE_SSC_TXFCON                   ( 0x0034)
#define DANUBE_SSC_TXFCON_RXFITL(value)             (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_SSC_TXFCON_TXTMEN                        (1 << 2)
#define DANUBE_SSC_TXFCON_TXFLU                          (1 << 1)
#define DANUBE_SSC_TXFCON_TXFEN                          (1 << 0)
/***SSC FIFO Status Register***/
#define DANUBE_SSC_FSTAT                    (0x0038)
#define DANUBE_SSC_FSTAT_TXFFL(value)              (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_SSC_FSTAT_RXFFL(value)              (((( 1 << 6) - 1) & (value)) << 0)
/***SSC Baudrate Timer Reload Register***/
#define DANUBE_SSC_BR                       (0x0040)
#define DANUBE_SSC_BR_BR_VALUE(value)          (((( 1 << 16) - 1) & (value)) << 0)
#define DANUBE_SSC_BRSTAT                       (0x0044)
#define DANUBE_SSC_SFCON                        (0x0060)
#define DANUBE_SSC_SFSTAT                       (0x0064)
#define DANUBE_SSC_GPOCON                       (0x0070)
#define DANUBE_SSC_GPOSTAT                      (0x0074)
#define DANUBE_SSC_WHBGPOSTAT                   (0x0078)
#define DANUBE_SSC_RXREQ                        (0x0080)
#define DANUBE_SSC_RXCNT                        (0x0084)
/*DMA Registers in Bus Clock Domain*/
#define DANUBE_SSC_DMA_CON                      (0x00EC)
/*interrupt Node Registers in Bus Clock Domain*/
#define DANUBE_SSC_IRNEN                        (0x00F4)
#define DANUBE_SSC_IRNCR                        (0x00F8)
#define DANUBE_SSC_IRNICR                       (0x00FC)
#define DANUBE_SSC_IRN_FIR			0x8
#define DANUBE_SSC_IRN_EIR			0x4
#define DANUBE_SSC_IRN_RIR			0x2
#define DANUBE_SSC_IRN_TIR			0x1


#define	DANUBE_SSC1_CLC			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_CLC))
#define	DANUBE_SSC1_ID			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_ID))
#define	DANUBE_SSC1_CON			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_CON))
#define	DANUBE_SSC1_STATE			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_STATE))
#define	DANUBE_SSC1_WHBSTATE			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_WHBSTATE))
#define	DANUBE_SSC1_TB			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_TB))
#define	DANUBE_SSC1_RB			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_RB))
#define	DANUBE_SSC1_FSTAT			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_FSTAT))
#define	DANUBE_SSC1_PISEL			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_PISEL))
#define	DANUBE_SSC1_RXFCON			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_RXFCON))
#define	DANUBE_SSC1_TXFCON			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_TXFCON))
#define	DANUBE_SSC1_BR			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_BR))
#define	DANUBE_SSC1_BRSTAT			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_BRSTAT))
#define	DANUBE_SSC1_SFCON			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_SFCON))
#define	DANUBE_SSC1_SFSTAT			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_SFSTAT))
#define	DANUBE_SSC1_GPOCON			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_GPOCON))
#define	DANUBE_SSC1_GPOSTAT			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_GPOSTAT))
#define	DANUBE_SSC1_WHBGPOSTAT			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_WHBGPOSTAT))
#define	DANUBE_SSC1_RXREQ			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_RXREQ))
#define	DANUBE_SSC1_RXCNT			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_RXCNT))
#define	DANUBE_SSC1_DMA_CON			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_DMA_CON))
#define	DANUBE_SSC1_IRNEN			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_IRNEN))
#define	DANUBE_SSC1_IRNICR			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_IRNICR))
#define	DANUBE_SSC1_IRNCR			((volatile u32*)(DANUBE_SSC1+DANUBE_SSC_IRNCR))

/***********************************************************************/
/*  Module      :  GPIO register address and bits                       */
/***********************************************************************/
#define DANUBE_GPIO                     (0xBE100B00)
/***Port 0 Data Output Register (0010H)***/
#define DANUBE_GPIO_P0_OUT              ((volatile u32 *)(DANUBE_GPIO+ 0x0010))
/***Port 1 Data Output Register (0040H)***/
#define DANUBE_GPIO_P1_OUT              ((volatile u32 *)(DANUBE_GPIO+ 0x0040))
/***Port 0 Data Input Register (0014H)***/
#define DANUBE_GPIO_P0_IN               ((volatile u32 *)(DANUBE_GPIO+ 0x0014))
/***Port 1 Data Input Register (0044H)***/
#define DANUBE_GPIO_P1_IN               ((volatile u32 *)(DANUBE_GPIO+ 0x0044))
/***Port 0 Direction Register (0018H)***/
#define DANUBE_GPIO_P0_DIR              ((volatile u32 *)(DANUBE_GPIO+ 0x0018))
/***Port 1 Direction Register (0048H)***/
#define DANUBE_GPIO_P1_DIR              ((volatile u32 *)(DANUBE_GPIO+ 0x0048))
/***Port 0 Alternate Function Select Register 0 (001C H) ***/
#define DANUBE_GPIO_P0_ALTSEL0          ((volatile u32 *)(DANUBE_GPIO+ 0x001C))
/***Port 1 Alternate Function Select Register 0 (004C H) ***/
#define DANUBE_GPIO_P1_ALTSEL0          ((volatile u32 *)(DANUBE_GPIO+ 0x004C))
/***Port 0 Alternate Function Select Register 1 (0020 H) ***/
#define DANUBE_GPIO_P0_ALTSEL1          ((volatile u32 *)(DANUBE_GPIO+ 0x0020))
/***Port 1 Alternate Function Select Register 0 (0050 H) ***/
#define DANUBE_GPIO_P1_ALTSEL1          ((volatile u32 *)(DANUBE_GPIO+ 0x0050))
/***Port 0 Open Drain Control Register (0024H)***/
#define DANUBE_GPIO_P0_OD               ((volatile u32 *)(DANUBE_GPIO+ 0x0024))
/***Port 1 Open Drain Control Register (0054H)***/
#define DANUBE_GPIO_P1_OD               ((volatile u32 *)(DANUBE_GPIO+ 0x0054))
/***Port 0 Input Schmitt-Trigger Off Register (0028 H) ***/
#define DANUBE_GPIO_P0_STOFF            ((volatile u32 *)(DANUBE_GPIO+ 0x0028))
/***Port 1 Input Schmitt-Trigger Off Register (0058 H) ***/
#define DANUBE_GPIO_P1_STOFF            ((volatile u32 *)(DANUBE_GPIO+ 0x0058))
/***Port 0 Pull Up/Pull Down Select Register (002C H)***/
#define DANUBE_GPIO_P0_PUDSEL           ((volatile u32 *)(DANUBE_GPIO+ 0x002C))
/***Port 1 Pull Up/Pull Down Select Register (005C H)***/
#define DANUBE_GPIO_P1_PUDSEL           ((volatile u32 *)(DANUBE_GPIO+ 0x005C))
/***Port 0 Pull Up Device Enable Register (0030 H)***/
#define DANUBE_GPIO_P0_PUDEN            ((volatile u32 *)(DANUBE_GPIO+ 0x0030))
/***Port 1 Pull Up Device Enable Register (0060 H)***/
#define DANUBE_GPIO_P1_PUDEN            ((volatile u32 *)(DANUBE_GPIO+ 0x0060))
/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/

#define DANUBE_CGU                          (0xBF103000)
/***********************************************************************/

/***CGU Clock PLL0 ***/
#define DANUBE_CGU_PLL0_CFG                	((volatile u32*)(DANUBE_CGU+ 0x0004))
/***CGU Clock PLL1 ***/
#define DANUBE_CGU_PLL1_CFG                	((volatile u32*)(DANUBE_CGU+ 0x0008))
/***CGU Clock SYS Mux Register***/
#define DANUBE_CGU_SYS                   	((volatile u32*)(DANUBE_CGU+ 0x0010))
/***CGU Interface Clock Control Register***/
#define DANUBE_CGU_IFCCR                        ((volatile u32*)(DANUBE_CGU+ 0x0018))
/***CGU PCI Clock Control Register**/
#define DANUBE_CGU_PCICR                          ((volatile u32*)(DANUBE_CGU+ 0x0034))


/***********************************************************************/
/*  Module      :  PCI register address and bits                       */
/***********************************************************************/
#define PCI_CR_PR_OFFSET  0xBE105400
#define PCI_CR_CLK_CTRL_REG         (PCI_CR_PR_OFFSET + 0x0000)

#define PCI_CR_PCI_ID_REG           (PCI_CR_PR_OFFSET + 0x0004)
#define PCI_CR_SFT_RST_REG          (PCI_CR_PR_OFFSET + 0x0010)
#define PCI_CR_PCI_FPI_ERR_ADDR_REG (PCI_CR_PR_OFFSET + 0x0014)
#define PCI_CR_FCI_PCI_ERR_ADDR_REG (PCI_CR_PR_OFFSET + 0x0018)
#define PCI_CR_FPI_ERR_TAG_REG      (PCI_CR_PR_OFFSET + 0x001C)
#define PCI_CR_PCI_IRR_REG          (PCI_CR_PR_OFFSET + 0x0020)
#define PCI_CR_PCI_IRA_REG          (PCI_CR_PR_OFFSET + 0x0024)
#define PCI_CR_PCI_IRM_REG          (PCI_CR_PR_OFFSET + 0x0028)
#define PCI_CR_PCI_EOI_REG          (PCI_CR_PR_OFFSET + 0x002C)
#define PCI_CR_PCI_MOD_REG          (PCI_CR_PR_OFFSET + 0x0030)
#define PCI_CR_DV_ID_REG            (PCI_CR_PR_OFFSET + 0x0034)
#define PCI_CR_SUBSYS_ID_REG        (PCI_CR_PR_OFFSET + 0x0038)
#define PCI_CR_PCI_PM_REG           (PCI_CR_PR_OFFSET + 0x003C)
#define PCI_CR_CLASS_CODE1_REG      (PCI_CR_PR_OFFSET + 0x0040)
#define PCI_CR_BAR11MASK_REG        (PCI_CR_PR_OFFSET + 0x0044)
#define PCI_CR_BAR12MASK_REG        (PCI_CR_PR_OFFSET + 0x0048)
#define PCI_CR_BAR13MASK_REG        (PCI_CR_PR_OFFSET + 0x004C)
#define PCI_CR_BAR14MASK_REG        (PCI_CR_PR_OFFSET + 0x0050)
#define PCI_CR_BAR15MASK_REG        (PCI_CR_PR_OFFSET + 0x0054)
#define PCI_CR_BAR16MASK_REG        (PCI_CR_PR_OFFSET + 0x0058)
#define PCI_CR_CIS_PT1_REG          (PCI_CR_PR_OFFSET + 0x005C)
#define PCI_CR_SUBSYS_ID1_REG       (PCI_CR_PR_OFFSET + 0x0060)
#define PCI_CR_PCI_ADDR_MAP11_REG   (PCI_CR_PR_OFFSET + 0x0064)
#define PCI_CR_PCI_ADDR_MAP12_REG   (PCI_CR_PR_OFFSET + 0x0068)
#define PCI_CR_PCI_ADDR_MAP13_REG   (PCI_CR_PR_OFFSET + 0x006C)
#define PCI_CR_PCI_ADDR_MAP14_REG   (PCI_CR_PR_OFFSET + 0x0070)
#define PCI_CR_PCI_ADDR_MAP15_REG   (PCI_CR_PR_OFFSET + 0x0074)
#define PCI_CR_PCI_ADDR_MAP16_REG   (PCI_CR_PR_OFFSET + 0x0078)
#define PCI_CR_FPI_SEG_EN_REG       (PCI_CR_PR_OFFSET + 0x007C)
#define PCI_CR_PC_ARB_REG           (PCI_CR_PR_OFFSET + 0x0080)
#define PCI_CR_BAR21MASK_REG        (PCI_CR_PR_OFFSET + 0x0084)
#define PCI_CR_BAR22MASK_REG        (PCI_CR_PR_OFFSET + 0x0088)
#define PCI_CR_BAR23MASK_REG        (PCI_CR_PR_OFFSET + 0x008C)
#define PCI_CR_BAR24MASK_REG        (PCI_CR_PR_OFFSET + 0x0090)
#define PCI_CR_BAR25MASK_REG        (PCI_CR_PR_OFFSET + 0x0094)
#define PCI_CR_BAR26MASK_REG        (PCI_CR_PR_OFFSET + 0x0098)
#define PCI_CR_CIS_PT2_REG          (PCI_CR_PR_OFFSET + 0x009C)
#define PCI_CR_SUBSYS_ID2_REG       (PCI_CR_PR_OFFSET + 0x00A0)
#define PCI_CR_PCI_ADDR_MAP21_REG   (PCI_CR_PR_OFFSET + 0x00A4)
#define PCI_CR_PCI_ADDR_MAP22_REG   (PCI_CR_PR_OFFSET + 0x00A8)
#define PCI_CR_PCI_ADDR_MAP23_REG   (PCI_CR_PR_OFFSET + 0x00AC)


/***********************************************************************/
/*  Module      :  MCD register address and bits                       */
/***********************************************************************/
#define DANUBE_MCD                          		(KSEG1+0x1F106000)

/***Manufacturer Identification Register***/
#define DANUBE_MCD_MANID                        	((volatile u32*)(DANUBE_MCD+ 0x0024))
#define DANUBE_MCD_MANID_MANUF(value)              	(((( 1 << 11) - 1) & (value)) << 5)

/***Chip Identification Register***/
#define DANUBE_MCD_CHIPID                       	((volatile u32*)(DANUBE_MCD+ 0x0028))
#define DANUBE_MCD_CHIPID_VERSION_GET(value)             (((value) >> 28) & ((1 << 4) - 1))
#define DANUBE_MCD_CHIPID_VERSION_SET(value)             (((( 1 << 4) - 1) & (value)) << 28)
#define DANUBE_MCD_CHIPID_PART_NUMBER_GET(value)         (((value) >> 12) & ((1 << 16) - 1))
#define DANUBE_MCD_CHIPID_PART_NUMBER_SET(value)         (((( 1 << 16) - 1) & (value)) << 12)
#define DANUBE_MCD_CHIPID_MANID_GET(value)               (((value) >> 1) & ((1 << 11) - 1))
#define DANUBE_MCD_CHIPID_MANID_SET(value)               (((( 1 << 11) - 1) & (value)) << 1)

#define DANUBE_CHIPID_STANDARD				0x00EB
#define DANUBE_CHIPID_YANGTSE				0x00ED

/***Redesign Tracing Identification Register***/
#define DANUBE_MCD_RTID                         	((volatile u32*)(DANUBE_MCD+ 0x002C))
#define DANUBE_MCD_RTID_LC                              (1 << 15)
#define DANUBE_MCD_RTID_RIX(value)                	(((( 1 << 3) - 1) & (value)) << 0)


/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/

#define DANUBE_EBU                          (0xBE105300)
#define EBU_NAND_CON       (volatile u32*)(DANUBE_EBU + 0xB0)
#define EBU_NAND_WAIT      (volatile u32*)(DANUBE_EBU + 0xB4)
#define EBU_NAND_ECC0      (volatile u32*)(DANUBE_EBU + 0xB8)
#define EBU_NAND_ECC_AC    (volatile u32*)(DANUBE_EBU + 0xBC)

/***********************************************************************/


/***EBU Clock Control Register***/
#define DANUBE_EBU_CLC                      ((volatile u32*)(DANUBE_EBU+ 0x0000))
#define DANUBE_EBU_CLC_DISS                            (1 << 1)
#define DANUBE_EBU_CLC_DISR                            (1 << 0)

/***EBU Global Control Register***/
#define DANUBE_EBU_CON                      ((volatile u32*)(DANUBE_EBU+ 0x0010))
#define DANUBE_EBU_CON_DTACS (value)              (((( 1 << 3) - 1) & (value)) << 20)
#define DANUBE_EBU_CON_DTARW (value)              (((( 1 << 3) - 1) & (value)) << 16)
#define DANUBE_EBU_CON_TOUTC (value)              (((( 1 << 8) - 1) & (value)) << 8)
#define DANUBE_EBU_CON_ARBMODE (value)            (((( 1 << 2) - 1) & (value)) << 6)
#define DANUBE_EBU_CON_ARBSYNC                      (1 << 5)
#define DANUBE_EBU_CON_1                              (1 << 3)

/***EBU Address Select Register 0***/
#define DANUBE_EBU_ADDSEL0                  ((volatile u32*)(DANUBE_EBU+ 0x0020))
#define DANUBE_EBU_ADDSEL0_BASE (value)               (((( 1 << 20) - 1) & (value)) << 12)
#define DANUBE_EBU_ADDSEL0_MASK (value)               (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_EBU_ADDSEL0_MIRRORE                      (1 << 1)
#define DANUBE_EBU_ADDSEL0_REGEN                          (1 << 0)

/***EBU Address Select Register 1***/
#define DANUBE_EBU_ADDSEL1                  ((volatile u32*)(DANUBE_EBU+ 0x0024))
#define DANUBE_EBU_ADDSEL1_BASE (value)               (((( 1 << 20) - 1) & (value)) << 12)
#define DANUBE_EBU_ADDSEL1_MASK (value)               (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_EBU_ADDSEL1_MIRRORE                      (1 << 1)
#define DANUBE_EBU_ADDSEL1_REGEN                          (1 << 0)

/***EBU Address Select Register 2***/
#define DANUBE_EBU_ADDSEL2                  ((volatile u32*)(DANUBE_EBU+ 0x0028))
#define DANUBE_EBU_ADDSEL2_BASE (value)               (((( 1 << 20) - 1) & (value)) << 12)
#define DANUBE_EBU_ADDSEL2_MASK (value)               (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_EBU_ADDSEL2_MIRRORE                      (1 << 1)
#define DANUBE_EBU_ADDSEL2_REGEN                          (1 << 0)

/***EBU Address Select Register 3***/
#define DANUBE_EBU_ADDSEL3                  ((volatile u32*)(DANUBE_EBU+ 0x002C))
#define DANUBE_EBU_ADDSEL3_BASE (value)               (((( 1 << 20) - 1) & (value)) << 12)
#define DANUBE_EBU_ADDSEL3_MASK (value)               (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_EBU_ADDSEL3_MIRRORE                      (1 << 1)
#define DANUBE_EBU_ADDSEL3_REGEN                          (1 << 0)

/***EBU Bus Configuration Register 0***/
#define DANUBE_EBU_BUSCON0                  ((volatile u32*)(DANUBE_EBU+ 0x0060))
#define DANUBE_EBU_BUSCON0_WRDIS                          (1 << 31)
#define DANUBE_EBU_BUSCON0_ALEC (value)               (((( 1 << 2) - 1) & (value)) << 29)
#define DANUBE_EBU_BUSCON0_BCGEN (value)              (((( 1 << 2) - 1) & (value)) << 27)
#define DANUBE_EBU_BUSCON0_AGEN (value)               (((( 1 << 2) - 1) & (value)) << 24)
#define DANUBE_EBU_BUSCON0_CMULTR (value)             (((( 1 << 2) - 1) & (value)) << 22)
#define DANUBE_EBU_BUSCON0_WAIT (value)               (((( 1 << 2) - 1) & (value)) << 20)
#define DANUBE_EBU_BUSCON0_WAITINV                      (1 << 19)
#define DANUBE_EBU_BUSCON0_SETUP                          (1 << 18)
#define DANUBE_EBU_BUSCON0_PORTW (value)              (((( 1 << 2) - 1) & (value)) << 16)
#define DANUBE_EBU_BUSCON0_WAITRDC (value)            (((( 1 << 7) - 1) & (value)) << 9)
#define DANUBE_EBU_BUSCON0_WAITWRC (value)            (((( 1 << 3) - 1) & (value)) << 6)
#define DANUBE_EBU_BUSCON0_HOLDC (value)              (((( 1 << 2) - 1) & (value)) << 4)
#define DANUBE_EBU_BUSCON0_RECOVC (value)             (((( 1 << 2) - 1) & (value)) << 2)
#define DANUBE_EBU_BUSCON0_CMULT (value)              (((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 1***/
#define DANUBE_EBU_BUSCON1                  ((volatile u32*)(DANUBE_EBU+ 0x0064))
#define DANUBE_EBU_BUSCON1_WRDIS                          (1 << 31)
#define DANUBE_EBU_BUSCON1_ALEC (value)               (((( 1 << 2) - 1) & (value)) << 29)
#define DANUBE_EBU_BUSCON1_BCGEN (value)              (((( 1 << 2) - 1) & (value)) << 27)
#define DANUBE_EBU_BUSCON1_AGEN (value)               (((( 1 << 2) - 1) & (value)) << 24)
#define DANUBE_EBU_BUSCON1_CMULTR (value)             (((( 1 << 2) - 1) & (value)) << 22)
#define DANUBE_EBU_BUSCON1_WAIT (value)               (((( 1 << 2) - 1) & (value)) << 20)
#define DANUBE_EBU_BUSCON1_WAITINV                      (1 << 19)
#define DANUBE_EBU_BUSCON1_SETUP                          (1 << 18)
#define DANUBE_EBU_BUSCON1_PORTW (value)              (((( 1 << 2) - 1) & (value)) << 16)
#define DANUBE_EBU_BUSCON1_WAITRDC (value)            (((( 1 << 7) - 1) & (value)) << 9)
#define DANUBE_EBU_BUSCON1_WAITWRC (value)            (((( 1 << 3) - 1) & (value)) << 6)
#define DANUBE_EBU_BUSCON1_HOLDC (value)              (((( 1 << 2) - 1) & (value)) << 4)
#define DANUBE_EBU_BUSCON1_RECOVC (value)             (((( 1 << 2) - 1) & (value)) << 2)
#define DANUBE_EBU_BUSCON1_CMULT (value)              (((( 1 << 2) - 1) & (value)) << 0)

/***EBU Bus Configuration Register 2***/
#define DANUBE_EBU_BUSCON2                  ((volatile u32*)(DANUBE_EBU+ 0x0068))
#define DANUBE_EBU_BUSCON2_WRDIS                          (1 << 31)
#define DANUBE_EBU_BUSCON2_ALEC (value)               (((( 1 << 2) - 1) & (value)) << 29)
#define DANUBE_EBU_BUSCON2_BCGEN (value)              (((( 1 << 2) - 1) & (value)) << 27)
#define DANUBE_EBU_BUSCON2_AGEN (value)               (((( 1 << 2) - 1) & (value)) << 24)
#define DANUBE_EBU_BUSCON2_CMULTR (value)             (((( 1 << 2) - 1) & (value)) << 22)
#define DANUBE_EBU_BUSCON2_WAIT (value)               (((( 1 << 2) - 1) & (value)) << 20)
#define DANUBE_EBU_BUSCON2_WAITINV                      (1 << 19)
#define DANUBE_EBU_BUSCON2_SETUP                          (1 << 18)
#define DANUBE_EBU_BUSCON2_PORTW (value)              (((( 1 << 2) - 1) & (value)) << 16)
#define DANUBE_EBU_BUSCON2_WAITRDC (value)            (((( 1 << 7) - 1) & (value)) << 9)
#define DANUBE_EBU_BUSCON2_WAITWRC (value)            (((( 1 << 3) - 1) & (value)) << 6)
#define DANUBE_EBU_BUSCON2_HOLDC (value)              (((( 1 << 2) - 1) & (value)) << 4)
#define DANUBE_EBU_BUSCON2_RECOVC (value)             (((( 1 << 2) - 1) & (value)) << 2)
#define DANUBE_EBU_BUSCON2_CMULT (value)              (((( 1 << 2) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  SDRAM register address and bits                     */
/***********************************************************************/

#define DANUBE_SDRAM                        (0xBF800000)
/***********************************************************************/


/***MC Access Error Cause Register***/
#define DANUBE_SDRAM_MC_ERRCAUSE                  ((volatile u32*)(DANUBE_SDRAM+ 0x0100))
#define DANUBE_SDRAM_MC_ERRCAUSE_ERR                              (1 << 31)
#define DANUBE_SDRAM_MC_ERRCAUSE_PORT (value)               (((( 1 << 4) - 1) & (value)) << 16)
#define DANUBE_SDRAM_MC_ERRCAUSE_CAUSE (value)              (((( 1 << 2) - 1) & (value)) << 0)
#define DANUBE_SDRAM_MC_ERRCAUSE_Res (value)                (((( 1 << NaN) - 1) & (value)) << NaN)

/***MC Access Error Address Register***/
#define DANUBE_SDRAM_MC_ERRADDR                   ((volatile u32*)(DANUBE_SDRAM+ 0x0108))
#define DANUBE_SDRAM_MC_ERRADDR_ADDR

/***MC I/O General Purpose Register***/
#define DANUBE_SDRAM_MC_IOGP                      ((volatile u32*)(DANUBE_SDRAM+ 0x0800))
#define DANUBE_SDRAM_MC_IOGP_GPR6 (value)               (((( 1 << 4) - 1) & (value)) << 28)
#define DANUBE_SDRAM_MC_IOGP_GPR5 (value)               (((( 1 << 4) - 1) & (value)) << 24)
#define DANUBE_SDRAM_MC_IOGP_GPR4 (value)               (((( 1 << 4) - 1) & (value)) << 20)
#define DANUBE_SDRAM_MC_IOGP_GPR3 (value)               (((( 1 << 4) - 1) & (value)) << 16)
#define DANUBE_SDRAM_MC_IOGP_GPR2 (value)               (((( 1 << 4) - 1) & (value)) << 12)
#define DANUBE_SDRAM_MC_IOGP_CPS                              (1 << 11)
#define DANUBE_SDRAM_MC_IOGP_CLKDELAY (value)          (((( 1 << 3) - 1) & (value)) << 8)
#define DANUBE_SDRAM_MC_IOGP_CLKRAT (value)             (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_SDRAM_MC_IOGP_RDDEL (value)              (((( 1 << 4) - 1) & (value)) << 0)

/***MC Self Refresh Register***/
#define DANUBE_SDRAM_MC_SELFRFSH                  ((volatile u32*)(DANUBE_SDRAM+ 0x0A00))
#define DANUBE_SDRAM_MC_SELFRFSH_PWDS                            (1 << 1)
#define DANUBE_SDRAM_MC_SELFRFSH_PWD                              (1 << 0)
#define DANUBE_SDRAM_MC_SELFRFSH_Res (value)                (((( 1 << 30) - 1) & (value)) << 2)

/***MC Enable Register***/
#define DANUBE_SDRAM_MC_CTRLENA                   ((volatile u32*)(DANUBE_SDRAM+ 0x1000))
#define DANUBE_SDRAM_MC_CTRLENA_ENA                              (1 << 0)
#define DANUBE_SDRAM_MC_CTRLENA_Res (value)                (((( 1 << 31) - 1) & (value)) << 1)

/***MC Mode Register Setup Code***/
#define DANUBE_SDRAM_MC_MRSCODE                   ((volatile u32*)(DANUBE_SDRAM+ 0x1008))
#define DANUBE_SDRAM_MC_MRSCODE_UMC (value)                (((( 1 << 5) - 1) & (value)) << 7)
#define DANUBE_SDRAM_MC_MRSCODE_CL (value)                (((( 1 << 3) - 1) & (value)) << 4)
#define DANUBE_SDRAM_MC_MRSCODE_WT                              (1 << 3)
#define DANUBE_SDRAM_MC_MRSCODE_BL (value)                (((( 1 << 3) - 1) & (value)) << 0)

/***MC Configuration Data-word Width Register***/
#define DANUBE_SDRAM_MC_CFGDW                    ((volatile u32*)(DANUBE_SDRAM+ 0x1010))
#define DANUBE_SDRAM_MC_CFGDW_DW (value)                (((( 1 << 4) - 1) & (value)) << 0)
#define DANUBE_SDRAM_MC_CFGDW_Res (value)                (((( 1 << 28) - 1) & (value)) << 4)

/***MC Configuration Physical Bank 0 Register***/
#define DANUBE_SDRAM_MC_CFGPB0                    ((volatile u32*)(DANUBE_SDRAM+ 0x1018))
#define DANUBE_SDRAM_MC_CFGPB0_MCSEN0 (value)             (((( 1 << 4) - 1) & (value)) << 12)
#define DANUBE_SDRAM_MC_CFGPB0_BANKN0 (value)             (((( 1 << 4) - 1) & (value)) << 8)
#define DANUBE_SDRAM_MC_CFGPB0_ROWW0 (value)              (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_SDRAM_MC_CFGPB0_COLW0 (value)              (((( 1 << 4) - 1) & (value)) << 0)
#define DANUBE_SDRAM_MC_CFGPB0_Res (value)                (((( 1 << 16) - 1) & (value)) << 16)

/***MC Latency Register***/
#define DANUBE_SDRAM_MC_LATENCY                   ((volatile u32*)(DANUBE_SDRAM+ 0x1038))
#define DANUBE_SDRAM_MC_LATENCY_TRP (value)                (((( 1 << 4) - 1) & (value)) << 16)
#define DANUBE_SDRAM_MC_LATENCY_TRAS (value)               (((( 1 << 4) - 1) & (value)) << 12)
#define DANUBE_SDRAM_MC_LATENCY_TRCD (value)               (((( 1 << 4) - 1) & (value)) << 8)
#define DANUBE_SDRAM_MC_LATENCY_TDPL (value)               (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_SDRAM_MC_LATENCY_TDAL (value)               (((( 1 << 4) - 1) & (value)) << 0)
#define DANUBE_SDRAM_MC_LATENCY_Res (value)                (((( 1 << 12) - 1) & (value)) << 20)

/***MC Refresh Cycle Time Register***/
#define DANUBE_SDRAM_MC_TREFRESH                  ((volatile u32*)(DANUBE_SDRAM+ 0x1040))
#define DANUBE_SDRAM_MC_TREFRESH_TREF (value)               (((( 1 << 13) - 1) & (value)) << 0)
#define DANUBE_SDRAM_MC_TREFRESH_Res (value)                (((( 1 << 19) - 1) & (value)) << 13)


/***********************************************************************/
/*  Module      :  GPTU register address and bits                      */
/***********************************************************************/

#define DANUBE_GPTU                         (0xB8000300)
/***********************************************************************/


/***GPT Clock Control Register***/
#define DANUBE_GPTU_GPT_CLC                      ((volatile u32*)(DANUBE_GPTU+ 0x0000))
#define DANUBE_GPTU_GPT_CLC_RMC (value)                (((( 1 << 8) - 1) & (value)) << 8)
#define DANUBE_GPTU_GPT_CLC_DISS                            (1 << 1)
#define DANUBE_GPTU_GPT_CLC_DISR                            (1 << 0)

/***GPT Timer 3 Control Register***/
#define DANUBE_GPTU_GPT_T3CON                    ((volatile u32*)(DANUBE_GPTU+ 0x0014))
#define DANUBE_GPTU_GPT_T3CON_T3RDIR                        (1 << 15)
#define DANUBE_GPTU_GPT_T3CON_T3CHDIR                      (1 << 14)
#define DANUBE_GPTU_GPT_T3CON_T3EDGE                        (1 << 13)
#define DANUBE_GPTU_GPT_T3CON_BPS1 (value)               (((( 1 << 2) - 1) & (value)) << 11)
#define DANUBE_GPTU_GPT_T3CON_T3OTL                          (1 << 10)
#define DANUBE_GPTU_GPT_T3CON_T3UD                            (1 << 7)
#define DANUBE_GPTU_GPT_T3CON_T3R                              (1 << 6)
#define DANUBE_GPTU_GPT_T3CON_T3M (value)                (((( 1 << 3) - 1) & (value)) << 3)
#define DANUBE_GPTU_GPT_T3CON_T3I (value)                (((( 1 << 3) - 1) & (value)) << 0)

/***GPT Write Hardware Modified Timer 3 Control Register
If set and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define DANUBE_GPTU_GPT_WHBT3CON                 ((volatile u32*)(DANUBE_GPTU+ 0x004C))
#define DANUBE_GPTU_GPT_WHBT3CON_SETT3CHDIR                (1 << 15)
#define DANUBE_GPTU_GPT_WHBT3CON_CLRT3CHDIR                (1 << 14)
#define DANUBE_GPTU_GPT_WHBT3CON_SETT3EDGE                  (1 << 13)
#define DANUBE_GPTU_GPT_WHBT3CON_CLRT3EDGE                  (1 << 12)
#define DANUBE_GPTU_GPT_WHBT3CON_SETT3OTL                  (1 << 11)
#define DANUBE_GPTU_GPT_WHBT3CON_CLRT3OTL                  (1 << 10)

/***GPT Timer 2 Control Register***/
#define DANUBE_GPTU_GPT_T2CON                    ((volatile u32*)(DANUBE_GPTU+ 0x0010))
#define DANUBE_GPTU_GPT_T2CON_TxRDIR                        (1 << 15)
#define DANUBE_GPTU_GPT_T2CON_TxCHDIR                      (1 << 14)
#define DANUBE_GPTU_GPT_T2CON_TxEDGE                        (1 << 13)
#define DANUBE_GPTU_GPT_T2CON_TxIRDIS                      (1 << 12)
#define DANUBE_GPTU_GPT_T2CON_TxRC                            (1 << 9)
#define DANUBE_GPTU_GPT_T2CON_TxUD                            (1 << 7)
#define DANUBE_GPTU_GPT_T2CON_TxR                              (1 << 6)
#define DANUBE_GPTU_GPT_T2CON_TxM (value)                (((( 1 << 3) - 1) & (value)) << 3)
#define DANUBE_GPTU_GPT_T2CON_TxI (value)                (((( 1 << 3) - 1) & (value)) << 0)

/***GPT Timer 4 Control Register***/
#define DANUBE_GPTU_GPT_T4CON                    ((volatile u32*)(DANUBE_GPTU+ 0x0018))
#define DANUBE_GPTU_GPT_T4CON_TxRDIR                        (1 << 15)
#define DANUBE_GPTU_GPT_T4CON_TxCHDIR                      (1 << 14)
#define DANUBE_GPTU_GPT_T4CON_TxEDGE                        (1 << 13)
#define DANUBE_GPTU_GPT_T4CON_TxIRDIS                      (1 << 12)
#define DANUBE_GPTU_GPT_T4CON_TxRC                            (1 << 9)
#define DANUBE_GPTU_GPT_T4CON_TxUD                            (1 << 7)
#define DANUBE_GPTU_GPT_T4CON_TxR                              (1 << 6)
#define DANUBE_GPTU_GPT_T4CON_TxM (value)                (((( 1 << 3) - 1) & (value)) << 3)
#define DANUBE_GPTU_GPT_T4CON_TxI (value)                (((( 1 << 3) - 1) & (value)) << 0)

/***GPT Write HW Modified Timer 2 Control Register If set
 and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define DANUBE_GPTU_GPT_WHBT2CON                 ((volatile u32*)(DANUBE_GPTU+ 0x0048))
#define DANUBE_GPTU_GPT_WHBT2CON_SETTxCHDIR                (1 << 15)
#define DANUBE_GPTU_GPT_WHBT2CON_CLRTxCHDIR                (1 << 14)
#define DANUBE_GPTU_GPT_WHBT2CON_SETTxEDGE                  (1 << 13)
#define DANUBE_GPTU_GPT_WHBT2CON_CLRTxEDGE                  (1 << 12)

/***GPT Write HW Modified Timer 4 Control Register If set
 and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define DANUBE_GPTU_GPT_WHBT4CON                 ((volatile u32*)(DANUBE_GPTU+ 0x0050))
#define DANUBE_GPTU_GPT_WHBT4CON_SETTxCHDIR                (1 << 15)
#define DANUBE_GPTU_GPT_WHBT4CON_CLRTxCHDIR                (1 << 14)
#define DANUBE_GPTU_GPT_WHBT4CON_SETTxEDGE                  (1 << 13)
#define DANUBE_GPTU_GPT_WHBT4CON_CLRTxEDGE                  (1 << 12)

/***GPT Capture Reload Register***/
#define DANUBE_GPTU_GPT_CAPREL                   ((volatile u32*)(DANUBE_GPTU+ 0x0030))
#define DANUBE_GPTU_GPT_CAPREL_CAPREL (value)             (((( 1 << 16) - 1) & (value)) << 0)

/***GPT Timer 2 Register***/
#define DANUBE_GPTU_GPT_T2                       ((volatile u32*)(DANUBE_GPTU+ 0x0034))
#define DANUBE_GPTU_GPT_T2_TVAL (value)               (((( 1 << 16) - 1) & (value)) << 0)

/***GPT Timer 3 Register***/
#define DANUBE_GPTU_GPT_T3                       ((volatile u32*)(DANUBE_GPTU+ 0x0038))
#define DANUBE_GPTU_GPT_T3_TVAL (value)               (((( 1 << 16) - 1) & (value)) << 0)

/***GPT Timer 4 Register***/
#define DANUBE_GPTU_GPT_T4                       ((volatile u32*)(DANUBE_GPTU+ 0x003C))
#define DANUBE_GPTU_GPT_T4_TVAL (value)               (((( 1 << 16) - 1) & (value)) << 0)

/***GPT Timer 5 Register***/
#define DANUBE_GPTU_GPT_T5                       ((volatile u32*)(DANUBE_GPTU+ 0x0040))
#define DANUBE_GPTU_GPT_T5_TVAL (value)               (((( 1 << 16) - 1) & (value)) << 0)

/***GPT Timer 6 Register***/
#define DANUBE_GPTU_GPT_T6                       ((volatile u32*)(DANUBE_GPTU+ 0x0044))
#define DANUBE_GPTU_GPT_T6_TVAL (value)               (((( 1 << 16) - 1) & (value)) << 0)

/***GPT Timer 6 Control Register***/
#define DANUBE_GPTU_GPT_T6CON                    ((volatile u32*)(DANUBE_GPTU+ 0x0020))
#define DANUBE_GPTU_GPT_T6CON_T6SR                            (1 << 15)
#define DANUBE_GPTU_GPT_T6CON_T6CLR                          (1 << 14)
#define DANUBE_GPTU_GPT_T6CON_BPS2 (value)               (((( 1 << 2) - 1) & (value)) << 11)
#define DANUBE_GPTU_GPT_T6CON_T6OTL                          (1 << 10)
#define DANUBE_GPTU_GPT_T6CON_T6UD                            (1 << 7)
#define DANUBE_GPTU_GPT_T6CON_T6R                              (1 << 6)
#define DANUBE_GPTU_GPT_T6CON_T6M (value)                (((( 1 << 3) - 1) & (value)) << 3)
#define DANUBE_GPTU_GPT_T6CON_T6I (value)                (((( 1 << 3) - 1) & (value)) << 0)

/***GPT Write HW Modified Timer 6 Control Register If set
 and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define DANUBE_GPTU_GPT_WHBT6CON                 ((volatile u32*)(DANUBE_GPTU+ 0x0054))
#define DANUBE_GPTU_GPT_WHBT6CON_SETT6OTL                  (1 << 11)
#define DANUBE_GPTU_GPT_WHBT6CON_CLRT6OTL                  (1 << 10)

/***GPT Timer 5 Control Register***/
#define DANUBE_GPTU_GPT_T5CON                    ((volatile u32*)(DANUBE_GPTU+ 0x001C))
#define DANUBE_GPTU_GPT_T5CON_T5SC                            (1 << 15)
#define DANUBE_GPTU_GPT_T5CON_T5CLR                          (1 << 14)
#define DANUBE_GPTU_GPT_T5CON_CI (value)                (((( 1 << 2) - 1) & (value)) << 12)
#define DANUBE_GPTU_GPT_T5CON_T5CC                            (1 << 11)
#define DANUBE_GPTU_GPT_T5CON_CT3                              (1 << 10)
#define DANUBE_GPTU_GPT_T5CON_T5RC                            (1 << 9)
#define DANUBE_GPTU_GPT_T5CON_T5UDE                          (1 << 8)
#define DANUBE_GPTU_GPT_T5CON_T5UD                            (1 << 7)
#define DANUBE_GPTU_GPT_T5CON_T5R                              (1 << 6)
#define DANUBE_GPTU_GPT_T5CON_T5M (value)                (((( 1 << 3) - 1) & (value)) << 3)
#define DANUBE_GPTU_GPT_T5CON_T5I (value)                (((( 1 << 3) - 1) & (value)) << 0)


/***********************************************************************/
/*  Module      :  IOM register address and bits                       */
/***********************************************************************/

#define DANUBE_IOM                          (0xBF105000)
/***********************************************************************/


/***Receive FIFO***/
#define DANUBE_IOM_RFIFO                        ((volatile u32*)(DANUBE_IOM+ 0x0000))
#define DANUBE_IOM_RFIFO_RXD (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***Transmit FIFO***/
#define DANUBE_IOM_XFIFO                        ((volatile u32*)(DANUBE_IOM+ 0x0000))
#define DANUBE_IOM_XFIFO_TXD (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***Interrupt Status Register HDLC***/
#define DANUBE_IOM_ISTAH                        ((volatile u32*)(DANUBE_IOM+ 0x0080))
#define DANUBE_IOM_ISTAH_RME                              (1 << 7)
#define DANUBE_IOM_ISTAH_RPF                              (1 << 6)
#define DANUBE_IOM_ISTAH_RFO                              (1 << 5)
#define DANUBE_IOM_ISTAH_XPR                              (1 << 4)
#define DANUBE_IOM_ISTAH_XMR                              (1 << 3)
#define DANUBE_IOM_ISTAH_XDU                              (1 << 2)

/***Interrupt Mask Register HDLC***/
#define DANUBE_IOM_MASKH                        ((volatile u32*)(DANUBE_IOM+ 0x0080))
#define DANUBE_IOM_MASKH_RME                              (1 << 7)
#define DANUBE_IOM_MASKH_RPF                              (1 << 6)
#define DANUBE_IOM_MASKH_RFO                              (1 << 5)
#define DANUBE_IOM_MASKH_XPR                              (1 << 4)
#define DANUBE_IOM_MASKH_XMR                              (1 << 3)
#define DANUBE_IOM_MASKH_XDU                              (1 << 2)

/***Status Register***/
#define DANUBE_IOM_STAR                         ((volatile u32*)(DANUBE_IOM+ 0x0084))
#define DANUBE_IOM_STAR_XDOV                            (1 << 7)
#define DANUBE_IOM_STAR_XFW                              (1 << 6)
#define DANUBE_IOM_STAR_RACI                            (1 << 3)
#define DANUBE_IOM_STAR_XACI                            (1 << 1)

/***Command Register***/
#define DANUBE_IOM_CMDR                         ((volatile u32*)(DANUBE_IOM+ 0x0084))
#define DANUBE_IOM_CMDR_RMC                              (1 << 7)
#define DANUBE_IOM_CMDR_RRES                            (1 << 6)
#define DANUBE_IOM_CMDR_XTF                              (1 << 3)
#define DANUBE_IOM_CMDR_XME                              (1 << 1)
#define DANUBE_IOM_CMDR_XRES                            (1 << 0)

/***Mode Register***/
#define DANUBE_IOM_MODEH                        ((volatile u32*)(DANUBE_IOM+ 0x0088))
#define DANUBE_IOM_MODEH_MDS2                            (1 << 7)
#define DANUBE_IOM_MODEH_MDS1                            (1 << 6)
#define DANUBE_IOM_MODEH_MDS0                            (1 << 5)
#define DANUBE_IOM_MODEH_RAC                              (1 << 3)
#define DANUBE_IOM_MODEH_DIM2                            (1 << 2)
#define DANUBE_IOM_MODEH_DIM1                            (1 << 1)
#define DANUBE_IOM_MODEH_DIM0                            (1 << 0)

/***Extended Mode Register***/
#define DANUBE_IOM_EXMR                         ((volatile u32*)(DANUBE_IOM+ 0x008C))
#define DANUBE_IOM_EXMR_XFBS                            (1 << 7)
#define DANUBE_IOM_EXMR_RFBS (value)               (((( 1 << 2) - 1) & (value)) << 5)
#define DANUBE_IOM_EXMR_SRA                              (1 << 4)
#define DANUBE_IOM_EXMR_XCRC                            (1 << 3)
#define DANUBE_IOM_EXMR_RCRC                            (1 << 2)
#define DANUBE_IOM_EXMR_ITF                              (1 << 0)

/***SAPI1 Register***/
#define DANUBE_IOM_SAP1                         ((volatile u32*)(DANUBE_IOM+ 0x0094))
#define DANUBE_IOM_SAP1_SAPI1 (value)              (((( 1 << 6) - 1) & (value)) << 2)
#define DANUBE_IOM_SAP1_MHA                              (1 << 0)

/***Receive Frame Byte Count Low***/
#define DANUBE_IOM_RBCL                         ((volatile u32*)(DANUBE_IOM+ 0x0098))
#define DANUBE_IOM_RBCL_RBC(value)              (1 << value)


/***SAPI2 Register***/
#define DANUBE_IOM_SAP2                         ((volatile u32*)(DANUBE_IOM+ 0x0098))
#define DANUBE_IOM_SAP2_SAPI2 (value)              (((( 1 << 6) - 1) & (value)) << 2)
#define DANUBE_IOM_SAP2_MLA                              (1 << 0)

/***Receive Frame Byte Count High***/
#define DANUBE_IOM_RBCH                         ((volatile u32*)(DANUBE_IOM+ 0x009C))
#define DANUBE_IOM_RBCH_OV                              (1 << 4)
#define DANUBE_IOM_RBCH_RBC11                          (1 << 3)
#define DANUBE_IOM_RBCH_RBC10                          (1 << 2)
#define DANUBE_IOM_RBCH_RBC9                            (1 << 1)
#define DANUBE_IOM_RBCH_RBC8                            (1 << 0)

/***TEI1 Register 1***/
#define DANUBE_IOM_TEI1                         ((volatile u32*)(DANUBE_IOM+ 0x009C))
#define DANUBE_IOM_TEI1_TEI1 (value)               (((( 1 << 7) - 1) & (value)) << 1)
#define DANUBE_IOM_TEI1_EA                              (1 << 0)

/***Receive Status Register***/
#define DANUBE_IOM_RSTA                         ((volatile u32*)(DANUBE_IOM+ 0x00A0))
#define DANUBE_IOM_RSTA_VFR                              (1 << 7)
#define DANUBE_IOM_RSTA_RDO                              (1 << 6)
#define DANUBE_IOM_RSTA_CRC                              (1 << 5)
#define DANUBE_IOM_RSTA_RAB                              (1 << 4)
#define DANUBE_IOM_RSTA_SA1                              (1 << 3)
#define DANUBE_IOM_RSTA_SA0                              (1 << 2)
#define DANUBE_IOM_RSTA_TA                              (1 << 0)
#define DANUBE_IOM_RSTA_CR                              (1 << 1)

/***TEI2 Register***/
#define DANUBE_IOM_TEI2                         ((volatile u32*)(DANUBE_IOM+ 0x00A0))
#define DANUBE_IOM_TEI2_TEI2 (value)               (((( 1 << 7) - 1) & (value)) << 1)
#define DANUBE_IOM_TEI2_EA                              (1 << 0)

/***Test Mode Register HDLC***/
#define DANUBE_IOM_TMH                          ((volatile u32*)(DANUBE_IOM+ 0x00A4))
#define DANUBE_IOM_TMH_TLP                              (1 << 0)

/***Command/Indication Receive 0***/
#define DANUBE_IOM_CIR0                         ((volatile u32*)(DANUBE_IOM+ 0x00B8))
#define DANUBE_IOM_CIR0_CODR0 (value)              (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_IOM_CIR0_CIC0                            (1 << 3)
#define DANUBE_IOM_CIR0_CIC1                            (1 << 2)
#define DANUBE_IOM_CIR0_SG                              (1 << 1)
#define DANUBE_IOM_CIR0_BAS                              (1 << 0)

/***Command/Indication Transmit 0***/
#define DANUBE_IOM_CIX0                         ((volatile u32*)(DANUBE_IOM+ 0x00B8))
#define DANUBE_IOM_CIX0_CODX0 (value)              (((( 1 << 4) - 1) & (value)) << 4)
#define DANUBE_IOM_CIX0_TBA2                            (1 << 3)
#define DANUBE_IOM_CIX0_TBA1                            (1 << 2)
#define DANUBE_IOM_CIX0_TBA0                            (1 << 1)
#define DANUBE_IOM_CIX0_BAC                              (1 << 0)

/***Command/Indication Receive 1***/
#define DANUBE_IOM_CIR1                         ((volatile u32*)(DANUBE_IOM+ 0x00BC))
#define DANUBE_IOM_CIR1_CODR1 (value)              (((( 1 << 6) - 1) & (value)) << 2)

/***Command/Indication Transmit 1***/
#define DANUBE_IOM_CIX1                         ((volatile u32*)(DANUBE_IOM+ 0x00BC))
#define DANUBE_IOM_CIX1_CODX1 (value)              (((( 1 << 6) - 1) & (value)) << 2)
#define DANUBE_IOM_CIX1_CICW                            (1 << 1)
#define DANUBE_IOM_CIX1_CI1E                            (1 << 0)

/***Controller Data Access Reg. (CH10)***/
#define DANUBE_IOM_CDA10                        ((volatile u32*)(DANUBE_IOM+ 0x0100))
#define DANUBE_IOM_CDA10_CDA (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***Controller Data Access Reg. (CH11)***/
#define DANUBE_IOM_CDA11                        ((volatile u32*)(DANUBE_IOM+ 0x0104))
#define DANUBE_IOM_CDA11_CDA (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***Controller Data Access Reg. (CH20)***/
#define DANUBE_IOM_CDA20                        ((volatile u32*)(DANUBE_IOM+ 0x0108))
#define DANUBE_IOM_CDA20_CDA (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***Controller Data Access Reg. (CH21)***/
#define DANUBE_IOM_CDA21                        ((volatile u32*)(DANUBE_IOM+ 0x010C))
#define DANUBE_IOM_CDA21_CDA (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH10)***/
#define DANUBE_IOM_CDA_TSDP10                   ((volatile u32*)(DANUBE_IOM+ 0x0110))
#define DANUBE_IOM_CDA_TSDP10_DPS                              (1 << 7)
#define DANUBE_IOM_CDA_TSDP10_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH11)***/
#define DANUBE_IOM_CDA_TSDP11                   ((volatile u32*)(DANUBE_IOM+ 0x0114))
#define DANUBE_IOM_CDA_TSDP11_DPS                              (1 << 7)
#define DANUBE_IOM_CDA_TSDP11_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH20)***/
#define DANUBE_IOM_CDA_TSDP20                   ((volatile u32*)(DANUBE_IOM+ 0x0118))
#define DANUBE_IOM_CDA_TSDP20_DPS                              (1 << 7)
#define DANUBE_IOM_CDA_TSDP20_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH21)***/
#define DANUBE_IOM_CDA_TSDP21                   ((volatile u32*)(DANUBE_IOM+ 0x011C))
#define DANUBE_IOM_CDA_TSDP21_DPS                              (1 << 7)
#define DANUBE_IOM_CDA_TSDP21_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH10)***/
#define DANUBE_IOM_CO_TSDP10                    ((volatile u32*)(DANUBE_IOM+ 0x0120))
#define DANUBE_IOM_CO_TSDP10_DPS                              (1 << 7)
#define DANUBE_IOM_CO_TSDP10_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH11)***/
#define DANUBE_IOM_CO_TSDP11                    ((volatile u32*)(DANUBE_IOM+ 0x0124))
#define DANUBE_IOM_CO_TSDP11_DPS                              (1 << 7)
#define DANUBE_IOM_CO_TSDP11_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH20)***/
#define DANUBE_IOM_CO_TSDP20                    ((volatile u32*)(DANUBE_IOM+ 0x0128))
#define DANUBE_IOM_CO_TSDP20_DPS                              (1 << 7)
#define DANUBE_IOM_CO_TSDP20_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Time Slot and Data Port Sel. (CH21)***/
#define DANUBE_IOM_CO_TSDP21                    ((volatile u32*)(DANUBE_IOM+ 0x012C))
#define DANUBE_IOM_CO_TSDP21_DPS                              (1 << 7)
#define DANUBE_IOM_CO_TSDP21_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Ctrl. Reg. Contr. Data Access CH1x***/
#define DANUBE_IOM_CDA1_CR                      ((volatile u32*)(DANUBE_IOM+ 0x0138))
#define DANUBE_IOM_CDA1_CR_EN_TBM                        (1 << 5)
#define DANUBE_IOM_CDA1_CR_EN_I1                          (1 << 4)
#define DANUBE_IOM_CDA1_CR_EN_I0                          (1 << 3)
#define DANUBE_IOM_CDA1_CR_EN_O1                          (1 << 2)
#define DANUBE_IOM_CDA1_CR_EN_O0                          (1 << 1)
#define DANUBE_IOM_CDA1_CR_SWAP                            (1 << 0)

/***Ctrl. Reg. Contr. Data Access CH1x***/
#define DANUBE_IOM_CDA2_CR                      ((volatile u32*)(DANUBE_IOM+ 0x013C))
#define DANUBE_IOM_CDA2_CR_EN_TBM                        (1 << 5)
#define DANUBE_IOM_CDA2_CR_EN_I1                          (1 << 4)
#define DANUBE_IOM_CDA2_CR_EN_I0                          (1 << 3)
#define DANUBE_IOM_CDA2_CR_EN_O1                          (1 << 2)
#define DANUBE_IOM_CDA2_CR_EN_O0                          (1 << 1)
#define DANUBE_IOM_CDA2_CR_SWAP                            (1 << 0)

/***Control Register B-Channel Data***/
#define DANUBE_IOM_BCHA_CR                      ((volatile u32*)(DANUBE_IOM+ 0x0144))
#define DANUBE_IOM_BCHA_CR_EN_BC2                        (1 << 4)
#define DANUBE_IOM_BCHA_CR_EN_BC1                        (1 << 3)

/***Control Register B-Channel Data***/
#define DANUBE_IOM_BCHB_CR                      ((volatile u32*)(DANUBE_IOM+ 0x0148))
#define DANUBE_IOM_BCHB_CR_EN_BC2                        (1 << 4)
#define DANUBE_IOM_BCHB_CR_EN_BC1                        (1 << 3)

/***Control Reg. for HDLC and CI1 Data***/
#define DANUBE_IOM_DCI_CR                       ((volatile u32*)(DANUBE_IOM+ 0x014C))
#define DANUBE_IOM_DCI_CR_DPS_CI1                      (1 << 7)
#define DANUBE_IOM_DCI_CR_EN_CI1                        (1 << 6)
#define DANUBE_IOM_DCI_CR_EN_D                            (1 << 5)

/***Control Reg. for HDLC and CI1 Data***/
#define DANUBE_IOM_DCIC_CR                      ((volatile u32*)(DANUBE_IOM+ 0x014C))
#define DANUBE_IOM_DCIC_CR_DPS_CI0                      (1 << 7)
#define DANUBE_IOM_DCIC_CR_EN_CI0                        (1 << 6)
#define DANUBE_IOM_DCIC_CR_DPS_D                          (1 << 5)

/***Control Reg. Serial Data Strobe x***/
#define DANUBE_IOM_SDS_CR                       ((volatile u32*)(DANUBE_IOM+ 0x0154))
#define DANUBE_IOM_SDS_CR_ENS_TSS                      (1 << 7)
#define DANUBE_IOM_SDS_CR_ENS_TSS_1                  (1 << 6)
#define DANUBE_IOM_SDS_CR_ENS_TSS_3                  (1 << 5)
#define DANUBE_IOM_SDS_CR_TSS (value)                (((( 1 << 4) - 1) & (value)) << 0)

/***Control Register IOM Data***/
#define DANUBE_IOM_IOM_CR                       ((volatile u32*)(DANUBE_IOM+ 0x015C))
#define DANUBE_IOM_IOM_CR_SPU                              (1 << 7)
#define DANUBE_IOM_IOM_CR_CI_CS                          (1 << 5)
#define DANUBE_IOM_IOM_CR_TIC_DIS                      (1 << 4)
#define DANUBE_IOM_IOM_CR_EN_BCL                        (1 << 3)
#define DANUBE_IOM_IOM_CR_CLKM                            (1 << 2)
#define DANUBE_IOM_IOM_CR_Res                              (1 << 1)
#define DANUBE_IOM_IOM_CR_DIS_IOM                      (1 << 0)

/***Synchronous Transfer Interrupt***/
#define DANUBE_IOM_STI                          ((volatile u32*)(DANUBE_IOM+ 0x0160))
#define DANUBE_IOM_STI_STOV21                        (1 << 7)
#define DANUBE_IOM_STI_STOV20                        (1 << 6)
#define DANUBE_IOM_STI_STOV11                        (1 << 5)
#define DANUBE_IOM_STI_STOV10                        (1 << 4)
#define DANUBE_IOM_STI_STI21                          (1 << 3)
#define DANUBE_IOM_STI_STI20                          (1 << 2)
#define DANUBE_IOM_STI_STI11                          (1 << 1)
#define DANUBE_IOM_STI_STI10                          (1 << 0)

/***Acknowledge Synchronous Transfer Interrupt***/
#define DANUBE_IOM_ASTI                         ((volatile u32*)(DANUBE_IOM+ 0x0160))
#define DANUBE_IOM_ASTI_ACK21                          (1 << 3)
#define DANUBE_IOM_ASTI_ACK20                          (1 << 2)
#define DANUBE_IOM_ASTI_ACK11                          (1 << 1)
#define DANUBE_IOM_ASTI_ACK10                          (1 << 0)

/***Mask Synchronous Transfer Interrupt***/
#define DANUBE_IOM_MSTI                         ((volatile u32*)(DANUBE_IOM+ 0x0164))
#define DANUBE_IOM_MSTI_STOV21                        (1 << 7)
#define DANUBE_IOM_MSTI_STOV20                        (1 << 6)
#define DANUBE_IOM_MSTI_STOV11                        (1 << 5)
#define DANUBE_IOM_MSTI_STOV10                        (1 << 4)
#define DANUBE_IOM_MSTI_STI21                          (1 << 3)
#define DANUBE_IOM_MSTI_STI20                          (1 << 2)
#define DANUBE_IOM_MSTI_STI11                          (1 << 1)
#define DANUBE_IOM_MSTI_STI10                          (1 << 0)

/***Configuration Register for Serial Data Strobes***/
#define DANUBE_IOM_SDS_CONF                    ((volatile u32*)(DANUBE_IOM+ 0x0168))
#define DANUBE_IOM_SDS_CONF_SDS_BCL                      (1 << 0)

/***Monitoring CDA Bits***/
#define DANUBE_IOM_MCDA                         ((volatile u32*)(DANUBE_IOM+ 0x016C))
#define DANUBE_IOM_MCDA_MCDA21 (value)             (((( 1 << 2) - 1) & (value)) << 6)
#define DANUBE_IOM_MCDA_MCDA20 (value)             (((( 1 << 2) - 1) & (value)) << 4)
#define DANUBE_IOM_MCDA_MCDA11 (value)             (((( 1 << 2) - 1) & (value)) << 2)
#define DANUBE_IOM_MCDA_MCDA10 (value)             (((( 1 << 2) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  ASC0 register address and bits                      */
/***********************************************************************/
#define DANUBE_ASC0                          (KSEG1+0x1E100400)
/***********************************************************************/
#define DANUBE_ASC0_TBUF                        ((volatile u32*)(DANUBE_ASC0 + 0x0020))
#define DANUBE_ASC0_RBUF                        ((volatile u32*)(DANUBE_ASC0 + 0x0024))
#define DANUBE_ASC0_FSTAT                       ((volatile u32*)(DANUBE_ASC0 + 0x0048))
#define DANUBE_ASC0_FSTAT_TXFREE_GET(value)     (((value) >> 24) & ((1 << 6) - 1))
#define DANUBE_ASC0_FSTAT_TXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 24)
#define DANUBE_ASC0_FSTAT_RXFREE_GET(value)     (((value) >> 16) & ((1 << 6) - 1))
#define DANUBE_ASC0_FSTAT_RXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 16)
#define DANUBE_ASC0_FSTAT_TXFFL_GET(value)      (((value) >> 8) & ((1 << 6) - 1))
#define DANUBE_ASC0_FSTAT_TXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_ASC0_FSTAT_RXFFL_GET(value)      (((value) >> 0) & ((1 << 6) - 1))
#define DANUBE_ASC0_FSTAT_RXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 0)


/***********************************************************************/
/*  Module      :  ASC1 register address and bits                      */
/***********************************************************************/

#define DANUBE_ASC1                          (KSEG1+0x1E100C00)
	/***********************************************************************/

#define DANUBE_ASC1_TBUF                        ((volatile u32*)(DANUBE_ASC1 + 0x0020))
#define DANUBE_ASC1_RBUF                        ((volatile u32*)(DANUBE_ASC1 + 0x0024))
#define DANUBE_ASC1_FSTAT                       ((volatile u32*)(DANUBE_ASC1 + 0x0048))
#define DANUBE_ASC1_FSTAT_TXFREE_GET(value)     (((value) >> 24) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_TXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 24)
#define DANUBE_ASC1_FSTAT_RXFREE_GET(value)     (((value) >> 16) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_RXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 16)
#define DANUBE_ASC1_FSTAT_TXFFL_GET(value)      (((value) >> 8) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_TXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_ASC1_FSTAT_RXFFL_GET(value)      (((value) >> 0) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_RXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/

#define DANUBE_DMA                          (0xBE104100)
/***********************************************************************/

#define DANUBE_DMA_BASE                 DANUBE_DMA
#define DANUBE_DMA_CLC                  (volatile u32*)DANUBE_DMA_BASE
#define DANUBE_DMA_ID                   (volatile u32*)(DANUBE_DMA_BASE+0x08)
#define DANUBE_DMA_CTRL                 (volatile u32*)(DANUBE_DMA_BASE+0x10)
#define DANUBE_DMA_CPOLL                (volatile u32*)(DANUBE_DMA_BASE+0x14)
#define DANUBE_DMA_CS                   (volatile u32*)(DANUBE_DMA_BASE+0x18)
#define DANUBE_DMA_CCTRL                (volatile u32*)(DANUBE_DMA_BASE+0x1C)
#define DANUBE_DMA_CDBA                 (volatile u32*)(DANUBE_DMA_BASE+0x20)
#define DANUBE_DMA_CDLEN                (volatile u32*)(DANUBE_DMA_BASE+0x24)
#define DANUBE_DMA_CIS                  (volatile u32*)(DANUBE_DMA_BASE+0x28)
#define DANUBE_DMA_CIE                  (volatile u32*)(DANUBE_DMA_BASE+0x2C)

#define DANUBE_DMA_PS                   (volatile u32*)(DANUBE_DMA_BASE+0x40)
#define DANUBE_DMA_PCTRL                (volatile u32*)(DANUBE_DMA_BASE+0x44)

#define DANUBE_DMA_IRNEN                (volatile u32*)(DANUBE_DMA_BASE+0xf4)
#define DANUBE_DMA_IRNCR                (volatile u32*)(DANUBE_DMA_BASE+0xf8)
#define DANUBE_DMA_IRNICR               (volatile u32*)(DANUBE_DMA_BASE+0xfc)
/***********************************************************************/
/*  Module      :  Debug register address and bits                     */
/***********************************************************************/

#define DANUBE_Debug                        (0xBF106000)
/***********************************************************************/


/***MCD Break Bus Switch Register***/
#define DANUBE_Debug_MCD_BBS                      ((volatile u32*)(DANUBE_Debug+ 0x0000))
#define DANUBE_Debug_MCD_BBS_BTP1                            (1 << 19)
#define DANUBE_Debug_MCD_BBS_BTP0                            (1 << 18)
#define DANUBE_Debug_MCD_BBS_BSP1                            (1 << 17)
#define DANUBE_Debug_MCD_BBS_BSP0                            (1 << 16)
#define DANUBE_Debug_MCD_BBS_BT5EN                          (1 << 15)
#define DANUBE_Debug_MCD_BBS_BT4EN                          (1 << 14)
#define DANUBE_Debug_MCD_BBS_BT5                              (1 << 13)
#define DANUBE_Debug_MCD_BBS_BT4                              (1 << 12)
#define DANUBE_Debug_MCD_BBS_BS5EN                          (1 << 7)
#define DANUBE_Debug_MCD_BBS_BS4EN                          (1 << 6)
#define DANUBE_Debug_MCD_BBS_BS5                              (1 << 5)
#define DANUBE_Debug_MCD_BBS_BS4                              (1 << 4)

/***MCD Multiplexer Control Register***/
#define DANUBE_Debug_MCD_MCR                      ((volatile u32*)(DANUBE_Debug+ 0x0008))
#define DANUBE_Debug_MCD_MCR_MUX5                            (1 << 4)
#define DANUBE_Debug_MCD_MCR_MUX4                            (1 << 3)
#define DANUBE_Debug_MCD_MCR_MUX1                            (1 << 0)


/***********************************************************************/
/*  Module      :  SRAM register address and bits                      */
/***********************************************************************/

#define DANUBE_SRAM                         (0xBF980000)
/***********************************************************************/


/***SRAM Size Register***/
#define DANUBE_SRAM_SRAM_SIZE                    ((volatile u32*)(DANUBE_SRAM+ 0x0800))
#define DANUBE_SRAM_SRAM_SIZE_SIZE (value)               (((( 1 << 23) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  BIU register address and bits                       */
/***********************************************************************/

#define DANUBE_BIU                          (0xBFA80000)
/***********************************************************************/


/***BIU Identification Register***/
#define DANUBE_BIU_BIU_ID                       ((volatile u32*)(DANUBE_BIU+ 0x0000))
#define DANUBE_BIU_BIU_ID_ARCH                            (1 << 16)
#define DANUBE_BIU_BIU_ID_ID (value)                (((( 1 << 8) - 1) & (value)) << 8)
#define DANUBE_BIU_BIU_ID_REV (value)                (((( 1 << 8) - 1) & (value)) << 0)

/***BIU Access Error Cause Register***/
#define DANUBE_BIU_BIU_ERRCAUSE                 ((volatile u32*)(DANUBE_BIU+ 0x0100))
#define DANUBE_BIU_BIU_ERRCAUSE_ERR                              (1 << 31)
#define DANUBE_BIU_BIU_ERRCAUSE_PORT (value)               (((( 1 << 4) - 1) & (value)) << 16)
#define DANUBE_BIU_BIU_ERRCAUSE_CAUSE (value)              (((( 1 << 2) - 1) & (value)) << 0)

/***BIU Access Error Address Register***/
#define DANUBE_BIU_BIU_ERRADDR                  ((volatile u32*)(DANUBE_BIU+ 0x0108))
#define DANUBE_BIU_BIU_ERRADDR_ADDR


/***********************************************************************/
/*  Module      :  ICU register address and bits                       */
/***********************************************************************/

#define DANUBE_ICU                          (0xBF880200)
#define DANUBE_ICU                          (0xBF880200)
#define DANUBE_ICU_EXI                      (0xBF101000)
/***********************************************************************/


/***IM0 Interrupt Status Register***/
#define DANUBE_ICU_IM0_ISR                      ((volatile u32*)(DANUBE_ICU+ 0x0000))
#define DANUBE_ICU_IM0_ISR_IR(value)               (1 << (value))


/***IM1 Interrupt Status Register***/
#define DANUBE_ICU_IM1_ISR                      ((volatile u32*)(DANUBE_ICU+ 0x0020))
#define DANUBE_ICU_IM1_ISR_IR(value)               (1 << (value))


/***IM2 Interrupt Status Register***/
#define DANUBE_ICU_IM2_ISR                      ((volatile u32*)(DANUBE_ICU+ 0x0040))
#define DANUBE_ICU_IM2_ISR_IR(value)               (1 << (value))

/***IM3 Interrupt Status Register***/
#define DANUBE_ICU_IM3_ISR                      ((volatile u32*)(DANUBE_ICU+ 0x0060))
#define DANUBE_ICU_IM3_ISR_IR(value)               (1 << (value))

/***IM4 Interrupt Status Register***/
#define DANUBE_ICU_IM4_ISR                      ((volatile u32*)(DANUBE_ICU+ 0x0080))
#define DANUBE_ICU_IM4_ISR_IR(value)               (1 << (value))


/***IM0 Interrupt Enable Register***/
#define DANUBE_ICU_IM0_IER                      ((volatile u32*)(DANUBE_ICU+ 0x0008))
#define DANUBE_ICU_IM0_IER_IR(value)               (1 << (value))


/***IM1 Interrupt Enable Register***/
#define DANUBE_ICU_IM1_IER                      ((volatile u32*)(DANUBE_ICU+ 0x0028))
#define DANUBE_ICU_IM1_IER_IR(value)               (1 << (value))


/***IM2 Interrupt Enable Register***/
#define DANUBE_ICU_IM2_IER                      ((volatile u32*)(DANUBE_ICU+ 0x0048))
#define DANUBE_ICU_IM2_IER_IR(value)               (1 << (value)8

/***IM3 Interrupt Enable Register***/
#define DANUBE_ICU_IM3_IER                      ((volatile u32*)(DANUBE_ICU+ 0x0068))
#define DANUBE_ICU_IM3_IER_IR(value)               (1 << (value))

/***IM4 Interrupt Enable Register***/
#define DANUBE_ICU_IM4_IER                      ((volatile u32*)(DANUBE_ICU+ 0x0088))
#define DANUBE_ICU_IM4_IER_IR(value)               (1 << (value))


/***IM0 Interrupt Output Status Register***/
#define DANUBE_ICU_IM0_IOSR                    ((volatile u32*)(DANUBE_ICU+ 0x0010))
#define DANUBE_ICU_IM0_IOSR_IR(value)               (1 << (value))


/***IM1 Interrupt Output Status Register***/
#define DANUBE_ICU_IM1_IOSR                    ((volatile u32*)(DANUBE_ICU+ 0x0030))
#define DANUBE_ICU_IM1_IOSR_IR(value)               (1 << (value))


/***IM2 Interrupt Output Status Register***/
#define DANUBE_ICU_IM2_IOSR                    ((volatile u32*)(DANUBE_ICU+ 0x0050))
#define DANUBE_ICU_IM2_IOSR_IR(value)               (1 << (value))

/***IM3 Interrupt Output Status Register***/
#define DANUBE_ICU_IM3_IOSR                    ((volatile u32*)(DANUBE_ICU+ 0x0070))
#define DANUBE_ICU_IM3_IOSR_IR(value)               (1 << (value))

/***IM4 Interrupt Output Status Register***/
#define DANUBE_ICU_IM4_IOSR                    ((volatile u32*)(DANUBE_ICU+ 0x0090))
#define DANUBE_ICU_IM4_IOSR_IR(value)               (1 << (value))


/***IM0 Interrupt Request Set Register***/
#define DANUBE_ICU_IM0_IRSR                    ((volatile u32*)(DANUBE_ICU+ 0x0018))
#define DANUBE_ICU_IM0_IRSR_IR(value)               (1 << (value))


/***IM1 Interrupt Request Set Register***/
#define DANUBE_ICU_IM1_IRSR                    ((volatile u32*)(DANUBE_ICU+ 0x0038))
#define DANUBE_ICU_IM1_IRSR_IR(value)               (1 << (value))


/***IM2 Interrupt Request Set Register***/
#define DANUBE_ICU_IM2_IRSR                    ((volatile u32*)(DANUBE_ICU+ 0x0058))
#define DANUBE_ICU_IM2_IRSR_IR(value)               (1 << (value))

/***IM3 Interrupt Request Set Register***/
#define DANUBE_ICU_IM3_IRSR                    ((volatile u32*)(DANUBE_ICU+ 0x0078))
#define DANUBE_ICU_IM3_IRSR_IR(value)               (1 << (value))

/***IM4 Interrupt Request Set Register***/
#define DANUBE_ICU_IM4_IRSR                    ((volatile u32*)(DANUBE_ICU+ 0x0098))
#define DANUBE_ICU_IM4_IRSR_IR(value)               (1 << (value))

/***Interrupt Vector Value Register***/
#define DANUBE_ICU_IM_VEC                      ((volatile u32*)(DANUBE_ICU+ 0x0060))

/***Interrupt Vector Value Mask***/
#define DANUBE_ICU_IM0_VEC_MASK                0x0000001f
#define DANUBE_ICU_IM1_VEC_MASK                0x000003e0
#define DANUBE_ICU_IM2_VEC_MASK                0x00007c00
#define DANUBE_ICU_IM3_VEC_MASK                0x000f8000
#define DANUBE_ICU_IM4_VEC_MASK                0x01f00000

/***DMA Interrupt Mask Value***/
#define DANUBE_DMA_H_MASK			0x00000fff

/***External Interrupt Control Register***/
#define DANUBE_ICU_EXTINTCR                ((volatile u32*)(DANUBE_ICU_EXI+ 0x0000))
#define DANUBE_ICU_IRNICR                  ((volatile u32*)(DANUBE_ICU_EXI+ 0x0004))
#define DANUBE_ICU_IRNCR                   ((volatile u32*)(DANUBE_ICU_EXI+ 0x0008))
#define DANUBE_ICU_IRNEN                   ((volatile u32*)(DANUBE_ICU_EXI+ 0x000c))
#define DANUBE_ICU_NMI_CR                   ((volatile u32*)(DANUBE_ICU_EXI+ 0x00f0))
#define DANUBE_ICU_NMI_SR                   ((volatile u32*)(DANUBE_ICU_EXI+ 0x00f4))

/***********************************************************************/
/*  Module      :  MPS register address and bits                       */
/***********************************************************************/

#define DANUBE_MPS                          (KSEG1+0x1F107000)
/***********************************************************************/

#define DANUBE_MPS_CHIPID                       ((volatile u32*)(DANUBE_MPS + 0x0344))
#define DANUBE_MPS_CHIPID_VERSION_GET(value)    (((value) >> 28) & ((1 << 4) - 1))
#define DANUBE_MPS_CHIPID_VERSION_SET(value)    (((( 1 << 4) - 1) & (value)) << 28)
#define DANUBE_MPS_CHIPID_PARTNUM_GET(value)    (((value) >> 12) & ((1 << 16) - 1))
#define DANUBE_MPS_CHIPID_PARTNUM_SET(value)    (((( 1 << 16) - 1) & (value)) << 12)
#define DANUBE_MPS_CHIPID_MANID_GET(value)      (((value) >> 1) & ((1 << 10) - 1))
#define DANUBE_MPS_CHIPID_MANID_SET(value)      (((( 1 << 10) - 1) & (value)) << 1)


/* voice channel 0 ... 3 interrupt enable register */
#define DANUBE_MPS_VC0ENR ((volatile u32*)(DANUBE_MPS + 0x0000))
#define DANUBE_MPS_VC1ENR ((volatile u32*)(DANUBE_MPS + 0x0004))
#define DANUBE_MPS_VC2ENR ((volatile u32*)(DANUBE_MPS + 0x0008))
#define DANUBE_MPS_VC3ENR ((volatile u32*)(DANUBE_MPS + 0x000C))
/* voice channel 0 ... 3 interrupt status read register */
#define DANUBE_MPS_RVC0SR ((volatile u32*)(DANUBE_MPS + 0x0010))
#define DANUBE_MPS_RVC1SR ((volatile u32*)(DANUBE_MPS + 0x0014))
#define DANUBE_MPS_RVC2SR ((volatile u32*)(DANUBE_MPS + 0x0018))
#define DANUBE_MPS_RVC3SR ((volatile u32*)(DANUBE_MPS + 0x001C))
/* voice channel 0 ... 3 interrupt status set register */
#define DANUBE_MPS_SVC0SR ((volatile u32*)(DANUBE_MPS + 0x0020))
#define DANUBE_MPS_SVC1SR ((volatile u32*)(DANUBE_MPS + 0x0024))
#define DANUBE_MPS_SVC2SR ((volatile u32*)(DANUBE_MPS + 0x0028))
#define DANUBE_MPS_SVC3SR ((volatile u32*)(DANUBE_MPS + 0x002C))
/* voice channel 0 ... 3 interrupt status clear register */
#define DANUBE_MPS_CVC0SR ((volatile u32*)(DANUBE_MPS + 0x0030))
#define DANUBE_MPS_CVC1SR ((volatile u32*)(DANUBE_MPS + 0x0034))
#define DANUBE_MPS_CVC2SR ((volatile u32*)(DANUBE_MPS + 0x0038))
#define DANUBE_MPS_CVC3SR ((volatile u32*)(DANUBE_MPS + 0x003C))
/* common status 0 and 1 read register */
#define DANUBE_MPS_RAD0SR ((volatile u32*)(DANUBE_MPS + 0x0040))
#define DANUBE_MPS_RAD1SR ((volatile u32*)(DANUBE_MPS + 0x0044))
/* common status 0 and 1 set register */
#define DANUBE_MPS_SAD0SR ((volatile u32*)(DANUBE_MPS + 0x0048))
#define DANUBE_MPS_SAD1SR ((volatile u32*)(DANUBE_MPS + 0x004C))
/* common status 0 and 1 clear register */
#define DANUBE_MPS_CAD0SR ((volatile u32*)(DANUBE_MPS + 0x0050))
#define DANUBE_MPS_CAD1SR ((volatile u32*)(DANUBE_MPS + 0x0054))
/* common status 0 and 1 enable register */
#define DANUBE_MPS_AD0ENR ((volatile u32*)(DANUBE_MPS + 0x0058))
#define DANUBE_MPS_AD1ENR ((volatile u32*)(DANUBE_MPS + 0x005C))
/* notification enable register */
#define DANUBE_MPS_CPU0_NFER ((volatile u32*)(DANUBE_MPS + 0x0060))
#define DANUBE_MPS_CPU1_NFER ((volatile u32*)(DANUBE_MPS + 0x0064))
/* CPU to CPU interrup request register */
#define DANUBE_MPS_CPU0_2_CPU1_IRR ((volatile u32*)(DANUBE_MPS + 0x0070))
#define DANUBE_MPS_CPU0_2_CPU1_IER ((volatile u32*)(DANUBE_MPS + 0x0074))
/* Global interrupt request and request enable register */
#define DANUBE_MPS_GIRR ((volatile u32*)(DANUBE_MPS + 0x0078))
#define DANUBE_MPS_GIER ((volatile u32*)(DANUBE_MPS + 0x007C))


#define DANUBE_MPS_CPU0_SMP0 ((volatile u32*)(DANUBE_MPS + 0x00100))

#define DANUBE_MPS_CPU1_SMP0 ((volatile u32*)(DANUBE_MPS + 0x00200))

/************************************************************************/
/*   Module       :   DEU register address and bits        		*/
/************************************************************************/
#define DANUBE_DEU_BASE_ADDR               (0xBE102000)
/*   DEU Control Register */
#define DANUBE_DEU_CLK                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0000))
#define DANUBE_DEU_ID                      ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0008))

/*   DEU control register */
#define DANUBE_DEU_CON                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0010))
#define DANUBE_DEU_IHR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0014))
#define DANUBE_DEU_ILR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0018))
#define DANUBE_DEU_K1HR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x001C))
#define DANUBE_DEU_K1LR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0020))
#define DANUBE_DEU_K3HR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0024))
#define DANUBE_DEU_K3LR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0028))
#define DANUBE_DEU_IVHR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x002C))
#define DANUBE_DEU_IVLR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0030))
#define DANUBE_DEU_OHR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0040))
#define DANUBE_DEU_OLR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0050))

/* AES DEU register */
#define DANUBE_AES_CON 			   ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0050))
#define DANUBE_AES_ID3R                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0054))
#define DANUBE_AES_ID2R                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0058))
#define DANUBE_AES_ID1R                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x005C))
#define DANUBE_AES_ID0R                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0060))

/* AES Key register */
#define DANUBE_AES_K7R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0064))
#define DANUBE_AES_K6R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0068))
#define DANUBE_AES_K5R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x006C))
#define DANUBE_AES_K4R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0070))
#define DANUBE_AES_K3R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0074))
#define DANUBE_AES_K2R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0078))
#define DANUBE_AES_K1R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x007C))
#define DANUBE_AES_K0R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0080))

/* AES vector register */
#define DANUBE_AES_IV3R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0084))
#define DANUBE_AES_IV2R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0088))
#define DANUBE_AES_IV1R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x008C))
#define DANUBE_AES_IV0R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0090))
#define DANUBE_AES_0D3R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0094))
#define DANUBE_AES_0D2R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0098))
#define DANUBE_AES_OD1R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x009C))
#define DANUBE_AES_OD0R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00A0))

/* hash control registe */
#define DANUBE_HASH_CON                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00B0))
#define DANUBE_HASH_MR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00B4))
#define DANUBE_HASH_D1R                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00B8 ))
#define DANUBE_HASH_D2R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00BC ))
#define DANUBE_HASH_D3R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00C0 ))
#define DANUBE_HASH_D4R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00C4))
#define DANUBE_HASH_D5R                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00C8))

#define DANUBE_CON                         ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00EC))




/************************************************************************/
/*   Module       :   PPE register address and bits        		*/
/************************************************************************/
#define DANUBE_PPE_BASE_ADDR                (KSEG1 + 0x1E180000)
#define DANUBE_PPE_PP32_DEBUG_REG_ADDR(x)          ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x0000) << 2)))
#define DANUBE_PPE_PPM_INT_REG_ADDR(x)             ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x0030) << 2)))
#define DANUBE_PPE_PP32_INTERNAL_RES_ADDR(x)       ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x0040) << 2)))
#define DANUBE_PPE_PPE_CLOCK_CONTROL_ADDR(x)       ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x0100) << 2)))
#define DANUBE_PPE_CDM_CODE_MEMORY_RAM0_ADDR(x)    ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x1000) << 2)))
#define DANUBE_PPE_CDM_CODE_MEMORY_RAM1_ADDR(x)    ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x2000) << 2)))
#define DANUBE_PPE_REG_ADDR(x)                     ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x4000) << 2)))
#define DANUBE_PPE_PP32_DATA_MEMORY_RAM1_ADDR(x)   ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x5000) << 2)))
#define DANUBE_PPE_PPM_INT_UNIT_ADDR(x)            ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x6000) << 2)))
#define DANUBE_PPE_PPM_TIMER0_ADDR(x)              ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x6100) << 2)))
#define DANUBE_PPE_PPM_TASK_IND_REG_ADDR(x)        ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x6200) << 2)))
#define DANUBE_PPE_PPS_BRK_ADDR(x)                 ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x6300) << 2)))
#define DANUBE_PPE_PPM_TIMER1_ADDR(x)              ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x6400) << 2)))
#define DANUBE_PPE_SB_RAM0_ADDR(x)                 ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x8000) << 2)))
#define DANUBE_PPE_SB_RAM1_ADDR(x)                 ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x8400) << 2)))
#define DANUBE_PPE_SB_RAM2_ADDR(x)                 ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x8C00) << 2)))
#define DANUBE_PPE_SB_RAM3_ADDR(x)                 ((volatile u32*)(DANUBE_PPE_BASE_ADDR + (((x) + 0x9600) << 2)))

#define DANUBE_PPE_PP32_SLEEP                      DANUBE_PPE_REG_ADDR(0x0010) /* PP32 Power Saving Register */
#define DANUBE_PPE_CDM_CFG                         DANUBE_PPE_REG_ADDR(0x0100) /* Code/Data Memory (CDM) Register */

/* Mailbox Registers */
#define DANUBE_PPE_MBOX_IGU0_ISRS                  DANUBE_PPE_REG_ADDR(0x0200)
#define DANUBE_PPE_MBOX_IGU0_ISRC                  DANUBE_PPE_REG_ADDR(0x0201)
#define DANUBE_PPE_MBOX_IGU0_ISR                   DANUBE_PPE_REG_ADDR(0x0202)
#define DANUBE_PPE_MBOX_IGU0_IER                   DANUBE_PPE_REG_ADDR(0x0203)
#define DANUBE_PPE_MBOX_IGU1_ISRS0                 DANUBE_PPE_REG_ADDR(0x0204)
#define DANUBE_PPE_MBOX_IGU1_ISRC0                 DANUBE_PPE_REG_ADDR(0x0205)
#define DANUBE_PPE_MBOX_IGU1_ISR0                  DANUBE_PPE_REG_ADDR(0x0206)
#define DANUBE_PPE_MBOX_IGU1_IER0                  DANUBE_PPE_REG_ADDR(0x0207)
#define DANUBE_PPE_MBOX_IGU1_ISRS1                 DANUBE_PPE_REG_ADDR(0x0208)
#define DANUBE_PPE_MBOX_IGU1_ISRC1                 DANUBE_PPE_REG_ADDR(0x0209)
#define DANUBE_PPE_MBOX_IGU1_ISR1                  DANUBE_PPE_REG_ADDR(0x020A)
#define DANUBE_PPE_MBOX_IGU1_IER1                  DANUBE_PPE_REG_ADDR(0x020B)
#define DANUBE_PPE_MBOX_IGU1_ISRS2                 DANUBE_PPE_REG_ADDR(0x020C)
#define DANUBE_PPE_MBOX_IGU1_ISRC2                 DANUBE_PPE_REG_ADDR(0x020D)
#define DANUBE_PPE_MBOX_IGU1_ISR2                  DANUBE_PPE_REG_ADDR(0x020E)
#define DANUBE_PPE_MBOX_IGU1_IER2                  DANUBE_PPE_REG_ADDR(0x020F)
#define DANUBE_PPE_MBOX_IGU2_ISRS                  DANUBE_PPE_REG_ADDR(0x0210)
#define DANUBE_PPE_MBOX_IGU2_ISRC                  DANUBE_PPE_REG_ADDR(0x0211)
#define DANUBE_PPE_MBOX_IGU2_ISR                   DANUBE_PPE_REG_ADDR(0x0212)
#define DANUBE_PPE_MBOX_IGU2_IER                   DANUBE_PPE_REG_ADDR(0x0213)
#define DANUBE_PPE_MBOX_IGU3_ISRS                  DANUBE_PPE_REG_ADDR(0x0214)
#define DANUBE_PPE_MBOX_IGU3_ISRC                  DANUBE_PPE_REG_ADDR(0x0215)
#define DANUBE_PPE_MBOX_IGU3_ISR                   DANUBE_PPE_REG_ADDR(0x0216)
#define DANUBE_PPE_MBOX_IGU3_IER                   DANUBE_PPE_REG_ADDR(0x0217)
#define DANUBE_PPE_MBOX_IGU4_ISRS                  DANUBE_PPE_REG_ADDR(0x0218)
#define DANUBE_PPE_MBOX_IGU4_ISRC                  DANUBE_PPE_REG_ADDR(0x0219)
#define DANUBE_PPE_MBOX_IGU4_ISR                   DANUBE_PPE_REG_ADDR(0x021A)
#define DANUBE_PPE_MBOX_IGU4_IER                   DANUBE_PPE_REG_ADDR(0x021B)
/*
 *    Shared Buffer (SB) Registers
 */
#define DANUBE_PPE_SB_MST_PRI0                     DANUBE_PPE_REG_ADDR(0x0300)
#define DANUBE_PPE_SB_MST_PRI1                     DANUBE_PPE_REG_ADDR(0x0301)
#define DANUBE_PPE_SB_MST_PRI2                     DANUBE_PPE_REG_ADDR(0x0302)
#define DANUBE_PPE_SB_MST_PRI3                     DANUBE_PPE_REG_ADDR(0x0303)
#define DANUBE_PPE_SB_MST_PRI4                     DANUBE_PPE_REG_ADDR(0x0304)
#define DANUBE_PPE_SB_MST_SEL                      DANUBE_PPE_REG_ADDR(0x0305)
/*
 *    RTHA Registers
 */
#define DANUBE_PPE_RFBI_CFG                        DANUBE_PPE_REG_ADDR(0x0400)
#define DANUBE_PPE_RBA_CFG0                        DANUBE_PPE_REG_ADDR(0x0404)
#define DANUBE_PPE_RBA_CFG1                        DANUBE_PPE_REG_ADDR(0x0405)
#define DANUBE_PPE_RCA_CFG0                        DANUBE_PPE_REG_ADDR(0x0408)
#define DANUBE_PPE_RCA_CFG1                        DANUBE_PPE_REG_ADDR(0x0409)
#define DANUBE_PPE_RDES_CFG0                       DANUBE_PPE_REG_ADDR(0x040C)
#define DANUBE_PPE_RDES_CFG1                       DANUBE_PPE_REG_ADDR(0x040D)
#define DANUBE_PPE_SFSM_STATE0                     DANUBE_PPE_REG_ADDR(0x0410)
#define DANUBE_PPE_SFSM_STATE1                     DANUBE_PPE_REG_ADDR(0x0411)
#define DANUBE_PPE_SFSM_DBA0                       DANUBE_PPE_REG_ADDR(0x0412)
#define DANUBE_PPE_SFSM_DBA1                       DANUBE_PPE_REG_ADDR(0x0413)
#define DANUBE_PPE_SFSM_CBA0                       DANUBE_PPE_REG_ADDR(0x0414)
#define DANUBE_PPE_SFSM_CBA1                       DANUBE_PPE_REG_ADDR(0x0415)
#define DANUBE_PPE_SFSM_CFG0                       DANUBE_PPE_REG_ADDR(0x0416)
#define DANUBE_PPE_SFSM_CFG1                       DANUBE_PPE_REG_ADDR(0x0417)
#define DANUBE_PPE_SFSM_PGCNT0                     DANUBE_PPE_REG_ADDR(0x041C)
#define DANUBE_PPE_SFSM_PGCNT1                     DANUBE_PPE_REG_ADDR(0x041D)
/*
 *    TTHA Registers
 */
#define DANUBE_PPE_FFSM_DBA0                       DANUBE_PPE_REG_ADDR(0x0508)
#define DANUBE_PPE_FFSM_DBA1                       DANUBE_PPE_REG_ADDR(0x0509)
#define DANUBE_PPE_FFSM_CFG0                       DANUBE_PPE_REG_ADDR(0x050A)
#define DANUBE_PPE_FFSM_CFG1                       DANUBE_PPE_REG_ADDR(0x050B)
#define DANUBE_PPE_FFSM_IDLE_HEAD_BC0              DANUBE_PPE_REG_ADDR(0x050E)
#define DANUBE_PPE_FFSM_IDLE_HEAD_BC1              DANUBE_PPE_REG_ADDR(0x050F)
#define DANUBE_PPE_FFSM_PGCNT0                     DANUBE_PPE_REG_ADDR(0x0514)
#define DANUBE_PPE_FFSM_PGCNT1                     DANUBE_PPE_REG_ADDR(0x0515)
/*
 *    ETOP MDIO Registers
 */
#define DANUBE_PPE_ETOP_MDIO_CFG                   DANUBE_PPE_REG_ADDR(0x0600)
#define DANUBE_PPE_ETOP_MDIO_ACC                   DANUBE_PPE_REG_ADDR(0x0601)
#define DANUBE_PPE_ETOP_CFG                        DANUBE_PPE_REG_ADDR(0x0602)
#define DANUBE_PPE_ETOP_IG_VLAN_COS                DANUBE_PPE_REG_ADDR(0x0603)
#define DANUBE_PPE_ETOP_IG_DSCP_COS3               DANUBE_PPE_REG_ADDR(0x0604)
#define DANUBE_PPE_ETOP_IG_DSCP_COS2               DANUBE_PPE_REG_ADDR(0x0605)
#define DANUBE_PPE_ETOP_IG_DSCP_COS1               DANUBE_PPE_REG_ADDR(0x0606)
#define DANUBE_PPE_ETOP_IG_DSCP_COS0               DANUBE_PPE_REG_ADDR(0x0607)
#define DANUBE_PPE_ETOP_IG_PLEN_CTRL0              DANUBE_PPE_REG_ADDR(0x0608)
#define DANUBE_PPE_ETOP_IG_PLEN_CTRL1              DANUBE_PPE_REG_ADDR(0x0609)
#define DANUBE_PPE_ETOP_ISR                        DANUBE_PPE_REG_ADDR(0x060A)
#define DANUBE_PPE_ETOP_IER                        DANUBE_PPE_REG_ADDR(0x060B)
#define DANUBE_PPE_ETOP_VPID                       DANUBE_PPE_REG_ADDR(0x060C)
#define DANUBE_PPE_ENET_MAC_CFG                    DANUBE_PPE_REG_ADDR(0x0610)
#define DANUBE_PPE_ENETS_DBA                       DANUBE_PPE_REG_ADDR(0x0612)
#define DANUBE_PPE_ENETS_CBA                       DANUBE_PPE_REG_ADDR(0x0613)
#define DANUBE_PPE_ENETS_CFG                       DANUBE_PPE_REG_ADDR(0x0614)
#define DANUBE_PPE_ENETS_PGCNT                     DANUBE_PPE_REG_ADDR(0x0615)
#define DANUBE_PPE_ENETS_PGCNT_DSRC_PP32	   (0x00020000)
#define DANUBE_PPE_ENETS_PGCNT_DVAL_SHIFT	   (9)
#define DANUBE_PPE_ENETS_PGCNT_DCMD	           (0x00000100)
#define DANUBE_PPE_ENETS_PKTCNT                    DANUBE_PPE_REG_ADDR(0x0616)
#define DANUBE_PPE_ENETS_PKTCNT_DSRC_PP32	   (0x00000200)
#define DANUBE_PPE_ENETS_PKTCNT_DCMD	           (0x00000100)
#define DANUBE_PPE_ENETS_PKTCNT_UPKT	           (0x000000FF)
#define DANUBE_PPE_ENETS_BUF_CTRL                  DANUBE_PPE_REG_ADDR(0x0617)
#define DANUBE_PPE_ENETS_COS_CFG                   DANUBE_PPE_REG_ADDR(0x0618)
#define DANUBE_PPE_ENETS_IGDROP                    DANUBE_PPE_REG_ADDR(0x0619)
#define DANUBE_PPE_ENETF_DBA                       DANUBE_PPE_REG_ADDR(0x0630)
#define DANUBE_PPE_ENETF_CBA                       DANUBE_PPE_REG_ADDR(0x0631)
#define DANUBE_PPE_ENETF_CFG                       DANUBE_PPE_REG_ADDR(0x0632)
#define DANUBE_PPE_ENETF_PGCNT                     DANUBE_PPE_REG_ADDR(0x0633)
#define DANUBE_PPE_ENETF_PGCNT_ISRC_PP32	   (0x00020000)
#define DANUBE_PPE_ENETF_PGCNT_IVAL_SHIFT	   (9)
#define DANUBE_PPE_ENETF_PGCNT_ICMD	           (0x00000100)
#define DANUBE_PPE_ENETF_PKTCNT                    DANUBE_PPE_REG_ADDR(0x0634)
#define DANUBE_PPE_ENETF_PKTCNT_ISRC_PP32	   (0x00000200)
#define DANUBE_PPE_ENETF_PKTCNT_ICMD	           (0x00000100)
#define DANUBE_PPE_ENETF_PKTCNT_VPKT	           (0x000000FF)
#define DANUBE_PPE_ENETF_HFCTRL                    DANUBE_PPE_REG_ADDR(0x0635)
#define DANUBE_PPE_ENETF_TXCTRL                    DANUBE_PPE_REG_ADDR(0x0636)
#define DANUBE_PPE_ENETF_VLCOS0                    DANUBE_PPE_REG_ADDR(0x0638)
#define DANUBE_PPE_ENETF_VLCOS1                    DANUBE_PPE_REG_ADDR(0x0639)
#define DANUBE_PPE_ENETF_VLCOS2                    DANUBE_PPE_REG_ADDR(0x063A)
#define DANUBE_PPE_ENETF_VLCOS3                    DANUBE_PPE_REG_ADDR(0x063B)
#define DANUBE_PPE_ENETF_EGERR                     DANUBE_PPE_REG_ADDR(0x063C)
#define DANUBE_PPE_ENETF_EGDROP                    DANUBE_PPE_REG_ADDR(0x063D)
/*
 *    DPLUS Registers
 */
#define DANUBE_PPE_DPLUS_TXDB                      DANUBE_PPE_REG_ADDR(0x0700)
#define DANUBE_PPE_DPLUS_TXCB                      DANUBE_PPE_REG_ADDR(0x0701)
#define DANUBE_PPE_DPLUS_TXCFG                     DANUBE_PPE_REG_ADDR(0x0702)
#define DANUBE_PPE_DPLUS_TXPGCNT                   DANUBE_PPE_REG_ADDR(0x0703)
#define DANUBE_PPE_DPLUS_RXDB                      DANUBE_PPE_REG_ADDR(0x0710)
#define DANUBE_PPE_DPLUS_RXCB                      DANUBE_PPE_REG_ADDR(0x0711)
#define DANUBE_PPE_DPLUS_RXCFG                     DANUBE_PPE_REG_ADDR(0x0712)
#define DANUBE_PPE_DPLUS_RXPGCNT                   DANUBE_PPE_REG_ADDR(0x0713)
/*
 *    BMC Registers
 */
#define DANUBE_PPE_BMC_CMD3                        DANUBE_PPE_REG_ADDR(0x0800)
#define DANUBE_PPE_BMC_CMD2                        DANUBE_PPE_REG_ADDR(0x0801)
#define DANUBE_PPE_BMC_CMD1                        DANUBE_PPE_REG_ADDR(0x0802)
#define DANUBE_PPE_BMC_CMD0                        DANUBE_PPE_REG_ADDR(0x0803)
#define DANUBE_PPE_BMC_CFG0                        DANUBE_PPE_REG_ADDR(0x0804)
#define DANUBE_PPE_BMC_CFG1                        DANUBE_PPE_REG_ADDR(0x0805)
#define DANUBE_PPE_BMC_POLY0                       DANUBE_PPE_REG_ADDR(0x0806)
#define DANUBE_PPE_BMC_POLY1                       DANUBE_PPE_REG_ADDR(0x0807)
#define DANUBE_PPE_BMC_CRC0                        DANUBE_PPE_REG_ADDR(0x0808)
#define DANUBE_PPE_BMC_CRC1                        DANUBE_PPE_REG_ADDR(0x0809)
/*
 *    SLL Registers
 */
#define DANUBE_PPE_SLL_CMD1                        DANUBE_PPE_REG_ADDR(0x0900)
#define DANUBE_PPE_SLL_CMD0                        DANUBE_PPE_REG_ADDR(0x0901)
#define DANUBE_PPE_SLL_KEY0                        DANUBE_PPE_REG_ADDR(0x0910)
#define DANUBE_PPE_SLL_KEY1                        DANUBE_PPE_REG_ADDR(0x0911)
#define DANUBE_PPE_SLL_KEY2                        DANUBE_PPE_REG_ADDR(0x0912)
#define DANUBE_PPE_SLL_KEY3                        DANUBE_PPE_REG_ADDR(0x0913)
#define DANUBE_PPE_SLL_KEY4                        DANUBE_PPE_REG_ADDR(0x0914)
#define DANUBE_PPE_SLL_KEY5                        DANUBE_PPE_REG_ADDR(0x0915)
#define DANUBE_PPE_SLL_RESULT                      DANUBE_PPE_REG_ADDR(0x0920)
/*
 *    EMA Registers
 */
#define DANUBE_PPE_EMA_CMD2                        DANUBE_PPE_REG_ADDR(0x0A00)
#define DANUBE_PPE_EMA_CMD1                        DANUBE_PPE_REG_ADDR(0x0A01)
#define DANUBE_PPE_EMA_CMD0                        DANUBE_PPE_REG_ADDR(0x0A02)
#define DANUBE_PPE_EMA_ISR                         DANUBE_PPE_REG_ADDR(0x0A04)
#define DANUBE_PPE_EMA_IER                         DANUBE_PPE_REG_ADDR(0x0A05)
#define DANUBE_PPE_EMA_CFG                         DANUBE_PPE_REG_ADDR(0x0A06)
/*
 *    UTPS Registers
 */
#define DANUBE_PPE_UTP_TXCA0                       DANUBE_PPE_REG_ADDR(0x0B00)
#define DANUBE_PPE_UTP_TXNA0                       DANUBE_PPE_REG_ADDR(0x0B01)
#define DANUBE_PPE_UTP_TXCA1                       DANUBE_PPE_REG_ADDR(0x0B02)
#define DANUBE_PPE_UTP_TXNA1                       DANUBE_PPE_REG_ADDR(0x0B03)
#define DANUBE_PPE_UTP_RXCA0                       DANUBE_PPE_REG_ADDR(0x0B10)
#define DANUBE_PPE_UTP_RXNA0                       DANUBE_PPE_REG_ADDR(0x0B11)
#define DANUBE_PPE_UTP_RXCA1                       DANUBE_PPE_REG_ADDR(0x0B12)
#define DANUBE_PPE_UTP_RXNA1                       DANUBE_PPE_REG_ADDR(0x0B13)
#define DANUBE_PPE_UTP_CFG                         DANUBE_PPE_REG_ADDR(0x0B20)
#define DANUBE_PPE_UTP_ISR                         DANUBE_PPE_REG_ADDR(0x0B30)
#define DANUBE_PPE_UTP_IER                         DANUBE_PPE_REG_ADDR(0x0B31)
/*
 *    QSB Registers
 */
#define DANUBE_PPE_QSB_RELOG                       DANUBE_PPE_REG_ADDR(0x0C00)
#define DANUBE_PPE_QSB_EMIT0                       DANUBE_PPE_REG_ADDR(0x0C01)
#define DANUBE_PPE_QSB_EMIT1                       DANUBE_PPE_REG_ADDR(0x0C02)
#define DANUBE_PPE_QSB_ICDV                        DANUBE_PPE_REG_ADDR(0x0C07)
#define DANUBE_PPE_QSB_SBL                         DANUBE_PPE_REG_ADDR(0x0C09)
#define DANUBE_PPE_QSB_CFG                         DANUBE_PPE_REG_ADDR(0x0C0A)
#define DANUBE_PPE_QSB_RTM                         DANUBE_PPE_REG_ADDR(0x0C0B)
#define DANUBE_PPE_QSB_RTD                         DANUBE_PPE_REG_ADDR(0x0C0C)
#define DANUBE_PPE_QSB_RAMAC                       DANUBE_PPE_REG_ADDR(0x0C0D)
#define DANUBE_PPE_QSB_ISTAT                       DANUBE_PPE_REG_ADDR(0x0C0E)
#define DANUBE_PPE_QSB_IMR                         DANUBE_PPE_REG_ADDR(0x0C0F)
#define DANUBE_PPE_QSB_SRC                         DANUBE_PPE_REG_ADDR(0x0C10)
/*
 *    DSP User Registers
 */
#define DANUBE_PPE_DREG_A_VERSION                  DANUBE_PPE_REG_ADDR(0x0D00)
#define DANUBE_PPE_DREG_A_CFG                      DANUBE_PPE_REG_ADDR(0x0D01)
#define DANUBE_PPE_DREG_AT_CTRL                    DANUBE_PPE_REG_ADDR(0x0D02)
#define DANUBE_PPE_DREG_AR_CTRL                    DANUBE_PPE_REG_ADDR(0x0D08)
#define DANUBE_PPE_DREG_A_UTPCFG                   DANUBE_PPE_REG_ADDR(0x0D0E)
#define DANUBE_PPE_DREG_A_STATUS                   DANUBE_PPE_REG_ADDR(0x0D0F)
#define DANUBE_PPE_DREG_AT_CFG0                    DANUBE_PPE_REG_ADDR(0x0D20)
#define DANUBE_PPE_DREG_AT_CFG1                    DANUBE_PPE_REG_ADDR(0x0D21)
#define DANUBE_PPE_DREG_FB_SIZE0                   DANUBE_PPE_REG_ADDR(0x0D22)
#define DANUBE_PPE_DREG_FB_SIZE1                   DANUBE_PPE_REG_ADDR(0x0D23)
#define DANUBE_PPE_DREG_AT_CELL0                   DANUBE_PPE_REG_ADDR(0x0D24)
#define DANUBE_PPE_DREG_AT_CELL1                   DANUBE_PPE_REG_ADDR(0x0D25)
#define DANUBE_PPE_DREG_AT_IDLE_CNT0               DANUBE_PPE_REG_ADDR(0x0D26)
#define DANUBE_PPE_DREG_AT_IDLE_CNT1               DANUBE_PPE_REG_ADDR(0x0D27)
#define DANUBE_PPE_DREG_AT_IDLE0                   DANUBE_PPE_REG_ADDR(0x0D28)
#define DANUBE_PPE_DREG_AT_IDLE1                   DANUBE_PPE_REG_ADDR(0x0D29)
#define DANUBE_PPE_DREG_AR_CFG0                    DANUBE_PPE_REG_ADDR(0x0D60)
#define DANUBE_PPE_DREG_AR_CFG1                    DANUBE_PPE_REG_ADDR(0x0D61)
#define DANUBE_PPE_DREG_AR_FB_START0               DANUBE_PPE_REG_ADDR(0x0D62)
#define DANUBE_PPE_DREG_AR_FB_START1               DANUBE_PPE_REG_ADDR(0x0D63)
#define DANUBE_PPE_DREG_AR_FB_END0                 DANUBE_PPE_REG_ADDR(0x0D64)
#define DANUBE_PPE_DREG_AR_FB_END1                 DANUBE_PPE_REG_ADDR(0x0D65)
#define DANUBE_PPE_DREG_AR_ATM_STAT0               DANUBE_PPE_REG_ADDR(0x0D66)
#define DANUBE_PPE_DREG_AR_ATM_STAT1               DANUBE_PPE_REG_ADDR(0x0D67)
#define DANUBE_PPE_DREG_AR_CELL0                   DANUBE_PPE_REG_ADDR(0x0D68)
#define DANUBE_PPE_DREG_AR_CELL1                   DANUBE_PPE_REG_ADDR(0x0D69)
#define DANUBE_PPE_DREG_AR_IDLE_CNT0               DANUBE_PPE_REG_ADDR(0x0D6A)
#define DANUBE_PPE_DREG_AR_IDLE_CNT1               DANUBE_PPE_REG_ADDR(0x0D6B)
#define DANUBE_PPE_DREG_AR_AIIDLE_CNT0             DANUBE_PPE_REG_ADDR(0x0D6C)
#define DANUBE_PPE_DREG_AR_AIIDLE_CNT1             DANUBE_PPE_REG_ADDR(0x0D6D)
#define DANUBE_PPE_DREG_AR_BE_CNT0                 DANUBE_PPE_REG_ADDR(0x0D6E)
#define DANUBE_PPE_DREG_AR_BE_CNT1                 DANUBE_PPE_REG_ADDR(0x0D6F)
#define DANUBE_PPE_DREG_AR_HEC_CNT0                DANUBE_PPE_REG_ADDR(0x0D70)
#define DANUBE_PPE_DREG_AR_HEC_CNT1                DANUBE_PPE_REG_ADDR(0x0D71)
#define DANUBE_PPE_DREG_AR_CD_CNT0                 DANUBE_PPE_REG_ADDR(0x0D72)
#define DANUBE_PPE_DREG_AR_CD_CNT1                 DANUBE_PPE_REG_ADDR(0x0D73)
#define DANUBE_PPE_DREG_AR_IDLE0                   DANUBE_PPE_REG_ADDR(0x0D74)
#define DANUBE_PPE_DREG_AR_IDLE1                   DANUBE_PPE_REG_ADDR(0x0D75)
#define DANUBE_PPE_DREG_AR_DELIN0                  DANUBE_PPE_REG_ADDR(0x0D76)
#define DANUBE_PPE_DREG_AR_DELIN1                  DANUBE_PPE_REG_ADDR(0x0D77)
#define DANUBE_PPE_DREG_RESV0                      DANUBE_PPE_REG_ADDR(0x0D78)
#define DANUBE_PPE_DREG_RESV1                      DANUBE_PPE_REG_ADDR(0x0D79)
#define DANUBE_PPE_DREG_RX_MIB_CMD0                DANUBE_PPE_REG_ADDR(0x0D80)
#define DANUBE_PPE_DREG_RX_MIB_CMD1                DANUBE_PPE_REG_ADDR(0x0D81)
#define DANUBE_PPE_DREG_AR_OVDROP_CNT0             DANUBE_PPE_REG_ADDR(0x0D98)
#define DANUBE_PPE_DREG_AR_OVDROP_CNT1             DANUBE_PPE_REG_ADDR(0x0D99)


/************************************************************************/
/*   Module       :   PPE register address and bits        		*/
/************************************************************************/
#define DANUBE_PPE32_BASE  0xBE180000
#define DANUBE_PPE32_DEBUG_BREAK_TRACE_REG   (DANUBE_PPE32_BASE + (0x0000 * 4))
#define DANUBE_PPE32_INT_MASK_STATUS_REG     (DANUBE_PPE32_BASE + (0x0030 * 4))
#define DANUBE_PPE32_INT_RESOURCE_REG        (DANUBE_PPE32_BASE + (0x0040 * 4))
#define DANUBE_PPE32_CDM_CODE_MEM_B0         (DANUBE_PPE32_BASE + (0x1000 * 4))
#define DANUBE_PPE32_CDM_CODE_MEM_B1         (DANUBE_PPE32_BASE + (0x2000 * 4))
#define DANUBE_PPE32_DATA_MEM_MAP_REG_BASE   (DANUBE_PPE32_BASE + (0x4000 * 4))

/*
 *    ETOP MDIO Registers
 */
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
#define ENET_MAC_DA0           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x061B * 4)))
#define ENET_MAC_DA1           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x061C * 4)))

#define ENETF_DBA               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0630 * 4)))
#define ENETF_CBA               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0631 * 4)))
#define ENETF_CFG               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0632 * 4)))
#define ENETF_PGCNT             ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0633 * 4)))
#define ENETF_PKTCNT            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0634 * 4)))
#define ENETF_HFCTRL            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0635 * 4)))
#define ENETF_TXCTRL            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0636 * 4)))

#define ENETF_VLCOS0            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0638 * 4)))
#define ENETF_VLCOS1            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0639 * 4)))
#define ENETF_VLCOS2            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x063A * 4)))
#define ENETF_VLCOS3            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x063B * 4)))
#define ENETF_EGERR             ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x063C * 4)))
#define ENETF_EGDROP            ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x063D * 4)))


/*
 *    ETOP MDIO Registers
 */
#define DANUBE_PPE32_ETOP_MDIO_CFG           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0600 * 4)))
#define DANUBE_PPE32_ETOP_MDIO_ACC           ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0601 * 4)))
#define DANUBE_PPE32_ETOP_CFG                ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0602 * 4)))
#define DANUBE_PPE32_ETOP_IG_VLAN_COS        ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0603 * 4)))
#define DANUBE_PPE32_ETOP_IG_DSCP_COS3       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0604 * 4)))
#define DANUBE_PPE32_ETOP_IG_DSCP_COS2       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0605 * 4)))
#define DANUBE_PPE32_ETOP_IG_DSCP_COS1       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0606 * 4)))
#define DANUBE_PPE32_ETOP_IG_DSCP_COS0       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0607 * 4)))
#define DANUBE_PPE32_ETOP_IG_PLEN_CTRL       ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0608 * 4)))
#define DANUBE_PPE32_ETOP_ISR                ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x060A * 4)))
#define DANUBE_PPE32_ETOP_IER                ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x060B * 4)))
#define DANUBE_PPE32_ETOP_VPID               ((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x060C * 4)))


/* ENET Register */
#define DANUBE_PPE32_ENET_MAC_CFG            	((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0610 * 4)))
#define DANUBE_PPE32_ENET_IG_PKTDROP          	((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0619 * 4)))
#define DANUBE_PPE32_ENET_CoS_CFG          	((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0618 * 4)))

/*********LED register definition****************/

#define DANUBE_LED                      0xBE100BB0
#define DANUBE_LED_CON0                 ((volatile u32*)(DANUBE_LED + 0x0000))
#define DANUBE_LED_CON1                 ((volatile u32*)(DANUBE_LED + 0x0004))
#define DANUBE_LED_CPU0                 ((volatile u32*)(DANUBE_LED + 0x0008))
#define DANUBE_LED_CPU1                 ((volatile u32*)(DANUBE_LED + 0x000C))
#define DANUBE_LED_AR                   ((volatile u32*)(DANUBE_LED + 0x0010))




/***********************************************************************/
#define DANUBE_REG32(addr)		   *((volatile u32 *)(addr))
/***********************************************************************/
#endif //DANUBE_H
