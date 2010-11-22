/*
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Definitions for Coldfire V4e MMU
 */
#include <asm/movs.h>

#ifndef __CF_MMU_H__
#define __CF_MMU_H__


#define MMU_BASE 0xE8000000


#define MMUCR (MMU_BASE+0x00)
#define MMUCR_ASMN  1
#define MMUCR_ASM   (1<<MMUCR_ASMN)
#define MMUCR_ENN   0
#define MMUCR_EN    (1<<MMUCR_ENN)

#define MMUOR REG16(MMU_BASE+0x04+0x02)
#define MMUOR_AAN   16
#define MMUOR_AA    (0xffff<<MMUOR_AAN)
#define MMUOR_STLBN 8
#define MMUOR_STLB  (1<<MMUOR_STLBN)
#define MMUOR_CAN   7
#define MMUOR_CA    (1<<MMUOR_CAN)
#define MMUOR_CNLN  6
#define MMUOR_CNL   (1<<MMUOR_CNLN)
#define MMUOR_CASN  5
#define MMUOR_CAS   (1<<MMUOR_CASN)
#define MMUOR_ITLBN 4
#define MMUOR_ITLB  (1<<MMUOR_ITLBN)
#define MMUOR_ADRN  3
#define MMUOR_ADR   (1<<MMUOR_ADRN)
#define MMUOR_RWN   2
#define MMUOR_RW    (1<<MMUOR_RWN)
#define MMUOR_ACCN  1
#define MMUOR_ACC   (1<<MMUOR_ACCN)
#define MMUOR_UAAN  0
#define MMUOR_UAA   (1<<MMUOR_UAAN)

#define MMUSR REG32(MMU_BASE+0x08)
#define MMUSR_SPFN  5
#define MMUSR_SPF   (1<<MMUSR_SPFN)
#define MMUSR_RFN   4
#define MMUSR_RF    (1<<MMUSR_RFN)
#define MMUSR_WFN   3
#define MMUSR_WF    (1<<MMUSR_WFN)
#define MMUSR_HITN  1
#define MMUSR_HIT   (1<<MMUSR_HITN)

#define MMUAR REG32(MMU_BASE+0x10)
#define MMUAR_VPN   1
#define MMUAR_VP    (0xfffffffe)
#define MMUAR_SN    0
#define MMUAR_S     (1<<MMUAR_SN)

#define MMUTR REG32(MMU_BASE+0x14)
#define MMUTR_VAN   10
#define MMUTR_VA    (0xfffffc00)
#define MMUTR_IDN   2
#define MMUTR_ID    (0xff<<MMUTR_IDN)
#define MMUTR_SGN   1
#define MMUTR_SG    (1<<MMUTR_SGN)
#define MMUTR_VN    0
#define MMUTR_V     (1<<MMUTR_VN)

#define MMUDR REG32(MMU_BASE+0x18)
#define MMUDR_PAN   10
#define MMUDR_PA    (0xfffffc00)
#define MMUDR_SZN   8
#define MMUDR_SZ_MASK (0x2<<MMUDR_SZN)
#define MMUDR_SZ1M  (0<<MMUDR_SZN)
#define MMUDR_SZ4K  (1<<MMUDR_SZN)
#define MMUDR_SZ8K  (2<<MMUDR_SZN)
#define MMUDR_SZ16M (3<<MMUDR_SZN)
#define MMUDR_CMN   6
#define MMUDR_INC   (2<<MMUDR_CMN)
#define MMUDR_IC    (0<<MMUDR_CMN)
#define MMUDR_DWT   (0<<MMUDR_CMN)
#define MMUDR_DCB   (1<<MMUDR_CMN)
#define MMUDR_DNCP  (2<<MMUDR_CMN)
#define MMUDR_DNCIP (3<<MMUDR_CMN)
#define MMUDR_SPN   5
#define MMUDR_SP    (1<<MMUDR_SPN)
#define MMUDR_RN    4
#define MMUDR_R     (1<<MMUDR_RN)
#define MMUDR_WN    3
#define MMUDR_W     (1<<MMUDR_WN)
#define MMUDR_XN    2
#define MMUDR_X     (1<<MMUDR_XN)
#define MMUDR_LKN   1
#define MMUDR_LK    (1<<MMUDR_LKN)


#ifndef __ASSEMBLY__
#define CF_PMEGS_NUM		256
#define CF_INVALID_CONTEXT 	255
#define CF_PAGE_PGNUM_MASK 	(PAGE_MASK)

extern int cf_tlb_miss(struct pt_regs *regs, int write, int dtlb,
		       int extension_word);
#endif /* __ASSEMBLY__*/

#endif	/* !__CF_MMU_H__ */
