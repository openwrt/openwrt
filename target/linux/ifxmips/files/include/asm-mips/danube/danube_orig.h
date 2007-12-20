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
/*  Module      :  WDT register address and bits                       */
/***********************************************************************/

#define BOARD_SYSTEM_TYPE       "DANUBE"

#define DANUBE_BIU_WDT                           (KSEG1+0x1F880000)

/***Watchdog Timer Control Register ***/
#define DANUBE_BIU_WDT_CR                   ((volatile u32*)(DANUBE_BIU_WDT + 0x03F0))
#define DANUBE_BIU_WDT_CR_GEN               (1 << 31)
#define DANUBE_BIU_WDT_CR_DSEN              (1 << 30)
#define DANUBE_BIU_WDT_CR_LPEN              (1 << 29)
#define DANUBE_BIU_WDT_CR_PWL_GET(value)    (((value) >> 26) & ((1 << 2) - 1))
#define DANUBE_BIU_WDT_CR_PWL_SET(value)    (((( 1 << 2) - 1) & (value)) << 26)
#define DANUBE_BIU_WDT_CR_CLKDIV_GET(value) (((value) >> 24) & ((1 << 2) - 1))
#define DANUBE_BIU_WDT_CR_CLKDIV_SET(value) (((( 1 << 2) - 1) & (value)) << 24)
#define DANUBE_BIU_WDT_CR_PW_GET(value)     (((value) >> 16) & ((1 << 8) - 1))
#define DANUBE_BIU_WDT_CR_PW_SET(value)     (((( 1 << 8) - 1) & (value)) << 16)
#define DANUBE_BIU_WDT_CR_RELOAD_GET(value) (((value) >> 0) & ((1 << 16) - 1))
#define DANUBE_BIU_WDT_CR_RELOAD_SET(value) (((( 1 << 16) - 1) & (value)) << 0)

/***Watchdog Timer Status Register***/
#define DANUBE_BIU_WDT_SR                   ((volatile u32*)(DANUBE_BIU_WDT + 0x03F8))
#define DANUBE_BIU_WDT_SR_EN                (1 << 31)
#define DANUBE_BIU_WDT_SR_AE                (1 << 30)
#define DANUBE_BIU_WDT_SR_PRW               (1 << 29)
#define DANUBE_BIU_WDT_SR_EXP               (1 << 28)
#define DANUBE_BIU_WDT_SR_PWD               (1 << 27)
#define DANUBE_BIU_WDT_SR_DS                (1 << 26)
#define DANUBE_BIU_WDT_SR_VALUE_GET(value)  (((value) >> 0) & ((1 << 16) - 1))
#define DANUBE_BIU_WDT_SR_VALUE_SET(value)  (((( 1 << 16) - 1) & (value)) << 0)

/***********************************************************************/
/*  Module      :  PMU register address and bits                       */
/***********************************************************************/

#define DANUBE_PMU 			        (KSEG1+0x1F102000)

/* PMU Power down Control Register */
#define DANUBE_PMU_PWDCR                        ((volatile u32*)(DANUBE_PMU+0x001C))
#define DANUBE_PMU_PWDCR_GPT					(1 << 12)
#define DANUBE_PMU_PWDCR_FPI					(1 << 14)


/* PMU Status Register */
#define DANUBE_PMU_SR                           ((volatile u32*)(DANUBE_PMU+0x0020))

#define DANUBE_PMU_DMA_SHIFT                    5
#define DANUBE_PMU_PPE_SHIFT                    13
#define DANUBE_PMU_SDIO_SHIFT                   16
#define DANUBE_PMU_ETOP_SHIFT                   22
#define DANUBE_PMU_ENET0_SHIFT                  24
#define DANUBE_PMU_ENET1_SHIFT                  25

/***********************************************************************/
/*  Module      :  RCU register address and bits                       */
/***********************************************************************/
#define DANUBE_RCU_BASE_ADDR 		(0xBF203000)

#define DANUBE_RCU_REQ 			(0x0010 + DANUBE_RCU_BASE_ADDR)	/* will remove this, pls use DANUBE_RCU_RST_REQ */

#define DANUBE_RCU_RST_REQ              ((volatile u32*)(DANUBE_RCU_BASE_ADDR + 0x0010))
#define DANUBE_RCU_RST_STAT             ((volatile u32*)(DANUBE_RCU_BASE_ADDR + 0x0014))
#define DANUBE_RST_ALL                  (0x40000000)

/***Reset Request Register***/
#define DANUBE_RCU_RST_REQ_CPU0                 (1 << 31)
#define DANUBE_RCU_RST_REQ_CPU1                 (1 << 3)
#define DANUBE_RCU_RST_REQ_CPUSUB               (1 << 29)
#define DANUBE_RCU_RST_REQ_HRST                 (1 << 28)
#define DANUBE_RCU_RST_REQ_WDT0                 (1 << 27)
#define DANUBE_RCU_RST_REQ_WDT1                 (1 << 26)
#define DANUBE_RCU_RST_REQ_CFG_GET(value)       (((value) >> 23) & ((1 << 3) - 1))
#define DANUBE_RCU_RST_REQ_CFG_SET(value)       (((( 1 << 3) - 1) & (value)) << 23)
#define DANUBE_RCU_RST_REQ_SWTBOOT              (1 << 22)
#define DANUBE_RCU_RST_REQ_DMA                  (1 << 21)
#define DANUBE_RCU_RST_REQ_ARC_JTAG              (1 << 20)
#define DANUBE_RCU_RST_REQ_ETHPHY0              (1 << 19)
#define DANUBE_RCU_RST_REQ_CPU0_BR              (1 << 18)

#define DANBUE_RCU_RST_REQ_AFE			(1 << 11)
#define DANBUE_RCU_RST_REQ_DFE			(1 << 7)

/* CPU0, CPU1, CPUSUB, HRST, WDT0, WDT1, DMA, ETHPHY1, ETHPHY0 */
#define DANUBE_RCU_RST_REQ_ALL                  DANUBE_RST_ALL

#define DANUBE_RCU_STAT		        (0x0014 + DANUBE_RCU_BASE_ADDR)
#define DANUBE_RCU_RST_SR	        ( (volatile u32 *)(DANUBE_RCU_STAT))	/* will remove this, pls use DANUBE_RCU_RST_STAT */

/*#define DANUBE_RCU_MON                  (0x0030 + DANUBE_RCU_BASE_ADDR) */

/***********************************************************************/
/*  Module      :  BCU  register address and bits                       */
/***********************************************************************/

#define DANUBE_BCU_BASE_ADDR 			(KSEG1+0x1E100000)

