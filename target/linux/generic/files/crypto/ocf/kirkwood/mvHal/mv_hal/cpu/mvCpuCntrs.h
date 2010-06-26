/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.


********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
*******************************************************************************/
#ifndef __mvCpuCntrs_h__
#define __mvCpuCntrs_h__

#include "mvTypes.h"
#include "mvOs.h"


#define MV_CPU_CNTRS_NUM            4
#define MV_CPU_CNTRS_OPS_NUM        32

typedef enum
{
    MV_CPU_CNTRS_INVALID = 0,
    MV_CPU_CNTRS_CYCLES,
    MV_CPU_CNTRS_ICACHE_READ_MISS,
    MV_CPU_CNTRS_DCACHE_ACCESS,
    MV_CPU_CNTRS_DCACHE_READ_MISS,
    MV_CPU_CNTRS_DCACHE_READ_HIT,
    MV_CPU_CNTRS_DCACHE_WRITE_MISS,
    MV_CPU_CNTRS_DCACHE_WRITE_HIT,
    MV_CPU_CNTRS_DTLB_MISS,
    MV_CPU_CNTRS_TLB_MISS,
    MV_CPU_CNTRS_ITLB_MISS,
    MV_CPU_CNTRS_INSTRUCTIONS,
    MV_CPU_CNTRS_SINGLE_ISSUE,
    MV_CPU_CNTRS_MMU_READ_LATENCY,
    MV_CPU_CNTRS_MMU_READ_BEAT,
    MV_CPU_CNTRS_BRANCH_RETIRED,
    MV_CPU_CNTRS_BRANCH_TAKEN,
    MV_CPU_CNTRS_BRANCH_PREDICT_MISS,
    MV_CPU_CNTRS_BRANCH_PREDICT_COUNT,
    MV_CPU_CNTRS_WB_FULL_CYCLES,
    MV_CPU_CNTRS_WB_WRITE_LATENCY,
    MV_CPU_CNTRS_WB_WRITE_BEAT,
    MV_CPU_CNTRS_ICACHE_READ_LATENCY,
    MV_CPU_CNTRS_ICACHE_READ_BEAT,
    MV_CPU_CNTRS_DCACHE_READ_LATENCY,
    MV_CPU_CNTRS_DCACHE_READ_BEAT,
    MV_CPU_CNTRS_DCACHE_WRITE_LATENCY,
    MV_CPU_CNTRS_DCACHE_WRITE_BEAT,
    MV_CPU_CNTRS_LDM_STM_HOLD,
    MV_CPU_CNTRS_IS_HOLD,
    MV_CPU_CNTRS_DATA_WRITE_ACCESS,
    MV_CPU_CNTRS_DATA_READ_ACCESS,
    MV_CPU_CNTRS_BIU_SIMULT_ACCESS,
    MV_CPU_CNTRS_BIU_ANY_ACCESS,

} MV_CPU_CNTRS_OPS;

typedef struct
{
    char                name[16];
    MV_CPU_CNTRS_OPS    operation;
    int                 opIdx;
    MV_U32              overhead;
    
} MV_CPU_CNTRS_ENTRY;


typedef struct
{
    char   name[16];
    MV_U32 num_of_measurements;
    MV_U32 avg_sample_count;
    MV_U64 counters_before[MV_CPU_CNTRS_NUM];
    MV_U64 counters_after[MV_CPU_CNTRS_NUM];
    MV_U64 counters_sum[MV_CPU_CNTRS_NUM];

} MV_CPU_CNTRS_EVENT;

extern MV_CPU_CNTRS_ENTRY  mvCpuCntrsTbl[MV_CPU_CNTRS_NUM];


MV_STATUS           mvCpuCntrsProgram(int counter, MV_CPU_CNTRS_OPS op, 
                                      char* name, MV_U32 overhead);
