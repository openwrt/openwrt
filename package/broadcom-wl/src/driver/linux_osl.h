/*
 * Linux OS Independent Layer
 *
 * Copyright 2007, Broadcom Corporation
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
#include <linuxver.h>
#include <osl.h>

#define OSL_PKTTAG_SZ      32 /* Size of PktTag */

/* microsecond delay */
extern void osl_delay(uint usec);

/* OSL initialization */
extern osl_t *osl_attach(void *pdev, uint bustype, bool pkttag);
extern void osl_detach(osl_t *osh);

#define PKTFREESETCB(osh, _tx_fn, _tx_ctx) \
	do { \
	   ((osl_pubinfo_t*)osh)->tx_fn = _tx_fn; \
	   ((osl_pubinfo_t*)osh)->tx_ctx = _tx_ctx; \
	} while (0)

/* host/bus architecture-specific byte swap */
#define BUS_SWAP32(v)		(v)


#define	MALLOC_FAILED(osh)	osl_malloc_failed((osh))

extern void *osl_malloc(osl_t *osh, uint size);
extern void osl_mfree(osl_t *osh, void *addr, uint size);
extern uint osl_malloced(osl_t *osh);
extern uint osl_malloc_failed(osl_t *osh);

/* allocate/free shared (dma-able) consistent memory */
#define	DMA_CONSISTENT_ALIGN	PAGE_SIZE
#define	DMA_ALLOC_CONSISTENT(osh, size, pap, dmah) \
	osl_dma_alloc_consistent((osh), (size), (pap))
#define	DMA_FREE_CONSISTENT(osh, va, size, pa, dmah) \
	osl_dma_free_consistent((osh), (void*)(va), (size), (pa))
extern void *osl_dma_alloc_consistent(osl_t *osh, uint size, ulong *pap);
extern void osl_dma_free_consistent(osl_t *osh, void *va, uint size, ulong pa);

/* map/unmap direction */
#define	DMA_TX	1	/* TX direction for DMA */
#define	DMA_RX	2	/* RX direction for DMA */

/* map/unmap shared (dma-able) memory */
#define	DMA_MAP(osh, va, size, direction, p, dmah) \
	osl_dma_map((osh), (va), (size), (direction))
#define	DMA_UNMAP(osh, pa, size, direction, p, dmah) \
	osl_dma_unmap((osh), (pa), (size), (direction))
extern uint osl_dma_map(osl_t *osh, void *va, uint size, int direction);
extern void osl_dma_unmap(osl_t *osh, uint pa, uint size, int direction);

/* API for DMA addressing capability */
#define OSL_DMADDRWIDTH(osh, addrwidth) do {} while (0)

/* register access macros */
#if defined(BCMJTAG)
#include <bcmjtag.h>
#define OSL_WRITE_REG(osh, r, v) (bcmjtag_write(NULL, (uintptr)(r), (v), sizeof(*(r))))
#define OSL_READ_REG(osh, r) (bcmjtag_read(NULL, (uintptr)(r), sizeof(*(r))))
#endif

#if defined(BCMJTAG)
#define SELECT_BUS_WRITE(osh, mmap_op, bus_op) if (((osl_pubinfo_t*)(osh))->mmbus) \
	mmap_op else bus_op
#define SELECT_BUS_READ(osh, mmap_op, bus_op) (((osl_pubinfo_t*)(osh))->mmbus) ? \
	mmap_op : bus_op
#else
#define SELECT_BUS_WRITE(osh, mmap_op, bus_op) mmap_op
#define SELECT_BUS_READ(osh, mmap_op, bus_op) mmap_op
#endif 

/*
 * BINOSL selects the slightly slower function-call-based binary compatible osl.
 * Macros expand to calls to functions defined in linux_osl.c .
 */
#ifndef BINOSL

/* string library, kernel mode */
#ifndef printf
#define	printf(fmt, args...)	printk(fmt, ## args)
#endif /* printf */
#include <linux/kernel.h>
#include <linux/string.h>

/* register access macros */
#define	AND_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) & (v))
#define	OR_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) | (v))

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
#endif /* mips */