/***BCU Control Register (0010H)***/
#define DANUBE_BCU_CON                          ((volatile u32*)(0x0010 + DANUBE_BCU_BASE_ADDR))
#define DANUBE_BCU_BCU_CON_SPC (value)                (((( 1 << 8) - 1) & (value)) << 24)
#define DANUBE_BCU_BCU_CON_SPE                              (1 << 19)
#define DANUBE_BCU_BCU_CON_PSE                              (1 << 18)
#define DANUBE_BCU_BCU_CON_DBG                              (1 << 16)
#define DANUBE_BCU_BCU_CON_TOUT (value)               (((( 1 << 16) - 1) & (value)) << 0)

/***BCU Error Control Capture Register (0020H)***/
#define DANUBE_BCU_ECON                         ((volatile u32*)(0x0020 + DANUBE_BCU_BASE_ADDR))
#define DANUBE_BCU_BCU_ECON_TAG (value)                (((( 1 << 4) - 1) & (value)) << 24)
#define DANUBE_BCU_BCU_ECON_RDN                              (1 << 23)
#define DANUBE_BCU_BCU_ECON_WRN                              (1 << 22)
#define DANUBE_BCU_BCU_ECON_SVM                              (1 << 21)
#define DANUBE_BCU_BCU_ECON_ACK (value)                (((( 1 << 2) - 1) & (value)) << 19)
#define DANUBE_BCU_BCU_ECON_ABT                              (1 << 18)
#define DANUBE_BCU_BCU_ECON_RDY                              (1 << 17)
#define DANUBE_BCU_BCU_ECON_TOUT                             (1 << 16)
#define DANUBE_BCU_BCU_ECON_ERRCNT (value)             (((( 1 << 16) - 1) & (value)) << 0)
#define DANUBE_BCU_BCU_ECON_OPC (value)                (((( 1 << 4) - 1) & (value)) << 28)

/***BCU Error Address Capture Register (0024 H)***/
#define DANUBE_BCU_EADD 			((volatile u32*)(0x0024 + DANUBE_BCU_BASE_ADDR))

/***BCU Error Data Capture Register (0028H)***/
#define DANUBE_BCU_EDAT 			((volatile u32*)(0x0028 + DANUBE_BCU_BASE_ADDR))
#define DANUBE_BCU_IRNEN 			((volatile u32*)(0x00F4 + DANUBE_BCU_BASE_ADDR))
#define DANUBE_BCU_IRNICR 			((volatile u32*)(0x00F8 + DANUBE_BCU_BASE_ADDR))
#define DANUBE_BCU_IRNCR 			((volatile u32*)(0x00FC + DANUBE_BCU_BASE_ADDR))

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
/*  Module      :  MEI register address and bits                       */
/***********************************************************************/
#define MEI_SPACE_ACCESS	0xBE116000

/***	Register address offsets, relative to MEI_SPACE_ADDRESS ***/
#define MEI_DATA_XFR				((volatile u32*)(0x0000 + MEI_SPACE_ACCESS))
#define	MEI_VERSION				((volatile u32*)(0x0004 + MEI_SPACE_ACCESS))
#define	MEI_ARC_GP_STAT				((volatile u32*)(0x0008 + MEI_SPACE_ACCESS))
#define MEI_DATA_XFR_STAT			((volatile u32*)(0x000C + MEI_SPACE_ACCESS))
#define	MEI_XFR_ADDR				((volatile u32*)(0x0010 + MEI_SPACE_ACCESS))
#define MEI_MAX_WAIT				((volatile u32*)(0x0014 + MEI_SPACE_ACCESS))
#define	MEI_TO_ARC_INT				((volatile u32*)(0x0018 + MEI_SPACE_ACCESS))
#define	ARC_TO_MEI_INT				((volatile u32*)(0x001C + MEI_SPACE_ACCESS))
#define	ARC_TO_MEI_INT_MASK			((volatile u32*)(0x0020 + MEI_SPACE_ACCESS))
#define	MEI_DEBUG_WAD				((volatile u32*)(0x0024 + MEI_SPACE_ACCESS))
#define MEI_DEBUG_RAD				((volatile u32*)(0x0028 + MEI_SPACE_ACCESS))
#define	MEI_DEBUG_DATA				((volatile u32*)(0x002C + MEI_SPACE_ACCESS))
#define	MEI_DEBUG_DEC				((volatile u32*)(0x0030 + MEI_SPACE_ACCESS))
#define MEI_CONFIG				((volatile u32*)(0x0034 + MEI_SPACE_ACCESS))
#define	MEI_RST_CONTROL				((volatile u32*)(0x0038 + MEI_SPACE_ACCESS))
#define	MEI_DBG_MASTER				((volatile u32*)(0x003C + MEI_SPACE_ACCESS))
#define	MEI_CLK_CONTROL				((volatile u32*)(0x0040 + MEI_SPACE_ACCESS))
#define	MEI_BIST_CONTROL			((volatile u32*)(0x0044 + MEI_SPACE_ACCESS))
#define	MEI_BIST_STAT				((volatile u32*)(0x0048 + MEI_SPACE_ACCESS))
#define MEI_XDATA_BASE_SH			((volatile u32*)(0x004c + MEI_SPACE_ACCESS))
#define MEI_XDATA_BASE				((volatile u32*)(0x0050 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR_BASE			((volatile u32*)(0x0054 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR0				((volatile u32*)(0x0054 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR1				((volatile u32*)(0x0058 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR2				((volatile u32*)(0x005C + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR3				((volatile u32*)(0x0060 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR4				((volatile u32*)(0x0064 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR5				((volatile u32*)(0x0068 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR6				((volatile u32*)(0x006C + MEI_SPACE_ACCESS)))
#define MEI_XMEM_BAR7				((volatile u32*)(0x0070 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR8				((volatile u32*)(0x0074 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR9				((volatile u32*)(0x0078 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR10				((volatile u32*)(0x007C + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR11				((volatile u32*)(0x0080 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR12				((volatile u32*)(0x0084 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR13				((volatile u32*)(0x0088 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR14				((volatile u32*)(0x008C + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR15				((volatile u32*)(0x0090 + MEI_SPACE_ACCESS))
#define MEI_XMEM_BAR16				((volatile u32*)(0x0094 + MEI_SPACE_ACCESS))

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
#define CLOCK_60M                60000000
#define CLOCK_83M                83333333
#define CLOCK_111M               111111111
#define CLOCK_133M               133333333
#define CLOCK_167M               166666667
#define CLOCK_333M               333333333

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
#define DANUBE_EBU_ADDSEL3                  ((volatile u32*)(DANUBE_EBU+ 0x0028))
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

#define DANUBE_EBU_PCC_CON                  ((volatile u32*)(DANUBE_EBU+ 0x0090))
#define DANUBE_EBU_PCC_STAT                  ((volatile u32*)(DANUBE_EBU+ 0x0094))
#define DANUBE_EBU_PCC_ISTAT                  ((volatile u32*)(DANUBE_EBU+ 0x00A0))
#define DANUBE_EBU_PCC_IEN                  ((volatile u32*)(DANUBE_EBU+ 0x00A4))
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
/*  Module      :  GPTC register address and bits                       */
/***********************************************************************/