void                mvCpuCntrsInit(void);
MV_CPU_CNTRS_EVENT* mvCpuCntrsEventCreate(char* name, MV_U32 print_threshold);
void                mvCpuCntrsEventDelete(MV_CPU_CNTRS_EVENT* event);
void                mvCpuCntrsReset(void);
void                mvCpuCntrsShow(MV_CPU_CNTRS_EVENT* pEvent);
void 		    mvCpuCntrsEventClear(MV_CPU_CNTRS_EVENT* pEvent);

/* internal */
void 		    program_counter(int counter, int op);

static INLINE MV_U64 mvCpuCntrsRead(const int counter)
{
    MV_U32 low = 0, high = 0;
    MV_U32 ll = 0;
    
    switch(counter)
    {
        case 0:
            MV_ASM  ("mcr p15, 0, %0, c15, c12, 0" : : "r" (ll));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 0" : "=r" (low));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 1" : "=r" (high));
         break;

        case 1:
            MV_ASM  ("mcr p15, 0, %0, c15, c12, 1" : : "r" (ll));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 2" : "=r" (low));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 3" : "=r" (high));
         break;

        case 2:
            MV_ASM  ("mcr p15, 0, %0, c15, c12, 2" : : "r" (ll));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 4" : "=r" (low));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 5" : "=r" (high));
         break;

        case 3:
            MV_ASM  ("mcr p15, 0, %0, c15, c12, 3" : : "r" (ll));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 6" : "=r" (low));
            MV_ASM  ("mrc p15, 0, %0, c15, c13, 7" : "=r" (high));
         break;

        default:
            mvOsPrintf("mv_cpu_cntrs_read: bad counter number (%d)\n", counter);
    }
    program_counter(counter, mvCpuCntrsTbl[counter].opIdx);
    return (((MV_U64)high << 32 ) | low);

}


static INLINE void mvCpuCntrsReadBefore(MV_CPU_CNTRS_EVENT* pEvent)
{
#if 0
    int i;

    /* order is important - we want to measure the cycle count last here! */
    for(i=0; i<MV_CPU_CNTRS_NUM; i++)
        pEvent->counters_before[i] = mvCpuCntrsRead(i);
#else
    pEvent->counters_before[1] = mvCpuCntrsRead(1);
    pEvent->counters_before[3] = mvCpuCntrsRead(3);
    pEvent->counters_before[0] = mvCpuCntrsRead(0);
    pEvent->counters_before[2] = mvCpuCntrsRead(2);
#endif
}

static INLINE void mvCpuCntrsReadAfter(MV_CPU_CNTRS_EVENT* pEvent)
{
    int i;

#if 0
    /* order is important - we want to measure the cycle count first here! */
    for(i=0; i<MV_CPU_CNTRS_NUM; i++)
        pEvent->counters_after[i] = mvCpuCntrsRead(i);
#else
    pEvent->counters_after[2] = mvCpuCntrsRead(2);
    pEvent->counters_after[0] = mvCpuCntrsRead(0);
    pEvent->counters_after[3] = mvCpuCntrsRead(3);
    pEvent->counters_after[1] = mvCpuCntrsRead(1);
#endif 

    for(i=0; i<MV_CPU_CNTRS_NUM; i++)
    {
        pEvent->counters_sum[i] += (pEvent->counters_after[i] - pEvent->counters_before[i]);	
    }
    pEvent->num_of_measurements++;
}


#ifdef CONFIG_MV_CPU_PERF_CNTRS

#define MV_CPU_CNTRS_READ(counter)  mvCpuCntrsRead(counter)

#define MV_CPU_CNTRS_START(event)	mvCpuCntrsReadBefore(event)

#define MV_CPU_CNTRS_STOP(event)	mvCpuCntrsReadAfter(event)
			
#define MV_CPU_CNTRS_SHOW(event)	mvCpuCntrsShow(event)

#else

#define MV_CPU_CNTRS_READ(counter)
#define MV_CPU_CNTRS_START(event)
#define MV_CPU_CNTRS_STOP(event)
#define MV_CPU_CNTRS_SHOW(event)

#endif /* CONFIG_MV_CPU_PERF_CNTRS */


#endif /* __mvCpuCntrs_h__ */