/* get processor cycle count */
#if defined(mips)
#define	OSL_GETCYCLES(x)	((x) = read_c0_count() * 2)
#elif defined(__i386__)
#define	OSL_GETCYCLES(x)	rdtscl((x))
#else
#define OSL_GETCYCLES(x)	((x) = 0)
#endif /* defined(mips) */

/* dereference an address that may cause a bus exception */
#ifdef mips
#if defined(MODULE) && (LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 17))
#define BUSPROBE(val, addr)	panic("get_dbe() will not fixup a bus exception when compiled into"\
					" a module")
#else
#define	BUSPROBE(val, addr)	get_dbe((val), (addr))
#include <asm/paccess.h>
#endif /* defined(MODULE) && (LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 17)) */
#else
#define	BUSPROBE(val, addr)	({ (val) = R_REG(NULL, (addr)); 0; })
#endif /* mips */

/* map/unmap physical to virtual I/O */
#define	REG_MAP(pa, size)	ioremap_nocache((unsigned long)(pa), (unsigned long)(size))
#define	REG_UNMAP(va)		iounmap((void *)(va))

/* shared (dma-able) memory access macros */
#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))
#define	BZERO_SM(r, len)	memset((r), '\0', (len))

/* packet primitives */
#define	PKTGET(osh, len, send)		osl_pktget((osh), (len))
#define	PKTFREE(osh, skb, send)		osl_pktfree((osh), (skb), (send))
#define	PKTDATA(osh, skb)		(((struct sk_buff*)(skb))->data)
#define	PKTLEN(osh, skb)		(((struct sk_buff*)(skb))->len)
#define PKTHEADROOM(osh, skb)		(PKTDATA(osh, skb)-(((struct sk_buff*)(skb))->head))
#define PKTTAILROOM(osh, skb) ((((struct sk_buff*)(skb))->end)-(((struct sk_buff*)(skb))->tail))
#define	PKTNEXT(osh, skb)		(((struct sk_buff*)(skb))->next)
#define	PKTSETNEXT(osh, skb, x)		(((struct sk_buff*)(skb))->next = (struct sk_buff*)(x))
#define	PKTSETLEN(osh, skb, len)	__skb_trim((struct sk_buff*)(skb), (len))
#define	PKTPUSH(osh, skb, bytes)	skb_push((struct sk_buff*)(skb), (bytes))
#define	PKTPULL(osh, skb, bytes)	skb_pull((struct sk_buff*)(skb), (bytes))
#define	PKTDUP(osh, skb)		osl_pktdup((osh), (skb))
#define	PKTTAG(skb)			((void*)(((struct sk_buff*)(skb))->cb))
#define PKTALLOCED(osh)			((osl_pubinfo_t *)(osh))->pktalloced
#ifdef BCMDBG_PKT     /* pkt logging for debugging */
#define PKTLIST_DUMP(osh, buf) 		osl_pktlist_dump(osh, buf)
#else /* BCMDBG_PKT */
#define PKTLIST_DUMP(osh, buf)
#endif /* BCMDBG_PKT */

#ifdef BCMDBG_PKT     /* pkt logging for debugging */
extern void osl_pktlist_add(osl_t *osh, void *p);
extern void osl_pktlist_remove(osl_t *osh, void *p);
extern char *osl_pktlist_dump(osl_t *osh, char *buf);
#endif /* BCMDBG_PKT */

/* Convert a native(OS) packet to driver packet.
 * In the process, native packet is destroyed, there is no copying
 * Also, a packettag is zeroed out
 */
static INLINE void *
osl_pkt_frmnative(osl_pubinfo_t *osh, struct sk_buff *skb)
{
	struct sk_buff *nskb;

	if (osh->pkttag)
		bzero((void*)skb->cb, OSL_PKTTAG_SZ);

	/* Increment the packet counter */
	for (nskb = skb; nskb; nskb = nskb->next) {
#ifdef BCMDBG_PKT
		osl_pktlist_add((osl_t *)osh, (void *) nskb);
#endif  /* BCMDBG_PKT */
		osh->pktalloced++;
	}

	return (void *)skb;
}
#define PKTFRMNATIVE(osh, skb)	osl_pkt_frmnative(((osl_pubinfo_t *)osh), (struct sk_buff*)(skb))

