/******************************************************************************/
/*                                                                            */
/* Broadcom BCM5700 Linux Network Driver, Copyright (c) 2000 - 2004 Broadcom  */
/* Corporation.                                                               */
/* All rights reserved.                                                       */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/******************************************************************************/


#ifndef MM_H
#define MM_H

#include <linux/config.h>

#if defined(CONFIG_SMP) && !defined(__SMP__)
#define __SMP__
#endif

#if defined(CONFIG_MODVERSIONS) && defined(MODULE) && !defined(MODVERSIONS)
#ifndef BCM_SMALL_DRV
#define MODVERSIONS
#endif
#endif

#ifndef B57UM
#define __NO_VERSION__
#endif
#include <linux/version.h>

#ifdef MODULE

#if defined(MODVERSIONS) && (LINUX_VERSION_CODE < 0x020500)
#ifndef BCM_SMALL_DRV
#include <linux/modversions.h>
#endif
#endif

#if (LINUX_VERSION_CODE < 0x020605)
#include <linux/module.h>
#else
#include <linux/moduleparam.h>
#endif

#else

#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#define SET_MODULE_OWNER(dev)
#define MODULE_DEVICE_TABLE(pci, pci_tbl)
#endif


#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/reboot.h>
#include <asm/processor.h>		/* Processor type for cache alignment. */
#include <asm/bitops.h>
#include <asm/io.h>
#include <asm/unaligned.h>
#include <linux/delay.h>
#include <asm/byteorder.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#if (LINUX_VERSION_CODE >= 0x020400)
#if (LINUX_VERSION_CODE < 0x020500)
#include <linux/wrapper.h>
#endif
#include <linux/ethtool.h>
#endif
#ifdef CONFIG_PROC_FS
#include <linux/smp_lock.h>
#include <linux/proc_fs.h>
#define BCM_PROC_FS 1
#endif
#ifdef NETIF_F_HW_VLAN_TX
#include <linux/if_vlan.h>
#define BCM_VLAN 1
#endif
#ifdef NETIF_F_TSO
#define BCM_TSO 1
#define INCLUDE_TCP_SEG_SUPPORT 1
#include <net/ip.h>
#include <net/tcp.h>
#include <net/checksum.h>
#endif

#ifndef LINUX_KERNEL_VERSION
#define LINUX_KERNEL_VERSION	0
#endif

#ifndef MAX_SKB_FRAGS
#define MAX_SKB_FRAGS	0
#endif

#if (LINUX_VERSION_CODE >= 0x020400)
#ifndef ETHTOOL_GEEPROM

#define ETHTOOL_GEEPROM		0x0000000b /* Get EEPROM data */
#define ETHTOOL_SEEPROM		0x0000000c /* Set EEPROM data */

/* for passing EEPROM chunks */
struct ethtool_eeprom {
	u32	cmd;
	u32	magic;
	u32	offset; /* in bytes */
	u32	len; /* in bytes */
	u8	data[0];
};
#define BCM_EEDUMP_LEN(info_p, size) *((u32 *) &((info_p)->reserved1[24]))=size

#else

#define BCM_EEDUMP_LEN(info_p, size) (info_p)->eedump_len=size

#endif
#endif

#define BCM_INT_COAL 1
#define BCM_NIC_SEND_BD 1
#define BCM_ASF 1
#define BCM_WOL 1
#define BCM_TASKLET 1

#if HAVE_NETIF_RECEIVE_SKB
#define BCM_NAPI_RXPOLL 1
#undef BCM_TASKLET
#endif

#if defined(CONFIG_PPC64)
#define BCM_DISCONNECT_AT_CACHELINE 1
#endif

#ifdef BCM_SMALL_DRV
#undef BCM_PROC_FS
#undef ETHTOOL_GEEPROM
#undef ETHTOOL_SEEPROM
#undef ETHTOOL_GREGS
#undef ETHTOOL_GPAUSEPARAM
#undef ETHTOOL_GRXCSUM
#undef ETHTOOL_TEST
#undef BCM_INT_COAL
#undef BCM_NIC_SEND_BD
#undef BCM_WOL
#undef BCM_TASKLET
#undef BCM_TSO
#endif

#ifdef __BIG_ENDIAN
#define BIG_ENDIAN_HOST 1
#endif

