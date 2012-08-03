/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 Thomas Langer <thomas.langer@lantiq.com>
 */

#ifndef _FALCON_IRQ__
#define _FALCON_IRQ__

#define INT_NUM_IRQ0			8
#define INT_NUM_IM0_IRL0		(INT_NUM_IRQ0 + 0)
#define INT_NUM_IM1_IRL0		(INT_NUM_IM0_IRL0 + 32)
#define INT_NUM_IM2_IRL0		(INT_NUM_IM1_IRL0 + 32)
#define INT_NUM_IM3_IRL0		(INT_NUM_IM2_IRL0 + 32)
#define INT_NUM_IM4_IRL0		(INT_NUM_IM3_IRL0 + 32)
#define INT_NUM_EXTRA_START		(INT_NUM_IM4_IRL0 + 32)
#define INT_NUM_IM_OFFSET		(INT_NUM_IM1_IRL0 - INT_NUM_IM0_IRL0)

#define MIPS_CPU_TIMER_IRQ			7

/* HOST IF Event Interrupt */
#define FALCON_IRQ_HOST				(INT_NUM_IM0_IRL0 + 0)
/* HOST IF Mailbox0 Receive Interrupt */
#define FALCON_IRQ_HOST_MB0_RX			(INT_NUM_IM0_IRL0 + 1)
/* HOST IF Mailbox0 Transmit Interrupt */
#define FALCON_IRQ_HOST_MB0_TX			(INT_NUM_IM0_IRL0 + 2)
/* HOST IF Mailbox1 Receive Interrupt */
#define FALCON_IRQ_HOST_MB1_RX			(INT_NUM_IM0_IRL0 + 3)
/* HOST IF Mailbox1 Transmit Interrupt */
#define FALCON_IRQ_HOST_MB1_TX			(INT_NUM_IM0_IRL0 + 4)
/* I2C Last Single Data Transfer Request */
#define FALCON_IRQ_I2C_LSREQ			(INT_NUM_IM0_IRL0 + 8)
/* I2C Single Data Transfer Request */
#define FALCON_IRQ_I2C_SREQ			(INT_NUM_IM0_IRL0 + 9)
/* I2C Last Burst Data Transfer Request */
#define FALCON_IRQ_I2C_LBREQ			(INT_NUM_IM0_IRL0 + 10)
/* I2C Burst Data Transfer Request */
#define FALCON_IRQ_I2C_BREQ			(INT_NUM_IM0_IRL0 + 11)
/* I2C Error Interrupt */
#define FALCON_IRQ_I2C_I2C_ERR			(INT_NUM_IM0_IRL0 + 12)
/* I2C Protocol Interrupt */
#define FALCON_IRQ_I2C_I2C_P			(INT_NUM_IM0_IRL0 + 13)
/* SSC Transmit Interrupt */
#define FALCON_IRQ_SSC_T			(INT_NUM_IM0_IRL0 + 14)
/* SSC Receive Interrupt */
#define FALCON_IRQ_SSC_R			(INT_NUM_IM0_IRL0 + 15)
/* SSC Error Interrupt */
#define FALCON_IRQ_SSC_E			(INT_NUM_IM0_IRL0 + 16)
/* SSC Frame Interrupt */
#define FALCON_IRQ_SSC_F			(INT_NUM_IM0_IRL0 + 17)
/* Advanced Encryption Standard Interrupt */
#define FALCON_IRQ_AES_AES			(INT_NUM_IM0_IRL0 + 27)
/* Secure Hash Algorithm Interrupt */
#define FALCON_IRQ_SHA_HASH			(INT_NUM_IM0_IRL0 + 28)
/* PCM Receive Interrupt */
#define FALCON_IRQ_PCM_RX			(INT_NUM_IM0_IRL0 + 29)
/* PCM Transmit Interrupt */
#define FALCON_IRQ_PCM_TX			(INT_NUM_IM0_IRL0 + 30)
/* PCM Transmit Crash Interrupt */
#define FALCON_IRQ_PCM_HW2_CRASH		(INT_NUM_IM0_IRL0 + 31)

