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
#ifndef __mvCpuL2Cntrs_h__
#define __mvCpuL2Cntrs_h__

#include "mvTypes.h"
#include "mvOs.h"


#define MV_CPU_L2_CNTRS_NUM         2

typedef enum
{
    MV_CPU_L2_CNTRS_ENABLE = 0,
    MV_CPU_L2_CNTRS_DATA_REQ,
    MV_CPU_L2_CNTRS_DATA_MISS_REQ,
    MV_CPU_L2_CNTRS_INST_REQ,
    MV_CPU_L2_CNTRS_INST_MISS_REQ,
    MV_CPU_L2_CNTRS_DATA_READ_REQ,
    MV_CPU_L2_CNTRS_DATA_READ_MISS_REQ,
    MV_CPU_L2_CNTRS_DATA_WRITE_REQ,
    MV_CPU_L2_CNTRS_DATA_WRITE_MISS_REQ,
    MV_CPU_L2_CNTRS_RESERVED,
    MV_CPU_L2_CNTRS_DIRTY_EVICT_REQ,
    MV_CPU_L2_CNTRS_EVICT_BUFF_STALL,
    MV_CPU_L2_CNTRS_ACTIVE_CYCLES,

} MV_CPU_L2_CNTRS_OPS;

typedef struct
{
    char                name[16];
    MV_CPU_L2_CNTRS_OPS operation;
    int                 opIdx;
    MV_U32              overhead;
    
} MV_CPU_L2_CNTRS_ENTRY;


typedef struct
{
    char   name[16];
    MV_U32 num_of_measurements;
    MV_U32 avg_sample_count;
    MV_U64 counters_before[MV_CPU_L2_CNTRS_NUM];
    MV_U64 counters_after[MV_CPU_L2_CNTRS_NUM];
    MV_U64 counters_sum[MV_CPU_L2_CNTRS_NUM];

} MV_CPU_L2_CNTRS_EVENT;


MV_STATUS               mvCpuL2CntrsProgram(int counter, MV_CPU_L2_CNTRS_OPS op, 
                                        char* name, MV_U32 overhead);
void                    mvCpuL2CntrsInit(void);
MV_CPU_L2_CNTRS_EVENT*  mvCpuL2CntrsEventCreate(char* name, MV_U32 print_threshold);
void                    mvCpuL2CntrsEventDelete(MV_CPU_L2_CNTRS_EVENT* event);
void                    mvCpuL2CntrsReset(void);
void                    mvCpuL2CntrsShow(MV_CPU_L2_CNTRS_EVENT* pEvent);
void 			mvCpuL2CntrsEventClear(MV_CPU_L2_CNTRS_EVENT* pEvent);

static INLINE MV_U64 mvCpuL2CntrsRead(const int counter)
{
    MV_U32 low = 0, high = 0;
    
    switch(counter)
    {
        case 0:
            MV_ASM  ("mrc p15, 6, %0, c15, c13, 0" : "=r" (low));
            MV_ASM  ("mrc p15, 6, %0, c15, c13, 1" : "=r" (high));
         break;

        case 1:
            MV_ASM  ("mrc p15, 6, %0, c15, c13, 2" : "=r" (low));
            MV_ASM  ("mrc p15, 6, %0, c15, c13, 3" : "=r" (high));
         break;

        default:
            mvOsPrintf("mvCpuL2CntrsRead: bad counter number (%d)\n", counter);
    }
    return (((MV_U64)high << 32 ) | low);

}

static INLINE void mvCpuL2CntrsReadBefore(MV_CPU_L2_CNTRS_EVENT* pEvent)
{
    int i;

    for(i=0; i<MV_CPU_L2_CNTRS_NUM; i++)
        pEvent->counters_before[i] = mvCpuL2CntrsRead(i);
}

static INLINE void mvCpuL2CntrsReadAfter(MV_CPU_L2_CNTRS_EVENT* pEvent)
{
    int i;

    for(i=0; i<MV_CPU_L2_CNTRS_NUM; i++) 
    {
        pEvent->counters_after[i] = mvCpuL2CntrsRead(i);
        pEvent->counters_sum[i] += (pEvent->counters_after[i] - pEvent->counters_before[i]);	
    }
    pEvent->num_of_measurements++;
}


#ifdef CONFIG_MV_CPU_L2_PERF_CNTRS

#define MV_CPU_L2_CNTRS_READ(counter)   mvCpuL2CntrsRead(counter)

#define MV_CPU_L2_CNTRS_START(event)	mvCpuL2CntrsReadBefore(event)

#define MV_CPU_L2_CNTRS_STOP(event)	    mvCpuL2CntrsReadAfter(event)
			
#define MV_CPU_L2_CNTRS_SHOW(event)	    mvCpuL2CntrsShow(event)

#else

#define MV_CPU_L2_CNTRS_READ(counter)
#define MV_CPU_L2_CNTRS_START(event)
#define MV_CPU_L2_CNTRS_STOP(event)
#define MV_CPU_L2_CNTRS_SHOW(event)

#endif /* CONFIG_MV_CPU_L2_PERF_CNTRS */


#endif /* __mvCpuL2Cntrs_h__ */

