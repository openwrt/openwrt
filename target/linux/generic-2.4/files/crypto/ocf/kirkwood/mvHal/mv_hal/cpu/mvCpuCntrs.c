/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "mvOs.h"
#include "mvCpuCntrs.h"


const static MV_CPU_CNTRS_OPS  mvCpuCntrsOpsTbl[MV_CPU_CNTRS_NUM][MV_CPU_CNTRS_OPS_NUM] =
{   
    /*0*/
    {   
        MV_CPU_CNTRS_CYCLES,            MV_CPU_CNTRS_DCACHE_READ_HIT,       MV_CPU_CNTRS_DCACHE_READ_MISS,
        MV_CPU_CNTRS_DCACHE_WRITE_HIT,  MV_CPU_CNTRS_DCACHE_WRITE_MISS,     MV_CPU_CNTRS_INSTRUCTIONS, 
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_INVALID,
        MV_CPU_CNTRS_MMU_READ_LATENCY,  MV_CPU_CNTRS_ICACHE_READ_LATENCY,   MV_CPU_CNTRS_WB_WRITE_LATENCY,
        MV_CPU_CNTRS_LDM_STM_HOLD,      MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_INVALID,
        MV_CPU_CNTRS_DATA_WRITE_ACCESS, MV_CPU_CNTRS_DATA_READ_ACCESS,      MV_CPU_CNTRS_INVALID,
        MV_CPU_CNTRS_BRANCH_PREDICT_COUNT,
    },
    /*1*/
    {   
        MV_CPU_CNTRS_CYCLES,            MV_CPU_CNTRS_ICACHE_READ_MISS,      MV_CPU_CNTRS_DCACHE_READ_MISS,
        MV_CPU_CNTRS_DCACHE_WRITE_MISS, MV_CPU_CNTRS_ITLB_MISS,             MV_CPU_CNTRS_SINGLE_ISSUE,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_BRANCH_RETIRED,        MV_CPU_CNTRS_INVALID, 
        MV_CPU_CNTRS_MMU_READ_BEAT,     MV_CPU_CNTRS_ICACHE_READ_LATENCY,   MV_CPU_CNTRS_WB_WRITE_BEAT,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_IS_HOLD,               MV_CPU_CNTRS_DATA_READ_ACCESS,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_INVALID,
        MV_CPU_CNTRS_INVALID,
    },
    /*2*/   
    {
        MV_CPU_CNTRS_CYCLES,            MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_DCACHE_ACCESS, 
        MV_CPU_CNTRS_DTLB_MISS,         MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_INVALID,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_BRANCH_PREDICT_MISS,   MV_CPU_CNTRS_WB_WRITE_BEAT,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_DCACHE_READ_LATENCY,   MV_CPU_CNTRS_DCACHE_WRITE_LATENCY,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_BIU_SIMULT_ACCESS,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_INVALID,
        MV_CPU_CNTRS_INVALID,
    },
    /*3*/   
    {
        MV_CPU_CNTRS_CYCLES,            MV_CPU_CNTRS_DCACHE_READ_MISS,      MV_CPU_CNTRS_DCACHE_WRITE_MISS,
        MV_CPU_CNTRS_TLB_MISS,          MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_INVALID, 
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_BRANCH_TAKEN,          MV_CPU_CNTRS_WB_FULL_CYCLES,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_DCACHE_READ_BEAT,      MV_CPU_CNTRS_DCACHE_WRITE_BEAT,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_BIU_ANY_ACCESS,
        MV_CPU_CNTRS_INVALID,           MV_CPU_CNTRS_INVALID,               MV_CPU_CNTRS_DATA_WRITE_ACCESS,
        MV_CPU_CNTRS_INVALID,
    }
};

MV_CPU_CNTRS_ENTRY  mvCpuCntrsTbl[MV_CPU_CNTRS_NUM];

MV_CPU_CNTRS_EVENT*   mvCpuCntrsEventTbl[128];

void mvCpuCntrsReset(void)
{
    MV_U32 reg = 0;
   
    MV_ASM ("mcr p15, 0, %0, c15, c13, 0" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 1" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 2" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 3" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 4" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 5" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 6" : : "r" (reg));
    MV_ASM ("mcr p15, 0, %0, c15, c13, 7" : : "r" (reg));
}
               