/* Convert a driver packet to native(OS) packet
 * In the process, packettag is zeroed out before sending up
 * IP code depends on skb->cb to be setup correctly with various options
 * In our case, that means it should be 0
 */
static INLINE struct sk_buff *
osl_pkt_tonative(osl_pubinfo_t *osh, void *pkt)
{
	struct sk_buff *nskb;

	if (osh->pkttag)
		bzero(((struct sk_buff*)pkt)->cb, OSL_PKTTAG_SZ);

	/* Decrement the packet counter */
	for (nskb = (struct sk_buff *)pkt; nskb; nskb = nskb->next) {
#ifdef BCMDBG_PKT
		osl_pktlist_remove((osl_t *)osh, (void *) nskb);
#endif  /* BCMDBG_PKT */
		osh->pktalloced--;
	}

	return (struct sk_buff *)pkt;
}
#define PKTTONATIVE(osh, pkt)		osl_pkt_tonative((osl_pubinfo_t *)(osh), (pkt))

#define	PKTLINK(skb)			(((struct sk_buff*)(skb))->prev)
#define	PKTSETLINK(skb, x)		(((struct sk_buff*)(skb))->prev = (struct sk_buff*)(x))
#define	PKTPRIO(skb)			(((struct sk_buff*)(skb))->priority)
#define	PKTSETPRIO(skb, x)		(((struct sk_buff*)(skb))->priority = (x))
#define PKTSUMNEEDED(skb)		(((struct sk_buff*)(skb))->ip_summed == CHECKSUM_HW)
#define PKTSETSUMGOOD(skb, x)		(((struct sk_buff*)(skb))->ip_summed = \
						((x) ? CHECKSUM_UNNECESSARY : CHECKSUM_NONE))
/* PKTSETSUMNEEDED and PKTSUMGOOD are not possible because skb->ip_summed is overloaded */
#define PKTSHARED(skb)                  (((struct sk_buff*)(skb))->cloned)

extern void *osl_pktget(osl_t *osh, uint len);
extern void osl_pktfree(osl_t *osh, void *skb, bool send);
extern void *osl_pktdup(osl_t *osh, void *skb);
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
#endif /* LINUX_OSL */
extern int osl_printf(const char *format, ...);
extern int osl_sprintf(char *buf, const char *format, ...);
extern int osl_strcmp(const char *s1, const char *s2);
extern int osl_strncmp(const char *s1, const char *s2, uint n);
extern int osl_strlen(const char *s);
extern char* osl_strcpy(char *d, const char *s);
extern char* osl_strncpy(char *d, const char *s, uint n);

/* register access macros */
#if !defined(BCMJTAG)
#define R_REG(osh, r) (\
	sizeof(*(r)) == sizeof(uint8) ? osl_readb((volatile uint8*)(r)) : \
	sizeof(*(r)) == sizeof(uint16) ? osl_readw((volatile uint16*)(r)) : \
	osl_readl((volatile uint32*)(r)) \
)
#define W_REG(osh, r, v) do { \
	switch (sizeof(*(r))) { \
	case sizeof(uint8):	osl_writeb((uint8)(v), (volatile uint8*)(r)); break; \
	case sizeof(uint16):	osl_writew((uint16)(v), (volatile uint16*)(r)); break; \
	case sizeof(uint32):	osl_writel((uint32)(v), (volatile uint32*)(r)); break; \
	} \
} while (0)
#endif 

#define	AND_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) & (v))
#define	OR_REG(osh, r, v)		W_REG(osh, (r), R_REG(osh, r) | (v))
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

/* shared (dma-able) memory access macros */
#define	R_SM(r)			*(r)
#define	W_SM(r, v)		(*(r) = (v))
#define	BZERO_SM(r, len)	bzero((r), (len))