#define MM_SWAP_LE32(x) cpu_to_le32(x)
#define MM_SWAP_BE32(x) cpu_to_be32(x)

#if (LINUX_VERSION_CODE < 0x020327)
#define __raw_readl readl
#define __raw_writel writel
#endif

#define MM_MEMWRITEL(ptr, val) __raw_writel(val, ptr)
#define MM_MEMREADL(ptr) __raw_readl(ptr)

typedef atomic_t MM_ATOMIC_T;

#define MM_ATOMIC_SET(ptr, val) atomic_set(ptr, val)
#define MM_ATOMIC_READ(ptr) atomic_read(ptr)
#define MM_ATOMIC_INC(ptr) atomic_inc(ptr)
#define MM_ATOMIC_ADD(ptr, val) atomic_add(val, ptr)
#define MM_ATOMIC_DEC(ptr) atomic_dec(ptr)
#define MM_ATOMIC_SUB(ptr, val) atomic_sub(val, ptr)


#ifndef mmiowb
#define mmiowb()
#endif


#define MM_MB() mb()
#define MM_WMB() wmb()
#define MM_RMB() rmb()
#define MM_MMIOWB() mmiowb()

#include "lm.h"
#include "queue.h"
#include "tigon3.h"

#if DBG
#define STATIC
#else
#define STATIC static
#endif

extern int MM_Packet_Desc_Size;

#define MM_PACKET_DESC_SIZE MM_Packet_Desc_Size

DECLARE_QUEUE_TYPE(UM_RX_PACKET_Q, MAX_RX_PACKET_DESC_COUNT+1);

#define MAX_MEM 16
#define MAX_MEM2 4

#if (LINUX_VERSION_CODE < 0x020211)
typedef u32 dma_addr_t;
#endif

#if (LINUX_VERSION_CODE < 0x02032a)
#define pci_map_single(dev, address, size, dir) virt_to_bus(address)
#define pci_unmap_single(dev, dma_addr, size, dir)
#endif

#if MAX_SKB_FRAGS
#if (LINUX_VERSION_CODE >= 0x02040d)

typedef dma_addr_t dmaaddr_high_t;

#else

#if defined(CONFIG_HIGHMEM) && defined(CONFIG_X86) && !defined(CONFIG_X86_64)

#if defined(CONFIG_HIGHMEM64G)
typedef unsigned long long dmaaddr_high_t;
#else
typedef dma_addr_t dmaaddr_high_t;
#endif

#ifndef pci_map_page
#define pci_map_page bcm_pci_map_page
#endif

static inline dmaaddr_high_t
bcm_pci_map_page(struct pci_dev *dev, struct page *page,
		    int offset, size_t size, int dir)
{
	dmaaddr_high_t phys;

	phys = (page-mem_map) *	(dmaaddr_high_t) PAGE_SIZE + offset;

	return phys;
}

#ifndef pci_unmap_page
#define pci_unmap_page(dev, map, size, dir)
#endif

#else /* #if defined(CONFIG_HIGHMEM) && defined(CONFIG_X86) && ! defined(CONFIG_X86_64)*/

typedef dma_addr_t dmaaddr_high_t;

/* Warning - This may not work for all architectures if HIGHMEM is defined */

#ifndef pci_map_page
#define pci_map_page(dev, page, offset, size, dir) \
	pci_map_single(dev, page_address(page) + (offset), size, dir)
#endif
#ifndef pci_unmap_page
#define pci_unmap_page(dev, map, size, dir) \
	pci_unmap_single(dev, map, size, dir)
#endif

#endif /* #if defined(CONFIG_HIGHMEM) && defined(CONFIG_X86) && ! defined(CONFIG_X86_64)*/

#endif /* #if (LINUX_VERSION_CODE >= 0x02040d)*/
#endif /* #if MAX_SKB_FRAGS*/

#if defined(CONFIG_X86) && !defined(CONFIG_X86_64)
#define NO_PCI_UNMAP 1
#endif

#if (LINUX_VERSION_CODE < 0x020412)
#if !defined(NO_PCI_UNMAP)
#define DECLARE_PCI_UNMAP_ADDR(ADDR_NAME) dma_addr_t ADDR_NAME;
#define DECLARE_PCI_UNMAP_LEN(LEN_NAME) __u32 LEN_NAME;

#define pci_unmap_addr(PTR, ADDR_NAME)	\
	((PTR)->ADDR_NAME)