void program_counter(int counter, int op)
{
    MV_U32 reg =  (1 << op) | 0x1; /*enable*/

    switch(counter)
    {
        case 0:
         __asm__ __volatile__ ("mcr p15, 0, %0, c15, c12, 0" : : "r" (reg));
         return;

        case 1:
         __asm__ __volatile__ ("mcr p15, 0, %0, c15, c12, 1" : : "r" (reg));
         return;

        case 2:
         __asm__ __volatile__ ("mcr p15, 0, %0, c15, c12, 2" : : "r" (reg));
         return;

        case 3:
         __asm__ __volatile__ ("mcr p15, 0, %0, c15, c12, 3" : : "r" (reg));
         return;

        default:
            mvOsPrintf("error in program_counter: bad counter number (%d)\n", counter);
    }
    return;
}

void mvCpuCntrsEventClear(MV_CPU_CNTRS_EVENT* pEvent)
{
    int i;

    for(i=0; i<MV_CPU_CNTRS_NUM; i++)
    {
        pEvent->counters_sum[i] = 0;
    }
    pEvent->num_of_measurements = 0;
}

                                                                                                              
MV_CPU_CNTRS_EVENT* mvCpuCntrsEventCreate(char* name, MV_U32 print_threshold)
{
    int                     i;
    MV_CPU_CNTRS_EVENT*     event = mvOsMalloc(sizeof(MV_CPU_CNTRS_EVENT));

    if(event)
    {
        strncpy(event->name, name, sizeof(event->name));
        event->num_of_measurements = 0;
        event->avg_sample_count = print_threshold;
        for(i=0; i<MV_CPU_CNTRS_NUM; i++)
        {
            event->counters_before[i] = 0;
            event->counters_after[i] = 0;
            event->counters_sum[i] = 0;
        }
    }
    return event;
}

void    mvCpuCntrsEventDelete(MV_CPU_CNTRS_EVENT* event)
{
    if(event != NULL)
        mvOsFree(event);
}

                                                                                     
MV_STATUS   mvCpuCntrsProgram(int counter, MV_CPU_CNTRS_OPS op, 
                                 char* name, MV_U32 overhead)
{
    int     i;

    /* Find required operations */
    for(i=0; i<MV_CPU_CNTRS_OPS_NUM; i++)
    {
        if( mvCpuCntrsOpsTbl[counter][i] == op)
        {
            strncpy(mvCpuCntrsTbl[counter].name, name, sizeof(mvCpuCntrsTbl[counter].name));
            mvCpuCntrsTbl[counter].operation = op;
            mvCpuCntrsTbl[counter].opIdx = i+1;
            mvCpuCntrsTbl[counter].overhead = overhead;
            program_counter(counter, mvCpuCntrsTbl[counter].opIdx);
            mvOsPrintf("Counter=%d, opIdx=%d, overhead=%d\n",
                        counter, mvCpuCntrsTbl[counter].opIdx, mvCpuCntrsTbl[counter].overhead);
            return MV_OK;
        }
    }
    return MV_NOT_FOUND;
}

void    mvCpuCntrsShow(MV_CPU_CNTRS_EVENT* pEvent)
{
    int     i;
    MV_U64  counters_avg;

    if(pEvent->num_of_measurements < pEvent->avg_sample_count) 
        return;

    mvOsPrintf("%16s: ", pEvent->name);
    for(i=0; i<MV_CPU_CNTRS_NUM; i++)
    {
        counters_avg = mvOsDivMod64(pEvent->counters_sum[i], 
                                  pEvent->num_of_measurements, NULL);
        if(counters_avg >= mvCpuCntrsTbl[i].overhead)
            counters_avg -= mvCpuCntrsTbl[i].overhead;
        else
            counters_avg = 0;

        mvOsPrintf("%s=%5llu, ", mvCpuCntrsTbl[i].name, counters_avg);
    }
    mvOsPrintf("\n");
    mvCpuCntrsEventClear(pEvent);
    mvCpuCntrsReset();
}

void    mvCpuCntrsStatus(void)
{
    int i;

    for(i=0; i<MV_CPU_CNTRS_NUM; i++)
    {
        mvOsPrintf("#%d: %s, overhead=%d\n", 
            i, mvCpuCntrsTbl[i].name, mvCpuCntrsTbl[i].overhead);
    }
}