/* packet primitives */
#define	PKTGET(osh, len, send)		osl_pktget((osh), (len))
#define	PKTFREE(osh, skb, send)		osl_pktfree((osh), (skb), (send))
#define	PKTDATA(osh, skb)		osl_pktdata((osh), (skb))
#define	PKTLEN(osh, skb)		osl_pktlen((osh), (skb))
#define PKTHEADROOM(osh, skb)		osl_pktheadroom((osh), (skb))
#define PKTTAILROOM(osh, skb)		osl_pkttailroom((osh), (skb))
#define	PKTNEXT(osh, skb)		osl_pktnext((osh), (skb))
#define	PKTSETNEXT(osh, skb, x)		osl_pktsetnext((skb), (x))
#define	PKTSETLEN(osh, skb, len)	osl_pktsetlen((osh), (skb), (len))
#define	PKTPUSH(osh, skb, bytes)	osl_pktpush((osh), (skb), (bytes))
#define	PKTPULL(osh, skb, bytes)	osl_pktpull((osh), (skb), (bytes))
#define	PKTDUP(osh, skb)		osl_pktdup((osh), (skb))
#define PKTTAG(skb)			osl_pkttag((skb))
#define PKTFRMNATIVE(osh, skb)		osl_pkt_frmnative((osh), (struct sk_buff*)(skb))
#define PKTTONATIVE(osh, pkt)		osl_pkt_tonative((osh), (pkt))
#define	PKTLINK(skb)			osl_pktlink((skb))
#define	PKTSETLINK(skb, x)		osl_pktsetlink((skb), (x))
#define	PKTPRIO(skb)			osl_pktprio((skb))
#define	PKTSETPRIO(skb, x)		osl_pktsetprio((skb), (x))
#define PKTSHARED(skb)                  osl_pktshared((skb))
#define PKTALLOCED(osh)			osl_pktalloced((osh))
#ifdef BCMDBG_PKT
#define PKTLIST_DUMP(osh, buf) 		osl_pktlist_dump(osh, buf)
#else /* BCMDBG_PKT */
#define PKTLIST_DUMP(osh, buf)
#endif /* BCMDBG_PKT */

extern void *osl_pktget(osl_t *osh, uint len);
extern void osl_pktfree(osl_t *osh, void *skb, bool send);
extern uchar *osl_pktdata(osl_t *osh, void *skb);
extern uint osl_pktlen(osl_t *osh, void *skb);
extern uint osl_pktheadroom(osl_t *osh, void *skb);
extern uint osl_pkttailroom(osl_t *osh, void *skb);
extern void *osl_pktnext(osl_t *osh, void *skb);
extern void osl_pktsetnext(void *skb, void *x);
extern void osl_pktsetlen(osl_t *osh, void *skb, uint len);
extern uchar *osl_pktpush(osl_t *osh, void *skb, int bytes);
extern uchar *osl_pktpull(osl_t *osh, void *skb, int bytes);
extern void *osl_pktdup(osl_t *osh, void *skb);
extern void *osl_pkttag(void *skb);
extern void *osl_pktlink(void *skb);
extern void osl_pktsetlink(void *skb, void *x);
extern uint osl_pktprio(void *skb);
extern void osl_pktsetprio(void *skb, uint x);
extern void *osl_pkt_frmnative(osl_t *osh, struct sk_buff *skb);
extern struct sk_buff *osl_pkt_tonative(osl_t *osh, void *pkt);
extern bool osl_pktshared(void *skb);
extern uint osl_pktalloced(osl_t *osh);

#ifdef BCMDBG_PKT     /* pkt logging for debugging */
extern char *osl_pktlist_dump(osl_t *osh, char *buf);
extern void osl_pktlist_add(osl_t *osh, void *p);
extern void osl_pktlist_remove(osl_t *osh, void *p);
#endif /* BCMDBG_PKT */

#endif	/* BINOSL */

#define OSL_ERROR(bcmerror)	osl_error(bcmerror)
extern int osl_error(int bcmerror);

/* the largest reasonable packet buffer driver uses for ethernet MTU in bytes */
#define	PKTBUFSZ	2048   /* largest reasonable packet buffer, driver uses for ethernet MTU */

#endif	/* _linux_osl_h_ */
