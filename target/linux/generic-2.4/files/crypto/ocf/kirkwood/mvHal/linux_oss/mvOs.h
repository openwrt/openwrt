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
#ifndef _MV_OS_LNX_H_
#define _MV_OS_LNX_H_
                                                                                                                                               
                                                                                                                                               
#ifdef __KERNEL__
/* for kernel space */
#include <linux/autoconf.h>
#include <linux/interrupt.h>
#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/reboot.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/mm.h>
  
#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/hardirq.h>
#include <asm/dma.h>
#include <asm/io.h>
 
#include <linux/random.h>

#include "dbg-trace.h"

extern void mv_early_printk(char *fmt,...);

#define MV_ASM              __asm__ __volatile__  
#define INLINE              inline
#define MV_TRC_REC	        TRC_REC
#define mvOsPrintf          printk
#define mvOsEarlyPrintf	    mv_early_printk
#define mvOsOutput          printk
#define mvOsSPrintf         sprintf
#define mvOsMalloc(_size_)  kmalloc(_size_,GFP_ATOMIC)
#define mvOsFree            kfree
#define mvOsMemcpy          memcpy
#define mvOsSleep(_mils_)   mdelay(_mils_)
#define mvOsTaskLock()
#define mvOsTaskUnlock()
#define strtol              simple_strtoul
#define mvOsDelay(x)        mdelay(x)
#define mvOsUDelay(x)       udelay(x)
#define mvCopyFromOs        copy_from_user
#define mvCopyToOs          copy_to_user

 
#include "mvTypes.h"
#include "mvCommon.h"
  
#ifdef MV_NDEBUG
#define mvOsAssert(cond)
#else
#define mvOsAssert(cond) { do { if(!(cond)) { BUG(); } }while(0); }
#endif /* MV_NDEBUG */
 
#else /* __KERNEL__ */
 
/* for user space applications */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
 
#define INLINE inline
#define mvOsPrintf printf
#define mvOsOutput printf
#define mvOsMalloc(_size_) malloc(_size_)
#define mvOsFree free
#define mvOsAssert(cond) assert(cond)
 
#endif /* __KERNEL__ */                                                                                                                                               
#define mvOsIoVirtToPhy(pDev, pVirtAddr)                            \
    pci_map_single( (pDev), (pVirtAddr), 0, PCI_DMA_BIDIRECTIONAL )

#define mvOsCacheClear(pDev, p, size )                              \
    pci_map_single( (pDev), (p), (size), PCI_DMA_BIDIRECTIONAL)
 
#define mvOsCacheFlush(pDev, p, size )                              \
    pci_map_single( (pDev), (p), (size), PCI_DMA_TODEVICE)
 
#define mvOsCacheInvalidate(pDev, p, size)                          \
    pci_map_single( (pDev), (p), (size), PCI_DMA_FROMDEVICE )

#define mvOsCacheUnmap(pDev, phys, size)                          \
    pci_unmap_single( (pDev), (dma_addr_t)(phys), (size), PCI_DMA_FROMDEVICE )


#define CPU_PHY_MEM(x)              (MV_U32)x
#define CPU_MEMIO_CACHED_ADDR(x)    (void*)x
#define CPU_MEMIO_UNCACHED_ADDR(x)  (void*)x


/* CPU architecture dependent 32, 16, 8 bit read/write IO addresses */
#define MV_MEMIO32_WRITE(addr, data)    \
    ((*((volatile unsigned int*)(addr))) = ((unsigned int)(data)))

#define MV_MEMIO32_READ(addr)           \
    ((*((volatile unsigned int*)(addr))))

#define MV_MEMIO16_WRITE(addr, data)    \
    ((*((volatile unsigned short*)(addr))) = ((unsigned short)(data)))

#define MV_MEMIO16_READ(addr)           \
    ((*((volatile unsigned short*)(addr))))

#define MV_MEMIO8_WRITE(addr, data)     \
    ((*((volatile unsigned char*)(addr))) = ((unsigned char)(data)))

#define MV_MEMIO8_READ(addr)            \
    ((*((volatile unsigned char*)(addr))))


/* No Fast Swap implementation (in assembler) for ARM */
#define MV_32BIT_LE_FAST(val)            MV_32BIT_LE(val)
#define MV_16BIT_LE_FAST(val)            MV_16BIT_LE(val)
#define MV_32BIT_BE_FAST(val)            MV_32BIT_BE(val)
#define MV_16BIT_BE_FAST(val)            MV_16BIT_BE(val)
    
/* 32 and 16 bit read/write in big/little endian mode */