#define pci_unmap_len(PTR, LEN_NAME)	\
	((PTR)->LEN_NAME)

#define pci_unmap_addr_set(PTR, ADDR_NAME, VAL)	\
	(((PTR)->ADDR_NAME) = (VAL))

#define pci_unmap_len_set(PTR, LEN_NAME, VAL)	\
	(((PTR)->LEN_NAME) = (VAL))
#else
#define DECLARE_PCI_UNMAP_ADDR(ADDR_NAME)
#define DECLARE_PCI_UNMAP_LEN(ADDR_NAME)

#define pci_unmap_addr(PTR, ADDR_NAME)	0
#define pci_unmap_len(PTR, LEN_NAME)	0
#define pci_unmap_addr_set(PTR, ADDR_NAME, VAL) do { } while (0)
#define pci_unmap_len_set(PTR, LEN_NAME, VAL) do { } while (0)
#endif
#endif

#if (LINUX_VERSION_CODE < 0x02030e)
#define net_device device
#define netif_carrier_on(dev)
#define netif_carrier_off(dev)
#endif

#if (LINUX_VERSION_CODE < 0x02032b)
#define tasklet_struct			tq_struct
#endif

typedef struct _UM_DEVICE_BLOCK {
	LM_DEVICE_BLOCK lm_dev;
	struct net_device *dev;
	struct pci_dev *pdev;
	struct net_device *next_module;
	char *name;
#ifdef BCM_PROC_FS
	struct proc_dir_entry *pfs_entry;
	char pfs_name[32];
#endif
	void *mem_list[MAX_MEM];
	dma_addr_t dma_list[MAX_MEM];
	int mem_size_list[MAX_MEM];
	int mem_list_num;

	int index;
	int opened;
	int suspended;
	int using_dac;		/* dual address cycle */
	int delayed_link_ind; /* Delay link status during initial load */
	int adapter_just_inited; /* the first few seconds after init. */
	int timer_interval;
	int statstimer_interval;
	int adaptive_expiry;
	int crc_counter_expiry;
	int poll_tbi_interval;
	int poll_tbi_expiry;
	int asf_heartbeat;
	int tx_full;
	int tx_queued;
	int line_speed;		/* in Mbps, 0 if link is down */
	UM_RX_PACKET_Q rx_out_of_buf_q;
	int rx_out_of_buf;
	int rx_buf_repl_thresh;
	int rx_buf_repl_panic_thresh;
	int rx_buf_repl_isr_limit;
	int rx_buf_align;
	struct timer_list timer;
	struct timer_list statstimer;
	int do_global_lock;
	spinlock_t global_lock;
	spinlock_t undi_lock;
	spinlock_t phy_lock;
	unsigned long undi_flags;
	volatile unsigned long interrupt;
	atomic_t intr_sem;
	int tasklet_pending;
	volatile unsigned long tasklet_busy;
	struct tasklet_struct tasklet;
	struct net_device_stats stats;
	int intr_test;
	int intr_test_result;
#ifdef NETIF_F_HW_VLAN_TX
	struct vlan_group *vlgrp;
#endif
	int vlan_tag_mode;	/* Setting to allow ASF to work properly with */
				/* VLANs                                      */
	#define VLAN_TAG_MODE_AUTO_STRIP              0
	#define VLAN_TAG_MODE_NORMAL_STRIP            1
	#define VLAN_TAG_MODE_FORCED_STRIP            2

	/* Auto mode - VLAN TAGs are always stripped if ASF is enabled,   */
	/*             If ASF is not enabled, it will be in normal mode.  */
	/* Normal mode - VLAN TAGs are stripped when VLANs are registered */
	/* Forced mode - VLAN TAGs are always stripped.                   */

	int adaptive_coalesce;
	uint rx_last_cnt;
	uint tx_last_cnt;
	uint rx_curr_coalesce_frames;
	uint rx_curr_coalesce_frames_intr;
	uint rx_curr_coalesce_ticks;
	uint tx_curr_coalesce_frames;
#if TIGON3_DEBUG
	unsigned long tx_zc_count;
	unsigned long tx_chksum_count;
	unsigned long tx_himem_count;
	unsigned long rx_good_chksum_count;
#endif
	unsigned long rx_bad_chksum_count;
#ifdef BCM_TSO
	unsigned long tso_pkt_count;
#endif
	unsigned long rx_misc_errors;
	uint64_t phy_crc_count;
	unsigned int spurious_int;

	void		*sbh;
	unsigned long	boardflags;
	void		*robo;
	int		qos;
} UM_DEVICE_BLOCK, *PUM_DEVICE_BLOCK;