#define DANUBE_GPTC                   (KSEG1 + 0x1E100A00)
#define DANUBE_GPTC_CLC               ((volatile u32*) (DANUBE_GPTC + 0x0000))
#define DANUBE_GPTC_ID                ((volatile u32*) (DANUBE_GPTC + 0x0008))
#define DANUBE_GPTC_IRNEN             ((volatile u32*) (DANUBE_GPTC + 0x00F4))
#define DANUBE_GPTC_IRNICR    ((volatile u32*) (DANUBE_GPTC + 0x00F8)
#define DANUBE_GPTC_IRNCR             ((volatile u32*) (DANUBE_GPTC + 0x00FC))

#define DANUBE_GPTC_CON_1A    ((volatile u32*) (DANUBE_GPTC + 0x0010))
#define DANUBE_GPTC_RUN_1A    ((volatile u32*) (DANUBE_GPTC + 0x0018))
#define DANUBE_GPTC_RELOAD_1A ((volatile u32*) (DANUBE_GPTC + 0x0020))
#define DANUBE_GPTC_COUNT_1A  ((volatile u32*) (DANUBE_GPTC + 0x0028))

#define DANUBE_GPTC_CON_1B    ((volatile u32*) (DANUBE_GPTC + 0x0014))
#define DANUBE_GPTC_RUN_1B    ((volatile u32*) (DANUBE_GPTC + 0x001C))
#define DANUBE_GPTC_RELOAD_1B ((volatile u32*) (DANUBE_GPTC + 0x0024))
#define DANUBE_GPTC_COUNT_1B  ((volatile u32*) (DANUBE_GPTC + 0x002C))

#define DANUBE_GPTC_CON_2A    ((volatile u32*) (DANUBE_GPTC + 0x0030))
#define DANUBE_GPTC_RUN_2A    ((volatile u32*) (DANUBE_GPTC + 0x0038))
#define DANUBE_GPTC_RELOAD_2A ((volatile u32*) (DANUBE_GPTC + 0x0040))
#define DANUBE_GPTC_COUNT_2A  ((volatile u32*) (DANUBE_GPTC + 0x0048))

#define DANUBE_GPTC_CON_2B    ((volatile u32*) (DANUBE_GPTC + 0x0034))
#define DANUBE_GPTC_RUN_2B    ((volatile u32*) (DANUBE_GPTC + 0x003C))
#define DANUBE_GPTC_RELOAD_2B ((volatile u32*) (DANUBE_GPTC + 0x0044))
#define DANUBE_GPTC_COUNT_2B  ((volatile u32*) (DANUBE_GPTC + 0x004C))

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
/***ASC Clock Control Register***/
#define DANUBE_ASC1_CLC                      ((volatile u32*)(DANUBE_ASC1+ 0x0000))
#define DANUBE_ASC1_CLC_RMC(value)                (((( 1 << 8) - 1) & (value)) << 8)
#define DANUBE_ASC1_CLC_DISS                            (1 << 1)
#define DANUBE_ASC1_CLC_DISR                            (1 << 0)

/***ASC Port Input Select Register***/
#define DANUBE_ASC1_PISEL                    ((volatile u32*)(DANUBE_ASC1+ 0x0004))
#define DANUBE_ASC1_PISEL                    ((volatile u32*)(DANUBE_ASC1+ 0x0004))
#define DANUBE_ASC1_PISEL_RIS                              (1 << 0)

/***ASC Control Register***/
#define DANUBE_ASC1_CON                      ((volatile u32*)(DANUBE_ASC1+ 0x0010))
#define DANUBE_ASC1_CON_BEN                              (1 << 20)
#define DANUBE_ASC1_CON_TOEN                              (1 << 20)
#define DANUBE_ASC1_CON_ROEN                              (1 << 19)
#define DANUBE_ASC1_CON_RUEN                              (1 << 18)
#define DANUBE_ASC1_CON_FEN                              (1 << 17)
#define DANUBE_ASC1_CON_PAL                              (1 << 16)
#define DANUBE_ASC1_CON_R                              (1 << 15)
#define DANUBE_ASC1_CON_ACO                             (1 << 14)
#define DANUBE_ASC1_CON_LB                              (1 << 13)
#define DANUBE_ASC1_CON_ERCLK                             (1 << 10)
#define DANUBE_ASC1_CON_FDE                              (1 << 9)
#define DANUBE_ASC1_CON_BRS                              (1 << 8)
#define DANUBE_ASC1_CON_STP                              (1 << 7)
#define DANUBE_ASC1_CON_SP                              (1 << 6)
#define DANUBE_ASC1_CON_ODD                              (1 << 5)
#define DANUBE_ASC1_CON_PEN                              (1 << 4)
#define DANUBE_ASC1_CON_M(value)                (((( 1 << 3) - 1) & (value)) << 0)

/***ASC Staus Register***/
#define DANUBE_ASC1_STATE                      ((volatile u32*)(DANUBE_ASC1+ 0x0014))
/***ASC Write Hardware Modified Control Register***/
#define DANUBE_ASC1_WHBSTATE                   ((volatile u32*)(DANUBE_ASC1+ 0x0018))
#define DANUBE_ASC1_WHBSTATE_SETBE                          (1 << 113)
#define DANUBE_ASC1_WHBSTATE_SETTOE                          (1 << 12)
#define DANUBE_ASC1_WHBSTATE_SETROE                          (1 << 11)
#define DANUBE_ASC1_WHBSTATE_SETRUE                          (1 << 10)
#define DANUBE_ASC1_WHBSTATE_SETFE                          (1 << 19)
#define DANUBE_ASC1_WHBSTATE_SETPE                          (1 << 18)
#define DANUBE_ASC1_WHBSTATE_CLRBE                          (1 << 17)
#define DANUBE_ASC1_WHBSTATE_CLRTOE                          (1 << 6)
#define DANUBE_ASC1_WHBSTATE_CLRROE                          (1 << 5)
#define DANUBE_ASC1_WHBSTATE_CLRRUE                          (1 << 4)
#define DANUBE_ASC1_WHBSTATE_CLRFE                          (1 << 3)
#define DANUBE_ASC1_WHBSTATE_CLRPE                          (1 << 2)
#define DANUBE_ASC1_WHBSTATE_SETREN                        (1 << 1)
#define DANUBE_ASC1_WHBSTATE_CLRREN                        (1 << 0)

/***ASC Baudrate Timer/Reload Register***/
#define DANUBE_ASC1_BG                      ((volatile u32*)(DANUBE_ASC1+ 0x0050))
#define DANUBE_ASC1_BG_BR_VALUE(value)          (((( 1 << 13) - 1) & (value)) << 0)

/***ASC Fractional Divider Register***/
#define DANUBE_ASC1_FDV                      ((volatile u32*)(DANUBE_ASC1+ 0x0018))
#define DANUBE_ASC1_FDV_FD_VALUE(value)          (((( 1 << 9) - 1) & (value)) << 0)

