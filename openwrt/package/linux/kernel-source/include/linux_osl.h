/*
 * Linux OS Independent Layer
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _linux_osl_h_
#define _linux_osl_h_

#include <typedefs.h>

/* use current 2.4.x calling conventions */
#include <linuxver.h>

/* assert and panic */
#define	ASSERT(exp)		do {} while (0)

/* PCMCIA attribute space access macros */
#if defined(CONFIG_PCMCIA) || defined(CONFIG_PCMCIA_MODULE)
struct pcmcia_dev {
	dev_link_t link;	/* PCMCIA device pointer */
	dev_node_t node;	/* PCMCIA node structure */
	void *base;		/* Mapped attribute memory window */
	size_t size;		/* Size of window */
	void *drv;		/* Driver data */
};
#endif
#define	OSL_PCMCIA_READ_ATTR(osh, offset, buf, size) \
	osl_pcmcia_read_attr((osh), (offset), (buf), (size))
#define	OSL_PCMCIA_WRITE_ATTR(osh, offset, buf, size) \
	osl_pcmcia_write_attr((osh), (offset), (buf), (size))
extern void osl_pcmcia_read_attr(void *osh, uint offset, void *buf, int size);
extern void osl_pcmcia_write_attr(void *osh, uint offset, void *buf, int size);

/* PCI configuration space access macros */
#define	OSL_PCI_READ_CONFIG(loc, offset, size) \
	osl_pci_read_config((loc), (offset), (size))
#define	OSL_PCI_WRITE_CONFIG(loc, offset, size, val) \
	osl_pci_write_config((loc), (offset), (size), (val))
extern uint32 osl_pci_read_config(void *loc, uint size, uint offset);
extern void osl_pci_write_config(void *loc, uint offset, uint size, uint val);

/* OSL initialization */
#define osl_init()		do {} while (0)

/* host/bus architecture-specific byte swap */
#define BUS_SWAP32(v)		(v)

/* general purpose memory allocation */
#if defined(BINOSL)

#if defined(BCMDBG_MEM)

#define	MALLOC(size)		osl_debug_malloc((size), __LINE__, __FILE__)
#define	MFREE(addr, size)	osl_debug_mfree((addr), (size))
#define MALLOCED()		osl_malloced()
extern void* osl_debug_malloc(uint size, int line, char* file);
extern void osl_debug_mfree(void *addr, uint size);
extern void osl_debug_memdump(void);

#else

#define	MALLOC(size)		osl_malloc((size))
#define	MFREE(addr, size)	osl_mfree((addr), (size))
#define MALLOCED()		osl_malloced()

#endif

extern void *osl_malloc(uint size);
extern void osl_mfree(void *addr, uint size);
extern uint osl_malloced(void);

#else

#define	MALLOC(size)		kmalloc((size), GFP_ATOMIC)
#define	MFREE(addr, size)	kfree((addr))
#define MALLOCED()		(0)

#endif

/*
 * BINOSL selects the slightly slower function-call-based binary compatible osl.
 * Macros expand to calls to functions defined in linux_osl.c .
 */
#ifndef BINOSL

/* string library, kernel mode */
#define	printf(fmt, args...)	printk(fmt, ## args)
#include <linux/kernel.h>
#include <linux/string.h>

/* register access macros */
#define R_REG(r) ( \
	sizeof(*(r)) == sizeof(uint8) ? readb((volatile uint8*)(r)) : \
	sizeof(*(r)) == sizeof(uint16) ? readw((volatile uint16*)(r)) : \
	readl((volatile uint32*)(r)) \
)
#define W_REG(r, v) do { \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	writeb((uint8)(v), (volatile uint8*)(r)); break; \
	case sizeof(uint16):	writew((uint16)(v), (volatile uint16*)(r)); break; \
	case sizeof(uint32):	writel((uint32)(v), (volatile uint32*)(r)); break; \
	} \
} while (0)

#define	AND_REG(r, v)		W_REG((r), R_REG(r) & (v))
#define	OR_REG(r, v)		W_REG((r), R_REG(r) | (v))

