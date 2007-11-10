/*
 * Linux OS Independent Layer
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: linux_osl.h,v 1.1.1.13 2006/04/08 06:13:39 honor Exp $
 */

#ifndef _linux_osl_h_
#define _linux_osl_h_

#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>

#define OSL_PKTTAG_SZ     32 /* Size of PktTag */

/* osl handle type forward declaration */
typedef struct osl_dmainfo osldma_t;

/* OSL initialization */
extern osl_t *osl_attach(void *pdev, bool pkttag);
extern void osl_detach(osl_t *osh);

/* host/bus architecture-specific byte swap */
#define BUS_SWAP32(v)		(v)
#define	MALLOC_FAILED(osh)	osl_malloc_failed((osh))

extern void *osl_malloc(osl_t *osh, uint size);
extern void osl_mfree(osl_t *osh, void *addr, uint size);
extern uint osl_malloced(osl_t *osh);
extern uint osl_malloc_failed(osl_t *osh);

/* API for DMA addressing capability */
#define	DMA_MAP(osh, va, size, direction, p) \
	osl_dma_map((osh), (va), (size), (direction))
#define	DMA_UNMAP(osh, pa, size, direction, p) \
	osl_dma_unmap((osh), (pa), (size), (direction))
static inline uint
osl_dma_map(void *osh, void *va, uint size, int direction)
{
	int dir;
	struct pci_dev *dev;

	dev = (osh == NULL ? NULL : ((osl_t *)osh)->pdev);
	dir = (direction == DMA_TX)? PCI_DMA_TODEVICE: PCI_DMA_FROMDEVICE;
	return (pci_map_single(dev, va, size, dir));
}

static inline void
osl_dma_unmap(void *osh, uint pa, uint size, int direction)
{
	int dir;
	struct pci_dev *dev;

	dev = (osh == NULL ? NULL : ((osl_t *)osh)->pdev);
	dir = (direction == DMA_TX)? PCI_DMA_TODEVICE: PCI_DMA_FROMDEVICE;
	pci_unmap_single(dev, (uint32)pa, size, dir);
}

#define OSL_DMADDRWIDTH(osh, addrwidth) do {} while (0)
#define	DMA_CONSISTENT_ALIGN	PAGE_SIZE
#define	DMA_ALLOC_CONSISTENT(osh, size, pap, dmah) \
	osl_dma_alloc_consistent((osh), (size), (pap))
#define	DMA_FREE_CONSISTENT(osh, va, size, pa, dmah) \
	osl_dma_free_consistent((osh), (void*)(va), (size), (pa))
static inline void*
osl_dma_alloc_consistent(osl_t *osh, uint size, ulong *pap)
{
	return (pci_alloc_consistent(osh->pdev, size, (dma_addr_t*)pap));
}

static inline void
osl_dma_free_consistent(osl_t *osh, void *va, uint size, ulong pa)
{
	pci_free_consistent(osh->pdev, size, va, (dma_addr_t)pa);
}


/* register access macros */
#if defined(BCMJTAG)
#include <bcmjtag.h>
#define	R_REG(osh, r)	bcmjtag_read(NULL, (uint32)(r), sizeof(*(r)))
#define	W_REG(osh, r, v)	bcmjtag_write(NULL, (uint32)(r), (uint32)(v), sizeof(*(r)))
#endif /* defined(BCMSDIO) */

/* packet primitives */
#define	PKTGET(osh, len, send)		osl_pktget((osh), (len), (send))
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
#define PKTALLOCED(osh)			osl_pktalloced((osh))
#define PKTLIST_DUMP(osh, buf)

/* Convert a native(OS) packet to driver packet.
 * In the process, native packet is destroyed, there is no copying
 * Also, a packettag is zeroed out
 */
static INLINE void *
osl_pkt_frmnative(osl_pubinfo_t*osh, struct sk_buff *skb)
{
	struct sk_buff *nskb;

	if (osh->pkttag)
		bzero((void*)skb->cb, OSL_PKTTAG_SZ);

	/* Increment the packet counter */
	for (nskb = skb; nskb; nskb = nskb->next) {
		osh->pktalloced++;
	}

	return (void *)skb;
}
#define PKTFRMNATIVE(osh, skb)	osl_pkt_frmnative(((osl_pubinfo_t*)osh), \
							(struct sk_buff*)(skb))

/* Convert a driver packet to native(OS) packet
 * In the process, packettag is zeroed out before sending up
 * IP code depends on skb->cb to be setup correctly with various options
 * In our case, that means it should be 0
 */
static INLINE struct sk_buff *
osl_pkt_tonative(osl_pubinfo_t*osh, void *pkt)
{
	struct sk_buff *nskb;

	if (osh->pkttag)
		bzero(((struct sk_buff*)pkt)->cb, OSL_PKTTAG_SZ);

	/* Decrement the packet counter */
	for (nskb = (struct sk_buff *)pkt; nskb; nskb = nskb->next) {
		osh->pktalloced--;
	}

	return (struct sk_buff *)pkt;
}
#define PKTTONATIVE(osh, pkt)		osl_pkt_tonative((osl_pubinfo_t*)(osh), (pkt))

#define	PKTLINK(skb)			(((struct sk_buff*)(skb))->prev)
#define	PKTSETLINK(skb, x)		(((struct sk_buff*)(skb))->prev = (struct sk_buff*)(x))
#define	PKTPRIO(skb)			(((struct sk_buff*)(skb))->priority)
#define	PKTSETPRIO(skb, x)		(((struct sk_buff*)(skb))->priority = (x))
#define PKTSHARED(skb)                  (((struct sk_buff*)(skb))->cloned)

extern void *osl_pktget(osl_t *osh, uint len, bool send);
extern void osl_pktfree(osl_t *osh, void *skb, bool send);
extern void *osl_pktdup(osl_t *osh, void *skb);
extern uint osl_pktalloced(osl_t *osh);

#define OSL_ERROR(bcmerror)	osl_error(bcmerror)
extern int osl_error(int bcmerror);

/* the largest reasonable packet buffer driver uses for ethernet MTU in bytes */
#define	PKTBUFSZ	2048   /* largest reasonable packet buffer, driver uses for ethernet MTU */

#endif	/* _linux_osl_h_ */