/***ASC Transmit Buffer Register***/
#define DANUBE_ASC1_TBUF                    ((volatile u32*)(DANUBE_ASC1+ 0x0020))
#define DANUBE_ASC1_TBUF_TD_VALUE(value)          (((( 1 << 9) - 1) & (value)) << 0)

/***ASC Receive Buffer Register***/
#define DANUBE_ASC1_RBUF                    ((volatile u32*)(DANUBE_ASC1+ 0x0024))
#define DANUBE_ASC1_RBUF_RD_VALUE(value)          (((( 1 << 9) - 1) & (value)) << 0)

/***ASC Autobaud Control Register***/
#define DANUBE_ASC1_ABCON                    ((volatile u32*)(DANUBE_ASC1+ 0x0030))
#define DANUBE_ASC1_ABCON_RXINV                          (1 << 11)
#define DANUBE_ASC1_ABCON_TXINV                          (1 << 10)
#define DANUBE_ASC1_ABCON_ABEM(value)               (((( 1 << 2) - 1) & (value)) << 8)
#define DANUBE_ASC1_ABCON_FCDETEN                      (1 << 4)
#define DANUBE_ASC1_ABCON_ABDETEN                      (1 << 3)
#define DANUBE_ASC1_ABCON_ABSTEN                        (1 << 2)
#define DANUBE_ASC1_ABCON_AUREN                          (1 << 1)
#define DANUBE_ASC1_ABCON_ABEN                            (1 << 0)

/***Receive FIFO Control Register***/
#define DANUBE_ASC1_RXFCON                       ((volatile u32*)(DANUBE_ASC1+ 0x0040))
#define DANUBE_ASC1_RXFCON_RXFITL(value)             (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_ASC1_RXFCON_RXFFLU                        (1 << 1)
#define DANUBE_ASC1_RXFCON_RXFEN                          (1 << 0)

/***Transmit FIFO Control Register***/
#define DANUBE_ASC1_TXFCON                       ((volatile u32*)(DANUBE_ASC1+ 0x0044))
#define DANUBE_ASC1_TXFCON_TXFITL(value)             (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_ASC1_TXFCON_TXFFLU                        (1 << 1)
#define DANUBE_ASC1_TXFCON_TXFEN                          (1 << 0)

/***FIFO Status Register***/
#define DANUBE_ASC1_FSTAT                        ((volatile u32*)(DANUBE_ASC1+ 0x0048))
#define DANUBE_ASC1_FSTAT_TXFFL(value)              (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_ASC1_FSTAT_RXFFL(value)              (((( 1 << 6) - 1) & (value)) << 0)
#define DANUBE_ASC1_FSTAT_TXFREE_GET(value)     (((value) >> 24) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_TXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 24)
#define DANUBE_ASC1_FSTAT_RXFREE_GET(value)     (((value) >> 16) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_RXFREE_SET(value)     (((( 1 << 6) - 1) & (value)) << 16)
#define DANUBE_ASC1_FSTAT_TXFFL_GET(value)      (((value) >> 8) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_TXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 8)
#define DANUBE_ASC1_FSTAT_RXFFL_GET(value)      (((value) >> 0) & ((1 << 6) - 1))
#define DANUBE_ASC1_FSTAT_RXFFL_SET(value)      (((( 1 << 6) - 1) & (value)) << 0)

/***ASC Autobaud Status Register***/
#define DANUBE_ASC1_ABSTAT                   ((volatile u32*)(DANUBE_ASC1+ 0x0034))
#define DANUBE_ASC1_ABSTAT_DETWAIT                      (1 << 4)
#define DANUBE_ASC1_ABSTAT_SCCDET                        (1 << 3)
#define DANUBE_ASC1_ABSTAT_SCSDET                        (1 << 2)
#define DANUBE_ASC1_ABSTAT_FCCDET                        (1 << 1)
#define DANUBE_ASC1_ABSTAT_FCSDET                        (1 << 0)

/***ASC Write HW Modified Autobaud Status Register***/
#define DANUBE_ASC1_WHBABSTAT                 ((volatile u32*)(DANUBE_ASC1+ 0x003C))
#define DANUBE_ASC1_WHBABSTAT_SETDETWAIT                (1 << 9)
#define DANUBE_ASC1_WHBABSTAT_CLRDETWAIT                (1 << 8)
#define DANUBE_ASC1_WHBABSTAT_SETSCCDET                  (1 << 7)
#define DANUBE_ASC1_WHBABSTAT_CLRSCCDET                  (1 << 6)
#define DANUBE_ASC1_WHBABSTAT_SETSCSDET                  (1 << 5)
#define DANUBE_ASC1_WHBABSTAT_CLRSCSDET                  (1 << 4)
#define DANUBE_ASC1_WHBABSTAT_SETFCCDET                  (1 << 3)
#define DANUBE_ASC1_WHBABSTAT_CLRFCCDET                  (1 << 2)
#define DANUBE_ASC1_WHBABSTAT_SETFCSDET                  (1 << 1)
#define DANUBE_ASC1_WHBABSTAT_CLRFCSDET                  (1 << 0)

/***ASC IRNCR0 **/
#define DANUBE_ASC1_IRNREN			((volatile u32*)(DANUBE_ASC1+ 0x00F4))
#define DANUBE_ASC1_IRNICR			((volatile u32*)(DANUBE_ASC1+ 0x00FC))
/***ASC IRNCR1 **/
#define DANUBE_ASC1_IRNCR			((volatile u32*)(DANUBE_ASC1+ 0x00F8))
#define ASC_IRNCR_TIR	0x4
#define ASC_IRNCR_RIR	0x2
#define ASC_IRNCR_EIR	0x4

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
/***********************************************************************/
#define DANUBE_ICU_IM0_ISR                      ((volatile u32*)(DANUBE_ICU + 0x0000))
#define DANUBE_ICU_IM0_IER                      ((volatile u32*)(DANUBE_ICU + 0x0008))
#define DANUBE_ICU_IM0_IOSR                     ((volatile u32*)(DANUBE_ICU + 0x0010))
#define DANUBE_ICU_IM0_IRSR                     ((volatile u32*)(DANUBE_ICU + 0x0018))
#define DANUBE_ICU_IM0_IMR                      ((volatile u32*)(DANUBE_ICU + 0x0020))
#define DANUBE_ICU_IM0_IMR_IID                  (1 << 31)
#define DANUBE_ICU_IM0_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define DANUBE_ICU_IM0_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define DANUBE_ICU_IM0_IR(value)                (1 << (value))