/* bcopy, bcmp, and bzero */
#define	bcopy(src, dst, len)	memcpy((dst), (src), (len))
#define	bcmp(b1, b2, len)	memcmp((b1), (b2), (len))
#define	bzero(b, len)		memset((b), '\0', (len))

/* uncached virtual address */
#ifdef mips
#define OSL_UNCACHED(va)	KSEG1ADDR((va))
#include <asm/addrspace.h>
#else
#define OSL_UNCACHED(va)	(va)
#endif

/* get processor cycle count */
#if defined(mips)
#define	OSL_GETCYCLES(x)	((x) = read_c0_count() * 2)
#elif defined(__i386__)
#define	OSL_GETCYCLES(x)	rdtscl((x))
#else
#define OSL_GETCYCLES(x)	((x) = 0)
#endif

/* dereference an address that may cause a bus exception */
#ifdef mips
#if defined(MODULE) && (LINUX_VERSION_CODE < KERNEL_VERSION(2,4,17))
#define BUSPROBE(val, addr)	panic("get_dbe() will not fixup a bus exception when compiled into a module")
#else
#define	BUSPROBE(val, addr)	get_dbe((val), (addr))
#include <asm/paccess.h>
#endif
#else
#define	BUSPROBE(val, addr)	({ (val) = R_REG((addr)); 0; })
#endif

/* map/unmap physical to virtual I/O */
#define	REG_MAP(pa, size)	ioremap_nocache((unsigned long)(pa), (unsigned long)(size))
#define	REG_UNMAP(va)		iounmap((void *)(va))

/* allocate/free shared (dma-able) consistent (uncached) memory */
#define	DMA_ALLOC_CONSISTENT(dev, size, pap) \
	pci_alloc_consistent((dev), (size), (dma_addr_t*)(pap))
#define	DMA_FREE_CONSISTENT(dev, va, size, pa) \
	pci_free_consistent((dev), (size), (va), (dma_addr_t)(pa))

/* map/unmap direction */
#define	DMA_TX			PCI_DMA_TODEVICE
#define	DMA_RX			PCI_DMA_FROMDEVICE

/* map/unmap shared (dma-able) memory */
#define	DMA_MAP(dev, va, size, direction, p) \
	pci_map_single((dev), (va), (size), (direction))
#define	DMA_UNMAP(dev, pa, size, direction, p) \
	pci_unmap_single((dev), (dma_addr_t)(pa), (size), (direction))

/* microsecond delay */
#define	OSL_DELAY(usec)		udelay(usec)
#include <linux/delay.h>

/* shared (dma-able) memory access macros */
#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))
#define	BZERO_SM(r, len)	memset((r), '\0', (len))

/* packet primitives */
#define	PKTGET(drv, len, send)		osl_pktget((drv), (len), (send))
#define	PKTFREE(drv, skb, send)		osl_pktfree((skb))
#define	PKTDATA(drv, skb)		(((struct sk_buff*)(skb))->data)
#define	PKTLEN(drv, skb)		(((struct sk_buff*)(skb))->len)
#define PKTHEADROOM(drv, skb)		(PKTDATA(drv,skb)-(((struct sk_buff*)(skb))->head))
#define PKTTAILROOM(drv, skb)		((((struct sk_buff*)(skb))->end)-(((struct sk_buff*)(skb))->tail))
#define	PKTNEXT(drv, skb)		(((struct sk_buff*)(skb))->next)
#define	PKTSETNEXT(skb, x)		(((struct sk_buff*)(skb))->next = (struct sk_buff*)(x))
#define	PKTSETLEN(drv, skb, len)	__skb_trim((struct sk_buff*)(skb), (len))
#define	PKTPUSH(drv, skb, bytes)	skb_push((struct sk_buff*)(skb), (bytes))
#define	PKTPULL(drv, skb, bytes)	skb_pull((struct sk_buff*)(skb), (bytes))
#define	PKTDUP(drv, skb)		skb_clone((struct sk_buff*)(skb), GFP_ATOMIC)
#define	PKTCOOKIE(skb)			((void*)((struct sk_buff*)(skb))->csum)
#define	PKTSETCOOKIE(skb, x)		(((struct sk_buff*)(skb))->csum = (uint)(x))
#define	PKTLINK(skb)			(((struct sk_buff*)(skb))->prev)
#define	PKTSETLINK(skb, x)		(((struct sk_buff*)(skb))->prev = (struct sk_buff*)(x))
#define	PKTPRIO(skb)			(((struct sk_buff*)(skb))->priority)
#define	PKTSETPRIO(skb, x)		(((struct sk_buff*)(skb))->priority = (x))
extern void *osl_pktget(void *drv, uint len, bool send);
extern void osl_pktfree(void *skb);

