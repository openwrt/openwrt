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
#include "mvCpuL2Cntrs.h"



MV_CPU_L2_CNTRS_ENTRY   mvCpuL2CntrsTbl[MV_CPU_L2_CNTRS_NUM];

MV_CPU_L2_CNTRS_EVENT*  mvCpuL2CntrsEventTbl[128];

void mvCpuL2CntrsReset(void)
{
    MV_U32 reg = 0;
   
    MV_ASM ("mcr p15, 6, %0, c15, c13, 0" : : "r" (reg));
    MV_ASM ("mcr p15, 6, %0, c15, c13, 1" : : "r" (reg));
    MV_ASM ("mcr p15, 6, %0, c15, c13, 2" : : "r" (reg));
    MV_ASM ("mcr p15, 6, %0, c15, c13, 3" : : "r" (reg));
}
               
static void mvCpuL2CntrConfig(int counter, int op)
{
    MV_U32 reg =  (1 << op) | 0x1; /*enable*/

    switch(counter)
    {
        case 0:
         MV_ASM ("mcr p15, 6, %0, c15, c12, 0" : : "r" (reg));
         return;

        case 1:
         MV_ASM ("mcr p15, 6, %0, c15, c12, 1" : : "r" (reg));
         return;

        default:
            mvOsPrintf("mvCpuL2CntrConfig: bad counter number (%d)\n", counter);
    }
    return;
}

void mvCpuL2CntrsEventClear(MV_CPU_L2_CNTRS_EVENT* pEvent)
{
    int i;

    for(i=0; i<MV_CPU_L2_CNTRS_NUM; i++)
    {
        pEvent->counters_sum[i] = 0;
    }
    pEvent->num_of_measurements = 0;
}

                                                                                                              
MV_CPU_L2_CNTRS_EVENT* mvCpuL2CntrsEventCreate(char* name, MV_U32 print_threshold)
{
    int                     i;
    MV_CPU_L2_CNTRS_EVENT*  event = mvOsMalloc(sizeof(MV_CPU_L2_CNTRS_EVENT));

    if(event)
    {
        strncpy(event->name, name, sizeof(event->name));
        event->num_of_measurements = 0;
        event->avg_sample_count = print_threshold;
        for(i=0; i<MV_CPU_L2_CNTRS_NUM; i++)
        {
            event->counters_before[i] = 0;
            event->counters_after[i] = 0;
            event->counters_sum[i] = 0;
        }
    }
    return event;
}

void    mvCpuL2CntrsEventDelete(MV_CPU_L2_CNTRS_EVENT* event)
{
    if(event != NULL)
        mvOsFree(event);
}

                                                                                     
MV_STATUS   mvCpuL2CntrsProgram(int counter, MV_CPU_L2_CNTRS_OPS op, 
                                 char* name, MV_U32 overhead)
{
    strncpy(mvCpuL2CntrsTbl[counter].name, name, sizeof(mvCpuL2CntrsTbl[counter].name));
    mvCpuL2CntrsTbl[counter].operation = op;
    mvCpuL2CntrsTbl[counter].opIdx = op;
    mvCpuL2CntrsTbl[counter].overhead = overhead;
    mvCpuL2CntrConfig(counter, op);
    mvOsPrintf("CPU L2 Counter %d: operation=%d, overhead=%d\n",
                        counter, op, overhead);
    return MV_OK;
}

void    mvCpuL2CntrsShow(MV_CPU_L2_CNTRS_EVENT* pEvent)
{
    int     i;
    MV_U64  counters_avg;

    if(pEvent->num_of_measurements < pEvent->avg_sample_count) 
        return;

    mvOsPrintf("%16s: ", pEvent->name);
    for(i=0; i<MV_CPU_L2_CNTRS_NUM; i++)
    {
        counters_avg = mvOsDivMod64(pEvent->counters_sum[i], 
                                    pEvent->num_of_measurements, NULL);

        if(counters_avg >= mvCpuL2CntrsTbl[i].overhead)
            counters_avg -= mvCpuL2CntrsTbl[i].overhead;
        else
            counters_avg = 0;

        mvOsPrintf("%s=%5llu, ", mvCpuL2CntrsTbl[i].name, counters_avg);
    }
    mvOsPrintf("\n");
    mvCpuL2CntrsEventClear(pEvent);
    mvCpuL2CntrsReset();
}

void    mvCpuL2CntrsStatus(void)
{
    int i;

    for(i=0; i<MV_CPU_L2_CNTRS_NUM; i++)
    {
        mvOsPrintf("#%d: %s, overhead=%d\n", 
            i, mvCpuL2CntrsTbl[i].name, mvCpuL2CntrsTbl[i].overhead);
    }
}