/* EBU Serial Flash Command Error */
#define FALCON_IRQ_EBU_SF_CMDERR		(INT_NUM_IM1_IRL0 + 0)
/* EBU Serial Flash Command Overwrite Error */
#define FALCON_IRQ_EBU_SF_COVERR		(INT_NUM_IM1_IRL0 + 1)
/* EBU Serial Flash Busy */
#define FALCON_IRQ_EBU_SF_BUSY			(INT_NUM_IM1_IRL0 + 2)
/* External Interrupt from GPIO P0 */
#define FALCON_IRQ_GPIO_P0			(INT_NUM_IM1_IRL0 + 4)
/* External Interrupt from GPIO P1 */
#define FALCON_IRQ_GPIO_P1			(INT_NUM_IM1_IRL0 + 5)
/* External Interrupt from GPIO P2 */
#define FALCON_IRQ_GPIO_P2			(INT_NUM_IM1_IRL0 + 6)
/* External Interrupt from GPIO P3 */
#define FALCON_IRQ_GPIO_P3			(INT_NUM_IM1_IRL0 + 7)
/* External Interrupt from GPIO P4 */
#define FALCON_IRQ_GPIO_P4			(INT_NUM_IM1_IRL0 + 8)
/* 8kHz backup interrupt derived from core-PLL */
#define FALCON_IRQ_FSC_BKP			(INT_NUM_IM1_IRL0 + 10)
/* FSC Timer Interrupt 0 */
#define FALCON_IRQ_FSCT_CMP0			(INT_NUM_IM1_IRL0 + 11)
/* FSC Timer Interrupt 1 */
#define FALCON_IRQ_FSCT_CMP1			(INT_NUM_IM1_IRL0 + 12)
/* 8kHz root interrupt derived from GPON interface */
#define FALCON_IRQ_FSC_ROOT			(INT_NUM_IM1_IRL0 + 13)
/* Time of Day */
#define FALCON_IRQ_TOD				(INT_NUM_IM1_IRL0 + 14)
/* PMA Interrupt from IntNode of the 200MHz Domain */
#define FALCON_IRQ_PMA_200M			(INT_NUM_IM1_IRL0 + 15)
/* PMA Interrupt from IntNode of the TX Clk Domain */
#define FALCON_IRQ_PMA_TX			(INT_NUM_IM1_IRL0 + 16)
/* PMA Interrupt from IntNode of the RX Clk Domain */
#define FALCON_IRQ_PMA_RX			(INT_NUM_IM1_IRL0 + 17)
/* SYS1 Interrupt */
#define FALCON_IRQ_SYS1				(INT_NUM_IM1_IRL0 + 20)
/* SYS GPE Interrupt */
#define FALCON_IRQ_SYS_GPE			(INT_NUM_IM1_IRL0 + 21)
/* Watchdog Access Error Interrupt */
#define FALCON_IRQ_WDT_AEIR			(INT_NUM_IM1_IRL0 + 24)
/* Watchdog Prewarning Interrupt */
#define FALCON_IRQ_WDT_PIR			(INT_NUM_IM1_IRL0 + 25)
/* SBIU interrupt */
#define FALCON_IRQ_SBIU0			(INT_NUM_IM1_IRL0 + 27)
/* FPI Bus Control Unit Interrupt */
#define FALCON_IRQ_BCU0				(INT_NUM_IM1_IRL0 + 29)
/* DDR Controller Interrupt */
#define FALCON_IRQ_DDR				(INT_NUM_IM1_IRL0 + 30)
/* Crossbar Error Interrupt */
#define FALCON_IRQ_XBAR_ERROR			(INT_NUM_IM1_IRL0 + 31)

/* ICTRLL 0 Interrupt */
#define FALCON_IRQ_ICTRLL0			(INT_NUM_IM2_IRL0 + 0)
/* ICTRLL 1 Interrupt */
#define FALCON_IRQ_ICTRLL1			(INT_NUM_IM2_IRL0 + 1)
/* ICTRLL 2 Interrupt */
#define FALCON_IRQ_ICTRLL2			(INT_NUM_IM2_IRL0 + 2)
/* ICTRLL 3 Interrupt */
#define FALCON_IRQ_ICTRLL3			(INT_NUM_IM2_IRL0 + 3)
/* OCTRLL 0 Interrupt */
#define FALCON_IRQ_OCTRLL0			(INT_NUM_IM2_IRL0 + 4)
/* OCTRLL 1 Interrupt */
#define FALCON_IRQ_OCTRLL1			(INT_NUM_IM2_IRL0 + 5)
/* OCTRLL 2 Interrupt */
#define FALCON_IRQ_OCTRLL2			(INT_NUM_IM2_IRL0 + 6)
/* OCTRLL 3 Interrupt */
#define FALCON_IRQ_OCTRLL3			(INT_NUM_IM2_IRL0 + 7)
/* OCTRLG Interrupt */
#define FALCON_IRQ_OCTRLG			(INT_NUM_IM2_IRL0 + 9)
/* IQM Interrupt */
#define FALCON_IRQ_IQM				(INT_NUM_IM2_IRL0 + 10)
/* FSQM Interrupt */
#define FALCON_IRQ_FSQM				(INT_NUM_IM2_IRL0 + 11)
/* TMU Interrupt */
#define FALCON_IRQ_TMU				(INT_NUM_IM2_IRL0 + 12)
/* LINK1 Interrupt */
#define FALCON_IRQ_LINK1			(INT_NUM_IM2_IRL0 + 14)
/* ICTRLC 0 Interrupt */
#define FALCON_IRQ_ICTRLC0			(INT_NUM_IM2_IRL0 + 16)
/* ICTRLC 1 Interrupt */
#define FALCON_IRQ_ICTRLC1			(INT_NUM_IM2_IRL0 + 17)
/* OCTRLC Interrupt */
#define FALCON_IRQ_OCTRLC			(INT_NUM_IM2_IRL0 + 18)
/* CONFIG Break Interrupt */
#define FALCON_IRQ_CONFIG_BREAK			(INT_NUM_IM2_IRL0 + 19)
/* CONFIG Interrupt */
#define FALCON_IRQ_CONFIG			(INT_NUM_IM2_IRL0 + 20)
/* Dispatcher Interrupt */
#define FALCON_IRQ_DISP				(INT_NUM_IM2_IRL0 + 21)
/* TBM Interrupt */
#define FALCON_IRQ_TBM				(INT_NUM_IM2_IRL0 + 22)
/* GTC Downstream Interrupt */
#define FALCON_IRQ_GTC_DS			(INT_NUM_IM2_IRL0 + 29)
/* GTC Upstream Interrupt */
#define FALCON_IRQ_GTC_US			(INT_NUM_IM2_IRL0 + 30)
/* EIM Interrupt */
#define FALCON_IRQ_EIM				(INT_NUM_IM2_IRL0 + 31)