#else	/* BINOSL */                                    

/* string library */
#ifndef LINUX_OSL
#undef printf
#define	printf(fmt, args...)		osl_printf((fmt), ## args)
#undef sprintf
#define sprintf(buf, fmt, args...)	osl_sprintf((buf), (fmt), ## args)
#undef strcmp
#define	strcmp(s1, s2)			osl_strcmp((s1), (s2))
#undef strncmp
#define	strncmp(s1, s2, n)		osl_strncmp((s1), (s2), (n))
#undef strlen
#define strlen(s)			osl_strlen((s))
#undef strcpy
#define	strcpy(d, s)			osl_strcpy((d), (s))
#undef strncpy
#define	strncpy(d, s, n)		osl_strncpy((d), (s), (n))
#endif
extern int osl_printf(const char *format, ...);
extern int osl_sprintf(char *buf, const char *format, ...);
extern int osl_strcmp(const char *s1, const char *s2);
extern int osl_strncmp(const char *s1, const char *s2, uint n);
extern int osl_strlen(char *s);
extern char* osl_strcpy(char *d, const char *s);
extern char* osl_strncpy(char *d, const char *s, uint n);

/* register access macros */
#define R_REG(r) ( \
	sizeof(*(r)) == sizeof(uint8) ? osl_readb((volatile uint8*)(r)) : \
	sizeof(*(r)) == sizeof(uint16) ? osl_readw((volatile uint16*)(r)) : \
	osl_readl((volatile uint32*)(r)) \
)
#define W_REG(r, v) do { \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	osl_writeb((uint8)(v), (volatile uint8*)(r)); break; \
	case sizeof(uint16):	osl_writew((uint16)(v), (volatile uint16*)(r)); break; \
	case sizeof(uint32):	osl_writel((uint32)(v), (volatile uint32*)(r)); break; \
	} \
} while (0)
#define	AND_REG(r, v)		W_REG((r), R_REG(r) & (v))
#define	OR_REG(r, v)		W_REG((r), R_REG(r) | (v))
extern uint8 osl_readb(volatile uint8 *r);
extern uint16 osl_readw(volatile uint16 *r);
extern uint32 osl_readl(volatile uint32 *r);
extern void osl_writeb(uint8 v, volatile uint8 *r);
extern void osl_writew(uint16 v, volatile uint16 *r);
extern void osl_writel(uint32 v, volatile uint32 *r);

/* bcopy, bcmp, and bzero */
extern void bcopy(const void *src, void *dst, int len);
extern int bcmp(const void *b1, const void *b2, int len);
extern void bzero(void *b, int len);

/* uncached virtual address */
#define OSL_UNCACHED(va)	osl_uncached((va))
extern void *osl_uncached(void *va);

/* get processor cycle count */
#define OSL_GETCYCLES(x)	((x) = osl_getcycles())
extern uint osl_getcycles(void);

/* dereference an address that may target abort */
#define	BUSPROBE(val, addr)	osl_busprobe(&(val), (addr))
extern int osl_busprobe(uint32 *val, uint32 addr);

/* map/unmap physical to virtual */
#define	REG_MAP(pa, size)	osl_reg_map((pa), (size))
#define	REG_UNMAP(va)		osl_reg_unmap((va))
extern void *osl_reg_map(uint32 pa, uint size);
extern void osl_reg_unmap(void *va);