/* 16bit write in little endian mode */
#define MV_MEMIO_LE16_WRITE(addr, data) \
        MV_MEMIO16_WRITE(addr, MV_16BIT_LE_FAST(data))

/* 16bit read in little endian mode */
static __inline MV_U16 MV_MEMIO_LE16_READ(MV_U32 addr)
{
    MV_U16 data;

    data= (MV_U16)MV_MEMIO16_READ(addr);

    return (MV_U16)MV_16BIT_LE_FAST(data);
}

/* 32bit write in little endian mode */
#define MV_MEMIO_LE32_WRITE(addr, data) \
        MV_MEMIO32_WRITE(addr, MV_32BIT_LE_FAST(data))

/* 32bit read in little endian mode */
static __inline MV_U32 MV_MEMIO_LE32_READ(MV_U32 addr)
{
    MV_U32 data;

    data= (MV_U32)MV_MEMIO32_READ(addr);

    return (MV_U32)MV_32BIT_LE_FAST(data);
}

static __inline void mvOsBCopy(char* srcAddr, char* dstAddr, int byteCount)
{
    while(byteCount != 0)
    {
        *dstAddr = *srcAddr;
        dstAddr++;
        srcAddr++;
        byteCount--;
    }
}

static INLINE MV_U64 mvOsDivMod64(MV_U64 divided, MV_U64 divisor, MV_U64* modulu)
{
    MV_U64  division = 0;

    if(divisor == 1)
	return divided;

    while(divided >= divisor)
    {
	    division++;
	    divided -= divisor;
    }
    if (modulu != NULL)
        *modulu = divided;

    return division;
}

#if defined(MV_BRIDGE_SYNC_REORDER)
extern MV_U32 *mvUncachedParam;

static __inline void mvOsBridgeReorderWA(void)
{
	volatile MV_U32 val = 0;

	val = mvUncachedParam[0];
}
#endif


/* Flash APIs */
#define MV_FL_8_READ            MV_MEMIO8_READ
#define MV_FL_16_READ           MV_MEMIO_LE16_READ
#define MV_FL_32_READ           MV_MEMIO_LE32_READ
#define MV_FL_8_DATA_READ       MV_MEMIO8_READ
#define MV_FL_16_DATA_READ      MV_MEMIO16_READ
#define MV_FL_32_DATA_READ      MV_MEMIO32_READ
#define MV_FL_8_WRITE           MV_MEMIO8_WRITE
#define MV_FL_16_WRITE          MV_MEMIO_LE16_WRITE
#define MV_FL_32_WRITE          MV_MEMIO_LE32_WRITE
#define MV_FL_8_DATA_WRITE      MV_MEMIO8_WRITE
#define MV_FL_16_DATA_WRITE     MV_MEMIO16_WRITE
#define MV_FL_32_DATA_WRITE     MV_MEMIO32_WRITE


/* CPU cache information */
#define CPU_I_CACHE_LINE_SIZE   32    /* 2do: replace 32 with linux core macro */
#define CPU_D_CACHE_LINE_SIZE   32    /* 2do: replace 32 with linux core macro */

#ifdef CONFIG_L2_CACHE_ENABLE
/* Data cache flush one line */
#define mvOsCacheLineFlushInv(handle, addr)                     \
{                                                               \
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c14, 1" : : "r" (addr));\
  __asm__ __volatile__ ("mcr p15, 1, %0, c15, c10, 1" : : "r" (addr));\
  __asm__ __volatile__ ("mcr p15, 0, r0, c7, c10, 4");		\
}

#else

/* Data cache flush one line */
#define mvOsCacheLineFlushInv(handle, addr)                     \
{                                                               \
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c14, 1" : : "r" (addr));\
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 4" : : "r" (addr)); \
}
#endif
 