#define DANUBE_ICU_IM1_ISR                      ((volatile u32*)(DANUBE_ICU + 0x0028))
#define DANUBE_ICU_IM1_IER                      ((volatile u32*)(DANUBE_ICU + 0x0030))
#define DANUBE_ICU_IM1_IOSR                     ((volatile u32*)(DANUBE_ICU + 0x0038))
#define DANUBE_ICU_IM1_IRSR                     ((volatile u32*)(DANUBE_ICU + 0x0040))
#define DANUBE_ICU_IM1_IMR                      ((volatile u32*)(DANUBE_ICU + 0x0048))
#define DANUBE_ICU_IM1_IMR_IID                  (1 << 31)
#define DANUBE_ICU_IM1_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define DANUBE_ICU_IM1_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define DANUBE_ICU_IM1_IR(value)                (1 << (value))

#define DANUBE_ICU_IM2_ISR                      ((volatile u32*)(DANUBE_ICU + 0x0050))
#define DANUBE_ICU_IM2_IER                      ((volatile u32*)(DANUBE_ICU + 0x0058))
#define DANUBE_ICU_IM2_IOSR                     ((volatile u32*)(DANUBE_ICU + 0x0060))
#define DANUBE_ICU_IM2_IRSR                     ((volatile u32*)(DANUBE_ICU + 0x0068))
#define DANUBE_ICU_IM2_IMR                      ((volatile u32*)(DANUBE_ICU + 0x0070))
#define DANUBE_ICU_IM2_IMR_IID                  (1 << 31)
#define DANUBE_ICU_IM2_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define DANUBE_ICU_IM2_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define DANUBE_ICU_IM2_IR(value)                (1 << (value))

#define DANUBE_ICU_IM3_ISR                      ((volatile u32*)(DANUBE_ICU + 0x0078))
#define DANUBE_ICU_IM3_IER                      ((volatile u32*)(DANUBE_ICU + 0x0080))
#define DANUBE_ICU_IM3_IOSR                     ((volatile u32*)(DANUBE_ICU + 0x0088))
#define DANUBE_ICU_IM3_IRSR                     ((volatile u32*)(DANUBE_ICU + 0x0090))
#define DANUBE_ICU_IM3_IMR                      ((volatile u32*)(DANUBE_ICU + 0x0098))
#define DANUBE_ICU_IM3_IMR_IID                  (1 << 31)
#define DANUBE_ICU_IM3_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define DANUBE_ICU_IM3_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define DANUBE_ICU_IM3_IR(value)                (1 << (value))

#define DANUBE_ICU_IM4_ISR                      ((volatile u32*)(DANUBE_ICU + 0x00A0))
#define DANUBE_ICU_IM4_IER                      ((volatile u32*)(DANUBE_ICU + 0x00A8))
#define DANUBE_ICU_IM4_IOSR                     ((volatile u32*)(DANUBE_ICU + 0x00B0))
#define DANUBE_ICU_IM4_IRSR                     ((volatile u32*)(DANUBE_ICU + 0x00B8))
#define DANUBE_ICU_IM4_IMR                      ((volatile u32*)(DANUBE_ICU + 0x00C0))
#define DANUBE_ICU_IM4_IMR_IID                  (1 << 31)
#define DANUBE_ICU_IM4_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define DANUBE_ICU_IM4_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define DANUBE_ICU_IM4_IR(value)                (1 << (value))

#define DANUBE_ICU_IM5_ISR                      ((volatile u32*)(DANUBE_ICU + 0x00C8))
#define DANUBE_ICU_IM5_IER                      ((volatile u32*)(DANUBE_ICU + 0x00D0))
#define DANUBE_ICU_IM5_IOSR                     ((volatile u32*)(DANUBE_ICU + 0x00D8))
#define DANUBE_ICU_IM5_IRSR                     ((volatile u32*)(DANUBE_ICU + 0x00E0))
#define DANUBE_ICU_IM5_IMR                      ((volatile u32*)(DANUBE_ICU + 0x00E8))
#define DANUBE_ICU_IM5_IMR_IID                  (1 << 31)
#define DANUBE_ICU_IM5_IMR_IN_GET(value)        (((value) >> 0) & ((1 << 5) - 1))
#define DANUBE_ICU_IM5_IMR_IN_SET(value)        (((( 1 << 5) - 1) & (value)) << 0)
#define DANUBE_ICU_IM5_IR(value)                (1 << (value))

/***Interrupt Vector Value Register***/
//#define DANUBE_ICU_IM_VEC                      ((volatile u32*)(DANUBE_ICU+ 0x00f0))
#define DANUBE_ICU_IM_VEC                      ((volatile u32*)(DANUBE_ICU+ 0x00EC))

/***Interrupt Vector Value Mask***/
#define DANUBE_ICU_IM0_VEC_MASK                0x0000001f
#define DANUBE_ICU_IM1_VEC_MASK                0x000003e0
#define DANUBE_ICU_IM2_VEC_MASK                0x00007c00
#define DANUBE_ICU_IM3_VEC_MASK                0x000f8000
#define DANUBE_ICU_IM4_VEC_MASK                0x01f00000

#define DANUBE_ICU_IM0_ISR_IR(value)          (1<<(value))
#define DANUBE_ICU_IM0_IER_IR(value)          (1<<(value))
#define DANUBE_ICU_IM1_ISR_IR(value)          (1<<(value))
#define DANUBE_ICU_IM1_IER_IR(value)          (1<<(value))
#define DANUBE_ICU_IM2_ISR_IR(value)          (1<<(value))
#define DANUBE_ICU_IM2_IER_IR(value)          (1<<(value))
#define DANUBE_ICU_IM3_ISR_IR(value)          (1<<(value))
#define DANUBE_ICU_IM3_IER_IR(value)          (1<<(value))
#define DANUBE_ICU_IM4_ISR_IR(value)          (1<<(value))
#define DANUBE_ICU_IM4_IER_IR(value)          (1<<(value))
#define DANUBE_ICU_IM5_ISR_IR(value)          (1<<(value))
#define DANUBE_ICU_IM5_IER_IR(value)          (1<<(value))

/***DMA Interrupt Mask Value***/
#define DANUBE_DMA_H_MASK			0x00000fff

/***External Interrupt Control Register***/
#define DANUBE_ICU_EIU                    (KSEG1+0x1f101000)
#define DANUBE_ICU_EIU_EXIN_C                ((volatile u32*)(DANUBE_ICU_EIU+ 0x0000))
#define DANUBE_ICU_EIU_INIC                  ((volatile u32*)(DANUBE_ICU_EIU+ 0x0004))
#define DANUBE_ICU_EIU_INC                   ((volatile u32*)(DANUBE_ICU_EIU+ 0x0008))
#define DANUBE_ICU_EIU_INEN                   ((volatile u32*)(DANUBE_ICU_EIU+ 0x000c))

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

#define DANUBE_MPS_SRAM ((volatile u32*)(KSEG1 + 0x1F200000))

#define DANUBE_MPS_VCPU_FW_AD ((volatile u32*)(KSEG1 + 0x1F2001E0))