typedef struct _UM_PACKET {
	LM_PACKET lm_packet;
	struct sk_buff *skbuff;
#if MAX_SKB_FRAGS
	DECLARE_PCI_UNMAP_ADDR(map[MAX_SKB_FRAGS + 1])
	DECLARE_PCI_UNMAP_LEN(map_len[MAX_SKB_FRAGS + 1])
#else
	DECLARE_PCI_UNMAP_ADDR(map[1])
	DECLARE_PCI_UNMAP_LEN(map_len[1])
#endif
} UM_PACKET, *PUM_PACKET;

static inline void MM_SetAddr(LM_PHYSICAL_ADDRESS *paddr, dma_addr_t addr)
{
#if BITS_PER_LONG == 64
	paddr->High = ((unsigned long) addr) >> 32;
	paddr->Low = ((unsigned long) addr) & 0xffffffff;
#else
	paddr->High = 0;
	paddr->Low = (unsigned long) addr;
#endif
}

static inline void MM_SetT3Addr(T3_64BIT_HOST_ADDR *paddr, dma_addr_t addr)
{
#if BITS_PER_LONG == 64
	paddr->High = ((unsigned long) addr) >> 32;
	paddr->Low = ((unsigned long) addr) & 0xffffffff;
#else
	paddr->High = 0;
	paddr->Low = (unsigned long) addr;
#endif
}

#if MAX_SKB_FRAGS
static inline void MM_SetT3AddrHigh(T3_64BIT_HOST_ADDR *paddr,
	dmaaddr_high_t addr)
{
#if defined(CONFIG_HIGHMEM64G) && defined(CONFIG_X86) && !defined(CONFIG_X86_64)
	paddr->High = (unsigned long) (addr >> 32);
	paddr->Low = (unsigned long) (addr & 0xffffffff);
#else
	MM_SetT3Addr(paddr, (dma_addr_t) addr);
#endif
}
#endif

static inline void MM_MapRxDma(PLM_DEVICE_BLOCK pDevice,
	struct _LM_PACKET *pPacket,
	T3_64BIT_HOST_ADDR *paddr)
{
	dma_addr_t map;
	struct sk_buff *skb = ((struct _UM_PACKET *) pPacket)->skbuff;

	map = pci_map_single(((struct _UM_DEVICE_BLOCK *)pDevice)->pdev,
			skb->tail,
			pPacket->u.Rx.RxBufferSize,
			PCI_DMA_FROMDEVICE);
	pci_unmap_addr_set(((struct _UM_PACKET *) pPacket), map[0], map);
	MM_SetT3Addr(paddr, map);
}

static inline void MM_MapTxDma(PLM_DEVICE_BLOCK pDevice,
	struct _LM_PACKET *pPacket,
	T3_64BIT_HOST_ADDR *paddr,
	LM_UINT32 *len,
	int frag)
{
	dma_addr_t map;
	struct sk_buff *skb = ((struct _UM_PACKET *) pPacket)->skbuff;
	unsigned int length;

	if (frag == 0) {
#if MAX_SKB_FRAGS
		if (skb_shinfo(skb)->nr_frags)
			length = skb->len - skb->data_len;
		else
#endif
			length = skb->len;
		map = pci_map_single(((struct _UM_DEVICE_BLOCK *)pDevice)->pdev,
			skb->data, length, PCI_DMA_TODEVICE);
		MM_SetT3Addr(paddr, map);
		pci_unmap_addr_set(((struct _UM_PACKET *)pPacket), map[0], map);
		pci_unmap_len_set(((struct _UM_PACKET *) pPacket), map_len[0],
			length);
		*len = length;
	}
#if MAX_SKB_FRAGS
	else {
		skb_frag_t *sk_frag;
		dmaaddr_high_t hi_map;

		sk_frag = &skb_shinfo(skb)->frags[frag - 1];
			
		hi_map = pci_map_page(
				((struct _UM_DEVICE_BLOCK *)pDevice)->pdev,
				sk_frag->page,
				sk_frag->page_offset,
				sk_frag->size, PCI_DMA_TODEVICE);

		MM_SetT3AddrHigh(paddr, hi_map);
		pci_unmap_addr_set(((struct _UM_PACKET *) pPacket), map[frag],
			hi_map);
		pci_unmap_len_set(((struct _UM_PACKET *) pPacket),
			map_len[frag], sk_frag->size);
		*len = sk_frag->size;
	}
#endif
}