/* ASC0 Transmit Interrupt */
#define FALCON_IRQ_ASC0_T			(INT_NUM_IM3_IRL0 + 0)
/* ASC0 Receive Interrupt */
#define FALCON_IRQ_ASC0_R			(INT_NUM_IM3_IRL0 + 1)
/* ASC0 Error Interrupt */
#define FALCON_IRQ_ASC0_E			(INT_NUM_IM3_IRL0 + 2)
/* ASC0 Transmit Buffer Interrupt */
#define FALCON_IRQ_ASC0_TB			(INT_NUM_IM3_IRL0 + 3)
/* ASC0 Autobaud Start Interrupt */
#define FALCON_IRQ_ASC0_ABST			(INT_NUM_IM3_IRL0 + 4)
/* ASC0 Autobaud Detection Interrupt */
#define FALCON_IRQ_ASC0_ABDET			(INT_NUM_IM3_IRL0 + 5)
/* ASC1 Modem Status Interrupt */
#define FALCON_IRQ_ASC0_MS			(INT_NUM_IM3_IRL0 + 6)
/* ASC0 Soft Flow Control Interrupt */
#define FALCON_IRQ_ASC0_SFC			(INT_NUM_IM3_IRL0 + 7)
/* ASC1 Transmit Interrupt */
#define FALCON_IRQ_ASC1_T			(INT_NUM_IM3_IRL0 + 8)
/* ASC1 Receive Interrupt */
#define FALCON_IRQ_ASC1_R			(INT_NUM_IM3_IRL0 + 9)
/* ASC1 Error Interrupt */
#define FALCON_IRQ_ASC1_E			(INT_NUM_IM3_IRL0 + 10)
/* ASC1 Transmit Buffer Interrupt */
#define FALCON_IRQ_ASC1_TB			(INT_NUM_IM3_IRL0 + 11)
/* ASC1 Autobaud Start Interrupt */
#define FALCON_IRQ_ASC1_ABST			(INT_NUM_IM3_IRL0 + 12)
/* ASC1 Autobaud Detection Interrupt */
#define FALCON_IRQ_ASC1_ABDET			(INT_NUM_IM3_IRL0 + 13)
/* ASC1 Modem Status Interrupt */
#define FALCON_IRQ_ASC1_MS			(INT_NUM_IM3_IRL0 + 14)
/* ASC1 Soft Flow Control Interrupt */
#define FALCON_IRQ_ASC1_SFC			(INT_NUM_IM3_IRL0 + 15)
/* GPTC Timer/Counter 1A Interrupt */
#define FALCON_IRQ_GPTC_TC1A			(INT_NUM_IM3_IRL0 + 16)
/* GPTC Timer/Counter 1B Interrupt */
#define FALCON_IRQ_GPTC_TC1B			(INT_NUM_IM3_IRL0 + 17)
/* GPTC Timer/Counter 2A Interrupt */
#define FALCON_IRQ_GPTC_TC2A			(INT_NUM_IM3_IRL0 + 18)
/* GPTC Timer/Counter 2B Interrupt */
#define FALCON_IRQ_GPTC_TC2B			(INT_NUM_IM3_IRL0 + 19)
/* GPTC Timer/Counter 3A Interrupt */
#define FALCON_IRQ_GPTC_TC3A			(INT_NUM_IM3_IRL0 + 20)
/* GPTC Timer/Counter 3B Interrupt */
#define FALCON_IRQ_GPTC_TC3B			(INT_NUM_IM3_IRL0 + 21)
/* DFEV0, Channel 1 Transmit Interrupt */
#define FALCON_IRQ_DFEV0_2TX			(INT_NUM_IM3_IRL0 + 26)
/* DFEV0, Channel 1 Receive Interrupt */
#define FALCON_IRQ_DFEV0_2RX			(INT_NUM_IM3_IRL0 + 27)
/* DFEV0, Channel 1 General Purpose Interrupt */
#define FALCON_IRQ_DFEV0_2GP			(INT_NUM_IM3_IRL0 + 28)
/* DFEV0, Channel 0 Transmit Interrupt */
#define FALCON_IRQ_DFEV0_1TX			(INT_NUM_IM3_IRL0 + 29)
/* DFEV0, Channel 0 Receive Interrupt */
#define FALCON_IRQ_DFEV0_1RX			(INT_NUM_IM3_IRL0 + 30)
/* DFEV0, Channel 0 General Purpose Interrupt */
#define FALCON_IRQ_DFEV0_1GP			(INT_NUM_IM3_IRL0 + 31)