#define DANUBE_FUSE_BASE_ADDR	(KSEG1+0x1F107354)

/************************************************************************/
/*   Module       :   DEU register address and bits        		*/
/************************************************************************/
//#define DANUBE_DEU_BASE_ADDR               (0xBE102000)
#define DANUBE_DEU_BASE_ADDR               (KSEG1 + 0x1E103100)
/*   DEU Control Register */
#define DANUBE_DEU_CLK                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0000))
#define DANUBE_DEU_ID                      ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0008))

/*   DEU control register */
#define DANUBE_DES_CON                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0010))
#define DANUBE_DES_IHR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0014))
#define DANUBE_DES_ILR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0018))
#define DANUBE_DES_K1HR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x001C))
#define DANUBE_DES_K1LR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0020))
#define DANUBE_DES_K3HR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0024))
#define DANUBE_DES_K3LR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0028))
#define DANUBE_DES_IVHR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x002C))
#define DANUBE_DES_IVLR                    ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0030))
#define DANUBE_DES_OHR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0040))
#define DANUBE_DES_OLR                     ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x0050))

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

#define DANUBE_DEU_IRNEN                         ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00F4))
#define DANUBE_DEU_IRNCR                         ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00F8))
#define DANUBE_DEU_IRNICR                         ((volatile u32 *)(DANUBE_DEU_BASE_ADDR + 0x00FC))

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

#define DANUBE_PPE32_SRST		     (DANUBE_PPE32_BASE + 0x10080)

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

/* Sharebuff SB RAM2 control data */

#define DANUBE_PPE32_SB2_DATABASE  	((DANUBE_PPE32_BASE + (0x8C00 * 4)))
#define DANUBE_PPE32_SB2_CTRLBASE  	((DANUBE_PPE32_BASE + (0x92E0 * 4)))
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

/* ENET Register */
#define DANUBE_PPE32_ENET_MAC_CFG            	((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0610 * 4)))
#define DANUBE_PPE32_ENET_IG_PKTDROP          	((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0619 * 4)))
#define DANUBE_PPE32_ENET_CoS_CFG          	((volatile u32 *)(DANUBE_PPE32_DATA_MEM_MAP_REG_BASE + (0x0618 * 4)))

/* Sharebuff SB RAM2 control data */

#define DANUBE_PPE32_SB2_DATABASE  	((DANUBE_PPE32_BASE + (0x8C00 * 4)))
#define DANUBE_PPE32_SB2_CTRLBASE  	((DANUBE_PPE32_BASE + (0x92E0 * 4)))

/***********************************************************************/
/*  Module      :  PCI register address and bits                       */
/***********************************************************************/
#define PCI_CR_PR_OFFSET            (KSEG1+0x1E105400)
#define PCI_CFG_BASE      		      (KSEG1+0x17000000)
#define PCI_MEM_BASE      		      (KSEG1+0x18000000)
#define PCI_CS_PR_OFFSET            (KSEG1+0x17000000)

/* PCI CONTROLLER REGISTER ADDRESS MAP */
#define PCI_CR_CLK_CTRL_REG         (PCI_CR_PR_OFFSET + 0x0000)
#define PCI_CR_PCI_ID_REG	        (PCI_CR_PR_OFFSET + 0x0004)
#define PCI_CR_SFT_RST_REG	        (PCI_CR_PR_OFFSET + 0x0010)
#define PCI_CR_PCI_FPI_ERR_ADDR_REG (PCI_CR_PR_OFFSET + 0x0014)
#define PCI_CR_FCI_PCI_ERR_ADDR_REG (PCI_CR_PR_OFFSET + 0x0018)
#define PCI_CR_FPI_ERR_TAG_REG	    (PCI_CR_PR_OFFSET + 0x001C)
#define PCI_CR_PCI_IRR_REG	        (PCI_CR_PR_OFFSET + 0x0020)
#define PCI_CR_PCI_IRA_REG	        (PCI_CR_PR_OFFSET + 0x0024)
#define PCI_CR_PCI_IRM_REG	        (PCI_CR_PR_OFFSET + 0x0028)
#define PCI_CR_PCI_EOI_REG	        (PCI_CR_PR_OFFSET + 0x002C)
#define PCI_CR_PCI_MOD_REG 	        (PCI_CR_PR_OFFSET + 0x0030)
#define PCI_CR_DV_ID_REG	        (PCI_CR_PR_OFFSET + 0x0034)
#define PCI_CR_SUBSYS_ID_REG	    (PCI_CR_PR_OFFSET + 0x0038)
#define PCI_CR_PCI_PM_REG           (PCI_CR_PR_OFFSET + 0x003C)
#define PCI_CR_CLASS_CODE1_REG      (PCI_CR_PR_OFFSET + 0x0040)
#define PCI_CR_BAR11MASK_REG	    (PCI_CR_PR_OFFSET + 0x0044)
#define PCI_CR_BAR12MASK_REG        (PCI_CR_PR_OFFSET + 0x0048)
#define PCI_CR_BAR13MASK_REG        (PCI_CR_PR_OFFSET + 0x004C)
#define PCI_CR_BAR14MASK_REG        (PCI_CR_PR_OFFSET + 0x0050)
#define PCI_CR_BAR15MASK_REG        (PCI_CR_PR_OFFSET + 0x0054)
#define PCI_CR_BAR16MASK_REG        (PCI_CR_PR_OFFSET + 0x0058)
#define PCI_CR_CIS_PT1_REG	        (PCI_CR_PR_OFFSET + 0x005C)
#define PCI_CR_SUBSYS_ID1_REG	    (PCI_CR_PR_OFFSET + 0x0060)
#define PCI_CR_PCI_ADDR_MAP11_REG   (PCI_CR_PR_OFFSET + 0x0064)
#define PCI_CR_PCI_ADDR_MAP12_REG   (PCI_CR_PR_OFFSET + 0x0068)
#define PCI_CR_PCI_ADDR_MAP13_REG   (PCI_CR_PR_OFFSET + 0x006C)
#define PCI_CR_PCI_ADDR_MAP14_REG   (PCI_CR_PR_OFFSET + 0x0070)
#define PCI_CR_PCI_ADDR_MAP15_REG   (PCI_CR_PR_OFFSET + 0x0074)
#define PCI_CR_PCI_ADDR_MAP16_REG   (PCI_CR_PR_OFFSET + 0x0078)
#define PCI_CR_FPI_SEG_EN_REG       (PCI_CR_PR_OFFSET + 0x007C)
#define PCI_CR_PC_ARB_REG	        (PCI_CR_PR_OFFSET + 0x0080)
#define PCI_CR_BAR21MASK_REG	    (PCI_CR_PR_OFFSET + 0x0084)
#define PCI_CR_BAR22MASK_REG        (PCI_CR_PR_OFFSET + 0x0088)
#define PCI_CR_BAR23MASK_REG        (PCI_CR_PR_OFFSET + 0x008C)
#define PCI_CR_BAR24MASK_REG        (PCI_CR_PR_OFFSET + 0x0090)
#define PCI_CR_BAR25MASK_REG        (PCI_CR_PR_OFFSET + 0x0094)
#define PCI_CR_BAR26MASK_REG        (PCI_CR_PR_OFFSET + 0x0098)
#define PCI_CR_CIS_PT2_REG	        (PCI_CR_PR_OFFSET + 0x009C)
#define PCI_CR_SUBSYS_ID2_REG	    (PCI_CR_PR_OFFSET + 0x00A0)
#define PCI_CR_PCI_ADDR_MAP21_REG   (PCI_CR_PR_OFFSET + 0x00A4)
#define PCI_CR_PCI_ADDR_MAP22_REG   (PCI_CR_PR_OFFSET + 0x00A8)
#define PCI_CR_PCI_ADDR_MAP23_REG   (PCI_CR_PR_OFFSET + 0x00AC)
#define PCI_CR_PCI_ADDR_MAP24_REG   (PCI_CR_PR_OFFSET + 0x00B0)
#define PCI_CR_PCI_ADDR_MAP25_REG   (PCI_CR_PR_OFFSET + 0x00B4)
#define PCI_CR_PCI_ADDR_MAP26_REG   (PCI_CR_PR_OFFSET + 0x00B8)
#define PCI_CR_FPI_ADDR_MASK_REG    (PCI_CR_PR_OFFSET + 0x00BC)
#define PCI_CR_FCI_ADDR_MAP0_REG    (PCI_CR_PR_OFFSET + 0x00C0)
#define PCI_CR_FCI_ADDR_MAP1_REG    (PCI_CR_PR_OFFSET + 0x00C4)
#define PCI_CR_FCI_ADDR_MAP2_REG    (PCI_CR_PR_OFFSET + 0x00C8)
#define PCI_CR_FCI_ADDR_MAP3_REG    (PCI_CR_PR_OFFSET + 0x00CC)
#define PCI_CR_FCI_ADDR_MAP4_REG    (PCI_CR_PR_OFFSET + 0x00D0)
#define PCI_CR_FCI_ADDR_MAP5_REG    (PCI_CR_PR_OFFSET + 0x00D4)
#define PCI_CR_FCI_ADDR_MAP6_REG    (PCI_CR_PR_OFFSET + 0x00D8)
#define PCI_CR_FCI_ADDR_MAP7_REG    (PCI_CR_PR_OFFSET + 0x00DC)
#define PCI_CR_FCI_ADDR_MAP11lo_REG (PCI_CR_PR_OFFSET + 0x00E0)
#define PCI_CR_FCI_ADDR_MAP11hg_REG (PCI_CR_PR_OFFSET + 0x00E4)
#define PCI_CR_FCI_BURST_LENGTH_REG (PCI_CR_PR_OFFSET + 0x00E8)
#define PCI_CR_PCI_SET_SERR_REG     (PCI_CR_PR_OFFSET + 0x00EC)
#define PCI_CR_DMA_FPI_ST_ADDR_REG  (PCI_CR_PR_OFFSET + 0x00F0)
#define PCI_CR_DMA_PCI_ST_ADDR_REG  (PCI_CR_PR_OFFSET + 0x00F4)
#define PCI_CR_DMA_TRAN_CNT_REG     (PCI_CR_PR_OFFSET + 0x00F8)
#define PCI_CR_DMA_TRAN_CTL_REG     (PCI_CR_PR_OFFSET + 0x00FC)