#define BCM5700_PHY_LOCK(pUmDevice, flags) {				\
	spinlock_t *lock;						\
	if ((pUmDevice)->do_global_lock) {				\
		lock = &(pUmDevice)->global_lock;			\
	}								\
	else {								\
		lock = &(pUmDevice)->phy_lock;				\
	}								\
	spin_lock_irqsave(lock, flags);					\
}

#define BCM5700_PHY_UNLOCK(pUmDevice, flags) {				\
	spinlock_t *lock;						\
	if ((pUmDevice)->do_global_lock) {				\
		lock = &(pUmDevice)->global_lock;			\
	}								\
	else {								\
		lock = &(pUmDevice)->phy_lock;				\
	}								\
	spin_unlock_irqrestore(lock, flags);				\
}


#define MM_ACQUIRE_UNDI_LOCK(_pDevice) \
	if (!(((PUM_DEVICE_BLOCK)(_pDevice))->do_global_lock)) {	\
		unsigned long flags;					\
		spin_lock_irqsave(&((PUM_DEVICE_BLOCK)(_pDevice))->undi_lock, flags);	\
		((PUM_DEVICE_BLOCK)(_pDevice))->undi_flags = flags; \
	}

#define MM_RELEASE_UNDI_LOCK(_pDevice) \
	if (!(((PUM_DEVICE_BLOCK)(_pDevice))->do_global_lock)) {	\
		unsigned long flags = ((PUM_DEVICE_BLOCK) (_pDevice))->undi_flags; \
		spin_unlock_irqrestore(&((PUM_DEVICE_BLOCK)(_pDevice))->undi_lock, flags); \
	}

#define MM_ACQUIRE_PHY_LOCK_IN_IRQ(_pDevice) \
	if (!(((PUM_DEVICE_BLOCK)(_pDevice))->do_global_lock)) {	\
		spin_lock(&((PUM_DEVICE_BLOCK)(_pDevice))->phy_lock);	\
	}

#define MM_RELEASE_PHY_LOCK_IN_IRQ(_pDevice) \
	if (!(((PUM_DEVICE_BLOCK)(_pDevice))->do_global_lock)) {	\
		spin_unlock(&((PUM_DEVICE_BLOCK)(_pDevice))->phy_lock); \
	}

#define MM_UINT_PTR(_ptr)   ((unsigned long) (_ptr))

#define MM_GETSTATS64(_Ctr) \
	(uint64_t) (_Ctr).Low + ((uint64_t) (_Ctr).High << 32)

#define MM_GETSTATS32(_Ctr) \
	(uint32_t) (_Ctr).Low

#if BITS_PER_LONG == 64
#define MM_GETSTATS(_Ctr) (unsigned long) MM_GETSTATS64(_Ctr)
#else
#define MM_GETSTATS(_Ctr) (unsigned long) MM_GETSTATS32(_Ctr)
#endif

#if (LINUX_VERSION_CODE >= 0x020600)
#define mm_copy_to_user( to, from, size ) \
	(in_atomic() ? (memcpy((to),(from),(size)), 0) : copy_to_user((to),(from),(size)))
#define mm_copy_from_user( to, from, size ) \
	(in_atomic() ? (memcpy((to),(from),(size)), 0) : copy_from_user((to),(from),(size)))
#else
#define mm_copy_to_user( to, from, size )	\
		copy_to_user((to),(from),(size) )
#define mm_copy_from_user( to, from, size )	\
		copy_from_user((to),(from),(size))
#endif

#ifndef printf
#define printf(fmt, args...) printk(KERN_WARNING fmt, ##args)
#endif

#define DbgPrint(fmt, arg...) printk(KERN_DEBUG fmt, ##arg)
#if defined(CONFIG_X86)
#define DbgBreakPoint() __asm__("int $129")
#else
#define DbgBreakPoint()
#endif
#define MM_Wait(time) udelay(time)

#endif
