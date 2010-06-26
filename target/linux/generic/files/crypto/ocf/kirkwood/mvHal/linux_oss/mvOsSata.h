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
/*******************************************************************************
* mvOsLinux.h - O.S. interface header file for Linux  
*
* DESCRIPTION:
*       This header file contains OS dependent definition under Linux
*
* DEPENDENCIES:
*       Linux kernel header files.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1 $
*******************************************************************************/

#ifndef __INCmvOsLinuxh
#define __INCmvOsLinuxh

/* Includes */
#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/genhd.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/pci.h>

#include <asm/byteorder.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "mvOs.h"


/* Definitions */
#define MV_DEFAULT_QUEUE_DEPTH 2
#define MV_SATA_SUPPORT_EDMA_SINGLE_DATA_REGION
#define MV_SATA_SUPPORT_GEN2E_128_QUEUE_LEN

#ifdef CONFIG_MV88F6082
 #define MV_SATA_OVERRIDE_SW_QUEUE_SIZE
 #define MV_SATA_REQUESTED_SW_QUEUE_SIZE 2
 #undef MV_SATA_SUPPORT_GEN2E_128_QUEUE_LEN
#endif

/* System dependent macro for flushing CPU write cache */
#if defined (MV_BRIDGE_SYNC_REORDER)
#define MV_CPU_WRITE_BUFFER_FLUSH()	do {	\
						wmb();	\
						mvOsBridgeReorderWA();	\
					} while (0)
#else
#define MV_CPU_WRITE_BUFFER_FLUSH()     wmb()
#endif /* CONFIG_MV78XX0 */

/* System dependent little endian from / to CPU conversions */
#define MV_CPU_TO_LE16(x)   cpu_to_le16(x)
#define MV_CPU_TO_LE32(x)   cpu_to_le32(x)

#define MV_LE16_TO_CPU(x)   le16_to_cpu(x)
#define MV_LE32_TO_CPU(x)   le32_to_cpu(x)

#ifdef __BIG_ENDIAN_BITFIELD
#define MV_BIG_ENDIAN_BITFIELD
#endif

/* System dependent register read / write in byte/word/dword variants */
#define MV_REG_WRITE_BYTE(base, offset, val)    writeb(val, base + offset)
#define MV_REG_WRITE_WORD(base, offset, val)    writew(val, base + offset)
#define MV_REG_WRITE_DWORD(base, offset, val)   writel(val, base + offset)
#define MV_REG_READ_BYTE(base, offset)          readb(base + offset)
#define MV_REG_READ_WORD(base, offset)          readw(base + offset)
#define MV_REG_READ_DWORD(base, offset)         readl(base + offset)


/* Typedefs    */

/* System dependant typedefs */
typedef void            *MV_VOID_PTR;
typedef u32             *MV_U32_PTR;
typedef u16             *MV_U16_PTR;
typedef u8              *MV_U8_PTR;
typedef char            *MV_CHAR_PTR;
typedef void            *MV_BUS_ADDR_T;
typedef unsigned long   MV_CPU_FLAGS;


/* Structures  */
/* System dependent structure */
typedef struct mvOsSemaphore
{
  int notUsed;
} MV_OS_SEMAPHORE;


/* Functions (User implemented)*/

/* Semaphore init, take and release */
#define mvOsSemInit(x) MV_TRUE
#define mvOsSemTake(x)
#define mvOsSemRelease(x)

/* Interrupt masking and unmasking functions */
MV_CPU_FLAGS mvOsSaveFlagsAndMaskCPUInterrupts(MV_VOID);
MV_VOID      mvOsRestoreFlags(MV_CPU_FLAGS);

/* Delay function in micro seconds resolution */
void mvMicroSecondsDelay(MV_VOID_PTR, MV_U32);

/* Typedefs    */
typedef enum mvBoolean
{
    MV_SFALSE, MV_STRUE
} MV_BOOLEAN;

/* System logging function */
#include "mvLog.h"
/* Enable READ/WRITE Long SCSI command only when driver is compiled for debugging */
#ifdef MV_LOGGER
#define MV_SATA_SUPPORT_READ_WRITE_LONG
#endif

#define MV_IAL_LOG_ID       3

#endif /* __INCmvOsLinuxh */