/* PCI CONFIGURATION SPACE REGISTER Base Address */
#define EXT_PCI1_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x0800
#define EXT_PCI2_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x1000
#define EXT_PCI3_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x1800
#define EXT_PCI4_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x2000
#define EXT_PCI5_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x2800
#define EXT_PCI6_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x3000
#define EXT_PCI7_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x3800
#define EXT_PCI8_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x4000
#define EXT_PCI9_CONFIG_SPACE_BASE_ADDR    	PCI_CFG_BASE + 0x4800
#define EXT_PCI10_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x5000
#define EXT_PCI11_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x5800
#define EXT_PCI12_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x6000
#define EXT_PCI13_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x6800
#define EXT_PCI14_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x7000
#define EXT_PCI15_CONFIG_SPACE_BASE_ADDR   	PCI_CFG_BASE + 0x7800
#define EXT_CARDBUS_CONFIG_SPACE_BASE_ADDR  	PCI_CFG_BASE + 0XF000

/* PCI CONFIGURATION SPACE REGISTER ADDRESS MAP */
#define PCI_CS_DEV_VEN_ID_REG        (PCI_CS_PR_OFFSET + 0x0000)
#define PCI_CS_STS_CMD_REG	         (PCI_CS_PR_OFFSET + 0x0004)
#define PCI_CS_CL_CODE_REVIDG        (PCI_CS_PR_OFFSET + 0x0008)
#define PCI_CS_BST_HT_LT_CLS_REG     (PCI_CS_PR_OFFSET + 0x000C)
#define PCI_CS_BASE_ADDR1_REG	     (PCI_CS_PR_OFFSET + 0x0010)
#define PCI_CS_BASE_ADDR2_REG        (PCI_CS_PR_OFFSET + 0x0014)
#define PCI_CS_BASE_ADDR3_REG        (PCI_CS_PR_OFFSET + 0x0018)
#define PCI_CS_BASE_ADDR4_REG        (PCI_CS_PR_OFFSET + 0x001C)
#define PCI_CS_BASE_ADDR5_REG        (PCI_CS_PR_OFFSET + 0x0020)
#define PCI_CS_BASE_ADDR6_REG        (PCI_CS_PR_OFFSET + 0x0024)
#define PCI_CS_CARDBUS_CIS_PT_REG    (PCI_CS_PR_OFFSET + 0x0028)
#define PCI_CS_SUBSYS_VEN_ID_REG     (PCI_CS_PR_OFFSET + 0x002C)
#define PCI_CS_EXROM_BAS_ADDR_REG    (PCI_CS_PR_OFFSET + 0x0030)
#define PCI_CS_RES1_REG	             (PCI_CS_PR_OFFSET + 0x0034)
#define PCI_CS_RES2_REG	             (PCI_CS_PR_OFFSET + 0x0038)
#define PCI_CS_LAT_GNT_INTR_REG      (PCI_CS_PR_OFFSET + 0x003C)
#define PCI_CS_PM_PT_CPID_REG        (PCI_CS_PR_OFFSET + 0x0040)
#define PCI_CS_DAT_PMCSR_PM_REG	     (PCI_CS_PR_OFFSET + 0x0044)
#define PCI_CS_RES3_REG              (PCI_CS_PR_OFFSET + 0x0048)
#define PCI_CS_RES4_REG              (PCI_CS_PR_OFFSET + 0x004C)
#define PCI_CS_ERR_ADDR_PCI_FPI_REG  (PCI_CS_PR_OFFSET + 0x0050)
#define PCI_CS_ERR_ADDR_FPI_PCI_REG  (PCI_CS_PR_OFFSET + 0x0054)
#define PCI_CS_ERR_TAG_FPI_PCI_REG   (PCI_CS_PR_OFFSET + 0x0058)
#define PCI_CS_PC_ARB_REG	         (PCI_CS_PR_OFFSET + 0x005C)
#define PCI_CS_FPI_PCI_INT_STS_REG   (PCI_CS_PR_OFFSET + 0x0060)
#define PCI_CS_FPI_PCI_INT_ACK_REG   (PCI_CS_PR_OFFSET + 0x0064)
#define PCI_CS_FPI_PCI_INT_MASK_REG  (PCI_CS_PR_OFFSET + 0x0068)
#define PCI_CS_CARDBUS_CTL_STS_REG   (PCI_CS_PR_OFFSET + 0x006C)