/* ICTRLL 0 Error */
#define FALCON_IRQ_ICTRLL0_ERR			(INT_NUM_IM4_IRL0 + 0)
/* ICTRLL 1 Error */
#define FALCON_IRQ_ICTRLL1_ERR			(INT_NUM_IM4_IRL0 + 1)
/* ICTRLL 2 Error */
#define FALCON_IRQ_ICTRLL2_ERR			(INT_NUM_IM4_IRL0 + 2)
/* ICTRLL 3 Error */
#define FALCON_IRQ_ICTRLL3_ERR			(INT_NUM_IM4_IRL0 + 3)
/* OCTRLL 0 Error */
#define FALCON_IRQ_OCTRLL0_ERR			(INT_NUM_IM4_IRL0 + 4)
/* OCTRLL 1 Error */
#define FALCON_IRQ_OCTRLL1_ERR			(INT_NUM_IM4_IRL0 + 5)
/* OCTRLL 2 Error */
#define FALCON_IRQ_OCTRLL2_ERR			(INT_NUM_IM4_IRL0 + 6)
/* OCTRLL 3 Error */
#define FALCON_IRQ_OCTRLL3_ERR			(INT_NUM_IM4_IRL0 + 7)
/* ICTRLG Error */
#define FALCON_IRQ_ICTRLG_ERR			(INT_NUM_IM4_IRL0 + 8)
/* OCTRLG Error */
#define FALCON_IRQ_OCTRLG_ERR			(INT_NUM_IM4_IRL0 + 9)
/* IQM Error */
#define FALCON_IRQ_IQM_ERR			(INT_NUM_IM4_IRL0 + 10)
/* FSQM Error */
#define FALCON_IRQ_FSQM_ERR			(INT_NUM_IM4_IRL0 + 11)
/* TMU Error */
#define FALCON_IRQ_TMU_ERR			(INT_NUM_IM4_IRL0 + 12)
/* MPS Status Interrupt #0 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR0			(INT_NUM_IM4_IRL0 + 14)
/* MPS Status Interrupt #1 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR1			(INT_NUM_IM4_IRL0 + 15)
/* MPS Status Interrupt #2 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR2			(INT_NUM_IM4_IRL0 + 16)
/* MPS Status Interrupt #3 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR3			(INT_NUM_IM4_IRL0 + 17)
/* MPS Status Interrupt #4 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR4			(INT_NUM_IM4_IRL0 + 18)
/* MPS Status Interrupt #5 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR5			(INT_NUM_IM4_IRL0 + 19)
/* MPS Status Interrupt #6 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR6			(INT_NUM_IM4_IRL0 + 20)
/* MPS Status Interrupt #7 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR7			(INT_NUM_IM4_IRL0 + 21)
/* MPS Status Interrupt #8 (VPE1 to VPE0) */
#define FALCON_IRQ_MPS_IR8			(INT_NUM_IM4_IRL0 + 22)
/* VPE0 Exception Level Flag Interrupt */
#define FALCON_IRQ_VPE0_EXL			(INT_NUM_IM4_IRL0 + 29)
/* VPE0 Error Level Flag Interrupt */
#define FALCON_IRQ_VPE0_ERL			(INT_NUM_IM4_IRL0 + 30)
/* VPE0 Performance Monitoring Counter Interrupt */
#define FALCON_IRQ_VPE0_PMCIR			(INT_NUM_IM4_IRL0 + 31)

#endif /* _FALCON_IRQ__ */