#ifdef CONFIG_L2_CACHE_ENABLE
#define mvOsCacheLineInv(handle,addr)                           \
{                                                               \
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c6, 1" : : "r" (addr)); \
 __asm__ __volatile__ ("mcr p15, 1, %0, c15, c11, 1" : : "r" (addr)); \
}
#else
#define mvOsCacheLineInv(handle,addr)                           \
{                                                               \
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c6, 1" : : "r" (addr)); \
}
#endif

#ifdef CONFIG_L2_CACHE_ENABLE
/* Data cache flush one line */
#define mvOsCacheLineFlush(handle, addr)                     \
{                                                               \
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 1" : : "r" (addr));\
  __asm__ __volatile__ ("mcr p15, 1, %0, c15, c9, 1" : : "r" (addr));\
  __asm__ __volatile__ ("mcr p15, 0, r0, c7, c10, 4");          \
}

#else
/* Data cache flush one line */
#define mvOsCacheLineFlush(handle, addr)                     \
{                                                               \
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 1" : : "r" (addr));\
  __asm__ __volatile__ ("mcr p15, 0, %0, c7, c10, 4" : : "r" (addr)); \
}
#endif 

static __inline void mvOsPrefetch(const void *ptr)
{
#ifdef CONFIG_USE_DSP
        __asm__ __volatile__(
                "pld\t%0"
                :
                : "o" (*(char *)ptr)
                : "cc");
#else
	return;
#endif
}


/* Flush CPU pipe */
#define CPU_PIPE_FLUSH





/* register manipulations  */

/******************************************************************************
* This debug function enable the write of each register that u-boot access to 
* to an array in the DRAM, the function record only MV_REG_WRITE access.
* The function could not be operate when booting from flash.
* In order to print the array we use the printreg command.
******************************************************************************/
/* #define REG_DEBUG */
#if defined(REG_DEBUG)
extern int reg_arry[2048][2];
extern int reg_arry_index;
#endif

/* Marvell controller register read/write macros */
#define MV_REG_VALUE(offset)          \
                (MV_MEMIO32_READ((INTER_REGS_BASE | (offset))))

#define MV_REG_READ(offset)             \
        (MV_MEMIO_LE32_READ(INTER_REGS_BASE | (offset)))

#if defined(REG_DEBUG)
#define MV_REG_WRITE(offset, val)    \
        MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (offset)), (val)); \
        { \
                reg_arry[reg_arry_index][0] = (INTER_REGS_BASE | (offset));\
                reg_arry[reg_arry_index][1] = (val);\
                reg_arry_index++;\
        }
#else
#define MV_REG_WRITE(offset, val)    \
        MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (offset)), (val));
#endif
                                                
#define MV_REG_BYTE_READ(offset)        \
        (MV_MEMIO8_READ((INTER_REGS_BASE | (offset))))

#if defined(REG_DEBUG)
#define MV_REG_BYTE_WRITE(offset, val)  \
        MV_MEMIO8_WRITE((INTER_REGS_BASE | (offset)), (val)); \
        { \
                reg_arry[reg_arry_index][0] = (INTER_REGS_BASE | (offset));\
                reg_arry[reg_arry_index][1] = (val);\
                reg_arry_index++;\
        }
#else
#define MV_REG_BYTE_WRITE(offset, val)  \
        MV_MEMIO8_WRITE((INTER_REGS_BASE | (offset)), (val))
#endif

#if defined(REG_DEBUG)
#define MV_REG_BIT_SET(offset, bitMask)                 \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) | \
          MV_32BIT_LE_FAST(bitMask)))); \
        { \
                reg_arry[reg_arry_index][0] = (INTER_REGS_BASE | (offset));\
                reg_arry[reg_arry_index][1] = (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)));\
                reg_arry_index++;\
        }
#else
#define MV_REG_BIT_SET(offset, bitMask)                 \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) | \
          MV_32BIT_LE_FAST(bitMask))))
#endif
        
#if defined(REG_DEBUG)
#define MV_REG_BIT_RESET(offset,bitMask)                \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) & \
          MV_32BIT_LE_FAST(~bitMask)))); \
        { \
                reg_arry[reg_arry_index][0] = (INTER_REGS_BASE | (offset));\
                reg_arry[reg_arry_index][1] = (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)));\
                reg_arry_index++;\
        }
#else
#define MV_REG_BIT_RESET(offset,bitMask)                \
        (MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)), \
         (MV_MEMIO32_READ(INTER_REGS_BASE | (offset)) & \
          MV_32BIT_LE_FAST(~bitMask))))
#endif



/* ARM architecture APIs */
MV_U32  mvOsCpuRevGet (MV_VOID);
MV_U32  mvOsCpuPartGet (MV_VOID);
MV_U32  mvOsCpuArchGet (MV_VOID);
MV_U32  mvOsCpuVarGet (MV_VOID);
MV_U32  mvOsCpuAsciiGet (MV_VOID);

/*  Other APIs  */
void* mvOsIoCachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr, MV_U32 *memHandle);
void* mvOsIoUncachedMalloc( void* osHandle, MV_U32 size, MV_ULONG* pPhyAddr, MV_U32 *memHandle );
void mvOsIoUncachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle );
void mvOsIoCachedFree( void* osHandle, MV_U32 size, MV_ULONG phyAddr, void* pVirtAddr, MV_U32 memHandle );
int mvOsRand(void);

#endif /* _MV_OS_LNX_H_ */