/* allocate/free shared (dma-able) consistent (uncached) memory */
#define	DMA_ALLOC_CONSISTENT(dev, size, pap) \
	osl_dma_alloc_consistent((dev), (size), (pap))
#define	DMA_FREE_CONSISTENT(dev, va, size, pa) \
	osl_dma_free_consistent((dev), (void*)(va), (size), (pa))
extern void *osl_dma_alloc_consistent(void *dev, uint size, ulong *pap);
extern void osl_dma_free_consistent(void *dev, void *va, uint size, ulong pa);

/* map/unmap direction */
#define	DMA_TX	1
#define	DMA_RX	2

/* map/unmap shared (dma-able) memory */
#define	DMA_MAP(dev, va, size, direction, p) \
	osl_dma_map((dev), (va), (size), (direction))
#define	DMA_UNMAP(dev, pa, size, direction, p) \
	osl_dma_unmap((dev), (pa), (size), (direction))
extern uint osl_dma_map(void *dev, void *va, uint size, int direction);
extern void osl_dma_unmap(void *dev, uint pa, uint size, int direction);

/* microsecond delay */
#define	OSL_DELAY(usec)		osl_delay((usec))
extern void osl_delay(uint usec);

/* shared (dma-able) memory access macros */
#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))
#define	BZERO_SM(r, len)	bzero((r), (len))

/* packet primitives */
#define	PKTGET(drv, len, send)		osl_pktget((drv), (len), (send))
#define	PKTFREE(drv, skb, send)		osl_pktfree((skb))
#define	PKTDATA(drv, skb)		osl_pktdata((drv), (skb))
#define	PKTLEN(drv, skb)		osl_pktlen((drv), (skb))
#define PKTHEADROOM(drv, skb)		osl_pktheadroom((drv), (skb))
#define PKTTAILROOM(drv, skb)		osl_pkttailroom((drv), (skb))
#define	PKTNEXT(drv, skb)		osl_pktnext((drv), (skb))
#define	PKTSETNEXT(skb, x)		osl_pktsetnext((skb), (x))
#define	PKTSETLEN(drv, skb, len)	osl_pktsetlen((drv), (skb), (len))
#define	PKTPUSH(drv, skb, bytes)	osl_pktpush((drv), (skb), (bytes))
#define	PKTPULL(drv, skb, bytes)	osl_pktpull((drv), (skb), (bytes))
#define	PKTDUP(drv, skb)		osl_pktdup((drv), (skb))
#define	PKTCOOKIE(skb)			osl_pktcookie((skb))
#define	PKTSETCOOKIE(skb, x)		osl_pktsetcookie((skb), (x))
#define	PKTLINK(skb)			osl_pktlink((skb))
#define	PKTSETLINK(skb, x)		osl_pktsetlink((skb), (x))
#define	PKTPRIO(skb)			osl_pktprio((skb))
#define	PKTSETPRIO(skb, x)		osl_pktsetprio((skb), (x))
extern void *osl_pktget(void *drv, uint len, bool send);
extern void osl_pktfree(void *skb);
extern uchar *osl_pktdata(void *drv, void *skb);
extern uint osl_pktlen(void *drv, void *skb);
extern uint osl_pktheadroom(void *drv, void *skb);
extern uint osl_pkttailroom(void *drv, void *skb);
extern void *osl_pktnext(void *drv, void *skb);
extern void osl_pktsetnext(void *skb, void *x);
extern void osl_pktsetlen(void *drv, void *skb, uint len);
extern uchar *osl_pktpush(void *drv, void *skb, int bytes);
extern uchar *osl_pktpull(void *drv, void *skb, int bytes);
extern void *osl_pktdup(void *drv, void *skb);
extern void *osl_pktcookie(void *skb);
extern void osl_pktsetcookie(void *skb, void *x);
extern void *osl_pktlink(void *skb);
extern void osl_pktsetlink(void *skb, void *x);
extern uint osl_pktprio(void *skb);
extern void osl_pktsetprio(void *skb, uint x);

#endif	/* BINOSL */

#endif	/* _linux_osl_h_ */