// PCI CONTROLLER ADDRESS SPACE
#define PCI_CA_PR_OFFSET 0xB8000000
#define PCI_CA_PR_END    0xBBFFFFFF

// PCI CONTROLLER REGISTER ADDRESS MASK
#define PCI_CR_CLK_CTRL_MSK              0x82000000
#define PCI_CR_PCI_ID_MSK	    	     0x00000000
#define PCI_CR_SFT_RST_MSK	    	     0x00000002
#define PCI_CR_PCI_FPI_ERR_ADDR_MSK 	 0x00000000
#define PCI_CR_FCI_PCI_ERR_ADDR_MSK 	 0x00000000
#define PCI_CR_FPI_ERR_TAG_MSK	    	 0x00000000
#define PCI_CR_PCI_IRR_MSK	    	     0x07013B2F
#define PCI_CR_PCI_IRA_MSK	    	     0x07013B2F
#define PCI_CR_PCI_IRM_MSK	    	     0x07013B2F
#define PCI_CR_PCI_EOI_MSK	    	     0x07013B2F
#define PCI_CR_PCI_MOD_MSK 	    	     0x1107070F
#define PCI_CR_DV_ID_MSK	    	     0x00000000
#define PCI_CR_SUBSYS_ID_MSK	    	 0x00000000
#define PCI_CR_PCI_PM_MSK           	 0x0000001F
#define PCI_CR_CLASS_CODE1_MSK      	 0x00000000
#define PCI_CR_BAR11MASK_MSK	    	 0x8FFFFFF8
#define PCI_CR_BAR12MASK_MSK        	 0x80001F08
#define PCI_CR_BAR13MASK_MSK        	 0x8FF00008
#define PCI_CR_BAR14MASK_MSK        	 0x8FFFFF08
#define PCI_CR_BAR15MASK_MSK        	 0x8FFFFF08
#define PCI_CR_BAR16MASK_MSK        	 0x8FFFFFF9
#define PCI_CR_CIS_PT1_MSK	    	     0x03FFFFFF
#define PCI_CR_SUBSYS_ID1_MSK	    	 0x00000000
#define PCI_CR_PCI_ADDR_MAP11_MSK   	 0x7FFF0001
#define PCI_CR_PCI_ADDR_MAP12_MSK   	 0x7FFFFF01
#define PCI_CR_PCI_ADDR_MAP13_MSK   	 0x7FF00001
#define PCI_CR_PCI_ADDR_MAP14_MSK   	 0x7FFFFF01
#define PCI_CR_PCI_ADDR_MAP15_MSK   	 0x7FF00001
#define PCI_CR_PCI_ADDR_MAP16_MSK   	 0x7FF00001
#define PCI_CR_FPI_SEG_EN_MSK       	 0x000003FF
#define PCI_CR_CLASS_CODE2_MSK      	 0x00000000
#define PCI_CR_BAR21MASK_MSK	    	 0x800F0008
#define PCI_CR_BAR22MASK_MSK        	 0x807F0008
#define PCI_CR_BAR23MASK_MSK        	 0x8FF00008
#define PCI_CR_BAR24MASK_MSK        	 0x8FFFFF08
#define PCI_CR_BAR25MASK_MSK        	 0x8FFFFF08
#define PCI_CR_BAR26MASK_MSK        	 0x8FFFFFF9
#define PCI_CR_CIS_PT2_MSK	    	     0x03FFFFFF
#define PCI_CR_SUBSYS_ID2_MSK	    	 0x00000000
#define PCI_CR_PCI_ADDR_MAP21_MSK   	 0x7FFE0001
#define PCI_CR_PCI_ADDR_MAP22_MSK   	 0x7FFF0001
#define PCI_CR_PCI_ADDR_MAP23_MSK   	 0x7FF00001
#define PCI_CR_PCI_ADDR_MAP24_MSK   	 0x7FFFFF01
#define PCI_CR_PCI_ADDR_MAP25_MSK   	 0x7FFFFF01
#define PCI_CR_PCI_ADDR_MAP26_MSK   	 0x7FF00001
#define PCI_CR_FPI_ADDR_MASK_MSK    	 0x00070000
#define PCI_CR_FCI_ADDR_MAP0_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP1_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP2_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP3_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP4_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP5_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP6_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP7_MSK    	 0xFFF00000
#define PCI_CR_FCI_ADDR_MAP11lo_MSK 	 0xFFFF0000
#define PCI_CR_FCI_ADDR_MAP11hg_MSK 	 0xFFF00000
#define PCI_CR_FCI_BURST_LENGTH_MSK 	 0x00000303
#define PCI_CR_PCI_SET_SERR_MSK     	 0x00000001
#define PCI_CR_DMA_FPI_ST_ADDR_MSK  	 0xFFFFFFFF
#define PCI_CR_DMA_PCI_ST_ADDR_MSK  	 0xFFFFFFFF
#define PCI_CR_DMA_TRAN_CNT_MSK     	 0x000003FF
#define PCI_CR_DMA_TRAN_CTL_MSK     	 0x00000003

#define INTERNAL_ARB_ENABLE_BIT         	0
#define ARB_SCHEME_BIT                  	1
#define PCI_MASTER0_PRIOR_2BITS         	2
#define PCI_MASTER1_PRIOR_2BITS         	4
#define PCI_MASTER2_PRIOR_2BITS         	6
#define PCI_MASTER0_REQ_MASK_2BITS      	8
#define PCI_MASTER1_REQ_MASK_2BITS      	10
#define PCI_MASTER2_REQ_MASK_2BITS      	12

#define IOPORT_RESOURCE_START 0x10000000
#define IOPORT_RESOURCE_END   0xffffffff
#define IOMEM_RESOURCE_START  0x10000000
#define IOMEM_RESOURCE_END    0xffffffff

/***********************************************************************/
#define DANUBE_REG32(addr)		   *((volatile u32 *)(addr))
/***********************************************************************/
#endif //DANUBE_H
