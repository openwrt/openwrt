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
* mvOsCpuArchLib.c - Marvell CPU architecture library
*
* DESCRIPTION:
*       This library introduce Marvell API for OS dependent CPU architecture 
*       APIs. This library introduce single CPU architecture services APKI 
*       cross OS.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#include <asm/processor.h>
#include "mvOs.h"

static MV_U32 read_p15_c0 (void);

/* defines  */
#define ARM_ID_REVISION_OFFS	0
#define ARM_ID_REVISION_MASK	(0xf << ARM_ID_REVISION_OFFS)

#define ARM_ID_PART_NUM_OFFS	4
#define ARM_ID_PART_NUM_MASK	(0xfff << ARM_ID_PART_NUM_OFFS)

#define ARM_ID_ARCH_OFFS	16
#define ARM_ID_ARCH_MASK	(0xf << ARM_ID_ARCH_OFFS)

#define ARM_ID_VAR_OFFS		20
#define ARM_ID_VAR_MASK		(0xf << ARM_ID_VAR_OFFS)

#define ARM_ID_ASCII_OFFS	24
#define ARM_ID_ASCII_MASK	(0xff << ARM_ID_ASCII_OFFS)



void* mvOsIoCachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr,
			  MV_U32 *memHandle)
{
    void *p = kmalloc( size, GFP_KERNEL );
    *pPhyAddr = pci_map_single( osHandle, p, 0, PCI_DMA_BIDIRECTIONAL );
    return p;
}
void* mvOsIoUncachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr,
			    MV_U32 *memHandle)
{
    return pci_alloc_consistent( osHandle, size, (dma_addr_t *)pPhyAddr );
}
 
void mvOsIoUncachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr,
			 MV_U32 memHandle)
{
    return pci_free_consistent( osHandle, size, pVirtAddr, (dma_addr_t)phyAddr );
} 
                                                                                                                                               
void mvOsIoCachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr,
		       MV_U32 memHandle )
{
    return kfree( pVirtAddr );
}
 
int mvOsRand(void)
{
    int rand;
    get_random_bytes(&rand, sizeof(rand) );
    return rand;
}

/*******************************************************************************
* mvOsCpuVerGet() - 
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Revision
*
*******************************************************************************/
MV_U32 mvOsCpuRevGet( MV_VOID )
{
	return ((read_p15_c0() & ARM_ID_REVISION_MASK ) >> ARM_ID_REVISION_OFFS);
}
/*******************************************************************************
* mvOsCpuPartGet() - 
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Part number
*
*******************************************************************************/
MV_U32 mvOsCpuPartGet( MV_VOID )
{
	return ((read_p15_c0() & ARM_ID_PART_NUM_MASK ) >> ARM_ID_PART_NUM_OFFS);
}
/*******************************************************************************
* mvOsCpuArchGet() - 
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Architicture number
*
*******************************************************************************/
MV_U32 mvOsCpuArchGet( MV_VOID )
{
    return ((read_p15_c0() & ARM_ID_ARCH_MASK ) >> ARM_ID_ARCH_OFFS);
}
/*******************************************************************************
* mvOsCpuVarGet() - 
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuVarGet( MV_VOID )
{
    return ((read_p15_c0() & ARM_ID_VAR_MASK ) >> ARM_ID_VAR_OFFS);
}
/*******************************************************************************
* mvOsCpuAsciiGet() - 
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit CPU Variant number
*
*******************************************************************************/
MV_U32 mvOsCpuAsciiGet( MV_VOID )
{
    return ((read_p15_c0() & ARM_ID_ASCII_MASK ) >> ARM_ID_ASCII_OFFS);
}



/*
static unsigned long read_p15_c0 (void)
*/
/* read co-processor 15, register #0 (ID register) */
static MV_U32 read_p15_c0 (void)
{
	MV_U32 value;

	__asm__ __volatile__(
		"mrc	p15, 0, %0, c0, c0, 0   @ read control reg\n"
		: "=r" (value)
		:
		: "memory");

	return value;
}

